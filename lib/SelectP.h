/*
 * Selection widget.
 *
 * This is a selection widget displays a switch on either right or left
 * hand side along with a label. These can be placed into a radio group as
 * well. 
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
 * SelectP.h - Private definitions for Selection object
 *
 * $Id: SelectP.h,v 1.1 1997/10/04 22:13:23 rich Exp rich $
 *
 * $Log: SelectP.h,v $
 * Revision 1.1  1997/10/04 22:13:23  rich
 * Initial revision
 *
 * 
 */

#ifndef _SelectP_h
#define _SelectP_h
#include "Select.h"
#include "labelP.h"
#include "threeDdrawP.h"

typedef struct _SelectClassPart {
    void                (*Set) ();
    void                (*Unset) ();
    void                (*Toggle) ();
    void                (*Notify) ();
    XtPointer           extension;
} SelectClassPart;

/* Full class record declaration */
typedef struct _SelectClassRec {
    CoreClassPart       core_class;
    SelectClassPart     select_class;
} SelectClassRec;

extern SelectClassRec selectClassRec;

typedef struct _RadioGroup {
    struct _RadioGroup *next, *prev;
    Widget              widget;
} RadioGroup;

/* New fields for the Select Object record */
typedef struct {
   /* resources */
    XtCallbackList      callbacks;	/* The callback list */
    _XpwLabel           label;
    ShapeType		switchShape;
    Dimension		switchSize;
    Dimension		switchShadow;
    Pixel		switchColor;
    Boolean             rightbutton;	/* Button is on right or left */
    Boolean             state;		/* Current state */
    Boolean             allownone;	/* If in radio group can we have none */
    XtPointer           radio_data;	/* Pointer to radiogroup data 
					 * selected */
    Widget              widget;
    String		clue;		/* Help Clue */
   /* Shadow info */
    _XpmThreeDFrame     threeS;
    _XpmThreeDFrame     threeD;

   /* private resources. */
    GC                  inverse_gc;	/* reverse color gc. */
    GC                  norm_gc;	/* normal color gc. */
    RadioGroup         *radio_group;	/* Pointer to other members of this group */

} SelectPart;

/* Full instance record declaration */
typedef struct _SelectRec {
    CorePart            core;
    SelectPart          select;
} SelectRec;

#endif /* _SelectP_h */
