/*
 * Frame widget.
 *
 * Draw a label with a border around it.
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
 * $Id: FrameP.h,v 1.1 1997/10/04 05:04:15 rich Exp rich $
 *
 * $Log: FrameP.h,v $
 * Revision 1.1  1997/10/04 05:04:15  rich
 * Initial revision
 *
 *
 */

#ifndef _FrameP_H_
#define _FrameP_H_
#include "Frame.h"
#include "labelP.h"
#include "threeDdrawP.h"

/*********************************************************************
 *
 * Frame Widget Private Data
 *
 *********************************************************************/

/* New fields for the Frame widget class record */

typedef struct {
    int                 foo;	/* Null record entry */
} FrameClassPart;

/* Full class record declaration */
typedef struct _FrameClassRec {
    CoreClassPart       core_class;
    CompositeClassPart  composite_class;
    FrameClassPart      frame_class;
} FrameClassRec;

/* New fields for the Frame widget Record */
typedef struct {
   /* resources */
    _XpwLabel		label;
    Dimension           frame_width;
    Boolean             labelontop;
    Boolean             outline;
    Boolean		usePrefered;

   /* Shadow info */
    _XpmThreeDFrame	threeD;
    _XpmThreeDFrame	threeO;

   /* Private info */
    Dimension		label_height;
} FramePart;

/* Full instance record declaration */
typedef struct _FrameRec {
    CorePart            core;
    CompositePart       composite;
    FramePart           frame;
} FrameRec;

extern FrameClassRec frameClassRec;

#endif /* _FrameP_H_ */
