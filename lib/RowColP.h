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
 * $Id: RowColP.h,v 1.1 1997/10/07 05:35:41 rich Exp rich $
 *
 * $Log: RowColP.h,v $
 * Revision 1.1  1997/10/07 05:35:41  rich
 * Initial revision
 *
 *
 */

#ifndef _RowColP_h
#define _RowColP_h

#include "RowCol.h"

/*********************************************************************
 *
 * RowCol Widget Private Data
 *
 *********************************************************************/

/* New fields for the RowCol widget class record */

typedef struct _RowColClassPart {
    int                 foo;	/* keep compiler happy. */
} RowColClassPart;

/* Full Class record declaration */
typedef struct _RowColClassRec {
    CoreClassPart       core_class;
    CompositeClassPart  composite_class;
    ConstraintClassPart constraint_class;
    RowColClassPart     rowcol_class;
} RowColClassRec;

extern RowColClassRec rowcolClassRec;

/* RowCol constraint record */
typedef struct _RowColConstraintsPart {
   /* Resources. */
    Boolean             allow_resize;	/* Child resize requests are ok */
    Boolean             skip_adjust;	/* Don't Adjust this Child */
    Dimension		min;		/* Min size for this Child */
    Dimension		max;		/* Min size for this Child */

   /* changed without explicit user action. */
    Boolean             resize_to_pref;	/* resize this child to its preferred
					 * size on a resize or change managed
					 * after realize. */
   /* Private state. */
    Position            location;	/* Desired Location */
    Dimension           wp_size;	/* widget's preferred size */
    int                 size;		/* the size the widget will actually
					 * get. */

} RowColConstraintsPart, *RowColConstraints;

typedef struct _RowColConstraintsRec {
    RowColConstraintsPart rowcol;
} RowColConstraintsRec;

/* New Fields for the RowCol widget record */
typedef struct {
   /* resources */
    Boolean             refiguremode;	/* Whether to refigure changes 
					 * right now */
    XtOrientation       orientation;	/* Orientation of rowcol widget. */
    XpwPackingType      packing;	/* How to pack unfull windows */
    Dimension		spacing;	/* How much space to leave between */

   /* private */
    Boolean             recursively_called;	/* for ChangeManaged */
    int                 num_children;	/* count of managed children */
} RowColPart;

/**************************************************************************
 *
 * Full instance record declaration
 *
 **************************************************************************/

typedef struct _RowColRec {
    CorePart            core;
    CompositePart       composite;
    ConstraintPart      constraint;
    RowColPart          rowcol;
} RowColRec;

#endif /* _RowColP_h */
