/*
 * Paned.c - Paned Widget.
 *
 * Paned arranges it's children in rows or columns, and provides a grip to allow
 * the user to resize individual panes.
 *
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
 * $Id: PanedP.h,v 1.1 1997/10/12 05:18:12 rich Exp rich $
 *
 * $Log: PanedP.h,v $
 * Revision 1.1  1997/10/12 05:18:12  rich
 * Initial revision
 *
 *
 */

#ifndef _PanedP_h
#define _PanedP_h
#include "Paned.h"

/*********************************************************************
 *
 * Paned Widget Private Data
 *
 *********************************************************************/

/* New fields for the Paned widget class record */

typedef struct _PanedClassPart {
    int                 foo;	/* keep compiler happy. */
} PanedClassPart;

/* Full Class record declaration */
typedef struct _PanedClassRec {
    CoreClassPart       core_class;
    CompositeClassPart  composite_class;
    ConstraintClassPart constraint_class;
    PanedClassPart      paned_class;
} PanedClassRec;

extern PanedClassRec panedClassRec;

/* Paned children constraint record */
typedef struct _PanedConstraintsPart {
   /* Resources. */
    Boolean             allow_resize;	/* Child resize requests are ok */
    Boolean             skip_adjust;	/* Don't Adjust this Child */
    Boolean		auto_grip;	/* Automatictly determine if we need a
					 * grip */
    Boolean		show_grip;	/* Put a grip on child */
    Boolean             resize_to_pref;	/* resize this child to its preferred
					 * size on a resize or change managed
					 * after realize. */
    Dimension		min;		/* Min size for this Child */
    Dimension		max;		/* Min size for this Child */

   /* Private state. */
    Boolean		user_adjusted;	/* User changed size */
    Position            location;	/* Desired Location */
    Position            new_location;	/* New location, set during grip moves */
    Dimension           wp_size;	/* widget's preferred size */
    int                 size;		/* the size the widget will actually
					 * get. */
    int                 new_size;	/* size of widget set during grip moves */
    Widget		grip;

} PanedConstraintsPart, *PanedConstraints;

typedef struct _PanedConstraintsRec {
    PanedConstraintsPart paned;
} PanedConstraintsRec;

/* New Fields for the Paned widget record */
typedef struct {
   /* resources */
    Boolean             refiguremode;	/* Whether to refigure changes 
					 * right now */
    XtOrientation       orientation;	/* Orientation of paned widget. */
    XpwPackingType      packing;	/* How to pack unfull windows */
    Dimension		spacing;	/* How much space to leave between */

    int			griploc;	/* Percent to place grip */
    Pixel		foreground;	/* Color to show grip moves in */

    Cursor		grip_cursor;
    Cursor		v_grip_cursor;
    Cursor		h_grip_cursor;
    Cursor		adjust_this_cursor;
    Cursor		v_adjust_this_cursor;
    Cursor		h_adjust_this_cursor;

    XtTranslations	grip_translations;

   /* private */
    Boolean             recursively_called;	/* for ChangeManaged */
    int                 num_children;	/* count of managed children */
    int			num_resize;	/* Number resizable panes */
    Dimension		mouse_base;	/* Original location of mouse */
    GC			grip_gc;	/* Move draw gc. */
} PanedPart;

/* Full instance record declaration */
typedef struct _PanedRec {
    CorePart            core;
    CompositePart       composite;
    ConstraintPart      constraint;
    PanedPart           paned;
} PanedRec;

#endif /* _PanedP_h */
