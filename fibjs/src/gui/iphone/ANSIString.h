

#import <UIKit/UIKit.h>

@interface ANSIString : NSObject

+ (NSAttributedString *)parseColorCodes:(NSString *)ansiString;

@end
