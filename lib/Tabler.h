/*
 * Tabler.c - Tabler Composite Widget.
 *
 * Tabler arranges it's children in rows or columns.
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
 * $Id: Tabler.h,v 1.1 1997/11/28 19:57:31 rich Exp $
 *
 * $Log: Tabler.h,v $
 * Revision 1.1  1997/11/28 19:57:31  rich
 * Initial revision
 *
 *
 */

#ifndef _Tabler_h
#define _Tabler_h

#include <X11/Constraint.h>
#include <X11/Xmu/Converters.h>

/****************************************************************
 *
 * Tabler Widget (SubClass of CompositeClass)
 *
 ****************************************************************/

/* Tabler Widget Resources:
 * 
 * Name                  Class             RepType          Default Value
 * ----                  -----             -------          -------------
 * cols			 Cols              Int              0
 * colSpacing            Spacing           Dimension        2
 * refigureMode          Boolean           Boolean          On
 * rows			 Rows		   Int		    0
 * rowSpacing            Spacing           Dimension        2
 * sideSpacing           Spacing           Dimension        2
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
 * col		      Cols              Int             -1
 * colSpan            Span              Int             0
 * colOmit            Omit              Int             0
 * newRow             NewRow            Boolean         False
 * noBorder           NoBorder          Boolean         False
 * resizeToPreferred  Boolean           Boolean         True
 * row		      Rows              Int             -1
 * rowSpan            Span              Int             0
 * rowOmit            Omit              Int             0
 * position           Position          Int             0
 * 
 */

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
#define XtNallowResize	"allowResize"
#define XtNrefigureMode	"refigureMode"
#define XtNresizeToPreferred "resizeToPreferred"
#define XtNrowSpacing	"rowSpacing"
#define XtNcolSpacing	"colSpacing"
#define XtNsideSpacing	"sideSpacing"
#define XtNskipAdjust	"skipAdjust"
#define XtNmin		"min"
#define XtNmax		"max"
#define XtNrows		"rows"
#define XtCRows		"Rows"
#define XtNcols		"cols"
#define XtCCols		"Cols"
#define XtNrow		"row"
#define XtNcol		"col"
#define XtNrowSpan	"rowSpan"
#define XtNcolSpan	"colSpan"
#define XtCSpan		"Span"
#define XtNrowOmit	"rowOmit"
#define XtNcolOmit	"colOmit"
#define XtCOmit		"Omit"
#define XtNposition	"position"
#define XtNnewRow	"newRow"
#define XtCNewRow	"NewRow"
#define XtNnoBorder	"noBorder"
#define XtCNoBorder	"NoBorder"

#define XtCSpacing	"Spacing"
#define XtCMin		"Min"
#define XtCMax		"Max"

/* Class record constant */
extern WidgetClass  tablerWidgetClass;

typedef struct _TablerClassRec *TablerWidgetClass;
typedef struct _TablerRec *TablerWidget;

/************************************************************
 *
 *  Public Procedures 
 *
 ************************************************************/

_XFUNCPROTOBEGIN

/*
 * Allows a flag to be set the will inhibit the rowCol widgets relayout routine.
 */

extern void         XpwTablerSetRefigureMode(
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

extern int          XpwTablerGetNumSub(
#if NeedFunctionPrototypes
			 Widget		/* w */
#endif
);

/* 
 * Allows a flag to be set that determines if the rowCol widget will allow
 * geometry requests from this child
 */

extern void         XpwTablerAllowResize(
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

#endif /* _Tabler_h */
