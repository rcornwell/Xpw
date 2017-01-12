/*
 * Stack.c - Stack Composite Widget.
 *
 * Stack manager displays only one child at a time and sets it's window size
 * to hold the largest child.
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
 * $Id: StackP.h,v 1.2 1997/11/01 06:39:09 rich Beta $
 *
 * $Log: StackP.h,v $
 * Revision 1.2  1997/11/01 06:39:09  rich
 * Cleaned up comments.
 *
 * Revision 1.1  1997/10/12 05:17:36  rich
 * Initial revision
 *
 *
 *
 */

#ifndef _StackP_h
#define _StackP_h
#include "Stack.h"

/*********************************************************************
 *
 * Stack Widget Private Data
 *
 *********************************************************************/

/* New fields for the Stack widget class record */

typedef struct _StackClassPart {
    int                 foo;	/* keep compiler happy. */
} StackClassPart;

/* Full Class record declaration */
typedef struct _StackClassRec {
    CoreClassPart       core_class;
    CompositeClassPart  composite_class;
    ConstraintClassPart constraint_class;
    StackClassPart      stack_class;
} StackClassRec;

extern StackClassRec stackClassRec;

/* Stack constraint record */
typedef struct _StackConstraintsPart {
    Dimension		pref_h;		/* Prefered h */
    Dimension		pref_w;		/* Prefered w */

} StackConstraintsPart;

typedef struct _StackConstraintsRec {
    StackConstraintsPart stack;
} StackConstraintsRec, *StackConstraints;

/* New Fields for the Stack widget record */
typedef struct {
   /* private */
    Widget		showing;
    Boolean             recursively_called;	/* for ChangeManaged */
    int                 num_children;	/* count of managed children */
} StackPart;

/* Full instance record declaration */
typedef struct _StackRec {
    CorePart            core;
    CompositePart       composite;
    ConstraintPart      constraint;
    StackPart           stack;
} StackRec;

#endif /* _StackP_h */
