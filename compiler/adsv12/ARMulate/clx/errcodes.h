/*
 * ARMulator : errcodes.h
 * Copyright (C) ARM Limited, 1999. All rights reserved.
 *
 * RCS $Revision: 1.1 $
 * Checkin $Date: 1999/11/27 17:21:00 $
 * Revising $Author: dsinclai $
 */

#ifndef errcodes__h
#define errcodes__h 1


/*
 * These numbers just happen to be the same as some
 * similar RDIErrors.
 */
typedef enum
{
    ErrorCode_NoError = 0,
    ErrorCode_OutOfStore = 154,
    ErrorCode_NotInitialised = 128,
    ErrorCode_UnableToInitialise = 129
} ErrorCode;


#endif
