/* -*-C-*-
 *
 * $Revision: 1.3.6.5 $
 *   $Author: lmacgreg $
 *     $Date: 2001/08/24 12:57:50 $
 *
 * Copyright (c) 1999 - 2001  ARM Limited
 * All Rights Reserved
 *
 * armul_hostif.h - library for armulator and peripherals to access hostif
 *                  functions through.
 */
#ifndef armul_hostif__h
#define armul_hostif__h
#include "rdi_hif.h"

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

/* opaque */ struct RDI_HostosInterface;

void Hostif_DebugPrint(const struct RDI_HostosInterface *hostif,
                       const char *format, ...);
void Hostif_ConsolePrint(const struct RDI_HostosInterface *hostif,
                         const char *format, ...);
void Hostif_DebugPause(const struct RDI_HostosInterface *hostif);

int Hostif_ConsoleReadC(const struct RDI_HostosInterface *hostif);

void Hostif_ConsoleWriteC(const struct RDI_HostosInterface *hostif, int c);

char *Hostif_ConsoleRead(const struct RDI_HostosInterface *hostif,
                         char *buffer, int len);
int Hostif_ConsoleWrite(const struct RDI_HostosInterface *hostif,
                        const char *buffer, int len);
void Hostif_RaiseError(const struct RDI_HostosInterface *hostif,
                       const char *format, ...);

struct hashblk;

void Hostif_PrettyPuts(const struct RDI_HostosInterface *hostif,
                       struct hashblk * /*toolconf*/ config,
                       const char *buf);

void Hostif_PrettyPrint(const struct RDI_HostosInterface *hostif,
                        struct hashblk * /*toolconf*/ config,
                        const char *format, ...);


/* Display a message box of some kind. (Refer to rdi_hif.h). The message
 * must be less than 32 characters for a RDI_MB_STATUS message
 */
#if RDI_VERSION >= 151
extern RDI_Hif_UserMessageReturn ARMul_UserMessage_i(
    const struct RDI_HostosInterface *hostif,
    uint32 type, char const *format, ...);
extern RDI_Hif_UserMessageReturn ARMul_UserMessage(
    const struct RDI_HostosInterface *hosti
  , uint32 type, char const *format, ...);
#endif

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /* def armul_hostif__h */



