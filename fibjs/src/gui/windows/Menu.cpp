/*
 * Menu.cpp
 *
 *  Created on: Oct 11, 2024
 *      Author: lion
 */

#ifdef _WIN32

#include "object.h"
#include "Menu.h"
#include "EventInfo.h"
#include "utf8.h"
#include <unordered_map>

namespace fibjs {

static exlib::spinlock g_menuLock;
static std::unordered_map<uint32_t, MenuItem*> g_menuItemMap;
static uint32_t g_nextMenuItemId = 1;

HBITMAP LoadPngBitmap(const BYTE* pngData, size_t pngSize);
HBITMAP ResizeBitmap(HBITMAP hBitmap, int newWidth, int newHeight);

void on_click_menu(uint32_t id)
{
    g_menuLock.lock();
    auto it = g_menuItemMap.find(id);
    g_menuLock.unlock();

    if (it != g_menuItemMap.end()) {
        MenuItem* item = it->second;

        if (item->checked.has_value()) {
            item->checked = !item->checked.value();

            UINT uCheck = item->checked.value() ? MF_CHECKED : MF_UNCHECKED;
            CheckMenuItem((HMENU)item->m_menu_item, item->m_id, MF_BYCOMMAND | uCheck);
        }

        obj_ptr<EventInfo> ei = new EventInfo(item, "click");
        item->_emit("click", ei);
    }
}

void MenuItem::update_os_menu_item()
{
    HMENU hMenu = (HMENU)m_menu_item;
    MENUITEMINFOW mii = { 0 };
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_STRING | MIIM_STATE;

    exlib::wstring wlabel = utf8to16String(label.value());
    mii.dwTypeData = (LPWSTR)wlabel.c_str();

    mii.fState = enabled.value() ? MFS_ENABLED : MFS_DISABLED;
    SetMenuItemInfoW(hMenu, m_id, FALSE, &mii);

    if (checked.has_value()) {
        UINT uCheck = checked.value() ? MF_CHECKED : MF_UNCHECKED;
        CheckMenuItem(hMenu, m_id, MF_BYCOMMAND | uCheck);
    }
}

int32_t GetTextHeight()
{
    HDC hdc = GetDC(NULL);
    SIZE size;
    GetTextExtentPoint32W(hdc, L"A", 1, &size);
    ReleaseDC(NULL, hdc);
    return size.cy;
}

static void* create_os_menu_recursive(Menu* menu, const char* title, bool is_popup)
{
    HMENU hMenu = is_popup ? CreatePopupMenu() : CreateMenu();

    for (MenuItem* item : menu->m_items) {
        UINT uFlags = MF_STRING;

        if (item->checked.has_value() && item->checked.value())
            uFlags |= MF_CHECKED;

        if (!item->enabled.value())
            uFlags |= MF_GRAYED;

        if (item->type.value() == "separator") {
            AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
        } else {
            exlib::wstring wlabel = utf8to16String(item->label.value());
            if (item->submenu.has_value()) {
                HMENU hSubMenu = (HMENU)create_os_menu_recursive(item->submenu.value(), item->label->c_str(), true);
                AppendMenuW(hMenu, uFlags | MF_POPUP, (UINT_PTR)hSubMenu, (LPWSTR)wlabel.c_str());
            } else {
                item->m_id = g_nextMenuItemId++;

                g_menuLock.lock();
                g_menuItemMap[item->m_id] = item;
                g_menuLock.unlock();

                AppendMenuW(hMenu, uFlags, item->m_id, (LPWSTR)wlabel.c_str());

                if (item->m_icon) {
                    HBITMAP hBitmap = LoadPngBitmap(item->m_icon->data(), item->m_icon->length());
                    if (hBitmap) {
                        BITMAP bm;
                        GetObject(hBitmap, sizeof(bm), &bm);

                        int height = GetTextHeight();
                        int newHeight = height * 1.6;
                        int newWidth = bm.bmWidth * newHeight / bm.bmHeight;

                        HBITMAP hResizedBitmap = ResizeBitmap(hBitmap, newWidth, newHeight);
                        DeleteObject(hBitmap);
                        hBitmap = hResizedBitmap;
                    }
                    SetMenuItemBitmaps(hMenu, item->m_id, MF_BYCOMMAND, hBitmap, hBitmap);
                }
            }
        }

        item->m_menu_item = hMenu;
    }

    menu->m_menu = hMenu;

    return hMenu;
}

void* Menu::create_os_menu(bool is_popup)
{
    if (m_menu)
        return m_menu;

    return create_os_menu_recursive(this, "main", is_popup);
}

static void release_os_menu_recursive(Menu* menu)
{
    for (MenuItem* item : menu->m_items) {
        if (item->submenu.has_value()) {
            release_os_menu_recursive(item->submenu.value());
        }
        if (item->m_menu_item) {
            g_menuLock.lock();
            g_menuItemMap.erase(item->m_id);
            g_menuLock.unlock();

            item->m_id = 0;
            item->m_menu_item = nullptr;
        }
    }

    if (menu->m_menu) {
        DestroyMenu((HMENU)menu->m_menu);
        menu->m_menu = nullptr;
    }
}

void Menu::release_os_menu()
{
    release_os_menu_recursive(this);
}

}

#endif
