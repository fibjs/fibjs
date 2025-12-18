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
#include <cstdint>

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

    /**
     * Strip TypeScript type annotations in-place on UTF-8 buffer
     * 
     * This function modifies the buffer data directly, avoiding extra memory copies.
     * The buffer length remains unchanged (types are replaced with spaces).
     * 
     * @param data Pointer to UTF-8 encoded TypeScript source
     * @param length Length of the data in bytes
     * @throws Error for unsupported syntax (enum, namespace with values, etc.)
     */
    void stripInPlace(uint8_t* data, size_t length);

} // namespace ts_strip
} // namespace fibjs
