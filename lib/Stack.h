/*
 * Stack.c - Stack Composite Widget.
 *
 * Stack manager displays only one child at a time and sets it's window size
 * to hold the largest child.
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
 * $Id: Stack.h,v 1.1 1997/10/12 05:17:36 rich Beta $
 *
 * $Log: Stack.h,v $
 * Revision 1.1  1997/10/12 05:17:36  rich
 * Initial revision
 *
 *
 *
 */

#ifndef _Stack_h
#define _Stack_h

#include <X11/Constraint.h>
#include <X11/Xmu/Converters.h>

/****************************************************************
 *
 * Stack Widget (SubClass of CompositeClass)
 *
 ****************************************************************/

/* Stack Widget Resources:
 * 
 * From Core:
 *
 * Name                  Class             RepType          Default Value
 * ----                  -----             -------          -------------
 * background            Background        Pixel            XtDefaultBackground
 * border                BorderColor       Pixel            XtDefaultForeground
 * borderWidth           BorderWidth       Dimension        1
 * cursor                Cursor            Cursor           None
 * destroyCallback       Callback          Pointer          NULL
 * height                Height            Dimension        0
 * mappedWhenManaged     MappedWhenManaged Boolean          True
 * sensitive             Sensitive         Boolean          True
 * width                 Width             Dimension        0
 * x                     Position          Position         0
 * y                     Position          Position         0
 * 
 * 
 * Child Constraint Resources:
 * 
 * Name               Class             RepType         Default Value
 * ----               -----             -------         -------------
 * 
 */

/* Class record constant */
extern WidgetClass  stackWidgetClass;

typedef struct _StackClassRec *StackWidgetClass;
typedef struct _StackRec *StackWidget;

/************************************************************
 *
 *  Public Procedures 
 *
 ************************************************************/

_XFUNCPROTOBEGIN

/*
 * Set which child to display.
 */

extern void         XpwStackSetChild(
#if NeedFunctionPrototypes
		       Widget /* w */ ,
		       Widget /* childw */ 
#endif
);

/*
 * Show next child in loop.
 */
extern void         XpwStackNextChild(
#if NeedFunctionPrototypes
		       Widget /* w */ 
#endif
);

/*
 * Show prev child in loop.
 */
extern void         XpwStackPrevChild(
#if NeedFunctionPrototypes
		       Widget /* w */ 
#endif
);

/*
 * Show child number loop.
 */
extern void         XpwStackShowChild(
#if NeedFunctionPrototypes
		       Widget /* w */ ,
		       int    /* child */ 
#endif
);


/*
 * Returns the number of children in the rowCol widget.
 */

extern int          XpwStackGetNumSub(
#if NeedFunctionPrototypes
			 Widget		/* w */
#endif
);

_XFUNCPROTOEND

#endif /* _Stack_h */
