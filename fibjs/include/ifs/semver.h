/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class SemVerOptions;

class semver_base : public object_base {
    DECLARE_CLASS(semver_base);

public:
    using Union_inc_identifierBase = std::variant<int32_t, bool>;
    using Union_coerce_version = std::variant<exlib::string, double>;

public:
    class ParseType : public NType {
    public:
        virtual void to_value(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = isolate->context();
            retVal->Set(context, isolate->NewString("version"), GetReturnValue(isolate, version)).Check();
            retVal->Set(context, isolate->NewString("raw"), GetReturnValue(isolate, raw)).Check();
            retVal->Set(context, isolate->NewString("major"), GetReturnValue(isolate, major)).Check();
            retVal->Set(context, isolate->NewString("minor"), GetReturnValue(isolate, minor)).Check();
            retVal->Set(context, isolate->NewString("patch"), GetReturnValue(isolate, patch)).Check();
            retVal->Set(context, isolate->NewString("prerelease"), GetReturnValue(isolate, prerelease)).Check();
            retVal->Set(context, isolate->NewString("build"), GetReturnValue(isolate, build)).Check();
        }

        virtual void to_args(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, version));
            args.push_back(GetReturnValue(isolate, raw));
            args.push_back(GetReturnValue(isolate, major));
            args.push_back(GetReturnValue(isolate, minor));
            args.push_back(GetReturnValue(isolate, patch));
            args.push_back(GetReturnValue(isolate, prerelease));
            args.push_back(GetReturnValue(isolate, build));
        }

    public:
        exlib::string version;
        exlib::string raw;
        int64_t major;
        int64_t minor;
        int64_t patch;
        v8::Local<v8::Array> prerelease;
        std::vector<exlib::string> build;
    };
    class MinVersionType : public NType {
    public:
        virtual void to_value(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = isolate->context();
            retVal->Set(context, isolate->NewString("version"), GetReturnValue(isolate, version)).Check();
            retVal->Set(context, isolate->NewString("raw"), GetReturnValue(isolate, raw)).Check();
            retVal->Set(context, isolate->NewString("major"), GetReturnValue(isolate, major)).Check();
            retVal->Set(context, isolate->NewString("minor"), GetReturnValue(isolate, minor)).Check();
            retVal->Set(context, isolate->NewString("patch"), GetReturnValue(isolate, patch)).Check();
            retVal->Set(context, isolate->NewString("prerelease"), GetReturnValue(isolate, prerelease)).Check();
            retVal->Set(context, isolate->NewString("build"), GetReturnValue(isolate, build)).Check();
        }

        virtual void to_args(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, version));
            args.push_back(GetReturnValue(isolate, raw));
            args.push_back(GetReturnValue(isolate, major));
            args.push_back(GetReturnValue(isolate, minor));
            args.push_back(GetReturnValue(isolate, patch));
            args.push_back(GetReturnValue(isolate, prerelease));
            args.push_back(GetReturnValue(isolate, build));
        }

    public:
        exlib::string version;
        exlib::string raw;
        int64_t major;
        int64_t minor;
        int64_t patch;
        v8::Local<v8::Array> prerelease;
        std::vector<exlib::string> build;
    };
    class CoerceType : public NType {
    public:
        virtual void to_value(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = isolate->context();
            retVal->Set(context, isolate->NewString("version"), GetReturnValue(isolate, version)).Check();
            retVal->Set(context, isolate->NewString("raw"), GetReturnValue(isolate, raw)).Check();
            retVal->Set(context, isolate->NewString("major"), GetReturnValue(isolate, major)).Check();
            retVal->Set(context, isolate->NewString("minor"), GetReturnValue(isolate, minor)).Check();
            retVal->Set(context, isolate->NewString("patch"), GetReturnValue(isolate, patch)).Check();
            retVal->Set(context, isolate->NewString("prerelease"), GetReturnValue(isolate, prerelease)).Check();
            retVal->Set(context, isolate->NewString("build"), GetReturnValue(isolate, build)).Check();
        }

        virtual void to_args(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, version));
            args.push_back(GetReturnValue(isolate, raw));
            args.push_back(GetReturnValue(isolate, major));
            args.push_back(GetReturnValue(isolate, minor));
            args.push_back(GetReturnValue(isolate, patch));
            args.push_back(GetReturnValue(isolate, prerelease));
            args.push_back(GetReturnValue(isolate, build));
        }

    public:
        exlib::string version;
        exlib::string raw;
        int64_t major;
        int64_t minor;
        int64_t patch;
        v8::Local<v8::Array> prerelease;
        std::vector<exlib::string> build;
    };

public:
    // semver_base
    static result_t get_SEMVER_SPEC_VERSION(exlib::string& retVal);
    static result_t parse(exlib::string version, SemVerOptions* options, obj_ptr<ParseType>& retVal);
    static result_t valid(exlib::string version, SemVerOptions* options, exlib::string& retVal);
    static result_t clean(exlib::string version, SemVerOptions* options, exlib::string& retVal);
    static result_t compare(exlib::string version1, exlib::string version2, SemVerOptions* options, int32_t& retVal);
    static result_t compareLoose(exlib::string version1, exlib::string version2, int32_t& retVal);
    static result_t compareBuild(exlib::string version1, exlib::string version2, SemVerOptions* options, int32_t& retVal);
    static result_t rcompare(exlib::string version1, exlib::string version2, SemVerOptions* options, int32_t& retVal);
    static result_t sort(std::vector<exlib::string>& list, SemVerOptions* options, std::vector<exlib::string>& retVal);
    static result_t rsort(std::vector<exlib::string>& list, SemVerOptions* options, std::vector<exlib::string>& retVal);
    static result_t gt(exlib::string version1, exlib::string version2, SemVerOptions* options, bool& retVal);
    static result_t gte(exlib::string version1, exlib::string version2, SemVerOptions* options, bool& retVal);
    static result_t lt(exlib::string version1, exlib::string version2, SemVerOptions* options, bool& retVal);
    static result_t lte(exlib::string version1, exlib::string version2, SemVerOptions* options, bool& retVal);
    static result_t eq(exlib::string version1, exlib::string version2, SemVerOptions* options, bool& retVal);
    static result_t neq(exlib::string version1, exlib::string version2, SemVerOptions* options, bool& retVal);
    static result_t cmp(exlib::string version1, exlib::string op, exlib::string version2, SemVerOptions* options, bool& retVal);
    static result_t validRange(exlib::string range, SemVerOptions* options, exlib::string& retVal);
    static result_t satisfies(exlib::string version, exlib::string range, SemVerOptions* options, bool& retVal);
    static result_t maxSatisfying(std::vector<exlib::string>& versions, exlib::string range, SemVerOptions* options, exlib::string& retVal);
    static result_t minSatisfying(std::vector<exlib::string>& versions, exlib::string range, SemVerOptions* options, exlib::string& retVal);
    static result_t minVersion(exlib::string range, SemVerOptions* options, obj_ptr<MinVersionType>& retVal);
    static result_t gtr(exlib::string version, exlib::string range, SemVerOptions* options, bool& retVal);
    static result_t ltr(exlib::string version, exlib::string range, SemVerOptions* options, bool& retVal);
    static result_t outside(exlib::string version, exlib::string range, exlib::string hilo, SemVerOptions* options, bool& retVal);
    static result_t intersects(exlib::string range1, exlib::string range2, SemVerOptions* options, bool& retVal);
    static result_t prerelease(exlib::string version, SemVerOptions* options, v8::Local<v8::Array>& retVal);
    static result_t major(exlib::string version, SemVerOptions* options, int64_t& retVal);
    static result_t minor(exlib::string version, SemVerOptions* options, int64_t& retVal);
    static result_t patch(exlib::string version, SemVerOptions* options, int64_t& retVal);
    static result_t inc(exlib::string version, exlib::string release, SemVerOptions* options, exlib::string identifier, Union_inc_identifierBase identifierBase, exlib::string& retVal);
    static result_t diff(exlib::string version1, exlib::string version2, exlib::string& retVal);
    static result_t coerce(Union_coerce_version version, SemVerOptions* options, obj_ptr<CoerceType>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<semver_base>& retVal)
    { return result_t::type_mismatch(); }

public:
    static void s_static_get_SEMVER_SPEC_VERSION(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_parse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_valid(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_clean(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_compare(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_compareLoose(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_compareBuild(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_rcompare(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_sort(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_rsort(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_gt(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_gte(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_lt(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_lte(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_eq(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_neq(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_cmp(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_validRange(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_satisfies(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_maxSatisfying(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_minSatisfying(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_minVersion(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_gtr(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_ltr(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_outside(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_intersects(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_prerelease(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_major(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_minor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_patch(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_inc(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_diff(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_coerce(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/SemVerOptions.h"

namespace fibjs {
inline ClassInfo& semver_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "parse", s_static_parse, true, ClassData::ASYNC_SYNC },
        { "valid", s_static_valid, true, ClassData::ASYNC_SYNC },
        { "clean", s_static_clean, true, ClassData::ASYNC_SYNC },
        { "compare", s_static_compare, true, ClassData::ASYNC_SYNC },
        { "compareLoose", s_static_compareLoose, true, ClassData::ASYNC_SYNC },
        { "compareBuild", s_static_compareBuild, true, ClassData::ASYNC_SYNC },
        { "rcompare", s_static_rcompare, true, ClassData::ASYNC_SYNC },
        { "sort", s_static_sort, true, ClassData::ASYNC_SYNC },
        { "rsort", s_static_rsort, true, ClassData::ASYNC_SYNC },
        { "gt", s_static_gt, true, ClassData::ASYNC_SYNC },
        { "gte", s_static_gte, true, ClassData::ASYNC_SYNC },
        { "lt", s_static_lt, true, ClassData::ASYNC_SYNC },
        { "lte", s_static_lte, true, ClassData::ASYNC_SYNC },
        { "eq", s_static_eq, true, ClassData::ASYNC_SYNC },
        { "neq", s_static_neq, true, ClassData::ASYNC_SYNC },
        { "cmp", s_static_cmp, true, ClassData::ASYNC_SYNC },
        { "validRange", s_static_validRange, true, ClassData::ASYNC_SYNC },
        { "satisfies", s_static_satisfies, true, ClassData::ASYNC_SYNC },
        { "maxSatisfying", s_static_maxSatisfying, true, ClassData::ASYNC_SYNC },
        { "minSatisfying", s_static_minSatisfying, true, ClassData::ASYNC_SYNC },
        { "minVersion", s_static_minVersion, true, ClassData::ASYNC_SYNC },
        { "gtr", s_static_gtr, true, ClassData::ASYNC_SYNC },
        { "ltr", s_static_ltr, true, ClassData::ASYNC_SYNC },
        { "outside", s_static_outside, true, ClassData::ASYNC_SYNC },
        { "intersects", s_static_intersects, true, ClassData::ASYNC_SYNC },
        { "prerelease", s_static_prerelease, true, ClassData::ASYNC_SYNC },
        { "major", s_static_major, true, ClassData::ASYNC_SYNC },
        { "minor", s_static_minor, true, ClassData::ASYNC_SYNC },
        { "patch", s_static_patch, true, ClassData::ASYNC_SYNC },
        { "inc", s_static_inc, true, ClassData::ASYNC_SYNC },
        { "diff", s_static_diff, true, ClassData::ASYNC_SYNC },
        { "coerce", s_static_coerce, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "SEMVER_SPEC_VERSION", s_static_get_SEMVER_SPEC_VERSION, block_set, true }
    };

    static ClassData s_cd = {
        "semver", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void semver_base::s_static_get_SEMVER_SPEC_VERSION(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_SEMVER_SPEC_VERSION(vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_parse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<ParseType> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 1, "version");

    ARG_NAMED(exlib::string, 0, "version");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 1, new SemVerOptions(), "options");

    hr = parse(v0, (SemVerOptions*)v1, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_valid(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 1, "version");

    ARG_NAMED(exlib::string, 0, "version");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 1, new SemVerOptions(), "options");

    hr = valid(v0, (SemVerOptions*)v1, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_clean(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 1, "version");

    ARG_NAMED(exlib::string, 0, "version");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 1, new SemVerOptions(), "options");

    hr = clean(v0, (SemVerOptions*)v1, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_compare(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(3, 2, "version2");

    ARG_NAMED(exlib::string, 0, "version1");
    ARG_NAMED(exlib::string, 1, "version2");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 2, new SemVerOptions(), "options");

    hr = compare(v0, v1, (SemVerOptions*)v2, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_compareLoose(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 2, "version2");

    ARG_NAMED(exlib::string, 0, "version1");
    ARG_NAMED(exlib::string, 1, "version2");

    hr = compareLoose(v0, v1, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_compareBuild(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(3, 2, "version2");

    ARG_NAMED(exlib::string, 0, "version1");
    ARG_NAMED(exlib::string, 1, "version2");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 2, new SemVerOptions(), "options");

    hr = compareBuild(v0, v1, (SemVerOptions*)v2, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_rcompare(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(3, 2, "version2");

    ARG_NAMED(exlib::string, 0, "version1");
    ARG_NAMED(exlib::string, 1, "version2");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 2, new SemVerOptions(), "options");

    hr = rcompare(v0, v1, (SemVerOptions*)v2, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_sort(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::vector<exlib::string> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 1, "list");

    ARG_NAMED(std::vector<exlib::string>, 0, "list");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 1, new SemVerOptions(), "options");

    hr = sort(v0, (SemVerOptions*)v1, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_rsort(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::vector<exlib::string> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 1, "list");

    ARG_NAMED(std::vector<exlib::string>, 0, "list");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 1, new SemVerOptions(), "options");

    hr = rsort(v0, (SemVerOptions*)v1, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_gt(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(3, 2, "version2");

    ARG_NAMED(exlib::string, 0, "version1");
    ARG_NAMED(exlib::string, 1, "version2");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 2, new SemVerOptions(), "options");

    hr = gt(v0, v1, (SemVerOptions*)v2, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_gte(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(3, 2, "version2");

    ARG_NAMED(exlib::string, 0, "version1");
    ARG_NAMED(exlib::string, 1, "version2");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 2, new SemVerOptions(), "options");

    hr = gte(v0, v1, (SemVerOptions*)v2, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_lt(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(3, 2, "version2");

    ARG_NAMED(exlib::string, 0, "version1");
    ARG_NAMED(exlib::string, 1, "version2");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 2, new SemVerOptions(), "options");

    hr = lt(v0, v1, (SemVerOptions*)v2, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_lte(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(3, 2, "version2");

    ARG_NAMED(exlib::string, 0, "version1");
    ARG_NAMED(exlib::string, 1, "version2");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 2, new SemVerOptions(), "options");

    hr = lte(v0, v1, (SemVerOptions*)v2, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_eq(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(3, 2, "version2");

    ARG_NAMED(exlib::string, 0, "version1");
    ARG_NAMED(exlib::string, 1, "version2");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 2, new SemVerOptions(), "options");

    hr = eq(v0, v1, (SemVerOptions*)v2, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_neq(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(3, 2, "version2");

    ARG_NAMED(exlib::string, 0, "version1");
    ARG_NAMED(exlib::string, 1, "version2");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 2, new SemVerOptions(), "options");

    hr = neq(v0, v1, (SemVerOptions*)v2, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_cmp(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(4, 3, "version2");

    ARG_NAMED(exlib::string, 0, "version1");
    ARG_NAMED(exlib::string, 1, "op");
    ARG_NAMED(exlib::string, 2, "version2");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 3, new SemVerOptions(), "options");

    hr = cmp(v0, v1, v2, (SemVerOptions*)v3, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_validRange(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 1, "range");

    ARG_NAMED(exlib::string, 0, "range");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 1, new SemVerOptions(), "options");

    hr = validRange(v0, (SemVerOptions*)v1, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_satisfies(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(3, 2, "range");

    ARG_NAMED(exlib::string, 0, "version");
    ARG_NAMED(exlib::string, 1, "range");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 2, new SemVerOptions(), "options");

    hr = satisfies(v0, v1, (SemVerOptions*)v2, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_maxSatisfying(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(3, 2, "range");

    ARG_NAMED(std::vector<exlib::string>, 0, "versions");
    ARG_NAMED(exlib::string, 1, "range");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 2, new SemVerOptions(), "options");

    hr = maxSatisfying(v0, v1, (SemVerOptions*)v2, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_minSatisfying(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(3, 2, "range");

    ARG_NAMED(std::vector<exlib::string>, 0, "versions");
    ARG_NAMED(exlib::string, 1, "range");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 2, new SemVerOptions(), "options");

    hr = minSatisfying(v0, v1, (SemVerOptions*)v2, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_minVersion(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<MinVersionType> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 1, "range");

    ARG_NAMED(exlib::string, 0, "range");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 1, new SemVerOptions(), "options");

    hr = minVersion(v0, (SemVerOptions*)v1, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_gtr(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(3, 2, "range");

    ARG_NAMED(exlib::string, 0, "version");
    ARG_NAMED(exlib::string, 1, "range");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 2, new SemVerOptions(), "options");

    hr = gtr(v0, v1, (SemVerOptions*)v2, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_ltr(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(3, 2, "range");

    ARG_NAMED(exlib::string, 0, "version");
    ARG_NAMED(exlib::string, 1, "range");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 2, new SemVerOptions(), "options");

    hr = ltr(v0, v1, (SemVerOptions*)v2, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_outside(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(4, 3, "hilo");

    ARG_NAMED(exlib::string, 0, "version");
    ARG_NAMED(exlib::string, 1, "range");
    ARG_NAMED(exlib::string, 2, "hilo");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 3, new SemVerOptions(), "options");

    hr = outside(v0, v1, v2, (SemVerOptions*)v3, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_intersects(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(3, 2, "range2");

    ARG_NAMED(exlib::string, 0, "range1");
    ARG_NAMED(exlib::string, 1, "range2");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 2, new SemVerOptions(), "options");

    hr = intersects(v0, v1, (SemVerOptions*)v2, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_prerelease(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 1, "version");

    ARG_NAMED(exlib::string, 0, "version");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 1, new SemVerOptions(), "options");

    hr = prerelease(v0, (SemVerOptions*)v1, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_major(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 1, "version");

    ARG_NAMED(exlib::string, 0, "version");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 1, new SemVerOptions(), "options");

    hr = major(v0, (SemVerOptions*)v1, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_minor(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 1, "version");

    ARG_NAMED(exlib::string, 0, "version");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 1, new SemVerOptions(), "options");

    hr = minor(v0, (SemVerOptions*)v1, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_patch(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 1, "version");

    ARG_NAMED(exlib::string, 0, "version");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 1, new SemVerOptions(), "options");

    hr = patch(v0, (SemVerOptions*)v1, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_inc(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(5, 2, "release");

    ARG_NAMED(exlib::string, 0, "version");
    ARG_NAMED(exlib::string, 1, "release");
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 2, new SemVerOptions(), "options");
    OPT_ARG_NAMED(exlib::string, 3, "", "identifier");
    OPT_ARG_NAMED(Union_inc_identifierBase, 4, 0, "identifierBase");

    hr = inc(v0, v1, (SemVerOptions*)v2, v3, v4, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_diff(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 2, "version2");

    ARG_NAMED(exlib::string, 0, "version1");
    ARG_NAMED(exlib::string, 1, "version2");

    hr = diff(v0, v1, vr);

    METHOD_RETURN();
}

inline void semver_base::s_static_coerce(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<CoerceType> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 1, "version");

    ARG(Union_coerce_version, 0);
    OPT_ARG_NAMED(obj_ptr<SemVerOptions>, 1, new SemVerOptions(), "options");

    hr = coerce(v0, (SemVerOptions*)v1, vr);

    METHOD_RETURN();
}
}
