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
 * $Id: CanvasP.h,v 1.1 1997/10/19 01:41:37 rich Exp rich $
 *
 * $Log: CanvasP.h,v $
 * Revision 1.1  1997/10/19 01:41:37  rich
 * Initial revision
 *
 *
 *
 */

#ifndef _CanvasP_H_
#define _CanvasP_H_
#include "Canvas.h"

typedef struct {
/* methods */
    int                 foo;	/* Null record entry */
/* class variables */
} CanvasClassPart;

typedef struct _CanvasClassRec {
    CoreClassPart       core_class;
    CanvasClassPart     canvas_class;
} CanvasClassRec;

typedef struct {
   /* resources */
    XtCallbackList	canvas_action;
    XtCallbackList	expose_call;
    XtCallbackList	resize_call;
    XpwCanvasResizePolicy resizePolicy;
    String		clue;
} CanvasPart;

typedef struct _CanvasRec {
    CorePart            core;
    CanvasPart          canvas;
} CanvasRec;

extern CanvasClassRec canvasClassRec;

#endif /* _CanvasP_H_ */
