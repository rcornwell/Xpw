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
 * DialogP.h - Private definitions for Dialog object
 *
 * $Id: DialogP.h,v 1.1 1997/11/28 19:56:42 rich Exp rich $
 *
 * $Log: DialogP.h,v $
 * Revision 1.1  1997/11/28 19:56:42  rich
 * Initial revision
 *
 * 
 */

#ifndef _DialogP_h
#define _DialogP_h
#include "Dialog.h"
#include "threeDdrawP.h"

/*********************************************************************
 *
 * Dialog Widget Private Data
 *
 *********************************************************************/

/* New fields for the Dialog widget class record */
typedef struct _DialogClassPart {
    int                foo;	/* Null record entry */
} DialogClassPart;

/* Full class record declaration */
typedef struct _DialogClassRec {
    CoreClassPart       core_class;
    CompositeClassPart  composite_class;
    ConstraintClassPart constraint_class;
    DialogClassPart   dialog_class;
} DialogClassRec;

extern DialogClassRec dialogClassRec;

/* Dialog constraint record */
typedef struct _DialogConstraintsPart {
   /* Private state. */
    Dimension           width;        	/* widget's preferred size */
    Dimension		height;
    Position		x_loc;		/* Were child was placed */
    Position		y_loc;
} DialogConstraintsPart, *DialogConstraints;

typedef struct _DialogConstraintsRec {
    DialogConstraintsPart rowcol;
} DialogConstraintsRec;

/* New fields for the Dialog widget record */
typedef struct {
   /* resources */
    Pixmap		icon;		/* Icon to display in label */
    Pixmap		icon_mask;	/* Mask for icon */
    String		message;	/* Message to display in label */
    int			defbut;		/* Defualt button to highlight */
    DialogType		dialog_type;	/* Type of dialog popup */
    Boolean		minimize;	/* Minimize buttons */
    Boolean		autoUnmanage;	/* Unmanage when a button is pressed */
    Dimension		active_border;	/* How a border to show around active
					 * button */
    Dimension		spacing;	/* How much space to leave between
					 * children */
    XpwPackingType	packing;	/* How we pack buttons */
    XtJustify		justify;	/* How buttons are justified */
    String		ok_str;		/* String for OK button */
    String		cancel_str;	/* String for Cancel Button */
    String		help_str;	/* String for Help Button */
    XtCallbackList 	ok_callbacks;	/* Callback list for buttons */
    XtCallbackList 	cancel_callbacks;
    XtCallbackList 	help_callbacks;
    _XpmThreeDFrame     threeD;

   /* Private Resources */
    Widget		label;		/* Widget holders for defualt objects */
    Widget		line;
    Widget		ok_button;
    Widget		cancel_button;
    Widget		help_button;
    int			used;		/* Last button pressed */
    int			active_child;	/* Child to highlight */
    Dimension		button_width;	/* Size of buttons */
    Dimension		button_height;

} DialogPart;

/* Full instance record declaration */
typedef struct _DialogRec {
    CorePart            core;
    CompositePart       composite;
    ConstraintPart      constraint;
    DialogPart      	dialog;
} DialogRec;


#endif /* _DialogP_h */


