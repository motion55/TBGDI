/* ********************************************************************** */
/*                                                                        */
/*   MEMORY.H                                                             */
/*                                                                        */
/* ********************************************************************** */

#ifndef __MEMORY_H__
#define __MEMORY_H__

/* -- HEADER FILES ------------------------------------------------------ */

#include "global.h"
#include "driver.h"

/* -- TYPE DEFINITIONS -------------------------------------------------- */

struct XMSExtendedMemoryMoveStructure {
	unsigned long Length;
	unsigned short SourceHandle;
	unsigned long SourceOffset;
	unsigned short DestinationHandle;
	unsigned long DestinationOffset;
};

/* -- PUBLIC DATA DECLARATIONS ------------------------------------------ */

extern unsigned short FrameBufferHandle;
extern unsigned char far *TransferBuffer;

/* -- PUBLIC FUNCTION PROTOTYPES ---------------------------------------- */

extern bool InitMemory(void);
extern void ShutdownMemory(void);
extern unsigned char AllocMemory(unsigned short size,
	void far * far *memoryBlock);
extern void FreeMemory(void far *memoryBlock);
extern unsigned short AllocExtendedMemory(unsigned short size);
extern void FreeExtendedMemory(unsigned short handle);
extern void MoveExtendedMemory(unsigned short sourceHandle,
	unsigned long sourceOffset, unsigned short destinationHandle,
	unsigned long destinationOffset);

#endif

/* -- END --------------------------------------------------------------- */
