/*
 * Dialog Widget.
 *
 * While not realy a widget, the dialog widget provides a simple way to display
 * a popup dialog window.
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
 * $Id: Dialog.h,v 1.2 1997/12/06 04:14:49 rich Exp $
 *
 * $Log: Dialog.h,v $
 * Revision 1.2  1997/12/06 04:14:49  rich
 * Added support for IconMask.
 *
 * Revision 1.1  1997/11/28 19:56:42  rich
 * Initial revision
 *
 *
 */

#ifndef _Dialog_h
#define _Dialog_h

#include <X11/Xmu/Converters.h>

/****************************************************************
 *
 * Dialog object
 *
 ****************************************************************/

/* Dialog Widget Resources:
 * 
 * Name              Class              RepType         Default Value
 * ----              -----              -------         -------------
 * activeBorder      Spacing            Dimension	4
 * autoUnmanage      AutoUnmanage       Boolean		True
 * cancelLabelString Label              String		NULL
 * cancelCallback    Callback           Callback        NULL
 * defaultButton     DefaultButton      ButtonType      XpwDialog_Ok_Button
 * dialogType        DialogType         DialogType      XpwDialog_Modal
 * helpLabelString   Label              String		NULL
 * helpCallback      Callback           Callback        NULL
 * icon	             Bitmap		Bitmap		None
 * iconMask          Bitmap		Bitmap		None
 * justify           Justify            Justify         Left
 * message           Label              String          None
 * minimizeButtons   MinimizeButtons    Boolean		True
 * okCallback        Callback           Callback        NULL
 * okLabelString     Label              String		NULL
 * packing           PackingType        PackingType	Fill
 * spacing           Spacing            Dimension	2
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

typedef struct _DialogClassRec *DialogClass;
typedef struct _DialogRec *DialogWidget;

extern WidgetClass  dialogWidgetClass;

typedef enum {
    XpwDialog_Modeless, XpwDialog_Modal, XpwDialog_Dialog
} DialogType;

#define XtNicon		"icon"
#define XtNmessage	"message"
#define XtNokCallback	"okCallback"
#ifndef XtNcancelCallback
#define XtNcancelCallback "cancelCallback"
#endif
#define XtNhelpCallback	"helpCallback"
#define XtNdialogType	"dialogType"
#define XtCDialogType	"DialogType"
#define XtRDialogType	"DialogType"

#define XtNdefaultButton "defaultButton"
#define XtCDefaultButton "DefaultButton"
#define XtRButtonType "ButtonType"
#define XtNminimizeButtons "minimizeButtons"
#define XtCMinimizeButtons "MinimizeButtons"
#define XtNactiveBorder		"activeBorder"
#define XtNokLabelString	"okLabelString"
#define XtNcancelLabelString	"cancelLabelString"
#define XtNhelpLabelString	"helpLabelString"
#define XtNautoUnmanage		"autoUnmanage"
#define XtCAutoUnmanage		"AutoUnmanage"

#define XpwDialog_Ok_Button	1
#define XpwDialog_Cancel_Button	2
#define XpwDialog_Help_Button	3
#define XpwDialog_Label		4


/************************************************************
 *
 *  Public Procedures 
 *
 ************************************************************/

_XFUNCPROTOBEGIN

/*
 * Create a Notice Popup.
 */
Widget XpwDialogCreateNotice(
#if NeedFunctionPrototypes
			Widget /*parent*/,
			String /*name*/,
			Arg */*args*/,
			int /*count*/
#endif
			);

/*
 * Create a Error Popup.
 */
Widget XpwDialogCreateError(
#if NeedFunctionPrototypes
			Widget /*parent*/,
			String /*name*/,
			Arg */*args*/,
			int /*count*/
#endif
			);

/*
 * Create a Info Popup.
 */
Widget XpwDialogCreateInfo(
#if NeedFunctionPrototypes
			Widget /*parent*/,
			String /*name*/,
			Arg */*args*/,
			int /*count*/
#endif
			);

/*
 * Create a Question Popup.
 */
Widget XpwDialogCreateQuestion(
#if NeedFunctionPrototypes
			Widget /*parent*/,
			String /*name*/,
			Arg */*args*/,
			int /*count*/
#endif
			);

/*
 * Create a Work in progress Popup.
 */
Widget XpwDialogCreateWork(
#if NeedFunctionPrototypes
			Widget /*parent*/,
			String /*name*/,
			Arg */*args*/,
			int /*count*/
#endif
			);

/*
 * Create a dialog.
 */
Widget XpwDialogCreateDialog(
#if NeedFunctionPrototypes
			Widget /*parent*/,
			String /*name*/,
			Arg */*args*/,
			int /*count*/
#endif
			);

/*
 * Pop Dialog up.
 */
void XpwDialogShow(
#if NeedFunctionPrototypes
			Widget /*dialog*/
#endif
			);

/*
 * Show dialog and wait for a responce.
 */
int XpwDialogDisplay(
#if NeedFunctionPrototypes
			Widget /*dialog*/
#endif
			);

/*
 * Return selected child.
 */
Widget XpwDialogGetChild(
#if NeedFunctionPrototypes
			Widget /*dialog*/,
			int /*what*/
#endif
			);

_XFUNCPROTOEND

#endif /* _Dialog_h */
