/*
 * dialog.mm
 *
 *  Created on: Oct 29, 2024
 *      Author: lion
 */

#if defined(OS_DESKTOP)

#include <Cocoa/Cocoa.h>

#include "object.h"
#include "ifs/gui.h"
#include "../gui.h"

extern int32_t s_window_count;

namespace fibjs {

result_t gui_base::chooseFile(v8::Local<v8::Object> options, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        start_gui();

        Isolate* isolate = Isolate::current(options);

        obj_ptr<DialogOptions> opts;
        result_t hr = DialogOptions::load(isolate, options, opts);
        if (hr < 0)
            return hr;

        ac->m_ctx.resize(1);
        ac->m_ctx[0] = opts;

        return CHECK_ERROR(CALL_E_GUICALL);
    }

    DialogOptions* opts = (DialogOptions*)ac->m_ctx[0].object();

    bool isSaveDialog = opts->type.value() == "saveFile";

    NSPanel* panel = isSaveDialog ? [NSSavePanel savePanel] : [NSOpenPanel openPanel];

    if (opts->title.has_value()) {
        [panel setTitle:[NSString stringWithUTF8String:opts->title->c_str()]];
    }

    if (opts->defaultPath.has_value()) {
        [panel setDirectoryURL:[NSURL fileURLWithPath:[NSString stringWithUTF8String:opts->defaultPath->c_str()]]];
    }

    NSView* accessoryView = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 200, 25)];
    NSPopUpButton* formatButton = [[NSPopUpButton alloc] initWithFrame:NSMakeRect(0, 0, 200, 25)];

    NSMutableArray* allowedTypes = [NSMutableArray array];
    if (opts->filters.has_value()) {
        for (int32_t i = 0; i < opts->filters->size(); i++) {
            auto& filter = opts->filters->at(i);
            [formatButton addItemWithTitle:[NSString stringWithUTF8String:filter->name.c_str()]];
            if (i == 0) {
                for (auto& ext : filter->extensions) {
                    if (ext == "*") {
                        allowedTypes = nil; // Allow all file types
                        break;
                    } else {
                        [allowedTypes addObject:[NSString stringWithUTF8String:ext.c_str()]];
                    }
                }
            }
        }
    }

    [accessoryView addSubview:formatButton];
    [panel setAccessoryView:accessoryView];

    if (!isSaveDialog) {
        if (opts->type.value() == "openDirectory") {
            [panel setCanChooseFiles:NO];
            [panel setCanChooseDirectories:YES];
        } else if (opts->type.value() == "openFile") {
            [panel setCanChooseFiles:YES];
            [panel setCanChooseDirectories:NO];
        } else
            return Runtime::setError("invalid dialog type: " + opts->type.value());

        [panel setAllowsMultipleSelection:opts->multiSelections.value() ? YES : NO];
        [panel setAccessoryViewDisclosed:YES];
    }

    [panel setAllowedFileTypes:allowedTypes];

    void (^formatChanged)(id) = ^(id sender) {
        NSPopUpButton* formatButton = (NSPopUpButton*)sender;
        NSString* selectedFormat = [[formatButton selectedItem] title];

        NSMutableArray* newAllowedTypes = [NSMutableArray array];
        for (auto& filter : *opts->filters) {
            if ([selectedFormat isEqualToString:[NSString stringWithUTF8String:filter->name.c_str()]]) {
                for (auto& ext : filter->extensions) {
                    if (ext == "*") {
                        newAllowedTypes = nil; // Allow all file types
                        break;
                    } else {
                        [newAllowedTypes addObject:[NSString stringWithUTF8String:ext.c_str()]];
                    }
                }
                break;
            }
        }

        [panel setAllowedFileTypes:newAllowedTypes];
    };

    id target = [NSBlockOperation blockOperationWithBlock:^{
        formatChanged(formatButton);
    }];

    [formatButton setAction:@selector(main)];
    [formatButton setTarget:target];

    if (++s_window_count == 1)
        [[NSApplication sharedApplication] setActivationPolicy:NSApplicationActivationPolicyRegular];
    [[NSRunningApplication currentApplication] activateWithOptions:NSApplicationActivateIgnoringOtherApps];

    NSModalResponse res = [panel runModal];

    if (--s_window_count == 0)
        [[NSApplication sharedApplication] setActivationPolicy:NSApplicationActivationPolicyAccessory];

    if (res == NSModalResponseOK) {
        retVal = new NArray();

        if (isSaveDialog) {
            retVal->append([[[panel URL] path] UTF8String]);
        } else {
            NSArray* urls = [panel URLs];
            for (NSURL* url in urls)
                retVal->append([[url path] UTF8String]);
        }

        return 0;
    }

    return CALL_RETURN_UNDEFINED;
}

}

#endif
