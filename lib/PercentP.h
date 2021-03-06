/*
 * Percent widget.
 *
 * Draw a percent graph.
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
 * $Id: PercentP.h,v 1.2 1997/11/01 06:39:05 rich Beta $
 *
 * $Log: PercentP.h,v $
 * Revision 1.2  1997/11/01 06:39:05  rich
 * Cleaned up comments.
 *
 * Revision 1.1  1997/10/08 04:03:06  rich
 * Initial revision
 *
 *
 */

#ifndef _PercentP_H_
#define _PercentP_H_
#include "Percent.h"
#include "threeDdrawP.h"

/*********************************************************************
 *
 * Percent Widget Private Data
 *
 *********************************************************************/

/* New fields for the Percent widget class record */

typedef struct {
    int                 foo;	/* Null record entry */
} PercentClassPart;


/* Full class record declaration */
typedef struct _PercentClassRec {
    CoreClassPart       core_class;
    PercentClassPart    percent_class;
} PercentClassRec;

/* New fields for the Percent widget Record */
typedef struct {
   /* resources */
    XtOrientation       orientation;
    Boolean             international;
    Boolean             showValue;
    Boolean		state;
    Pixel               foreground;     /* foreground color. */
    Pixel               thumb; 		/* thumb color. */
    XFontStruct        *font;           /* The font to show label in. */
    XFontSet            fontset;        /* or fontset */
    XtJustify           justify;        /* Justification for the label. */
    int			thickness;	/* Size of thumb */
    int                 vert_space;     /* extra vert space to leave, as a
                                         * percentage of the font height of the
                                         * label. */
    int			min;		/* Min value */
    int			max;		/* Max value */
    int			position;	/* Position */
    String		clue;

   /* Shadow info */
    _XpmThreeDFrame	threeD;

   /* private resources. */
    GC                  norm_gc;        /* Noral color gc. */
    GC                  norm_gray_gc;   /* Normal color (grayed out) gc. */
    GC                  thumb_gc;       /* Marker color gc. */
    GC                  thumb_gray_gc;  /* Marker color (grayed out) gc. */

} PercentPart;

/* Full instance record declaration */
typedef struct _PercentRec {
    CorePart            core;
    PercentPart         percent;
} PercentRec;

extern PercentClassRec percentClassRec;

#endif /* _PercentP_H_ */
