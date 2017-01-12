/*
 * PmeSelection widget.
 *
 * This is a selection widget displays a switch on either right or left
 * hand side along with a label. These can be placed into a radio group as
 * well. 
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
 * $Id: PmeSelect.h,v 1.1 1997/10/05 02:16:36 rich Beta $
 *
 * $Log: PmeSelect.h,v $
 * Revision 1.1  1997/10/05 02:16:36  rich
 * Initial revision
 *
 *
 * 
 */

#ifndef _PmeSelect_h
#define _PmeSelect_h
#include "Select.h"
#include "PmeEntry.h"
#include "label.h"
#include "threeDdraw.h"
/****************************************************************
 *
 * PmeSelect Object
 *
 ****************************************************************/

/* PmeSelect Object Resources:
 * 
 * Name              Class              RepType         Default Value
 * ----              -----              -------         -------------
 * callback          Callback           Callback        NULL
 * clue              Clue               String          NULL
 * state             State              Boolean         false
 * switchShape	     ShapeType		ShapeType       XaSquare
 * switchSize	     SwitchSize		Dimension	15
 * switchShadow	     ShadowWidth	Dimension	2
 * switchColor	     Foreground		Pixel		XtDefaultForeground
 * radioGroup        Widget             Widget          NULL
 * radioData         RadioData          Pointer         NULL
 * allowNone         AllowNone          Boolean         True
 * rightButton       RightButton        Boolean         True
 *
 * label Resources:
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

typedef struct _PmeSelectClassRec *PmeSelectObjectClass;
typedef struct _PmeSelectRec *PmeSelectObject;

extern WidgetClass  pmeSelectObjectClass;

#define XtNblankOff		"blankOff"
#define XtCBlankOff		"BlankOff"


_XFUNCPROTOBEGIN

/*      Function Name: XpwPmeSelectChangeRadioGroup
 *      Description: Allows a select widget to change radio lists.
 *      Arguments: w - The select widget to change lists.
 *                 radio_group - any widget in the new list.
 *      Returns: none.
 */

extern void         XpwPmeSelectChangeRadioGroup(
#if NeedFunctionPrototypes
			 Widget /* w */ ,
			 Widget	/* radio_group */
#endif
);

/*      Function Name: XpwPmeSelectGetCurrent
 *      Description: Returns the RadioData associated with the select
 *                   widget that is currently active in a select list.
 *      Arguments: radio_group - any select widget in the select list.
 *      Returns: The XtNradioData associated with the select widget.
 */

extern XtPointer    XpwPmeSelectGetCurrent(
#if NeedFunctionPrototypes
			Widget	/* radio_group */
#endif
);

/*      Function Name: XpwPmeSelectSetCurrent
 *      Description: Sets the PmeSelect widget associated with the
 *                   radio_data specified.
 *      Arguments: radio_group - any select widget in the select list.
 *                 radio_data - radio data of the select widget to set.
 *      Returns: none.
 */

extern void         XpwPmeSelectSetCurrent(
#if NeedFunctionPrototypes
			   Widget /* radio_group */ ,
			   XtPointer /* radio_data */
#endif
);

/*      Function Name: XpwPmeSelectUnsetCurrent
 *      Description: Unsets all PmeSelects in the radio_group specified.
 *      Arguments: radio_group - any select widget in the select list.
 *      Returns: none.
 */

extern void         XpwPmeSelectUnsetCurrent(
#if NeedFunctionPrototypes
			    Widget	/* radio_group */
#endif
);

_XFUNCPROTOEND

#endif /* _PmeSelect_h */
