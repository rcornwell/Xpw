/*
 * TextLine widget.
 * 
 * Simple widget for collecting text input.
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
 * $Id: TextLineP.h,v 1.2 1997/11/01 06:39:09 rich Beta rich $
 *
 * $Log: TextLineP.h,v $
 * Revision 1.2  1997/11/01 06:39:09  rich
 * Cleaned up comments.
 *
 * Revision 1.1  1997/10/25 22:15:47  rich
 * Initial revision
 *
 *
 */

#ifndef _TextLineP_H_
#define _TextLineP_H_
#include "TextLine.h"
#include "threeDdrawP.h"

/*********************************************************************
 *
 * TextLine Widget Private Data
 *
 *********************************************************************/

/* New fields for the TextLine widget class record */

typedef struct {
    int                 foo;	/* Null record entry */
} TextLineClassPart;

/* Full class record declaration */
typedef struct _TextLineClassRec {
    CoreClassPart       core_class;
    TextLineClassPart   textline_class;
} TextLineClassRec;

/* New fields for the TextLine widget Record */
typedef struct {
   /* resources */
    Boolean             international;
    Boolean		editable;
    Boolean		doArrows;
    Boolean		passmode;	/* Password mode */
    Boolean		infocus;
    Boolean		traversal;
    
    String		clue;		/* Pop up help clue */

    Pixel               foreground;	/* foreground color. */
    Pixel               cforeground;	/* cursor foreground color. */
    Pixel               aforeground;	/* arrow foreground color. */
    XFontStruct        *font;		/* The font to show text in. */
    XFontSet            fontset;	/* or fontset */

    Dimension		right_margin;
    Dimension		left_margin;
    int			vert_space;
    Dimension		centeramount;
    Widget		focusGroup;
    String		string;		/* Edit buffer */

    XtCallbackList	callbacks;

   /* Shadow info */
    _XpmThreeDFrame	threeD;

   /* private resources. */
    GC                  norm_gc;	/* Normal color gc. */
    GC                  norm_gray_gc;	/* Normal color (grayed out) gc. */
    GC                  rev_gc;		/* Reverse color gc. */
    GC                  rev_gray_gc;	/* Reverse color (grayed out) gc. */
    GC                  cursor_gc;	/* Cursor GC */
    GC                  erase_gc;	/* Cursor GC */
    GC                  arrow_gc;	/* Arrow GC */
    Boolean		left_arrow;	/* Arrow Flags */
    Boolean		right_arrow;
    int			textstart;	/* First character to display */
    int			highstart;	/* Highlight info */
    int			highend;
    Boolean		focused;

    int			stringsize;	/* Size of string */
    int			insert_point;

    int			cursor_x;	/* x offset to cursor */
    int			coffset;	/* Offset to cursor */

    String		select_text;	/* Text selection stuff */
    int			select_size;
    int			select_start;
    int			select_end;

} TextLinePart;

/* Full instance record declaration */
typedef struct _TextLineRec {
    CorePart            core;
    TextLinePart        text;
} TextLineRec;

extern TextLineClassRec textlineClassRec;

#endif /* _TextLineP_H_ */
