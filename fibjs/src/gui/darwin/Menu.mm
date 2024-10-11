/*
 * Menu.mm
 *
 *  Created on: Oct 11, 2024
 *      Author: lion
 */

#if defined(OS_DESKTOP)

#include <Cocoa/Cocoa.h>

#include "object.h"
#include "Menu.h"
#include "EventInfo.h"

@interface MyMenuItem : NSMenuItem

@property (nonatomic, assign) fibjs::MenuItem* item;

- (instancetype)initWithTitle:(NSString*)title action:(SEL)action keyEquivalent:(NSString*)keyEquivalent item:(fibjs::MenuItem*)item;

@end

@implementation MyMenuItem

- (instancetype)initWithTitle:(NSString*)title action:(SEL)action keyEquivalent:(NSString*)keyEquivalent item:(fibjs::MenuItem*)item
{
    self = [super initWithTitle:title action:action keyEquivalent:keyEquivalent];
    if (self) {
        _item = item;
    }
    return self;
}

@end

@interface MenuEventHandler : NSObject
@end

@implementation MenuEventHandler

- (void)onMenuItemClick:(id)sender
{
    MyMenuItem* menuItem = (MyMenuItem*)sender;
    fibjs::MenuItem* item = menuItem.item;

    if (item->checked.has_value()) {
        item->checked = !item->checked.value();
        [menuItem setState:item->checked.value() ? NSControlStateValueOn : NSControlStateValueOff];
    }

    fibjs::obj_ptr<fibjs::EventInfo> ei = new fibjs::EventInfo(item, "click");
    item->_emit("click", ei);
}

@end

namespace fibjs {

void MenuItem::update_os_menu_item()
{
    MyMenuItem* nsMenuItem = (MyMenuItem*)m_menu_item;

    [nsMenuItem setTitle:[NSString stringWithUTF8String:label->c_str()]];
    [nsMenuItem setToolTip:[NSString stringWithUTF8String:tooltip->c_str()]];

    if (checked.has_value())
        [nsMenuItem setState:checked.value() ? NSControlStateValueOn : NSControlStateValueOff];

    [nsMenuItem setEnabled:enabled.value() ? YES : NO];
}

static NSImage* resizeIcon(NSData* imageData, CGFloat height)
{
    NSImage* image = [[NSImage alloc] initWithData:imageData];
    if (!image)
        return nil;

    NSSize newSize = NSMakeSize(height, height);
    NSImage* resizedImage = [[NSImage alloc] initWithSize:newSize];

    [resizedImage lockFocus];
    [[NSGraphicsContext currentContext] setImageInterpolation:NSImageInterpolationHigh];
    [image drawInRect:NSMakeRect(0, 0, newSize.width, newSize.height)
             fromRect:NSZeroRect
            operation:NSCompositingOperationCopy
             fraction:1.0];
    [resizedImage unlockFocus];

    [image release];
    return resizedImage;
}

static void* create_os_menu_recursive(Menu* menu, const char* title, MenuEventHandler* eventHandler, bool is_popup)
{
    NSMenu* nsMenu = [[NSMenu alloc] initWithTitle:[NSString stringWithUTF8String:title]];
    [nsMenu setAutoenablesItems:NO];

    for (MenuItem* item : menu->m_items) {
        MyMenuItem* nsMenuItem = nil;
        if (item->type.value() == "separator") {
            nsMenuItem = [NSMenuItem separatorItem];
        } else {
            nsMenuItem = [[MyMenuItem alloc] initWithTitle:[NSString stringWithUTF8String:item->label->c_str()]
                                                    action:@selector(onMenuItemClick:)
                                             keyEquivalent:@""
                                                      item:item];
            if (item->submenu.has_value()) {
                NSMenu* subMenu = (NSMenu*)create_os_menu_recursive(item->submenu.value(), item->label->c_str(), eventHandler, true);
                [nsMenuItem setSubmenu:subMenu];
            }

            [nsMenuItem setTarget:eventHandler];
            if (item->tooltip.has_value())
                [nsMenuItem setToolTip:[NSString stringWithUTF8String:item->tooltip.value().c_str()]];

            if (item->m_icon) {
                NSData* imageData = [NSData dataWithBytes:item->m_icon->data() length:item->m_icon->length()];

                NSFont* font = [NSFont systemFontOfSize:[NSFont systemFontSize]];
                CGFloat fontLineHeight = ([font ascender] + fabs([font descender]));
                CGFloat lineHeight = fontLineHeight * 1.2;

                NSImage* resizedImage = resizeIcon(imageData, lineHeight);
                [nsMenuItem setImage:resizedImage];
                [resizedImage release];
            }

            if (item->checked.has_value())
                [nsMenuItem setState:item->checked.value() ? NSControlStateValueOn : NSControlStateValueOff];

            [nsMenuItem setEnabled:item->enabled.value() ? YES : NO];
        }

        [nsMenu addItem:nsMenuItem];

        [nsMenuItem retain];
        item->m_menu_item = nsMenuItem;
    }

    [nsMenu retain];
    menu->m_menu = nsMenu;

    return nsMenu;
}

void* Menu::create_os_menu(bool is_popup)
{
    if (m_menu)
        return m_menu;

    MenuEventHandler* eventHandler = [MenuEventHandler alloc];
    return create_os_menu_recursive(this, "main", eventHandler, is_popup);
}

static void release_os_menu_recursive(Menu* menu)
{
    for (MenuItem* item : menu->m_items) {
        if (item->submenu.has_value()) {
            release_os_menu_recursive(item->submenu.value());
        }
        if (item->m_menu_item) {
            [(MyMenuItem*)item->m_menu_item release];
            item->m_menu_item = nullptr;
        }
    }

    if (menu->m_menu) {
        [(NSMenu*)menu->m_menu release];
        menu->m_menu = nullptr;
    }
}

void Menu::release_os_menu()
{
    release_os_menu_recursive(this);
}

}

#endif
