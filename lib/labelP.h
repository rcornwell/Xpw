/* 
 * Text label drawing routines.
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
 */

/*
 * $Id$
 *
 * $Log:$
 *
 */

#ifndef _XpwlabelP_H
#define _XpwlabelP_H

#include  "label.h"

/* New fields for the label entries */
typedef struct {
   /* resources */
    Boolean             international;
    String              label;	/* The entry label. */
    int                 vert_space;	/* extra vert space to leave, as a
					 * percentage of the font height of the
					 * label. */
    Pixmap		bitmap;		/* Main bitmap */
    Pixmap              left_bitmap, right_bitmap;	/* bitmaps to show. */
    Dimension           left_margin, right_margin;	/* left and right margins. */
    Pixel               foreground;	/* foreground color. */
    XFontStruct        *font;		/* The font to show label in. */
    XFontSet            fontset;	/* or fontset */
    XtJustify           justify;	/* Justification for the label. */

/* private resources. */

    GC                  norm_gc;	/* noral color gc. */
    GC                  norm_gray_gc;	/* Normal color (grayed out) gc. */

    Dimension           bitmap_width;	/* size of each bitmap. */
    Dimension           bitmap_height;
    Dimension           left_bitmap_width;
    Dimension           left_bitmap_height;
    Dimension           right_bitmap_width;
    Dimension           right_bitmap_height;
    Dimension		label_width;
    Dimension		label_height;

} _XpwLabel;

/* Quickly insert the resouces. Assumes offset already defined */
#define	labelOffset(field)	offset(label.field)

#define LabelResources \
    {XtNinternational, XtCInternational, XtRBoolean, sizeof(Boolean),   \
     labelOffset(international), XtRImmediate, (XtPointer) FALSE},      \
    {XtNlabel, XtCLabel, XtRString, sizeof(String),                     \
     labelOffset(label), XtRString, NULL},                              \
    {XtNbitmap, XtCBitmap, XtRBitmap, sizeof(Pixmap),                   \
     labelOffset(bitmap), XtRImmediate, (XtPointer)None},               \
    {XtNvertSpace, XtCVertSpace, XtRInt, sizeof(int),                   \
     labelOffset(vert_space), XtRImmediate, (XtPointer) 25},            \
    {XtNleftBitmap, XtCLeftBitmap, XtRBitmap, sizeof(Pixmap),           \
     labelOffset(left_bitmap), XtRImmediate, (XtPointer) None},         \
    {XtNjustify, XtCJustify, XtRJustify, sizeof(XtJustify),             \
     labelOffset(justify), XtRImmediate, (XtPointer) XtJustifyLeft},    \
    {XtNrightBitmap, XtCRightBitmap, XtRBitmap, sizeof(Pixmap),         \
     labelOffset(right_bitmap), XtRImmediate, (XtPointer) None},        \
    {XtNleftMargin, XtCHorizontalMargins, XtRDimension, sizeof(Dimension), \
     labelOffset(left_margin), XtRImmediate, (XtPointer) 4},               \
    {XtNrightMargin, XtCHorizontalMargins, XtRDimension, sizeof(Dimension),\
     labelOffset(right_margin), XtRImmediate, (XtPointer) 4},           \
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),             \
     labelOffset(foreground), XtRString, XtDefaultForeground},          \
    {XtNfont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),            \
     labelOffset(font), XtRString, XtDefaultFont},                      \
    {XtNfontSet, XtCFontSet, XtRFontSet, sizeof(XFontSet),              \
     labelOffset(fontset), XtRString, XtDefaultFontSet},     

_XFUNCPROTOBEGIN

extern void _XpwLabelClassInit();
extern void _XpwLabelInit(
#if NeedFunctionPrototypes
	Widget              new,
	_XpwLabel	    *label,
	Pixel		    bg,
	int		    depth
#endif
);
extern void _XpwLabelDestroy(
#if NeedFunctionPrototypes
	Widget              w,
	_XpwLabel	    *label
#endif
);
extern void _XpwLabelDraw(
#if NeedFunctionPrototypes
	Widget              w,
	_XpwLabel	   *label,
	XEvent             *event,
	Region              region,
	Dimension	   x,
	Dimension	   y,
	Dimension	   wi,
	Dimension	   hi,
        Boolean		   dobg
#endif
);
extern Boolean _XpwLabelSetValues(
#if NeedFunctionPrototypes
	Widget              current,
	Widget	 	    new,
	_XpwLabel	    *old_label,
	_XpwLabel	    *label,
	Pixel		    bg,
	int		    depth
#endif
);

extern void _XpwLabelDefaultSize(
#if NeedFunctionPrototypes
	Widget              w,
	_XpwLabel	    *label,
	Dimension          *width,
	Dimension	   *height
#endif
);

_XFUNCPROTOEND

#endif 
