/*
 * Combo Box widget.
 *
 * The ComboBox widget maintains a pulldown menu of the elements of the
 * combobox associated with it.
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
 * ComboBoxP.h - Private definitions for ComboBox object
 *
 * $Id: $
 *
 * $Log: $
 * 
 */

#ifndef _ComboBoxP_h
#define _ComboBoxP_h
#include "CommandP.h"
#include "ComboBox.h"

/*********************************************************************
 *
 * ComboBox Widget Private Data
 *
 *********************************************************************/

/* New fields for the ComboBox widget class record */
typedef struct _ComboBoxClassPart {
    int                foo;	/* Null record entry */
} ComboBoxClassPart;

/* Full class record declaration */
typedef struct _ComboBoxClassRec {
    CoreClassPart       core_class;
    CompositeClassPart  composite_class;
    ComboBoxClassPart   combobox_class;
} ComboBoxClassRec;

extern ComboBoxClassRec comboBoxClassRec;

/* New fields for the ComboBox widget record */
typedef struct {
   /* resources */
    XtJustify           menu_justify;   /* Justification for the popup. */
    _XpwLabel		label;
    String		clue;
    Boolean		onright;
    String		*list;		/* List of Values. */
    XtCallbackList callbacks;

   /* Private Resources */
    Widget		*wlist;		/* List of children */
    int			nitems;		/* Size of list */

    int			select;		/* Selected item */

    Widget		arrow;
    Widget		menu;

   /* Shadow info */
    _XpmThreeDFrame	threeD;
} ComboBoxPart;

/* Full instance record declaration */
typedef struct _ComboBoxRec {
    CorePart            core;
    CompositePart       composite_class;
    ComboBoxPart      	combobox;
} ComboBoxRec;


#endif /* _ComboBoxP_h */

