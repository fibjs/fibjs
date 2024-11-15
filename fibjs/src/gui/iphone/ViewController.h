

#import <UIKit/UIKit.h>

@interface ViewController : UIViewController <UITableViewDataSource>
@property (strong, nonatomic) UIView* webViewContainer;
@property (strong, nonatomic) UITableView* consoleTableView;
@property (nonatomic) NSUInteger maxConsoleLines;
@property (strong, nonatomic) NSMutableArray<NSAttributedString*>* consoleLines;
@property (nonatomic) NSUInteger currentLine;
@property (strong, nonatomic) NSTimer* timer;
@end
