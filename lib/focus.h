/*
 * focus.h - Global keyboard focus stuff.
 *
 * Handle keyboard focus for widgets.
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
 * $Id: focus.h,v 1.1 1997/10/25 22:16:54 rich Beta $
 *
 * $Log: focus.h,v $
 * Revision 1.1  1997/10/25 22:16:54  rich
 * Initial revision
 *
 *
 */

#ifndef _Focus_h_
#define _Focus_h_

typedef enum { XpwFocusHome, XpwFocusNext, XpwFocusEnd, XpwFocusPrev}
	 XpwFocusDirection;

#ifndef XtNtraversalOn
#define XtNtraversalOn	"traversalOn"
#define XtCTraversalOn	"TraversalOn"
#endif

_XFUNCPROTOBEGIN

/*
 * Add a widget to a focus group.
 */
void _XpwAddFocusGroup(
#if NeedFunctionPrototypes
			Widget /*w*/,
			Widget /*fg*/
#endif
);

/*
 * Clears focus group associated with widget.
 */
void _XpwClearFocusGroup(
#if NeedFunctionPrototypes
			Widget /*w*/,
			Widget /*fg*/
#endif
);

/*
 * Sets the keyboard focus to widget.
 */
void _XpwSetFocus(
#if NeedFunctionPrototypes
			Widget /*w*/,
			Widget /*fg*/
#endif
);

/*
 * Move focus based on direction.
 */
Boolean XpwShiftFocus(
#if NeedFunctionPrototypes
			Widget /*w*/,
			XpwFocusDirection /*dir*/
#endif
);

_XFUNCPROTOEND

#endif /* _Focus_h_ */
