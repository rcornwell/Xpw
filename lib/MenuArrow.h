/*
 * Menu arrow widget.
 *
 * This is the menu arrow widget. It is a subclass of ArrowWidget.
 * It's purpose is for when you want to put a popup menu with a simple shape.
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
 * $Id: MenuArrow.h,v 1.3 1997/11/28 19:31:07 rich Beta $
 *
 * $Log: MenuArrow.h,v $
 * Revision 1.3  1997/11/28 19:31:07  rich
 * Added alignToParent resource for ComboBox.
 *
 * Revision 1.2  1997/11/01 06:39:04  rich
 * Changed justify to menuJustify to avoid conflict with label.
 *
 * Revision 1.1  1997/10/15 05:42:22  rich
 * Initial revision
 *
 *
 */

#ifndef _MenuArrow_h
#define _MenuArrow_h
#include "Arrow.h"

#include <X11/Xmu/Converters.h>

/****************************************************************
 *
 * MenuArrow object
 *
 ****************************************************************/

/* MenuArrow Widget Resources:
 * 
 * Name              Class              RepType         Default Value
 * ----              -----              -------         -------------
 * menuName          MenuName           String          NULL
 * menuJustify	     Justify		XtJustify	left
 * alignToParent     AlignToParent      Boolean         False
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

typedef struct _MenuArrowClassRec *MenuArrowClass;
typedef struct _MenuArrowRec *MenuArrow;

extern WidgetClass  menuarrowWidgetClass;

#ifndef XtNmenuName
#define XtNmenuName "menuName"
#define XtCMenuName "MenuName"
#endif

#ifndef XtNmenuJustify
#define XtNmenuJustify "menuJustify"
#endif

#ifndef XtNalignToParent
#define XtNalignToParent "alignToParent"
#define XtCAlignToParent "AlignToParent"
#endif

#endif /* _MenuArrow_h */
