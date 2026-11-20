/*
 * semver.cpp
 *
 *  Semantic Versioning 2.0.0 parser, comparator and range matcher.
 *  The algorithm mirrors the npm `semver` package (v7.x).
 */

#include "object.h"
#include "ifs/semver.h"
#include "ifs/SemVerOptions.h"
#include "semver.h"

#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

namespace fibjs {

DECLARE_MODULE(semver);

namespace semver_detail {

// ---------------------- string helpers :start ---------------------- //

static bool is_digit(char c)
{
    return c >= '0' && c <= '9';
}

static bool is_alpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static bool is_alnum(char c)
{
    return is_digit(c) || is_alpha(c);
}

static bool is_space(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

static std::string trim(const std::string& s)
{
    size_t b = 0, e = s.size();
    while (b < e && is_space(s[b]))
        b++;
    while (e > b && is_space(s[e - 1]))
        e--;
    return s.substr(b, e - b);
}

// JS `s.trim().replace(/\s+/g, ' ')`
static std::string collapse_ws(const std::string& s)
{
    std::string out;
    bool pendingSpace = false;

    for (size_t i = 0; i < s.size(); i++) {
        if (is_space(s[i])) {
            pendingSpace = !out.empty();
        } else {
            if (pendingSpace) {
                out += ' ';
                pendingSpace = false;
            }
            out += s[i];
        }
    }
    return out;
}

static std::vector<std::string> split_ws(const std::string& s)
{
    std::vector<std::string> out;
    size_t i = 0, n = s.size();

    if (n == 0) {
        out.push_back("");
        return out;
    }

    while (i < n) {
        while (i < n && is_space(s[i]))
            i++;
        if (i >= n)
            break;
        size_t b = i;
        while (i < n && !is_space(s[i]))
            i++;
        out.push_back(s.substr(b, i - b));
    }

    if (out.empty())
        out.push_back("");
    return out;
}

static bool starts_with(const std::string& s, const char* prefix)
{
    size_t n = strlen(prefix);
    return s.size() >= n && s.compare(0, n, prefix) == 0;
}

// ---------------------- number helpers :start ---------------------- //

static uint64_t identifier_number(const std::string& text)
{
    uint64_t v = 0;

    for (size_t i = 0; i < text.size(); i++) {
        if (!is_digit(text[i])) {
            if (i == 0)
                return 0;
            break;
        }
        if (v > (UINT64_MAX - 9) / 10)
            return UINT64_MAX;
        v = v * 10 + (uint64_t)(text[i] - '0');
    }
    return v;
}

static std::string next_number(const std::string& text)
{
    uint64_t v = identifier_number(text);
    if (v == UINT64_MAX)
        return std::to_string(v);
    return std::to_string(v + 1);
}

static int compare_numeric_text(const std::string& a, const std::string& b)
{
    size_t ai = 0, bi = 0;

    while (ai + 1 < a.size() && a[ai] == '0')
        ai++;
    while (bi + 1 < b.size() && b[bi] == '0')
        bi++;

    size_t alen = a.size() - ai;
    size_t blen = b.size() - bi;

    if (alen != blen)
        return alen < blen ? -1 : 1;

    int c = a.compare(ai, alen, b, bi, blen);
    return c < 0 ? -1 : (c > 0 ? 1 : 0);
}

// ---------------------- version :start ---------------------- //

void SemVer::format()
{
    version = std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);

    if (!prerelease.empty()) {
        version += '-';
        for (size_t i = 0; i < prerelease.size(); i++) {
            if (i)
                version += '.';
            version += prerelease[i].text;
        }
    }
}

static bool read_main_number(const std::string& s, size_t& pos, bool loose, uint64_t& out)
{
    size_t n = s.size();
    size_t start = pos;

    if (pos >= n || !is_digit(s[pos]))
        return false;

    if (!loose && s[pos] == '0') {
        pos++;
        if (pos < n && is_digit(s[pos]))
            return false;
        out = 0;
        return true;
    }

    uint64_t v = 0;
    while (pos < n && is_digit(s[pos])) {
        uint64_t d = (uint64_t)(s[pos] - '0');
        if (v > (MAX_SAFE_INTEGER - d) / 10)
            return false;
        v = v * 10 + d;
        pos++;
    }

    out = v;
    return pos > start;
}

static bool is_all_digits(const std::string& s)
{
    if (s.empty())
        return false;
    for (size_t i = 0; i < s.size(); i++)
        if (!is_digit(s[i]))
            return false;
    return true;
}

// A prerelease identifier is numeric (`0|[1-9]\d*` strict, `[0-9]+` loose) or
// non-numeric (`\d*[a-zA-Z-][a-zA-Z0-9-]*`).
static bool make_prerelease_identifier(const std::string& text, bool loose, Identifier& id)
{
    if (is_all_digits(text)) {
        if (!loose && text.size() > 1 && text[0] == '0')
            return false;

        id.numeric = true;

        uint64_t v = 0;
        bool overflow = false;
        for (size_t i = 0; i < text.size(); i++) {
            if (v > (UINT64_MAX - 9) / 10) {
                overflow = true;
                break;
            }
            v = v * 10 + (uint64_t)(text[i] - '0');
        }

        if (!overflow && v < MAX_SAFE_INTEGER) {
            id.is_number = true;
            id.number = v;
            id.text = std::to_string(v);
        } else {
            id.is_number = false;
            id.text = text;
        }
        return true;
    }

    size_t i = 0;
    while (i < text.size() && is_digit(text[i]))
        i++;
    if (i >= text.size())
        return false;
    if (!(is_alpha(text[i]) || text[i] == '-'))
        return false;

    for (size_t k = i; k < text.size(); k++)
        if (!(is_alnum(text[k]) || text[k] == '-'))
            return false;

    id.numeric = false;
    id.is_number = false;
    id.text = text;
    return true;
}

static Identifier identifier_from_text(const std::string& text)
{
    Identifier id;
    id.text = text;

    if (is_all_digits(text)) {
        id.numeric = true;

        uint64_t v = 0;
        bool overflow = false;
        for (size_t i = 0; i < text.size(); i++) {
            if (v > (UINT64_MAX - 9) / 10) {
                overflow = true;
                break;
            }
            v = v * 10 + (uint64_t)(text[i] - '0');
        }

        if (!overflow && v < MAX_SAFE_INTEGER) {
            id.is_number = true;
            id.number = v;
            id.text = std::to_string(v);
        }
    }

    return id;
}

static bool read_identifier_text(const std::string& s, size_t& pos, std::string& out)
{
    size_t start = pos;
    while (pos < s.size() && (is_alnum(s[pos]) || s[pos] == '-'))
        pos++;
    if (pos == start)
        return false;
    out = s.substr(start, pos - start);
    return true;
}

static bool read_prerelease_ids(const std::string& s, size_t& pos, bool loose,
    std::vector<Identifier>& out)
{
    while (true) {
        std::string text;
        if (!read_identifier_text(s, pos, text))
            return false;

        Identifier id;
        if (!make_prerelease_identifier(text, loose, id))
            return false;
        out.push_back(id);

        if (pos < s.size() && s[pos] == '.') {
            pos++;
            continue;
        }
        break;
    }
    return true;
}

static bool read_build_ids(const std::string& s, size_t& pos, std::vector<std::string>& out)
{
    while (true) {
        std::string text;
        if (!read_identifier_text(s, pos, text))
            return false;
        out.push_back(text);

        if (pos < s.size() && s[pos] == '.') {
            pos++;
            continue;
        }
        break;
    }
    return true;
}

bool parse_version(const std::string& input, const Options& options, SemVer& out)
{
    if (input.size() > MAX_LENGTH)
        return false;

    std::string s = trim(input);
    size_t pos = 0, n = s.size();

    if (options.loose) {
        while (pos < n && (s[pos] == 'v' || s[pos] == '=' || is_space(s[pos])))
            pos++;
    } else if (pos < n && s[pos] == 'v') {
        pos++;
    }

    uint64_t major, minor, patch;
    if (!read_main_number(s, pos, options.loose, major))
        return false;
    if (pos >= n || s[pos] != '.')
        return false;
    pos++;
    if (!read_main_number(s, pos, options.loose, minor))
        return false;
    if (pos >= n || s[pos] != '.')
        return false;
    pos++;
    if (!read_main_number(s, pos, options.loose, patch))
        return false;

    out.prerelease.clear();
    out.build.clear();

    if (pos < n && s[pos] == '-') {
        pos++;
        if (!read_prerelease_ids(s, pos, options.loose, out.prerelease))
            return false;
    } else if (options.loose && pos < n && s[pos] != '+') {
        if (!read_prerelease_ids(s, pos, options.loose, out.prerelease))
            return false;
    }

    if (pos < n && s[pos] == '+') {
        pos++;
        if (!read_build_ids(s, pos, out.build))
            return false;
    }

    if (pos != n)
        return false;

    out.major = major;
    out.minor = minor;
    out.patch = patch;
    out.raw = input;
    out.format();
    return true;
}

int compare_identifiers(const Identifier& a, const Identifier& b)
{
    if (a.numeric && b.numeric)
        return compare_numeric_text(a.text, b.text);
    if (a.numeric)
        return -1;
    if (b.numeric)
        return 1;
    if (a.text == b.text)
        return 0;
    return a.text < b.text ? -1 : 1;
}

int compare_main(const SemVer& a, const SemVer& b)
{
    if (a.major != b.major)
        return a.major < b.major ? -1 : 1;
    if (a.minor != b.minor)
        return a.minor < b.minor ? -1 : 1;
    if (a.patch != b.patch)
        return a.patch < b.patch ? -1 : 1;
    return 0;
}

int compare_pre(const SemVer& a, const SemVer& b)
{
    if (!a.prerelease.empty() && b.prerelease.empty())
        return -1;
    if (a.prerelease.empty() && !b.prerelease.empty())
        return 1;
    if (a.prerelease.empty() && b.prerelease.empty())
        return 0;

    size_t i = 0;
    while (true) {
        bool ae = i >= a.prerelease.size();
        bool be = i >= b.prerelease.size();

        if (ae && be)
            return 0;
        if (be)
            return 1;
        if (ae)
            return -1;

        if (a.prerelease[i].text != b.prerelease[i].text
            || a.prerelease[i].numeric != b.prerelease[i].numeric) {
            int c = compare_identifiers(a.prerelease[i], b.prerelease[i]);
            if (c)
                return c;
        }
        i++;
    }
}

int compare_build(const SemVer& a, const SemVer& b)
{
    size_t i = 0;
    while (true) {
        bool ae = i >= a.build.size();
        bool be = i >= b.build.size();

        if (ae && be)
            return 0;
        if (be)
            return 1;
        if (ae)
            return -1;

        if (a.build[i] != b.build[i]) {
            Identifier ai = identifier_from_text(a.build[i]);
            Identifier bi = identifier_from_text(b.build[i]);
            int c = compare_identifiers(ai, bi);
            if (c)
                return c;
        }
        i++;
    }
}

int compare_versions(const SemVer& a, const SemVer& b)
{
    int c = compare_main(a, b);
    if (c)
        return c;
    return compare_pre(a, b);
}

int compare_versions_build(const SemVer& a, const SemVer& b)
{
    int c = compare_versions(a, b);
    if (c)
        return c;
    return compare_build(a, b);
}

// ---------------------- comparator :start ---------------------- //

bool Comparator::parse(const std::string& input, const Options& options)
{
    std::string comp = collapse_ws(input);

    any = true;
    op.clear();
    value.clear();
    semver = SemVer();

    if (comp.empty())
        return true;

    size_t pos = 0, n = comp.size();

    if (pos < n && (comp[pos] == '<' || comp[pos] == '>')) {
        op += comp[pos++];
        if (pos < n && comp[pos] == '=')
            op += comp[pos++];
    } else if (pos < n && comp[pos] == '=') {
        op = "=";
        pos++;
    }

    while (pos < n && is_space(comp[pos]))
        pos++;

    if (pos >= n)
        return false;

    SemVer v;
    if (!parse_version(comp.substr(pos), options, v))
        return false;

    if (op == "=")
        op.clear();

    any = false;
    semver = v;
    value = op + semver.version;
    return true;
}

bool Comparator::test(const SemVer& version) const
{
    if (any)
        return true;

    int c = compare_versions(version, semver);

    if (op.empty())
        return c == 0;
    if (op == ">")
        return c > 0;
    if (op == ">=")
        return c >= 0;
    if (op == "<")
        return c < 0;
    if (op == "<=")
        return c <= 0;

    return false;
}

// ---------------------- range parsing :start ---------------------- //

struct XRangeParts {
    std::string gtlt;
    std::string M, m, p;
    bool has_minor = false;
    bool has_patch = false;
    bool xM = false;
    bool xm = false;
    bool xp = false;
    bool has_pr = false;
    std::string pr;
    std::string plain;
};

static bool is_x_identifier(const std::string& s)
{
    return s.empty() || s == "x" || s == "X" || s == "*";
}

static bool read_x_identifier(const std::string& s, size_t& pos, bool loose, std::string& out)
{
    if (pos >= s.size())
        return false;

    char c = s[pos];
    if (c == 'x' || c == 'X' || c == '*') {
        out = std::string(1, c);
        pos++;
        return true;
    }

    if (!is_digit(c))
        return false;

    size_t start = pos;
    while (pos < s.size() && is_digit(s[pos]))
        pos++;

    if (!loose && s[start] == '0' && pos - start > 1)
        return false;

    out = s.substr(start, pos - start);
    return true;
}

static bool parse_xrange_plain(const std::string& s, bool loose, bool allow_gtlt, XRangeParts& out)
{
    size_t pos = 0, n = s.size();

    out = XRangeParts();

    if (allow_gtlt) {
        if (pos < n && (s[pos] == '<' || s[pos] == '>')) {
            out.gtlt += s[pos++];
            if (pos < n && s[pos] == '=')
                out.gtlt += s[pos++];
        } else if (pos < n && s[pos] == '=') {
            out.gtlt = "=";
            pos++;
        }
    }

    size_t plain_start = pos;

    // `[v=\s]*`
    while (pos < n && (s[pos] == 'v' || s[pos] == '=' || is_space(s[pos])))
        pos++;

    if (!read_x_identifier(s, pos, loose, out.M))
        return false;

    if (pos < n && s[pos] == '.') {
        pos++;
        if (!read_x_identifier(s, pos, loose, out.m))
            return false;
        out.has_minor = true;

        if (pos < n && s[pos] == '.') {
            pos++;
            if (!read_x_identifier(s, pos, loose, out.p))
                return false;
            out.has_patch = true;
        }
    }

    out.xM = is_x_identifier(out.M);
    out.xm = out.xM || !out.has_minor || is_x_identifier(out.m);
    out.xp = out.xm || !out.has_patch || is_x_identifier(out.p);

    if (out.has_patch) {
        size_t pr_start = pos;

        if (pos < n && s[pos] == '-') {
            pos++;
            std::vector<Identifier> ids;
            if (!read_prerelease_ids(s, pos, loose, ids))
                return false;
            out.has_pr = true;
        } else if (loose && pos < n && s[pos] != '+') {
            std::vector<Identifier> ids;
            if (!read_prerelease_ids(s, pos, loose, ids))
                return false;
            out.has_pr = true;
        }

        if (out.has_pr) {
            out.pr = s.substr(pr_start, pos - pr_start);
            if (!out.pr.empty() && out.pr[0] == '-')
                out.pr.erase(0, 1);
        }

        if (pos < n && s[pos] == '+') {
            pos++;
            std::vector<std::string> build;
            if (!read_build_ids(s, pos, build))
                return false;
        }
    }

    if (pos != n)
        return false;

    out.plain = s.substr(plain_start);
    return true;
}

static std::string expand_caret(const XRangeParts& x, const Options& options)
{
    if (x.xM)
        return "";

    std::string z = options.includePrerelease ? "-0" : "";

    if (x.xm)
        return ">=" + x.M + ".0.0" + z + " <" + next_number(x.M) + ".0.0-0";

    if (x.xp) {
        if (x.M == "0")
            return ">=" + x.M + "." + x.m + ".0" + z + " <0." + next_number(x.m) + ".0-0";
        return ">=" + x.M + "." + x.m + ".0" + z + " <" + next_number(x.M) + ".0.0-0";
    }

    if (x.has_pr) {
        if (x.M == "0") {
            if (x.m == "0")
                return ">=" + x.M + "." + x.m + "." + x.p + "-" + x.pr
                    + " <0.0." + next_number(x.p) + "-0";
            return ">=" + x.M + "." + x.m + "." + x.p + "-" + x.pr
                + " <0." + next_number(x.m) + ".0-0";
        }
        return ">=" + x.M + "." + x.m + "." + x.p + "-" + x.pr
            + " <" + next_number(x.M) + ".0.0-0";
    }

    if (x.M == "0") {
        if (x.m == "0")
            return ">=" + x.M + "." + x.m + "." + x.p + z
                + " <0.0." + next_number(x.p) + "-0";
        return ">=" + x.M + "." + x.m + "." + x.p + z
            + " <0." + next_number(x.m) + ".0-0";
    }
    return ">=" + x.M + "." + x.m + "." + x.p + " <" + next_number(x.M) + ".0.0-0";
}

static std::string expand_tilde(const XRangeParts& x, const Options& options)
{
    (void)options;

    if (x.xM)
        return "";
    if (x.xm)
        return ">=" + x.M + ".0.0 <" + next_number(x.M) + ".0.0-0";
    if (x.xp)
        return ">=" + x.M + "." + x.m + ".0 <" + x.M + "." + next_number(x.m) + ".0-0";
    if (x.has_pr)
        return ">=" + x.M + "." + x.m + "." + x.p + "-" + x.pr
            + " <" + x.M + "." + next_number(x.m) + ".0-0";
    return ">=" + x.M + "." + x.m + "." + x.p
        + " <" + x.M + "." + next_number(x.m) + ".0-0";
}

static std::string expand_xrange(const XRangeParts& x, const Options& options)
{
    bool xm = x.xm;
    bool xp = x.xp;
    std::string gtlt = x.gtlt;

    if (gtlt == "=" && xp)
        gtlt.clear();

    std::string pr = options.includePrerelease ? "-0" : "";

    if (x.xM) {
        if (gtlt == ">" || gtlt == "<")
            return "<0.0.0-0";
        return "*";
    }

    if (!gtlt.empty() && xp) {
        std::string M = x.M;
        std::string m = xm ? "0" : x.m;
        std::string p = "0";

        if (gtlt == ">") {
            gtlt = ">=";
            if (xm) {
                M = next_number(x.M);
                m = "0";
                p = "0";
            } else {
                m = next_number(x.m);
                p = "0";
            }
        } else if (gtlt == "<=") {
            gtlt = "<";
            if (xm)
                M = next_number(x.M);
            else
                m = next_number(x.m);
        }

        if (gtlt == "<")
            pr = "-0";

        return gtlt + M + "." + m + "." + p + pr;
    }

    if (xm)
        return ">=" + x.M + ".0.0" + pr + " <" + next_number(x.M) + ".0.0-0";

    if (xp)
        return ">=" + x.M + "." + x.m + ".0" + pr
            + " <" + x.M + "." + next_number(x.m) + ".0-0";

    return "";
}

static bool strip_build(std::string& comp)
{
    size_t plus = comp.find('+');
    if (plus == std::string::npos)
        return false;

    std::string build = comp.substr(plus + 1);
    if (build.empty())
        return false;

    size_t pos = 0;
    std::vector<std::string> ids;
    if (!read_build_ids(build, pos, ids))
        return false;
    if (pos != build.size())
        return false;

    comp.erase(plus);
    return true;
}

static bool is_star_token(const std::string& comp)
{
    size_t pos = 0, n = comp.size();

    if (pos < n && (comp[pos] == '<' || comp[pos] == '>'))
        pos++;
    if (pos < n && comp[pos] == '=')
        pos++;
    while (pos < n && is_space(comp[pos]))
        pos++;
    if (pos < n && comp[pos] == '*') {
        pos++;
        while (pos < n && is_space(comp[pos]))
            pos++;
        return pos == n;
    }
    return false;
}

static std::string parse_comp(std::string comp, const Options& options)
{
    if (comp.empty())
        return comp;

    strip_build(comp);

    if (comp[0] == '^') {
        XRangeParts x;
        if (parse_xrange_plain(comp.substr(1), options.loose, false, x))
            return expand_caret(x, options);
        return comp;
    }

    if (comp[0] == '~') {
        size_t off = (comp.size() > 1 && comp[1] == '>') ? 2 : 1;
        XRangeParts x;
        if (parse_xrange_plain(comp.substr(off), options.loose, false, x))
            return expand_tilde(x, options);
        return comp;
    }

    XRangeParts x;
    if (parse_xrange_plain(comp, options.loose, true, x)) {
        if (!x.xM && !x.xm && !x.xp)
            return comp;
        std::string expanded = expand_xrange(x, options);
        return expanded == "*" ? std::string() : expanded;
    }

    if (is_star_token(comp))
        return "";

    return comp;
}

static bool is_operator_token(const std::string& token)
{
    return token == ">" || token == ">=" || token == "<" || token == "<="
        || token == "=" || token == "~" || token == "~>" || token == "^";
}

static bool replace_hyphen(const std::string& range, const Options& options, std::string& out)
{
    size_t pos = range.find(" - ");
    if (pos == std::string::npos)
        return false;
    if (range.find(" - ", pos + 3) != std::string::npos)
        return false;

    std::string left = trim(range.substr(0, pos));
    std::string right = trim(range.substr(pos + 3));
    if (left.empty() || right.empty())
        return false;

    XRangeParts f, t;
    if (!parse_xrange_plain(left, options.loose, false, f))
        return false;
    if (!parse_xrange_plain(right, options.loose, false, t))
        return false;

    bool incPr = options.includePrerelease;
    std::string from, to;

    if (f.xM)
        from.clear();
    else if (f.xm)
        from = ">=" + f.M + ".0.0" + (incPr ? "-0" : "");
    else if (f.xp)
        from = ">=" + f.M + "." + f.m + ".0" + (incPr ? "-0" : "");
    else if (f.has_pr)
        from = ">=" + f.plain;
    else
        from = ">=" + f.plain + (incPr ? "-0" : "");

    if (t.xM)
        to.clear();
    else if (t.xm)
        to = "<" + next_number(t.M) + ".0.0-0";
    else if (t.xp)
        to = "<" + t.M + "." + next_number(t.m) + ".0-0";
    else if (t.has_pr)
        to = "<=" + t.M + "." + t.m + "." + t.p + "-" + t.pr;
    else if (incPr)
        to = "<" + t.M + "." + t.m + "." + next_number(t.p) + "-0";
    else
        to = "<=" + t.plain;

    out = trim(from + " " + to);
    return true;
}

bool Range::parse(const std::string& range, const Options& options)
{
    this->options = options;
    std::string normalized = collapse_ws(range);
    set.clear();

    std::vector<std::string> parts;
    size_t start = 0;
    while (true) {
        size_t p = normalized.find("||", start);
        if (p == std::string::npos) {
            parts.push_back(normalized.substr(start));
            break;
        }
        parts.push_back(normalized.substr(start, p - start));
        start = p + 2;
    }

    for (size_t i = 0; i < parts.size(); i++) {
        std::vector<Comparator> comps;
        if (!parse_range(trim(parts[i]), options, comps))
            return false;
        if (!comps.empty())
            set.push_back(comps);
    }

    if (set.empty())
        return false;

    if (set.size() > 1) {
        std::vector<std::vector<Comparator>> first = { set[0] };
        std::vector<std::vector<Comparator>> filtered;

        for (size_t i = 0; i < set.size(); i++) {
            if (!(set[i].size() == 1 && set[i][0].value == "<0.0.0-0"))
                filtered.push_back(set[i]);
        }

        if (filtered.empty())
            set = first;
        else
            set = filtered;

        if (set.size() > 1) {
            for (size_t i = 0; i < set.size(); i++) {
                if (set[i].size() == 1 && set[i][0].value.empty()) {
                    std::vector<std::vector<Comparator>> one = { set[i] };
                    set = one;
                    break;
                }
            }
        }
    }

    return true;
}

// Parses one `||` alternative into its comparator set.
bool Range::parse_range(const std::string& input, const Options& options,
    std::vector<Comparator>& out)
{
    out.clear();

    std::string r = trim(input);

    std::string replaced;
    if (replace_hyphen(r, options, replaced))
        r = replaced;

    std::vector<std::string> tokens = split_ws(r);
    std::vector<std::string> merged;

    for (size_t i = 0; i < tokens.size();) {
        if (is_operator_token(tokens[i]) && i + 1 < tokens.size()) {
            merged.push_back(tokens[i] + tokens[i + 1]);
            i += 2;
        } else {
            merged.push_back(tokens[i]);
            i++;
        }
    }

    std::vector<std::string> comps;

    for (size_t i = 0; i < merged.size(); i++) {
        std::string expanded = parse_comp(merged[i], options);

        if (expanded.empty()) {
            comps.push_back("");
            continue;
        }

        std::vector<std::string> sub = split_ws(expanded);
        for (size_t k = 0; k < sub.size(); k++) {
            if (sub[k].empty())
                continue;

            if ((options.includePrerelease && sub[k] == ">=0.0.0-0")
                || (!options.includePrerelease && sub[k] == ">=0.0.0"))
                continue;

            comps.push_back(sub[k]);
        }
    }

    std::vector<Comparator> result;
    std::vector<std::string> seen;
    bool invalidSeen = false;

    for (size_t i = 0; i < comps.size(); i++) {
        Comparator c;
        if (!c.parse(comps[i], options)) {
            if (options.loose) {
                invalidSeen = true;
                continue;
            }
            return false;
        }

        if (c.value == "<0.0.0-0") {
            out.clear();
            out.push_back(c);
            return true;
        }

        bool dup = false;
        for (size_t k = 0; k < seen.size(); k++)
            if (seen[k] == c.value) {
                dup = true;
                break;
            }
        if (dup)
            continue;

        seen.push_back(c.value);
        result.push_back(c);
    }

    if (result.empty()) {
        // A loose alternative whose comparators were all invalid contributes
        // nothing; the Range constructor drops the empty set. An empty input
        // (or one that only held `>=0.0.0`) is still the ANY comparator.
        if (invalidSeen) {
            out.clear();
            return true;
        }

        Comparator c;
        if (!c.parse("", options))
            return false;
        result.push_back(c);
    }

    if (result.size() > 1) {
        std::vector<Comparator> filtered;
        for (size_t i = 0; i < result.size(); i++)
            if (!result[i].value.empty())
                filtered.push_back(result[i]);
        if (!filtered.empty())
            result = filtered;
    }

    out = result;
    return true;
}

std::string Range::to_string() const
{
    std::string out;

    for (size_t i = 0; i < set.size(); i++) {
        if (i)
            out += "||";
        for (size_t k = 0; k < set[i].size(); k++) {
            if (k)
                out += ' ';
            out += trim(set[i][k].value);
        }
    }
    return out;
}

static bool test_set(const std::vector<Comparator>& set, const SemVer& version,
    const Options& options)
{
    for (size_t i = 0; i < set.size(); i++)
        if (!set[i].test(version))
            return false;

    if (!version.prerelease.empty() && !options.includePrerelease) {
        for (size_t i = 0; i < set.size(); i++) {
            if (set[i].any)
                continue;
            if (!set[i].semver.prerelease.empty()
                && set[i].semver.major == version.major
                && set[i].semver.minor == version.minor
                && set[i].semver.patch == version.patch)
                return true;
        }
        return false;
    }

    return true;
}

bool Range::test(const SemVer& version) const
{
    for (size_t i = 0; i < set.size(); i++)
        if (test_set(set[i], version, options))
            return true;
    return false;
}

bool Range::test(const std::string& version) const
{
    SemVer v;
    if (!parse_version(version, options, v))
        return false;
    return test(v);
}

static bool comparator_intersects(const Comparator& a, const Comparator& b,
    const Options& options)
{
    if (a.op.empty()) {
        if (a.value.empty())
            return true;
        Range r;
        if (!r.parse(b.value, options))
            return false;
        return r.test(a.semver);
    }

    if (b.op.empty()) {
        if (b.value.empty())
            return true;
        Range r;
        if (!r.parse(a.value, options))
            return false;
        return r.test(b.semver);
    }

    if (options.includePrerelease
        && (a.value == "<0.0.0-0" || b.value == "<0.0.0-0"))
        return false;

    if (!options.includePrerelease
        && (starts_with(a.value, "<0.0.0") || starts_with(b.value, "<0.0.0")))
        return false;

    bool aGt = !a.op.empty() && a.op[0] == '>';
    bool bGt = !b.op.empty() && b.op[0] == '>';
    bool aLt = !a.op.empty() && a.op[0] == '<';
    bool bLt = !b.op.empty() && b.op[0] == '<';

    if (aGt && bGt)
        return true;
    if (aLt && bLt)
        return true;

    if (a.semver.version == b.semver.version
        && a.op.find('=') != std::string::npos
        && b.op.find('=') != std::string::npos)
        return true;

    int c = compare_versions(a.semver, b.semver);

    if (c < 0 && aGt && bLt)
        return true;
    if (c > 0 && aLt && bGt)
        return true;

    return false;
}

static bool is_satisfiable(const std::vector<Comparator>& comps, const Options& options)
{
    if (comps.empty())
        return true;

    std::vector<Comparator> remaining = comps;
    Comparator test = remaining.back();
    remaining.pop_back();

    bool result = true;
    while (result && !remaining.empty()) {
        for (size_t i = 0; i < remaining.size(); i++)
            if (!comparator_intersects(test, remaining[i], options)) {
                result = false;
                break;
            }
        test = remaining.back();
        remaining.pop_back();
    }
    return result;
}

bool Range::intersects(const Range& other, const Options& options) const
{
    for (size_t i = 0; i < set.size(); i++) {
        if (!is_satisfiable(set[i], options))
            continue;

        for (size_t k = 0; k < other.set.size(); k++) {
            if (!is_satisfiable(other.set[k], options))
                continue;

            bool all = true;
            for (size_t a = 0; a < set[i].size() && all; a++)
                for (size_t b = 0; b < other.set[k].size(); b++)
                    if (!comparator_intersects(set[i][a], other.set[k][b], options)) {
                        all = false;
                        break;
                    }

            if (all)
                return true;
        }
    }
    return false;
}

// ---------------------- top-level helpers :start ---------------------- //

bool max_satisfying(const std::vector<std::string>& versions, const std::string& range,
    const Options& options, size_t& index)
{
    Range r;
    if (!r.parse(range, options))
        return false;

    bool found = false;
    SemVer best;

    for (size_t i = 0; i < versions.size(); i++) {
        SemVer v;
        if (!parse_version(versions[i], options, v))
            continue;
        if (!r.test(v))
            continue;

        if (!found || compare_versions(best, v) < 0) {
            best = v;
            index = i;
            found = true;
        }
    }

    return found;
}

bool min_satisfying(const std::vector<std::string>& versions, const std::string& range,
    const Options& options, size_t& index)
{
    Range r;
    if (!r.parse(range, options))
        return false;

    bool found = false;
    SemVer best;

    for (size_t i = 0; i < versions.size(); i++) {
        SemVer v;
        if (!parse_version(versions[i], options, v))
            continue;
        if (!r.test(v))
            continue;

        if (!found || compare_versions(best, v) > 0) {
            best = v;
            index = i;
            found = true;
        }
    }

    return found;
}

bool min_version(const std::string& range, const Options& options, SemVer& out, bool& found)
{
    Range r;
    if (!r.parse(range, options))
        return false;

    found = true;

    SemVer zero;
    parse_version("0.0.0", options, zero);
    if (r.test(zero)) {
        out = zero;
        return true;
    }

    SemVer zeroPre;
    parse_version("0.0.0-0", options, zeroPre);
    if (r.test(zeroPre)) {
        out = zeroPre;
        return true;
    }

    bool have = false;
    SemVer minver;

    for (size_t i = 0; i < r.set.size(); i++) {
        bool haveSet = false;
        SemVer setMin;

        for (size_t k = 0; k < r.set[i].size(); k++) {
            const Comparator& comp = r.set[i][k];
            if (comp.any)
                continue;

            SemVer compver = comp.semver;
            if (comp.op == ">") {
                if (compver.prerelease.empty()) {
                    compver.patch++;
                } else {
                    Identifier zeroId;
                    zeroId.numeric = true;
                    zeroId.is_number = true;
                    zeroId.number = 0;
                    zeroId.text = "0";
                    compver.prerelease.push_back(zeroId);
                }
                compver.format();
            }

            if (comp.op.empty() || comp.op == ">" || comp.op == ">=") {
                if (!haveSet || compare_versions(compver, setMin) > 0) {
                    setMin = compver;
                    haveSet = true;
                }
            }
        }

        if (haveSet && (!have || compare_versions(minver, setMin) > 0)) {
            minver = setMin;
            have = true;
        }
    }

    if (have && r.test(minver)) {
        out = minver;
        return true;
    }

    found = false;
    return true;
}

bool outside(const std::string& version, const std::string& range, const char* hilo,
    const Options& options, bool& result)
{
    SemVer v;
    if (!parse_version(version, options, v))
        return false;

    Range r;
    if (!r.parse(range, options))
        return false;

    bool above;
    if (hilo && hilo[0] == '>' && hilo[1] == 0)
        above = true;
    else if (hilo && hilo[0] == '<' && hilo[1] == 0)
        above = false;
    else
        return false;

    if (r.test(v)) {
        result = false;
        return true;
    }

    bool all = true;

    for (size_t i = 0; i < r.set.size(); i++) {
        std::vector<Comparator> comps = r.set[i];

        for (size_t k = 0; k < comps.size(); k++)
            if (comps[k].any) {
                Options opt;
                comps[k].parse(">=0.0.0", opt);
            }

        if (comps.empty())
            continue;

        const Comparator* high = &comps[0];
        const Comparator* low = &comps[0];

        for (size_t k = 1; k < comps.size(); k++) {
            int ch = compare_versions(comps[k].semver, high->semver);
            int cl = compare_versions(comps[k].semver, low->semver);

            bool greater = above ? ch > 0 : ch < 0;
            bool less = above ? cl < 0 : cl > 0;

            if (greater)
                high = &comps[k];
            else if (less)
                low = &comps[k];
        }

        const char* comp = above ? ">" : "<";
        const char* ecomp = above ? ">=" : "<=";

        if (high->op == comp || high->op == ecomp) {
            all = false;
            break;
        }

        bool lowGreater = !low->op.empty() && low->op == ecomp
            && (above ? compare_versions(v, low->semver) < 0
                      : compare_versions(v, low->semver) > 0);
        bool lowBound = (low->op.empty() || low->op == comp)
            && (above ? compare_versions(v, low->semver) <= 0
                      : compare_versions(v, low->semver) >= 0);

        if (lowBound || lowGreater) {
            all = false;
            break;
        }
    }

    result = all;
    return true;
}

static bool valid_prerelease_string(const std::string& identifier, bool loose)
{
    size_t pos = 0;
    std::string s = identifier;

    // `-${identifier}` must match PRERELEASE / PRERELEASELOOSE.
    if (!s.empty() && s[0] == '-')
        pos = 1;

    std::vector<Identifier> ids;
    if (!read_prerelease_ids(s, pos, loose, ids))
        return false;
    return pos == s.size();
}

bool increment(std::string version, const std::string& release, const std::string& identifier,
    int identifier_base, bool identifier_base_false, const Options& options,
    std::string& out_without_build)
{
    SemVer v;
    if (!parse_version(version, options, v))
        return false;

    if (starts_with(release, "pre")) {
        if (identifier.empty() && identifier_base_false)
            return false;
        if (!identifier.empty() && !valid_prerelease_string("-" + identifier, options.loose))
            return false;
    }

    int base = identifier_base != 0 ? 1 : 0;

    auto inc_pre = [&]() -> bool {
        if (v.prerelease.empty()) {
            Identifier id;
            id.numeric = true;
            id.is_number = true;
            id.number = (uint64_t)base;
            id.text = std::to_string(base);
            v.prerelease.push_back(id);
        } else {
            int i = (int)v.prerelease.size();
            bool incremented = false;

            while (--i >= 0) {
                if (v.prerelease[i].is_number) {
                    v.prerelease[i].number++;
                    v.prerelease[i].text = std::to_string(v.prerelease[i].number);
                    incremented = true;
                    break;
                }
            }

            if (!incremented) {
                std::string joined;
                for (size_t k = 0; k < v.prerelease.size(); k++) {
                    if (k)
                        joined += '.';
                    joined += v.prerelease[k].text;
                }

                if (!identifier.empty() && identifier == joined && identifier_base_false)
                    return false;

                Identifier id;
                id.numeric = true;
                id.is_number = true;
                id.number = (uint64_t)base;
                id.text = std::to_string(base);
                v.prerelease.push_back(id);
            }
        }

        if (!identifier.empty()) {
            std::vector<Identifier> pre;
            Identifier id = identifier_from_text(identifier);
            pre.push_back(id);
            if (!identifier_base_false) {
                Identifier b;
                b.numeric = true;
                b.is_number = true;
                b.number = (uint64_t)base;
                b.text = std::to_string(base);
                pre.push_back(b);
            }

            bool same = !v.prerelease.empty()
                && compare_identifiers(v.prerelease[0], id) == 0;
            bool secondNaN = v.prerelease.size() < 2 || !v.prerelease[1].numeric;

            if (same) {
                if (secondNaN)
                    v.prerelease = pre;
            } else {
                v.prerelease = pre;
            }
        }

        return true;
    };

    if (release == "premajor") {
        v.prerelease.clear();
        v.patch = 0;
        v.minor = 0;
        v.major++;
        if (!inc_pre())
            return false;
    } else if (release == "preminor") {
        v.prerelease.clear();
        v.patch = 0;
        v.minor++;
        if (!inc_pre())
            return false;
    } else if (release == "prepatch") {
        v.prerelease.clear();
        v.patch++;
        if (!inc_pre())
            return false;
    } else if (release == "prerelease") {
        if (v.prerelease.empty())
            v.patch++;
        if (!inc_pre())
            return false;
    } else if (release == "release") {
        if (v.prerelease.empty())
            return false;
        v.prerelease.clear();
    } else if (release == "major") {
        if (v.minor != 0 || v.patch != 0 || v.prerelease.empty())
            v.major++;
        v.minor = 0;
        v.patch = 0;
        v.prerelease.clear();
    } else if (release == "minor") {
        if (v.patch != 0 || v.prerelease.empty())
            v.minor++;
        v.patch = 0;
        v.prerelease.clear();
    } else if (release == "patch") {
        if (v.prerelease.empty())
            v.patch++;
        v.prerelease.clear();
    } else if (release == "pre") {
        if (!inc_pre())
            return false;
    } else {
        return false;
    }

    v.format();
    out_without_build = v.version;
    return true;
}

bool diff_versions(const std::string& v1, const std::string& v2, std::string& out)
{
    Options options;
    SemVer a, b;

    if (!parse_version(v1, options, a) || !parse_version(v2, options, b))
        return false;

    int c = compare_versions(a, b);
    if (c == 0) {
        out.clear();
        return true;
    }

    const SemVer& high = c > 0 ? a : b;
    const SemVer& low = c > 0 ? b : a;
    bool highHasPre = !high.prerelease.empty();
    bool lowHasPre = !low.prerelease.empty();

    if (lowHasPre && !highHasPre) {
        if (!low.patch && !low.minor) {
            out = "major";
            return true;
        }
        if (compare_main(low, high) == 0) {
            if (low.minor && !low.patch) {
                out = "minor";
                return true;
            }
            out = "patch";
            return true;
        }
    }

    std::string prefix = highHasPre ? "pre" : "";

    if (a.major != b.major) {
        out = prefix + "major";
        return true;
    }
    if (a.minor != b.minor) {
        out = prefix + "minor";
        return true;
    }
    if (a.patch != b.patch) {
        out = prefix + "patch";
        return true;
    }

    out = "prerelease";
    return true;
}

// ---------------------- coerce :start ---------------------- //

struct CoerceMatch {
    size_t numeric_start = 0;
    size_t end = 0;
    size_t major_len = 0;
    std::string major, minor, patch, prerelease, build;
};

static bool coerce_match_at(const std::string& s, size_t p, bool includePrerelease,
    CoerceMatch& m)
{
    size_t n = s.size();

    if (p >= n || !is_digit(s[p]))
        return false;
    if (p > 0 && is_digit(s[p - 1]))
        return false;

    size_t start = p;

    size_t major_end = p;
    while (major_end < n && is_digit(s[major_end]) && major_end - p < 16)
        major_end++;
    if (major_end - p == 0 || (major_end < n && is_digit(s[major_end])))
        return false;

    m.major = s.substr(p, major_end - p);
    m.major_len = major_end - p;
    p = major_end;

    auto read_component = [&](std::string& out, bool& ok) {
        ok = false;
        if (p >= n || s[p] != '.')
            return;
        size_t q = p + 1;
        size_t e = q;
        while (e < n && is_digit(s[e]) && e - q < 16)
            e++;
        if (e == q)
            return;
        if (e < n && is_digit(s[e]))
            return;
        out = s.substr(q, e - q);
        p = e;
        ok = true;
    };

    bool ok = false;
    read_component(m.minor, ok);
    if (ok)
        read_component(m.patch, ok);

    if (includePrerelease) {
        size_t save = p;
        if (p < n && s[p] == '-') {
            size_t q = p + 1;
            std::vector<Identifier> ids;
            if (read_prerelease_ids(s, q, false, ids)) {
                m.prerelease = s.substr(p + 1, q - p - 1);
                p = q;
            } else {
                p = save;
            }
        }
        if (p < n && s[p] == '+') {
            size_t q = p + 1;
            std::vector<std::string> bids;
            if (read_build_ids(s, q, bids)) {
                m.build = s.substr(p + 1, q - p - 1);
                p = q;
            }
        }
    }

    if (p < n && is_digit(s[p]))
        return false;

    m.numeric_start = start;
    m.end = p < n ? p + 1 : p;
    return true;
}

static bool find_next_coerce(const std::string& s, size_t from, bool includePrerelease,
    CoerceMatch& out, bool& found)
{
    found = false;

    for (size_t p = from; p < s.size(); p++) {
        if (!is_digit(s[p]))
            continue;
        if (p > 0 && is_digit(s[p - 1]))
            continue;

        CoerceMatch m;
        if (coerce_match_at(s, p, includePrerelease, m)) {
            out = m;
            found = true;
            return true;
        }
    }
    return true;
}

bool coerce_version(const std::string& version, const Options& options, SemVer& out)
{
    CoerceMatch match;
    bool have = false;

    if (!options.rtl) {
        bool found = false;
        find_next_coerce(version, 0, options.includePrerelease, match, found);
        have = found;
    } else {
        size_t lastIndex = 0;

        while (true) {
            CoerceMatch next;
            bool found = false;
            find_next_coerce(version, lastIndex, options.includePrerelease, next, found);
            if (!found)
                break;

            if (have && match.end == version.size())
                break;

            if (!have || next.end != match.end) {
                match = next;
                have = true;
            }

            lastIndex = next.numeric_start + next.major_len;
        }
    }

    if (!have)
        return false;

    std::string v = match.major + "." + (match.minor.empty() ? "0" : match.minor)
        + "." + (match.patch.empty() ? "0" : match.patch);

    if (!match.prerelease.empty())
        v += "-" + match.prerelease;
    if (!match.build.empty())
        v += "+" + match.build;

    return parse_version(v, options, out);
}

} // namespace semver_detail

// ---------------------- JS glue :start ---------------------- //

namespace {

using namespace semver_detail;

static Options to_options(SemVerOptions* options)
{
    Options o;
    if (options) {
        o.loose = options->loose.value_or(false);
        o.includePrerelease = options->includePrerelease.value_or(false);
        o.rtl = options->rtl.value_or(false);
    }
    return o;
}

static result_t invalid_version(const exlib::string& version)
{
    std::string message = "Invalid Version: " + std::string(version);
    return result_t::make(CALL_E_INVALID_DATA)
        .with_type_name(errtype::kTypeError)
        .with_message(exlib::string(message.c_str(), message.size()));
}

static result_t invalid_range(const exlib::string& range)
{
    std::string message = "Invalid SemVer Range: " + std::string(range);
    return result_t::make(CALL_E_INVALID_DATA)
        .with_type_name(errtype::kTypeError)
        .with_message(exlib::string(message.c_str(), message.size()));
}

static result_t invalid_operator(const exlib::string& op)
{
    std::string message = "Invalid operator: " + std::string(op);
    return result_t::make(CALL_E_INVALID_DATA)
        .with_type_name(errtype::kTypeError)
        .with_message(exlib::string(message.c_str(), message.size()));
}

static v8::Local<v8::Array> make_prerelease_array(Isolate* isolate,
    const std::vector<Identifier>& ids)
{
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Array> arr = v8::Array::New(isolate->m_isolate, (int32_t)ids.size());

    for (size_t i = 0; i < ids.size(); i++) {
        v8::Local<v8::Value> v;
        if (ids[i].is_number)
            v = v8::Number::New(isolate->m_isolate, (double)ids[i].number);
        else
            v = isolate->NewString(ids[i].text.c_str(), (int)ids[i].text.size());
        arr->Set(context, (uint32_t)i, v).Check();
    }
    return arr;
}

template <typename T>
static void fill_version_type(Isolate* isolate, const SemVer& v, T* retVal)
{
    retVal->version = exlib::string(v.version.c_str(), v.version.size());
    retVal->raw = exlib::string(v.raw.c_str(), v.raw.size());
    retVal->major = (int64_t)v.major;
    retVal->minor = (int64_t)v.minor;
    retVal->patch = (int64_t)v.patch;
    retVal->prerelease = make_prerelease_array(isolate, v.prerelease);

    retVal->build.clear();
    retVal->build.reserve(v.build.size());
    for (const std::string& id : v.build)
        retVal->build.emplace_back(id.c_str(), id.size());
}

static result_t parse_pair(const exlib::string& v1, const exlib::string& v2,
    const Options& options, SemVer& a, SemVer& b)
{
    if (!parse_version(std::string(v1.c_str(), v1.size()), options, a))
        return invalid_version(v1);
    if (!parse_version(std::string(v2.c_str(), v2.size()), options, b))
        return invalid_version(v2);
    return result_t();
}

static std::string number_to_string(Isolate* isolate, double value)
{
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::String> str = v8::Number::New(isolate->m_isolate, value)
                                    ->ToString(context)
                                    .ToLocalChecked();
    v8::String::Utf8Value utf8(isolate->m_isolate, str);
    return std::string(*utf8, utf8.length());
}

} // namespace

result_t semver_base::get_SEMVER_SPEC_VERSION(exlib::string& retVal)
{
    retVal = "2.0.0";
    return result_t();
}

result_t semver_base::parse(exlib::string version, SemVerOptions* options,
    obj_ptr<ParseType>& retVal)
{
    Options o = to_options(options);
    SemVer v;

    if (!parse_version(std::string(version.c_str(), version.size()), o, v))
        return result_t::return_null();

    Isolate* isolate = Isolate::current();
    obj_ptr<ParseType> result = new ParseType();
    fill_version_type(isolate, v, result.get());
    retVal = result;
    return result_t();
}

result_t semver_base::valid(exlib::string version, SemVerOptions* options,
    exlib::string& retVal)
{
    SemVer v;
    if (!parse_version(std::string(version.c_str(), version.size()), to_options(options), v))
        return result_t::return_null();

    retVal = exlib::string(v.version.c_str(), v.version.size());
    return result_t();
}

result_t semver_base::clean(exlib::string version, SemVerOptions* options,
    exlib::string& retVal)
{
    std::string s(version.c_str(), version.size());
    std::string cleaned = trim(s);

    size_t pos = 0;
    while (pos < cleaned.size() && (cleaned[pos] == '=' || cleaned[pos] == 'v'))
        pos++;
    cleaned = cleaned.substr(pos);

    SemVer v;
    if (!parse_version(cleaned, to_options(options), v))
        return result_t::return_null();

    retVal = exlib::string(v.version.c_str(), v.version.size());
    return result_t();
}

result_t semver_base::compare(exlib::string version1, exlib::string version2,
    SemVerOptions* options, int32_t& retVal)
{
    Options o = to_options(options);
    SemVer a, b;
    result_t hr = parse_pair(version1, version2, o, a, b);
    if (hr.failed())
        return hr;
    retVal = compare_versions(a, b);
    return result_t();
}

result_t semver_base::compareLoose(exlib::string version1, exlib::string version2,
    int32_t& retVal)
{
    Options o;
    o.loose = true;
    SemVer a, b;
    result_t hr = parse_pair(version1, version2, o, a, b);
    if (hr.failed())
        return hr;
    retVal = compare_versions(a, b);
    return result_t();
}

result_t semver_base::compareBuild(exlib::string version1, exlib::string version2,
    SemVerOptions* options, int32_t& retVal)
{
    Options o = to_options(options);
    SemVer a, b;
    result_t hr = parse_pair(version1, version2, o, a, b);
    if (hr.failed())
        return hr;
    retVal = compare_versions_build(a, b);
    return result_t();
}

result_t semver_base::rcompare(exlib::string version1, exlib::string version2,
    SemVerOptions* options, int32_t& retVal)
{
    Options o = to_options(options);
    SemVer a, b;
    result_t hr = parse_pair(version1, version2, o, a, b);
    if (hr.failed())
        return hr;
    retVal = compare_versions(b, a);
    return result_t();
}

result_t semver_base::sort(std::vector<exlib::string>& list, SemVerOptions* options,
    std::vector<exlib::string>& retVal)
{
    Options o = to_options(options);
    std::vector<std::pair<SemVer, exlib::string>> items;

    for (size_t i = 0; i < list.size(); i++) {
        SemVer v;
        std::string s(list[i].c_str(), list[i].size());
        if (!parse_version(s, o, v))
            return invalid_version(list[i]);
        items.push_back(std::make_pair(v, list[i]));
    }

    std::stable_sort(items.begin(), items.end(),
        [](const std::pair<SemVer, exlib::string>& a,
            const std::pair<SemVer, exlib::string>& b) {
            return compare_versions_build(a.first, b.first) < 0;
        });

    retVal.clear();
    for (size_t i = 0; i < items.size(); i++)
        retVal.push_back(items[i].second);
    return result_t();
}

result_t semver_base::rsort(std::vector<exlib::string>& list, SemVerOptions* options,
    std::vector<exlib::string>& retVal)
{
    Options o = to_options(options);
    std::vector<std::pair<SemVer, exlib::string>> items;

    for (size_t i = 0; i < list.size(); i++) {
        SemVer v;
        std::string s(list[i].c_str(), list[i].size());
        if (!parse_version(s, o, v))
            return invalid_version(list[i]);
        items.push_back(std::make_pair(v, list[i]));
    }

    std::stable_sort(items.begin(), items.end(),
        [](const std::pair<SemVer, exlib::string>& a,
            const std::pair<SemVer, exlib::string>& b) {
            return compare_versions_build(a.first, b.first) > 0;
        });

    retVal.clear();
    for (size_t i = 0; i < items.size(); i++)
        retVal.push_back(items[i].second);
    return result_t();
}

result_t semver_base::gt(exlib::string version1, exlib::string version2,
    SemVerOptions* options, bool& retVal)
{
    int32_t c;
    result_t hr = compare(version1, version2, options, c);
    if (hr.failed())
        return hr;
    retVal = c > 0;
    return result_t();
}

result_t semver_base::gte(exlib::string version1, exlib::string version2,
    SemVerOptions* options, bool& retVal)
{
    int32_t c;
    result_t hr = compare(version1, version2, options, c);
    if (hr.failed())
        return hr;
    retVal = c >= 0;
    return result_t();
}

result_t semver_base::lt(exlib::string version1, exlib::string version2,
    SemVerOptions* options, bool& retVal)
{
    int32_t c;
    result_t hr = compare(version1, version2, options, c);
    if (hr.failed())
        return hr;
    retVal = c < 0;
    return result_t();
}

result_t semver_base::lte(exlib::string version1, exlib::string version2,
    SemVerOptions* options, bool& retVal)
{
    int32_t c;
    result_t hr = compare(version1, version2, options, c);
    if (hr.failed())
        return hr;
    retVal = c <= 0;
    return result_t();
}

result_t semver_base::eq(exlib::string version1, exlib::string version2,
    SemVerOptions* options, bool& retVal)
{
    int32_t c;
    result_t hr = compare(version1, version2, options, c);
    if (hr.failed())
        return hr;
    retVal = c == 0;
    return result_t();
}

result_t semver_base::neq(exlib::string version1, exlib::string version2,
    SemVerOptions* options, bool& retVal)
{
    int32_t c;
    result_t hr = compare(version1, version2, options, c);
    if (hr.failed())
        return hr;
    retVal = c != 0;
    return result_t();
}

result_t semver_base::cmp(exlib::string version1, exlib::string op,
    exlib::string version2, SemVerOptions* options, bool& retVal)
{
    std::string oper(op.c_str(), op.size());
    Options o = to_options(options);

    if (oper == "===" || oper == "!==") {
        bool same = version1 == version2;
        retVal = oper == "===" ? same : !same;
        return result_t();
    }

    SemVer a, b;
    result_t hr = parse_pair(version1, version2, o, a, b);
    if (hr.failed())
        return hr;

    int c = compare_versions(a, b);

    if (oper.empty() || oper == "=" || oper == "==")
        retVal = c == 0;
    else if (oper == "!=")
        retVal = c != 0;
    else if (oper == ">")
        retVal = c > 0;
    else if (oper == ">=")
        retVal = c >= 0;
    else if (oper == "<")
        retVal = c < 0;
    else if (oper == "<=")
        retVal = c <= 0;
    else
        return invalid_operator(op);

    return result_t();
}

result_t semver_base::validRange(exlib::string range, SemVerOptions* options,
    exlib::string& retVal)
{
    Range r;
    if (!r.parse(std::string(range.c_str(), range.size()), to_options(options)))
        return result_t::return_null();

    std::string formatted = r.to_string();
    if (formatted.empty())
        formatted = "*";
    retVal = exlib::string(formatted.c_str(), formatted.size());
    return result_t();
}

result_t semver_base::satisfies(exlib::string version, exlib::string range,
    SemVerOptions* options, bool& retVal)
{
    Options o = to_options(options);
    Range r;
    if (!r.parse(std::string(range.c_str(), range.size()), o)) {
        retVal = false;
        return result_t();
    }

    retVal = r.test(std::string(version.c_str(), version.size()));
    return result_t();
}

result_t semver_base::maxSatisfying(std::vector<exlib::string>& versions,
    exlib::string range, SemVerOptions* options, exlib::string& retVal)
{
    std::vector<std::string> values;
    for (size_t i = 0; i < versions.size(); i++)
        values.push_back(std::string(versions[i].c_str(), versions[i].size()));

    size_t index = 0;
    if (!max_satisfying(values, std::string(range.c_str(), range.size()),
            to_options(options), index))
        return result_t::return_null();

    retVal = versions[index];
    return result_t();
}

result_t semver_base::minSatisfying(std::vector<exlib::string>& versions,
    exlib::string range, SemVerOptions* options, exlib::string& retVal)
{
    std::vector<std::string> values;
    for (size_t i = 0; i < versions.size(); i++)
        values.push_back(std::string(versions[i].c_str(), versions[i].size()));

    size_t index = 0;
    if (!min_satisfying(values, std::string(range.c_str(), range.size()),
            to_options(options), index))
        return result_t::return_null();

    retVal = versions[index];
    return result_t();
}

result_t semver_base::minVersion(exlib::string range, SemVerOptions* options,
    obj_ptr<MinVersionType>& retVal)
{
    SemVer v;
    bool found = false;

    if (!min_version(std::string(range.c_str(), range.size()), to_options(options), v,
            found))
        return invalid_range(range);

    if (!found)
        return result_t::return_null();

    Isolate* isolate = Isolate::current();
    obj_ptr<MinVersionType> result = new MinVersionType();
    fill_version_type(isolate, v, result.get());
    retVal = result;
    return result_t();
}

result_t semver_base::gtr(exlib::string version, exlib::string range,
    SemVerOptions* options, bool& retVal)
{
    Options o = to_options(options);
    SemVer v;
    if (!parse_version(std::string(version.c_str(), version.size()), o, v))
        return invalid_version(version);

    Range r;
    if (!r.parse(std::string(range.c_str(), range.size()), o))
        return invalid_range(range);

    bool result = false;
    if (!semver_detail::outside(std::string(version.c_str(), version.size()),
            std::string(range.c_str(), range.size()), ">", o, result))
        return invalid_range(range);

    retVal = result;
    return result_t();
}

result_t semver_base::ltr(exlib::string version, exlib::string range,
    SemVerOptions* options, bool& retVal)
{
    Options o = to_options(options);
    SemVer v;
    if (!parse_version(std::string(version.c_str(), version.size()), o, v))
        return invalid_version(version);

    Range r;
    if (!r.parse(std::string(range.c_str(), range.size()), o))
        return invalid_range(range);

    bool result = false;
    if (!semver_detail::outside(std::string(version.c_str(), version.size()),
            std::string(range.c_str(), range.size()), "<", o, result))
        return invalid_range(range);

    retVal = result;
    return result_t();
}

result_t semver_base::outside(exlib::string version, exlib::string range,
    exlib::string hilo, SemVerOptions* options, bool& retVal)
{
    std::string direction(hilo.c_str(), hilo.size());
    if (direction != ">" && direction != "<")
        return result_t::make(CALL_E_INVALID_DATA)
            .with_type_name(errtype::kTypeError)
            .with_message("Must provide a hilo val of \"<\" or \">\"");

    Options o = to_options(options);
    SemVer v;
    if (!parse_version(std::string(version.c_str(), version.size()), o, v))
        return invalid_version(version);

    Range r;
    if (!r.parse(std::string(range.c_str(), range.size()), o))
        return invalid_range(range);

    bool result = false;
    if (!semver_detail::outside(std::string(version.c_str(), version.size()),
            std::string(range.c_str(), range.size()), direction.c_str(), o, result))
        return invalid_range(range);

    retVal = result;
    return result_t();
}

result_t semver_base::intersects(exlib::string range1, exlib::string range2,
    SemVerOptions* options, bool& retVal)
{
    Options o = to_options(options);
    Range a, b;

    if (!a.parse(std::string(range1.c_str(), range1.size()), o))
        return invalid_range(range1);
    if (!b.parse(std::string(range2.c_str(), range2.size()), o))
        return invalid_range(range2);

    retVal = a.intersects(b, o);
    return result_t();
}

result_t semver_base::prerelease(exlib::string version, SemVerOptions* options,
    v8::Local<v8::Array>& retVal)
{
    SemVer v;
    if (!parse_version(std::string(version.c_str(), version.size()), to_options(options), v)
        || v.prerelease.empty())
        return result_t::return_null();

    Isolate* isolate = Isolate::current();
    retVal = make_prerelease_array(isolate, v.prerelease);
    return result_t();
}

result_t semver_base::major(exlib::string version, SemVerOptions* options,
    int64_t& retVal)
{
    SemVer v;
    if (!parse_version(std::string(version.c_str(), version.size()), to_options(options), v))
        return invalid_version(version);

    retVal = (int64_t)v.major;
    return result_t();
}

result_t semver_base::minor(exlib::string version, SemVerOptions* options,
    int64_t& retVal)
{
    SemVer v;
    if (!parse_version(std::string(version.c_str(), version.size()), to_options(options), v))
        return invalid_version(version);

    retVal = (int64_t)v.minor;
    return result_t();
}

result_t semver_base::patch(exlib::string version, SemVerOptions* options,
    int64_t& retVal)
{
    SemVer v;
    if (!parse_version(std::string(version.c_str(), version.size()), to_options(options), v))
        return invalid_version(version);

    retVal = (int64_t)v.patch;
    return result_t();
}

result_t semver_base::inc(exlib::string version, exlib::string release,
    SemVerOptions* options, exlib::string identifier,
    Union_inc_identifierBase identifierBase, exlib::string& retVal)
{
    int base = 0;
    bool baseFalse = false;

    if (std::holds_alternative<int32_t>(identifierBase)) {
        base = std::get<int32_t>(identifierBase);
    } else {
        bool b = std::get<bool>(identifierBase);
        base = b ? 1 : 0;
        baseFalse = !b;
    }

    std::string out;
    if (!increment(std::string(version.c_str(), version.size()),
            std::string(release.c_str(), release.size()),
            std::string(identifier.c_str(), identifier.size()), base, baseFalse,
            to_options(options), out))
        return result_t::return_null();

    retVal = exlib::string(out.c_str(), out.size());
    return result_t();
}

result_t semver_base::diff(exlib::string version1, exlib::string version2,
    exlib::string& retVal)
{
    std::string out;
    if (!diff_versions(std::string(version1.c_str(), version1.size()),
            std::string(version2.c_str(), version2.size()), out))
        return invalid_version(version1);

    if (out.empty())
        return result_t::return_null();

    retVal = exlib::string(out.c_str(), out.size());
    return result_t();
}

result_t semver_base::coerce(Union_coerce_version version, SemVerOptions* options,
    obj_ptr<CoerceType>& retVal)
{
    std::string input;
    if (std::holds_alternative<exlib::string>(version)) {
        exlib::string& s = std::get<exlib::string>(version);
        input.assign(s.c_str(), s.size());
    } else {
        input = number_to_string(Isolate::current(), std::get<double>(version));
    }

    Options o = to_options(options);
    SemVer v;
    if (!coerce_version(input, o, v))
        return result_t::return_null();

    Isolate* isolate = Isolate::current();
    obj_ptr<CoerceType> result = new CoerceType();
    fill_version_type(isolate, v, result.get());
    retVal = result;
    return result_t();
}

} // namespace fibjs
