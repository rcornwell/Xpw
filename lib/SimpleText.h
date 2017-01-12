/*
 * SimpleText widget.
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
 * $Id$
 *
 * $Log:$
 *
 */

#ifndef _SimpleText_H_
#define _SimpleText_H_

/* SimpleText Widget Resources:
 * 
 * Name              Class              RepType         Default Value
 * ----              -----              -------         -------------
 * clue              Clue               String          NULL
 * centerAmount      HorizontalMargins  Dimension       75
 * gapSize           GapSize            Int             32
 * editable          Editable           Boolean         True
 * callback          Callback           Callback        Null
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
 * cforeground       Foreground         Pixel           XtDefaultForeground
 * scrollOnTop       ScrollOnTop        Boolean         False
 * scrollOnTeft      ScrollOnTeft       Boolean         False
 * vScroll           VScroll            Boolean         False
 * hScroll           HScroll            Boolean         False
 * scrollBorder      ScrollBorder       Int             4
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

typedef struct _SimpleTextClassRec *SimpleTextWidgetClass;
typedef struct _SimpleTextRec *SimpleTextWidget;

extern WidgetClass  simpleTextWidgetClass;
#define XtNgapSize "gapSize"
#define XtCGapSize "gapSize"

#define XtCVScroll              "vScroll"
#define XtCHScroll              "hScroll"
#define XtCScrollOnLeft         "ScrollOnLeft"
#define XtCScrollOnTop          "ScrollOnTop"
#define XtCTopColor             "topColor"
#define XtCScrollBorder         "scrollBorder"

#define XtNfont1                "font1"
#define XtNfont2                "font2"
#define XtNfont3                "font3"
#define XtNfontSet1             "fontSet1"
#define XtNfontSet2             "fontSet2"
#define XtNfontSet3             "fontSet3"

#define XtNvScroll              "vScroll"
#define XtNhScroll              "hScroll"
#define XtNscrollOnLeft         "ScrollOnLeft"
#define XtNscrollOnTop          "ScrollOnTop"
#define XtNtopColor             "topColor"
#define XtNscrollBorder         "scrollBorder"

#define XtNeditable "editable"
#define XtCEditable "editable"
#define XtNcforeground  "cforeground"

typedef struct _XpwSimpleTextReturnStruct {
        int     reason;         /* Motif compatibility */
        XEvent  *event;
        char    *string;
} XpwSimpleTextReturnStruct;

/* SimpleText.c */
_XFUNCPROTOBEGIN

extern Boolean XpwSimpleTextGetEditable(
#if NeedFunctionPrototypes
			Widget /*w*/
#endif
);
extern int XpwSimpleTextGetInsertionPosition(
#if NeedFunctionPrototypes
			Widget /*w*/
#endif
);
extern char *XpwSimpleTextGetString(
#if NeedFunctionPrototypes
			Widget /*w*/
#endif
);
extern void XpwSimpleTextInsert(
#if NeedFunctionPrototypes
			Widget /*w*/,
			int /*pos*/,
 			char */*str*/
#endif
);
extern void XpwSimpleTextReplace(
#if NeedFunctionPrototypes
			Widget /*w*/,
			int /*first*/,
			int /*last*/,
			char */*str*/
#endif
);
extern void XpwSimpleTextSetEditable(
#if NeedFunctionPrototypes
			Widget /*w*/,
			Boolean /*editable*/
#endif
);
extern void XpwSimpleTextSetInsertionPosition(
#if NeedFunctionPrototypes
			Widget /*w*/,
			int /*pos*/
#endif
);
extern void XpwSimpleTextSetSelection(
#if NeedFunctionPrototypes
			Widget /*w*/,
			int /*start*/,
			int /*end*/
#endif
);
extern void XpwSimpleTextSetString(
#if NeedFunctionPrototypes
			Widget /*w*/,
			char */*str*/
#endif
);

_XFUNCPROTOEND

#endif /*_SimpleText_H_*/

