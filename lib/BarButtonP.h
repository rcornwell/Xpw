/*
 * Bar Button Widget.
 *
 * This is the menu bar button widget. It is a Command widget without any
 * 3D info. But it has a spot to hold a popup menu name. It's purpose is to
 * display the label for the parent.
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
 * BarButtonP.h - Private definitions for BarButton object
 *
 * $Id: BarButtonP.h,v 1.1 1997/10/04 05:00:59 rich Exp rich $
 *
 * $Log: BarButtonP.h,v $
 * Revision 1.1  1997/10/04 05:00:59  rich
 * Initial revision
 *
 * 
 */

#ifndef _BarButtonP_h
#define _BarButtonP_h
#include "labelP.h"
#include "BarButton.h"

/***********************************************************************
 *
 * Bar Button Widget Private Data
 *
 ***********************************************************************/


/* New fields for the BarButton widget class record.  */

typedef struct _BarButtonClassPart {
    void                (*highlight) ();
    void                (*unhighlight) ();
    void                (*notify) ();
    XtPointer           extension;
} BarButtonClassPart;

/* Full class record declaration */
typedef struct _BarButtonClassRec {
    CoreClassPart       core_class;
    BarButtonClassPart  bar_class;
} BarButtonClassRec;

extern BarButtonClassRec barbuttonClassRec;

/* New fields for the BarButton widget record */
typedef struct {
   /* resources */
    XtCallbackList      callbacks;	/* The callback list */
    String              menu_name;	/* Sub Menu... used by parent! */
    Dimension           shadow_width;
    String		clue;		/* Clue string */
    _XpwLabel		label;
} BarButtonPart;

/* Full instance record declaration */
typedef struct _BarButtonRec {
    CorePart            core;
    BarButtonPart       bar_button;
} BarButtonRec;

#endif /* _BarButtonP_h */
