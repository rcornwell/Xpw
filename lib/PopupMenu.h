/*
 * PopupMenu.h - Public Header file for PopupMenu widget.
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
 * $Id: PopupMenu.h,v 1.1 1997/10/04 05:08:42 rich Exp rich $
 *
 * $Log: PopupMenu.h,v $
 * Revision 1.1  1997/10/04 05:08:42  rich
 * Initial revision
 *
 *
 */

#ifndef _PopupMenu_h
#define _PopupMenu_h

#include <X11/Shell.h>
#include <X11/Xmu/Converters.h>
#include "threeDdraw.h"

/****************************************************************
 *
 * PopupMenu widget
 *
 ****************************************************************/

/* PopupMenu Resources:
 * 
 * Name              Class              RepType         Default Value
 * ----              -----              -------         -------------
 * label             Label              String          Name of entry
 * labelClass        LabelClass         Widget          NULL
 * rowHeight         RowHeight          Dimension       0
 * topMargin         TopMargin          Dimension       0
 * bottomMargin      BottomMargin       Dimension       0
 * allowShellResize  AllowShellResize   Boolean         True
 * cursor            Cursor             XtRCursor       None
 * menuOnScreen      MenuOnScreen       Boolean         True
 * popupOnEntry      PopupOnEntry       Widget          NULL
 * backingStore      BackingStore       XtRBackingStore Always+WhenMapped+NotUs0
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

typedef struct _PopupMenuClassRec *PopupMenuWidgetClass;
typedef struct _PopupMenuRec *PopupMenuWidget;

extern WidgetClass  popupMenuWidgetClass;

#define XtNcursor "cursor"
#define XtNbottomMargin "bottomMargin"
#define XtNlabelClass "labelClass"
#define XtNmenuOnScreen "menuOnScreen"
#define XtNpopupOnEntry "popupOnEntry"
#define XtNrowHeight "rowHeight"
#define XtNtopMargin "topMargin"

#define XtCColumnWidth "ColumnWidth"
#define XtCLabelClass "LabelClass"
#define XtCMenuOnScreen "MenuOnScreen"
#define XtCPopupOnEntry "PopupOnEntry"
#define XtCRowHeight "RowHeight"
#define XtCVerticalMargins "VerticalMargins"


#endif /* _PopupMenu_h */
