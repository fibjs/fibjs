/*
 * Tray.mm
 *
 *  Created on: Oct 12, 2024
 *      Author: lion
 */

#if defined(OS_DESKTOP)

#include <Cocoa/Cocoa.h>

#include "object.h"
#include "ifs/gui.h"
#include "Tray.h"
#include "EventInfo.h"

@interface TrayEventHandler : NSObject
@property (nonatomic, assign) fibjs::Tray* tray;
@end

@implementation TrayEventHandler

- (instancetype)initWithTray:(fibjs::Tray*)tray
{
    self = [super init];
    if (self) {
        _tray = tray;
    }
    return self;
}

- (void)onClick:(id)sender
{
    fibjs::obj_ptr<fibjs::EventInfo> ei = new fibjs::EventInfo(_tray, "click");
    _tray->_emit("click", ei);
}

@end

namespace fibjs {

void Tray::createTray()
{
    NSImage* icon = [[NSImage alloc]
        initWithData:[NSData dataWithBytes:m_icon->data() length:m_icon->length()]];

    NSStatusBar* bar = [NSStatusBar systemStatusBar];
    NSStatusItem* tray = [bar statusItemWithLength:NSVariableStatusItemLength];
    m_tray = tray;

    [tray retain];
    [tray setEnabled:YES];

    if (icon) {
        CGFloat statusBarHeight = [bar thickness];
        NSSize iconSize = [icon size];
        CGFloat aspectRatio = iconSize.width / iconSize.height;
        NSSize newSize = NSMakeSize(statusBarHeight * aspectRatio, statusBarHeight);

        [icon setScalesWhenResized:YES];
        [icon setSize:newSize];

        [tray setImage:icon];
        [icon release];
    }

    if (m_options->title.has_value())
        [tray setTitle:[NSString stringWithUTF8String:m_options->title.value().c_str()]];

    if (m_options->tooltip.has_value())
        [tray setToolTip:[NSString stringWithUTF8String:m_options->tooltip.value().c_str()]];

    TrayEventHandler* eventHandler = [[TrayEventHandler alloc] initWithTray:this];

    [tray setTarget:eventHandler];
    [tray setAction:@selector(onClick:)];

    if (m_options->menu.has_value()) {
        NSMenu* menu = (NSMenu*)m_options->menu.value()->create_os_menu();
        [tray setMenu:menu];
    }
}

result_t Tray::close(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    [m_tray release];
    release();

    return 0;
}

} // namespace fibjs

#endif // OS_DESKTOP