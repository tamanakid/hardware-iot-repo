#ifndef _FILE_HANDLER_H
#define _FILE_HANDLER_H

#include "time.h"

#include "sketch.h"



String fileRead (String fileURI);

bool fileWrite (String fileURI, String contents);

bool fileWriteWithTimestamp (String fileURI, String contents, struct tm *state_time);

void fileSetup(String *state_filename, String file_suffix, struct tm *state_time);


#endif // _FILE_HANDLER_H
