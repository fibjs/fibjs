/*
 * ANSIString.cpp
 *
 *  Created on: Nov 11, 2024
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#ifdef iPhone

#import "ANSIString.h"
#import <UIKit/UIKit.h>

@implementation ANSIString

static NSInteger findNext(NSString *string, NSString *needle, NSInteger start)
{
    if(start >= [string length])
    {
        return NSNotFound;
    }
    else
    {
        NSInteger i = [string rangeOfString:needle options:0
                                      range:NSMakeRange(start, [string length] - start)].location;
        return i;
    }
}

static UIFont *defaultFont(void)
{
    return [UIFont monospacedSystemFontOfSize:12 weight:UIFontWeightRegular];
}

static UIFont *boldFont(void)
{
    return [UIFont monospacedSystemFontOfSize:12 weight:UIFontWeightBold];
}

static UIColor *ansiColor(int c)
{
    float a = 1.0;
    float r = 0.0;
    float g = 0.0;
    float b = 0.0;
    if(c & 1)
        r = 0.5;
    if(c & 2)
        g = 0.5;
    if(c & 4)
        b = 0.5;

    return [UIColor colorWithRed:r green:g blue:b alpha:a];
}

static NSDictionary *attributes(bool bold, bool blink, bool reverse, int bg, int fg, bool isReset)
{
    NSMutableDictionary *attrs = [NSMutableDictionary dictionary];

    // 设置字体
    attrs[NSFontAttributeName] = bold ? boldFont() : defaultFont();

    // 设置颜色
    if (!isReset) {
        if (fg != -1) {
            attrs[NSForegroundColorAttributeName] = ansiColor(fg);
        }
        if (bg != -1) {
            attrs[NSBackgroundColorAttributeName] = ansiColor(bg);
        }
    }

    return [attrs copy];
}

+ (NSAttributedString *)parseColorCodes:(NSString *)ansiString
{
    NSMutableAttributedString *retval = [[NSMutableAttributedString alloc] initWithString:@""];
    NSInteger length = (NSInteger)[ansiString length];
    NSInteger last = 0;
    NSInteger current = -1;

    static const int DefaultFGColor = -1; // Use system default color
    static const int DefaultBGColor = -1; // Use transparent background
    bool bold = false;
    bool blink = false;
    bool reverse = false;
    bool isReset = true;
    int bgColor = DefaultBGColor;
    int fgColor = DefaultFGColor;

    while((current = findNext(ansiString, @"\e[", current + 1)) != NSNotFound)
    {
        NSInteger updateLength = current - last + 1;
        NSAttributedString *update = [[NSAttributedString alloc]
          initWithString:[ansiString substringWithRange:NSMakeRange(last, updateLength)]
          attributes:attributes(bold, blink, reverse, bgColor, fgColor, isReset)];
        [retval appendAttributedString:update];
        last = current;

        NSInteger end = findNext(ansiString, @"m", current + 1);
        if(end == NSNotFound)
        {
            current = length;
        }
        else
        {
            bool valid = true;
            NSInteger i;
            for(i = current + 2; valid && (i < end); ++i)
            {
                unichar c = [ansiString characterAtIndex:i];
                switch(c)
                {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case ';':
                    break;
                default:
                    valid = false;
                }
            }
            if(valid)
            {
                NSArray *codes = [[ansiString substringWithRange:
                                  NSMakeRange(current + 2, end - current - 2)]
                                  componentsSeparatedByString:@";"];
                for(i = 0; i < [codes count]; ++i)
                {
                    int c = [[codes objectAtIndex:i] intValue];
                    switch(c)
                    {
                    case 0:
                        bold = false;
                        blink = false;
                        reverse = false;
                        isReset = true;
                        bgColor = DefaultBGColor;
                        fgColor = DefaultFGColor;
                        break;
                    case 1:
                        bold = true;
                        isReset = false;
                        break;
                    case 5:
                        blink = true;
                        isReset = false;
                        break;
                    case 7:
                        reverse = true;
                        isReset = false;
                        break;
                    case 30:
                    case 31:
                    case 32:
                    case 33:
                    case 34:
                    case 35:
                    case 36:
                    case 37:
                        fgColor = c - 30;
                        isReset = false;
                        break;
                    case 40:
                    case 41:
                    case 42:
                    case 43:
                    case 44:
                    case 45:
                    case 46:
                    case 47:
                        bgColor = c - 40;
                        isReset = false;
                        break;
                    }
                }

                NSInteger codeLength = end - current + 1;
                current += codeLength - 1;
                last = current + 1;
            }
        }
    }

    if(last < length)
    {
        NSAttributedString *rest = [[NSAttributedString alloc]
              initWithString:[ansiString substringFromIndex:last]
              attributes:attributes(bold, blink, reverse, bgColor, fgColor, isReset)];
        [retval appendAttributedString:rest];
    }

    return retval;
}

@end

#endif