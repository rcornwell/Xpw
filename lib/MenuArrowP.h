/*
 * Menu arrow widget.
 *
 * This is the menu arrow widget. It is a subclass of ArrowWidget.
 * It's purpose is for when you want to put a popup menu with a simple shape.
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
 * MenuArrowP.h - Private definitions for MenuArrow.
 *
 * $Id: MenuArrowP.h,v 1.2 1997/11/01 06:39:04 rich Beta rich $
 *
 * $Log: MenuArrowP.h,v $
 * Revision 1.2  1997/11/01 06:39:04  rich
 * Changed justify to menuJustify to avoid conflict with label.
 * Cleaned up comments.
 *
 * Revision 1.1  1997/10/15 05:42:22  rich
 * Initial revision
 *
 * 
 */

#ifndef _MenuArrowP_h
#define _MenuArrowP_h
#include "ArrowP.h"
#include "MenuArrow.h"

/*********************************************************************
 *
 * MenuArrow Widget Private Data
 *
 *********************************************************************/

/* New fields for the MenuArrow widget class record */

typedef struct _MenuArrowClassPart {
    int           	foo;	/* Null record entry */
} MenuArrowClassPart;

/* Full class record declaration */
typedef struct _MenuArrowClassRec {
    CoreClassPart       core_class;
    ArrowClassPart	arrow_class;
    MenuArrowClassPart  menuarrow_class;
} MenuArrowClassRec;

extern MenuArrowClassRec menuarrowClassRec;

/* New fields for the MenuArrow record */
typedef struct {
   /* resources */
    String              menu_name;	
    XtJustify           menu_justify;        /* Justification for the popup. */
    Boolean		align_parent;
} MenuArrowPart;

/* Full instance record declaration */
typedef struct _MenuArrowRec {
    CorePart            core;
    ArrowPart		arrow;
    MenuArrowPart       menuarrow;
} MenuArrowRec;

#endif /* _MenuArrowP_h */
