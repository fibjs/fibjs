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

class PerformanceObserverOptions : public obj_base {
public:
    LOAD_OPTIONS(PerformanceObserverOptions, (entryTypes)(type));

public:
    std::optional<std::vector<exlib::string>> entryTypes;
    std::optional<exlib::string> type;
};
}
