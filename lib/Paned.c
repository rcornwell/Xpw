/*
 * Paned.c - Paned Composite Widget.
 *
 * Paned arranges it's children in rows or columns, and provides a grip to allow
 * the user to resize individual panes.
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
 *
 */

#ifndef lint
static char        *rcsid = "$Id: $";

#endif

#include <X11/IntrinsicP.h>
#include <X11/cursorfont.h>
#include <X11/StringDefs.h>

#include <X11/Xmu/Misc.h>
#include <X11/Xmu/Converters.h>

#include "XpwInit.h"
#include <stdio.h>
#include "PanedP.h"
#include "Grip.h"

#include <ctype.h>

#define ChildInfo(w)	((PanedConstraints)(w)->core.constraints)
#define IsVert(w)       ( (w)->paned.orientation == XtorientVertical )
#define ChildSize(w, vert)   (((vert))?((w)->core.height):((w)->core.width))
#define GetRequestInfo(geo, vert)  (((vert)?((geo)->height):((geo)->width)))
#define IsGrip(w)	((w)->core.widget_class == gripWidgetClass)

#define ForAllChildren(self, childP) \
  for ( (childP) = (self)->composite.children ; \
        (childP) < (self)->composite.children + (self)->composite.num_children ; \
        (childP)++ )

/*****************************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************************/

static char         defGripTranslations[] =
    "<Btn1Down>:   Leave() GripAction(Start, This) \n\
     <Btn2Down>:   Leave() GripAction(Start, Only) \n\
   <Btn1Motion>:   GripAction(Move, This) \n\
   <Btn2Motion>:   GripAction(Move, Only) \n\
        <BtnUp>:   GripAction(Commit) \n\
  <EnterWindow>:   Enter() \n\
  <LeaveWindow>:   Leave() ";

#define offset(field) XtOffsetOf(PanedRec, paned.field)

static XtResource   resources[] =
{
    {XtNrefigureMode, XtCBoolean, XtRBoolean, sizeof(Boolean),
     offset(refiguremode), XtRImmediate, (XtPointer) TRUE},
    {XtNorientation, XtCOrientation, XtROrientation, sizeof(XtOrientation),
     offset(orientation), XtRImmediate, (XtPointer) XtorientVertical},
    {XtNpacking, XtCPackingType, XtRPackingType, sizeof(XpwPackingType),
     offset(packing), XtRImmediate, (XtPointer) XpwFill},
    {XtNgripIndent, XtCGripIndent, XtRPosition, sizeof(Position),
     offset(griploc), XtRImmediate, (XtPointer) 90},
    {XtNspacing, XtCSpacing, XtRDimension, sizeof(Dimension),
     offset(spacing), XtRImmediate, (XtPointer) 2},
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
     offset(foreground), XtRString, XtDefaultForeground},
    {XtNgripCursor, XtCCursor, XtRCursor, sizeof(Cursor),
     offset(grip_cursor), XtRImmediate, None},
    {XtNverticalGripCursor, XtCCursor, XtRCursor, sizeof(Cursor),
     offset(v_grip_cursor), XtRString, "sb_v_double_arrow"},
    {XtNhorizontalGripCursor, XtCCursor, XtRCursor, sizeof(Cursor),
     offset(h_grip_cursor), XtRString, "sb_h_double_arrow"},

    {XtNbetweenCursor, XtCCursor, XtRCursor, sizeof(Cursor),
     offset(adjust_this_cursor), XtRString, None},
    {XtNverticalBetweenCursor, XtCCursor, XtRCursor, sizeof(Cursor),
     offset(v_adjust_this_cursor), XtRString, "sb_left_arrow"},
    {XtNhorizontalBetweenCursor, XtCCursor, XtRCursor, sizeof(Cursor),
     offset(h_adjust_this_cursor), XtRString, "sb_up_arrow"},
    {XtNgripTranslations, XtCTranslations, XtRTranslationTable,
     sizeof(XtTranslations),
  offset(grip_translations), XtRString, (XtPointer) defGripTranslations},
};

#undef offset

#define offset(field) XtOffsetOf(PanedConstraintsRec, paned.field)

static XtResource   subresources[] =
{
    {XtNallowResize, XtCBoolean, XtRBoolean, sizeof(Boolean),
     offset(allow_resize), XtRImmediate, (XtPointer) FALSE},
    {XtNresizeToPreferred, XtCBoolean, XtRBoolean, sizeof(Boolean),
     offset(resize_to_pref), XtRImmediate, (XtPointer) TRUE},
    {XtNskipAdjust, XtCBoolean, XtRBoolean, sizeof(Boolean),
     offset(skip_adjust), XtRImmediate, (XtPointer) FALSE},
    {XtNautoGrip, XtCBoolean, XtRBoolean, sizeof(Boolean),
     offset(auto_grip), XtRImmediate, (XtPointer) TRUE},
    {XtNshowGrip, XtCBoolean, XtRBoolean, sizeof(Boolean),
     offset(show_grip), XtRImmediate, (XtPointer) FALSE},
    {XtNmin, XtCMin, XtRDimension, sizeof(Dimension),
     offset(min), XtRImmediate, (XtPointer) 1},
    {XtNmax, XtCMax, XtRDimension, sizeof(Dimension),
     offset(max), XtRImmediate, (XtPointer) - 1},
};

#undef offset

/* Semi-public interface functions */
static void         ClassInitialize(void);
static void         Initialize(Widget /*request */ , Widget /*new */ ,
				 ArgList /*args */ , Cardinal * /*num_args */ );
static Boolean      SetValues(Widget /*old */ , Widget /*request */ ,
				 Widget /*new */ , ArgList /*args */ ,
				 Cardinal * /*num_args */ );
static Boolean      ChildSetValues(Widget /*old */ , Widget /*request */ ,
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
static void         Destroy(Widget /*w */ );

/* private functions */
static int          SetChildrenSizes(PanedWidget /*self */ , int /*off_size */ );
static void         MoveChildren(PanedWidget /*self */ );
static void         PositionChildren(PanedWidget /*self */ );
static void         ResizeChildren(PanedWidget /*self */ , int * /*size */ );
static void         ResizePaned(PanedWidget /*self */ , int /*off_size */ ,
				 XtGeometryResult * /*result_ret */ ,
				 Dimension * /*on_size_ret */ ,
				 Dimension * /*off_size_ret */ );
static void         DrawOutlines(PanedWidget /*self */ );
static void         StartUserAdjust(PanedWidget /*self */ );
static void         CommitNewLocations(PanedWidget /*self */ );
static void         MakeGrip(PanedConstraints /*child */ ,
				 PanedWidget /*self */ );
static void         HandleGrip(Widget /*wid */ , XtPointer /*client_data */ ,
				 XtPointer /*call_data */ );
static void         CreateGC(PanedWidget /*self */ );

/* type converters */
extern Boolean      cvtStringToPackingType(Display * /*dpy */ ,
					   XrmValuePtr /*args */ ,
					   Cardinal * /*num_args */ ,
					   XrmValuePtr /*from */ ,
					   XrmValuePtr /*to */ ,
				       XtPointer * /*converter_data */ );
extern Boolean      cvtPackingTypeToString(Display * /*dpy */ ,
					   XrmValuePtr /*args */ ,
					   Cardinal * /*num_args */ ,
					   XrmValuePtr /*from */ ,
					   XrmValuePtr /*to */ ,
				       XtPointer * /*converter_data */ );

#define SuperClass ((ConstraintWidgetClass)&constraintClassRec)

PanedClassRec       panedClassRec =
{
    {	/* core class fields */
	(WidgetClass) SuperClass,	/* superclass          */
	"Paned",			/* class name          */
	sizeof(PanedRec),		/* size                */
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
	Destroy,			/* destroy             */
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
	subresources,			/* subresources        */
	XtNumber(subresources),		/* subresource_count   */
	sizeof(PanedConstraintsRec),	/* constraint_size     */
	NULL,				/* initialize          */
	NULL,				/* destroy             */
	ChildSetValues,			/* set_values          */
	NULL				/* extension           */
    }
};

WidgetClass         panedWidgetClass = (WidgetClass) & panedClassRec;

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
    PanedWidget         self = (PanedWidget) new;

    self->paned.recursively_called = False;
    self->paned.num_children = 0;
    CreateGC(self);
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
    PanedWidget         old_self = (PanedWidget) old;
    PanedWidget         new_self = (PanedWidget) new;

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
    if (old_self->paned.foreground != new_self->paned.foreground) {
	XtReleaseGC(old, old_self->paned.grip_gc);
	CreateGC(new_self);
    }
    if (old_self->paned.packing != new_self->paned.packing) {
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
    PanedConstraints    old_self = ChildInfo(old);
    PanedConstraints	new_self = ChildInfo(new);
    PanedWidget         self = (PanedWidget) XtParent(new);

    if ((old_self->auto_grip != new_self->auto_grip) ||
	(old_self->show_grip != new_self->show_grip) ||
	(old_self->resize_to_pref != new_self->resize_to_pref)) {
       /* Update number of resizeable panes */
	if (old_self->show_grip || (old_self->auto_grip && !old_self->resize_to_pref))
	    self->paned.num_resize--;
	if (new_self->show_grip || (new_self->auto_grip && !new_self->resize_to_pref))
	    self->paned.num_resize++;
	MakeGrip(new_self, self);
       /* If we made a new grip make sure it gets realized */
	if (new_self->grip && !XtIsRealized(new_self->grip))
	    XtRealizeWidget(new_self->grip);
       /* Manage grip.. this will cause ChangeManaged to get called and
        * position grip as needed.   
        */
	XtManageChild(new_self->grip);
	return TRUE;
    }
    return FALSE;
}

static void
Realize(w, valueMask, attributes)
	Widget              w;
	Mask               *valueMask;
	XSetWindowAttributes *attributes;
{
    PanedWidget         self = (PanedWidget) w;
    Widget             *childP;

    (*SuperClass->core_class.realize) (w, valueMask, attributes);

    ForAllChildren(self, childP)
	XtRealizeWidget(*childP);

   /* Update grips and children places. */
    ChangeManaged(w);
}

/*
 * Calculate preferred size, given constraining box, caching it in the widget.
 */

static              XtGeometryResult
QueryGeometry(w, intended, return_val)
	Widget              w;
	XtWidgetGeometry   *intended, *return_val;
{
    PanedWidget         self = (PanedWidget) w;
    Dimension           width, height, off_size, size;
    XtGeometryResult    ret_val = XtGeometryYes;
    XtGeometryMask      mode = intended->request_mode;
    Widget             *childP;
    int                 num;

    return_val->request_mode = 0;

   /* Set children to there prefered sizes */
    off_size = SetChildrenSizes(self, 0 /*ChildSize(self, !IsVert(self)) */ );

    if (off_size == 0)
	off_size = ChildSize(self, !IsVert(self));

   /* Compute estimate of size of children. */
    size = 0;
    num = 0;
    ForAllChildren(self, childP) {
	if (!XtIsManaged(*childP) || IsGrip(*childP))
	    continue;
	size += ChildInfo(*childP)->size;
	num++;
    }

    size += (num - 1) * self->paned.spacing;

    if (IsVert(self)) {
	height = size;
	width = off_size;
    } else {
	height = off_size;
	width = size;
    }

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
	    ((mode & CWHeight) && (height == self->core.height))) {
	    ret_val = XtGeometryNo;
	}
    }
    return (ret_val);
}

/* The Geometry Manager only allows changes after Realize if
 * allow_resize is True in the constraints record.  
 * 
 * For vertically paned widgets:
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
    PanedWidget         self = (PanedWidget) XtParent(w);
    XtGeometryMask      mask = request->request_mode;
    PanedConstraints    child = ChildInfo(w);
    Dimension           old_size, old_wpsize, old_paned_size;
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
    old_paned_size = ChildSize((Widget) self, vert);
    old_wpsize = child->wp_size;
    old_size = child->size;

    StartUserAdjust(self);
    child->wp_size = child->new_size = GetRequestInfo(request, vert);

   /* Try relayout with requested size, allow parent to resize if needed */
    ResizePaned(self, ChildSize((Widget) self, !vert), &result,
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
	self->core.height = old_paned_size;
	reply->height = child->new_size;
	reply->width = off_size;
    } else {
	self->core.width = old_paned_size;
	reply->height = off_size;
	reply->width = child->new_size;
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
	child->new_size = old_size;
	PositionChildren(self);
	reply->request_mode = CWWidth | CWHeight;
	if (almost)
	    return XtGeometryAlmost;
    } else {

       /* If we matched, then actualy move the children */
	ResizePaned(self, ChildSize((Widget) self, !vert),
		    (XtGeometryResult *) NULL, &on_size, &off_size);
	PositionChildren(self);
	CommitNewLocations(self);
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
    PanedWidget         self = (PanedWidget) w;

   /* Set children to there prefered sizes */
    SetChildrenSizes(self, ChildSize(w, !IsVert(self)));
   /* Move children to there new places */
    StartUserAdjust(self);
    PositionChildren(self);
    CommitNewLocations(self);
    MoveChildren(self);
}

static void
InsertChild(w)
	Widget              w;
{
    PanedConstraints	child = ChildInfo(w);

   /* insert the child widget in the composite children list with the */
   /* superclass insert_child routine.                                */
    (*SuperClass->composite_class.insert_child) (w);

    child->size = 0;
    child->new_size = 0;
    child->grip = NULL;
    child->user_adjusted = FALSE;
}

static void
DeleteChild(w)
	Widget              w;
{
    PanedConstraints	child = ChildInfo(w);

   /* Remove the grip if we have one */
    if (child->grip)
	XtDestroyWidget(child->grip);
   /* delete the child widget in the composite children list with the */
   /* superclass delete_child routine.                                */
    (*SuperClass->composite_class.delete_child) (w);
}

static void
ChangeManaged(w)
	Widget              w;
{
    PanedWidget         self = (PanedWidget) w;
    Boolean             vert = IsVert(self);
    Dimension           size;
    Widget             *childP;
    int			i;

   /* Don't allow us to be called recursivly */
    if (self->paned.recursively_called++)
	return;

   /* If other dimension is zero, set to widest */
    if ((size = ChildSize(w, !vert)) == 0) {
	size = 1;
	ForAllChildren(self, childP) {
	    if (IsGrip(*childP))
		continue;
	    if (XtIsManaged(*childP) && (ChildSize(*childP, !vert) > size))
		size = ChildSize(*childP, !vert);
	}
    }
   /* Compute number of managed children */
    self->paned.num_children = 0;
    self->paned.num_resize = 0;
    ForAllChildren(self, childP) {
	PanedConstraints child = ChildInfo(*childP);

	if (IsGrip(*childP))
	    continue;
	if (XtIsManaged(*childP))
	    self->paned.num_children++;
	if (child->show_grip || (child->auto_grip && !child->resize_to_pref))
	    self->paned.num_resize++;
    }

   /* Set children to prefered sizes */
    SetChildrenSizes(self, size);
    StartUserAdjust(self);
   /* Attempt to resize parent to fit us */
    ResizePaned(self, size, (XtGeometryResult *) NULL, (Dimension *) NULL,
		(Dimension *) NULL);
    if (!XtIsRealized(w)) {
	self->paned.recursively_called = FALSE;
	return;
    }

   /* Update grips */
   /* Have to use array access since we are adding children and pointer
    * could move on us.
    */
    for ( i = 0; i < self->composite.num_children; i++ ) {
        childP = &self->composite.children[i]; 
	if (!IsGrip(*childP))
	    MakeGrip(ChildInfo(*childP), self);
    }

   /* If we made any grips make sure they are correctly managed. */
    ForAllChildren(self, childP) {
	PanedConstraints  child = ChildInfo(*childP);

	if (IsGrip(*childP))
	    continue;
	if (XtIsManaged(*childP)) {
	    if (child->grip && !XtIsManaged(child->grip))
		XtManageChild(child->grip);
	} else {
	    if (child->grip && XtIsManaged(child->grip))
		XtUnmanageChild(child->grip);
	}
    }

   /* If we are realized, move children */
    PositionChildren(self);
    CommitNewLocations(self);
    MoveChildren(self);

    self->paned.recursively_called = FALSE;
}

/*
 * Destroy widget.
 */
static void
Destroy(w)
	Widget              w;
{
    PanedWidget         self = (PanedWidget) w;

    XtReleaseGC(w, self->paned.grip_gc);
}

/************************************************************
 *
 * Private functions
 *
 ************************************************************/

/*
 * Set size of each child to it's prefered size.
 */
static int
SetChildrenSizes(self, off_size)
	PanedWidget         self;
	Dimension           off_size;
{
    Widget             *childP;
    Boolean             vert = IsVert(self);
    XtWidgetGeometry    request, reply;
    Dimension           n_off_size = off_size;

    ForAllChildren(self, childP) {
	PanedConstraints child = ChildInfo(*childP);

	if (!XtIsManaged(*childP) || IsGrip(*childP))
	    continue;

	if (vert) {
	    request.request_mode = CWWidth;
	    request.width = off_size;
	} else {
	    request.request_mode = CWHeight;
	    request.height = off_size;
	}
	if (off_size == 0)
	    request.request_mode = CWHeight | CWWidth;
	if (XtQueryGeometry(*childP, &request, &reply) == XtGeometryAlmost &&
	    (reply.request_mode = (vert ? CWHeight : CWWidth))) {
	    child->wp_size = GetRequestInfo(&reply, vert);
	    if (off_size == 0 && GetRequestInfo(&reply, !vert) < n_off_size)
		n_off_size = GetRequestInfo(&reply, !vert);
	} else
	    child->wp_size = ChildSize(*childP, vert);
	if (child->resize_to_pref)
	    child->size = child->wp_size;
	else
	    child->size = ChildSize(*childP, vert);

    }
    return n_off_size;

}

/*
 * Move children to their new home.
 */
static void
MoveChildren(self)
	PanedWidget         self;
{
    Widget             *childP;
    Boolean             vert = IsVert(self);
    Dimension           gx, gy, gh, gw;
    XWindowChanges      changes;
    Cursor              cursor;

    changes.stack_mode = Above;

    if (self->paned.grip_cursor != None)
	cursor = self->paned.grip_cursor;
    else
	cursor = (vert) ? self->paned.v_grip_cursor : self->paned.h_grip_cursor;

    ForAllChildren(self, childP) {
	PanedConstraints child = ChildInfo(*childP);

	if (!XtIsManaged(*childP) || IsGrip(*childP))
	    continue;

	if (vert) {
	    XtMoveWidget(*childP, (Position) 0, child->location);
	    XtResizeWidget(*childP, self->core.width,
			   (Dimension) child->size, 0);
	    if (child->grip) {
		changes.x = (self->core.width * self->paned.griploc) / 100 -
		    child->grip->core.width / 2;
		changes.y = (child->location + child->size) - child->grip->core.height / 2;
	    }
	} else {
	    XtMoveWidget(*childP, child->location, (Position) 0);
	    XtResizeWidget(*childP, (Dimension) child->size,
			   self->core.height, 0);
	    if (child->grip) {
		changes.x = (child->location + child->size) - child->grip->core.width / 2;
		changes.y = (self->core.height * self->paned.griploc) / 100 -
		    child->grip->core.height / 2;
	    }
	}
	if (child->grip) {
	    Display            *dpy = XtDisplayOfObject(child->grip);
	    Window              win = XtWindowOfObject(child->grip);

	    child->grip->core.x = changes.x;
	    child->grip->core.y = changes.y;
	    if (!XtIsRealized(child->grip))
		continue;
	    XConfigureWindow(dpy, win, CWX | CWY | CWStackMode, &changes);
	    if (cursor != None)
		XDefineCursor(dpy, win, cursor);
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
	PanedWidget         self;
{
    Boolean             vert = IsVert(self);
    int                 size, gap, loc;
    Widget             *childP;

    if (!XtIsRealized((Widget) self) ||
	self->paned.num_children == 0 || !self->paned.refiguremode)
	return;
   /* Compute estimate of size of children. */
    size = 0;
    ForAllChildren(self, childP) {
	if (!XtIsManaged(*childP) || IsGrip(*childP))
	    continue;
	size += ChildInfo(*childP)->new_size;
    }

   /* If they don't fit try readjusting */
    if (size != ChildSize((Widget) self, vert))
	ResizeChildren(self, (int *) &size);

    size += (self->paned.num_children - 1) * self->paned.spacing;

   /* Compute gap if needed */
    if (size < ChildSize((Widget) self, vert) &&
	self->paned.packing == XpwEven && self->paned.num_children > 2)
	gap = (ChildSize((Widget) self, vert) - size) /
	    (self->paned.num_children - 1);
    else if (size < ChildSize((Widget) self, vert) &&
	 self->paned.packing == XpwEven && self->paned.num_children == 2)
	gap = ChildSize((Widget) self, vert) - size;
    else
	gap = 0;

    gap += self->paned.spacing;
    loc = 0;
   /* Now set there location */
    ForAllChildren(self, childP) {
	if (!XtIsManaged(*childP) || IsGrip(*childP))
	    continue;
	ChildInfo(*childP)->new_location = loc;
	loc += ChildInfo(*childP)->new_size + gap;
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
	PanedWidget         self;
	int                *size;
{
    int                 win_size = ChildSize((Widget) self, IsVert(self));
    int                 resizable = 0;
    int                 amount;
    Boolean             changed;
    Widget             *childP;

   /* Compute number of children we can play with */
    ForAllChildren(self, childP) {
	PanedConstraints child = ChildInfo(*childP);

	if (IsGrip(*childP))
	    continue;

	if (!XtIsManaged(*childP) || child->skip_adjust || child->user_adjusted ||
	    (child->resize_to_pref && child->size == child->wp_size))
	    continue;
	resizable++;

    }

   /* If noone can change we are stuck */
    if (resizable == 0)
	return;

   /* Remove Gap size from computations */
    win_size -= (self->paned.num_children - 1) * self->paned.spacing;

   /* If we are not filling, then set all children left to their
    * prefered size.
    */
    if (self->paned.packing == XpwNoFill) {
	ForAllChildren(self, childP) {
	    PanedConstraints child = ChildInfo(*childP);
	    int                 nsize;

	    if (IsGrip(*childP))
		continue;
	    if (!XtIsManaged(*childP) || child->resize_to_pref
		|| child->skip_adjust || child->user_adjusted)
		continue;

	    nsize = child->wp_size;
	    if (nsize > child->max)
		nsize = child->max;
	    if (nsize < child->min)
		nsize = child->min;
	    *size += nsize - child->new_size;
	    child->new_size = nsize;
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
	    PanedConstraints child = ChildInfo(*childP);
	    int                 nsize;

	    if (IsGrip(*childP))
		continue;
	    if (!XtIsManaged(*childP) || child->resize_to_pref
		|| child->skip_adjust || child->user_adjusted)
		continue;

	    nsize = child->new_size + amount;
	    if (nsize > child->max)
		nsize = child->max;
	    if (nsize < child->min)
		nsize = child->min;
	    if (nsize != child->new_size) {
		changed = TRUE;
		*size += nsize - child->new_size;
		child->new_size = nsize;
	    }
	}
    }
}

/*
 * Resize Paned widget itself.
 */
static void
ResizePaned(self, off_size, result_ret, on_size_ret, off_size_ret)
	PanedWidget         self;
	Dimension           off_size;
	XtGeometryResult   *result_ret;
	Dimension          *on_size_ret, *off_size_ret;
{
    Dimension           old_size = ChildSize((Widget) self, IsVert(self));
    Dimension           new_size = 0;
    Widget             *childP;
    XtWidgetGeometry    request, reply;
    int                 resizable = 0;

    request.request_mode = CWWidth | CWHeight;

    ForAllChildren(self, childP) {
	PanedConstraints child = ChildInfo(*childP);

	if (!XtIsManaged(*childP) || IsGrip(*childP))
	    continue;
	new_size += child->new_size;
	if (child->skip_adjust || child->user_adjusted ||
	    (child->resize_to_pref && child->new_size == child->wp_size))
	    continue;
	resizable++;

    }

    new_size += (self->paned.num_children - 1) * self->paned.spacing;

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

static void
DrawOutlines(self)
	PanedWidget         self;
{
    Boolean             vert = IsVert(self);
    Widget             *childP;
    Display            *dpy = XtDisplayOfObject((Widget) self);
    Window              win = XtWindowOfObject((Widget) self);
    int                 c_loc = 0;
    GC                  gc = self->paned.grip_gc;

    ForAllChildren(self, childP) {
	PanedConstraints child = ChildInfo(*childP);

	if (!XtIsManaged(*childP) || IsGrip(*childP))
	    continue;

	if (child->new_location != c_loc) {
	    if (vert)
		XDrawLine(dpy, win, gc, 0, child->new_location,
			  self->core.width, child->new_location);
	    else
		XDrawLine(dpy, win, gc, child->new_location, 0,
			  child->new_location, self->core.height);
	}
	c_loc = child->new_location + child->new_size;
	if (vert)
	    XDrawLine(dpy, win, gc, 0, c_loc, self->core.width, c_loc);
	else
	    XDrawLine(dpy, win, gc, c_loc, 0, c_loc, self->core.height);

    }
}

static void
StartUserAdjust(self)
	PanedWidget         self;
{
    Widget             *childP;

   /* Now set there location */
    ForAllChildren(self, childP) {
	PanedConstraints child = ChildInfo(*childP);

	if (!XtIsManaged(*childP) || IsGrip(*childP))
	    continue;
	child->new_location = child->location;
	child->new_size = child->size;
    }
}

static void
CommitNewLocations(self)
	PanedWidget         self;
{
    Widget             *childP;

   /* Now set there location */
    ForAllChildren(self, childP) {
	PanedConstraints child = ChildInfo(*childP);

	if (!XtIsManaged(*childP) || IsGrip(*childP))
	    continue;
	child->location = child->new_location;
	child->size = child->new_size;
    }
}

static void
MakeGrip(child, self)
	PanedConstraints child;
	PanedWidget         self;
{
    Arg                 arg[1];

    XtSetArg(arg[0], XtNtranslations, self->paned.grip_translations);

    if (child->show_grip || (child->auto_grip && !child->resize_to_pref
			     && self->paned.num_resize > 1)) {
	if (!child->grip) {
	    child->grip = XtCreateWidget("grip", gripWidgetClass,
					 (Widget) self, arg, 1);
	    XtAddCallback(child->grip, XtNcallback, HandleGrip,
			  (XtPointer) child);
	    XtRealizeWidget(child->grip);
	}
    } else {
	if (child->grip) {
	    XtDestroyWidget(child->grip);
	    child->grip = NULL;
	}
    }
}

static void
HandleGrip(wid, client_data, call_data)
	Widget              wid;
	XtPointer           client_data;
	XtPointer           call_data;
{
    PanedWidget         self = (PanedWidget) XtParent(wid);
    PanedConstraints    child = (PanedConstraints) client_data;
    XpwGripCallDataRec *grip_info = (XpwGripCallDataRec *) call_data;
    Cursor              cursor;
    Arg                 arg[1];
    int                 offset;

    if (grip_info->num_params == 0 || grip_info->num_params > 2)
	goto error;

   /* Figure position of mouse event */
    if (grip_info->event->type == MotionNotify)
	offset = IsVert(self) ?
	    grip_info->event->xmotion.y : grip_info->event->xmotion.x;
    offset = IsVert(self) ?
	grip_info->event->xbutton.y : grip_info->event->xbutton.x;

    switch (grip_info->params[0][0]) {
    case 'S':
	if ((grip_info->num_params != 2))
	    goto error;
	if (self->paned.adjust_this_cursor)
	    cursor = self->paned.adjust_this_cursor;
	else
	    cursor = IsVert(self) ? self->paned.v_adjust_this_cursor : self->paned.h_adjust_this_cursor;
	if (cursor != None)
	    XDefineCursor(XtDisplay(wid), XtWindow(wid), cursor);

	StartUserAdjust(self);
	child->user_adjusted = TRUE;
	self->paned.mouse_base = offset;
       /* Draw outlines */
	DrawOutlines(self);
	break;
    case 'M':
	if ((grip_info->num_params != 2))
	    goto error;
       /* Clear old lines */
	DrawOutlines(self);
       /* Update size */
	child->new_size = child->size + (offset - self->paned.mouse_base);
       /* Range check size */
	if (child->new_size < child->min)
	    child->new_size = child->min;
	if (child->new_size > child->max)
	    child->new_size = child->max;
       /* If moving all, readjust them */
	if (grip_info->params[1][0] == 'T')
	    PositionChildren(self);
       /* Draw new lines */
	DrawOutlines(self);
	break;
    case 'C':
	if ((grip_info->num_params != 1))
	    goto error;
       /* Clear old lines */
	DrawOutlines(self);
       /* Do a final position on children */
	PositionChildren(self);
       /* Check if anything was done */
	if (child->location == child->new_location &&
	    child->size == child->new_size)
	    child->user_adjusted = FALSE;
	CommitNewLocations(self);
	MoveChildren(self);
	break;
    }
    return;
  error:
    XtWarning("Invalid parameters passed to GripAction action table.");
    return;

}

/*
 * Draw Track lines (animate pane borders) in foreground color ^ bg color.
 */

static void
CreateGC(self)
	PanedWidget         self;
{
    XtGCMask            mask;
    XGCValues           values;

    values.function = GXinvert;
    values.plane_mask = self->paned.foreground ^ self->core.background_pixel;
    values.subwindow_mode = IncludeInferiors;
    mask = GCPlaneMask | GCFunction | GCSubwindowMode;
    self->paned.grip_gc = XtGetGC((Widget) self, mask, &values);
}

/************************************************************
 *
 * Public routines. 
 *
 ************************************************************/

/* 
 * Allows a flag to be set the will inhibit the paned widgets relayout routine.
 */

void
XpwPanedSetRefigureMode(w, mode)
	Widget              w;
	Boolean             mode;

{
    ((PanedWidget) w)->paned.refiguremode = mode;
}

/*
 * Returns the number of children in the paned widget.
 */

int
XpwPanedGetNumSub(w)
	Widget              w;

{
    return ((PanedWidget) w)->paned.num_children;
}

/*
 * Allows a flag to be set that determines if the paned widget will allow
 * geometry requests from this child
 */

void
XpwPanedAllowResize(widget, allow_resize)
	Widget              widget;
	Boolean             allow_resize;

{
    ChildInfo(widget)->allow_resize = allow_resize;
}
