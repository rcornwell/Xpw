/*
 * Three D frame helper library.
 *
 * Library of internal routines for doing 3D frame and outline drawing.
 * This library includes GC caching to improve preformace.
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
 */

/*
 * $Id: $
 *
 * $Log: $
 *
 */

#ifndef _threeDdrawP_H_
#define _threeDdrawP_H_

#include "threeDdraw.h"

typedef struct {
   /* Shadow info */
    Dimension           shadow_width;		/* How wide is the shadow */
    Pixel               top_shadow_pixel;	/* Pixel to use for drawing */
    Pixel               bot_shadow_pixel;
    int                 top_shadow_contrast;	/* Contrast between fg/bg */
    int                 bot_shadow_contrast;
    Pixmap		top_shadow_pxmap;
    Pixmap		bot_shadow_pxmap;
    Boolean             be_nice_to_cmap;	
    GC                  top_shadow_GC;
    GC                  bot_shadow_GC;
    GC                  erase_GC;
    Boolean             raised;
    FrameType           frameType;
} _XpmThreeDFrame;

/*
 * These are here since they are private to the widget library.
 * Nobody should be calling these.
 */

_XFUNCPROTOBEGIN

extern void _XpwThreeDClassInit();
extern void _XpwThreeDInit(
#if NeedFunctionPrototypes
	Widget	new,
	_XpmThreeDFrame	*threeD,
	Pixel	bg
#endif
);

extern Boolean _XpmThreeDSetValues(
#if NeedFunctionPrototypes
	Widget		w,
	_XpmThreeDFrame	*current, 
	_XpmThreeDFrame *new,
	Pixel		bg
#endif
);

extern void         _XpwThreeDDrawShadow(
#if NeedFunctionPrototypes
	Widget		wid,
	XEvent		*event,
	Region		region,
	_XpmThreeDFrame	*threeD,
	Dimension	x,
	Dimension	y,
	Dimension	w,
	Dimension	h,
	Boolean		comp
#endif
);

extern void         _XpwThreeDEraseShadow(
#if NeedFunctionPrototypes
	Widget		wid,
	XEvent		*event,
	Region		region,
	_XpmThreeDFrame	*threeD,
	Dimension	x,
	Dimension	y,
	Dimension	w,
	Dimension	h
#endif
);

extern void         _XpwThreeDDrawShape(
#if NeedFunctionPrototypes
	Widget		wid,
	XEvent		*event,
	Region		region,
	_XpmThreeDFrame	*threeD,
	GC		gc,
	ShapeType	shape,
	Dimension	x,
	Dimension	y,
	Dimension	w,
	Dimension	h,
	Boolean		comp
#endif
);


extern void _XpwThreeDDestroyShadow(
#if NeedFunctionPrototypes
	Widget		w,
	_XpmThreeDFrame	*threeD 
#endif
);

extern void _XpwThreeDAllocateShadowGC(
#if NeedFunctionPrototypes
	Widget		w,
	_XpmThreeDFrame *threeD,
	Pixel		 bg
#endif
);

_XFUNCPROTOEND

/* Our resources */
#define XtNtopShadowPixmap "topShadowPixmap"
#define XtCTopShadowPixmap "TopShadowPixmap"
#define XtNbottomShadowPixmap "bottomShadowPixmap"
#define XtCBottomShadowPixmap "BottomShadowPixmap"
#define XtNshadowed "shadowed"
#define XtCShadowed "Shadowed"

/* Quickly insert the resources. Assumes offset is already defined */
#define threeDoffset(field) offset(threeD.field)

#define threeDresources	\
    {XtNraised, XtCraised, XtRBoolean, sizeof(Boolean),                     \
     threeDoffset(raised), XtRImmediate, (XtPointer) TRUE},                 \
    {XtNframeType, XtCFrameType, XtRFrameType, sizeof(FrameType),           \
     threeDoffset(frameType), XtRImmediate, (XtPointer) XfBeveled},         \
    {XtNtopShadowPixel, XtCTopShadowPixel, XtRPixel, sizeof(Pixel),         \
     threeDoffset(top_shadow_pixel), XtRString, XtDefaultForeground},       \
    {XtNbottomShadowPixel, XtCBottomShadowPixel, XtRPixel, sizeof(Pixel),   \
     threeDoffset(bot_shadow_pixel), XtRString, XtDefaultForeground},       \
    {XtNtopShadowPixmap, XtCTopShadowPixmap, XtRPixmap, sizeof(Pixmap),     \
     threeDoffset(top_shadow_pxmap), XtRImmediate, (XtPointer) NULL},       \
    {XtNbottomShadowPixmap, XtCBottomShadowPixmap, XtRPixmap, sizeof(Pixmap), \
     threeDoffset(bot_shadow_pxmap), XtRImmediate, (XtPointer) NULL},       \
    {XtNtopShadowContrast, XtCTopShadowContrast, XtRInt, sizeof(int),       \
     threeDoffset(top_shadow_contrast), XtRImmediate, (XtPointer) 20},      \
    {XtNbottomShadowContrast, XtCBottomShadowContrast, XtRInt, sizeof(int), \
     threeDoffset(bot_shadow_contrast), XtRImmediate, (XtPointer) 40},      \
    {XtNshadowWidth, XtCShadowWidth, XtRDimension, sizeof(Dimension),       \
     threeDoffset(shadow_width), XtRImmediate, (XtPointer) 2},              \
    {XtNbeNiceToColormap, XtCBeNiceToColormap, XtRBoolean, sizeof(Boolean), \
     threeDoffset(be_nice_to_cmap), XtRImmediate, (XtPointer) True},        

#endif
