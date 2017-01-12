/*
 * SimpleText widget.
 * 
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
 * $Id: $
 *
 * $Log: $
 *
 */

#ifndef _SimpleTextP_H_
#define _SimpleTextP_H_
#include "SimpleText.h"
#include "threeDdrawP.h"

typedef struct {
/* methods */
    int                 foo;	/* Null record entry */
/* class variables */
} SimpleTextClassPart;

typedef struct _SimpleTextClassRec {
    CoreClassPart       core_class;
    CompositeClassPart  composite_class;
    SimpleTextClassPart simpletext_class;
} SimpleTextClassRec;

/* Struct describing the text buffer */
typedef struct {
   Dimension		height;		/* Height of line */
   Dimension		width;		/* Width of line */
   Position		x_off;		/* Offset to x placement */
   Position		y_off;		/* Offset to y placement */
   int			len;		/* Number of chars */
   int			off;		/* Offset from start of buffer */
   String		str;		/* Pointer to string */
   unsigned short	font:2;		/* Font number */
   unsigned short	under:1;	/* Underline */
   unsigned short	rev:1;		/* Rev */
   unsigned short	link:1;		/* Is Hyperlink */
} _XpwST_Line;


typedef struct {
   /* resources */
    Boolean             international;
    Boolean             use_v_scroll;   /* Force vertical scrollbar on */
    Boolean             use_h_scroll;   /* Force horizontal scrollbar on */
    Boolean             v_scroll_ontop; /* Horizontal on top/bot */
    Boolean             h_scroll_onleft;/* Vertical on right/left */
    Boolean             do_redisplay;   /* Redisplay on any change */
    Boolean		editable;
    Dimension		right_margin;
    Dimension		left_margin;
    Dimension		tablen;		/* Size of tabspacing */
    
    String		clue;		/* Pop up help clue */

    int			gapsize;	/* Amount of space to leave around
					   cursor */
    int                 scrollborder;
    Pixel               foreground;	/* foreground color. */
    Pixel               cforeground;	/* cursor foreground color. */
    Pixel               topcolor;
    XFontStruct        *font[4];        /* The font to show text in. */
    XFontSet            fontset[4];     /* or fontset */
    int			attr;		/* Current insert attr number */

    XtCallbackList	callbacks;
    XtCallbackList	linkcallbacks;

   /* Shadow info */
    _XpmThreeDFrame	threeD;

    /* private resources. */
    int                 xoff;           /* Where screen starts */
    int                 yoff;

    GC                  norm_gc;	/* Normal color gc. */
    GC                  norm_gray_gc;	/* Normal color (grayed out) gc. */
    GC                  rev_gc;		/* Reverse color gc. */
    GC                  rev_gray_gc;	/* Reverse color (grayed out) gc. */
    GC                  cursor_gc;	/* Cursor GC */
    GC                  erase_gc;	/* Cursor GC */
    GC                  top_gc;         /* Top Color gc */
    int			textstart;	/* First character to display */
    int			highstart;	/* Highlight info */
    int			highend;
    Boolean		focused;

    Dimension		mheight;	/* Max Size. */
    Dimension		mwidth;

    String		textbuffer;	/* Real edit buffer! */
    int			stringsize;	/* Size of string */
    int			cgap_size;	/* Current gap size */
    int			insert_point;
    int			lindex;		/* Index to current larray */
    _XpwST_Line		*larray;	/* Line info array */
    int			nlarray;	/* Number of entries in array */

    int			cursor_x;	/* x offset to cursor */
    int			coffset;	/* Offset to cursor */

    String		select_text;	/* Text selection stuff */
    int			select_size;
    int			select_start;
    int			select_end;

    Widget              h_scrollbar;    /* Horizontal Scrollbar */
    Widget              v_scrollbar;    /* Vertical Scrollbar */

} SimpleTextPart;

typedef struct _SimpleTextRec {
    CorePart            core;
    CompositePart       composite;
    SimpleTextPart      text;
} SimpleTextRec;

extern SimpleTextClassRec textlineClassRec;

#endif /* _SimpleTextP_H_ */
