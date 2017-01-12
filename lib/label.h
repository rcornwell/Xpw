/*
 * label.h - Global stuff for dealing with labels.
 *
 * Many widgets need to write a generic label on, the label widget helps them
 * by providing functions, without begin a subclass of the label.
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
 * $Id: label.h,v 1.2 1997/12/06 04:14:51 rich Exp $
 *
 * $Log: label.h,v $
 * Revision 1.2  1997/12/06 04:14:51  rich
 * Added support for color images.
 *
 * Revision 1.1  1997/10/12 05:20:24  rich
 * Initial revision
 *
 *
 */

#ifndef _Label_h_
#define _Label_h_

#include <X11/Xmu/Converters.h>

/****************************************************************
 *
 * label library
 *
 ****************************************************************/

/* label Resources:
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
 * bitmapMask        BitmapMask         Pixmap          None
 * leftBitmap        LeftBitmap         Pixmap          None
 * leftBitmapMask    LeftBitmapMask     Pixmap          None
 * leftMargin        HorizontalMargins  Dimension       4
 * rightBitmap       RightBitmap        Pixmap          None
 * rightBitmapMask   RightBitmapMask    Pixmap          None
 * rightMargin       HorizontalMargins  Dimension       4
 * vertSpace         VertSpace          int             25
 * 
 */


#ifndef XtCInternational
#define XtCInternational        "International"
#endif

#ifndef XtNinternational
#define XtNinternational        "international"
#endif

#define XtNleftBitmap "leftBitmap"
#define XtNleftMargin "leftMargin"
#define XtNrightBitmap "rightBitmap"
#define XtNrightMargin "rightMargin"
#define XtNvertSpace   "vertSpace"
#define XtNbitmapMask  "bitmapMask"
#define XtNrightBitmapMask  "rightBitmapMask"
#define XtNleftBitmapMask  "leftBitmapMask"

#ifndef XtNfontSet
#define XtNfontSet		"fontSet"
#endif

#ifndef XtCFontSet
#define XtCFontSet		"FontSet"
#endif

#define XtCLeftBitmap "LeftBitmap"
#define XtCLeftBitmapMask "LeftBitmapMask"
#define XtCHorizontalMargins "HorizontalMargins"
#define XtCRightBitmap "RightBitmap"
#define XtCRightBitmapMask "RightBitmapMask"
#define XtCVertSpace   "VertSpace"
#define XtCBitmapMask  "BitmapMask"

#endif /* _label_h */
