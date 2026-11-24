/*
 * semver.h
 *
 *  Semantic Versioning 2.0.0 parser, comparator and range matcher.
 *  The algorithm mirrors the npm `semver` package (v7.x) so that the
 *  installer, package engines checks and user code agree on ranges.
 */

#pragma once

#include <stdint.h>
#include <string>
#include <vector>

namespace fibjs {
namespace semver_detail {

const size_t MAX_LENGTH = 256;
const uint64_t MAX_SAFE_INTEGER = 9007199254740991ULL;

struct Options {
    bool loose = false;
    bool includePrerelease = false;
    bool rtl = false;
};

// A prerelease identifier. `numeric` is true when the text is all digits,
// which is how semver chooses numeric or lexical ordering. `is_number` marks
// the safe-integer numbers that the package stores as JavaScript numbers;
// `text` is their canonical decimal. A numeric identifier outside the safe
// range keeps `is_number == false` and its original text.
struct Identifier {
    bool numeric = false;
    bool is_number = false;
    uint64_t number = 0;
    std::string text;
};

struct SemVer {
    uint64_t major = 0;
    uint64_t minor = 0;
    uint64_t patch = 0;
    std::vector<Identifier> prerelease;
    std::vector<std::string> build;
    std::string raw;
    std::string version;

    void format();
};

// Parses a version. `version` is trimmed before matching, as the package
// does. Returns false instead of throwing.
bool parse_version(const std::string& version, const Options& options, SemVer& out);

int compare_main(const SemVer& a, const SemVer& b);
int compare_pre(const SemVer& a, const SemVer& b);
int compare_build(const SemVer& a, const SemVer& b);
int compare_versions(const SemVer& a, const SemVer& b);
int compare_versions_build(const SemVer& a, const SemVer& b);
int compare_identifiers(const Identifier& a, const Identifier& b);

struct Comparator {
    // `any` is the empty comparator (`''` or `*`), which matches everything.
    bool any = true;
    std::string op; // "", ">", ">=", "<", "<=" (never "=")
    SemVer semver;
    std::string value; // op + semver.version, or "" for any

    bool parse(const std::string& comp, const Options& options);
    bool test(const SemVer& version) const;
};

struct Range {
    Options options;
    std::vector<std::vector<Comparator>> set;

    bool parse(const std::string& range, const Options& options);
    // Uncached parser used by parse() to fill the LRU entry.
    bool parse_uncached(const std::string& range, const Options& options);
    bool parse_range(const std::string& range, const Options& options,
        std::vector<Comparator>& out);
    std::string to_string() const;
    bool test(const SemVer& version) const;
    bool test(const std::string& version) const;

    bool intersects(const Range& other, const Options& options) const;
};

bool max_satisfying(const std::vector<std::string>& versions, const std::string& range,
    const Options& options, size_t& index);
bool min_satisfying(const std::vector<std::string>& versions, const std::string& range,
    const Options& options, size_t& index);

// Returns false when the range is invalid (the caller reports that as null),
// otherwise stores the minimum version and whether one exists.
bool min_version(const std::string& range, const Options& options, SemVer& out, bool& found);

bool outside(const std::string& version, const std::string& range, const char* hilo,
    const Options& options, bool& result);

bool increment(std::string version, const std::string& release, const std::string& identifier,
    int identifier_base, bool identifier_base_false, const Options& options,
    std::string& out_without_build);

bool diff_versions(const std::string& v1, const std::string& v2, std::string& out);

// Scans `version` for a coercible number sequence. Returns false only when no
// sequence exists; `out` is then untouched.
bool coerce_version(const std::string& version, const Options& options, SemVer& out);

} // namespace semver_detail
} // namespace fibjs
