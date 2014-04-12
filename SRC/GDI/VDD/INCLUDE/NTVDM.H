/* ********************************************************************** */
/*                                                                        */
/*   NTVDM.H                                                              */
/*                                                                        */
/* ********************************************************************** */

#ifndef __NTVDM_H__
#define __NTVDM_H__

/* -- HEADER FILES ------------------------------------------------------ */

#include <windows.h>
#include "global.h"

/* -- PRAGMAS ----------------------------------------------------------- */

/* -- MACROS ------------------------------------------------------------ */

/* -- TYPE DEFINITIONS -------------------------------------------------- */

typedef enum {
    VDM_V86,
    VDM_PM
} VDM_MODE;

/* -- EXTERNAL DATA DECLARATIONS ---------------------------------------- */

/* -- PUBLIC DATA DEFINITIONS ------------------------------------------- */

/* -- EXTERNAL FUNCTION PROTOTYPES -------------------------------------- */

extern PVOID WINAPI VdmMapFlat(USHORT selector, ULONG offset, VDM_MODE mode);
extern ULONG WINAPI getEAX(VOID);
extern USHORT getAX(VOID);
extern UCHAR WINAPI getAL(VOID);
extern UCHAR WINAPI getAH(VOID);
extern ULONG WINAPI getEBX(VOID);
extern USHORT WINAPI getBX(VOID);
extern UCHAR WINAPI getBL(VOID);
extern UCHAR WINAPI getBH(VOID);
extern ULONG WINAPI getECX(VOID);
extern USHORT WINAPI getCX(VOID);
extern UCHAR WINAPI getCL(VOID);
extern UCHAR WINAPI getCH(VOID);
extern ULONG WINAPI getEDX(VOID);
extern USHORT WINAPI getDX(VOID);
extern UCHAR WINAPI getDL(VOID);
extern UCHAR WINAPI getDH(VOID);
extern ULONG WINAPI getESP(VOID);
extern USHORT WINAPI getSP(VOID);
extern ULONG WINAPI getEBP(VOID);
extern USHORT WINAPI getBP(VOID);
extern ULONG WINAPI getESI(VOID);
extern USHORT WINAPI getSI(VOID);
extern ULONG WINAPI getEDI(VOID);
extern USHORT WINAPI getDI(VOID);
extern ULONG WINAPI getEIP(VOID);
extern USHORT WINAPI getIP(VOID);
extern USHORT WINAPI getCS(VOID);
extern USHORT WINAPI getSS(VOID);
extern USHORT WINAPI getDS(VOID);
extern USHORT WINAPI getES(VOID);
extern USHORT WINAPI getFS(VOID);
extern USHORT WINAPI getGS(VOID);
extern ULONG WINAPI getCF(VOID);
extern ULONG WINAPI getPF(VOID);
extern ULONG WINAPI getAF(VOID);
extern ULONG WINAPI getZF(VOID);
extern ULONG WINAPI getSF(VOID);
extern ULONG WINAPI getIF(VOID);
extern ULONG WINAPI getDF(VOID);
extern ULONG WINAPI getOF(VOID);
extern USHORT WINAPI getMSW(VOID);
extern VOID WINAPI setEAX(ULONG);
extern VOID WINAPI setAX(USHORT);
extern VOID WINAPI setAH(UCHAR);
extern VOID WINAPI setAL(UCHAR);
extern VOID WINAPI setEBX(ULONG);
extern VOID WINAPI setBX(USHORT);
extern VOID WINAPI setBH(UCHAR);
extern VOID WINAPI setBL(UCHAR);
extern VOID WINAPI setECX(ULONG);
extern VOID WINAPI setCX(USHORT);
extern VOID WINAPI setCH(UCHAR);
extern VOID WINAPI setCL(UCHAR);
extern VOID WINAPI setEDX(ULONG);
extern VOID WINAPI setDX(USHORT);
extern VOID WINAPI setDH(UCHAR);
extern VOID WINAPI setDL(UCHAR);
extern VOID WINAPI setESP(ULONG);
extern VOID WINAPI setSP(USHORT);
extern VOID WINAPI setEBP(ULONG);
extern VOID WINAPI setBP(USHORT);
extern VOID WINAPI setESI(ULONG);
extern VOID WINAPI setSI(USHORT);
extern VOID WINAPI setEDI(ULONG);
extern VOID WINAPI setDI(USHORT);
extern VOID WINAPI setEIP(ULONG);
extern VOID WINAPI setIP(USHORT);
extern VOID WINAPI setCS(USHORT);
extern VOID WINAPI setSS(USHORT);
extern VOID WINAPI setDS(USHORT);
extern VOID WINAPI setES(USHORT);
extern VOID WINAPI setFS(USHORT);
extern VOID WINAPI setGS(USHORT);
extern VOID WINAPI setCF(ULONG);
extern VOID WINAPI setPF(ULONG);
extern VOID WINAPI setAF(ULONG);
extern VOID WINAPI setZF(ULONG);
extern VOID WINAPI setSF(ULONG);
extern VOID WINAPI setIF(ULONG);
extern VOID WINAPI setDF(ULONG);
extern VOID WINAPI setOF(ULONG);
extern VOID WINAPI setMSW(USHORT);

#endif

/* -- END --------------------------------------------------------------- */
