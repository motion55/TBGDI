/* ********************************************************************** */
/*                                                                        */
/*   DRIVER.H                                                             */
/*                                                                        */
/* ********************************************************************** */

#ifndef __DRIVER_H__
#define __DRIVER_H__

/* -- HEADER FILES ------------------------------------------------------ */

#include <windows.h>
#include "global.h"
#include "bitmap.h"
#include "grid.h"
#include "circle.h"
#include "cursor.h"
#include "line.h"
#include "ntvdm.h"
#include "rectngle.h"
#include "resolutn.h"
#include "state.h"
#include "text.h"
#include "video.h"

/* -- PRAGMAS ----------------------------------------------------------- */

/* -- MACROS ------------------------------------------------------------ */

/* -- TYPE DEFINITIONS -------------------------------------------------- */

/* -- EXTERNAL DATA DECLARATIONS ---------------------------------------- */

/* -- PUBLIC DATA DEFINITIONS ------------------------------------------- */

#define DEF_IDLECOUNT	500

extern int IdleCounter;

extern HINSTANCE hInstance;

/* -- EXTERNAL FUNCTION PROTOTYPES -------------------------------------- */

/* -- PUBLIC FUNCTION PROTOTYPES ---------------------------------------- */

#endif

/* -- END --------------------------------------------------------------- */
