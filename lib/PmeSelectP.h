/*
 * PmeSelection widget.
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
 * $Id: $
 *
 * $Log: $
 *
 * 
 */

#ifndef _PmeSelectP_h
#define _PmeSelectP_h

/***********************************************************************
 *
 * Private Data
 *
 ***********************************************************************/

#include "PmeSelect.h"
#include "labelP.h"
#include "threeDdrawP.h"

/************************************************************
 *
 * New fields for the PmeSelect Object class record.
 *
 ************************************************************/
typedef struct _PmeSelectClassPart {
    void                (*Notify) ();
    void                (*highlight) ();
    void                (*unhighlight) ();
    char               *(*getmenuname) ();
    XtPointer           extension;
} PmeSelectClassPart;

/* Full class record declaration */
typedef struct _PmeSelectClassRec {
    RectObjClassPart    rect_class;
    PmeSelectClassPart  pme_select_class;
} PmeSelectClassRec;

extern PmeSelectClassRec pmeSelectClassRec;

typedef struct _RadioGroup {
    struct _RadioGroup *next, *prev;
    Widget              widget;
} RadioGroup;

/* New fields for the PmeSelect Object record */
typedef struct {
   /* resources */
	/* First entries must be in same order as PmeEntry */
    XtCallbackList      callbacks;	/* The callback list */
    _XpwLabel           label;

    String		clue;		/* Help Clue */
    ShapeType		switchShape;
    Dimension		switchSize;
    Dimension		switchShadow;
    Pixel		switchColor;
    Boolean             rightbutton;	/* Button is on right or left */
    Boolean             state;		/* Current state */
    Boolean             allownone;	/* If in radio group can we have none */
    Boolean             blankOff;	/* Show no marker if selection is off */
    XtPointer           radio_data;	/* Pointer to radiogroup data 
					 * selected */
    Widget              widget;

   /* Parts associated with the label field. */

   /* private resources. */
    GC                  inverse_gc;	/* reverse color gc. */
    GC                  norm_gc;	/* normal color gc. */

    RadioGroup         *radio_group;	/* Pointer to other members of this group */

   /* Shadow info */
    _XpmThreeDFrame     threeD;

} PmeSelectPart;

/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _PmeSelectRec {
    ObjectPart          object;
    RectObjPart         rectangle;
    PmeSelectPart       pme_select;
} PmeSelectRec;

/************************************************************
 *
 * Private declarations.
 *
 ************************************************************/

#endif /* _PmeSelectP_h */
