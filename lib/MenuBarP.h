/*
 * MenuBar widget.
 *
 * Draws a bar of buttons. Each can have a pulldown menu. As user moves
 * around the popups track the cursor. The bar also includes geometry
 * management and threeD shadow drawing for it's children.
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
 * $Id$
 *
 * $Log:$
 *
 */

#ifndef _MenuBarP_H_
#define _MenuBarP_H_
#include "MenuBar.h"
#include "threeDdrawP.h"

typedef struct {
/* methods */
    int                 foo;	/* Null record entry */
/* class variables */
} MenuBarClassPart;

typedef struct _MenuBarClassRec {
    CoreClassPart       core_class;
    CompositeClassPart  composite_class;
    MenuBarClassPart    menubar_class;
} MenuBarClassRec;

typedef struct {
   /* resources */
    GC                  normal_GC;
    Pixel               foreground;
    Dimension           h_space, v_space;

   /* private state */
    Dimension           preferred_width, preferred_height;
    Dimension           last_query_width, last_query_height;
    XtGeometryMask      last_query_mode;
    Widget              current_menu;	/* Current selected button */
    Widget              current_popup;	/* Current popup */

   /* Shadow info */
    _XpmThreeDFrame	threeD;

} MenuBarPart;

typedef struct _MenuBarRec {
    CorePart            core;
    CompositePart       composite;
    MenuBarPart         menubar;
} MenuBarRec;

extern MenuBarClassRec menubarClassRec;

_XFUNCPROTOEND
#endif /* _MenuBarP_H_ */
