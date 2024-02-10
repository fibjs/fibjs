/*
 * crypto.h
 *
 *  Created on: Feb 11, 2024
 *      Author: lion
 */

#pragma once

#include "utils.h"

namespace fibjs {

result_t randomBytes(uint8_t* buf, int32_t size);

result_t openssl_error();

} /* namespace fibjs */
