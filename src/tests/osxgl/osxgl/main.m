//
//  main.m
//  osxgl
//
//  Created by gayton on 10/23/15.
//  Copyright © 2015 lindenlab. All rights reserved.
//

#import <Cocoa/Cocoa.h>

int main(int argc, const char * argv[]) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    // Note that NSApplicationMain never returns.
    int result = NSApplicationMain(argc, argv);

    [pool drain];
    return result;
}
