/*
 * RowCol.c - RowCol Composite Widget.
 *
 * RowCol arranges it's children in rows or columns.
 
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
 * $Log:$
 *
 */

#ifndef lint
static char        *rcsid = "$Id$";

#endif

#include <X11/IntrinsicP.h>
#include <X11/cursorfont.h>
#include <X11/StringDefs.h>

#include <X11/Xmu/Misc.h>
#include <X11/Xmu/Converters.h>

#include "XpwInit.h"
#include <stdio.h>
#include "RowColP.h"

#include <ctype.h>

#define ChildInfo(w)	((RowColConstraintsPart *)(w)->core.constraints)
#define IsVert(w)       ( (w)->rowcol.orientation == XtorientVertical )
#define ChildSize(w, vert)   (((vert))?((w)->core.height):((w)->core.width))
#define GetRequestInfo(geo, vert)  (((vert)?((geo)->height):((geo)->width)))

#define ForAllChildren(self, childP) \
  for ( (childP) = (self)->composite.children ; \
        (childP) < (self)->composite.children + (self)->composite.num_children ; \
        (childP)++ )

/*****************************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************************/

#define offset(field) XtOffsetOf(RowColRec, rowcol.field)

static XtResource   resources[] =
{
    {XtNrefigureMode, XtCBoolean, XtRBoolean, sizeof(Boolean),
     offset(refiguremode), XtRImmediate, (XtPointer) TRUE},
    {XtNorientation, XtCOrientation, XtROrientation, sizeof(XtOrientation),
     offset(orientation), XtRImmediate, (XtPointer) XtorientVertical},
    {XtNpacking, XtCPackingType, XtRPackingType, sizeof(XpwPackingType),
     offset(packing), XtRImmediate, (XtPointer) XpwFill},
    {XtNspacing, XtCSpacing, XtRDimension, sizeof(Dimension),
     offset(spacing), XtRImmediate, (XtPointer) 2}
};

#undef offset

#define offset(field) XtOffsetOf(RowColConstraintsRec, rowcol.field)

static XtResource   subresources[] =
{
    {XtNallowResize, XtCBoolean, XtRBoolean, sizeof(Boolean),
     offset(allow_resize), XtRImmediate, (XtPointer) FALSE},
    {XtNresizeToPreferred, XtCBoolean, XtRBoolean, sizeof(Boolean),
     offset(resize_to_pref), XtRImmediate, (XtPointer) TRUE},
    {XtNskipAdjust, XtCBoolean, XtRBoolean, sizeof(Boolean),
     offset(skip_adjust), XtRImmediate, (XtPointer) FALSE},
    {XtNmin, XtCMin, XtRDimension, sizeof(Dimension),
     offset(min), XtRImmediate, (XtPointer) 1},
    {XtNmax, XtCMax, XtRDimension, sizeof(Dimension),
     offset(max), XtRImmediate, (XtPointer) -1},
};

#undef offset

/* Semi-public interface functions */
static void         ClassInitialize(void);
static void         Initialize(Widget /*request */ , Widget /*new */ ,
			         ArgList /*args */ , Cardinal * /*num_args */ );
static Boolean      SetValues(Widget /*old */ , Widget /*request */
			         , Widget /*new */ , ArgList /*args */ ,
			         Cardinal * /*num_args */ );
static Boolean      ChildSetValues(Widget /*old */ , Widget /*request */ ,
			         Widget /*new */ , ArgList /*args */ ,
			         Cardinal * /*num_args */ );
static void         Realize(Widget /*w */ , Mask * /*valueMask */ ,
			         XSetWindowAttributes * /*attributes */ );
static XtGeometryResult GeometryManager(Widget /*w */ ,
			         XtWidgetGeometry * /*request */ ,
			         XtWidgetGeometry * /*reply */ );
static void         Resize(Widget /*w */ );
static void         InsertChild(Widget /*w */ );
static void         ChangeManaged(Widget /*w */ );

/* private functions */
static void         SetChildrenSizes(RowColWidget /*self */ , int /*off_size */ );
static void         MoveChildren(RowColWidget /*self */ );
static void         PositionChildren(RowColWidget /*self */ );
static void         ResizeChildren(RowColWidget /*self */ , int * /*size */ );
static void         ResizeRowCol(RowColWidget /*self */ , int /*off_size */ ,
			         XtGeometryResult * /*result_ret */ ,
			         Dimension * /*on_size_ret */ ,
			         Dimension * /*off_size_ret */ );

/* type converters */
Boolean             cvtStringToPackingType(Display * /*dpy */ ,
			         XrmValuePtr /*args */ ,
			         Cardinal * /*num_args */ ,
			         XrmValuePtr /*from */ ,
			         XrmValuePtr /*to */ ,
			         XtPointer * /*converter_data */ );
Boolean             cvtPackingTypeToString(Display * /*dpy */ ,
			         XrmValuePtr /*args */ ,
			         Cardinal * /*num_args */ ,
			         XrmValuePtr /*from */ ,
			         XrmValuePtr /*to */ ,
			         XtPointer * /*converter_data */ );

#define SuperClass ((ConstraintWidgetClass)&constraintClassRec)

RowColClassRec      rowcolClassRec =
{
    {	/* core class fields */
	(WidgetClass) SuperClass,	/* superclass          */
	"RowCol",			/* class name          */
	sizeof(RowColRec),		/* size                */
	ClassInitialize,		/* class_initialize    */
	NULL,				/* class_part init     */
	FALSE,				/* class_inited        */
	Initialize,			/* initialize          */
	NULL,				/* initialize_hook     */
	Realize,			/* realize             */
	NULL,				/* actions             */
	0,				/* num_actions         */
	resources,			/* resources           */
	XtNumber(resources),		/* resource_count      */
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
	XtInheritQueryGeometry,		/* query_geometry      */
	XtInheritDisplayAccelerator,	/* display_accelerator */
	NULL				/* extension           */
    },
    {	/* composite class fields */
	GeometryManager,		/* geometry_manager    */
	ChangeManaged,			/* change_managed      */
	InsertChild,			/* insert_child        */
	XtInheritDeleteChild,		/* delete_child        */
	NULL				/* extension           */
    },
    {	/* constraint class fields */
	subresources,			/* subresources        */
	XtNumber(subresources),		/* subresource_count   */
	sizeof(RowColConstraintsRec),	/* constraint_size     */
	NULL,				/* initialize          */
	NULL,				/* destroy             */
	ChildSetValues,			/* set_values          */
	NULL				/* extension           */
    }
};

WidgetClass         rowColWidgetClass = (WidgetClass) & rowcolClassRec;

/************************************************************
 *
 * Semi-public routines. 
 *
 ************************************************************/

/* 
 * Add required type converters.
 */

static void
ClassInitialize()
{
    XpwInitializeWidgetSet();
    XtAddConverter(XtRString, XtROrientation, XmuCvtStringToOrientation,
		   (XtConvertArgList) NULL, (Cardinal) 0);
    XtSetTypeConverter(XtRString, XtRPackingType, cvtStringToPackingType,
		       NULL, 0, XtCacheNone, NULL);
    XtSetTypeConverter(XtRPackingType, XtRString, cvtPackingTypeToString,
		       NULL, 0, XtCacheNone, NULL);

}

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
    RowColWidget        self = (RowColWidget) new;

    self->rowcol.recursively_called = False;
    self->rowcol.num_children = 0;
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
    RowColWidget        old_self = (RowColWidget) old;
    RowColWidget        new_self = (RowColWidget) new;

   /* If orientation not the same: */
    if (IsVert(old_self) != IsVert(new_self)) {
       /* Set size to zero and reconfigure children */
	if (IsVert(new_self))
	    new_self->core.width = 0;
	else
	    new_self->core.height = 0;
	ChangeManaged(new);
       /* Force a redraw */
	return TRUE;
    }

    if (old_self->rowcol.packing != new_self->rowcol.packing) {
	ChangeManaged(new);
       /* Force a redraw */
	return TRUE;
    }
    return FALSE;
}

/* ARGSUSED */
static              Boolean
ChildSetValues(old, request, new, args, num_args)
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
    RowColWidget        self = (RowColWidget) w;
    Widget             *childP;

    (*SuperClass->core_class.realize) (w, valueMask, attributes);

    ForAllChildren(self, childP)
	XtIsRealized(*childP);

   /* Set children to there prefered sizes */
    SetChildrenSizes(self, ChildSize(w, !IsVert(self)));
   /* Relayout and move children */
    PositionChildren(self);
    MoveChildren(self);
}

/* The Geometry Manager only allows changes after Realize if
 * allow_resize is True in the constraints record.  
 * 
 * For vertically rowcol widgets:
 *
 * It only allows height changes, but offers the requested height
 * as a compromise if both width and height changes were requested.
 *
 * For horizontal widgets the converse is true.
 * As all good Geometry Managers should, we will return No if the
 * request will have no effect; i.e. when the requestor is already
 * of the desired geometry.
 */

static              XtGeometryResult
GeometryManager(w, request, reply)
	Widget              w;
	XtWidgetGeometry   *request, *reply;
{
    RowColWidget        self = (RowColWidget) XtParent(w);
    XtGeometryMask      mask = request->request_mode;
    RowColConstraintsPart *child = ChildInfo(w);
    Dimension           old_size, old_wpsize, old_rowcol_size;
    Boolean             vert = IsVert(self);
    Dimension           on_size, off_size;
    XtGeometryResult    result;
    Boolean             almost = FALSE;

   /* If widget not realized, allow_resize is false, child is requesting change
    * to unmanaged size, same size, not a width or height change, or the
    * request is to it's current size return XtGeometryNo.
    */
    if ((XtIsRealized(w) && !child->allow_resize) ||
	!(mask & ((vert) ? CWHeight : CWWidth)) ||
	(mask & ~(CWWidth | CWHeight)) ||
	(GetRequestInfo(request, vert) == ChildSize(w, vert)))
	return XtGeometryNo;

   /* Save old infomation. */
    old_rowcol_size = ChildSize((Widget)self, vert);
    old_wpsize = child->wp_size;
    old_size = child->size;

    child->wp_size = child->size = GetRequestInfo(request, vert);

   /* Try relayout with requested size, allow parent to resize if needed */
    ResizeRowCol(self, ChildSize((Widget)self, !vert), &result,
		 &on_size, &off_size);

   /* If we did not fail, set size of child to returned size */
    if (result != XtGeometryNo) {
       /* Set children based on new size */
	if (vert)
	    self->core.height = on_size;
	else
	    self->core.width = on_size;
	PositionChildren(self);
    }
   /* Restore damage we did and put results into reply */
    if (vert) {
	self->core.height = old_rowcol_size;
	reply->height = child->size;
	reply->width = off_size;
    } else {
	self->core.width = old_rowcol_size;
	reply->height = off_size;
	reply->width = child->size;
    }
   /* If child asked for both sizes reply current for other */

    if (!((vert ? CWWidth : CWHeight) & mask))
	if (vert)
	    request->width = w->core.width;
	else
	    request->height = w->core.height;

   /* If we did not match requested or this was just a Query */
    almost = GetRequestInfo(request, !vert) != GetRequestInfo(reply, !vert);
    almost |= GetRequestInfo(request, vert) != GetRequestInfo(reply, vert);

    if ((mask & XtCWQueryOnly) || almost) {
       /* Return Almost and restore old locations */
	child->wp_size = old_wpsize;
	child->size = old_size;
	PositionChildren(self);
	reply->request_mode = CWWidth | CWHeight;
	if (almost)
	    return XtGeometryAlmost;
    } else {

       /* If we matched, then actualy move the children */
	ResizeRowCol(self, ChildSize((Widget)self, !vert),
		 (XtGeometryResult *) NULL, &on_size, &off_size);
	PositionChildren(self);
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
    RowColWidget        self = (RowColWidget) w;

   /* Set children to there prefered sizes */
    SetChildrenSizes(self, ChildSize(w, !IsVert(self)));
   /* Move children to there new places */
    PositionChildren(self);
    MoveChildren(self);
}

static void
InsertChild(w)
	Widget              w;
{
    RowColConstraintsPart *child = ChildInfo(w);

   /* insert the child widget in the composite children list with the */
   /* superclass insert_child routine.                                */
    (*SuperClass->composite_class.insert_child) (w);

    child->size = 0;
}

static void
ChangeManaged(w)
	Widget              w;
{
    RowColWidget        self = (RowColWidget) w;
    Boolean             vert = IsVert(self);
    Dimension           size;
    Widget             *childP;

   /* Don't allow us to be called recursivly */
    if (self->rowcol.recursively_called++)
	return;

   /* If other dimension is zero, set to widest */
    if ((size = ChildSize(w, !vert)) == 0) {
	size = 1;
	ForAllChildren(self, childP)
	    if (XtIsManaged(*childP) && (ChildSize(*childP, !vert) > size))
	    size = ChildSize(*childP, !vert);
    }
   /* Compute number of managed children */
    self->rowcol.num_children = 0;
    ForAllChildren(self, childP) {
	if (XtIsManaged(*childP))
	    self->rowcol.num_children++;
    }

   /* Set children to prefered sizes */
    SetChildrenSizes(self, size);
   /* See if we will fit, but don't request parent to resize */
    ResizeRowCol(self, size, (XtGeometryResult *) NULL, (Dimension *) NULL,
		 (Dimension *) NULL);
   /* If we are realized, move children */
    if (XtIsRealized(w)) {
	PositionChildren(self);
	MoveChildren(self);
    }
    self->rowcol.recursively_called = FALSE;
}

/************************************************************
 *
 * Private functions
 *
 ************************************************************/

/*
 * Set size of each child to it's prefered size.
 */
static void
SetChildrenSizes(self, off_size)
	RowColWidget        self;
	Dimension           off_size;
{
    Widget             *childP;
    Boolean             vert = IsVert(self);
    XtWidgetGeometry    request, reply;

    ForAllChildren(self, childP) {
	RowColConstraintsPart *child = ChildInfo(*childP);

	if (!XtIsManaged(*childP))
	    continue;

	if (vert) {
	    request.request_mode = CWWidth;
	    request.width = off_size;
	} else {
	    request.request_mode = CWHeight;
	    request.height = off_size;
	}
	if (XtQueryGeometry(*childP, &request, &reply)
	    == XtGeometryAlmost &&
	    (reply.request_mode = (vert ? CWHeight : CWWidth)))
	    child->wp_size = GetRequestInfo(&reply, vert);
	else
	    child->wp_size = ChildSize(*childP, vert);
	if (child->resize_to_pref)
	    child->size = child->wp_size;
	else
	    child->size = ChildSize(*childP, vert);
    }
}

/*
 * Move children to their new home.
 */
static void
MoveChildren(self)
	RowColWidget        self;
{
    Widget             *childP;
    Boolean             vert = IsVert(self);

    ForAllChildren(self, childP) {
	RowColConstraintsPart *child = ChildInfo(*childP);

	if (!XtIsManaged(*childP))
	    continue;

	if (vert) {
	    XtMoveWidget(*childP, (Position) 0, child->location);
	    XtResizeWidget(*childP, self->core.width,
				 (Dimension) child->size, 0);
	} else {
	    XtMoveWidget(*childP, child->location, (Position) 0);
	    XtResizeWidget(*childP, (Dimension) child->size,
				 self->core.height, 0);
	}
    }
}

/*
 * Position Children.
 *
 * Basicly we estimate the size total size of the children and spacing. If
 * that is not the same size as the window size, we call ResizeChildren
 * to attempt to better fill window. After we are done, we position the
 * children.
 */
static void
PositionChildren(self)
	RowColWidget        self;
{
    Boolean             vert = IsVert(self);
    int		        size, gap, loc;
    Widget             *childP;

    if (!XtIsRealized((Widget) self) ||
	self->rowcol.num_children == 0 || !self->rowcol.refiguremode)
	return;
   /* Compute estimate of size of children. */
    size = 0;
    ForAllChildren(self, childP) {
	if (!XtIsManaged(*childP))
	    continue;
	size += ChildInfo(*childP)->size;
    }

   /* If they don't fit try readjusting */
    if (size != ChildSize((Widget) self, IsVert(self))) 
	ResizeChildren(self, (int *) &size);

    size += (self->rowcol.num_children - 1) * self->rowcol.spacing;

   /* Compute gap if needed */
    if (size < ChildSize((Widget) self, IsVert(self)) &&
	self->rowcol.packing == XpwEven && self->rowcol.num_children > 2)
	gap = (ChildSize((Widget) self, IsVert(self)) - size) / (self->rowcol.num_children - 1);
    else if (size < ChildSize((Widget) self, IsVert(self)) &&
	self->rowcol.packing == XpwEven && self->rowcol.num_children == 2)
	gap = ChildSize((Widget) self, IsVert(self)) - size;
    else
	gap = 0;

    gap += self->rowcol.spacing;
    loc = 0;
   /* Now set there location */
    ForAllChildren(self, childP) {
	if (!XtIsManaged(*childP))
	    continue;
	ChildInfo(*childP)->location = loc;
	loc += ChildInfo(*childP)->size + gap;
    }

}

/*
 * Resize children.
 *
 * Step one we count the number of children that can be resized, if there is
 * none we exit immediatly, since we can't change anything. Next we remove
 * the spacing from window size and work without it. If we are in NoFill mode
 * We set all adjustable children to their preferred size. If we are less
 * then the window size, we are done. Otherwise we divide the discrpancy
 * from the number of resizeable windows and give each a amount. We repeat this
 * process until we equal window size or we don't make any changes.
 */
static void
ResizeChildren(self, size)
	RowColWidget        self;
	int                *size;
{
    int                 win_size = ChildSize((Widget) self, IsVert(self));
    int                 resizable = 0;
    int                 amount;
    Boolean             changed;
    Widget             *childP;

   /* Compute number of children we can play with */
    ForAllChildren(self, childP) {
	if (!XtIsManaged(*childP) || (ChildInfo(*childP)->skip_adjust) ||
	    (ChildInfo(*childP)->resize_to_pref &&
	      ChildInfo(*childP)->size == ChildInfo(*childP)->wp_size))
	    continue;
        resizable++;

    }

   /* If noone can change we are stuck */
    if (resizable == 0)
	return;

   /* Remove Gap size from computations */
    win_size -= (self->rowcol.num_children - 1) * self->rowcol.spacing;

   /* If we are not filling, then set all children left to their
    * prefered size.
    */
    if (self->rowcol.packing == XpwNoFill) {
    	ForAllChildren(self, childP) {
	    int                 nsize;

	    if (!XtIsManaged(*childP) || ChildInfo(*childP)->resize_to_pref
		|| ChildInfo(*childP)->skip_adjust)
		continue;

	    nsize = ChildInfo(*childP)->wp_size;
	    if (nsize > ChildInfo(*childP)->max)
		nsize = ChildInfo(*childP)->max;
	    if (nsize < ChildInfo(*childP)->min)
		nsize = ChildInfo(*childP)->min;
	    *size += nsize - ChildInfo(*childP)->size;
	    ChildInfo(*childP)->size = nsize;
	}
	/* If we got below the window size, leave */
	if (*size <= win_size)
	   return;
   }
    
   /* If still bigger, reduce any none prefered size widgets */
    changed = TRUE;
    while (*size != win_size && resizable != 0 && changed) {
	changed = FALSE;
	amount = (win_size - *size) / resizable;
	if (amount == 0)
	   break;
	ForAllChildren(self, childP) {
	    int                 nsize;

	    if (!XtIsManaged(*childP) || ChildInfo(*childP)->resize_to_pref
		|| ChildInfo(*childP)->skip_adjust)
		continue;

	    nsize = ChildInfo(*childP)->size + amount;
	    if (nsize > ChildInfo(*childP)->max)
		nsize = ChildInfo(*childP)->max;
	    if (nsize < ChildInfo(*childP)->min)
		nsize = ChildInfo(*childP)->min;
	    if (nsize != ChildInfo(*childP)->size) {
		changed = TRUE;
		*size += nsize - ChildInfo(*childP)->size;
		ChildInfo(*childP)->size = nsize;
	    }
	}
    }
}

/*
 * Resize RowCol widget itself.
 */
static void
ResizeRowCol(self, off_size, result_ret, on_size_ret, off_size_ret)
	RowColWidget        self;
	Dimension           off_size;
	XtGeometryResult   *result_ret;
	Dimension          *on_size_ret, *off_size_ret;
{
    Dimension           old_size = ChildSize((Widget) self, IsVert(self));
    Dimension           new_size = 0;
    Widget             *childP;
    XtWidgetGeometry    request, reply;
    int			resizable = 0;

    request.request_mode = CWWidth | CWHeight;

    ForAllChildren(self, childP) {
	if (!XtIsManaged(*childP))
	    continue;
	new_size += ChildInfo(*childP)->size;
	if (ChildInfo(*childP)->skip_adjust ||
	    (ChildInfo(*childP)->resize_to_pref &&
	      ChildInfo(*childP)->size == ChildInfo(*childP)->wp_size))
	    continue;
       resizable++;

    }

    new_size += (self->rowcol.num_children - 1) * self->rowcol.spacing;

    if (new_size < 1)
	new_size = 1;

   /* If we have resizables, don't allow us to shrink */
    if (resizable != 0 && new_size < old_size)
	return;

    if (IsVert(self)) {
	request.width = off_size;
	request.height = new_size;
    } else {
	request.width = new_size;
	request.height = off_size;
    }

    if (result_ret != NULL) {
	request.request_mode |= XtCWQueryOnly;

       /* Call parent and see if we can grow */
	*result_ret = XtMakeGeometryRequest((Widget) self, &request, &reply);
	if ((new_size == old_size) || (*result_ret == XtGeometryNo)) {
	    *on_size_ret = old_size;
	    *off_size_ret = off_size;
	    return;
	}
	if (*result_ret != XtGeometryAlmost) {
	    *on_size_ret = GetRequestInfo(&request, IsVert(self));
	    *off_size_ret = GetRequestInfo(&request, !IsVert(self));
	    return;
	}
	*on_size_ret = GetRequestInfo(&reply, IsVert(self));
	*off_size_ret = GetRequestInfo(&reply, !IsVert(self));
	return;
    } else {

	if (new_size == old_size)
	    return;
	if (XtMakeGeometryRequest((Widget) self, &request, &reply) == XtGeometryAlmost)
	    XtMakeGeometryRequest((Widget) self, &reply, &request);
    }
}

/************************************************************
 *
 * Type converters.
 *
 ************************************************************/

Boolean
cvtStringToPackingType(dpy, args, num_args, from, to, converter_data)
	Display            *dpy;
	XrmValuePtr         args;
	Cardinal           *num_args;
	XrmValuePtr         from;
	XrmValuePtr         to;
	XtPointer          *converter_data;
{
    String              s = (String) from->addr;
    static XpwPackingType result;

    if (*num_args != 0) {
	XtAppErrorMsg(XtDisplayToApplicationContext(dpy),
		      "cvtStringToPackingType", "wrongParameters",
		      "XtToolkitError",
		  "String to packing type conversion needs no arguments",
		      (String *) NULL, (Cardinal *) NULL);
	return FALSE;
    }
    if (XmuCompareISOLatin1(s, "fill") == 0)
	result = XpwFill;
    else if (XmuCompareISOLatin1(s, "nofill") == 0)
	result = XpwNoFill;
    else if (XmuCompareISOLatin1(s, "even") == 0)
	result = XpwEven;
    else {
	XtDisplayStringConversionWarning(dpy, s, XtRPackingType);
	return FALSE;
    }

    if (to->addr != NULL) {
	if (to->size < sizeof(XpwPackingType)) {
	    to->size = sizeof(XpwPackingType);
	    return FALSE;
	}
	*(XpwPackingType *) (to->addr) = result;
    } else
	(XpwPackingType *) to->addr = &result;
    to->size = sizeof(XpwPackingType);
    return TRUE;
}

Boolean
cvtPackingTypeToString(dpy, args, num_args, from, to, converter_data)
	Display            *dpy;
	XrmValuePtr         args;
	Cardinal           *num_args;
	XrmValuePtr         from;
	XrmValuePtr         to;
	XtPointer          *converter_data;
{
    static String       result;

    if (*num_args != 0) {
	XtAppErrorMsg(XtDisplayToApplicationContext(dpy),
		      "cvtPackingTypeToString", "wrongParameters",
		      "XtToolkitError",
		  "packing type to String conversion needs no arguments",
		      (String *) NULL, (Cardinal *) NULL);
	return FALSE;
    }
    switch (*(XpwPackingType *) from->addr) {
    case XpwFill:
	result = "fill";
    case XpwNoFill:
	result = "nofill";
    case XpwEven:
	result = "even";
    default:
	XtError("Illegal PackingType");
	return FALSE;
    }
    if (to->addr != NULL) {
	if (to->size < sizeof(String)) {
	    to->size = sizeof(String);
	    return FALSE;
	}
	*(String *) (to->addr) = result;
    } else
	(String *) to->addr = &result;
    to->size = sizeof(String);
    return TRUE;
}

/************************************************************
 *
 * Public routines. 
 *
 ************************************************************/

/* 
 * Allows a flag to be set the will inhibit the rowcol widgets relayout routine.
 */

void
XpwRowColSetRefigureMode(w, mode)
	Widget              w;
	Boolean             mode;

{
    ((RowColWidget) w)->rowcol.refiguremode = mode;
}

/*
 * Returns the number of children in the rowcol widget.
 */

int
XpwRowColGetNumSub(w)
	Widget              w;

{
    return ((RowColWidget) w)->rowcol.num_children;
}

/*
 * Allows a flag to be set that determines if the rowcol widget will allow
 * geometry requests from this child
 */

void
XpwRowColAllowResize(widget, allow_resize)
	Widget              widget;
	Boolean             allow_resize;

{
    ChildInfo(widget)->allow_resize = allow_resize;
}
