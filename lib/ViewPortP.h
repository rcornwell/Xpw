/*
 * ViewPort widget.
 *
 * Provide scrollbar support for child.
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
 * $Id: $
 *
 * $Log: $
 *
 */

#ifndef _ViewPortP_H_
#define _ViewPortP_H_
#include "ViewPort.h"
#include "threeDdrawP.h"

typedef struct {
/* methods */
    int                 foo;	/* Null record entry */
/* class variables */
} ViewPortClassPart;

typedef struct _ViewPortClassRec {
    CoreClassPart       core_class;
    CompositeClassPart  composite_class;
    ViewPortClassPart   viewport_class;
} ViewPortClassRec;

/* Main viewport object resources */
typedef struct {
    /* resources */
    Boolean		use_v_scroll;
    Boolean		use_h_scroll;
    Boolean		v_scroll_ontop;
    Boolean		h_scroll_onleft;
    Dimension		scrollborder;
    Pixel               foreground;
    Pixel               topcolor;

/* private resources. */
    Boolean		setup;
    GC			top_gc;		/* Top Color gc */

    Widget		h_scrollbar;	/* Horizontal Scrollbar */
    Widget		v_scrollbar;	/* Vertical Scrollbar */
    Widget		child;		/* Child window */
    Widget		clip;		/* Clip window */

    Position		xoff;		/* Where screen starts */
    Position		yoff;

   /* Shadow info */
    _XpmThreeDFrame	threeD;

} ViewPortPart;

typedef struct _ViewPortRec {
    CorePart            core;
    CompositePart       composite;
    ViewPortPart        viewport;
} ViewPortRec;

extern ViewPortClassRec viewportClassRec;

#endif /* _ViewPortP_H_ */
