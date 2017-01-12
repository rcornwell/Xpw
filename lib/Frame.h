/*
 * Frame widget.
 *
 * Draw a label with a border around it.
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
 * $Id: Frame.h,v 1.2 1997/11/01 06:39:02 rich Beta $
 *
 * $Log: Frame.h,v $
 * Revision 1.2  1997/11/01 06:39:02  rich
 * Added usePreferedSize Resource.
 *
 * Revision 1.1  1997/10/04 05:04:15  rich
 * Initial revision
 *
 *
 */
#ifndef _Frame_H_
#define _Frame_H_

#include <X11/Xmu/Converters.h>
#include "label.h"
#include "threeDdraw.h"

/* Frame Widget Resources:
 * 
 * Name                Class              RepType         Default Value
 * ----                -----              -------         -------------
 * labelOnTop          LabelOnTop         Boolean	  True
 * outline             Outline      	  Boolean	  True
 * outlineType         FrameType          XtRFrameType    Grooved
 * outlineRaised       OutlineRaised      Boolean         False
 * frameWidth          FrameWidth         Dimension       10
 * usePreferedSize     UsePreferedSize    Boolean	  False
 *
 * From label Resources:
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
 * 
 */

typedef struct _FrameClassRec *FrameWidgetClass;
typedef struct _FrameRec *FrameWidget;

extern WidgetClass  frameWidgetClass;

#define XtNframeWidth "frameWidth"
#define XtCframeWidth "frameWidth"
#define XtNusePreferedSize "usePreferedSize"
#define XtCUsePreferedSize "UsePreferedSize"
#define XtNlabelOnTop	"labelOnTop"
#define XtCLabelOnTop	"labelOnTop"
#define XtNoutline	"outline"
#define XtCOutline	"outline"
#define XtNoutlineType	"outlineType"
#define XtNoutlineRaised	"outlineRaised"
#define XtCOutlineType	"outlineType"
#define XtCOutlineRaised	"outlineRaised"

#endif /*_Frame_H_*/
