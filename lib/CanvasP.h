/*
 * Canvas widget.
 *
 * Blank widget, with callbacks for expose. 
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
 * $Id: CanvasP.h,v 1.3 1997/11/01 06:39:01 rich Beta $
 *
 * $Log: CanvasP.h,v $
 * Revision 1.3  1997/11/01 06:39:01  rich
 * Cleaned up comments.
 *
 * Revision 1.2  1997/10/21 18:50:30  rich
 * Removed reference to 3D library.
 *
 * Revision 1.1  1997/10/19 01:41:37  rich
 * Initial revision
 *
 *
 *
 */

#ifndef _CanvasP_H_
#define _CanvasP_H_
#include "Canvas.h"

/*********************************************************************
 *
 * Canvas Widget Private Data
 *
 *********************************************************************/

/* New fields for the Canvas widget class record */

typedef struct {
    int                 foo;	/* Null record entry */
} CanvasClassPart;

/* Full class record declaration */
typedef struct _CanvasClassRec {
    CoreClassPart       core_class;
    CanvasClassPart     canvas_class;
} CanvasClassRec;

/* New fields for the Canvas widget Record */
typedef struct {
   /* resources */
    XtCallbackList	canvas_action;
    XtCallbackList	expose_call;
    XtCallbackList	resize_call;
    XpwCanvasResizePolicy resizePolicy;
    String		clue;
} CanvasPart;

/* Full instance record declaration */
typedef struct _CanvasRec {
    CorePart            core;
    CanvasPart          canvas;
} CanvasRec;

extern CanvasClassRec canvasClassRec;

#endif /* _CanvasP_H_ */
