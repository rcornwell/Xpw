/*
 * Arrow widget.
 *
 * The Arrow widget draws a arrow in a specified direction, and returns
 * button press events.
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
 * $Id: ArrowP.h,v 1.1 1997/10/04 18:59:34 rich Exp rich $
 *
 * $Log: ArrowP.h,v $
 * Revision 1.1  1997/10/04 18:59:34  rich
 * Initial revision
 *
 *
 */

#ifndef _ArrowP_H_
#define _ArrowP_H_
#include "Arrow.h"
#include "threeDdrawP.h"

/*********************************************************************
 *
 * Arrow Widget Private Data
 *
 *********************************************************************/

/* New fields for the Arrow widget class record */

typedef struct {
    int                 foo;	/* Null record entry */
} ArrowClassPart;

/* Full class record declaration */
typedef struct _ArrowClassRec {
    CoreClassPart       core_class;
    ArrowClassPart      arrow_class;
} ArrowClassRec;

/* New fields for the Arrow widget Record */
typedef struct {
   /* Resources */
   
   /* Public Resources */
    ShapeType		direction;
    int			delayTime;
    int			repeatTime;
    Pixel               foreground;     /* foreground color. */
    XtCallbackList callbacks;

   /* private resources. */
    Boolean		set;

    GC                  norm_GC;        /* normal color gc. */
    GC                  inverse_GC;     /* set color gc. */
    GC                  norm_gray_GC;   /* Normal color (grayed out) gc. */
    XtIntervalId        timer;          /* Timer for repeat events */

   /* Shadow info */
    _XpmThreeDFrame	threeD;
} ArrowPart;

/* Full instance record declaration */
typedef struct _ArrowRec {
    CorePart            core;
    ArrowPart           arrow;
} ArrowRec;

extern ArrowClassRec arrowClassRec;

#endif /* _ArrowP_H_ */
