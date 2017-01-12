/*
 * Arrow widget.
 *
 * An Arrow widget, draws an arrow in a window and accepts mouse input.
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
 * $Id: Arrow.h,v 1.1 1997/10/04 18:59:34 rich Beta $
 *
 * $Log: Arrow.h,v $
 * Revision 1.1  1997/10/04 18:59:34  rich
 * Initial revision
 *
 *
 */
#ifndef _Arrow_H_
#define _Arrow_H_

#include <X11/Xmu/Converters.h>
#include "threeDdraw.h"

/* Arrow Resources:
 * 
 * Name                Class              RepType         Default Value
 * ----                -----              -------         -------------
 * callBacks           Callback           Pointer         NULL
 * foreground          Foreground         Pixel           XtDefaultForeground
 * arrowType           ShapeType          XtRShapeType    XaUp
 * delayTime           DelayTime          Int             0
 * repeatTime          RepeatTime         Int             0
 *
 * From ThreeD Highlight:
 * 
 * Name                Class              RepType         Default Value
 * ----                -----              -------         -------------
 * raised              Raised             Boolean         True
 * frameType           FrameType          FrameType       XfBeveled
 * topShadowPixel      TopShadowPixel     Pixel           XtDefaultForeground 
 * botShadowPixel      BotShadowPixel     Pixel           XtDefaultForeground 
 * topShadowPixmap     TopShadowPixmap    Pixmap          Null 
 * botShadowPixmap     BotShadowPixmap    Pixmap          Null 
 * topShadowContrast   TopShadowContrast  Int             20 
 * botShadowContrast   BotShadowContrast  Int             40 
 * shadowWidth         ShadowWidth        Dimension       2
 * beNiceToColorMap    BeNiceToColorMap   Boolean         True
 * 
 * From Core:
 * 
 * Name                Class              RepType         Default Value
 * ----                -----              -------         -------------
 * borderWidth         BorderWidth        Dimension       2
 * sensitive           Sensitive          Boolean         True
 * height              Height             Dimension       0
 * width               Width              Dimension       0
 * x                   Position           Position        0
 * y                   Position           Position        0
 * 
 */


typedef struct _ArrowClassRec *ArrowWidgetClass;
typedef struct _ArrowRec *ArrowWidget;

extern WidgetClass  arrowWidgetClass;

#ifndef XtNarrowType
#define XtNarrowType "arrowType"
#endif

#define XtNdelayTime "delayTime"
#define XtCDelayTime "DelayTime"

#define XtNrepeatTime "repeatTime"
#define XtCRepeatTime "RepeatTime"

#endif /*_Arrow_H_*/
