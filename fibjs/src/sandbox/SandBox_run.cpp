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

// <name> in node_modules/.bin, nearest ancestor first. Absolute paths and names
// carrying a separator are left to the caller: only bare command names are
// resolved like npm exec does.
static bool find_bin_upward(const exlib::string& name, exlib::string& retVal)
{
    if (name.empty() || name.find('/') != exlib::string::npos || name.find('\\') != exlib::string::npos)
        return false;

    exlib::string dir;
    process_base::cwd(dir);

    while (!dir.empty()) {
        exlib::string path = dir + "/node_modules/.bin/" + name;

        if (path_type(path) == 0) {
            retVal = path;
            return true;
        }

        exlib::string parent;
        os_dirname(dir, parent);

        if (parent.empty() || parent == dir)
            break;

        dir = parent;
    }

    return false;
}

// Nearest ancestor that holds a package.json.
static bool find_package_root(exlib::string& retVal)
{
    exlib::string dir;
    process_base::cwd(dir);

    while (!dir.empty()) {
        if (path_type(dir + "/package.json") == 0) {
            retVal = dir;
            return true;
        }

        exlib::string parent;
        os_dirname(dir, parent);

        if (parent.empty() || parent == dir)
            break;

        dir = parent;
    }

    return false;
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

    if (hr < 0)
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

static result_t run_shell(exlib::string cmd_str, const std::vector<char*>& args, exlib::string cwd)
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

    // a package script runs from the root of its package (npm runs lifecycle
    // scripts there, and keeps the directory the user typed in INIT_CWD)
    if (!cwd.empty())
        opts->Set(context, isolate->NewString("cwd"), isolate->NewString(cwd)).IsJust();

    // Add node_modules/.bin to PATH. The tooling comes from the directory the
    // command runs in: a package script runs from the root of its package, a
    // .bin entry from the directory it was typed in.
    v8::Local<v8::Object> env;
    process_base::get_env(env);

    exlib::string workdir;
    process_base::cwd(workdir);

    exlib::string bin_path = (cwd.empty() ? workdir : cwd) + "/node_modules/.bin";
    v8::Local<v8::Value> path_val = env->Get(context, isolate->NewString("PATH")).FromMaybe(v8::Local<v8::Value>());
    exlib::string new_path = bin_path;
    if (!IsEmpty(path_val)) {
        new_path += PATH_DELIMITER;
        new_path += isolate->toString(path_val);
    }
    env->Set(context, isolate->NewString("PATH"), isolate->NewString(new_path)).IsJust();
    env->Set(context, isolate->NewString("INIT_CWD"), isolate->NewString(workdir)).IsJust();
    opts->Set(context, isolate->NewString("env"), env).IsJust();

    // While the script runs, Ctrl-C / SIGTERM are forwarded to it and its exit
    // status decides ours; a signal death becomes 128 + signum.
    process_signal_forward_children(true);

    obj_ptr<child_process_base::ExecType> exec_retVal;
    result_t hr = child_process_base::ac_exec(cmd, opts, exec_retVal);

    process_signal_forward_children(false);

    if (hr < 0)
        return hr;

    int32_t code = exec_retVal->exitCode;

    if (code < 0) {
        // the child was killed by a signal: leave the same way
        int32_t signum = -code;

        if (!signal_reraise(signum))
            code = 128 + signum;
    }

    process_base::exit(code);
    return 0;
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
        if (hr < 0) {
            if (isAbs)
                return hr;

            exlib::string workdir;
            process_base::cwd(workdir);

            // node_modules/.bin/<name>, nearest ancestor first: like `npm exec`,
            // a script started in a subdirectory finds the tooling installed at
            // the root of the project.
            exlib::string bin_path;

            if (find_bin_upward(rname, bin_path)) {
                exlib::string resolved = bin_path;

                hr = resolveFile(resolved, "", bin, kCommonJS, resolved, NULL);
                if (hr >= 0) {
                    Buffer* b = Buffer::Cast(bin);

                    // A node/fibjs script runs on fibjs itself; anything else
                    // (shell script, binary, no shebang) goes to the shell.
                    if (is_node_shebang((const char*)b->data(), b->length())) {
                        // run in-process, argv[1] is updated below
                        fname = resolved;
                        needUpdateArgv = true;
                    } else {
                        return run_shell(resolved, s_argv, workdir);
                    }
                }
            }

            if (hr < 0) {
                // package.json scripts of the nearest package, run from its root
                exlib::string pkg_root;

                if (!find_package_root(pkg_root))
                    return CALL_E_FILE_NOT_FOUND;

                v8::Local<v8::Value> v;
                exlib::string buf;
                Isolate* isolate = holder();
                v8::Local<v8::Context> context = isolate->context();

                hr = loadFile(pkg_root + "/package.json", bin);
                if (hr < 0)
                    return CALL_E_FILE_NOT_FOUND;

                bin->toString(buf);
                hr = json_base::decode(buf, v);
                if (hr < 0)
                    return hr;

                if (v.IsEmpty() || !v->IsObject())
                    return CHECK_ERROR(Runtime::setError("SandBox: Invalid package.json"));

                v8::Local<v8::Object> o = v.As<v8::Object>();
                v8::Local<v8::Value> scripts = o->Get(context, isolate->NewString("scripts", 7)).FromMaybe(v8::Local<v8::Value>());
                if (IsEmpty(scripts) || !scripts->IsObject())
                    return CALL_E_FILE_NOT_FOUND;

                o = scripts.As<v8::Object>();

                v8::Local<v8::Value> cmd = o->Get(context, isolate->NewString(rname)).FromMaybe(v8::Local<v8::Value>());
                if (IsEmpty(cmd) || !cmd->IsString())
                    return CALL_E_FILE_NOT_FOUND;

                return run_shell(isolate->toString(cmd), s_argv, pkg_root);
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
    if (hr < 0)
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
    if (hr < 0)
        return hr;

    path_base::isAbsolute(fname, isAbs);
    if (!isAbs)
        return CHECK_ERROR(Runtime::setError("SandBox: Invalid file name."));
    path_base::normalize(fname, fname);

    obj_ptr<Buffer_base> bin;
    hr = resolveFile(fname, "", bin, kCommonJS, fname, NULL);
    if (hr < 0)
        return hr;

    obj_ptr<ExtLoader> l;
    hr = get_loader(fname, l);
    if (hr < 0)
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
    if (hr < 0)
        return hr;

    path_base::isAbsolute(fname, isAbs);
    if (!isAbs)
        return CHECK_ERROR(Runtime::setError("SandBox: Invalid file name."));
    path_base::normalize(fname, fname);

    obj_ptr<Buffer_base> bin = new Buffer(source.c_str(), source.length());

    obj_ptr<ExtLoader> l;
    hr = get_loader(fname, l);
    if (hr < 0)
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
    if (hr < 0)
        return hr;

    path_base::isAbsolute(fname, isAbs);
    if (!isAbs)
        return CHECK_ERROR(Runtime::setError("SandBox: Invalid file name."));
    path_base::normalize(fname, fname);

    obj_ptr<Buffer_base> bin;
    hr = resolveFile(fname, "", bin, kCommonJS, fname, NULL);
    if (hr < 0)
        return hr;

    obj_ptr<ExtLoader> l;
    hr = get_loader(fname, l);
    if (hr < 0)
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
