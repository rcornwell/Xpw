/*
 * Bitmap converter and cache.
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
 * $Log: $
 *
 */

#ifndef lint
static char         rcsid[] = "$Id: $";

#endif

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xos.h>
#include <X11/Xmu/Drawing.h>
#include "bitmapcache.h"
#include <stdio.h>

typedef struct _BitmapList {
	char		*name;
	unsigned char	*data;
	Dimension	width;
	Dimension	height;
} BitmapList;

typedef struct _BitmapCache {
	char		*name;
	Pixmax		data;
	Display		dpy;
	Window		win;
	int		refcount;
	struct		_BitmapCache	left, right;
} BitmapCache;

#define BitmapEntry(name)	{name, name##_bits, name##_width, name##_height},
XpwSetBitmapPath(path)
char	*path;
{

}


XtSetTypeConverter(XtRString, XtRBitmap, cvtStringToBitmap,
			   NULL, 0, XtCacheNone, NULL);

XpwLoadBuiltinBitmaps(blist)
BitmapList	*blist;
{
}

/************************************************************
 *
 * Type converters.
 *
 ************************************************************/

Boolean
cvtStringToBitmap(dpy, args, num_args, from, to, converter_data)
	Display            *dpy;
	XrmValuePtr         args;
	Cardinal           *num_args;
	XrmValuePtr         from;
	XrmValuePtr         to;
	XtPointer          *converter_data;
{
    String              s = (String) from->addr;
    static Bitmap    result;

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


    dpy = XtDisplay(toplevel);
    win = DefaultRootWindow(dpy);
   /* Make sure we have a icon */
    image = None;
    XtSetArg(args[0], XtNiconPixmap, &image);
    XtSetArg(args[1], XtNiconic, &iconic);
    XtGetValues(toplevel, args, 2);
    if (image == None) {
	image = XCreateBitmapFromData(dpy,
				  win,
		   (char *) mixicon_bits, mixicon_width, mixicon_height);
