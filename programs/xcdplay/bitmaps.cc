/*
 * XCdplay: Plays a cd.
 *
 * $Log: $
 *
 */

#ifndef lint
static char        *rcsid = "$Id: $";
#endif

/* System stuff */
#include <stdio.h>
#include <stdlib.h>
#define __USE_GNU
#include <string.h>

/* Include all the widget stuff we need */
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include "bitmaps.h"

#include "bitmaps/cdrom.xbm"
#include "bitmaps/cdrom.xpm"
#include "bitmaps/toolEject.xbm"
#include "bitmaps/toolEject.xpm"
#include "bitmaps/fastback.xbm"
#include "bitmaps/fastforw.xbm"
#include "bitmaps/pausebits.xbm"
#include "bitmaps/playbut.xbm"
#include "bitmaps/skipback.xbm"
#include "bitmaps/skipforw.xbm"
#include "bitmaps/stopbut.xbm"
#include "bitmaps/toolEditPlay.xbm"
#include "bitmaps/toolEditTrack.xbm"
#include "bitmaps/toolPlay.xbm"
#include "bitmaps/toolPlay.xpm"
#include "bitmaps/toolStop.xbm"
#include "bitmaps/toolStop.xpm"
#include "bitmaps/toolIntro.xbm"
#include "bitmaps/toolIntro.xpm"
#include "bitmaps/toolNormal.xbm"
#include "bitmaps/toolNormal.xpm"
#include "bitmaps/toolRandom.xbm"
#include "bitmaps/toolRandom.xpm"
#include "bitmaps/toolRepeat.xbm"
#include "bitmaps/toolRepeat.xpm"
#include "bitmaps/toolRemaining.xbm"
#include "bitmaps/toolRemaining.xpm"
#include "bitmaps/toolElapsed.xbm"
#include "bitmaps/toolElapsed.xpm"
#include "bitmaps/toolDisc.xbm"
#include "bitmaps/toolDisc.xpm"
#include "bitmaps/toolTotal.xbm"
#include "bitmaps/toolTotal.xpm"
#include "bitmaps/toolExit.xbm"
#include "bitmaps/toolExit.xpm"
#include "bitmaps/top.xbm"
#include "bitmaps/up.xbm"
#include "bitmaps/down.xbm"
#include "bitmaps/bot.xbm"

Pixmap	Bitmaps[26];
Pixmap  Bitmask[26];

#define entry(name, image, xpm) { name, image##_bits, xpm,  \
			        image##_height, image##_width },
struct {
	Bitmapnames	name;
	unsigned char	*data;
	char		**cdata;
	int		height;
	int		width;
} MapData[] = {
	entry(icon, cdrom, cdrom_xpm) 
	entry(fastback, fastback, NULL)
	entry(fastforw, fastforw, NULL)
 	entry(pausebits, pausebits, NULL)
 	entry(playbut, playbut, NULL) 
 	entry(skipback, skipback, NULL)
 	entry(skipforw, skipforw, NULL)
 	entry(stopbut, stopbut, NULL) 
	entry(toolEject, toolEject, toolEject_xpm)
 	entry(toolEditPlay, toolEditPlay, NULL)
 	entry(toolEditTrack, toolEditTrack, NULL)
 	entry(toolElapsed, toolElapsed, toolElapsed_xpm) 
 	entry(toolIntro, toolIntro, toolIntro_xpm) 
 	entry(toolNormal, toolNormal, toolNormal_xpm)
 	entry(toolRepeat, toolRepeat, toolRepeat_xpm)
 	entry(toolRandom, toolRandom, toolRandom_xpm) 
 	entry(toolPlay, toolPlay, toolPlay_xpm) 
 	entry(toolStop, toolStop, toolStop_xpm) 
 	entry(toolDisc, toolDisc, toolDisc_xpm)
 	entry(toolRemaining, toolRemaining, toolRemaining_xpm) 
 	entry(toolTotal, toolTotal, toolTotal_xpm) 
 	entry(toolExit, toolExit, toolExit_xpm)
 	entry(top, top, NULL)
 	entry(up, up, NULL)
 	entry(down, down, NULL)
 	entry(bot, bot, NULL)
	{(Bitmapnames)-1, NULL, NULL, 0, 0},
};

void mkbitmaps(Display *dpy)
{
    Window		win = DefaultRootWindow(dpy);
    int                 scn = DefaultScreen(dpy);
    Visual             *visual = XDefaultVisual(dpy, scn);
    Colormap            cmap = DefaultColormap(dpy, scn);
    unsigned int        depth = XDefaultDepth(dpy, scn);
    XColor              *clist;
    char                *map;
    XImage             *img;
    Pixmap              result;
    unsigned char      *mask_data;
    int                 i, j, dp, maskcell;
    int                 bitmap_pad, width, height;
    char	      **data, *p;
    GC                  gc;
    XGCValues           values;

    if (depth > 16)
        bitmap_pad = 32;
    else if (depth > 8)
        bitmap_pad = 16;
    else
        bitmap_pad = 8;

    values.foreground = 1;
    values.background = 0;
    
    for(int bm = 0; MapData[bm].name >= 0; bm++) {
 	if (depth == 1 || MapData[bm].cdata == NULL) {
	    Bitmaps[MapData[bm].name] = XCreateBitmapFromData(dpy, win, 
			(char *)MapData[bm].data, MapData[bm].width,
			 MapData[bm].height);
	    Bitmask[MapData[bm].name] = (Pixmap)NULL;
	    continue;
	}
	data = MapData[bm].cdata;
	/* Parse geometry in first line */
        i = 2;              /* Default Cmap */
        width = atoi(data[0]);
        if ((p = strchr(data[0], ' ')) != NULL) {
            height = atoi(p+1);
            if ((p = strchr(p+1, ' ')) != NULL) {
               i = atoi(p);
            }
        }
       /* Now read the color map */
        map = XtMalloc(i+1);
        map[i] = '\0';
        clist = (XColor *)XtMalloc((i * sizeof(XColor))+1);
        for(dp = 1, i--; i >=0; i--) {
            map[i] = *data[dp];
	    p = data[dp++];
            if ((p = strchr(&p[1], 'c')) == NULL)
                    continue;
            for(p++; *p == ' '; p++);
            if (strncasecmp(p, "None", 4) == 0)
                maskcell = i;
            else {
                if (XParseColor(dpy, cmap, p, &clist[i]) == 0)
                    fprintf(stderr, "Unable to parse color %s\n", p);
                XAllocColor(dpy, cmap, &clist[i]);
            }
        }

       /* Make mask bitmap */
	i = width&0x7;
	i = height * ((width + ((i != 0)? 8:0))/8);
        mask_data = (unsigned char *)XtMalloc(i);
        for (; i >= 0; mask_data[--i] = 0) ;
       /* Make image */
        img = XCreateImage(dpy, visual, depth, ZPixmap, 0, 0,
                           width, height, bitmap_pad, 0);
        img->data = (char *)XtMalloc(img->bytes_per_line * height);
    
       /* Make image and mask */
        for (i = 0; i < height; i++) {
            char               *line = data[dp++];
            for (j = 0; j < width; j++) {
                int                 pix;
    
                if((p = strchr(map, line[j])) == NULL)
                    continue;
                pix = p - map;
                if (pix != maskcell) {
		    int pos = width & 0x7;
		    pos = i * ((width + ((pos != 0)?8:0))/8);
                    mask_data[pos + j/8] |= 1 << (j & 0x7);
		}
                XPutPixel(img, j, i, clist[pix].pixel);
            }
        }
    
       /* Build mask if one was requested */
	Bitmask[MapData[bm].name] = XCreateBitmapFromData(dpy, win,
                                           (char *) mask_data, width, height);

       /* Make the Pixmap */
        result = XCreatePixmap(dpy, win, width, height, depth);

        gc = XCreateGC(dpy, result, GCForeground | GCBackground, &values);
        XPutImage(dpy, result, gc, img, 0, 0, 0, 0, width, height);
    
       /* Free stuff we don't need */
        XDestroyImage(img);
        XFreeGC(dpy, gc);
        XtFree((char *)mask_data);
        XtFree(map);
        XtFree((char *)clist);
	Bitmaps[MapData[bm].name] = result;
    }
}
