/*
 * TextLine widget.
 *
 * Simple widget for collecting text input.
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

#ifndef _TextLine_H_
#define _TextLine_H_

/* TextLine Widget Resources:
 * 
 * Name              Class              RepType         Default Value
 * ----              -----              -------         -------------
 * clue              Clue               String          NULL
 * centerAmount      HorizontalMargins  Dimension       75
 * gapSize           GapSize            Int             32
 * editable          Editable           Boolean         True
 * doArrows          DoArrows           Boolean         True
 * callback          Callback           Callback        Null
 * international     International      Boolean         False
 * focusGroup        FocusGroup         Widget          None
 * font              Font               XFontStruct *   XtDefaultFont
 * fontSet           FontSet            XFontSet *      XtDefaultFontSet
 * foreground        Foreground         Pixel           XtDefaultForeground
 * aforeground       Foreground         Pixel           XtDefaultForeground
 * cforeground       Foreground         Pixel           XtDefaultForeground
 * leftMargin        HorizontalMargins  Dimension       4
 * rightMargin       HorizontalMargins  Dimension       4
 * vertSpace         VertSpace          int             25
 * string	     String		String		Name of entry
 * traversalOn       TraversalOn        Boolean         False
 * passmode          PassMode           Boolean         False
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

typedef struct _TextLineClassRec *TextLineWidgetClass;
typedef struct _TextLineRec *TextLineWidget;

extern WidgetClass  textLineWidgetClass;
#define XtNgapSize	"gapSize"
#define XtCGapSize	"gapSize"

#define XtNeditable	"editable"
#define XtCEditable	"editable"
#define XtNcenterAmount "centerAmount"
#define XtNcforeground  "cforeground"
#define XtNaforeground  "aforeground"
#define XtNdoArrows	"doArrows"
#define XtCdoArrows	"doArrows"

#define XtNpassMode     "passMode"
#define XtCPassMode	"PassMode"

#define XtNfocusGroup   "focusGroup"
#define XtCFocusGroup	"FocusGroup"

#define XtNtraversalOn	"traversalOn"
#define XtCTraversalOn	"TraversalOn"

typedef struct _XpwTextLineReturnStruct {
        int     reason;         /* Motif compatibility */
        XEvent  *event;
        char    *string;
} XpwTextLineReturnStruct;

/************************************************************
 *
 * Public interface functions
 *
 ************************************************************/

_XFUNCPROTOBEGIN

extern Boolean XpwTextLineGetEditable(
#if NeedFunctionPrototypes
			Widget /*w*/
#endif
);
extern int XpwTextLineGetInsertionPosition(
#if NeedFunctionPrototypes
			Widget /*w*/
#endif
);
extern char *XpwTextLineGetString(
#if NeedFunctionPrototypes
			Widget /*w*/
#endif
);
extern void XpwTextLineInsert(
#if NeedFunctionPrototypes
			Widget /*w*/,
			int /*pos*/,
 			char */*str*/
#endif
);
extern void XpwTextLineReplace(
#if NeedFunctionPrototypes
			Widget /*w*/,
			int /*first*/,
			int /*last*/,
			char */*str*/
#endif
);
extern void XpwTextLineSetEditable(
#if NeedFunctionPrototypes
			Widget /*w*/,
			Boolean /*editable*/
#endif
);
extern void XpwTextLineSetInsertionPosition(
#if NeedFunctionPrototypes
			Widget /*w*/,
			int /*pos*/
#endif
);
extern void XpwTextLineSetSelection(
#if NeedFunctionPrototypes
			Widget /*w*/,
			int /*start*/,
			int /*end*/
#endif
);
extern void XpwTextLineSetString(
#if NeedFunctionPrototypes
			Widget /*w*/,
			char */*str*/
#endif
);

_XFUNCPROTOEND

#endif /*_TextLine_H_*/

