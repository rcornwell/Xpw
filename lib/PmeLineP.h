/* 
 * PmeLineP.h - Private definitions for PmeLine widget
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
 * $Id: PmeLineP.h,v 1.1 1997/10/04 05:08:13 rich Exp rich $
 *
 * $Log: PmeLineP.h,v $
 * Revision 1.1  1997/10/04 05:08:13  rich
 * Initial revision
 *
 *
 */

#ifndef _PmeLineP_h
#define _PmeLineP_h

/***********************************************************************
 *
 * PmeLine Widget Private Data
 *
 ***********************************************************************/

#include "PmeLine.h"

/************************************************************
 *
 * New fields for the PmeLine widget class record.
 *
 ************************************************************/

typedef struct _PmeLinePart {
    void                (*notify) ();
    void                (*highlight) ();
    void                (*unhighlight) ();
    char               *(*getmenuname) ();
    XtPointer           extension;
} PmeLineClassPart;

/* Full class record declaration */
typedef struct _PmeLineClassRec {
    RectObjClassPart    rect_class;
    PmeLineClassPart    pme_line_class;
} PmeLineClassRec;

extern PmeLineClassRec pmeLineClassRec;

/* New fields for the PmeLine widget record */
typedef struct {
   /* resources */
    Pixel               foreground;	/* Foreground color. */
    Pixmap              stipple;	/* Line Stipple. */
    Dimension           line_width;	/* Width of the line. */

   /* private data.  */

    GC                  gc;	/* Graphics context for drawing line. */
} PmeLinePart;

/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _PmeLineRec {
    ObjectPart          object;
    RectObjPart         rectangle;
    PmeLinePart         pme_line;
} PmeLineRec;

/************************************************************
 *
 * Private declarations.
 *
 ************************************************************/

#endif /* _PmeLineP_h */
