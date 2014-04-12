/* ********************************************************************** */
/*                                                                        */
/*   MEMORY.C                                                             */
/*                                                                        */
/* ********************************************************************** */

/* -- HEADER FILES ------------------------------------------------------ */

#include "memory.h"

/* -- PUBLIC DATA DEFINITIONS ------------------------------------------- */

unsigned short FrameBufferHandle = 0;
unsigned char far *TransferBuffer = NULL;

/* -- PRIVATE DATA DEFINITIONS ------------------------------------------ */

static bool ExtendedMemoryAvailable = false;
static bool FrameBufferAllocated = false;
static void (far *XMSControlFunction)(void);

/* -- PRIVATE FUNCTION PROTOTYPES --------------------------------------- */

unsigned char DOSAllocMemory(unsigned short size,
	void far * far *memoryBlock);
unsigned short DOSGetFreeMemory(void);
void DOSFreeMemory(void far * far *memoryBlock);
unsigned char XMSDriverInstalled(void);
void far *XMSGetControlFunction(void);
unsigned short XMSGetVersionNumber(void);
unsigned short XMSAllocExtendedMemory(unsigned short size);
void XMSFreeExtendedMemory(unsigned short handle);
void XMSMoveExtendedMemory(
	struct XMSExtendedMemoryMoveStructure far *moveStructure);

/* -- CODE -------------------------------------------------------------- */

/* ====================================================================== */
/*                                                                        */
/*   InitMemory                                                           */
/*                                                                        */
/* ====================================================================== */

bool InitMemory(void)
{
	long fbSize;

	if (FrameBufferAllocated == true) {
		return true;
	}

	if (ExtendedMemoryAvailable == false) {
		if (XMSDriverInstalled() != 0x80) {
			PrintString("No XMS Driver installed!\r\n");
			return false;
		}

		XMSControlFunction = XMSGetControlFunction();

		if (XMSGetVersionNumber() < 0x0200) {
			PrintString("XMS 2.0 or higher required!\r\n");
			return false;
		}

		ExtendedMemoryAvailable = true;
	}

	if (FrameBufferHandle == 0) {
		fbSize = (((long)ResolutionY * ResolutionX * 2) + 1023) / 1024;
		if ((FrameBufferHandle = AllocExtendedMemory(fbSize)) == 0) {
			PrintString("Not enough free extended memory to allocate frame buffer!\r\n");
			return false;
		}
	}

	if (TransferBuffer == NULL) {
		if ((AllocMemory((65536 / 16), (void far* far*)&TransferBuffer)) != 0) {
			PrintString("Not enough free conventional memory to allocate transfer buffer!\r\n");
			FreeExtendedMemory(FrameBufferHandle);
			return false;
		}
	}

	FrameBufferAllocated = true;
	return true;
}

/* ====================================================================== */
/*                                                                        */
/*   ShutdownMemory                                                       */
/*                                                                        */
/* ====================================================================== */

void ShutdownMemory()
{
	if (FrameBufferAllocated == false) {
		return;
	}

	FreeExtendedMemory(FrameBufferHandle);
	FreeMemory(TransferBuffer);

	FrameBufferHandle = 0;
	TransferBuffer = NULL;
	FrameBufferAllocated = false;
}

/* ====================================================================== */
/*                                                                        */
/*   AllocMemory                                                          */
/*                                                                        */
/* ====================================================================== */

unsigned char AllocMemory(unsigned short size, void far * far *memoryBlock)
{
	unsigned short freeMemory;
	unsigned char dosErrorCode;
	void far *dummyBlock;

	freeMemory = DOSGetFreeMemory();

	dosErrorCode = DOSAllocMemory((freeMemory - size - 2), &dummyBlock);
	if (dosErrorCode != 0) {
		return dosErrorCode;
	}

	dosErrorCode = DOSAllocMemory(size, memoryBlock);
	DOSFreeMemory(dummyBlock);
	return dosErrorCode;
}

/* ====================================================================== */
/*                                                                        */
/*   FreeMemory                                                           */
/*                                                                        */
/* ====================================================================== */

void FreeMemory(void far *memoryBlock)
{
	DOSFreeMemory(memoryBlock);
}

/* ====================================================================== */
/*                                                                        */
/*   AllocExtendedMemory                                                  */
/*                                                                        */
/* ====================================================================== */

unsigned short AllocExtendedMemory(unsigned short size)
{
	if (ExtendedMemoryAvailable == false) {
		return 0;
	}

	return XMSAllocExtendedMemory(size);
}

/* ====================================================================== */
/*                                                                        */
/*   FreeExtendedMemory                                                   */
/*                                                                        */
/* ====================================================================== */

void FreeExtendedMemory(unsigned short handle)
{
	if (ExtendedMemoryAvailable == false) {
		return;
	}

	XMSFreeExtendedMemory(handle);
}

/* ====================================================================== */
/*                                                                        */
/*   MoveExtendedMemory                                                   */
/*                                                                        */
/* ====================================================================== */

void MoveExtendedMemory(unsigned short sourceHandle,
	unsigned long sourceOffset, unsigned short destinationHandle,
	unsigned long destinationOffset)
{
	struct XMSExtendedMemoryMoveStructure moveStructure;

	if (ExtendedMemoryAvailable == false) {
		return;
	}

	moveStructure.Length = 65536L;
	moveStructure.SourceHandle = sourceHandle;
	moveStructure.SourceOffset = sourceOffset;
	moveStructure.DestinationHandle = destinationHandle;
	moveStructure.DestinationOffset = destinationOffset;
	XMSMoveExtendedMemory(&moveStructure);
}

/* ====================================================================== */
/*                                                                        */
/*   DOSAllocMemory                                                       */
/*                                                                        */
/* ====================================================================== */

#pragma aux DOSAllocMemory =		\
	"mov	ah, 48h"		\
	"int	21h"			\
	"jc	done"			\
	"mov	es:[di+2], ax"		\
	"xor	ax, ax"			\
	"mov	es:[di], ax"		\
	"done:"				\
	parm [bx] [es di]		\
	value [al]			\
	modify exact [ax bx es di];

/* ====================================================================== */
/*                                                                        */
/*   DOSGetFreeMemory                                                     */
/*                                                                        */
/* ====================================================================== */

#pragma aux DOSGetFreeMemory =		\
	"mov	ah, 48h"		\
	"mov	bx, 0ffffh"		\
	"int	21h"			\
	parm [] nomemory		\
	value [bx]			\
	modify exact [ax bx] nomemory;

/* ====================================================================== */
/*                                                                        */
/*   DOSFreeMemory                                                        */
/*                                                                        */
/* ====================================================================== */

#pragma aux DOSFreeMemory =		\
	"mov	ah, 49h"		\
	"int	21h"			\
	parm [es di]			\
	value [ax]			\
	modify exact [ax es di] nomemory;

/* ====================================================================== */
/*                                                                        */
/*   XMSDriverInstalled                                                   */
/*                                                                        */
/* ====================================================================== */

#pragma aux XMSDriverInstalled =		\
	"mov	ax, 4300h"			\
	"int	2fh"	 			\
	parm [] nomemory 			\
	value [al]
//	modify exact [ax] nomemory;

/* ====================================================================== */
/*                                                                        */
/*   XMSGetControlFunction                                                */
/*                                                                        */
/* ====================================================================== */

#pragma aux XMSGetControlFunction =		\
	"mov	ax, 4310h"			\
	"int	2fh"	 			\
	parm [] nomemory 			\
	value [es bx]
//	modify exact [ax bx es] nomemory;

/* ====================================================================== */
/*                                                                        */
/*   XMSGetVersionNumber                                                  */
/*                                                                        */
/* ====================================================================== */

#pragma aux XMSGetVersionNumber =		\
	"mov	ah, 00h"			\
	"call	dword ptr [XMSControlFunction]"	\
	parm [] nomemory 			\
	value [ax]
//	modify exact [ax bx dx] nomemory;

/* ====================================================================== */
/*                                                                        */
/*   XMSAllocExtendedMemory                                               */
/*                                                                        */
/* ====================================================================== */

#pragma aux XMSAllocExtendedMemory =		\
	"mov	ah, 09h"			\
	"call	dword ptr [XMSControlFunction]"	\
	"mov	ax, dx"				\
	parm [dx]				\
	value [ax]
//	modify exact [ax bl dx];

/* ====================================================================== */
/*                                                                        */
/*   XMSFreeExtendedMemory                                                */
/*                                                                        */
/* ====================================================================== */

#pragma aux XMSFreeExtendedMemory =		\
	"mov	ah, 0ah"			\
	"call	dword ptr [XMSControlFunction]" \
	parm [dx] nomemory
//	modify exact [ax bl dx] nomemory;

/* ====================================================================== */
/*                                                                        */
/*   XMSMoveExtendedMemory	                                          */
/*                                                                        */
/* ====================================================================== */

#pragma aux XMSMoveExtendedMemory =		\
	"push	ds"				\
	"push	es"				\
	"pop	ds"				\
	"mov	ah, 0bh"			\
	"call	dword ptr cs:[XMSControlFunction]" \
	"pop	ds"				\
	parm [es si]				\
	modify exact [ax bl];

/* -- END --------------------------------------------------------------- */
