/*
 * Slider widget.
 *
 * Draw a moveable slider. Unlike scroller thumb is of fixed size and there
 * is no arrows.
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
 * $Id: SliderP.h,v 1.1 1997/10/08 04:08:42 rich Exp rich $
 *
 * $Log: SliderP.h,v $
 * Revision 1.1  1997/10/08 04:08:42  rich
 * Initial revision
 *
 *
 */

#ifndef _SliderP_H_
#define _SliderP_H_
#include "Slider.h"
#include "threeDdrawP.h"

/*********************************************************************
 *
 * Slider Widget Private Data
 *
 *********************************************************************/

/* New fields for the Slider widget class record */

typedef struct {
    int                 foo;	/* Null record entry */
} SliderClassPart;

/* Full class record declaration */
typedef struct _SliderClassRec {
    CoreClassPart       core_class;
    SliderClassPart     slider_class;
} SliderClassRec;

/* New fields for the Slider widget Record */
typedef struct {
    /* resources */
    XtOrientation	orientation;
    int		    	min;		/* Minium output */
    int			max;		/* Maximum output */
    Dimension		thickness;	/* How wide to make thumb */
    Pixel               foreground;
    XtCallbackList	callbacks;
    String		clue;
    Cursor		horCursor;
    Cursor		verCursor;

   /* Shadow info */
    _XpmThreeDFrame	threeD;

   /* private resources. */
    int			t_top;		/* Top of thumb */
    int			t_bottom;	/* Bottom of thumb */
    int			position;	/* Current position */
    int			oposition;	/* Last reported position */
    GC                  norm_gc;	/* normal color gc. */
    GC                  gray_gc;	/* Normal color (grayed out) gc. */

} SliderPart;

/* Full instance record declaration */
typedef struct _SliderRec {
    CorePart            core;
    SliderPart        	slider;
} SliderRec;

extern SliderClassRec sliderClassRec;

#endif /* _SliderP_H_ */
