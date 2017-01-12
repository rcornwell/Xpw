/*
 * Chart widget.
 *
 *  Draws a strip chart. 
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
 * $Id: Chart.h,v 1.1 1997/10/15 04:57:46 rich Beta $
 *
 * $Log: Chart.h,v $
 * Revision 1.1  1997/10/15 04:57:46  rich
 * Initial revision
 *
 *
 */
#ifndef _Chart_H_
#define _Chart_H_

#include <X11/Xmu/Converters.h>
#include "label.h"
#include "threeDdraw.h"

/* Chart Widget Resources:
 * 
 * Name              Class              RepType         Default Value
 * ----              -----              -------         -------------
 * interval	     Interval	  	Int		10
 * minScale	     Scale		Int		1
 * datacolor	     Foreground	  	Pixel		XtDefaultForeground
 * scalecolor	     Foreground	  	Pixel		XtDefaultForeground
 * getValue	     Callback	  	Callback	Null
 * scrollAmount	     ScrollAmount	Int		10
 * clue              Clue               String          NULL
 *
 * From label:
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

typedef struct _ChartClassRec *ChartWidgetClass;
typedef struct _ChartRec *ChartWidget;

extern WidgetClass  chartWidgetClass;

#define XtNinterval	"interval"
#define XtNminScale	"minScale"
#define XtCScale	"Scale"
#define XtNdatacolor	"datacolor"
#define XtNscalecolor	"scalecolor"
#define XtNgetValue	"getValue"
#define XtNscrollAmount	"scrollAmount"
#define XtCScrollAmount	"ScrollAmount"

_XFUNCPROTOBEGIN

/*
 * Add a data point to chart.
 */
void XpwChartAddData(
#if NeedFunctionPrototypes
			Widget	/*w*/,
			double	/*value*/
#endif
);

_XFUNCPROTOEND

#endif /*_Chart_H_*/
