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
 * $Id: ViewPortP.h,v 1.2 1997/11/01 06:39:10 rich Beta $
 *
 * $Log: ViewPortP.h,v $
 * Revision 1.2  1997/11/01 06:39:10  rich
 * Cleaned up comments.
 *
 * Revision 1.1  1997/10/19 01:42:08  rich
 * Initial revision
 *
 *
 */

#ifndef _ViewPortP_H_
#define _ViewPortP_H_
#include "ViewPort.h"
#include "threeDdrawP.h"

/*********************************************************************
 *
 * ViewPort Widget Private Data
 *
 *********************************************************************/

/* New fields for the ViewPort widget class record */

typedef struct {
    int                 foo;	/* Null record entry */
} ViewPortClassPart;

/* Full class record declaration */
typedef struct _ViewPortClassRec {
    CoreClassPart       core_class;
    CompositeClassPart  composite_class;
    ViewPortClassPart   viewport_class;
} ViewPortClassRec;

/* New fields for the ViewPort widget Record */
typedef struct {
   /* resources */
    Boolean		use_v_scroll;   /* Force vertical scrollbar on */
    Boolean		use_h_scroll;   /* Force horizontal scrollbar on */
    Boolean		v_scroll_ontop; /* Horizontal on top/bot */
    Boolean		h_scroll_onleft;/* Vertical on right/left */
    Dimension		scrollborder;   /* Border space between scrollbars */
    Pixel               foreground;
    Pixel               topcolor;
   /* Shadow info */
    _XpmThreeDFrame	threeD;

   /* private resources. */
    Boolean		setup;
    GC			top_gc;		/* Top Color gc */
    Widget		h_scrollbar;	/* Horizontal Scrollbar */
    Widget		v_scrollbar;	/* Vertical Scrollbar */
    Widget		child;		/* Child window */
    Widget		clip;		/* Clip window */
    Position		xoff;		/* Where screen starts */
    Position		yoff;
} ViewPortPart;

/* Full instance record declaration */
typedef struct _ViewPortRec {
    CorePart            core;
    CompositePart       composite;
    ViewPortPart        viewport;
} ViewPortRec;

extern ViewPortClassRec viewportClassRec;

#endif /* _ViewPortP_H_ */
