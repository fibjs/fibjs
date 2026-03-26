/*
 * SandBox_run.cpp
 *
 *  Created on: Oct 22, 2012
 *      Author: lion
 */

#include "object.h"
#include "SandBox.h"
#include "Buffer.h"
#include "path.h"
#include "parse.h"
#include "options.h"
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

// Check if a .bin script has node/fibjs shebang
static bool is_node_shebang_script(const exlib::string& cmd)
{
    exlib::string cwd;
    process_base::cwd(cwd);

    exlib::string bin_path = cwd + "/node_modules/.bin/" + cmd;

    Variant var;
    result_t hr = fs_base::cc_readFile(bin_path, "", var, Isolate::current());
    if (hr < 0)
        return false;

    Buffer* b = (Buffer*)var.object();
    if (!b)
        return false;

    const char* pdata = (const char*)b->data();
    size_t len = b->length();

    if (len < 2 || pdata[0] != '#' || pdata[1] != '!')
        return false;

    _parser p(pdata, (int32_t)len);
    exlib::string line;
    p.getLine(line);

    // If shebang contains "node" or "fibjs", it's a node script
    return line.find("node") != exlib::string::npos || line.find("fibjs") != exlib::string::npos;
}

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
            } else if (is_node_shebang_script(original_cmd)) {
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

static result_t run_shell(exlib::string fname, const std::vector<char*>& args)
{
    exlib::string cmd_str = replace_node_command(fname);

    for (size_t i = 2; i < args.size(); i++) {
        cmd_str += " ";
        cmd_str += args[i];
    }

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> opts = v8::Object::New(isolate->m_isolate);
    opts->Set(context, isolate->NewString("stdio"), isolate->NewString("inherit")).IsJust();

    // Add node_modules/.bin to PATH
    v8::Local<v8::Object> env;
    process_base::get_env(env);

    exlib::string cwd;
    process_base::cwd(cwd);

    exlib::string bin_path = cwd + "/node_modules/.bin";
    v8::Local<v8::Value> path_val = env->Get(context, isolate->NewString("PATH")).FromMaybe(v8::Local<v8::Value>());
    exlib::string new_path = bin_path;
    if (!IsEmpty(path_val)) {
        new_path += PATH_DELIMITER;
        new_path += isolate->toString(path_val);
    }
    env->Set(context, isolate->NewString("PATH"), isolate->NewString(new_path)).IsJust();
    opts->Set(context, isolate->NewString("env"), env).IsJust();

    obj_ptr<child_process_base::ExecType> exec_retVal;
    result_t hr = child_process_base::ac_exec(cmd_str, opts, exec_retVal);
    if (hr < 0)
        return hr;

    process_base::exit(exec_retVal->exitCode);
    return 0;
}

static result_t run_shell(exlib::string cmd_str)
{
    return run_shell(cmd_str, std::vector<char*>());
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

            fname = "node_modules/.bin/" + rname;
            os_resolve(fname);

            hr = resolveFile(fname, "", bin, kCommonJS, fname, NULL);
            if (hr >= 0) {
                // Check shebang to determine if we can run directly with fibjs
                Buffer* b = Buffer::Cast(bin);
                const char* pdata = (const char*)b->data();

                if (pdata[0] == '#' && pdata[1] == '!') {
                    _parser p(pdata, (int32_t)b->length());
                    exlib::string line;
                    p.getLine(line);

                    // If shebang contains "node" or "fibjs", run directly with fibjs
                    if (line.find("node") != exlib::string::npos || line.find("fibjs") != exlib::string::npos) {
                        // Run with fibjs directly, argv[1] update handled below
                    } else {
                        // Not a node/fibjs script, run via shell
                        return run_shell(fname, s_argv);
                    }
                } else {
                    // No shebang, run via shell
                    return run_shell(fname, s_argv);
                }
            } else {
                // File not found in .bin, try package.json scripts
                v8::Local<v8::Value> v;
                exlib::string buf;
                Isolate* isolate = holder();
                v8::Local<v8::Context> context = isolate->context();

                hr = loadFile("package.json", bin);
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

                return run_shell(isolate->toString(cmd), s_argv);
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

result_t SandBox::run(exlib::string fname, bool in_cjs)
{
    Scope _scope(this);

    result_t hr;
    bool isAbs;

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
