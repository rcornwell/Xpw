/*
 * Label drawing stubs. 
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
 * $Log: label.c,v $
 * Revision 1.2  1997/11/01 06:39:10  rich
 * Fixed sensitivity logic.
 *
 * Revision 1.1  1997/10/12 05:20:24  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char         rcsid[] = "$Id: label.c,v 1.2 1997/11/01 06:39:10 rich Beta rich $";

#endif

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xos.h>
#include <X11/Xmu/Drawing.h>
#include "labelP.h"
#include <stdio.h>

/* label.c */

/* Private functions */
static void         GetBitmapInfo(Widget /*w */ , Pixmap /*bitmap */ ,
		       Dimension * /*width */ , Dimension * /*height */ ,
				  char * /*what */ );
static void         GetTextInfo(Widget /*w */ , _XpwLabel * /*label */ );
static void         CreateGCs(Widget /*w */ , _XpwLabel * /*label */ ,
			      Pixel /*bg */ , int /*depth */ );
static void         DestroyGCs(Widget /*w */ , _XpwLabel * /*label */ );

/* 
 * Do initialization for label pseudo class.
 */

void
_XpwLabelClassInit()
{
    static	int	install = 1;
    if (install) {
	install = 0;
	XtAddConverter(XtRString, XtRJustify, XmuCvtStringToJustify,
		   (XtConvertArgList) NULL, (Cardinal) 0);
    }
}

/* 
 * Initialize a label pseudo class.
 */

/* ARGSUSED */
void
_XpwLabelInit(new, label, bg, depth)
	Widget              new;
	_XpwLabel          *label;
	Pixel               bg;
	int                 depth;
{
    if (label->label == NULL)
        label->label = XtNewString(XtName(new));
    else
        label->label = XtNewString(label->label);

    CreateGCs(new, label, bg, depth);

    GetTextInfo(new, label);

    GetBitmapInfo(new, label->bitmap, &label->bitmap_width,
		  &label->bitmap_height, "");
    GetBitmapInfo(new, label->left_bitmap, &label->left_bitmap_width,
		  &label->left_bitmap_height, "Left");
    GetBitmapInfo(new, label->right_bitmap, &label->right_bitmap_width,
		  &label->right_bitmap_height, "Right");
}

/*  
 * Handle set resource requests for label psuedo class.
 */

/* ARGSUSED */
Boolean
_XpwLabelSetValues(current, new, old_label, label, bg, depth)
	Widget              current, new;
	_XpwLabel          *old_label, *label;
	Pixel               bg;
	int                 depth;
{
    Boolean             ret_val = FALSE;

    if (old_label->label != label->label) {
	if (old_label->label != NULL)
	    XtFree((char *) old_label->label);

	label->label = XtNewString(label->label);

	GetTextInfo(new, label);
	ret_val = True;
    }
    if (label->bitmap != old_label->bitmap) {
	GetBitmapInfo(new, label->bitmap, &label->bitmap_width,
		      &label->bitmap_height, "");
	ret_val = TRUE;
    }
    if (label->left_bitmap != old_label->left_bitmap) {
	GetBitmapInfo(new, label->left_bitmap, &label->left_bitmap_width,
		      &label->left_bitmap_height, "Left");
	ret_val = TRUE;
    }
    if (label->right_bitmap != old_label->right_bitmap) {
	GetBitmapInfo(new, label->right_bitmap, &label->right_bitmap_width,
		      &label->right_bitmap_height, "Right");
	ret_val = TRUE;
    }
    if (((old_label->font != label->font) &&
	 (old_label->international == False)) ||
	(old_label->foreground != label->foreground)) {
	DestroyGCs(current, old_label);
	CreateGCs(new, label, bg, depth);
	ret_val = TRUE;
    }
    if ((old_label->fontset != label->fontset) &&
	(old_label->international == True))
       /* DONT changes the GCs, because the fontset is not in them. */
	ret_val = TRUE;

    if (old_label->justify != label->justify)
	ret_val = TRUE;

    return (ret_val);
}

/* 
 * Return default size for label psuedo class.
 */

void
_XpwLabelDefaultSize(w, label, width, height)
	Widget              w;
	_XpwLabel          *label;
	Dimension          *width, *height;
{
    *width = label->left_margin + label->right_margin;
    *height = 0;
    if (label->bitmap != None) {
	*width += label->bitmap_width;
	*height += label->bitmap_height;
    } else {
	GetTextInfo(w, label);	/* For Clue widget help */
	*width += label->label_width;
	*height += label->label_height;
    }
    *height = ((int) *height * (100 + label->vert_space)) / 100;
    if (label->left_margin == 0 && label->left_bitmap != None)
	*width += label->left_bitmap_width;
    if (label->right_margin == 0 && label->right_bitmap != None)
	*width += label->right_bitmap_width;

    if (label->left_bitmap != None && *height < label->left_bitmap_height)
	*height = label->left_bitmap_height;
    if (label->right_bitmap != None && *height < label->right_bitmap_height)
	*height = label->right_bitmap_height;
}

/*
 * Redisplay a label psuedo class.
 */

#define abs(x)	(((x)<0)?-(x):(x))

/* ARGSUSED */
void
_XpwLabelDraw(w, label, event, region, x, y, wi, hi, dobg)
	Widget              w;
	_XpwLabel          *label;
	XEvent             *event;
	Region              region;
	Dimension           x, y;
	Dimension           wi, hi;
	Boolean             dobg;
{
    GC                  gc;
    int                 font_ascent = 0, font_descent = 0, x_loc, y_loc;
    XFontSetExtents    *ext = XExtentsOfFontSet(label->fontset);
    Display            *dpy = XtDisplayOfObject(w);
    Window              win = XtWindowOfObject(w);
    int                 width, th, t_width;
    char               *p, *p1;

   /* Set the context based on sensitivity setting */
    if (XtIsSensitive(w))
	gc = label->norm_gc;
    else
	gc = label->norm_gray_gc;

    x_loc = x + label->left_margin;
   /* Bitmap overrides label */
    if (label->bitmap != None) {
	t_width = label->bitmap_width;

	switch (label->justify) {
	case XtJustifyCenter:
	    width = wi - (label->left_margin + label->right_margin);
	    x_loc += (width - t_width) / 2;
	    break;
	case XtJustifyRight:
	    x_loc += wi - (label->right_margin + t_width);
	    break;
	case XtJustifyLeft:
	default:
	    break;
	}
	y_loc = y + (int) (hi - label->bitmap_height) / 2;

	if (region == NULL || XRectInRegion(region, (int) x_loc, (int) y_loc,
				      (unsigned int) label->bitmap_width,
				     (unsigned int) label->bitmap_height)
	    != RectangleOut) {
	    if (dobg)
		XClearArea(dpy, win, x_loc, y_loc, label->bitmap_width,
			   label->bitmap_height, FALSE);
	    XCopyPlane(dpy, label->bitmap, win, gc, 0, 0,
		       label->bitmap_width, label->bitmap_height,
		       x_loc, y_loc, 1);
	}
    } else {
	if (label->label != NULL) {
	    int                 len;

	   /* Cache font info */
	    if (label->international == True) {
		font_ascent = abs(ext->max_ink_extent.y);
		font_descent = ext->max_ink_extent.height - font_ascent;
	    } else {		/*else, compute size from font like R5 */
		font_ascent = label->font->max_bounds.ascent;
		font_descent = label->font->max_bounds.descent;
	    }
	    th = font_ascent + font_descent;

	   /* Justify label */
	    y_loc = y;
	    t_width = label->label_width;
	    switch (label->justify) {
	    case XtJustifyCenter:
		width = wi - (label->left_margin + label->right_margin);
		x_loc += (width - t_width) / 2;
		break;
	    case XtJustifyRight:
		x_loc += wi - (label->right_margin + t_width);
		break;
	    case XtJustifyLeft:
	    default:
		break;
	    }

	   /* this will center the text in the gadget top-to-bottom */

	    y_loc += (hi - label->label_height)/2; 
	    y_loc += th + 2;
	    p1 = p = label->label;
	    len = 0;
	    do {
		len++;
		if (*p != '\n' && *p != '\0')
		    continue;
		len--;
		if (region == NULL || XRectInRegion(region,
			       (int) x_loc, (int) y_loc,
			       (unsigned int) t_width, (unsigned int) th)
		    != RectangleOut) {
		    if (dobg)
			XClearArea(dpy, win, x_loc, y_loc - th,
						 t_width, th, FALSE);
		    if (label->international == True)
			XmbDrawString(dpy, win, label->fontset, gc,
				 x_loc, y_loc, p1, len);
		    else
			XDrawString(dpy, win, gc, x_loc, y_loc, p1, len);
		}
		len = 0;
	        p1 = p;
	        p1++;
	        y_loc += th + 2;
	    } while (*p++ != '\0');
	}
    }

/*
 * Draw Left Bitmap.
 */

    if (label->left_bitmap != None) {
	x_loc = (int) (x + label->left_margin - label->left_bitmap_width) / 2;

	y_loc = y + (int) (hi - label->left_bitmap_height) / 2;

	if (region == NULL || XRectInRegion(region, (int) x_loc, (int) y_loc,
				 (unsigned int) label->left_bitmap_width,
				(unsigned int) label->left_bitmap_height)
	    != RectangleOut) {
	    if (dobg)
		XClearArea(dpy, win, x_loc, y_loc, label->left_bitmap_width,
			   label->left_bitmap_height, FALSE);
	    XCopyPlane(dpy, label->left_bitmap, win, gc, 0, 0,
		       label->left_bitmap_width,
		       label->left_bitmap_height, x_loc, y_loc, 1);
	}
    }
/*
 * Draw Right Bitmap.
 */

    if (label->right_bitmap != None) {
	x_loc = wi - (int) (label->right_margin + label->right_bitmap_width) / 2;

	y_loc = y + (int) (hi - label->right_bitmap_height) / 2;

	if (region == NULL || XRectInRegion(region, (int) x_loc, (int) y_loc,
				(unsigned int) label->right_bitmap_width,
			       (unsigned int) label->right_bitmap_height)
	    != RectangleOut) {
	    if (dobg)
		XClearArea(dpy, win, x_loc, y_loc, label->right_bitmap_width,
			   label->right_bitmap_height, FALSE);
	    XCopyPlane(dpy, label->right_bitmap, win, gc, 0, 0,
		       label->right_bitmap_width,
		       label->right_bitmap_height, x_loc, y_loc, 1);
	}
    }
}

/*  
 * Free resources used by lable pseudo class.
 */

void
_XpwLabelDestroy(w, label)
	Widget              w;
	_XpwLabel          *label;
{
    DestroyGCs(w, label);
    if (label->label != XtName(w))
	XtFree(label->label);
}

/************************************************************
 *
 * Private functions
 *
 ************************************************************/

/*  
 * Gets the size of a bitmap.
 */

static void
GetBitmapInfo(wid, bitmap, width, height, what)
	Widget              wid;
	Pixmap              bitmap;
	Dimension          *width;
	Dimension          *height;
	char               *what;
{
    unsigned int        depth, bw;
    Window              root;
    int                 x, y;
    unsigned int        w, h;
    char                buf[BUFSIZ];

    if (bitmap != None) {
	if (!XGetGeometry(XtDisplayOfObject(wid),
			  bitmap, &root,
			  &x, &y, &w, &h, &bw, &depth)) {
	    strcpy(buf, "Xpw label for object: Could not get ");
	    strcat(buf, what);
	    strcat(buf, " Bitmap geometry information \"");
	    strcat(buf, XtName(wid));
	    strcat(buf, "\"");
	    XtAppError(XtWidgetToApplicationContext(wid), buf);
	}
	if (depth != 1) {
	    strcpy(buf, "Xpw label for object: ");
	    strcat(buf, what);
	    strcat(buf, " Bitmap of \"");
	    strcat(buf, XtName(wid));
	    strcat(buf, "\" is not one bit deep.");
	    XtAppError(XtWidgetToApplicationContext(wid), buf);
	}
	*width = (Dimension) w;
	*height = (Dimension) h;
    } else {
	*width = (Dimension) 0;
	*height = (Dimension) 0;
    }
}

/*
 * Sets the width and height of the label part of widget.
 */

static void
GetTextInfo(wid, label)
	Widget              wid;
	_XpwLabel          *label;
{
    XFontSetExtents    *ext = XExtentsOfFontSet(label->fontset);
    char               *p, *p1;
    int                 len, w, h, wt;

   /* Make sure we have a label */
    if (label->label == NULL || *label->label == '\0') {
	label->label_width = 0;
	label->label_height = 0;
	return;
    }
    w = h = len = 0;
    p1 = p = label->label;
    do {
	len++;
	if (*p != '\n' && *p != '\0')
	    continue;
	len--;
	if (label->international == True) {
	    wt = XmbTextEscapement(label->fontset, p1, len);
	    h +=  2 + ext->max_ink_extent.height;
	} else {
	    wt = XTextWidth(label->font, p1, len);
	    h += 2 +(label->font->max_bounds.ascent +
		  label->font->max_bounds.descent);
	}
	if (wt > w)
	    w = wt;
	len = 0;
	p1 = p;
	p1++;
    } while (*p++ != '\0');
    label->label_width = w;
    label->label_height = h;
}

/*  
 *  Creates all gc's for the label pseudo class.
 */

static void
CreateGCs(w, label, bg, depth)
	Widget              w;
	_XpwLabel          *label;
	Pixel               bg;
	int                 depth;
{
    XGCValues           values;
    XtGCMask            mask, mask_i18n;

    values.foreground = label->foreground;
    values.background = bg;
    values.font = label->font->fid;
    values.graphics_exposures = FALSE;
    mask = GCForeground | GCBackground | GCGraphicsExposures | GCFont;
    mask_i18n = GCForeground | GCBackground | GCGraphicsExposures;

    if (label->international == True)
	label->norm_gc = XtAllocateGC(w, 0, mask_i18n, &values,
				      GCFont, 0);
    else
	label->norm_gc = XtGetGC(w, mask, &values);

    values.fill_style = FillTiled;
    values.tile = XmuCreateStippledPixmap(XtScreenOfObject(w),
					  label->foreground, bg, depth);
    values.graphics_exposures = FALSE;
    mask |= GCTile | GCFillStyle;
    if (label->international == True)
	label->norm_gray_gc = XtAllocateGC(w, 0, mask_i18n, &values,
					   GCFont, 0);
    else
	label->norm_gray_gc = XtGetGC(w, mask, &values);

}

/* 
 * Free GC's used by label psuede class.
 */

static void
DestroyGCs(w, label)
	Widget              w;
	_XpwLabel          *label;
{
    XtReleaseGC(w, label->norm_gc);
    XtReleaseGC(w, label->norm_gray_gc);
}
