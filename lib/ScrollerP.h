/*
 * Scroller widget.
 *
 * Draw a scroller with a border around it.
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

#ifndef _ScrollerP_H_
#define _ScrollerP_H_
#include "Scroller.h"
#include "threeDdrawP.h"

typedef struct {
/* methods */
    int                 foo;	/* Null record entry */
/* class variables */
} ScrollerClassPart;

typedef struct _ScrollerClassRec {
    CoreClassPart       core_class;
    CompositeClassPart  composite_class;
    ScrollerClassPart   scroller_class;
} ScrollerClassRec;

typedef struct {
    /* resources */
    XtOrientation	orientation;
    Boolean		usearrows;
    int		    	tlenght;	/* Total units high */
    int			tshown;		/* Total units showing */
    Dimension		min_thumb;	/* Thumb never gets any smaller */
    Dimension		thickness;	/* How wide to make thumb */
    int			delayTime;	/* Delay time for arrows */
    int			repeatTime;	/* Repeat time for arrows */

    Pixel               foreground;
    Pixel		thumb;
    XtCallbackList	callbacks;

/* private resources. */
    Widget		tr_arrow;
    Widget		bl_arrow;
    int			t_top;		/* Top of thumb */
    int			t_bottom;	/* Bottom of thumb */
    int			position;	/* Current position */
    GC                  norm_gc;	/* normal color gc. */
    GC                  thumb_gc;	/* Thumb GC */
    GC                  gray_gc;	/* Normal color (grayed out) gc. */
    GC                  gray_thumb_gc;	/* Gray Thumb GC */

   /* Shadow info */
    _XpmThreeDFrame	threeD;

} ScrollerPart;

typedef struct _ScrollerRec {
    CorePart            core;
    CompositePart       composite;
    ScrollerPart        scroller;
} ScrollerRec;

extern ScrollerClassRec scrollerClassRec;

#endif /* _ScrollerP_H_ */
