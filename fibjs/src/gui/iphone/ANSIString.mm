/*
 * ANSIString.mm
 *
 *  Created on: Nov 11, 2024
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#ifdef iPhone

#import "ANSIString.h"
#import <UIKit/UIKit.h>

@implementation ANSIString

+ (NSAttributedString*)parseColorCodes:(NSString*)ansiString
{
    NSMutableAttributedString* retval = [[NSMutableAttributedString alloc] init];

    NSMutableDictionary* currentAttributes = [NSMutableDictionary dictionary];
    currentAttributes[NSFontAttributeName] = [UIFont fontWithName:@"Menlo" size:14.0];
    NSMutableString* currentText = [NSMutableString string];

    NSUInteger length = [ansiString length];
    NSUInteger i = 0;

    while (i < length) {
        unichar c = [ansiString characterAtIndex:i];

        if (c == '\033' && i + 1 < length && [ansiString characterAtIndex:i + 1] == '[') {
            if ([currentText length] > 0) {
                NSAttributedString* segment = [[NSAttributedString alloc] initWithString:currentText attributes:currentAttributes];
                [retval appendAttributedString:segment];
                [currentText setString:@""];
            }

            NSUInteger end = i + 2;
            while (end < length && [ansiString characterAtIndex:end] != 'm') {
                end++;
            }

            if (end < length) {
                NSString* codeStr = [ansiString substringWithRange:NSMakeRange(i + 2, end - (i + 2))];
                NSArray* codes = [codeStr componentsSeparatedByString:@";"];

                for (NSString* code in codes) {
                    NSInteger styleCode = [code integerValue];

                    if (styleCode == 0) {
                        [currentAttributes removeAllObjects];
                        currentAttributes[NSFontAttributeName] = [UIFont fontWithName:@"Menlo" size:14.0];
                    } else if (styleCode == 1) {
                        currentAttributes[NSFontAttributeName] = [UIFont fontWithName:@"Menlo-Bold" size:14.0];
                    } else if ((styleCode >= 30 && styleCode <= 37) || (styleCode >= 90 && styleCode <= 97)) {
                        UIColor* color;
                        switch (styleCode) {
                        case 30:
                            color = [UIColor colorWithRed:0.3 green:0.3 blue:0.3 alpha:1.0];
                            break;
                        case 90:
                            color = [UIColor darkGrayColor];
                            break;
                        case 31:
                            color = [UIColor colorWithRed:0.7 green:0.0 blue:0.0 alpha:1.0];
                            break;
                        case 91:
                            color = [UIColor redColor];
                            break;
                        case 32:
                            color = [UIColor colorWithRed:0.0 green:0.6 blue:0.0 alpha:1.0];
                            break;
                        case 92:
                            color = [UIColor greenColor];
                            break;
                        case 33:
                            color = [UIColor colorWithRed:0.7 green:0.5 blue:0.0 alpha:1.0];
                            break;
                        case 93:
                            color = [UIColor yellowColor];
                            break;
                        case 34:
                            color = [UIColor colorWithRed:0.0 green:0.0 blue:0.7 alpha:1.0];
                            break;
                        case 94:
                            color = [UIColor blueColor];
                            break;
                        case 35:
                            color = [UIColor colorWithRed:0.5 green:0.0 blue:0.5 alpha:1.0];
                            break;
                        case 95:
                            color = [UIColor purpleColor];
                            break;
                        case 36:
                            color = [UIColor colorWithRed:0.0 green:0.5 blue:0.5 alpha:1.0];
                            break;
                        case 96:
                            color = [UIColor cyanColor];
                            break;
                        case 37:
                            color = [UIColor colorWithRed:0.7 green:0.7 blue:0.7 alpha:1.0];
                            break;
                        case 97:
                            color = [UIColor whiteColor];
                            break;
                        default:
                            color = [UIColor blackColor];
                        }
                        currentAttributes[NSForegroundColorAttributeName] = color;
                    } else if ((styleCode >= 40 && styleCode <= 47) || (styleCode >= 100 && styleCode <= 107)) {
                        UIColor* bgColor;
                        switch (styleCode) {
                        case 40:
                            bgColor = [UIColor colorWithRed:0.3 green:0.3 blue:0.3 alpha:1.0];
                            break;
                        case 100:
                            bgColor = [UIColor darkGrayColor];
                            break;
                        case 41:
                            bgColor = [UIColor colorWithRed:0.7 green:0.0 blue:0.0 alpha:1.0];
                            break;
                        case 101:
                            bgColor = [UIColor redColor];
                            break;
                        case 42:
                            bgColor = [UIColor colorWithRed:0.0 green:0.6 blue:0.0 alpha:1.0];
                            break;
                        case 102:
                            bgColor = [UIColor greenColor];
                            break;
                        case 43:
                            bgColor = [UIColor colorWithRed:0.7 green:0.5 blue:0.0 alpha:1.0];
                            break;
                        case 103:
                            bgColor = [UIColor yellowColor];
                            break;
                        case 44:
                            bgColor = [UIColor colorWithRed:0.0 green:0.0 blue:0.7 alpha:1.0];
                            break;
                        case 104:
                            bgColor = [UIColor blueColor];
                            break;
                        case 45:
                            bgColor = [UIColor colorWithRed:0.5 green:0.0 blue:0.5 alpha:1.0];
                            break;
                        case 105:
                            bgColor = [UIColor purpleColor];
                            break;
                        case 46:
                            bgColor = [UIColor colorWithRed:0.0 green:0.5 blue:0.5 alpha:1.0];
                            break;
                        case 106:
                            bgColor = [UIColor cyanColor];
                            break;
                        case 47:
                            bgColor = [UIColor colorWithRed:0.7 green:0.7 blue:0.7 alpha:1.0];
                            break;
                        case 107:
                            bgColor = [UIColor whiteColor];
                            break;
                        default:
                            bgColor = [UIColor clearColor];
                        }
                        currentAttributes[NSBackgroundColorAttributeName] = bgColor;
                    }
                }
                i = end + 1;
                continue;
            }
        }

        [currentText appendString:[NSString stringWithCharacters:&c length:1]];
        i++;
    }

    if ([currentText length] > 0) {
        NSAttributedString* segment = [[NSAttributedString alloc] initWithString:currentText attributes:currentAttributes];
        [retval appendAttributedString:segment];
    }

    return retval;
}

@end

#endif