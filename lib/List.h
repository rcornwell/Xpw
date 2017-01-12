/*
 * List widget.
 *
 * Manage a list of items.
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
 * $Id: List.h,v 1.2 1998/01/24 20:43:28 rich Exp $
 *
 * $Log: List.h,v $
 * Revision 1.2  1998/01/24 20:43:28  rich
 * XpwGetItem now returns text string, if pointer is non-null.
 *
 * Revision 1.1  1997/10/09 02:40:35  rich
 * Initial revision
 *
 *
 */
#ifndef _List_H_
#define _List_H_

#include <X11/Xmu/Converters.h>
#include "threeDdraw.h"

/* List Resources:
 * 
 * Name              Class              RepType         Default Value
 * ----              -----              -------         -------------
 * international     International      Boolean         False
 * font              Font               XFontStruct *   XtDefaultFont
 * font1             Font               XFontStruct *   XtDefaultFont
 * font2             Font               XFontStruct *   XtDefaultFont
 * font3             Font               XFontStruct *   XtDefaultFont
 * fontSet           FontSet            XFontSet *      XtDefaultFontSet
 * fontSet1          FontSet            XFontSet *      XtDefaultFontSet
 * fontSet2          FontSet            XFontSet *      XtDefaultFontSet
 * fontSet3          FontSet            XFontSet *      XtDefaultFontSet
 * foreground        Foreground         Pixel           XtDefaultForeground
 * color1            ListColor          Pixel           XtDefaultForeground
 * color2            ListColor          Pixel           XtDefaultForeground
 * color3            ListColor          Pixel           XtDefaultForeground
 * color4            ListColor          Pixel           XtDefaultForeground
 * color5            ListColor          Pixel           XtDefaultForeground
 * color6            ListColor          Pixel           XtDefaultForeground
 * color7            ListColor          Pixel           XtDefaultForeground
 * onlyOne           OnlyOne            Boolean         False
 * alwaysNotify      AlwaysNotify       Boolean         False
 * forceColumns      ForceColumns       Boolean         False
 * defaultColumns    Columns            Int             2
 * longest           Longest            Int             0
 * columnSpacing     Spacing            Int             6
 * rowSpacing        Spacing            Int             6
 * scrollOnTop       ScrollOnTop        Boolean         False
 * scrollOnTeft      ScrollOnTeft       Boolean         False
 * vScroll           VScroll            Boolean         False
 * hScroll           HScroll            Boolean         False
 * scrollBorder      ScrollBorder       Int             4
 * topColor          TopColor           Pixel           XtDefaultBackground
 * markColor         MarkColor          Pixel           XtDefaultForeground
 * markShape         ShapeType          ShapeType       none
 * markSize          MarkSize           Dimension       0
 * clue              Label              String          0
 * callback          Callback           Callback        None
 * multiCallback     Callback           Callback        None
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

typedef struct _ListClassRec *ListWidgetClass;
typedef struct _ListRec *ListWidget;

extern WidgetClass  listWidgetClass;

#define XtCSpacing		"Spacing"
#define XtCColumns		"Columns"
#define XtCForceColumns		"forceColumns"
#define XtCOnlyOne		"OnlyOne"
#define XtCAlwaysNotify		"AlwaysNotify"
#define XtCLongest		"Longest"
#define XtCNumberStrings	"NumberStrings"
#define XtCVScroll		"vScroll"
#define XtCHScroll		"hScroll"
#define XtCScrollOnLeft		"ScrollOnLeft"
#define XtCScrollOnTop		"ScrollOnTop"
#define XtCTopColor		"topColor"
#define XtCScrollBorder		"scrollBorder"
#define XtCMarkSize		"MarkSize"
#define XtCMarkColor		"MarkColor"
#define XtCListColor		"ListColor"

#define XtNfont1		"font1"
#define XtNfont2		"font2"
#define XtNfont3		"font3"
#define XtNfontSet1		"fontSet1"
#define XtNfontSet2		"fontSet2"
#define XtNfontSet3		"fontSet3"

#define XtNcolor1		"color1"
#define XtNcolor2		"color2"
#define XtNcolor3		"color3"
#define XtNcolor4		"color4"
#define XtNcolor5		"color5"
#define XtNcolor6		"color6"
#define XtNcolor7		"color7"

#define XtNcursor		"cursor"
#define XtNcolumnSpacing	"columnSpacing"
#define XtNonlyOne		"onlyOne"
#define XtNalwaysNotify		"alwaysNotify"
#define XtNdefaultColumns	"defaultColumns"
#define XtNforceColumns		"forceColumns"
#define XtNlongest		"longest"
#define XtNrowSpacing		"rowSpacing"
#define XtNvScroll		"vScroll"
#define XtNhScroll		"hScroll"
#define XtNscrollOnLeft		"ScrollOnLeft"
#define XtNscrollOnTop		"ScrollOnTop"
#define XtNtopColor		"topColor"
#define XtNscrollBorder		"scrollBorder"
#define XtNmarkShape		"markShape"
#define XtNmarkColor		"markColor"
#define XtNmarkSize		"markSize"
#define XtNmultiCallback	"multiCallback"
 
typedef struct _XpwListReturnStruct {
	int		index;
	String		string;
} XpwListReturnStruct;

/* Attribute mask for list elements. */
#define XpwListNone		0
#define XpwListOutline		1
#define XpwListUnderline	2
#define XpwListCrossout		4
#define XpwListShape		8

/************************************************************
 *
 *  Public Procedures 
 *
 ************************************************************/

_XFUNCPROTOBEGIN

/*
 * Clear the list.
 */
extern void XpwListNew(
#if NeedFunctionPrototypes
			Widget /*w*/
#endif
);

/*
 * Add one item to the list.
 */
extern void XpwListAddItem(
#if NeedFunctionPrototypes
			Widget /*w*/,
			String /*item*/,
			int /*after*/,
			int /*font*/,
			int /*attr*/,
			int /*color*/
#endif
);

/*
 * Add a list of items to the list.
 */
extern void XpwListAddItems(
#if NeedFunctionPrototypes
			Widget /*w*/,
			String */*items*/,
			int /*after*/,
			int /*font*/,
			int /*attr*/,
			int /*color*/
#endif
			);

/*
 * Delete a range of items from the list.
 */
extern void XpwListDeleteItems(
#if NeedFunctionPrototypes
			Widget /*w*/,
			int /*from*/,
			int /*to*/
#endif
			);

/*
 * Replace an item in the list.
 */
extern void XpwListReplaceItem(
#if NeedFunctionPrototypes
			Widget /*w*/,
			String /*item*/,
			int /*index*/,
			int /*font*/,
			int /*attr*/,
			int /*color*/
#endif
			);

/*
 * Replace a range of items with a new list.
 */
extern void XpwListReplaceItems(
#if NeedFunctionPrototypes
			Widget /*w*/,
			String */*items*/,
			int /*index*/,
			int /*font*/,
			int /*attr*/,
			int /*color*/
#endif
			);

/*
 * Don't update the display, used when making massive changes to list.
 */
extern void XpwListEnableDisplay(
#if NeedFunctionPrototypes
			Widget /*w*/
#endif
			);

/*
 * Update the display from now on.
 */
extern void XpwListDisableDisplay(
#if NeedFunctionPrototypes
			Widget /*w*/
#endif
			);

/*
 * Set item selected. If adjust center item in window.
 */
extern void XpwListSetItem(
#if NeedFunctionPrototypes
			Widget /*w*/,
			int /*index*/,
			Boolean /*adjust*/
#endif
			);

/*
 * Return the state of item.
 */
extern Boolean XpwListGetItem(
#if NeedFunctionPrototypes
			Widget /*w*/,
			int /*index*/,
			XpwListReturnStruct */*rs*/
#endif
			);
	
/*
 * Compliment item. If adjust center item in window.
 */
extern void XpwListToggleItem(
#if NeedFunctionPrototypes
			Widget /*w*/,
			int	/* index */,
			Boolean /*adjust*/
#endif
			);

/*
 * Clear selection item. If adjust center item in window.
 */
extern void XpwListClearItem(
#if NeedFunctionPrototypes
			Widget /*w*/,
			int /*index*/,
			Boolean /*adjust*/
#endif
			);

/*
 * Clear all items.
 */
extern void XpwListClearAll(
#if NeedFunctionPrototypes
			Widget /*w*/
#endif
			);

/*
 * Return a list of all items currently marked as selected.
 */
extern int XpwListGetAllSelected(
#if NeedFunctionPrototypes
			Widget /*w*/,
			XpwListReturnStruct **/*rs*/
#endif
			);

/*
 * Return the selected item.
 */
extern void XpwListGetSelected(
#if NeedFunctionPrototypes
			Widget /*w*/,
			XpwListReturnStruct */*rs*/
#endif
			);

_XFUNCPROTOEND

#endif
/*_List_H_*/
