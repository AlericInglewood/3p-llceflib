//
//  AppDelegate.h
//  osxgl
//
//  Created by gayton on 10/23/15.
//  Copyright © 2015 lindenlab. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate> {
    NSWindow *_window;
}

@property (weak) IBOutlet NSWindow *window;

@end

