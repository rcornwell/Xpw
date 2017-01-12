/*
 * Percent widget.
 *
 * Draw a work completed bar graph.
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
 * $Id: Percent.h,v 1.1 1997/10/08 04:03:06 rich Beta $
 *
 * $Log: Percent.h,v $
 * Revision 1.1  1997/10/08 04:03:06  rich
 * Initial revision
 *
 *
 *
 */
#ifndef _Percent_H_
#define _Percent_H_

#include <X11/Xmu/Converters.h>
#include "threeDdraw.h"

/* Percent Widget Resources:
 * 
 * Name              Class              RepType         Default Value
 * ----              -----              -------         -------------
 * clue              Clue               String          NULL
 * showValue	     Boolean		Boolean		True
 * foreground        Foreground         Pixel           XtDefaultForeground
 * thumb             Foreground         Pixel           XtDefaultBackground
 * thickness         Thickness          Int             14
 * min               Min                Int             0
 * max               Max                Int             100
 * position          Position           Int             0
 * orientation       Orientation        Orientation     orientVertical
 * international     International      Boolean         False
 * font              Font               XFontStruct *   XtDefaultFont
 * fontSet           FontSet            XFontSet *      XtDefaultFontSet
 * justify           Justify            XtJustify       Left
 * vertSpace         VertSpace          int             25
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

#ifndef XtNmin
#define XtNmin "min"
#define XtCMin "Min"
#endif

#ifndef XtNmax
#define XtNmax "max"
#define XtCMax "Max"
#endif

#ifndef XtNposition
#define XtNposition     "position"
#endif

#ifndef XtNshowValue
#define XtNshowValue	"showValue"
#define XtCShowValue	"ShowValue"
#endif

typedef struct _PercentClassRec *PercentWidgetClass;
typedef struct _PercentRec *PercentWidget;

extern WidgetClass  percentWidgetClass;

/************************************************************
 *
 *  Public Procedures 
 *
 ************************************************************/

_XFUNCPROTOBEGIN

void
XpwPercentSetMinMax(
#if NeedFunctionPrototypes
                        Widget  /*w*/,
                        int     /*min*/,
                        int     /*max*/
);
#endif


void
XpwPercentSetPosition(
#if NeedFunctionPrototypes
                        Widget  /*w*/,
                        int     /*pos*/
);
#endif

void
XpwPercentSetState(
#if NeedFunctionPrototypes
        		Widget  /*w*/,
		        Boolean /*state*/
#endif
);


_XFUNCPROTOEND

#endif /*_Percent_H_*/
