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
 * $Log: Stack.c,v $
 * Revision 1.1  1997/10/12 05:17:24  rich
 * Initial revision
 *
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: Stack.c,v 1.1 1997/10/12 05:17:24 rich Exp rich $";

#endif

#include <X11/IntrinsicP.h>
#include <X11/cursorfont.h>
#include <X11/StringDefs.h>

#include <X11/Xmu/Misc.h>
#include <X11/Xmu/Converters.h>

#include "XpwInit.h"
#include <stdio.h>
#include "StackP.h"

#include <ctype.h>

#define ChildInfo(w)	((StackConstraintsPart *)(w)->core.constraints)

#define ForAllChildren(self, childP) \
  for ( (childP) = (self)->composite.children ; \
        (childP) < (self)->composite.children + (self)->composite.num_children ; \
        (childP)++ )

/*****************************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************************/

/* Semi-public interface functions */
static void         Initialize(Widget /*request */ , Widget /*new */ ,
			  ArgList /*args */ , Cardinal * /*num_args */ );
static Boolean      SetValues(Widget /*old */ , Widget /*request */ ,
			      Widget /*new */ , ArgList /*args */ ,
			      Cardinal * /*num_args */ );
static void         Realize(Widget /*w */ , Mask * /*valueMask */ ,
			    XSetWindowAttributes * /*attributes */ );
static XtGeometryResult QueryGeometry(Widget /*w */ ,
				      XtWidgetGeometry * /*intended */ ,
				    XtWidgetGeometry * /*return_val */ );
static XtGeometryResult GeometryManager(Widget /*w */ ,
					XtWidgetGeometry * /*request */ ,
					XtWidgetGeometry * /*reply */ );
static void         Resize(Widget /*w */ );
static void         InsertChild(Widget /*w */ );
static void         DeleteChild(Widget /*w */ );
static void         ChangeManaged(Widget /*w */ );

/* private functions */

static void         ManageChild(StackWidget /*self */ );
static void         SetChildrenSizes(StackWidget /*self */ );
static void         FindBoxSize(StackWidget /*self */ , Dimension * /*width */ ,
				Dimension * /*height */ );
static void         MoveChildren(StackWidget /*self */ );
static void         ResizeStack(StackWidget /*self */ , int /*width */ ,
				int /*height */ ,
				XtGeometryResult * /*result_ret */ ,
				Dimension * /*nwidth */ ,
				Dimension * /*nheight */ );

#define SuperClass ((ConstraintWidgetClass)&constraintClassRec)

StackClassRec       stackClassRec =
{
    {	/* core class fields */
	(WidgetClass) SuperClass,	/* superclass          */
	"Stack",			/* class name          */
	sizeof(StackRec),		/* size                */
	XpwInitializeWidgetSet,		/* class_initialize    */
	NULL,				/* class_part init     */
	FALSE,				/* class_inited        */
	Initialize,			/* initialize          */
	NULL,				/* initialize_hook     */
	Realize,			/* realize             */
	NULL,				/* actions             */
	0,				/* num_actions         */
	NULL,				/* resources           */
	0,				/* resource_count      */
	NULLQUARK,			/* xrm_class           */
	TRUE,				/* compress_motion     */
	TRUE,				/* compress_exposure   */
	TRUE,				/* compress_enterleave */
	FALSE,				/* visible_interest    */
	NULL,				/* destroy             */
	Resize,				/* resize              */
	NULL,				/* expose              */
	SetValues,			/* set_values          */
	NULL,				/* set_values_hook     */
	XtInheritSetValuesAlmost,	/* set_values_almost   */
	NULL,				/* get_values_hook     */
	NULL,				/* accept_focus        */
	XtVersion,			/* version             */
	NULL,				/* callback_private    */
	NULL,				/* tm_table            */
	QueryGeometry,			/* query_geometry      */
	XtInheritDisplayAccelerator,	/* display_accelerator */
	NULL				/* extension           */
    },
    {	/* composite class fields */
	GeometryManager,		/* geometry_manager    */
	ChangeManaged,			/* change_managed      */
	InsertChild,			/* insert_child        */
	DeleteChild,			/* delete_child        */
	NULL				/* extension           */
    },
    {	/* constraint class fields */
	NULL,				/* subresources        */
	0,				/* subresource_count   */
	sizeof(StackConstraintsRec),	/* constraint_size     */
	NULL,				/* initialize          */
	NULL,				/* destroy             */
	NULL,				/* set_values          */
	NULL				/* extension           */
    }
};

WidgetClass         stackWidgetClass = (WidgetClass) & stackClassRec;

/************************************************************
 *
 * Semi-public routines. 
 *
 ************************************************************/

/*
 * Set up the widget.
 */
/* ARGSUSED */
static void
Initialize(request, new, args, num_args)
	Widget              request, new;
	ArgList             args;
	Cardinal           *num_args;
{
    StackWidget         self = (StackWidget) new;

    self->stack.recursively_called = False;
    self->stack.num_children = 0;
    self->stack.showing = NULL;
    self->core.width = 10;
    self->core.height = 10;
}

/*
 * On set values, refigure of something change.
 */
/* ARGSUSED */
static              Boolean
SetValues(old, request, new, args, num_args)
	Widget              old, request, new;
	ArgList             args;
	Cardinal           *num_args;
{
    return FALSE;
}

static void
Realize(w, valueMask, attributes)
	Widget              w;
	Mask               *valueMask;
	XSetWindowAttributes *attributes;
{
    StackWidget         self = (StackWidget) w;
    Dimension           width = 0, height = 0;
    Widget             *childP;

    (*SuperClass->core_class.realize) (w, valueMask, attributes);

    ForAllChildren(self, childP)
	XtRealizeWidget(*childP);

   /* Set children to there prefered sizes */
    SetChildrenSizes(self);
    FindBoxSize(self, &width, &height);

   /* Size us big enough for children */
    ResizeStack(self, width, height, (XtGeometryResult *) NULL,
		(Dimension *) NULL, (Dimension *) NULL);

   /* Relayout and move children */
    MoveChildren(self);
}

static              XtGeometryResult
QueryGeometry(w, intended, return_val)
	Widget              w;
	XtWidgetGeometry   *intended, *return_val;
{
    StackWidget         self = (StackWidget) w;
    Dimension           width, height;
    XtGeometryResult    ret_val = XtGeometryYes;
    XtGeometryMask      mode = intended->request_mode;

    SetChildrenSizes(self);
    FindBoxSize(self, &width, &height);

    if (((mode & CWWidth) && (intended->width != width)) ||
	!(mode & CWWidth)) {
	return_val->request_mode |= CWWidth;
	return_val->width = width;
	ret_val = XtGeometryAlmost;
    }
    if (((mode & CWHeight) && (intended->height != height)) ||
	!(mode & CWHeight)) {
	return_val->request_mode |= CWHeight;
	return_val->height = height;
	ret_val = XtGeometryAlmost;
    }
    if (ret_val == XtGeometryAlmost) {
	mode = return_val->request_mode;

	if (((mode & CWWidth) && (width == self->core.width)) &&
	    ((mode & CWHeight) && (height == self->core.height)))
	    return (XtGeometryNo);
    }
    return (ret_val);
}

/*
 * The Geometry Manager only allows changes after Realize.
 */

static              XtGeometryResult
GeometryManager(w, request, reply)
	Widget              w;
	XtWidgetGeometry   *request, *reply;
{
    StackWidget         self = (StackWidget) XtParent(w);
    XtGeometryMask      mask = request->request_mode;
    StackConstraintsPart *child = ChildInfo(w);
    Dimension           owidth, oheight;
    Dimension           oswidth, osheight;
    Dimension           rwidth, rheight;
    Dimension           nwidth, nheight;
    XtGeometryResult    result;
    Boolean             almost = FALSE;

   /* If widget not realized, child is requesting change
    * to unmanaged size, same size, not a width or height change, or the
    * request is to it's current size return XtGeometryNo.
    */
    if (!XtIsRealized(w))
	return XtGeometryNo;

    if ((mask & CWWidth) && request->width == self->core.width
	&&
	(mask & CWHeight) && request->height == self->core.height)
	return XtGeometryNo;

   /* Save old infomation. */
    owidth = child->pref_w;
    oheight = child->pref_h;
    oswidth = self->core.width;
    osheight = self->core.height;

   /* Put in new info */
    child->pref_w = request->width;
    child->pref_h = request->height;

   /* Now see if new size will change us */
    FindBoxSize(self, &rwidth, &rheight);

   /* If there was no change, just return a no answer */
    if (rwidth == oswidth && rheight == osheight)
	return XtGeometryNo;

   /* Try to resize requested size, allow parent to resize if needed */
    ResizeStack(self, rwidth, rheight, &result, &nwidth, &nheight);

   /* If we did not fail, set size of child to returned size */
    if (result != XtGeometryNo) {
       /* Set children based on new size */
	self->core.height = nheight;
	self->core.width = nwidth;
	reply->height = nheight;
	reply->width = nwidth;
    } else {
       /* Restore damage we did and put results into reply */
	self->core.height = osheight;
	self->core.width = oswidth;
	reply->height = osheight;
	reply->width = oswidth;
    }

   /* If we did not match requested or this was just a Query */
    almost = request->height != reply->height;
    almost |= request->width != reply->width;

    if ((mask & XtCWQueryOnly) || almost) {
       /* Return Almost and restore old locations */
	child->pref_w = owidth;
	child->pref_h = oheight;
	reply->request_mode = CWWidth | CWHeight;
	if (almost)
	    return XtGeometryAlmost;
    } else {

       /* If we matched, then actualy resize */
	ResizeStack(self, self->core.width, self->core.height,
		    (XtGeometryResult *) NULL,
		    (Dimension *) NULL, (Dimension *) NULL);
	MoveChildren(self);
    }
    return XtGeometryDone;
}

/*
 * Resize widget
 */

static void
Resize(w)
	Widget              w;
{
    StackWidget         self = (StackWidget) w;

    if (XtIsRealized(w))
       /* Move children to there new places */
	MoveChildren(self);
}

static void
InsertChild(w)
	Widget              w;
{
    StackConstraintsPart *child = ChildInfo(w);
    StackWidget         self = (StackWidget) XtParent(w);

   /* insert the child widget in the composite children list with the */
   /* superclass insert_child routine.                                */
    (*SuperClass->composite_class.insert_child) (w);

    child->pref_h = 0;
    child->pref_w = 0;

    self->stack.num_children++;
    if (self->stack.showing == NULL)
	self->stack.showing = w;

    ManageChild(self);
}

static void
DeleteChild(w)
	Widget              w;
{
    StackWidget         self = (StackWidget) XtParent(w);
    int                 redo = FALSE;

    if (self->stack.showing == w) {
	self->stack.showing = NULL;
	redo = TRUE;
    }
   /* delete the child widget in the composite children list with the */
   /* superclass delete_child routine.                                */
    (*SuperClass->composite_class.delete_child) (w);

    self->stack.num_children--;
    if (redo)
	ManageChild(self);
}

static void
ChangeManaged(w)
	Widget              w;
{
    StackWidget         self = (StackWidget) w;
    Dimension           width, height;
    Widget             *childP;
    Boolean             remanage = FALSE;

   /* Don't allow us to be called recursivly */
    if (self->stack.recursively_called++)
	return;

   /* Compute number of children */
    self->stack.num_children = 0;
    ForAllChildren(self, childP) {
	self->stack.num_children++;
	if (XtIsManaged(*childP) && self->stack.showing != *childP) {
	    self->stack.showing = *childP;
	    remanage = TRUE;
	}
    }

   /* Set children to there prefered sizes */
    SetChildrenSizes(self);
   /* See how big we need to be. */
    FindBoxSize(self, &width, &height);

   /* Size us big enough for children */
    ResizeStack(self, width, height, (XtGeometryResult *) NULL,
		(Dimension *) NULL, (Dimension *) NULL);

   /* If we are realized, move children */
    if (XtIsRealized(w))
	MoveChildren(self);

   /* We are done */
    self->stack.recursively_called = 0;

   /* If someone else changed management, update our idea */
    if (remanage)
	ManageChild(self);
}

/************************************************************
 *
 * Private functions
 *
 ************************************************************/

/*
 * Make sure the managed child is properly showing.
 */
static void
ManageChild(self)
	StackWidget         self;
{
    Widget             *childP;

    if (self->stack.num_children == 0)
	return;

    if (self->stack.recursively_called++)
	return;

    if (self->stack.showing == NULL)
	self->stack.showing = *self->composite.children;

    ForAllChildren(self, childP) {
	if (self->stack.showing == *childP) {
	    if (!XtIsManaged(*childP))
		XtManageChild(*childP);
	} else {
	    if (XtIsManaged(*childP))
		XtUnmanageChild(*childP);
	}
    }
    self->stack.recursively_called = 0;
}

/*
 * Set size of each child to it's prefered size.
 */
static void
SetChildrenSizes(self)
	StackWidget         self;
{
    Widget             *childP;
    XtWidgetGeometry    request, reply;
    XtGeometryResult    ret_val;

    ForAllChildren(self, childP) {
	StackConstraintsPart *child = ChildInfo(*childP);

	request.request_mode = CWWidth | CWHeight;
	request.width = self->core.width;
	request.height = self->core.height;
	ret_val = XtQueryGeometry(*childP, &request, &reply);
	if (reply.request_mode & CWHeight)
	    child->pref_h = reply.height;
	else
	    child->pref_h = self->core.height;
	if (reply.request_mode & CWWidth)
	    child->pref_w = reply.width;
	else
	    child->pref_w = self->core.width;
    }
}

/*
 * Find box big enough to fit all requested sizes.
 */

static void
FindBoxSize(self, width, height)
	StackWidget         self;
	Dimension          *width;
	Dimension          *height;
{
    Widget             *childP;

    *width = 0;
    *height = 0;
    ForAllChildren(self, childP) {
	StackConstraintsPart *child = ChildInfo(*childP);

	if (*width < child->pref_w)
	    *width = child->pref_w;
	if (*height < child->pref_h)
	    *height = child->pref_h;
    }
}

/*
 * Move children to their new home.
 */
static void
MoveChildren(self)
	StackWidget         self;
{
    Widget             *childP;

    ForAllChildren(self, childP) {
	XtMoveWidget(*childP, (Position) 0, (Position) 0);
	XtResizeWidget(*childP, self->core.width, self->core.height, 0);
    }
}

/*
 * Resize Stack widget itself.
 */
static void
ResizeStack(self, width, height, result_ret, nwidth, nheight)
	StackWidget         self;
	Dimension           width, height;
	XtGeometryResult   *result_ret;
	Dimension          *nwidth, *nheight;
{
    XtWidgetGeometry    request, reply;

    request.request_mode = CWWidth | CWHeight;

    if (width < 1)
	width = 1;
    if (height < 1)
	height = 1;

    request.width = width;
    request.height = height;

    if (result_ret != NULL) {
	request.request_mode |= XtCWQueryOnly;

       /* Call parent and see if we can grow */
	*result_ret = XtMakeGeometryRequest((Widget) self, &request, &reply);
	if (*result_ret == XtGeometryNo ||
	    (width == self->core.width && height == self->core.height)) {
	    *nwidth = self->core.width;
	    *nheight = self->core.height;
	    return;
	}
	if (*result_ret != XtGeometryAlmost) {
	    *nwidth = request.width;
	    *nheight = request.height;
	    return;
	}
	*nwidth = reply.width;
	*nheight = reply.height;
	return;
    } else {
	if (width == self->core.width && height == self->core.height)
	    return;

	if (XtMakeGeometryRequest((Widget) self, &request, &reply) == XtGeometryAlmost)
	    XtMakeGeometryRequest((Widget) self, &reply, &request);
    }
}

/************************************************************
 *
 * Public routines. 
 *
 ************************************************************/

/*
 * Returns the number of children in the stack widget.
 */

int
XpwStackGetNumSub(w)
	Widget              w;

{
    return ((StackWidget) w)->stack.num_children;
}

/*
 * Set which child to display.
 */

void
XpwStackSetChild(w, childw)
	Widget              w;
	Widget              childw;
{
    StackWidget         self = (StackWidget) w;

    if (XtParent(childw) == w) {
	self->stack.showing = childw;
	ManageChild(self);
    }
}

/*
 * Show next child in loop.
 */
void
XpwStackNextChild(w)
	Widget              w;
{
    StackWidget         self = (StackWidget) w;
    Widget             *childP;
    int                 i = self->stack.num_children;

    ForAllChildren(self, childP) {
	if (*childP == self->stack.showing) {
	    if (i > 0)
		self->stack.showing = *++childP;
	    else
		self->stack.showing = NULL;
	    ManageChild(self);
	    break;
	}
	i++;
    }
}

/*
 * Show prev child in loop.
 */
void
XpwStackPrevChild(w)
	Widget              w;
{
    StackWidget         self = (StackWidget) w;
    Widget             *childP;
    int                 i = 0;

    ForAllChildren(self, childP) {
	if (*childP == self->stack.showing) {
	    if (i > 0)
		self->stack.showing = *--childP;
	    else
		self->stack.showing = NULL;
	    ManageChild(self);
	    break;
	}
	i++;
    }
}

/*
 * Show child number loop.
 */
void
XpwStackShowChild(w, child)
	Widget              w;
	int                 child;
{
    StackWidget         self = (StackWidget) w;

    if (child < 0 || child > self->stack.num_children)
	return;
    self->stack.showing = self->composite.children[child];
    ManageChild(self);
}
