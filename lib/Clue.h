/*
 * Clue widget.
 *
 * Pop up a clue for a widget. This widget is realy a place holder. Widgets
 * which have popup help available should call _XpwArmClue() when they detect
 * a enter window event. When there is an event in the window widgets should
 * call _XpwDisArmClue() so that the clue will not be popped up. Widgets can
 * also call _XpwHoldClue() if they need to delay a clue for some time.
 * 
 * When the clue is finaly poped up it is a box with a label. The text of the
 * label is overwritten with the contect of the widget supplied text.
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
 * $Id: Clue.h,v 1.1 1997/10/04 05:02:12 rich Beta rich $
 *
 * $Log: Clue.h,v $
 * Revision 1.1  1997/10/04 05:02:12  rich
 * Initial revision
 *
 *
 */
#ifndef _Clue_H_
#define _Clue_H_

#include <X11/Xmu/Converters.h>
#include "label.h"

/* Clue has the following Resources:
 * 
 * Name                Class              RepType         Default Value
 * ----                -----              -------         -------------
 * timeOut             TimeOut            Int             500
 * showClue            ShowClue           Boolean         True
 *
 * label Resources:
 * 
 * Name              Class              RepType         Default Value
 * ----              -----              -------         -------------
 * international     International      Boolean         False
 * font              Font               XFontStruct *   XtDefaultFont
 * fontSet           FontSet            XFontSet *      XtDefaultFontSet
 * foreground        Foreground         Pixel           XtDefaultForeground
 * label             Label              String          Name of entry
 * justify           Justify            XtJustify       Left
 * bitmap            Bitmap             Pixmap          None
 * leftBitmap        LeftBitmap         Pixmap          None
 * leftMargin        HorizontalMargins  Dimension       4
 * rightBitmap       RightBitmap        Pixmap          None
 * rightMargin       HorizontalMargins  Dimension       4
 * vertSpace         VertSpace          int             25
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

typedef struct _ClueClassRec *ClueWidgetClass;
typedef struct _ClueRec *ClueWidget;

extern WidgetClass  clueWidgetClass;

#define XtNtimeOut        "timeOut"
#define XtCTimeOut        "TimeOut"
#define XtNshowClue       "showClue"
#define XtCShowClue       "ShowClue"
#define XtNclue		  "clue"

_XFUNCPROTOBEGIN
/*
 * Arm a clue to popup for widget w.
 */
extern void _XpwArmClue(
#if NeedFunctionPrototypes
Widget w,
String clue
#endif
);

/*
 * Cancel a clue popup, for widget w.
 */
extern void _XpwDisArmClue(
#if NeedFunctionPrototypes
Widget w
#endif
);

/*
 * Hold off a clue timeout.
 */
extern void _XpwHoldClue(
#if NeedFunctionPrototypes
Widget w
#endif
);

_XFUNCPROTOEND
#endif /*_Clue_H_*/
