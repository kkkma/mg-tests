///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>

#if defined(_MGGAL_COMMLCD) && defined(__TARGET_EXTERNAL__)

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/exstubs.h>

#define SCREEN_WIDTH    240
#define SCREEN_HEIGHT   240
#define BYTES_PER_PIXEL 4
#define COLOR_DEPTH     32
#define PITCH           (SCREEN_WIDTH * BYTES_PER_PIXEL)
#define FB_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT * BYTES_PER_PIXEL)

/* all methods for COMMLCD engine should return zero for success */

int __commlcd_drv_init (void)
{
    return 0;
}

static BYTE* sg_fb;

/* return zero for success */
int __commlcd_drv_getinfo (struct commlcd_info *li, int width, int height, int depth)
{
    sg_fb = (BYTE*) mmap (0, FB_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (sg_fb == MAP_FAILED) {
        perror ("Failed calling __commlcd_drv_getinfo: ");
        return -1;
    }

    li->type = COMMLCD_TRUE_ARGB8888;
    li->height = SCREEN_HEIGHT;
    li->width = SCREEN_WIDTH;
    li->bpp = COLOR_DEPTH;
    li->pitch = SCREEN_WIDTH * BYTES_PER_PIXEL;
    li->update_method = COMMLCD_UPDATE_ASYNC;
    li->fb = sg_fb;
    return 0;
}

/* this method will be called async; do not call. */
int __commlcd_drv_update (const RECT* rc_dirty)
{
    char filename [PATH_MAX + 1];
    struct timeval tv;

    MYBITMAP my_bmp = {
        flags: MYBMP_TYPE_RGB | MYBMP_FLOW_DOWN,
        frames: 1,
        depth: 32,
        w: SCREEN_WIDTH,
        h: SCREEN_HEIGHT,
        pitch: PITCH,
        size: FB_SIZE,
        bits: sg_fb
    };

    printf ("__commlcd_drv_update called (%d, %d, %d, %d)\n",
            rc_dirty->left, rc_dirty->top,
            rc_dirty->right, rc_dirty->bottom);

    gettimeofday (&tv, NULL);
    sprintf (filename, "screenshot-%d.%d.bmp", (int)tv.tv_sec, (int)tv.tv_usec);
    SaveMyBitmapToFile (&my_bmp, NULL, filename);
    return 0;
}

int __commlcd_drv_release (void)
{
    munmap (sg_fb, FB_SIZE);
    return 0;
}

int __commlcd_drv_setclut (int firstcolor, int ncolors, GAL_Color *colors)
{
    return 0;
}

#endif /* _MGGAL_COMMLCD && __TARGET_EXTERNAL__ */
