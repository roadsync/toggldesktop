//
//  AutoComleteInput.h
//  LiteComplete
//
//  Created by Indrek Vändrik on 20/02/2018.
//  Copyright © 2018 Toggl. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "UndoTextField.h"

@class AutoCompleteTable;
@class AutoCompleteTableContainer;
@class AutocompleteItem;

typedef NS_ENUM (NSUInteger, AutoCompleteDisplayMode)
{
	AutoCompleteDisplayModeCompact,
	AutoCompleteDisplayModeFullscreen,
};

@protocol AutoCompleteInputDelegate <NSObject>

- (void)autoCompleteInputShouldResetFilter;

@end

@interface AutoCompleteInput : UndoTextField
@property (weak, nonatomic) id<AutoCompleteInputDelegate> inputDelegate;
@property (strong, nonatomic, readonly) AutoCompleteTable *autocompleteTableView;
@property (strong, nonatomic, readonly) AutoCompleteTableContainer *autocompleteTableContainer;
@property (assign, nonatomic, readonly) CGFloat itemHeight;
@property (assign, nonatomic, readonly) CGFloat worksapceItemHeight;
@property (assign, nonatomic) AutoCompleteDisplayMode displayMode;
@property (nonatomic, readonly) BOOL autocompleteHidden;

- (void)toggleTableViewWithNumberOfItem:(NSInteger)numberOfItem;
- (void)updateDropdownWithHeight:(CGFloat)height;
- (void)setPos:(int)posy;
- (void)hide;
- (void)resetTable;
- (void)showAutoComplete:(BOOL)show;
- (void)setButton:(NSButton *)button;
- (void)reloadAutocomplete:(NSArray<AutocompleteItem *> *)array;
- (CGFloat)calculateTotalHeightFromArray:(NSArray<AutocompleteItem *> *)array;
@end
