/*
 * Scroller widget.
 *
 * Draw a scroller.
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
 * 
 */

/*
 * $Id: Scroller.h,v 1.2 1997/10/08 04:12:15 rich Exp rich $
 *
 * $Log: Scroller.h,v $
 * Revision 1.2  1997/10/08 04:12:15  rich
 * Make sure XtNposition is not defined multiple times.
 *
 * Revision 1.1  1997/10/04 05:09:08  rich
 * Initial revision
 *
 *
 */
#ifndef _Scroller_H_
#define _Scroller_H_

#include <X11/Xmu/Converters.h>
#include "threeDdraw.h"

/* Scroller Resources:
 * 
 * Name                Class              RepType         Default Value
 * ----                -----              -------         -------------
 * callback            Callback           Pointer         NULL
 * foreground          Foreground         Pixel           XtDefaultForeground
 * useArrows           UseArrows          Boolean         True
 * minimumThumb        MinimumThumb       Int             7
 * shown               Shown              Int     	  0
 * length              Lenght             Int     	  1
 * position            Position           Int     	  0
 * thickness           Thickness          Int     	  14
 * orientation         Orientation        Orientation	  orientVertical
 * delayTime           DelayTime          Int             0
 * repeatTime          RepeatTime         Int             0
 * clue		       Label		  String	  Null
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

#define XtCMinimumThumb "MinimumThumb"
#define XtNminimumThumb "minimumThumb"

#define XtCShown "Shown"

#define XtNuseArrows	"useArrows"
#define XtCUseArrows	"UseArrows"

#ifndef XtNposition
#define XtNposition	"position"
#endif

typedef struct _ScrollerRec      *ScrollerWidget;
typedef struct _ScrollerClassRec *ScrollerWidgetClass;

extern WidgetClass scrollerWidgetClass;

/************************************************************
 *
 *  Public Procedures 
 *
 ************************************************************/

_XFUNCPROTOBEGIN

void
XpwScrollerSetThumb(
#if NeedFunctionPrototypes
		        Widget  /*w*/,
        		int     /*length*/,
        		int     /*shown*/
);
#endif


void
XpwScrollerSetPosition(
#if NeedFunctionPrototypes
        		Widget  /*w*/,
        		int     /*pos*/
);
#endif

_XFUNCPROTOEND

#endif
/*_Scroller_H_*/
