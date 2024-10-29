/*
 * gui.h
 *
 *  Created on: Oct 25, 2024
 *      Author: lion
 */

#include "object.h"
#include "ifs/gui.h"
#include "ifs/fs.h"
#include <boost/preprocessor.hpp>

namespace fibjs {

extern exlib::Event g_gui_ready;

void start_gui();
void run_os_gui();

inline result_t check_gui(AsyncEvent* ac)
{
    if (ac->isSync()) {
        start_gui();
        return CHECK_ERROR(CALL_E_GUICALL);
    }

    return 0;
}

class DialogOptions : public obj_base {
public:
    class FilterItem : public obj_base {
    public:
        LOAD_OPTIONS(FilterItem, (name)(extensions))

    public:
        exlib::string name;
        std::vector<exlib::string> extensions;
    };

public:
    LOAD_OPTIONS(DialogOptions, (title)(type)(defaultPath)(filters)(multiSelections))

public:
    std::optional<exlib::string> title;
    std::optional<exlib::string> type = "openFile";
    std::optional<exlib::string> defaultPath;
    std::optional<std::vector<obj_ptr<FilterItem>>> filters;
    std::optional<bool> multiSelections = false;
};

} // namespace fibjs
