/*
 * Keyboard focus routines.
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
 * $Log: $
 *
 */

#ifndef lint
static char         rcsid[] = "$Id: $";

#endif

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xos.h>
#include <X11/Xmu/Drawing.h>
#include "focus.h"
#include <stdio.h>

typedef struct _focusWidget {
    Widget              self;	/* Widget in group */
    struct _focusWidget *next;	/* Next widget */
    struct _focusWidget *prev;	/* Previous Widget */
} focusWidget      , *FocusWidget;

typedef struct _focusGroup {
    Widget              toplevel;	/* Widget in group */
    FocusWidget         tree;	/* Tree */
    struct _focusGroup *next;	/* Next group */
    struct _focusGroup *prev;	/* Previous group */
} focusGroup       , *FocusGroup;

/* Top level focus group structure */
static FocusGroup   topgroup = NULL;

static void         _XpwDestroyFocusGroup(Widget /*w */ ,
					  XtPointer /*client_data */ ,
					  XtPointer /*call_data */ );
static FocusGroup   _XpwFindFocusGroup(Widget /*toplevel */ );
static FocusWidget  _XpwFindFocusWidget(FocusGroup /*grp */ , Widget /*w */ );
static void         _XpwRemoveFocusWidget(FocusGroup /*grp */ ,
					  FocusWidget /*fw */ );

/*
 * Add a widget to a focus group. Create a new toplevel if none exists.
 */
void
_XpwAddFocusGroup(w, fg)
	Widget              w;
	Widget              fg;
{
    FocusGroup          grp;
    FocusWidget         fw, nfw;

   /* Locate focus group for this tree */
    if ((grp = _XpwFindFocusGroup(fg)) == NULL) {
       /* Did not find one, so make new one */
	grp = (FocusGroup) XtMalloc(sizeof(focusGroup));
	grp->toplevel = fg;
	grp->tree = NULL;
	grp->next = grp->prev = NULL;
	if (topgroup != NULL) {
	    FocusGroup          ngrp = topgroup->next;

	    grp->next = ngrp;
	    grp->prev = topgroup;
	    topgroup->next = grp;
	    if (ngrp != NULL)
	    	ngrp->prev = grp;
	} else
	    topgroup = grp;
    }
    if ((fw = _XpwFindFocusWidget(grp, w)) != NULL)
	return;
   /* Did not find one, so make new one */
    nfw = (FocusWidget) XtMalloc(sizeof(focusWidget));
    nfw->self = w;
    if (grp->tree != NULL) {
       /* Find end of list */
	for (fw = grp->tree; fw->next != NULL; fw = fw->next) ;
	nfw->next = NULL;
	nfw->prev = fw;
	fw->next = nfw;
    } else {
	nfw->next = nfw->prev = NULL;
	grp->tree = nfw;
    }
   /* Make sure it will get removed */
    XtAddCallback(w, XtNdestroyCallback, _XpwDestroyFocusGroup,
		  (XtPointer) grp);
    return;
}

/*
 * Remove widget from focus group.
 */
void
_XpwClearFocusGroup(w, fg)
	Widget              w;
	Widget              fg;
{
    FocusGroup          grp;
    FocusWidget         fw;

   /* Locate focus group for this tree */
    grp = _XpwFindFocusGroup(fg);

    if ((fw = _XpwFindFocusWidget(grp, w)) == NULL)
	return;
    XtRemoveCallback(w, XtNdestroyCallback, _XpwDestroyFocusGroup,
		     (XtPointer) grp);
    _XpwRemoveFocusWidget(grp, fw);
}

/*
 * Remove widgets from groups on destroy widget calls.
 */
static void
_XpwDestroyFocusGroup(w, client_data, call_data)
	Widget              w;
	XtPointer           client_data;
	XtPointer           call_data;
{
    FocusGroup          grp = (FocusGroup) client_data;
    FocusWidget         fw;

    if ((fw = _XpwFindFocusWidget(grp, w)) == NULL)
	return;
    _XpwRemoveFocusWidget(grp, fw);
}

/*
 * Take the keyboard focus.
 */
void
_XpwSetFocus(w, fg)
	Widget              w;
	Widget              fg;
{
    XtSetKeyboardFocus(fg, w);
}

/*
 * Move the focus to another widget.
 */
Boolean
XpwShiftFocus(w, dir)
	Widget              w;
	XpwFocusDirection   dir;
{
    Widget              top = w;
    FocusGroup          grp = NULL;
    FocusWidget         fw;
    Arg                 arg[1];
    Boolean             ok;

   /* Find focus group of widget */
    while ((top = XtParent(top)) != None) {
	if ((grp = _XpwFindFocusGroup(top)) != NULL)
	    break;
    }
    if (grp == NULL)
	return FALSE;

    if ((fw = _XpwFindFocusWidget(grp, w)) == NULL)
	return False;

    XtSetArg(arg[0], XtNtraversalOn, &ok);
    switch (dir) {
    case XpwFocusHome:
	for (fw = grp->tree; fw != NULL; fw = fw->next) {
	    if (!XtIsManaged(fw->self))
		continue;
	    ok = False;
	    XtGetValues(fw->self, arg, 1);
	    if (ok == True)
		break;
	}
	break;
    case XpwFocusNext:
	for (fw = fw->next; fw != NULL; fw = fw->next) {
	    if (!XtIsManaged(fw->self))
		continue;
	    ok = False;
	    XtGetValues(fw->self, arg, 1);
	    if (ok == True)
		break;
	}
	break;
    case XpwFocusEnd:
	for (fw = grp->tree; fw->next != NULL; fw = fw->next) ;
	for (; fw != NULL; fw = fw->prev) {
	    if (!XtIsManaged(fw->self))
		continue;
	    ok = False;
	    XtGetValues(fw->self, arg, 1);
	    if (ok == True)
		break;
	}
	break;
    case XpwFocusPrev:
	for (fw = fw->prev; fw != NULL; fw = fw->prev) {
	    if (!XtIsManaged(fw->self))
		continue;
	    ok = False;
	    XtGetValues(fw->self, arg, 1);
	    if (ok == True)
		break;
	}
	break;
    }
    XtSetKeyboardFocus(grp->toplevel, (fw != NULL) ? fw->self : None);
    return (fw != NULL);
}

/*
 * Find the group assiociated with this parent.
 * Create one if none yet.
 */
static              FocusGroup
_XpwFindFocusGroup(toplevel)
	Widget              toplevel;
{
    FocusGroup          grp;

    for (grp = topgroup; grp != NULL; grp = grp->next) {
	if (grp->toplevel == toplevel)
	    return grp;
    }
    return NULL;
}

/*
 * Find the entry for widget in tree.
 */
static              FocusWidget
_XpwFindFocusWidget(grp, w)
	FocusGroup          grp;
	Widget              w;
{
    FocusWidget         fw, nfw;

    for (fw = grp->tree; fw != NULL; fw = fw->next) {
	if (fw->self == w)
	    return fw;
    }
    return NULL;
}

/*
 * Remove a widget from tree, remove tree entry if nothing left.
 */
static void
_XpwRemoveFocusWidget(grp, fw)
	FocusGroup          grp;
	FocusWidget         fw;
{
    FocusWidget         nfw = fw->next;
    FocusWidget         pfw = fw->prev;
    FocusGroup          ngrp, pgrp;

    if (pfw != NULL)
	pfw->next = nfw;
    if (nfw != NULL)
	nfw->prev = pfw;
    if (fw == grp->tree)
	grp->tree = nfw;
    XtFree((XtPointer) fw);

    if (grp->tree != NULL)
	return;

    ngrp = grp->next;
    pgrp = grp->prev;
    if (pgrp != NULL)
	pgrp->next = ngrp;
    if (ngrp != NULL)
	ngrp->prev = pgrp;
    if (topgroup == grp)
	topgroup = ngrp;
    XtFree((XtPointer) grp);
}
