/*
 * PmeLine.h - Public Header file for PmeLine object.
 *
 * Draws a simple line for a popup menu.
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
 * $Id: PmeLine.h,v 1.1 1997/10/04 05:08:13 rich Beta $
 *
 * $Log: PmeLine.h,v $
 * Revision 1.1  1997/10/04 05:08:13  rich
 * Initial revision
 *
 *
 */

#ifndef _PmeLine_h
#define _PmeLine_h

#include <X11/Xmu/Converters.h>

/****************************************************************
 *
 * PmeLine Object
 *
 ****************************************************************/

/* Pme Line Entry Resources:
 * 
 * Name              Class              RepType         Default Value
 * ----              -----              -------         -------------
 * lineWidth         LineWidth          Dimension       1
 * stipple           Stipple            Pixmap          NULL
 * foreground        Foreground         Pixel           XtDefaultForeground
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

#define XtCLineWidth "LineWidth"
#define XtCStipple "Stipple"

#define XtNlineWidth "lineWidth"
#define XtNstipple "stipple"

typedef struct _PmeLineClassRec *PmeLineObjectClass;
typedef struct _PmeLineRec *PmeLineObject;

extern WidgetClass  pmeLineObjectClass;

#endif /* _PmeLine_h */
