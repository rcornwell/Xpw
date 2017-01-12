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
 * $Id: PmeSelectP.h,v 1.2 1997/11/01 06:39:06 rich Beta $
 *
 * $Log: PmeSelectP.h,v $
 * Revision 1.2  1997/11/01 06:39:06  rich
 * Cleaned up comments.
 *
 * Revision 1.1  1997/10/05 02:16:36  rich
 * Initial revision
 *
 *
 * 
 */

#ifndef _PmeSelectP_h
#define _PmeSelectP_h
#include "PmeSelect.h"
#include "labelP.h"
#include "threeDdrawP.h"

/*********************************************************************
 *
 * PmeSelect Object Private Data
 *
 *********************************************************************/

/* New fields for the PmeSelect object class record */

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
    _XpwLabel           label;
    String		menu_name;	/* Unused */
    String		clue;		/* Help Clue */
    XtCallbackList      callbacks;	/* The callback list */

    Boolean             rightbutton;	/* Button is on right or left */
    Boolean             state;		/* Current state */
    Boolean             allownone;	/* If in radio group can we have none */
    Boolean             blankOff;	/* Show no marker if selection is off */
    XtPointer           radio_data;	/* Pointer to radiogroup data 
					 * selected */
    ShapeType		switchShape;
    Dimension		switchSize;
    Dimension		switchShadow;
    Pixel		switchColor;
   /* Shadow info */
    _XpmThreeDFrame     threeD;
    Widget              widget;

   /* private resources. */
    GC                  inverse_gc;	/* reverse color gc. */
    GC                  norm_gc;	/* normal color gc. */
    RadioGroup         *radio_group;	/* Pointer to other members of this group */

} PmeSelectPart;

/* Full instance record declaration */
typedef struct _PmeSelectRec {
    ObjectPart          object;
    RectObjPart         rectangle;
    PmeSelectPart       pme_select;
} PmeSelectRec;

#endif /* _PmeSelectP_h */
