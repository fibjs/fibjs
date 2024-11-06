#include <exlib/include/osconfig.h>

#ifdef iPhone

#import <Foundation/Foundation.h>

extern "C" const char* get_documents_directory()
{
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    return [documentsDirectory UTF8String];
}

#endif