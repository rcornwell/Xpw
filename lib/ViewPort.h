/*
 * ViewPort widget.
 *
 * Provide scrollbar support for child.
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
 * $Id: $
 *
 * $Log: $
 *
 */
#ifndef _ViewPort_H_
#define _ViewPort_H_

#include <X11/Xmu/Converters.h>
#include "threeDdraw.h"

/* ViewPort Resources:
 * 
 * Name              Class              RepType         Default Value
 * ----              -----              -------         -------------
 * foreground        Foreground         Pixel           XtDefaultForeground
 * scrollOnTop       ScrollOnTop        Boolean         False
 * scrollOnTeft      ScrollOnTeft       Boolean         False
 * vScroll           VScroll            Boolean         False
 * hScroll           HScroll            Boolean         False
 * scrollBorder      ScrollBorder       Dimension       4
 * topColor          TopColor           Pixel           XtDefaultBackground
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

typedef struct _ViewPortClassRec *ViewPortWidgetClass;
typedef struct _ViewPortRec *ViewPortWidget;

extern WidgetClass  viewportWidgetClass;

#define XtCVScroll		"vScroll"
#define XtCHScroll		"hScroll"
#define XtCScrollOnLeft		"ScrollOnLeft"
#define XtCScrollOnTop		"ScrollOnTop"
#define XtCTopColor		"topColor"
#define XtCScrollBorder		"scrollBorder"

#define XtNvScroll		"vScroll"
#define XtNhScroll		"hScroll"
#define XtNscrollOnLeft		"ScrollOnLeft"
#define XtNscrollOnTop		"ScrollOnTop"
#define XtNtopColor		"topColor"
#define XtNscrollBorder		"scrollBorder"
 
/************************************************************
 *
 *  Public Procedures 
 *
 ************************************************************/

_XFUNCPROTOBEGIN
void XpwViewportSetLocation(
#if NeedFunctionPrototypes
			Widget /*w*/,
			double /*xoff*/,
			double /*yoff*/
#endif
			);

void XpwViewportSetCoorinates(
#if NeedFunctionPrototypes
			Widget /*w*/,
			Position /*xoff*/,
			Position /*yoff*/
#endif
			);

_XFUNCPROTOEND

#endif
/*_ViewPort_H_*/
