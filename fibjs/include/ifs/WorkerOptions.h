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
#include <boost/preprocessor.hpp>

namespace fibjs {

class WorkerOptions : public obj_base {
public:
    LOAD_OPTIONS(WorkerOptions, (eval)(workerData)(file_system)(safe_buffer));

public:
    std::optional<bool> eval = false;
    std::optional<v8::Local<v8::Value>> workerData;
    std::optional<bool> file_system = true;
    std::optional<bool> safe_buffer = false;
};
}
