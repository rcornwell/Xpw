/*
 * MenuBar widget.
 *
 * Draws a bar of buttons. Each can have a pulldown menu. As user moves
 * around the popups track the cursor. The bar also includes geometry
 * management and threeD shadow drawing for it's children.
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
 * $Id: MenuBar.h,v 1.2 1997/10/15 05:42:55 rich Exp rich $
 *
 * $Log: MenuBar.h,v $
 * Revision 1.2  1997/10/15 05:42:55  rich
 * Added justification to pulldown menus.
 *
 * Revision 1.1  1997/10/04 05:06:09  rich
 * Initial revision
 *
 *
 */

#ifndef _MenuBar_H_
#define _MenuBar_H_

#include "threeDdraw.h"

/*
 * MenuBar Resources
 *
 * Name                Class              RepType         Default Value
 * ----                -----              -------         -------------
 * hSpace              HSpace             Dimension       4
 * vSpace              VSpace             Dimension       4
 * menuJustify         Justify            Justify         Right
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

typedef struct _MenuBarClassRec *MenuBarWidgetClass;
typedef struct _MenuBarRec *MenuBarWidget;

extern WidgetClass  menubarWidgetClass;

#ifndef _XtStringDefs_h_
#define XtNhSpace "hSpace"
#define XtNvSpace "vSpace"
#endif

#ifndef XtNmenuJustify
#define XtNmenuJustify "menuJustify"
#endif

/************************************************************
 *
 * Public Functions.
 *
 ************************************************************/

_XFUNCPROTOBEGIN
extern void         _XpwMenuPopupEntry(
#if NeedFunctionPrototypes
			 Widget /* w */ ,
			 Widget	/* menu */
#endif
);

extern void         _XpwMenuPopdownEntry(
#if NeedFunctionPrototypes
			  Widget /* w */
#endif
);

_XFUNCPROTOEND

#endif /*_MenuBar_H_*/
