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

class WindowBounds : public obj_base {
public:
    LOAD_OPTIONS(WindowBounds, (x)(y)(width)(height));

public:
    std::optional<double> x;
    std::optional<double> y;
    std::optional<double> width;
    std::optional<double> height;
};
}
