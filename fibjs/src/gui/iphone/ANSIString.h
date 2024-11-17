/*
 * ANSIString.h
 *
 *  Created on: Nov 11, 2024
 *      Author: lion
 */

#import <UIKit/UIKit.h>

@interface ANSIString : NSObject

+ (NSAttributedString *)parseColorCodes:(NSString *)ansiString;

@end
