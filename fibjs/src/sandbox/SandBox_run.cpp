/*
 * SandBox_run.cpp
 *
 *  Created on: Oct 22, 2012
 *      Author: lion
 */

#include "object.h"
#include "SandBox.h"
#include "Buffer.h"
#include "file_path.h"
#include "path.h"
#include "parse.h"
#include "options.h"
#include "process_signal.h"
#include "ifs/global.h"
#include "ifs/encoding.h"
#include "ifs/process.h"
#include "ifs/fs.h"
#include "ifs/json.h"
#undef stdout
#undef stderr
#include "ifs/child_process.h"
#include <regex>

namespace fibjs {

extern std::vector<char*> s_argv;

static bool is_node_command(const char* cmd, size_t len)
{
    // Check if command is "node" or "node.exe" (case insensitive on Windows)
#ifdef _WIN32
    if (len == 4 && !qstricmp(cmd, "node", 4))
        return true;
    if (len == 8 && !qstricmp(cmd, "node.exe", 8))
        return true;
#else
    if (len == 4 && !qstrcmp(cmd, "node", 4))
        return true;
#endif
    return false;
}

// ---------------------------------------------------------------------------
// shebang handling
//
// A node_modules/.bin entry is executed by fibjs itself only when its shebang
// really names a node compatible runtime: node / node.exe / fibjs / fibjs.exe,
// as a path or behind env (including `env -S`, the portable way to carry
// arguments in a shebang). Substring matching used to accept `my-nodelike`,
// `nodejs`, `python3 # node` and even `# node` comments as node scripts.

static exlib::string lower_ascii(exlib::string s)
{
    for (size_t i = 0; i < s.length(); i++)
        if (s[i] >= 'A' && s[i] <= 'Z')
            s[i] = (char)(s[i] - 'A' + 'a');

    return s;
}

// basename of a path, both separators accepted (a Windows path may reach us
// from a shim written on another platform)
static exlib::string path_basename(exlib::string s)
{
    size_t pos = s.find_last_of("/\\");

    if (pos != exlib::string::npos)
        s = s.substr(pos + 1);

    return s;
}

static bool is_node_interpreter(const exlib::string& prog)
{
    exlib::string base = lower_ascii(path_basename(prog));

    return base == "node" || base == "node.exe" || base == "fibjs" || base == "fibjs.exe";
}

static void split_words(const char* p, size_t len, std::vector<exlib::string>& words)
{
    size_t i = 0;

    while (i < len) {
        while (i < len && (p[i] == ' ' || p[i] == '\t'))
            i++;

        if (i >= len)
            break;

        size_t start = i;

        while (i < len && p[i] != ' ' && p[i] != '\t')
            i++;

        words.push_back(exlib::string(p + start, i - start));
    }
}

// Interpreter program of a shebang line (`#!` included). Returns false when the
// buffer does not start with a shebang.
static bool shebang_program(const char* pdata, size_t len, exlib::string& prog)
{
    if (len < 2 || pdata[0] != '#' || pdata[1] != '!')
        return false;

    // the shebang line only
    size_t end = 2;

    while (end < len && pdata[end] != '\n' && pdata[end] != '\r')
        end++;

    std::vector<exlib::string> words;

    split_words(pdata + 2, end - 2, words);

    if (words.empty())
        return false;

    size_t i = 0;

    if (lower_ascii(path_basename(words[0])) == "env") {
        i = 1;

        for (; i < words.size(); i++) {
            const exlib::string& w = words[i];

            // `env -S <cmd>` / `env --split-string <cmd>`: the next word starts
            // the command line env splits on its own, its first token is the
            // program
            if (w == "-S" || w == "--split-string") {
                i++;
                break;
            }

            if (w.length() > 14 && w.substr(0, 14) == "--split-string" && w[14] == '=') {
                std::vector<exlib::string> inner;
                exlib::string value = w.substr(15);

                split_words(value.c_str(), value.length(), inner);

                if (inner.empty())
                    return false;

                prog = inner[0];
                return true;
            }

            // any other env option (-i, -u NAME, ...) is skipped
            if (w.length() > 1 && w[0] == '-')
                continue;

            break;
        }

        if (i >= words.size())
            return false;
    }

    prog = words[i];
    return true;
}

static bool is_node_shebang(const char* pdata, size_t len)
{
    exlib::string prog;

    return shebang_program(pdata, len, prog) && is_node_interpreter(prog);
}

// ---------------------------------------------------------------------------
// upward lookup (npm semantics)
//
// npm resolves the package root and the executables in node_modules/.bin by
// walking up from the working directory, so a script started in a subdirectory
// still finds the tooling installed at the root of the project. fibjs used to
// look at the working directory only.

static int32_t path_type(const exlib::string& path)
{
    Isolate* isolate = Isolate::current();

    if (!isolate || !isolate->m_topSandbox)
        return -1;

    return isolate->m_topSandbox->file_type(path);
}

// Walk from `start` to the filesystem root. `fn` returns true to stop early.
template <typename Fn>
static bool walk_up(const exlib::string& start, Fn fn)
{
    exlib::string dir = start;

    while (!dir.empty()) {
        if (fn(dir))
            return true;

        exlib::string parent;
        os_dirname(dir, parent);

        if (parent.empty() || parent == dir)
            break;

        dir = parent;
    }

    return false;
}

// Only bare command names are resolved like npm exec: absolute paths and
// names carrying a separator are left to the caller.
static bool is_bare_name(const exlib::string& name)
{
    return !name.empty() && name.find('/') == exlib::string::npos && name.find('\\') == exlib::string::npos;
}

// <name> in node_modules/.bin, nearest ancestor first.
static bool find_bin_upward(const exlib::string& name, exlib::string& retVal)
{
    if (!is_bare_name(name))
        return false;

    exlib::string dir;
    process_base::cwd(dir);

    return walk_up(dir, [&](const exlib::string& current) {
        exlib::string path = current + "/node_modules/.bin/" + name;

        if (path_type(path) != 0)
            return false;

        retVal = path;
        return true;
    });
}

// Nearest ancestor that holds a package.json.
static bool find_package_root(exlib::string& retVal)
{
    exlib::string dir;
    process_base::cwd(dir);

    return walk_up(dir, [&](const exlib::string& current) {
        if (path_type(current + "/package.json") != 0)
            return false;

        retVal = current;
        return true;
    });
}

// Nearest ancestor that anchors a project the way npm does: a folder holding
// either a package.json or a node_modules directory. `npm exec` uses this as
// its local prefix, so a workspace package (node_modules only) still counts.
static bool find_local_prefix(exlib::string& retVal)
{
    exlib::string dir;
    process_base::cwd(dir);

    return walk_up(dir, [&](const exlib::string& current) {
        if (path_type(current + "/package.json") != 0 && path_type(current + "/node_modules") != 1)
            return false;

        retVal = current;
        return true;
    });
}

// Read <dir>/package.json. Returns false when it is missing or is not a JSON
// object.
static bool load_package_json(const exlib::string& dir, v8::Local<v8::Value>& retVal)
{
    Variant var;
    result_t hr = fs_base::cc_readFile(dir + "/package.json", "", var, Isolate::current());

    if (hr.failed())
        return false;

    Buffer* b = (Buffer*)var.object();

    if (!b)
        return false;

    exlib::string buf;
    b->toString(buf);
    hr = json_base::decode(buf, retVal);

    return hr.ok() && !retVal.IsEmpty() && retVal->IsObject();
}

// npm-normalize-package-bin makes every bin target relative to the package
// root: backslashes become '/', and `..` (or a leading '/') can never leave
// the package. `../cli.js` and `/cli.js` both mean `<pkg>/cli.js`.
static exlib::string normalize_bin_target(const exlib::string& target)
{
    exlib::string unix_target;

    for (size_t i = 0; i < target.length(); i++) {
        char c = target[i];
        unix_target += (c == '\\') ? '/' : c;
    }

    std::vector<exlib::string> parts;
    size_t i = 0;

    while (i <= unix_target.length()) {
        size_t end = unix_target.find('/', i);

        if (end == exlib::string::npos)
            end = unix_target.length();

        exlib::string part = unix_target.substr(i, end - i);

        if (part.empty() || part == ".") {
            // nothing to add
        } else if (part == "..") {
            if (!parts.empty())
                parts.pop_back();
        } else
            parts.push_back(part);

        if (end == unix_target.length())
            break;

        i = end + 1;
    }

    exlib::string retVal;

    for (auto& part : parts) {
        if (!retVal.empty())
            retVal += '/';
        retVal += part;
    }

    return retVal;
}

// A normalized `package.json#bin` entry. `name` is the command exposed in
// node_modules/.bin; `target` is package-relative (npm-normalize-package-bin).
struct bin_entry {
    exlib::string name;
    exlib::string target;
};

// Read the package name and normalized bin entries. The command name is the
// basename of the key (or of the package name for `bin: "cli.js"`, or of the
// target for the legacy array form).
static bool read_package_bins(const v8::Local<v8::Value>& v, std::vector<bin_entry>& bins, exlib::string& pkg_name)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> o = v.As<v8::Object>();
    v8::Local<v8::Value> namev = o->Get(context, isolate->NewString("name")).FromMaybe(v8::Local<v8::Value>());

    if (!namev.IsEmpty() && namev->IsString())
        pkg_name = isolate->toString(namev);

    v8::Local<v8::Value> binv = o->Get(context, isolate->NewString("bin")).FromMaybe(v8::Local<v8::Value>());

    if (binv.IsEmpty())
        return false;

    auto add = [&](const exlib::string& name, const exlib::string& target) {
        exlib::string bin_name = path_basename(name);
        exlib::string bin_target = normalize_bin_target(target);

        if (bin_name.empty() || bin_target.empty())
            return;

        // normalizeObject: a later duplicate key replaces the earlier one.
        for (auto& entry : bins) {
            if (entry.name == bin_name) {
                entry.target = bin_target;
                return;
            }
        }

        bins.push_back({ bin_name, bin_target });
    };

    if (binv->IsString()) {
        if (pkg_name.empty())
            return false;
        add(pkg_name, isolate->toString(binv));
    } else if (binv->IsArray()) {
        // Legacy array form: every entry is exposed under its basename.
        v8::Local<v8::Array> arr = binv.As<v8::Array>();

        for (uint32_t i = 0; i < arr->Length(); i++) {
            v8::Local<v8::Value> value;

            if (!arr->Get(context, i).ToLocal(&value) || !value->IsString())
                continue;

            exlib::string target = isolate->toString(value);
            add(target, target);
        }
    } else if (binv->IsObject()) {
        v8::Local<v8::Object> obj = binv.As<v8::Object>();
        v8::Local<v8::Array> keys;

        if (!obj->GetOwnPropertyNames(context).ToLocal(&keys))
            return false;

        for (uint32_t i = 0; i < keys->Length(); i++) {
            v8::Local<v8::Value> key;

            if (!keys->Get(context, i).ToLocal(&key) || !key->IsString())
                continue;

            v8::Local<v8::Value> value;

            if (!obj->Get(context, key).ToLocal(&value) || !value->IsString())
                continue;

            add(isolate->toString(key), isolate->toString(value));
        }
    } else
        return false;

    return !bins.empty();
}

static bool load_package_bins(const exlib::string& pkg_dir, std::vector<bin_entry>& bins, exlib::string& pkg_name)
{
    v8::Local<v8::Value> v;

    return load_package_json(pkg_dir, v) && read_package_bins(v, bins, pkg_name);
}

// A bin entry of the local project's own package.json. npm exec checks this
// before node_modules/.bin, so a package can run its own cli even when it was
// never linked into .bin (`npm exec` installs the local directory into its
// cache instead; fibjs runs the file in place). `bin_dir` is the package's
// .bin directory, which npm puts first on PATH for the command.
static bool find_package_bin(const exlib::string& name, exlib::string& retVal, exlib::string& bin_dir)
{
    if (!is_bare_name(name))
        return false;

    exlib::string pkg_root;

    if (!find_local_prefix(pkg_root))
        return false;

    std::vector<bin_entry> bins;
    exlib::string pkg_name;

    if (!load_package_bins(pkg_root, bins, pkg_name))
        return false;

    exlib::string target;

    for (auto& entry : bins) {
        if (entry.name == name) {
            target = entry.target;
            break;
        }
    }

    if (target.empty())
        return false;

    exlib::string path = pkg_root + "/" + target;
    path_base::normalize(path, path);

    if (path_type(path) != 0)
        return false;

    bin_dir = pkg_root + "/node_modules/.bin";
    retVal = path;
    return true;
}

static bool get_env_string(const char* key, exlib::string& retVal)
{
    const char* value = getenv(key);

    if (!value || !*value)
        return false;

    retVal = value;
    return true;
}

// <global prefix>/bin/<name>, matching npm's globalPrefix derivation: an
// explicit prefix wins, otherwise the runtime's install prefix. `npm exec`
// checks this after node_modules/.bin and before resolving a package spec.
static bool find_global_bin(const exlib::string& name, exlib::string& retVal)
{
    if (!is_bare_name(name))
        return false;

    exlib::string prefix;
    bool from_exec_path = false;

    if (!get_env_string("npm_config_prefix", prefix) && !get_env_string("NPM_CONFIG_PREFIX", prefix)) {
        if (!get_env_string("PREFIX", prefix)) {
            exlib::string execPath;

            if (process_base::get_execPath(execPath).failed())
                return false;

#ifdef _WIN32
            os_dirname(execPath, prefix);
#else
            exlib::string bin_dir;
            os_dirname(execPath, bin_dir);
            os_dirname(bin_dir, prefix);
#endif
            from_exec_path = true;
        }
    }

#ifndef _WIN32
    // npm applies DESTDIR only to the prefix derived from the executable.
    exlib::string destdir;

    if (from_exec_path && get_env_string("DESTDIR", destdir))
        prefix = destdir + prefix;
#endif

#ifdef _WIN32
    exlib::string path = prefix + "/" + name;
#else
    exlib::string path = prefix + "/bin/" + name;
#endif

    path_base::normalize(path, path);

    if (path_type(path) != 0)
        return false;

    retVal = path;
    return true;
}

// npm's get-bin-from-manifest: when every bin aliases the same target the
// first key wins; otherwise the bin named after the unscoped package name is
// selected. Anything else has no executable npm can pick.
static bool select_package_bin(const std::vector<bin_entry>& bins, const exlib::string& pkg_name, exlib::string& retVal)
{
    if (bins.empty())
        return false;

    bool same_target = true;

    for (size_t i = 1; i < bins.size(); i++) {
        if (bins[i].target != bins[0].target) {
            same_target = false;
            break;
        }
    }

    if (same_target) {
        retVal = bins[0].name;
        return true;
    }

    exlib::string unscoped = path_basename(pkg_name);

    for (auto& entry : bins) {
        if (entry.name == unscoped) {
            retVal = entry.name;
            return true;
        }
    }

    return false;
}

// A bare package name, or a scoped `@scope/name`. Path-like arguments are not
// package specs and must not probe node_modules.
static bool is_package_name(const exlib::string& name)
{
    if (name.empty() || name[0] == '.' || name[0] == '/' || name[0] == '\\')
        return false;

    if (name.find('\\') != exlib::string::npos)
        return false;

    size_t slash = name.find('/');

    if (slash == exlib::string::npos)
        return true;

    exlib::string rest = name.substr(slash + 1);

    return name[0] == '@' && slash > 1 && !rest.empty()
        && rest != "." && rest != ".."
        && name.find('/', slash + 1) == exlib::string::npos;
}

// `npx <package>` translates the package name to its bin name through the
// top-level local tree (npm's Arborist can also find nested nodes), then runs
// the resulting .bin entry; npm runs it by name, so the link has to exist. The
// global equivalent needs the registry manifest first in npm, so it is
// intentionally not part of this offline lookup.
static bool find_local_package_bin(const exlib::string& name, exlib::string& retVal)
{
    if (!is_package_name(name))
        return false;

    exlib::string prefix;

    if (!find_local_prefix(prefix))
        return false;

    std::vector<bin_entry> bins;
    exlib::string pkg_name;

    if (!load_package_bins(prefix + "/node_modules/" + name, bins, pkg_name))
        return false;

    exlib::string bin_name;

    if (!select_package_bin(bins, pkg_name, bin_name))
        return false;

    return find_bin_upward(bin_name, retVal);
}

// Is <cmd> a node script installed in node_modules/.bin? Used by the command
// rewriter to decide whether a bare command is prefixed with the fibjs binary.
static bool is_node_bin_script(const exlib::string& cmd)
{
    if (cmd.empty() || cmd[0] == '.')
        return false;

    exlib::string path;

    if (!find_bin_upward(cmd, path))
        return false;

    Variant var;
    result_t hr = fs_base::cc_readFile(path, "", var, Isolate::current());

    if (hr.failed())
        return false;

    Buffer* b = (Buffer*)var.object();

    if (!b)
        return false;

    return is_node_shebang((const char*)b->data(), b->length());
}

// ---------------------------------------------------------------------------
// argument escaping
//
// Extra arguments used to be appended to the shell command line verbatim, so
// `fibjs <script> "a b" "c;d"` was split again by the shell and the `;` started
// a new command. Every argument is now quoted for the shell the command line is
// handed to (npm does the same in @npmcli/promise-spawn/lib/escape.js).

#ifndef _WIN32
static void append_shell_arg(exlib::string& cmd, const char* arg)
{
    size_t len = strlen(arg);
    bool safe = len > 0;

    for (size_t i = 0; safe && i < len; i++) {
        unsigned char c = (unsigned char)arg[i];

        safe = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')
            || c == '_' || c == '-' || c == '.' || c == '/' || c == ':' || c == '@'
            || c == '%' || c == '+' || c == '=' || c == ',';
    }

    if (safe) {
        cmd.append(arg, len);
        return;
    }

    cmd += '\'';

    for (size_t i = 0; i < len; i++) {
        if (arg[i] == '\'')
            cmd += "'\\''";
        else
            cmd += arg[i];
    }

    cmd += '\'';
}
#else
static void append_shell_arg(exlib::string& cmd, const char* arg)
{
    // cmd.exe quoting, following @npmcli/promise-spawn/lib/escape.js: quote when
    // the argument holds blanks or quotes (doubling the backslashes in front of
    // a quote), then prefix every cmd.exe meta character with '^'.
    size_t len = strlen(arg);
    bool quote = false;

    for (size_t i = 0; i < len; i++)
        if (arg[i] == ' ' || arg[i] == '\t' || arg[i] == '\n' || arg[i] == '\v' || arg[i] == '"')
            quote = true;

    exlib::string out;

    if (len == 0)
        out = "\"\"";
    else if (!quote)
        out.assign(arg, len);
    else {
        out += '"';

        for (size_t i = 0; i <= len; i++) {
            size_t slash = 0;

            while (i < len && arg[i] == '\\') {
                i++;
                slash++;
            }

            if (i == len) {
                for (size_t n = 0; n < slash * 2; n++)
                    out += '\\';
                break;
            }

            if (arg[i] == '"') {
                for (size_t n = 0; n < slash * 2 + 1; n++)
                    out += '\\';
                out += '"';
            } else {
                for (size_t n = 0; n < slash; n++)
                    out += '\\';
                out += arg[i];
            }
        }

        out += '"';
    }

    for (size_t i = 0; i < out.length(); i++) {
        char c = out[i];

        if (c == ' ' || c == '!' || c == '%' || c == '^' || c == '&' || c == '('
            || c == ')' || c == '<' || c == '>' || c == '|' || c == '"')
            cmd += '^';

        cmd += c;
    }
}
#endif

// Mask quoted strings to avoid matching special chars inside quotes
static std::string mask_quotes(const std::string& str)
{
    std::string result;
    result.reserve(str.length());

    size_t i = 0;
    while (i < str.length()) {
        if (str[i] == '"' || str[i] == '\'') {
            char quote = str[i];
            result += quote;
            i++;
            while (i < str.length() && str[i] != quote) {
                if (str[i] == '\\' && i + 1 < str.length()) {
                    result += "##"; // placeholder for escaped char
                    i += 2;
                } else {
                    result += '#'; // placeholder
                    i++;
                }
            }
            if (i < str.length()) {
                result += quote;
                i++;
            }
        } else {
            result += str[i];
            i++;
        }
    }
    return result;
}

static exlib::string replace_node_command(exlib::string cmd_str)
{
    exlib::string execPath;
    std::string str(cmd_str.c_str(), cmd_str.length());
    std::string masked = mask_quotes(str);

    // A script that wants the real node (or another runtime) can opt out.
    const char* no_rewrite = getenv("FIBJS_NO_NODE_REWRITE");
    if (no_rewrite && qstrcmp(no_rewrite, "0"))
        return cmd_str;

    // Regex pattern to find command positions:
    // - After: start of string, &&, ||, |, ;, ( or & (Windows only)
    // - Skip env var assignments (VAR=value) on Unix
    // - Capture the command name

    // Collect all replacements
    struct Replacement {
        size_t pos;
        size_t len;
        exlib::string replacement;
        bool prepend; // true = prepend execPath, false = replace with execPath
    };
    std::vector<Replacement> replacements;

#ifdef _WIN32
    // Windows: operators are &&, ||, |, &, ;, (
    // No inline env var assignment - 'set' is a command
    static std::regex pattern(R"_regex_((?:^|&&|\|\||\||&|;|\()\s*([^\s&|;()"'<>]+))_regex_");
#else
    // Unix: operators are &&, ||, |, ;, (
    // Support inline env var assignments
    static std::regex pattern(R"_regex_((?:^|&&|\|\||\||;|\()\s*(?:[A-Za-z_][A-Za-z0-9_]*=\S*\s+)*([^\s&|;()"'<>]+))_regex_");
#endif

    std::sregex_iterator it(masked.begin(), masked.end(), pattern);
    std::sregex_iterator end;

    while (it != end) {
        std::smatch match = *it;
        std::string cmd = match[1].str();

        if (!cmd.empty()) {
            // Find the actual position of the command in the match
            size_t cmd_pos_in_match = match[0].str().rfind(cmd);
            size_t actual_pos = match.position() + cmd_pos_in_match;

            // Get the original command from the unmasked string
            exlib::string original_cmd = cmd_str.substr(actual_pos, cmd.length());

            if (is_node_command(original_cmd.c_str(), original_cmd.length())) {
                // Replace "node" with execPath
                if (execPath.empty())
                    process_base::get_execPath(execPath);
                replacements.push_back({ actual_pos, original_cmd.length(), execPath, false });
            } else if (is_node_bin_script(original_cmd)) {
                // Prepend execPath before .bin script
                if (execPath.empty())
                    process_base::get_execPath(execPath);
                replacements.push_back({ actual_pos, 0, execPath + " ", true });
            }
        }
        ++it;
    }

    // Apply replacements from end to start to preserve positions
    exlib::string result = cmd_str;
    for (auto rit = replacements.rbegin(); rit != replacements.rend(); ++rit) {
        if (rit->prepend) {
            result = result.substr(0, rit->pos) + rit->replacement + result.substr(rit->pos);
        } else {
            result = result.substr(0, rit->pos) + rit->replacement + result.substr(rit->pos + rit->len);
        }
    }

    return result;
}

// npm's set-path: <start>/node_modules/.bin and every ancestor's, nearest
// first. Missing directories stay on PATH, exactly like npm.
static exlib::string build_bin_path(const exlib::string& start)
{
    exlib::string retVal;

    walk_up(start, [&](const exlib::string& dir) {
        if (!retVal.empty())
            retVal += PATH_DELIMITER;
        retVal += dir + "/node_modules/.bin";
        return false;
    });

    return retVal;
}

static exlib::string join_bin_path(const exlib::string& extra_bin, const exlib::string& start, const exlib::string& inherited)
{
    exlib::string retVal;

    if (!extra_bin.empty()) {
        retVal = extra_bin;
        retVal += PATH_DELIMITER;
    }

    retVal += build_bin_path(start);

    if (!inherited.empty()) {
        retVal += PATH_DELIMITER;
        retVal += inherited;
    }

    return retVal;
}

// A node/fibjs script stays in this process, so its child_process calls have
// to see the same PATH a spawned script would get.
static void prepend_process_path(const exlib::string& start, const exlib::string& extra_bin)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Object> env;
    result_t hr = process_base::get_env(env);

    if (hr.failed())
        return;

    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Value> path_val = env->Get(context, isolate->NewString("PATH")).FromMaybe(v8::Local<v8::Value>());
    exlib::string inherited;

    if (!IsEmpty(path_val))
        inherited = isolate->toString(path_val);

    env->Set(context, isolate->NewString("PATH"),
                isolate->NewString(join_bin_path(extra_bin, start, inherited)))
        .IsJust();
    // npm exports INIT_CWD for the command it runs, including a .bin script.
    env->Set(context, isolate->NewString("INIT_CWD"), isolate->NewString(start)).IsJust();
}

static result_t run_shell(exlib::string cmd_str, const std::vector<char*>& args, exlib::string cwd, const exlib::string& extra_bin = "")
{
    exlib::string cmd = replace_node_command(cmd_str);

    for (size_t i = 2; i < args.size(); i++) {
        cmd += ' ';
        append_shell_arg(cmd, args[i]);
    }

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> opts = v8::Object::New(isolate->m_isolate);
    opts->Set(context, isolate->NewString("stdio"), isolate->NewString("inherit")).IsJust();

#ifndef _WIN32
    // Own session and process group: forwarding a signal to the direct child
    // is not enough for `sh -c "sleep 25"` on shells that fork instead of
    // exec'ing the command (dash, busybox ash), the sleep would survive the
    // shell. Detached makes the child a group leader so
    // ChildProcess::signalAliveChildren can signal the whole tree; the stdio
    // handles are inherited as before and reads from the tty keep working
    // (no controlling terminal, hence no SIGTTIN).
    opts->Set(context, isolate->NewString("detached"), v8::True(isolate->m_isolate)).IsJust();
#endif

    // a package script runs from the root of its package (npm runs lifecycle
    // scripts there, and keeps the directory the user typed in INIT_CWD)
    if (!cwd.empty())
        opts->Set(context, isolate->NewString("cwd"), isolate->NewString(cwd)).IsJust();

    // Add every ancestor node_modules/.bin to PATH, like npm's run-script:
    // the tooling of a project is visible to the command and to the commands
    // it spawns even when the working directory is deeply nested. When the
    // command came from the global prefix, that directory comes first.
    v8::Local<v8::Object> env;
    process_base::get_env(env);

    exlib::string workdir;
    process_base::cwd(workdir);

    v8::Local<v8::Value> path_val = env->Get(context, isolate->NewString("PATH")).FromMaybe(v8::Local<v8::Value>());
    exlib::string inherited;
    if (!IsEmpty(path_val))
        inherited = isolate->toString(path_val);

    exlib::string new_path = join_bin_path(extra_bin, cwd.empty() ? workdir : cwd, inherited);
    env->Set(context, isolate->NewString("PATH"), isolate->NewString(new_path)).IsJust();
    env->Set(context, isolate->NewString("INIT_CWD"), isolate->NewString(workdir)).IsJust();
    opts->Set(context, isolate->NewString("env"), env).IsJust();

    // While the script runs, Ctrl-C / SIGTERM are forwarded to it and its exit
    // status decides ours; a signal death becomes 128 + signum.
    process_signal_forward_children(true);

    obj_ptr<child_process_base::ExecType> exec_retVal;
    result_t hr = child_process_base::ac_exec(cmd, opts, exec_retVal);

    process_signal_forward_children(false);

    if (hr.failed())
        return hr;

    int32_t code = exec_retVal->exitCode;

    if (code < 0) {
        // the child was killed by a signal: leave the same way
        int32_t signum = -code;

        if (!signal_reraise(signum))
            code = 128 + signum;
    }

    process_base::exit(code);
    return result_t();
}

// Try to use `path` as the command implementation. A node/fibjs shebang is
// left to run_main's in-process loader; anything else (shell script, binary,
// no shebang) goes to the shell. `handled` is false when the file cannot be
// resolved as a command, so the caller can try the next lookup layer.
static result_t run_bin_candidate(const exlib::string& path, const exlib::string& extra_bin,
    const exlib::string& workdir, obj_ptr<Buffer_base>& bin, exlib::string& fname,
    bool& needUpdateArgv, bool& handled)
{
    handled = false;

    Isolate* isolate = Isolate::current();

    if (!isolate || !isolate->m_topSandbox)
        return result_t();

    exlib::string resolved = path;
    result_t hr = isolate->m_topSandbox->resolveFile(resolved, "", bin, SandBox::kCommonJS, resolved, NULL);

    if (hr.failed())
        return result_t();

    handled = true;

    Buffer* b = Buffer::Cast(bin);

    if (is_node_shebang((const char*)b->data(), b->length())) {
        prepend_process_path(workdir, extra_bin);
        fname = resolved;
        needUpdateArgv = true;
        return result_t();
    }

    return run_shell(resolved, s_argv, workdir, extra_bin);
}

result_t SandBox::run_main(exlib::string fname)
{
    result_t hr;
    obj_ptr<Buffer_base> bin;
    bool needUpdateArgv = false;

    if (fname[0] == '-' && fname[1] == '-') {
        int32_t i;
        exlib::string tmp("opt_tools/");
        tmp += fname.c_str() + 2;

        for (i = 0; opt_tools[i].name && qstrcmp(opt_tools[i].name, tmp.c_str()); i++)
            ;
        opt_tools[i].getDate(bin);
        fname += ".cjs";
    } else {
        bool isAbs;
        exlib::string rname;

        path_base::isAbsolute(fname, isAbs);
        if (!isAbs) {
            needUpdateArgv = true;
            rname = fname;
            os_resolve(fname);
        } else
            path_base::normalize(fname, fname);

        hr = resolveFile(fname, "", bin, kCommonJS, fname, NULL);
        if (hr.failed()) {
            if (isAbs)
                return hr;

            exlib::string workdir;
            process_base::cwd(workdir);

            // `npm exec` order: the local package's own bin entries, then
            // node_modules/.bin from the working directory upward, then the
            // global prefix, then a locally installed package name translated
            // to its bin. fibjs keeps its package.json scripts fallback
            // between the local bins and the global prefix, so a project
            // command is not shadowed by an unrelated global binary.
            hr = result_t();

            exlib::string bin_path;
            exlib::string bin_dir;
            bool handled = false;

            if (find_package_bin(rname, bin_path, bin_dir))
                hr = run_bin_candidate(bin_path, bin_dir, workdir, bin, fname, needUpdateArgv, handled);

            if (hr.ok() && !handled && find_bin_upward(rname, bin_path)) {
                // npm puts the directory the command came from first on PATH,
                // before the working directory's own ancestors.
                os_dirname(bin_path, bin_dir);
                hr = run_bin_candidate(bin_path, bin_dir, workdir, bin, fname, needUpdateArgv, handled);
            }

            if (hr.failed())
                return hr;

            if (!handled) {
                // package.json scripts of the nearest package, run from its root
                exlib::string pkg_root;

                if (find_package_root(pkg_root)) {
                    v8::Local<v8::Value> v;
                    exlib::string buf;
                    Isolate* isolate = holder();
                    v8::Local<v8::Context> context = isolate->context();

                    hr = loadFile(pkg_root + "/package.json", bin);
                    if (hr.failed())
                        return result_t(CALL_E_FILE_NOT_FOUND);

                    bin->toString(buf);
                    hr = json_base::decode(buf, v);
                    if (hr.failed())
                        return hr;

                    if (v.IsEmpty() || !v->IsObject())
                        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_INVALID_PACKAGE_CONFIG").with_message("SandBox: Invalid package.json");

                    v8::Local<v8::Object> o = v.As<v8::Object>();
                    v8::Local<v8::Value> scripts = o->Get(context, isolate->NewString("scripts", 7)).FromMaybe(v8::Local<v8::Value>());

                    if (!IsEmpty(scripts) && scripts->IsObject()) {
                        o = scripts.As<v8::Object>();

                        v8::Local<v8::Value> cmd = o->Get(context, isolate->NewString(rname)).FromMaybe(v8::Local<v8::Value>());
                        if (!IsEmpty(cmd) && cmd->IsString())
                            return run_shell(isolate->toString(cmd), s_argv, pkg_root);
                    }
                }

                // Last resort: a bin under npm's global prefix.
                exlib::string global_path;

                if (find_global_bin(rname, global_path)) {
                    exlib::string global_dir;

                    os_dirname(global_path, global_dir);
                    hr = run_bin_candidate(global_path, global_dir, workdir, bin, fname, needUpdateArgv, handled);
                    if (hr.failed())
                        return hr;
                }

                // `npm exec <package>`: translate a locally installed package
                // name to the bin npm would pick from its manifest.
                if (!handled && find_local_package_bin(rname, bin_path)) {
                    os_dirname(bin_path, bin_dir);
                    hr = run_bin_candidate(bin_path, bin_dir, workdir, bin, fname, needUpdateArgv, handled);
                    if (hr.failed())
                        return hr;
                }

                if (!handled)
                    return result_t(CALL_E_FILE_NOT_FOUND);
            }
        }
    }

    // Update s_argv[1] with the resolved absolute file path when original was relative
    if (needUpdateArgv) {
        static exlib::string s_main_fname;
        s_main_fname = fname;
        s_argv[1] = s_main_fname.data();
        holder()->m_argv.Reset();
    }

    v8::Local<v8::Array> argv;
    process_base::get_argv(argv);

    obj_ptr<ExtLoader> l;
    hr = get_loader(fname, l);
    if (hr.failed())
        return hr;

    Context context(this, fname);

    std::vector<ExtLoader::arg> extarg(1);
    extarg[0] = ExtLoader::arg("__argv", argv);

    return l->run_script(&context, bin, fname, extarg, true, true);
}

result_t SandBox::run_worker(exlib::string fname, Worker_base* master)
{
    result_t hr;
    bool isAbs;

    hr = absolute_file_path_like(fname, fname);
    if (hr.failed())
        return hr;

    path_base::isAbsolute(fname, isAbs);
    if (!isAbs)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("SandBox: Invalid file name.");
    path_base::normalize(fname, fname);

    obj_ptr<Buffer_base> bin;
    hr = resolveFile(fname, "", bin, kCommonJS, fname, NULL);
    if (hr.failed())
        return hr;

    obj_ptr<ExtLoader> l;
    hr = get_loader(fname, l);
    if (hr.failed())
        return hr;

    Context context(this, fname);

    std::vector<ExtLoader::arg> extarg(1);
    extarg[0] = ExtLoader::arg("Master", master->wrap());

    return l->run_script(&context, bin, fname, extarg, false, true);
}

result_t SandBox::run_worker_source(exlib::string fname, exlib::string source, Worker_base* master)
{
    result_t hr;
    bool isAbs;

    hr = absolute_file_path_like(fname, fname);
    if (hr.failed())
        return hr;

    path_base::isAbsolute(fname, isAbs);
    if (!isAbs)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("SandBox: Invalid file name.");
    path_base::normalize(fname, fname);

    obj_ptr<Buffer_base> bin = new Buffer(source.c_str(), source.length());

    obj_ptr<ExtLoader> l;
    hr = get_loader(fname, l);
    if (hr.failed())
        return hr;

    Context context(this, fname);

    std::vector<ExtLoader::arg> extarg(1);
    extarg[0] = ExtLoader::arg("Master", master->wrap());

    return l->run_script(&context, bin, fname, extarg, false, true);
}

result_t SandBox::run(exlib::string fname, bool in_cjs)
{
    Scope _scope(this);

    result_t hr;
    bool isAbs;

    hr = absolute_file_path_like(fname, fname);
    if (hr.failed())
        return hr;

    path_base::isAbsolute(fname, isAbs);
    if (!isAbs)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("SandBox: Invalid file name.");
    path_base::normalize(fname, fname);

    obj_ptr<Buffer_base> bin;
    hr = resolveFile(fname, "", bin, kCommonJS, fname, NULL);
    if (hr.failed())
        return hr;

    obj_ptr<ExtLoader> l;
    hr = get_loader(fname, l);
    if (hr.failed())
        return hr;

    Context context(this, fname);
    std::vector<ExtLoader::arg> extarg;

    return l->run_script(&context, bin, fname, extarg, false, in_cjs);
}

result_t SandBox::run(exlib::string fname)
{
    return run(fname, false);
}

}
