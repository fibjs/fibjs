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

class TrayOptions : public obj_base {
public:
    LOAD_OPTIONS(TrayOptions, (icon)(title)(tooltip)(menu));

public:
    exlib::string icon{};
    std::optional<exlib::string> title;
    std::optional<exlib::string> tooltip;
    std::optional<v8::Local<v8::Value>> menu;
};
}
