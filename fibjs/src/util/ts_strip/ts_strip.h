/**
 * @file ts_strip.h
 * @brief TypeScript type stripping for strip-only mode
 *
 * This implementation is inspired by:
 * - TypeRunner (C++ TypeScript parser): Scanner/Parser architecture
 * - SWC swc_ts_fast_strip (Rust): Visitor-based span collection + space replacement
 */

#pragma once

#include "Buffer.h"
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
     * Strip TypeScript type annotations into a buffer of its own
     *
     * `source` is never written to: the erasing happens on a copy this function
     * owns and returns, so a caller can hand over memory it does not own - the
     * loaders pass the buffer from the isolate's file cache, which every sandbox in
     * the process shares - and can drop the result whenever it likes.
     *
     * The result is exactly as large as it needs to be: as long as `source` except
     * for the assignments lowered for parameter properties, which make it longer.
     *
     * @param source TypeScript source code (UTF-8)
     * @param length Length of the data in bytes
     * @return JavaScript code with types removed (UTF-8)
     * @throws Error for unsupported syntax (enum, namespace with values, etc.)
     */
    obj_ptr<Buffer_base> stripToBuffer(const uint8_t* source, size_t length);

} // namespace ts_strip
} // namespace fibjs
