/*
 * RowCol.c - RowCol Composite Widget.
 *
 * RowCol arranges it's children in rows or columns.
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
 * $Id: RowCol.h,v 1.1 1997/10/07 05:35:41 rich Exp rich $
 *
 * $Log: RowCol.h,v $
 * Revision 1.1  1997/10/07 05:35:41  rich
 * Initial revision
 *
 *
 */

#ifndef _RowCol_h
#define _RowCol_h

#include <X11/Constraint.h>
#include <X11/Xmu/Converters.h>

/****************************************************************
 *
 * RowCol Widget (SubClass of CompositeClass)
 *
 ****************************************************************/

/* RowCol Widget Resources:
 * 
 * Name                  Class             RepType          Default Value
 * ----                  -----             -------          -------------
 * orientation           Orientation       XtOrientation    XtorientVertical
 * refigureMode          Boolean           Boolean          On
 * packing               PackingType       PackingType      fill
 * spacing               Spacing           Dimension        2
 * 
 * From Core:
 *
 * Name                  Class             RepType          Default Value
 * ----                  -----             -------          -------------
 * background            Background        Pixel            XtDefaultBackground
 * border                BorderColor       Pixel            XtDefaultForeground
 * borderWidth           BorderWidth       Dimension        1
 * cursor                Cursor            Cursor           None
 * destroyCallback       Callback          Pointer          NULL
 * height                Height            Dimension        0
 * mappedWhenManaged     MappedWhenManaged Boolean          True
 * sensitive             Sensitive         Boolean          True
 * width                 Width             Dimension        0
 * x                     Position          Position         0
 * y                     Position          Position         0
 * 
 * 
 * Child Constraint Resources:
 * 
 * Name               Class             RepType         Default Value
 * ----               -----             -------         -------------
 * allowResize        Boolean           Boolean         False
 * resizeToPreferred  Boolean           Boolean         True
 * skipAdjust         Boolean           Boolean         False
 * min                Min               Dimension       1
 * max                Max               Dimension       -1
 * 
 */

#define ROWCOL_ASK_CHILD 0


#ifndef XtNpacking
#define XtNpacking "packing"
typedef enum { XpwFill, XpwNoFill, XpwEven } XpwPackingType;
#endif
#ifndef XtCPackingType
#define XtCPackingType "PackingType"
#endif
#ifndef XtRPackingType
#define XtRPackingType "PackingType"
#endif

/* New Fields */
#define XtNspacing	"spacing"
#define XtNallowResize "allowResize"
#define XtNrefigureMode "refigureMode"
#define XtNresizeToPreferred "resizeToPreferred"
#define XtNskipAdjust "skipAdjust"
#define XtNmin "min"
#define XtNmax "max"

#define XtCSpacing "Spacing"
#define XtCMin "Min"
#define XtCMax "Max"

/* Class record constant */
extern WidgetClass  rowColWidgetClass;

typedef struct _RowColClassRec *RowColWidgetClass;
typedef struct _RowColRec *RowColWidget;

/************************************************************
 *
 *  Public Procedures 
 *
 ************************************************************/

_XFUNCPROTOBEGIN

/*
 * Allows a flag to be set the will inhibit the rowCol widgets relayout routine.
 */

extern void         XpwRowColSetRefigureMode(
#if NeedFunctionPrototypes
		       Widget /* w */ ,
#if NeedWidePrototypes
		/* Boolean */ int /* mode */
#else
		       Boolean	/* mode */
#endif
#endif
);

/*
 * Returns the number of children in the rowCol widget.
 */

extern int          XpwRowColGetNumSub(
#if NeedFunctionPrototypes
			 Widget		/* w */
#endif
);

/* 
 * Allows a flag to be set that determines if the rowCol widget will allow
 * geometry requests from this child
 */

extern void         XpwRowColAllowResize(
#if NeedFunctionPrototypes
			   Widget /* w */ ,
#if NeedWidePrototypes
			/* Boolean */ int /* allow_resize */
#else
			   Boolean	/* allow_resize */
#endif
#endif
);

_XFUNCPROTOEND

#endif /* _RowCol_h */
