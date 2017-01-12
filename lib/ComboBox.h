/*
 * Combo Box widget.
 *
 * The ComboBox widget maintains a pulldown menu of the elements of the
 * combobox associated with it.
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
 * $Id: ComboBox.h,v 1.1 1997/11/28 19:55:48 rich Exp $
 *
 * $Log: ComboBox.h,v $
 * Revision 1.1  1997/11/28 19:55:48  rich
 * Initial revision
 *
 *
 */

#ifndef _ComboBox_h
#define _ComboBox_h

#include <X11/Xmu/Converters.h>

/****************************************************************
 *
 * ComboBox object
 *
 ****************************************************************/

/* ComboBox Widget Resources:
 * 
 * Name              Class              RepType         Default Value
 * ----              -----              -------         -------------
 * itemList	     ItemList		ItemList	None
 * callback          Callback           Callback        NULL
 * clue              Label              String          NULL
 * onRight           OnRight            Boolean         False
 *
 * label Resources:
 * 
 * Name              Class              RepType         Default Value
 * ----              -----              -------         -------------
 * international     International      Boolean         False
 * font              Font               XFontStruct *   XtDefaultFont
 * fontSet           FontSet            XFontSet *      XtDefaultFontSet
 * foreground        Foreground         Pixel           XtDefaultForeground
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

typedef struct _ComboBoxClassRec *ComboBoxClass;
typedef struct _ComboBoxRec *ComboBoxWidget;

extern WidgetClass  comboBoxWidgetClass;

#ifndef XtNmenuJustify
#define XtNmenuJustify "menuJustify"
#endif

#ifndef XtNitemList
#define XtNitemList	"itemList"
#define XtCItemList	"ItemList"
#define XtRItemList	"ItemList"
#endif

#ifndef XtNonRight
#define XtNonRight	"onRight"
#define XtCOnRight	"OnRight"
#endif

typedef struct _XpwComboBoxReturnStruct {
        int             index;
        String          string;
} XpwComboBoxReturnStruct;

/************************************************************
 *
 *  Public Procedures 
 *
 ************************************************************/

_XFUNCPROTOBEGIN

/*
 * Clear the list.
 */
extern void XpwComboBoxNew(
#if NeedFunctionPrototypes
			Widget /*w*/
#endif
);

/*
 * Add one item to the list.
 */
extern void XpwComboBoxAddItem(
#if NeedFunctionPrototypes
			Widget /*w*/,
			String /*item*/,
			int /*after*/
#endif
);

/*
 * Add a list of items to the list.
 */
extern void XpwComboBoxAddItems(
#if NeedFunctionPrototypes
			Widget /*w*/,
			String */*items*/,
			int /*after*/
#endif
			);

/*
 * Delete a range of items from the list.
 */
extern void XpwComboBoxDeleteItems(
#if NeedFunctionPrototypes
			Widget /*w*/,
			int /*from*/,
			int /*to*/
#endif
			);

/*
 * Replace an item in the list.
 */
extern void XpwComboBoxReplaceItem(
#if NeedFunctionPrototypes
			Widget /*w*/,
			String /*item*/,
			int /*index*/
#endif
			);

/*
 * Replace a range of items with a new list.
 */
extern void XpwComboBoxReplaceItems(
#if NeedFunctionPrototypes
			Widget /*w*/,
			String */*items*/,
			int /*index*/
#endif
			);

/*
 * Set item selected.
 */
extern void XpwComboBoxSetItem(
#if NeedFunctionPrototypes
			Widget /*w*/,
			int /*index*/
#endif
			);

/*
 * Return the selected item.
 */
extern void XpwComboBoxGetSelected(
#if NeedFunctionPrototypes
			Widget /*w*/,
			XpwComboBoxReturnStruct */*rs*/
			
#endif
			);
/*
 * Return the menu for item.
 */
extern Widget XpwComboBoxGetMenu(
#if NeedFunctionPrototypes
			Widget /*w*/,
			int /*index*/
#endif
			);

_XFUNCPROTOEND

#endif /* _ComboBox_h */
