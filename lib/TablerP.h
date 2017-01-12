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
 * $Id: TablerP.h,v 1.1 1997/11/28 19:57:31 rich Exp $
 *
 * $Log: TablerP.h,v $
 * Revision 1.1  1997/11/28 19:57:31  rich
 * Initial revision
 *
 *
 */

#ifndef _TablerP_h
#define _TablerP_h
#include "Tabler.h"

/*********************************************************************
 *
 * Tabler Widget Private Data
 *
 *********************************************************************/

/* New fields for the Tabler widget class record */

typedef struct _TablerClassPart {
    int                 foo;	/* keep compiler happy. */
} TablerClassPart;

/* Full Class record declaration */
typedef struct _TablerClassRec {
    CoreClassPart       core_class;
    CompositeClassPart  composite_class;
    ConstraintClassPart constraint_class;
    TablerClassPart     tabler_class;
} TablerClassRec;

extern TablerClassRec tablerClassRec;

/* Tabler constraint record */
typedef struct _TablerConstraintsPart {
   /* Resources. */
    int			row;		/* Row and column to place widget */
    int			col;
    int			rspan;		/* How many rows to span */
    int			cspan;		/* How many columns to span */
    int			romit;		/* Omit the next number rows */
    int			comit;		/* Omit the next number columns */
    int			position;	/* Position */
    Boolean             resize_to_pref;	/* resize this child to its preferred
					 * size on a resize or change managed
					 * after realize. */
    Boolean		allow_resize;	/* Allow widget to resize */
    Boolean		newrow;		/* Place child on new row */
    Boolean		noborder;	/* Don't use a border */

   /* Private state. */
    Position		x_loc;		/* Where child is. */
    Position		y_loc;
    int			crow;		/* Computed row and col */
    int			ccol;
    Dimension		width;		/* How big to make child */
    Dimension		height;
    Dimension           pref_width;	/* widget's preferred size */
    Dimension           pref_height;	/* widget's preferred size */

} TablerConstraintsPart, *TablerConstraints;

typedef struct _TablerConstraintsRec {
    TablerConstraintsPart tabler;
} TablerConstraintsRec;

/* New Fields for the Tabler widget record */
typedef struct {
   /* resources */
    Boolean             refiguremode;	/* Whether to refigure changes 
					 * right now */
    Dimension		row_spacing;	/* How much space to leave between rows */
    Dimension		col_spacing;	/* How much space to leave between cols */
    Dimension		side_spacing;	/* How much space to leave around edge */
    int			nrows;		/* Number of rows */
    int			ncols;		/* Number of cols */

   /* private */
    Boolean             recursively_called;	/* for ChangeManaged */
    int                 num_children;	/* count of managed children */
    int                 next_child;	/* next child number */
    int			crows;		/* Computed number of rows */
    int			ccols;		/* Computed number of cols */
} TablerPart;

/* Full instance record declaration */
typedef struct _TablerRec {
    CorePart            core;
    CompositePart       composite;
    ConstraintPart      constraint;
    TablerPart          tabler;
} TablerRec;

#endif /* _TablerP_h */
