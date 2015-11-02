//
//  LLOsxglView.h
//  osxgl
//
//  Created by gayton on 10/23/15.
//  Copyright © 2015 lindenlab. All rights reserved.
//

#import <Cocoa/Cocoa.h>

class LLCEFLib;

@interface LLOsxglView : NSView {
    NSOpenGLContext *_openGLContext;
    LLCEFLib *_llCefLib;
    NSTimer *_timer;
    BOOL _needsShutdown;
}

@property (retain) NSOpenGLContext *openGLContext;
@property (assign) LLCEFLib *llCefLib;
@property (retain) NSTimer *timer;
@property (assign) BOOL needsShutdown;

+ (LLOsxglView *)current;

- (void)onPageChangedCallbackPixels:(unsigned char *)pixels width:(int)width height:(int)height;

@end
