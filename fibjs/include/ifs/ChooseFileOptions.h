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
#include "ifs/DialogFilter.h"

namespace fibjs {

class ChooseFileOptions : public obj_base {
public:
    LOAD_OPTIONS(ChooseFileOptions, (title)(type)(defaultPath)(multiSelections)(filters));

public:
    std::optional<exlib::string> title;
    std::optional<exlib::string> type = "openFile";
    std::optional<exlib::string> defaultPath;
    std::optional<bool> multiSelections = false;
    std::optional<std::vector<obj_ptr<DialogFilter>>> filters;
};
}
