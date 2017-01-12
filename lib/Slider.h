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
 * 
 */

/*
 * $Id: Slider.h,v 1.3 1997/12/06 04:14:50 rich Beta $
 *
 * $Log: Slider.h,v $
 * Revision 1.3  1997/12/06 04:14:50  rich
 * Thickness is of type Dimension.
 *
 * Revision 1.2  1997/11/01 06:39:08  rich
 * Removed a comment error.
 *
 * Revision 1.1  1997/10/08 04:08:42  rich
 * Initial revision
 *
 *
 *
 */
#ifndef _Slider_H_
#define _Slider_H_

#include <X11/Xmu/Converters.h>
#include "threeDdraw.h"

/* Slider Resources:
 * 
 * Name                Class              RepType         Default Value
 * ----                -----              -------         -------------
 * callback            Callback           Pointer         NULL
 * foreground          Foreground         Pixel           XtDefaultForeground
 * min                 Min                Int     	  0
 * max                 Max                Int     	  100
 * position            Position           Int     	  0
 * thickness           Thickness          Dimension    	  14
 * orientation         Orientation        Orientation	  orientVertical
 * clue                Label              String          NULL
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


#ifndef XtNmin
#define XtNmin "min"
#define XtCMin "Min"
#endif

#ifndef XtNmax
#define XtNmax "max"
#define XtCMax "Max"
#endif

#ifndef XtNposition
#define XtNposition	"position"
#endif

typedef struct _SliderRec      *SliderWidget;
typedef struct _SliderClassRec *SliderWidgetClass;

extern WidgetClass sliderWidgetClass;

/************************************************************
 *
 *  Public Procedures 
 *
 ************************************************************/

_XFUNCPROTOBEGIN

void
XpwSliderSetMinMax(
#if NeedFunctionPrototypes
		        Widget  /*w*/,
        		int     /*min*/,
        		int     /*max*/
);
#endif


void
XpwSliderSetPosition(
#if NeedFunctionPrototypes
        		Widget  /*w*/,
        		int     /*pos*/
);
#endif

_XFUNCPROTOEND

#endif
/*_Slider_H_*/
