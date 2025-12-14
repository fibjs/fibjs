/**
 * @file ts_strip.h
 * @brief TypeScript type stripping for strip-only mode
 *
 * This implementation is inspired by:
 * - TypeRunner (C++ TypeScript parser): Scanner/Parser architecture
 * - SWC swc_ts_fast_strip (Rust): Visitor-based span collection + space replacement
 */

#pragma once

#include "qstring.h"

namespace fibjs {
namespace ts_strip {

    /**
     * Strip TypeScript type annotations from source code
     * 
     * This function removes TS-only syntax while preserving line/column positions
     * by replacing removed content with spaces (keeping newlines).
     * 
     * @param source TypeScript source code (UTF-8)
     * @return JavaScript code with types removed (UTF-8)
     * @throws Error for unsupported syntax (enum, namespace with values, etc.)
     */
    exlib::string strip(const exlib::string& source);

} // namespace ts_strip
} // namespace fibjs
