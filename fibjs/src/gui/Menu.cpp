/*
 * Menu.cpp
 *
 *  Created on: Oct 11, 2024
 *      Author: lion
 */

#ifdef OS_DESKTOP

#include "object.h"
#include "ifs/gui.h"
#include "ifs/fs.h"
#include "Menu.h"

namespace fibjs {

result_t gui_base::createMenu(v8::Local<v8::Array> items, obj_ptr<Menu_base>& retVal)
{
    obj_ptr<Menu> menu = new Menu();
    retVal = menu;
    return menu->_append_items(items);
}

result_t Menu::create(v8::Local<v8::Array> items, obj_ptr<Menu>& retVal)
{
    obj_ptr<Menu> menu = new Menu();
    retVal = menu;
    return menu->_append_items(items);
}

result_t MenuItem::get_id(exlib::string& retVal)
{
    retVal = id.value();
    return 0;
}

result_t MenuItem::set_id(exlib::string newVal)
{
    id = newVal;
    return 0;
}

result_t MenuItem::get_type(exlib::string& retVal)
{
    retVal = type.value();
    return 0;
}

result_t MenuItem::get_icon(exlib::string& retVal)
{
    if (!icon.has_value())
        return CALL_RETURN_UNDEFINED;
    retVal = icon.value();
    return 0;
}

result_t MenuItem::set_icon(exlib::string newVal)
{
    if (!icon.has_value())
        return CALL_RETURN_UNDEFINED;
    icon = newVal;
    return 0;
}

result_t MenuItem::get_label(exlib::string& retVal)
{
    if (!label.has_value())
        return CALL_RETURN_UNDEFINED;
    retVal = label.value();
    return 0;
}

result_t MenuItem::set_label(exlib::string newVal)
{
    if (!label.has_value())
        return CALL_RETURN_UNDEFINED;
    label = newVal;
    update_os_menu_item();
    return 0;
}

result_t MenuItem::get_tooltip(exlib::string& retVal)
{
    if (!tooltip.has_value())
        return CALL_RETURN_UNDEFINED;
    retVal = tooltip.value();
    return 0;
}

result_t MenuItem::set_tooltip(exlib::string newVal)
{
    if (!tooltip.has_value())
        return CALL_RETURN_UNDEFINED;
    tooltip = newVal;
    update_os_menu_item();
    return 0;
}

result_t MenuItem::get_enabled(bool& retVal)
{
    retVal = enabled.value();
    return 0;
}

result_t MenuItem::set_enabled(bool newVal)
{
    enabled = newVal;
    update_os_menu_item();
    return 0;
}

result_t MenuItem::get_checked(bool& retVal)
{
    if (!checked.has_value())
        return CALL_RETURN_UNDEFINED;
    retVal = checked.value();
    return 0;
}

result_t MenuItem::set_checked(bool newVal)
{
    if (!checked.has_value())
        return CALL_RETURN_UNDEFINED;

    checked = newVal;
    update_os_menu_item();
    return 0;
}

result_t MenuItem::get_submenu(obj_ptr<Menu_base>& retVal)
{
    if (!submenu.has_value())
        return CALL_RETURN_NULL;

    retVal = submenu.value();
    return 0;
}

result_t MenuItem::toJSON(exlib::string key, v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> obj = v8::Object::New(isolate->m_isolate);

    if (!id.value().empty())
        obj->Set(context, isolate->NewString("id"), isolate->NewString(id.value())).IsJust();

    obj->Set(context, isolate->NewString("type"), isolate->NewString(type.value())).IsJust();

    if (icon.has_value())
        obj->Set(context, isolate->NewString("icon"), isolate->NewString(icon.value())).IsJust();

    if (label.has_value())
        obj->Set(context, isolate->NewString("label"), isolate->NewString(label.value())).IsJust();

    if (tooltip.has_value())
        obj->Set(context, isolate->NewString("tooltip"), isolate->NewString(tooltip.value())).IsJust();

    if (!enabled.value())
        obj->Set(context, isolate->NewString("enabled"), v8::False(isolate->m_isolate)).IsJust();

    if (checked.has_value())
        obj->Set(context, isolate->NewString("checked"), v8::True(isolate->m_isolate)).IsJust();

    if (submenu.has_value()) {
        v8::Local<v8::Value> v;
        submenu.value()->toJSON("", v);
        obj->Set(context, isolate->NewString("submenu"), v).IsJust();
    }

    retVal = obj;

    return 0;
}

result_t MenuItem::create(v8::Local<v8::Object> item, obj_ptr<MenuItem>& retVal)
{
    obj_ptr<MenuItem> mi;
    result_t hr = MenuItem::load(item, mi);
    if (hr < 0)
        return hr;

    if (!mi->type.has_value()) {
        if (mi->submenu.has_value())
            mi->type = "submenu";
        else if (mi->checked.has_value())
            mi->type = "checkbox";
        else if (mi->label.has_value())
            mi->type = "normal";
        else
            mi->type = "separator";
    } else if (mi->type != "normal" && mi->type != "separator" && mi->type != "checkbox" && mi->type != "submenu")
        return Runtime::setError("MenuItem: Invalid menu item type: " + mi->type.value());

    if (mi->type.value() == "separator") {
        if (mi->label.has_value())
            return Runtime::setError("MenuItem: Separator menu item can't have label");

        if (mi->tooltip.has_value())
            return Runtime::setError("MenuItem: Separator menu item can't have tooltip");

        if (mi->icon.has_value())
            return Runtime::setError("MenuItem: Separator menu item can't have icon");

        if (mi->submenu.has_value())
            return Runtime::setError("MenuItem: Separator menu item can't have submenu");

        if (mi->checked.has_value())
            return Runtime::setError("MenuItem: Separator menu item can't have checked");
    } else {
        if (!mi->label.has_value() || mi->label.value().empty())
            return Runtime::setError("MenuItem: Menu item label is empty");

        if (mi->type.value() == "submenu") {
            if (mi->checked.has_value())
                return Runtime::setError("MenuItem: Submenu menu item can't have checked");

            if (!mi->submenu.has_value())
                return Runtime::setError("MenuItem: Submenu menu item must have submenu");
        } else if (mi->type.value() == "checkbox") {
            if (!mi->checked.has_value())
                mi->checked = false;

            if (mi->submenu.has_value())
                return Runtime::setError("MenuItem: Checkbox menu item can't have submenu");
        } else {
            if (mi->checked.has_value())
                return Runtime::setError("MenuItem: Normal menu item can't have checked");

            if (mi->submenu.has_value())
                return Runtime::setError("MenuItem: Normal menu item can't have submenu");
        }
    }

    if (mi->icon.has_value()) {
        Variant var;
        hr = fs_base::ac_readFile(mi->icon.value(), "", var);
        if (hr)
            return hr;

        mi->m_icon = (Buffer*)var.object();
        if (!mi->m_icon)
            return Runtime::setError("Menu item icon is empty");
    }

    if (mi->onclick.has_value())
        mi->set_onclick(mi->onclick.value());

    retVal = mi;

    return 0;
}

result_t Menu::_append_items(v8::Local<v8::Array> items)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    result_t hr;
    int32_t len = items->Length();

    for (int32_t i = 0; i < len; i++) {
        v8::Local<v8::Value> item = items->Get(context, i).FromMaybe(v8::Local<v8::Value>());
        if (!IsJSObject(item))
            return Runtime::setError("Menu: Invalid menu item");

        v8::Local<v8::Object> o = item.As<v8::Object>();
        hr = append(o);
        if (hr < 0)
            return hr;
    }

    return 0;
}

result_t Menu::append(v8::Local<v8::Object> item)
{
    if (m_menu)
        return Runtime::setError("Menu: Menu is already attached to a window");

    obj_ptr<MenuItem> mi;
    result_t hr = MenuItem::create(item, mi);
    if (hr < 0)
        return hr;

    m_items.push_back(mi);

    return 0;
}

result_t Menu::insert(int32_t pos, v8::Local<v8::Object> item)
{
    if (m_menu)
        return Runtime::setError("Menu: Menu is already attached to a window");

    if (pos < 0 || pos > m_items.size())
        return CALL_E_OUTRANGE;

    obj_ptr<MenuItem> mi;
    result_t hr = MenuItem::create(item, mi);
    if (hr < 0)
        return hr;

    m_items.insert(m_items.begin() + pos, mi);

    return 0;
}

result_t Menu::remove(int32_t pos)
{
    if (m_menu)
        return Runtime::setError("Menu: Menu is already attached to a window");

    if (pos < 0 || pos >= m_items.size())
        return CALL_E_OUTRANGE;

    m_items.erase(m_items.begin() + pos);

    return 0;
}

result_t Menu::get_length(int32_t& retVal)
{
    retVal = m_items.size();
    return 0;
}

result_t Menu::_indexed_getter(uint32_t index, obj_ptr<MenuItem_base>& retVal)
{
    if (index >= m_items.size())
        return CALL_E_OUTRANGE;
    retVal = m_items[index];
    return 0;
}

result_t Menu::toJSON(exlib::string key, v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Array> arr = v8::Array::New(isolate->m_isolate, m_items.size());

    for (int i = 0; i < m_items.size(); i++) {
        v8::Local<v8::Value> v;
        m_items[i]->toJSON("", v);
        arr->Set(context, i, v);
    }

    retVal = arr;

    return 0;
}

}

#endif
