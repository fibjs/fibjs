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
     * Parameter properties (`constructor(private x: number)`) are the exception:
     * they cannot be erased, because TypeScript declares the field and assigns it in
     * the constructor. They are lowered into `this.x = x;` at the top of the
     * constructor body (after a leading `super(...)` call), so the result can be
     * longer than the input for files that use them.
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
     * The one thing an in-place buffer cannot hold is the assignment generated for a
     * parameter property, so when the file uses one the caller has to use `out`
     * instead of the buffer (which is left stripped, but without the assignments).
     * 
     * @param data Pointer to UTF-8 encoded TypeScript source
     * @param length Length of the data in bytes
     * @param out Filled with the stripped code when the buffer cannot hold it
     * @return true when the buffer holds the result, false when `out` must be used
     * @throws Error for unsupported syntax (enum, namespace with values, etc.)
     */
    bool stripInPlace(uint8_t* data, size_t length, exlib::string& out);

} // namespace ts_strip
} // namespace fibjs
