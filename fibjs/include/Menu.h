/*
 * Menu.h
 *
 *  Created on: Oct 11, 2024
 *      Author: lion
 */

#pragma once

#include <boost/preprocessor.hpp>

#include "ifs/Menu.h"
#include "Buffer.h"
#include "Event.h"

namespace fibjs {

class Menu;

class MenuItem : public MenuItem_base {
    EVENT_SUPPORT();

public:
    // MenuItem_base
    virtual result_t get_id(exlib::string& retVal);
    virtual result_t set_id(exlib::string newVal);
    virtual result_t get_type(exlib::string& retVal);
    virtual result_t get_icon(exlib::string& retVal);
    virtual result_t set_icon(exlib::string newVal);
    virtual result_t get_label(exlib::string& retVal);
    virtual result_t set_label(exlib::string newVal);
    virtual result_t get_tooltip(exlib::string& retVal);
    virtual result_t set_tooltip(exlib::string newVal);
    virtual result_t get_enabled(bool& retVal);
    virtual result_t set_enabled(bool newVal);
    virtual result_t get_checked(bool& retVal);
    virtual result_t set_checked(bool newVal);
    virtual result_t get_submenu(obj_ptr<Menu_base>& retVal);

public:
    EVENT_FUNC(click);

public:
    // object_base
    virtual result_t toJSON(exlib::string key, v8::Local<v8::Value>& retVal);

public:
    LOAD_OPTIONS(MenuItem, (id)(type)(icon)(label)(tooltip)(enabled)(checked)(submenu)(onclick));
    static result_t create(v8::Local<v8::Object> item, obj_ptr<MenuItem>& retVal);

public:
    void update_os_menu_item();
    void update_menu_item()
    {
        if (m_menu_item) {
            Ref();
            async([this]() {
                if (m_menu_item)
                    update_os_menu_item();

                Unref();
            },
                CALL_E_GUICALL);
        }
    }

public:
    std::optional<exlib::string> id = "";
    std::optional<exlib::string> type;
    std::optional<exlib::string> icon;
    std::optional<exlib::string> label;
    std::optional<exlib::string> tooltip;
    std::optional<bool> enabled = true;
    std::optional<bool> checked;
    std::optional<obj_ptr<Menu>> submenu;

    std::optional<v8::Local<v8::Function>> onclick;

    uint32_t m_id = 0;
    obj_ptr<Buffer> m_icon;

    void* m_menu_item = nullptr;
};

class Menu : public Menu_base {
public:
    ~Menu()
    {
        release_os_menu();
    }

public:
    EVENT_SUPPORT();

public:
    // Menu_base
    virtual result_t append(v8::Local<v8::Object> item);
    virtual result_t insert(int32_t pos, v8::Local<v8::Object> item);
    virtual result_t remove(int32_t pos);
    virtual result_t get_length(int32_t& retVal);
    virtual result_t _indexed_getter(uint32_t index, obj_ptr<MenuItem_base>& retVal);

public:
    // object_base
    virtual result_t toJSON(exlib::string key, v8::Local<v8::Value>& retVal);

public:
    static result_t create(v8::Local<v8::Array> items, obj_ptr<Menu>& retVal);
    void* create_os_menu(bool is_popup = true);
    void release_os_menu();

public:
    result_t _append_items(v8::Local<v8::Array> items);

public:
    std::vector<obj_ptr<MenuItem>> m_items;

    void* m_menu = nullptr;
};

inline result_t GetArgumentValue(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<Menu>& vr, bool bStrict = false)
{
    vr = (Menu*)Menu_base::getInstance(v);
    if (vr == NULL) {
        if (v->IsArray())
            return Menu::create(v.As<v8::Array>(), vr);

        return CALL_E_TYPEMISMATCH;
    }

    return 0;
}

}
