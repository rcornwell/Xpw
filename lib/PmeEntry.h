/*
 * PmeEntry.h - Public Header file for PmeEntry object.
 *
 * Pme is a popup menu entry. Basicly a label widget. Setting resource
 * menuname will cause PopupMenu to popup the cascaded menu of that name
 * if it can find one.
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
 * $Id$
 *
 * $Log:$
 *
 */

#ifndef _PmeEntry_h
#define _PmeEntry_h

#include "label.h"
#include <X11/Xmu/Converters.h>

/****************************************************************
 *
 * PmeEntry object
 *
 ****************************************************************/

/* Pme Menu Entry Resources:
 * 
 * Name              Class              RepType         Default Value
 * ----              -----              -------         -------------
 * callback          Callback           Callback        NULL
 * menuName          MenuName           String          NULL
 * clue              Clue               String          NULL
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
 * 
 */

typedef struct _PmeEntryClassRec *PmeEntryObjectClass;
typedef struct _PmeEntryRec *PmeEntryObject;

extern WidgetClass  pmeEntryObjectClass;

#define XtNmenuName	"menuName"
#define XtCMenuName	"MenuName"

#endif /* _PmeEntry_h */
