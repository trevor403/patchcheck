#include <stdio.h>
#include <gccore.h>  // Wrapper to include common libogc headers
#include <ogcsys.h>  // Needed for console support
#include <ogc/dvd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include "libpatcher.h"

/*** 2D Video Globals ***/
GXRModeObj *vmode;		/*** Graphics Mode Object ***/
u32 *xfb[2] = { NULL, NULL };    /*** Framebuffers ***/
int whichfb = 0;        /*** Frame buffer toggle ***/
void ProperScanPADS(){	PAD_ScanPads(); }

/****************************************************************************
* Initialise Video
*
* Before doing anything in libogc, it's recommended to configure a video
* output.
****************************************************************************/
static void Initialise (void)
{
  VIDEO_Init ();        /*** ALWAYS CALL FIRST IN ANY LIBOGC PROJECT!
                     Not only does it initialise the video
                     subsystem, but also sets up the ogc os
                ***/

  PAD_Init ();            /*** Initialise pads for input ***/

    /*** Try to match the current video display mode
         using the higher resolution interlaced.

         So NTSC/MPAL gives a display area of 640x480
         PAL display area is 640x528
    ***/
  vmode = VIDEO_GetPreferredMode(NULL);
    /*** Let libogc configure the mode ***/
  VIDEO_Configure (vmode);

    /*** Now configure the framebuffer.
         Really a framebuffer is just a chunk of memory
         to hold the display line by line.
    ***/

  xfb[0] = (u32 *) MEM_K0_TO_K1 (SYS_AllocateFramebuffer (vmode));
    /*** I prefer also to have a second buffer for double-buffering.
         This is not needed for the console demo.
    ***/
  xfb[1] = (u32 *) MEM_K0_TO_K1 (SYS_AllocateFramebuffer (vmode));
     /*** Define a console ***/
    		/*			x	y     w   h			*/
  console_init (xfb[0], 50, 180, vmode->fbWidth,480, vmode->fbWidth * 2);
    /*** Clear framebuffer to black ***/
  VIDEO_ClearFrameBuffer (vmode, xfb[0], COLOR_BLACK);
  VIDEO_ClearFrameBuffer (vmode, xfb[1], COLOR_BLACK);

    /*** Set the framebuffer to be displayed at next VBlank ***/
  VIDEO_SetNextFramebuffer (xfb[0]);

    /*** Get the PAD status updated by libogc ***/
  VIDEO_SetPostRetraceCallback (ProperScanPADS);
  VIDEO_SetBlack (0);

    /*** Update the video for next vblank ***/
  VIDEO_Flush ();

  VIDEO_WaitVSync ();        /*** Wait for VBL ***/
  if (vmode->viTVMode & VI_NON_INTERLACE)
    VIDEO_WaitVSync ();
}

void wait_press_A()
{
	printf("Press A to continue..\n");
	while((PAD_ButtonsHeld(0) & PAD_BUTTON_A));
	while(!(PAD_ButtonsHeld(0) & PAD_BUTTON_A));
}

int main(void) {
    Initialise();

    printf("Starting...\n");
    printf("Ready!\n");

    bool success = apply_patches();
    if (!success) {
        printf("Failed to apply patches!\n");
    } else {
        printf("Successfully applied patches!\n");
    }

    wait_press_A();

    return 0;
}
