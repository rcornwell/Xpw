/*
 * threeDdraw.
 *
 * Library of internal routines for doing 3D frame and outline drawing.
 * This library includes GC caching to improve preformace.
 * 
 *
 * Copyright 1997 Richard P. Cornwell All Rights Reserved,
 *
 * The software is provided "as is", without warranty of any kind, express
 * or implied, including but not limited to the warranties of
 * merchantability, fitness for a particular purpose and non-infringement.
 * In no event shall Richard Cornwell be liable for any claim, damages
 * or other liability, whether in an action of contract, tort or otherwise,
 * arising from, out of or in connection with the software or the use or other
 * dealings in the software.
 *
 * Permission to use, copy, and distribute this software and its
 * documentation for non commercial use is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation.
 *
 * The sale, resale, or use of this library for profit without the
 * express written consent of the author Richard Cornwell is forbidden.
 * Please see attached License file for information about using this
 * library in commercial applications, or for commercial software distribution.
 *
 * $Log: threeDdraw.c,v $
 * Revision 1.1  1997/10/26 04:22:04  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: threeDdraw.c,v 1.1 1997/10/26 04:22:04 rich Exp rich $";

#endif

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Misc.h>
#include <X11/Xmu/CharSet.h>
#include <X11/Shell.h>
#include <X11/CoreP.h>
#include "XpwInit.h"
#include "threeDdrawP.h"

/* Private Functions */
static void         AllocTopShadowPixel(Widget /*new */ ,
			 _XpmThreeDFrame * /*threeD */ , Pixel /*bg */ );
static void         AllocBotShadowPixel(Widget /*new */ ,
			 _XpmThreeDFrame * /*threeD */ , Pixel /*bg */ );
static void         AllocTopShadowPixmap(Widget /*new */ ,
			 _XpmThreeDFrame * /*threeD */ , Pixel /*bg */ );
static void         AllocBotShadowPixmap(Widget /*new */ ,
			 _XpmThreeDFrame * /*threeD */ , Pixel /*bg */ );

/*
 * Called by any class at ClassInit time wishing to use 3D converters.
 * Will ultimatly be called by Xpw call init library.
 */
void 
_XpwThreeDClassInit()
{
    static int          initdone = 0;

    if (!initdone) {
	XtAddConverter(XtRString, XtRJustify, XmuCvtStringToJustify,
		       (XtConvertArgList) NULL, (Cardinal) 0);
	XtSetTypeConverter(XtRString, XtRFrameType, cvtStringToFrameType,
			   NULL, 0, XtCacheNone, NULL);
	XtSetTypeConverter(XtRFrameType, XtRString, cvtFrameTypeToString,
			   NULL, 0, XtCacheNone, NULL);
	XtSetTypeConverter(XtRString, XtRShapeType, cvtStringToShapeType,
			   NULL, 0, XtCacheNone, NULL);
	XtSetTypeConverter(XtRShapeType, XtRString, cvtShapeTypeToString,
			   NULL, 0, XtCacheNone, NULL);
	initdone = 1;
    }
}

/*
 * Called during Initialize to set up 3D stuff.
 */

void
_XpwThreeDInit(new, threeD, bg)
	Widget              new;
	_XpmThreeDFrame    *threeD;
	Pixel               bg;
{
    Screen             *scr = XtScreenOfObject(new);

    if (threeD->be_nice_to_cmap || DefaultDepthOfScreen(scr) == 1) {
	AllocTopShadowPixmap(new, threeD, bg);
	AllocBotShadowPixmap(new, threeD, bg);
    } else {
	if (threeD->top_shadow_pixel == threeD->bot_shadow_pixel) {
	    AllocTopShadowPixel(new, threeD, bg);
	    AllocBotShadowPixel(new, threeD, bg);
	}
	threeD->top_shadow_pxmap = threeD->bot_shadow_pxmap = 0;
    }
    _XpwThreeDAllocateShadowGC(new, threeD, bg);
}				/* Initialize */

/*
 * Do set values code. Return true if we need a redraw.
 */

/* ARGSUSED */
Boolean 
_XpmThreeDSetValues(w, current, new, bg)
	Widget              w;	/* Widget we are allocating for */
	_XpmThreeDFrame    *current, *new;	/* Structure to play in */
	Pixel               bg;	/* Background pixel */
{
    Boolean             alloc_top_pixel = FALSE;
    Boolean             alloc_bot_pixel = FALSE;
    Boolean             alloc_top_pixmap = FALSE;
    Boolean             alloc_bot_pixmap = FALSE;
    Boolean             alloc_gc = FALSE;
    Boolean             ret_val = FALSE;

    if (new->be_nice_to_cmap != current->be_nice_to_cmap) {
	if (new->be_nice_to_cmap) {
	    alloc_top_pixmap = TRUE;
	    alloc_bot_pixmap = TRUE;
	} else {
	    alloc_top_pixel = TRUE;
	    alloc_bot_pixel = TRUE;
	}
	ret_val = TRUE;
    }
    if (!new->be_nice_to_cmap &&
	new->top_shadow_contrast != current->top_shadow_contrast)
	alloc_top_pixel = TRUE;
    if (!new->be_nice_to_cmap &&
	new->bot_shadow_contrast != current->bot_shadow_contrast)
	alloc_bot_pixel = TRUE;
    if (alloc_top_pixel)
	AllocTopShadowPixel(w, current, bg);
    if (alloc_bot_pixel)
	AllocBotShadowPixel(w, current, bg);
    if (alloc_top_pixmap)
	AllocTopShadowPixmap(w, current, bg);
    if (alloc_bot_pixmap)
	AllocBotShadowPixmap(w, current, bg);
    if (!new->be_nice_to_cmap &&
	new->top_shadow_pixel != current->top_shadow_pixel)
	alloc_top_pixel = TRUE;
    if (!new->be_nice_to_cmap &&
	new->bot_shadow_pixel != current->bot_shadow_pixel)
	alloc_bot_pixel = TRUE;
    if (new->be_nice_to_cmap) {
	if (alloc_top_pixmap || alloc_bot_pixmap)
	    alloc_gc = TRUE;
    } else {
	if (alloc_top_pixel) {
	    if (new->top_shadow_pxmap) {
		XFreePixmap(XtDisplayOfObject(w), new->top_shadow_pxmap);
		new->top_shadow_pxmap = (Pixmap) NULL;
	    }
	    alloc_gc = TRUE;
	}
	if (alloc_bot_pixel) {
	    if (new->bot_shadow_pxmap) {
		XFreePixmap(XtDisplayOfObject(w), new->bot_shadow_pxmap);
		new->bot_shadow_pxmap = (Pixmap) NULL;
	    }
	    alloc_gc = TRUE;
	}
    }
    if (alloc_gc) {
	_XpwThreeDDestroyShadow(w, current);
	_XpwThreeDAllocateShadowGC(w, new, bg);
	ret_val = TRUE;
    }
    if (new->raised != current->raised || new->frameType != current->frameType)
	ret_val = TRUE;
    return (ret_val);
}

/*
 * Draw a 3D shadow around a area.
 */

void 
_XpwThreeDDrawShadow(wid, event, region, threeD, x, y, w, h, comp)
	Widget              wid;
	XEvent             *event;
	Region              region;
	_XpmThreeDFrame    *threeD;
	Dimension           x;
	Dimension           y;
	Dimension           w;
	Dimension           h;
	Boolean             comp;
{
    Dimension           s = threeD->shadow_width;
    Dimension           f = s / 2;
    XPoint              pt[6];
    Display            *dpy = XtDisplayOfObject(wid);
    Window              win = XtWindowOfObject(wid);
    GC                  top, bot;

   /* Draw shadows around main window */
    if (s <= 0)
	return;

    if (threeD->raised ^ comp) {
	top = threeD->top_shadow_GC;
	bot = threeD->bot_shadow_GC;
    } else {
	top = threeD->bot_shadow_GC;
	bot = threeD->top_shadow_GC;
    }
    switch (threeD->frameType) {
    case XfBeveled:
       /* top-left shadow */
	pt[0].x = x;		    pt[0].y = y + h;
	pt[1].x = x;		    pt[1].y = y;
	pt[2].x = x + w;	    pt[2].y = y;
	pt[3].x = x + w - s;	    pt[3].y = y + s;
	pt[4].x = x + s;	    pt[4].y = y + s;
	pt[5].x = x + s;	    pt[5].y = y + h - s;
	XFillPolygon(dpy, win, top, pt, 6, Complex, CoordModeOrigin);

       /* bottom-right shadow */
       /* pt[0].x = x;              pt[0].y = y + h;        */
	  pt[1].x = x + w;	    pt[1].y = y + h;
       /* pt[2].x = x + w;          pt[2].y = y;            */
       /* pt[3].x = x + w - s;      pt[3].y = y + s;        */
	  pt[4].x = x + w - s;	    pt[4].y = y + h - s;
       /* pt[5].x = x + s;          pt[5].y = y + h - s;    */
	XFillPolygon(dpy, win, bot, pt, 6, Complex, CoordModeOrigin);
	break;
    case XfGrooved:
       /* top shadow */
	pt[0].x = x;		    pt[0].y = y + h;
	pt[1].x = x;		    pt[1].y = y;
	pt[2].x = x + w;	    pt[2].y = y;
	pt[3].x = x + w - f;	    pt[3].y = y + f;
	pt[4].x = x + f;	    pt[4].y = y + f;
	pt[5].x = x + f;	    pt[5].y = y + h - f;
	XFillPolygon(dpy, win, top, pt, 6, Complex, CoordModeOrigin);

       /* top shadow */
	pt[0].x = x + f;	    pt[0].y = y + h - f;
	pt[1].x = x + w - f;	    pt[1].y = y + h - f;
	pt[2].x = x + w - f;	    pt[2].y = y + f;
	pt[3].x = x + w - s;	    pt[3].y = y + s;
	pt[4].x = x + w - s;	    pt[4].y = y + h - s;
	pt[5].x = x + s;	    pt[5].y = y + h - s;
	XFillPolygon(dpy, win, top, pt, 6, Complex, CoordModeOrigin);
       /* bot shadow */
	pt[0].x = x + f;	    pt[0].y = y + h - f;
	pt[1].x = x + f;	    pt[1].y = y + f;
	pt[2].x = x + w - f;	    pt[2].y = y + f;
	pt[3].x = x + w - s;	    pt[3].y = y + s;
	pt[4].x = x + s;	    pt[4].y = y + s;
	pt[5].x = x + s;	    pt[5].y = y + h - s;
	XFillPolygon(dpy, win, bot, pt, 6, Complex, CoordModeOrigin);

       /* bot shadow */
	pt[0].x = x;		    pt[0].y = y + h;
	pt[1].x = x + w;	    pt[1].y = y + h;
	pt[2].x = x + w;	    pt[2].y = y;
	pt[3].x = x + w - f;	    pt[3].y = y + f;
	pt[4].x = x + w - f;	    pt[4].y = y + h - f;
	pt[5].x = x + f;	    pt[5].y = y + h - f;
	XFillPolygon(dpy, win, bot, pt, 6, Complex, CoordModeOrigin);
	break;
    case XfEdge:
	if (threeD->raised ^ comp) {
	   /* bottom-right shadow */
	    pt[0].x = x;		    pt[0].y = y + h;
	    pt[1].x = x + w;		    pt[1].y = y + h;
	    pt[2].x = x + w;		    pt[2].y = y - 1;
	    pt[3].x = x + w - s;	    pt[3].y = y - 1;
	    pt[4].x = x + w - s;	    pt[4].y = y + h - s;
	    pt[5].x = x - 1;		    pt[5].y = y + h - s;
	    XFillPolygon(dpy, win, bot, pt, 6, Complex, CoordModeOrigin);
	 /* pt[0].x = x;		    pt[0].y = y + h; */
	    pt[1].x = x;		    pt[1].y = y;
	    pt[2].x = x + w;		    pt[2].y = y;
	    XDrawLines(dpy, win, top, pt, 3, CoordModeOrigin);
	} else {
	   /* top-left shadow */
	    pt[0].x = x;		    pt[0].y = y + h - 1;
	    pt[1].x = x;		    pt[1].y = y;
	    pt[2].x = x + w - 1;	    pt[2].y = y;
	    pt[3].x = x + w - 1;	    pt[3].y = y + s;
	    pt[4].x = x + s;		    pt[4].y = y + s;
	    pt[5].x = x + s;		    pt[5].y = y + h - 1;
	    XFillPolygon(dpy, win, top, pt, 6, Complex, CoordModeOrigin);
	 /* pt[0].x = x;		    pt[0].y = y + h - 1; */
	    pt[1].x = x + w - 1;	    pt[1].y = y + h - 1;
	    pt[2].x = x + w - 1;	    pt[2].y = y;
	    XDrawLines(dpy, win, bot, pt, 3, CoordModeOrigin);
	}
    case XfOutline:
       /* top-left shadow */
	pt[0].x = x;		    pt[0].y = y + h;
	pt[1].x = x;		    pt[1].y = y;
	pt[2].x = x + w;	    pt[2].y = y;
	pt[3].x = x + w;	    pt[3].y = y + h;
	XDrawLines(dpy, win, top, pt, 4, CoordModeOrigin);
	break;
    case XfNone:
	break;
    }
}

/*
 * Erase a 3D highlight.
 */
void 
_XpwThreeDEraseShadow(wid, event, region, threeD, x, y, w, h)
	Widget              wid;
	XEvent             *event;
	Region              region;
	_XpmThreeDFrame    *threeD;
	Dimension           x;
	Dimension           y;
	Dimension           w;
	Dimension           h;
{
    Dimension           s = threeD->shadow_width;
    XPoint              pt[6];
    Display            *dpy = XtDisplayOfObject(wid);
    Window              win = XtWindowOfObject(wid);
    GC                  gc = threeD->erase_GC;

   /* Draw shadows around main window */
    if (s <= 0)
	return;

    switch (threeD->frameType) {
    case XfBeveled:
    case XfGrooved:
    case XfEdge:
       /* top-left shadow */
	pt[0].x = x;		    pt[0].y = y + h;
	pt[1].x = x;		    pt[1].y = y;
	pt[2].x = x + w;	    pt[2].y = y;
	pt[3].x = x + w - s;	    pt[3].y = y + s;
	pt[4].x = x + s;	    pt[4].y = y + s;
	pt[5].x = x + s;	    pt[5].y = y + h - s;
	XFillPolygon(dpy, win, gc, pt, 6, Complex, CoordModeOrigin);

       /* bottom-right shadow */
       /* pt[0].x = x;              pt[0].y = y + h;        */
	  pt[1].x = x + w;	    pt[1].y = y + h;
       /* pt[2].x = x + w;          pt[2].y = y;            */
       /* pt[3].x = x + w - s;      pt[3].y = y + s;        */
	  pt[4].x = x + w - s;	    pt[4].y = y + h - s;
       /* pt[5].x = x + s;          pt[5].y = y + h - s;    */
	XFillPolygon(dpy, win, gc, pt, 6, Complex, CoordModeOrigin);
	break;
    case XfOutline:
       /* top-left shadow */
	pt[0].x = x;		    pt[0].y = y + h;
	pt[1].x = x;		    pt[1].y = y;
	pt[2].x = x + w;	    pt[2].y = y;
	pt[3].x = x + w;	    pt[3].y = y + h;
	XDrawLines(dpy, win, gc, pt, 4, CoordModeOrigin);
	break;
    case XfNone:
	break;
    }
}

void 
_XpwThreeDDrawShape(wid, event, region, threeD, gc, shape, x, y, w, h, comp)
	Widget              wid;
	XEvent             *event;
	Region              region;
	_XpmThreeDFrame    *threeD;
	GC		    gc;
	ShapeType	    shape;
	Dimension           x;
	Dimension           y;
	Dimension           w;
	Dimension           h;
	Boolean             comp;
{
    Dimension           s = threeD->shadow_width;
    XPoint              pt[10];
    Display            *dpy = XtDisplayOfObject(wid);
    Window              win = XtWindowOfObject(wid);
    GC                  top, bot;

   /* Force the width */
    if (s <= 0)
	s = 1;
   /* Figure out what colors to make shape */
    if (threeD->raised ^ comp) {
	top = threeD->top_shadow_GC;
	bot = threeD->bot_shadow_GC;
    } else {
	top = threeD->bot_shadow_GC;
	bot = threeD->top_shadow_GC;
    }

   /* Draw the arrow */
    switch (shape) {
    case XaNone:
	break;

    case XaUp:
       /* top-left shadow */
	pt[0].x = x + w / 2;	 pt[0].y = y;
	pt[1].x = x;		 pt[1].y = y + h;
	pt[2].x = x + s;	 pt[2].y = y + h - s;
	pt[3].x = x + w / 2;	 pt[3].y = y + s;
	XFillPolygon(dpy, win, top, pt, 4, Complex, CoordModeOrigin);

       /* bottom-right shadow */
     /* pt[0].x = x + w/2;       pt[0].y = y;        */
	pt[1].x = x + w;	 pt[1].y = y + h;
	pt[2].x = x;		 pt[2].y = y + h;
	pt[3].x = x + s;	 pt[3].y = y + h - s;
	pt[4].x = x + w - s;	 pt[4].y = y + h - s;
	pt[5].x = x + w / 2;	 pt[5].y = y + s;
	XFillPolygon(dpy, win, bot, pt, 6, Complex, CoordModeOrigin);

       /* Center window */
	pt[0].x = x + w / 2;	 pt[0].y = y + s;
	pt[1].x = x + s;	 pt[1].y = y + h - s;
	pt[2].x = x + w - s;	 pt[2].y = y + h - s;
	XFillPolygon(dpy, win, gc, pt, 3, Complex, CoordModeOrigin);
	break;

    case XaDown:
       /* top-left shadow */
	pt[0].x = x + w;	 pt[0].y = y;
	pt[1].x = x;		 pt[1].y = y;
	pt[2].x = x + w / 2;	 pt[2].y = y + h;
	pt[3].x = x + w / 2;	 pt[3].y = y + h - s;
	pt[4].x = x + s;	 pt[4].y = y + s;
	pt[5].x = x + w;	 pt[5].y = y + s;
	XFillPolygon(dpy, win, top, pt, 6, Complex, CoordModeOrigin);

       /* bottom-right shadow */
     /* pt[0].x = x + w;         pt[0].y = y; */
	pt[1].x = x + w / 2;	 pt[1].y = y + h;
	pt[2].x = x + w / 2; 	 pt[2].y = y + h - s;
	pt[3].x = x + w - s;	 pt[3].y = y + s;
	XFillPolygon(dpy, win, bot, pt, 4, Complex, CoordModeOrigin);

       /* Center window */
	pt[0].x = x + w / 2;	 pt[0].y = y + h - s;
	pt[1].x = x + s;	 pt[1].y = y + s;
	pt[2].x = x + w - s;	 pt[2].y = y + s;
	XFillPolygon(dpy, win, gc, pt, 3, Complex, CoordModeOrigin);
	break;

    case XaLeft:
       /* top-left shadow */
	pt[0].x = x + w;	 pt[0].y = y;
	pt[1].x = x;		 pt[1].y = y + h / 2;
	pt[2].x = x + s;	 pt[2].y = y + h / 2;
	pt[3].x = x + w;	 pt[3].y = y + s;
	XFillPolygon(dpy, win, top, pt, 4, Complex, CoordModeOrigin);

       /* bottom-right shadow */
     /* pt[0].x = x + w;         pt[0].y = y;        */
	pt[1].x = x + w;	 pt[1].y = y + h;
	pt[2].x = x;		 pt[2].y = y + h / 2;
	pt[3].x = x + s;	 pt[3].y = y + h / 2;
	pt[4].x = x + w - s;	 pt[4].y = y + h - s;
	pt[5].x = x + w - s;	 pt[5].y = y + s;
	XFillPolygon(dpy, win, bot, pt, 6, Complex, CoordModeOrigin);

       /* Center window */
	pt[0].x = x + s;	 pt[0].y = y + h / 2;
	pt[1].x = x + w - s;	 pt[1].y = y + s;
	pt[2].x = x + w - s;	 pt[2].y = y + h - s;
	XFillPolygon(dpy, win, gc, pt, 3, Complex, CoordModeOrigin);
	break;

    case XaRight:
       /* top-left shadow */
	pt[0].x = x;		 pt[0].y = y + h;
	pt[1].x = x;		 pt[1].y = y;
	pt[2].x = x + w;	 pt[2].y = y + h / 2;
	pt[3].x = x + w - s;	 pt[3].y = y + h / 2;
	pt[4].x = x + s;	 pt[4].y = y + s;
	pt[5].x = x + s;	 pt[5].y = y + h - s;
	XFillPolygon(dpy, win, top, pt, 6, Complex, CoordModeOrigin);

       /* bottom-right shadow */
       /*  pt[0].x = x;          pt[0].y = y + h; */
	pt[1].x = x + w;	 pt[1].y = y + h / 2;
	pt[2].x = x + w - s;	 pt[2].y = y + h / 2;
	pt[3].x = x + s;	 pt[3].y = y + h - s;
	XFillPolygon(dpy, win, bot, pt, 4, Complex, CoordModeOrigin);

       /* Center window */
	pt[0].x = x + s;	 pt[0].y = y + s;
	pt[1].x = x + w - s;	 pt[1].y = y + h / 2;
	pt[2].x = x + s;	 pt[2].y = y + h - s;
	XFillPolygon(dpy, win, gc, pt, 3, Complex, CoordModeOrigin);
	break;

    case XaSquare:
       /* top-left shadow */
	pt[0].x = x + w;	 pt[0].y = y;
	pt[1].x = x;		 pt[1].y = y;
	pt[2].x = x;		 pt[2].y = y + h;
	pt[3].x = x + s;	 pt[3].y = y + h - s;
	pt[4].x = x + s;	 pt[4].y = y + s;
	pt[5].x = x + w - s;	 pt[5].y = y + s;
	XFillPolygon(dpy, win, top, pt, 6, Complex, CoordModeOrigin);

       /* bottom-right shadow */
     /* pt[0].x = x + w;         pt[0].y = y; */
	pt[1].x = x + w;	 pt[1].y = y + h;
	pt[2].x = x;		 pt[2].y = y + h;
	pt[3].x = x + s;	 pt[3].y = y + h - s;
	pt[4].x = x + w - s;	 pt[4].y = y + h - s;
     /*	pt[5].x = x + w - s;	 pt[5].y = y + s; */
	XFillPolygon(dpy, win, bot, pt, 6, Complex, CoordModeOrigin);

       /* Center window */
	pt[0].x = x + s;	 pt[0].y = y + s;
	pt[1].x = x + w - s;	 pt[1].y = y + s;
	pt[2].x = x + w - s;	 pt[2].y = y + h - s;
	pt[3].x = x + s;	 pt[3].y = y + h - s;
	XFillPolygon(dpy, win, gc, pt, 4, Complex, CoordModeOrigin);
	break;

    case XaDiamond:
       /* top-left shadow */
	pt[0].x = x + w/2;	 pt[0].y = y;
	pt[1].x = x;		 pt[1].y = y + h/2;
	pt[2].x = x + s;	 pt[2].y = y + h/2;
	pt[3].x = x + w/2 + s;	 pt[3].y = y + 2 * s;
	XFillPolygon(dpy, win, top, pt, 4, Complex, CoordModeOrigin);

       /* bottom-right shadow */
     /* pt[0].x = x + w/2;       pt[0].y = y; */
	pt[1].x = x + w;	 pt[1].y = y + h/2;
	pt[2].x = x + w/2;	 pt[2].y = y + h;
	pt[3].x = x;		 pt[3].y = y + h/2;
	pt[4].x = x + s;	 pt[4].y = y + h/2;
       	pt[5].x = x + w/2;	 pt[5].y = y + h - s;
       	pt[6].x = x + w - s;	 pt[6].y = y + h/2;
       	pt[7].x = x + w/2;	 pt[7].y = y + s;
	XFillPolygon(dpy, win, bot, pt, 8, Complex, CoordModeOrigin);

       /* Center window */
	pt[0].x = x + w/2;	 pt[0].y = y + s;
	pt[1].x = x + s;	 pt[1].y = y + h/2;
	pt[2].x = x + w/2;	 pt[2].y = y + h - s;
	pt[3].x = x + w - s;	 pt[3].y = y + h/2;
	XFillPolygon(dpy, win, gc, pt, 4, Complex, CoordModeOrigin);
	break;

    }

}

/*
 * Remove the GC's.
 */
void 
_XpwThreeDDestroyShadow(w, threeD)
	Widget              w;	/* Widget we are allocating for */
	_XpmThreeDFrame    *threeD;	/* Structure to play in */
{
    XtReleaseGC(w, threeD->top_shadow_GC);
    XtReleaseGC(w, threeD->bot_shadow_GC);
    XtReleaseGC(w, threeD->erase_GC);
}

/************************************************************
 *
 * Type converters.
 *
 ************************************************************/

Boolean
cvtStringToFrameType(dpy, args, num_args, from, to, converter_data)
	Display            *dpy;
	XrmValuePtr         args;
	Cardinal           *num_args;
	XrmValuePtr         from;
	XrmValuePtr         to;
	XtPointer          *converter_data;
{
    String              s = (String) from->addr;
    static FrameType    result;

    if (*num_args != 0) {
	XtAppErrorMsg(XtDisplayToApplicationContext(dpy),
		      "cvtStringToFrameType", "wrongParameters",
		      "XtToolkitError",
		    "String to frame type conversion needs no arguments",
		      (String *) NULL, (Cardinal *) NULL);
	return FALSE;
    }
    if (XmuCompareISOLatin1(s, "beveled") == 0)
	result = XfBeveled;
    else if (XmuCompareISOLatin1(s, "grooved") == 0)
	result = XfGrooved;
    else if (XmuCompareISOLatin1(s, "edge") == 0)
	result = XfEdge;
    else if (XmuCompareISOLatin1(s, "outline") == 0)
	result = XfOutline;
    else if (XmuCompareISOLatin1(s, "none") == 0)
	result = XfNone;
    else {
	XtDisplayStringConversionWarning(dpy, s, XtRFrameType);
	return FALSE;
    }

    if (to->addr != NULL) {
	if (to->size < sizeof(FrameType)) {
	    to->size = sizeof(FrameType);
	    return FALSE;
	}
	*(FrameType *) (to->addr) = result;
    } else
	(FrameType *) to->addr = &result;
    to->size = sizeof(FrameType);
    return TRUE;
}

Boolean
cvtFrameTypeToString(dpy, args, num_args, from, to, converter_data)
	Display            *dpy;
	XrmValuePtr         args;
	Cardinal           *num_args;
	XrmValuePtr         from;
	XrmValuePtr         to;
	XtPointer          *converter_data;
{
    static String       result;

    if (*num_args != 0) {
	XtAppErrorMsg(XtDisplayToApplicationContext(dpy),
		      "cvtFrameTypeToString", "wrongParameters",
		      "XtToolkitError",
		   "Fframe type to String conversion needs no arguments",
		      (String *) NULL, (Cardinal *) NULL);
	return FALSE;
    }
    switch (*(FrameType *) from->addr) {
    case XfBeveled:
	result = "beveled";
    case XfGrooved:
	result = "grooved";
    case XfEdge:
	result = "edge";
    case XfOutline:
	result = "outline";
    case XfNone:
	result = "none";
    default:
	XtError("Illegal FrameType");
	return FALSE;
    }
    if (to->addr != NULL) {
	if (to->size < sizeof(String)) {
	    to->size = sizeof(String);
	    return FALSE;
	}
	*(String *) (to->addr) = result;
    } else
	(String *) to->addr = &result;
    to->size = sizeof(String);
    return TRUE;
}

Boolean
cvtStringToShapeType(dpy, args, num_args, from, to, converter_data)
	Display            *dpy;
	XrmValuePtr         args;
	Cardinal           *num_args;
	XrmValuePtr         from;
	XrmValuePtr         to;
	XtPointer          *converter_data;
{
    String              s = (String) from->addr;
    static ShapeType    result;

    if (*num_args != 0) {
	XtAppErrorMsg(XtDisplayToApplicationContext(dpy),
		      "cvtStringToShapeType", "wrongParameters",
		      "XtToolkitError",
		    "String to frame type conversion needs no arguments",
		      (String *) NULL, (Cardinal *) NULL);
	return FALSE;
    }
    if (XmuCompareISOLatin1(s, "up") == 0)
	result = XaUp;
    else if (XmuCompareISOLatin1(s, "down") == 0)
	result = XaDown;
    else if (XmuCompareISOLatin1(s, "right") == 0)
	result = XaRight;
    else if (XmuCompareISOLatin1(s, "left") == 0)
	result = XaLeft;
    else if (XmuCompareISOLatin1(s, "square") == 0)
	result = XaSquare;
    else if (XmuCompareISOLatin1(s, "diamond") == 0)
	result = XaDiamond;
    else {
	XtDisplayStringConversionWarning(dpy, s, XtRShapeType);
	return FALSE;
    }

    if (to->addr != NULL) {
	if (to->size < sizeof(ShapeType)) {
	    to->size = sizeof(ShapeType);
	    return FALSE;
	}
	*(ShapeType *) (to->addr) = result;
    } else
	(ShapeType *) to->addr = &result;
    to->size = sizeof(ShapeType);
    return TRUE;
}

Boolean
cvtShapeTypeToString(dpy, args, num_args, from, to, converter_data)
	Display            *dpy;
	XrmValuePtr         args;
	Cardinal           *num_args;
	XrmValuePtr         from;
	XrmValuePtr         to;
	XtPointer          *converter_data;
{
    static String       result;

    if (*num_args != 0) {
	XtAppErrorMsg(XtDisplayToApplicationContext(dpy),
		      "cvtArrowTypeToString", "wrongParameters",
		      "XtToolkitError",
		   "Fframe type to String conversion needs no arguments",
		      (String *) NULL, (Cardinal *) NULL);
	return FALSE;
    }
    switch (*(ShapeType *) from->addr) {
    case XaUp:
	result = "up";
    case XaDown:
	result = "down";
    case XaRight:
	result = "right";
    case XaLeft:
	result = "left";
    case XaSquare:
	result = "square";
    case XaDiamond:
	result = "diamond";
    default:
	XtError("Illegal ShapeType");
	return FALSE;
    }
    if (to->addr != NULL) {
	if (to->size < sizeof(String)) {
	    to->size = sizeof(String);
	    return FALSE;
	}
	*(String *) (to->addr) = result;
    } else
	(String *) to->addr = &result;
    to->size = sizeof(String);
    return TRUE;
}

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

#define shadowpm_width 8
#define shadowpm_height 8
static char         shadowpm_bits[] =
{
    0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55};

static char         mtshadowpm_bits[] =
{
    0x92, 0x24, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};

static char         mbshadowpm_bits[] =
{
    0x6d, 0xdb, 0xb6, 0x6d, 0xdb, 0xb6, 0x6d, 0xdb};

void 
_XpwThreeDAllocateShadowGC(w, threeD, bg)
	Widget              w;	/* Widget we are allocating for */
	_XpmThreeDFrame    *threeD;	/* Structure to play in */
	Pixel               bg;	/* Background pixel */
{
    Screen             *scn = XtScreenOfObject(w);
    XtGCMask            valuemask;
    XGCValues           topGC, botGC, eraGC;

    if (threeD->be_nice_to_cmap || DefaultDepthOfScreen(scn) == 1) {
	valuemask = GCTile | GCFillStyle;
	topGC.tile = threeD->top_shadow_pxmap;
	topGC.fill_style = FillTiled;
	botGC.tile = threeD->bot_shadow_pxmap;
	botGC.fill_style = FillTiled;
    } else {
	valuemask = GCForeground;
	topGC.foreground = threeD->top_shadow_pixel;
	botGC.foreground = threeD->bot_shadow_pixel;
    }
    threeD->top_shadow_GC = XtGetGC(w, valuemask, &topGC);
    threeD->bot_shadow_GC = XtGetGC(w, valuemask, &botGC);
    valuemask = GCForeground;
    eraGC.foreground = XtParent(w)->core.background_pixel;
    threeD->erase_GC = XtGetGC(w, valuemask, &eraGC);
}

/* ARGSUSED */
static void
AllocTopShadowPixel(new, threeD, bg)
	Widget              new;
	_XpmThreeDFrame    *threeD;	/* Structure to play in */
	Pixel               bg;
{
    XColor              set_c, get_c;
    Display            *dpy = XtDisplayOfObject(new);
    Screen             *scn = XtScreenOfObject(new);
    Colormap            cmap = DefaultColormapOfScreen(scn);
    double              contrast;

    get_c.pixel = bg;
    if (get_c.pixel == WhitePixelOfScreen(scn) ||
	get_c.pixel == BlackPixelOfScreen(scn)) {
	contrast = (100 - threeD->top_shadow_contrast) / 100.0;
	set_c.red = contrast * 65535.0;
	set_c.green = contrast * 65535.0;
	set_c.blue = contrast * 65535.0;
    } else {
	contrast = 1.0 + threeD->top_shadow_contrast / 100.0;
	XQueryColor(dpy, cmap, &get_c);
#define MIN(x,y) (unsigned short) (x < y) ? x : y
	set_c.red = MIN(65535, (int) (contrast * (double) get_c.red));
	set_c.green = MIN(65535, (int) (contrast * (double) get_c.green));
	set_c.blue = MIN(65535, (int) (contrast * (double) get_c.blue));
#undef MIN
    }
    (void) XAllocColor(dpy, cmap, &set_c);
    threeD->top_shadow_pixel = set_c.pixel;
}

/* ARGSUSED */
static void
AllocBotShadowPixel(new, threeD, bg)
	Widget              new;
	_XpmThreeDFrame    *threeD;	/* Structure to play in */
	Pixel               bg;
{
    XColor              set_c, get_c;
    Display            *dpy = XtDisplayOfObject(new);
    Screen             *scn = XtScreenOfObject(new);
    Colormap            cmap = DefaultColormapOfScreen(scn);
    double              contrast;

    get_c.pixel = bg;
    if (get_c.pixel == WhitePixelOfScreen(scn) ||
	get_c.pixel == BlackPixelOfScreen(scn)) {
	contrast = threeD->bot_shadow_contrast / 100.0;
	set_c.red = contrast * 65535.0;
	set_c.green = contrast * 65535.0;
	set_c.blue = contrast * 65535.0;
    } else {
	XQueryColor(dpy, cmap, &get_c);
	contrast = (100 - threeD->bot_shadow_contrast) / 100.0;
	set_c.red = contrast * get_c.red;
	set_c.green = contrast * get_c.green;
	set_c.blue = contrast * get_c.blue;
    }
    (void) XAllocColor(dpy, cmap, &set_c);
    threeD->bot_shadow_pixel = set_c.pixel;
}

/* ARGSUSED */
static void
AllocTopShadowPixmap(new, threeD, bg)
	Widget              new;
	_XpmThreeDFrame    *threeD;	/* Structure to play in */
	Pixel               bg;
{
    Display            *dpy = XtDisplayOfObject(new);
    Screen             *scn = XtScreenOfObject(new);
    unsigned long       top_fg_pixel = 0, top_bg_pixel = 0;
    char               *pm_data;
    Boolean             create_pixmap = FALSE;

   /*
    * I know, we're going to create two pixmaps for each and every
    * shadow'd widget.  Yeuck.  I'm semi-relying on server side
    * pixmap cacheing.
    */

    pm_data = mtshadowpm_bits;
    if (DefaultDepthOfScreen(scn) == 1) {
	top_fg_pixel = BlackPixelOfScreen(scn);
	top_bg_pixel = WhitePixelOfScreen(scn);
	create_pixmap = TRUE;
    } else if (threeD->be_nice_to_cmap) {
	if (bg == WhitePixelOfScreen(scn)) {
	    top_fg_pixel = WhitePixelOfScreen(scn);
	    top_bg_pixel = BlackPixelOfScreen(scn);
	} else if (bg == BlackPixelOfScreen(scn)) {
	    top_fg_pixel = BlackPixelOfScreen(scn);
	    top_bg_pixel = WhitePixelOfScreen(scn);
	} else {
	    top_fg_pixel = bg;
	    top_bg_pixel = WhitePixelOfScreen(scn);
	}
	if (bg != WhitePixelOfScreen(scn) && bg != BlackPixelOfScreen(scn))
	    pm_data = shadowpm_bits;
	create_pixmap = TRUE;
    }
    if (create_pixmap)
	threeD->top_shadow_pxmap = XCreatePixmapFromBitmapData(dpy,
						 RootWindowOfScreen(scn),
							       pm_data,
							  shadowpm_width,
							 shadowpm_height,
							    top_fg_pixel,
							    top_bg_pixel,
					      DefaultDepthOfScreen(scn));
}

/* ARGSUSED */
static void
AllocBotShadowPixmap(new, threeD, bg)
	Widget              new;
	_XpmThreeDFrame    *threeD;	/* Structure to play in */
	Pixel               bg;
{
    Display            *dpy = XtDisplayOfObject(new);
    Screen             *scn = XtScreenOfObject(new);
    unsigned long       bot_fg_pixel = 0, bot_bg_pixel = 0;
    char               *pm_data;
    Boolean             create_pixmap = FALSE;

    pm_data = mbshadowpm_bits;
    if (DefaultDepthOfScreen(scn) == 1) {
	bot_fg_pixel = BlackPixelOfScreen(scn);
	bot_bg_pixel = WhitePixelOfScreen(scn);
	create_pixmap = TRUE;
    } else if (threeD->be_nice_to_cmap) {
	if (bg == WhitePixelOfScreen(scn)) {
	    bot_fg_pixel = WhitePixelOfScreen(scn);
	    bot_bg_pixel = BlackPixelOfScreen(scn);
	} else if (bg == BlackPixelOfScreen(scn)) {
	    bot_fg_pixel = BlackPixelOfScreen(scn);
	    bot_bg_pixel = WhitePixelOfScreen(scn);
	} else {
	    bot_fg_pixel = bg;
	    bot_bg_pixel = BlackPixelOfScreen(scn);
	}
	if (bg != WhitePixelOfScreen(scn) && bg != BlackPixelOfScreen(scn))
	    pm_data = shadowpm_bits;
	create_pixmap = TRUE;
    }
    if (create_pixmap)
	threeD->bot_shadow_pxmap = XCreatePixmapFromBitmapData(dpy,
						 RootWindowOfScreen(scn),
							       pm_data,
							  shadowpm_width,
							 shadowpm_height,
							    bot_fg_pixel,
							    bot_bg_pixel,
					      DefaultDepthOfScreen(scn));
}
