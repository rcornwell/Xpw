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
 * $Id: Canvas.h,v 1.2 1997/10/21 18:50:30 rich Exp rich $
 *
 * $Log: Canvas.h,v $
 * Revision 1.2  1997/10/21 18:50:30  rich
 * Removed reference to 3D library.
 *
 * Revision 1.1  1997/10/19 01:41:37  rich
 * Initial revision
 *
 *
 */

#ifndef _Canvas_H_
#define _Canvas_H_

#include <X11/Xmu/Converters.h>

/* Canvas Widget Resources:
 * 
 * Name              Class              RepType         Default Value
 * ----              -----              -------         -------------
 * actionCallback    Callback           Pointer         NULL
 * resizeCallback    Callback           Pointer         NULL
 * exposeCallback    Callback           Pointer         NULL
 * resizePolicy      ResizePolicy       XpwCanvasResizePolicy         Any
 * clue              Label              String          NULL
 *
 * From Core:
 * 
 * Name                Class              RepType         Default Value
 * ----                -----              -------         -------------
 * destroyCallback     Callback           Pointer         NULL
 * borderWidth         BorderWidth        Dimension       0
 * sensitive           Sensitive          Boolean         True
 * height              Height             Dimension       0
 * width               Width              Dimension       0
 * x                   Position           Position        0
 * y                   Position           Position        0
 * 
 */

typedef struct _CanvasClassRec *CanvasWidgetClass;
typedef struct _CanvasRec *CanvasWidget;

extern WidgetClass  canvasWidgetClass;

typedef enum {
    XpwResize_Any, XpwResize_None, XpwResize_Grow
} XpwCanvasResizePolicy;

#define XtRResizePolicy "ResizePolicy"

#define XtNexposeCallback "exposeCallback"
#define XtNresizeCallback "resizeCallback"
#define XtNactionCallback "actionCallback"

#define XpwCanvasResize	0
#define XpwCanvasExpose	1
#define XpwCanvasAction	2

#define XtNresizePolicy "resizePolicy"
#define XtCResizePolicy "ResizePolicy"

typedef struct _XpwCanvasCallbackData {
  int	reason;			/* Reason for call back */
  XEvent *event;                /* the event causing the CanvasAction */
  Window window;		/* Window widget is on */
  String *params;               /* the TranslationTable params */
  Cardinal num_params;          /* count of params */
} XpwCanvasCallbackDataRec, *XpwCanvasCallbackData;

#endif /*_Canvas_H_*/


