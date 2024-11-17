/*
 * ViewController.mm
 *
 *  Created on: Nov 11, 2024
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#ifdef iPhone

#import <UIKit/UIKit.h>
#import <WebKit/WebKit.h>

#import "ViewController.h"

@interface CustomTableViewCell : UITableViewCell
@property (nonatomic, strong) UILabel* customLabel;
@end

@implementation CustomTableViewCell

- (instancetype)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString*)reuseIdentifier
{
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
        self.customLabel = [[UILabel alloc] initWithFrame:self.contentView.bounds];
        self.customLabel.numberOfLines = 0;
        self.customLabel.translatesAutoresizingMaskIntoConstraints = NO;
        [self.contentView addSubview:self.customLabel];

        [NSLayoutConstraint activateConstraints:@[
            [self.customLabel.topAnchor constraintEqualToAnchor:self.contentView.topAnchor],
            [self.customLabel.leadingAnchor constraintEqualToAnchor:self.contentView.leadingAnchor],
            [self.customLabel.bottomAnchor constraintEqualToAnchor:self.contentView.bottomAnchor],
            [self.customLabel.trailingAnchor constraintEqualToAnchor:self.contentView.trailingAnchor]
        ]];
    }
    return self;
}

@end

@implementation ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];

    self.maxConsoleLines = 200;
    self.consoleLines = [NSMutableArray array];
    self.currentLine = 0;

    self.webViewContainer = [[UIView alloc] initWithFrame:self.view.bounds];
    self.webViewContainer.hidden = YES;
    [self.view addSubview:self.webViewContainer];

    self.consoleTableView = [[UITableView alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
    self.consoleTableView.dataSource = self;
    self.consoleTableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    self.consoleTableView.allowsSelection = NO;
    [self.consoleTableView registerClass:[CustomTableViewCell class] forCellReuseIdentifier:@"ConsoleCell"];
    [self.view addSubview:self.consoleTableView];
}

- (void)viewDidLayoutSubviews
{
    [super viewDidLayoutSubviews];

    if (self.webViewContainer.hidden) {
        self.consoleTableView.frame = self.view.bounds;
        self.webViewContainer.layer.borderWidth = 0;
    } else {
        if (self.consoleLines.count == 0) {
            self.webViewContainer.frame = self.view.bounds;
            self.consoleTableView.frame = CGRectMake(0, 0, 0, 0);

            self.webViewContainer.layer.borderWidth = 0;
        } else {
            CGFloat webViewHeight = self.view.bounds.size.height * 2 / 3;
            CGFloat consoleHeight = self.view.bounds.size.height / 3;

            self.webViewContainer.frame = CGRectMake(0, 0, self.view.bounds.size.width, webViewHeight);

            self.consoleTableView.frame = CGRectMake(0, webViewHeight, self.view.bounds.size.width, consoleHeight);

            self.webViewContainer.layer.borderWidth = 0.3;
            self.webViewContainer.layer.borderColor = [UIColor grayColor].CGColor;
        }
    }

    for (UIView* subview in self.webViewContainer.subviews) {
        subview.frame = self.webViewContainer.bounds;
    }
}

- (void)appendToConsole:(NSAttributedString*)attributedText
{
    [self.consoleLines addObject:attributedText];

    if (self.consoleLines.count > self.maxConsoleLines) {
        [self.consoleLines removeObjectAtIndex:0];
        NSIndexPath* indexPathToDelete = [NSIndexPath indexPathForRow:0 inSection:0];
        [self.consoleTableView deleteRowsAtIndexPaths:@[ indexPathToDelete ] withRowAnimation:UITableViewRowAnimationNone];
    }

    NSIndexPath* indexPathToInsert = [NSIndexPath indexPathForRow:self.consoleLines.count - 1 inSection:0];
    [self.consoleTableView insertRowsAtIndexPaths:@[ indexPathToInsert ] withRowAnimation:UITableViewRowAnimationNone];

    if (self.consoleLines.count == 1) {
        [self.view setNeedsLayout];
    }

    [self.consoleTableView scrollToRowAtIndexPath:indexPathToInsert atScrollPosition:UITableViewScrollPositionBottom animated:NO];
}

- (void)addWebView:(WKWebView*)webView
{
    if (self.webViewContainer.hidden) {
        self.webViewContainer.hidden = NO;
        [self.view setNeedsLayout];
    }

    [webView setFrame:self.webViewContainer.bounds];
    [self.webViewContainer addSubview:webView];
}

- (void)removeWebView:(WKWebView*)webView
{
    [webView stopLoading];
    [webView removeFromSuperview];
    if (self.webViewContainer.subviews.count == 0) {
        self.webViewContainer.hidden = YES;
        [self.view setNeedsLayout];
    }
}

#pragma mark - UITableViewDataSource

- (NSInteger)tableView:(UITableView*)tableView numberOfRowsInSection:(NSInteger)section
{
    return self.consoleLines.count;
}

- (UITableViewCell*)tableView:(UITableView*)tableView cellForRowAtIndexPath:(NSIndexPath*)indexPath
{
    static NSString* cellIdentifier = @"ConsoleCell";
    CustomTableViewCell* cell = [tableView dequeueReusableCellWithIdentifier:cellIdentifier forIndexPath:indexPath];
    if (!cell) {
        cell = [[CustomTableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:cellIdentifier];
    }

    NSAttributedString* attributedText = self.consoleLines[indexPath.row];
    cell.customLabel.attributedText = attributedText;

    return cell;
}

@end

#endif
