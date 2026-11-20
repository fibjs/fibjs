/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description Semantic Versioning 2.0.0 parsing, comparison and range matching
 *
 * The module implements the algorithm of the `semver` package used by npm: a version is
 * `major.minor.patch` with an optional prerelease and build metadata, and a range combines
 * comparators with `||`. Version precedence ignores build metadata; prerelease identifiers are
 * compared numerically when both are numeric and a version without a prerelease outranks one
 * with it. Ranges understand exact versions, `>`, `>=`, `<`, `<=`, `=`, hyphen ranges
 * (`1.2.3 - 2.0.0`), tilde (`~1.2`), caret (`^1.2.3`), x-ranges (`1.x`, `1.2.*`) and `*`.
 *
 * Concepts:
 *
 * - **Version**: `major.minor.patch`, each a non-negative integer no larger than
 *   `9007199254740991`; a leading `v` is accepted. The optional prerelease follows `-` and the
 *   optional build metadata follows `+`.
 * - **Options**: every function takes a `SemVerOptions` object. `loose` accepts leading zeros
 *   and a prerelease without the hyphen (`1.2.3beta`), `includePrerelease` lets prereleases
 *   match ranges that would otherwise exclude them, and `rtl` makes `coerce` scan from the
 *   right. Omitted fields use their defaults.
 * - **Ranges**: an empty string, `*` and `x` match any version. A comparator set is the
 *   conjunction of its comparators and `||` is disjunction. By default a prerelease only
 *   satisfies a range when a comparator with the same major, minor and patch also carries a
 *   prerelease.
 * - **Return values**: `parse` returns an object with `version`, `raw`, `major`, `minor`,
 *   `patch`, `prerelease` and `build` (or `null`), `valid`/`validRange`/`clean` return a string
 *   or `null`, and the comparison helpers throw a TypeError for an invalid version.
 *
 * Import:
 * ```JavaScript
 * const semver = require('semver');
 * ```
 *
 * Example 1 — parse and compare versions:
 * ```JavaScript
 * const semver = require('semver');
 *
 * const v = semver.parse('v1.2.3-beta.1+build.7');
 * console.log(v.major, v.minor, v.patch); // 1 2 3
 * console.log(v.prerelease); // [ 'beta', 1 ]
 * console.log(semver.compare('1.2.3', '1.2.3-beta')); // 1
 * console.log(semver.rcompare('1.2.3', '1.2.3-beta')); // -1
 * ```
 *
 * Example 2 — match ranges:
 * ```JavaScript
 * const semver = require('semver');
 *
 * console.log(semver.satisfies('1.5.0', '^1.2.3')); // true
 * console.log(semver.satisfies('2.0.0', '^1.2.3')); // false
 * console.log(semver.validRange('~1.2.3 || >=2')); // >=1.2.3 <1.3.0-0||>=2.0.0
 * console.log(semver.maxSatisfying(['1.2.3', '1.2.9', '1.3.0'], '~1.2.3')); // 1.2.9
 * ```
 *
 * Example 3 — ranges intersect and versions sort:
 * ```JavaScript
 * const semver = require('semver');
 *
 * console.log(semver.intersects('>=1.0.0 <2.0.0', '>=1.5.0')); // true
 * console.log(semver.intersects('^1.0.0', '^2.0.0')); // false
 * console.log(semver.sort(['1.10.0', '1.2.0', '1.2.0-beta']));
 * // [ '1.2.0-beta', '1.2.0', '1.10.0' ]
 * ```
 *
 */
declare module 'semver' {
    /**
     * @description The Semantic Versioning specification implemented by this module
     *
     *      Always `"2.0.0"`; it names the semver.org specification, not the version of the
     *      algorithm or of the fibjs binary.
     *
     */
    const SEMVER_SPEC_VERSION: string;

    /**
     * @description Parses a version string into its components
     *
     *      Returns `null` instead of throwing when the input is not a valid version, which makes
     *      the function suitable for validating untrusted input. The returned object carries the
     *      formatted `version` (build metadata omitted, as in every comparison), the original
     *      `raw` string, the numeric `major`, `minor` and `patch`, and the `prerelease` and
     *      `build` identifier arrays. A prerelease identifier that is a number below
     *      `9007199254740991` is stored as a number, every other identifier as a string.
     *
     *      @param version the version string to parse
     *      @param options the options object; `loose` relaxes the syntax
     *      @return the parsed object, or `null` when the version is invalid
     *
     */
    function parse(version: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): {
        version: string;
        raw: string;
        major: number;
        minor: number;
        patch: number;
        prerelease: any[];
        build: string[];
    };

    /**
     * @description Returns the normalized form of a version string
     *
     *      `valid` is `parse` reduced to the formatted version: `"v1.2.3+build"` becomes
     *      `"1.2.3"` and an invalid version becomes `null`. Build metadata is never part of the
     *      result because it does not take part in precedence.
     *
     *      @param version the version string to validate
     *      @param options the options object; `loose` relaxes the syntax
     *      @return the normalized version, or `null` when the version is invalid
     *
     */
    function valid(version: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): string;

    /**
     * @description Strips surrounding whitespace, a leading `=`/`v` and build metadata
     *
     *      Trims the string, removes every leading `=` and `v`, parses the result and returns its
     *      normalized version. Range operators such as `>`, `~` and `^` are not stripped, so those
     *      inputs return `null` instead of a version.
     *
     *      @param version the string to clean
     *      @param options the options object; `loose` relaxes the syntax
     *      @return the cleaned version, or `null` when the input is not a version
     *
     */
    function clean(version: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): string;

    /**
     * @description Compares two versions by precedence
     *
     *      Returns `-1` when the first version has lower precedence, `1` when it has higher
     *      precedence and `0` when they are equivalent. Build metadata is ignored, a version
     *      without a prerelease outranks one with the same main version and prerelease, and
     *      numeric prerelease identifiers compare numerically while mixed identifiers compare as
     *      strings. An invalid version throws a TypeError.
     *
     *      @param version1 the left version
     *      @param version2 the right version
     *      @param options the options object; `loose` relaxes the syntax
     *      @return -1, 0 or 1
     *
     */
    function compare(version1: string, version2: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): number;

    /**
     * @description Compares two versions using the loose syntax
     *
     *      Shorthand for `compare(version1, version2, { "loose": true })`; it accepts leading
     *      zeros and a prerelease written without the hyphen. An invalid version throws a
     *      TypeError.
     *
     *      @param version1 the left version
     *      @param version2 the right version
     *      @return -1, 0 or 1
     *
     */
    function compareLoose(version1: string, version2: string): number;

    /**
     * @description Compares two versions including their build metadata
     *
     *      Like `compare`, but an equal precedence falls back to comparing the build identifier
     *      arrays, which is what `sort` and `rsort` use. An invalid version throws a TypeError.
     *
     *      @param version1 the left version
     *      @param version2 the right version
     *      @param options the options object; `loose` relaxes the syntax
     *      @return -1, 0 or 1
     *
     */
    function compareBuild(version1: string, version2: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): number;

    /**
     * @description Compares two versions in reverse order
     *
     *      Equivalent to `compare(version2, version1)`; useful directly as a sort comparator
     *      when the newest version must come first. An invalid version throws a TypeError.
     *
     *      @param version1 the left version
     *      @param version2 the right version
     *      @param options the options object; `loose` relaxes the syntax
     *      @return -1, 0 or 1
     *
     */
    function rcompare(version1: string, version2: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): number;

    /**
     * @description Sorts version strings in ascending precedence order
     *
     *      Returns a new array with the same values sorted by `compareBuild`; build metadata
     *      breaks ties and the input array is left untouched. Every element must be a valid
     *      version or the call throws a TypeError.
     *
     *      @param list the array of version strings to sort
     *      @param options the options object; `loose` relaxes the syntax
     *      @return a new sorted array
     *
     */
    function sort(list: string[], options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): string[];

    /**
     * @description Sorts version strings in descending precedence order
     *
     *      Returns a new array with the same values sorted by the reverse of `compareBuild`, so
     *      the newest version comes first. Every element must be a valid version or the call
     *      throws a TypeError.
     *
     *      @param list the array of version strings to sort
     *      @param options the options object; `loose` relaxes the syntax
     *      @return a new sorted array
     *
     */
    function rsort(list: string[], options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): string[];

    /**
     * @description Tests whether the first version has higher precedence
     *
     *      Returns `true` when `compare(version1, version2)` is greater than zero. An invalid
     *      version throws a TypeError.
     *
     *      @param version1 the left version
     *      @param version2 the right version
     *      @param options the options object; `loose` relaxes the syntax
     *      @return whether the first version is greater
     *
     */
    function gt(version1: string, version2: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): boolean;

    /**
     * @description Tests whether the first version has higher or equal precedence
     *
     *      Returns `true` when `compare(version1, version2)` is zero or greater. An invalid
     *      version throws a TypeError.
     *
     *      @param version1 the left version
     *      @param version2 the right version
     *      @param options the options object; `loose` relaxes the syntax
     *      @return whether the first version is greater than or equal
     *
     */
    function gte(version1: string, version2: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): boolean;

    /**
     * @description Tests whether the first version has lower precedence
     *
     *      Returns `true` when `compare(version1, version2)` is less than zero. An invalid
     *      version throws a TypeError.
     *
     *      @param version1 the left version
     *      @param version2 the right version
     *      @param options the options object; `loose` relaxes the syntax
     *      @return whether the first version is less
     *
     */
    function lt(version1: string, version2: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): boolean;

    /**
     * @description Tests whether the first version has lower or equal precedence
     *
     *      Returns `true` when `compare(version1, version2)` is zero or less. An invalid version
     *      throws a TypeError.
     *
     *      @param version1 the left version
     *      @param version2 the right version
     *      @param options the options object; `loose` relaxes the syntax
     *      @return whether the first version is less than or equal
     *
     */
    function lte(version1: string, version2: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): boolean;

    /**
     * @description Tests whether two versions have equal precedence
     *
     *      Build metadata and the spelling of the main version do not matter, so `"v1.2.3"` and
     *      `"1.2.3+other"` are equal. An invalid version throws a TypeError.
     *
     *      @param version1 the left version
     *      @param version2 the right version
     *      @param options the options object; `loose` relaxes the syntax
     *      @return whether the two versions are equivalent
     *
     */
    function eq(version1: string, version2: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): boolean;

    /**
     * @description Tests whether two versions have different precedence
     *
     *      The negation of `eq`; build metadata does not make versions different. An invalid
     *      version throws a TypeError.
     *
     *      @param version1 the left version
     *      @param version2 the right version
     *      @param options the options object; `loose` relaxes the syntax
     *      @return whether the two versions are not equivalent
     *
     */
    function neq(version1: string, version2: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): boolean;

    /**
     * @description Compares two versions with an explicit operator
     *
     *      Accepts the operators `=`, `==`, `!=`, `>`, `>=`, `<` and `<=`; `===` and `!==`
     *      compare the raw strings instead of their precedence. Any other operator throws a
     *      TypeError, as does an invalid version.
     *
     *      @param version1 the left version
     *      @param op one of `=`, `==`, `!=`, `>`, `>=`, `<`, `<=`, `===`, `!==`
     *      @param version2 the right version
     *      @param options the options object; `loose` relaxes the syntax
     *      @return the result of the comparison
     *
     */
    function cmp(version1: string, op: string, version2: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): boolean;

    /**
     * @description Returns the canonical form of a range
     *
     *      Parses the range and returns its normalized comparator form, or `null` when the range
     *      is invalid. An empty range and `*` become `"*"`, whitespace around operators is
     *      removed and every comparator set is joined with `||` without spaces. In loose mode
     *      comparators that do not parse are discarded.
     *
     *      @param range the range string to validate
     *      @param options the options object; `loose` and `includePrerelease` affect expansion
     *      @return the canonical range, or `null` when the range is invalid
     *
     */
    function validRange(range: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): string;

    /**
     * @description Tests whether a version satisfies a range
     *
     *      Returns `false` for an invalid version or an invalid range instead of throwing. A
     *      prerelease version only matches when a comparator with the same major, minor and patch
     *      also carries a prerelease, unless `includePrerelease` is set.
     *
     *      @param version the version to test
     *      @param range the range to test against
     *      @param options the options object; `loose` and `includePrerelease` change the matching
     *      @return whether the version satisfies the range
     *
     */
    function satisfies(version: string, range: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): boolean;

    /**
     * @description Returns the highest version of a list that satisfies a range
     *
     *      Returns one of the input values, preserving its spelling, or `null` when no value
     *      matches or the range is invalid. Versions that are not valid are ignored.
     *
     *      @param versions the candidate version strings
     *      @param range the range every candidate is tested against
     *      @param options the options object; `loose` and `includePrerelease` change the matching
     *      @return the highest matching value, or `null`
     *
     */
    function maxSatisfying(versions: string[], range: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): string;

    /**
     * @description Returns the lowest version of a list that satisfies a range
     *
     *      Returns one of the input values, preserving its spelling, or `null` when no value
     *      matches or the range is invalid. Versions that are not valid are ignored.
     *
     *      @param versions the candidate version strings
     *      @param range the range every candidate is tested against
     *      @param options the options object; `loose` and `includePrerelease` change the matching
     *      @return the lowest matching value, or `null`
     *
     */
    function minSatisfying(versions: string[], range: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): string;

    /**
     * @description Returns the lowest version that can satisfy a range
     *
     *      Returns a parsed object (with the same shape as `parse`) for the smallest possible
     *      version, or `null` when an empty intersection makes the range impossible. An invalid
     *      range throws a TypeError.
     *
     *      @param range the range to inspect
     *      @param options the options object; `loose` and `includePrerelease` affect expansion
     *      @return the minimum version object, or `null`
     *
     */
    function minVersion(range: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): {
        version: string;
        raw: string;
        major: number;
        minor: number;
        patch: number;
        prerelease: any[];
        build: string[];
    };

    /**
     * @description Tests whether a version is greater than every version of a range
     *
     *      Returns `false` when the version satisfies the range or when the comparison is
     *      inconclusive. An invalid version or range throws a TypeError.
     *
     *      @param version the version to test
     *      @param range the range to compare against
     *      @param options the options object; `loose` and `includePrerelease` change the matching
     *      @return whether the version is above the whole range
     *
     */
    function gtr(version: string, range: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): boolean;

    /**
     * @description Tests whether a version is less than every version of a range
     *
     *      Returns `false` when the version satisfies the range or when the comparison is
     *      inconclusive. An invalid version or range throws a TypeError.
     *
     *      @param version the version to test
     *      @param range the range to compare against
     *      @param options the options object; `loose` and `includePrerelease` change the matching
     *      @return whether the version is below the whole range
     *
     */
    function ltr(version: string, range: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): boolean;

    /**
     * @description Tests whether a version lies outside a range in the given direction
     *
     *      The general form of `gtr` and `ltr`: `hilo` is `">"` for a version above the range or
     *      `"<"` for one below it. Any other value throws a TypeError, as does an invalid version
     *      or range.
     *
     *      @param version the version to test
     *      @param range the range to compare against
     *      @param hilo `>` for above, `<` for below
     *      @param options the options object; `loose` and `includePrerelease` change the matching
     *      @return whether the version is outside the range in that direction
     *
     */
    function outside(version: string, range: string, hilo: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): boolean;

    /**
     * @description Tests whether two ranges have at least one version in common
     *
     *      Both ranges are expanded into comparator sets; the call returns `true` when some set
     *      of the first range is compatible with some set of the second. An invalid range throws
     *      a TypeError.
     *
     *      @param range1 the first range
     *      @param range2 the second range
     *      @param options the options object; `loose` and `includePrerelease` affect expansion
     *      @return whether the ranges intersect
     *
     */
    function intersects(range1: string, range2: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): boolean;

    /**
     * @description Returns the prerelease identifiers of a version
     *
     *      Returns the identifier array (`[ 'beta', 1 ]` for `1.2.3-beta.1`) or `null` when the
     *      version has no prerelease or is invalid. The array is a copy, so changing it does not
     *      affect anything else.
     *
     *      @param version the version to inspect
     *      @param options the options object; `loose` relaxes the syntax
     *      @return the prerelease identifiers, or `null`
     *
     */
    function prerelease(version: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): any[];

    /**
     * @description Returns the major number of a version
     *
     *      Parses the version and returns its major component. An invalid version throws a
     *      TypeError.
     *
     *      @param version the version to inspect
     *      @param options the options object; `loose` relaxes the syntax
     *      @return the major number
     *
     */
    function major(version: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): number;

    /**
     * @description Returns the minor number of a version
     *
     *      Parses the version and returns its minor component. An invalid version throws a
     *      TypeError.
     *
     *      @param version the version to inspect
     *      @param options the options object; `loose` relaxes the syntax
     *      @return the minor number
     *
     */
    function minor(version: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): number;

    /**
     * @description Returns the patch number of a version
     *
     *      Parses the version and returns its patch component. An invalid version throws a
     *      TypeError.
     *
     *      @param version the version to inspect
     *      @param options the options object; `loose` relaxes the syntax
     *      @return the patch number
     *
     */
    function patch(version: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): number;

    /**
     * @description Increments a version
     *
     *      Accepts the release names `major`, `minor`, `patch`, `premajor`, `preminor`,
     *      `prepatch`, `prerelease` and `release`. The `pre*` names drop the existing
     *      prerelease and enter a new one; `release` removes the prerelease of a version and
     *      throws when the version has none. `identifier` prefixes the new prerelease (for
     *      example `dev` yields `1.2.4-dev.0`), and `identifierBase` chooses the starting number
     *      (`0` or `1`) or `false` omits the number entirely. Returns `null` for an invalid
     *      version or release name.
     *
     *      @param version the version to increment
     *      @param release the release name
     *      @param options the options object; `loose` relaxes the syntax
     *      @param identifier an optional prerelease identifier
     *      @param identifierBase the starting number, or `false` to omit it
     *      @return the incremented version, or `null`
     *
     */
    function inc(version: string, release: string, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }, identifier?: string, identifierBase?: number | boolean): string;

    /**
     * @description Returns the kind of difference between two versions
     *
     *      Returns `major`, `minor`, `patch`, `prerelease` or one of their `pre`-prefixed forms,
     *      or `null` when the versions have equal precedence. An invalid version throws a
     *      TypeError.
     *
     *      @param version1 the first version
     *      @param version2 the second version
     *      @return the kind of difference, or `null`
     *
     */
    function diff(version1: string, version2: string): string;

    /**
     * @description Coerces a string into a version
     *
     *      Scans the string for the first (or, with `rtl`, the last) sequence of one to three
     *      numbers and builds a version from it, filling missing minor and patch numbers with
     *      zero. The first number must not be preceded by another digit. Returns `null` when no
     *      number sequence exists.
     *
     *      @param version the string or number to coerce
     *      @param options the options object; `rtl` scans from the right
     *      @return the coerced version object, or `null`
     *
     */
    function coerce(version: string | number, options?: {
        loose?: boolean;
        includePrerelease?: boolean;
        rtl?: boolean;
    }): {
        version: string;
        raw: string;
        major: number;
        minor: number;
        patch: number;
        prerelease: any[];
        build: string[];
    };

}

