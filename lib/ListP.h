/*
 * List widget.
 *
 * Manage a list of items.
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
 * $Id: ListP.h,v 1.2 1997/11/01 06:39:03 rich Beta $
 *
 * $Log: ListP.h,v $
 * Revision 1.2  1997/11/01 06:39:03  rich
 * Cleaned up comments.
 *
 * Revision 1.1  1997/10/09 02:40:35  rich
 * Initial revision
 *
 *
 */

#ifndef _ListP_H_
#define _ListP_H_
#include "List.h"
#include "threeDdrawP.h"

/*********************************************************************
 *
 * List Widget Private Data
 *
 *********************************************************************/

/* New fields for the List widget class record */

typedef struct {
    int                 foo;	/* Null record entry */
} ListClassPart;

/* Full class record declaration */
typedef struct _ListClassRec {
    CoreClassPart       core_class;
    CompositeClassPart  composite_class;
    ListClassPart       list_class;
} ListClassRec;

/* Struct describing the list types. */
typedef struct {
    unsigned short	selected:1;	/* Entry selected */
    unsigned short	oldstate:1;	/* Entry oldstate */
    unsigned short	outline:1;	/* Entry outlined */
    unsigned short	underline:1;	/* Entry underlined */
    unsigned short	crossout:1;	/* Entry crossed out */
    unsigned short	shape:1;	/* Entry shape in front */
    unsigned short	font:2;		/* Entry font */
    unsigned short	color:3;	/* Entry color */
} _XpwList_attr;
	

/* New fields for the List widget Record */
typedef struct {
    /* resources */
    Boolean             international;
    Boolean		use_v_scroll;	/* Force vertical scrollbar on */
    Boolean		use_h_scroll;	/* Force horizontal scrollbar on */
    Boolean		v_scroll_ontop;	/* Horizontal on top/bot */
    Boolean		h_scroll_onleft;/* Vertical on right/left */
    Boolean		do_redisplay;	/* Redisplay on any change */
    Boolean		only_one;	/* Allow for only one item */
    Boolean		always_notify;	/* Notify on any change or only sets */

    String		*list;		/* List of strings */
    _XpwList_attr	*list_attr;	/* List of attributes */
    int			nitems;		/* Number of items. */
    int			ncols;		/* Number of columns */
    int			nrows;		/* Number of rows */
    int			longest;	/* Longest string length */
    int			tallest;	/* Tallest string */
    int			freedoms;	/* Layout freedom masks */
    int			selected;	/* Item last selected */

    int			xoff;		/* Where screen starts */
    int			yoff;

    Boolean		forcedcols;	/* Force number of columns */
    int			rowspace;	/* Space between rows */
    int			colspace;	/* Space between cols */
    int			scrollborder;	/* Border space between scrollbars */
    Pixel               foreground;
    Pixel               topcolor;
    Pixel		color[8];
    XFontStruct        *font[4];	/* The font to show label in. */
    XFontSet            fontset[4];	/* or fontset */
    String              clue;
    ShapeType		markShape;	/* Marker for items */
    Dimension		markSize;
    Pixel		markColor;
    XtCallbackList      callbacks;	/* Single item callback */
    XtCallbackList      multicallbacks; /* Multi item Callback */

   /* Shadow info */
    _XpmThreeDFrame	threeD;

   /* private resources. */

    Dimension		fh, fw;		/* Font hieght/width */
    GC                  norm_gc;	/* normal color gc. */
    GC                  rev_gc;		/* Reversed fg/bg color gc. */
    GC                  gray_gc;	/* Normal color (grayed out) gc. */
    GC                  rev_gray_gc;	/* Reversed fg/bg color gc. */
    GC			top_gc;		/* Top Color gc */
    GC			mark_gc;	/* Mark gc */

    int			first_item;	/* First item selected */
    int			last_item;	/* Last item selected */

    Widget		h_scrollbar;	/* Horizontal Scrollbar */
    Widget		v_scrollbar;	/* Vertical Scrollbar */

} ListPart;

/* Full instance record declaration */
typedef struct _ListRec {
    CorePart            core;
    CompositePart       composite;
    ListPart            list;
} ListRec;

extern ListClassRec listClassRec;

#endif /* _ListP_H_ */
