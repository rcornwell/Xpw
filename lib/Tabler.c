/*
 * Tabler.c - Tabler Composite Widget.
 *
 * Tabler arranges it's children in a table.
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
 * $Log: Tabler.c,v $
 * Revision 1.1  1997/11/28 19:57:31  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: Tabler.c,v 1.1 1997/11/28 19:57:31 rich Exp rich $";

#endif

#include <X11/IntrinsicP.h>
#include <X11/cursorfont.h>
#include <X11/StringDefs.h>

#include <X11/Xmu/Misc.h>
#include <X11/Xmu/Converters.h>

#include "XpwInit.h"
#include <stdio.h>
#include <stdlib.h>
#include "TablerP.h"

#define DEFAULT_COLS	4

#include <ctype.h>

#define ChildInfo(w)	((TablerConstraints)(w)->core.constraints)
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

#define offset(field) XtOffsetOf(TablerRec, tabler.field)

static XtResource   resources[] =
{
    {XtNrefigureMode, XtCBoolean, XtRBoolean, sizeof(Boolean),
     offset(refiguremode), XtRImmediate, (XtPointer) TRUE},
    {XtNrowSpacing, XtCSpacing, XtRDimension, sizeof(Dimension),
     offset(row_spacing), XtRImmediate, (XtPointer) 2},
    {XtNcolSpacing, XtCSpacing, XtRDimension, sizeof(Dimension),
     offset(col_spacing), XtRImmediate, (XtPointer) 2},
    {XtNsideSpacing, XtCSpacing, XtRDimension, sizeof(Dimension),
     offset(side_spacing), XtRImmediate, (XtPointer) 2},
    {XtNrows, XtCRows, XtRInt, sizeof(int),
     offset(nrows), XtRImmediate, (XtPointer) 0},
    {XtNcols, XtCCols, XtRInt, sizeof(int),
     offset(ncols), XtRImmediate, (XtPointer) 0},
};

#undef offset

#define offset(field) XtOffsetOf(TablerConstraintsRec, tabler.field)

static XtResource   subresources[] =
{
    {XtNresizeToPreferred, XtCBoolean, XtRBoolean, sizeof(Boolean),
     offset(resize_to_pref), XtRImmediate, (XtPointer) FALSE},
    {XtNallowResize, XtCBoolean, XtRBoolean, sizeof(Boolean),
     offset(allow_resize), XtRImmediate, (XtPointer) TRUE},
    {XtNrow, XtCRows, XtRInt, sizeof(int),
     offset(row), XtRImmediate, (XtPointer) - 1},
    {XtNcol, XtCCols, XtRInt, sizeof(int),
     offset(col), XtRImmediate, (XtPointer) - 1},
    {XtNrowSpan, XtCSpan, XtRInt, sizeof(int),
     offset(rspan), XtRImmediate, (XtPointer) 0},
    {XtNcolSpan, XtCSpan, XtRInt, sizeof(int),
     offset(cspan), XtRImmediate, (XtPointer) 0},
    {XtNrowOmit, XtCOmit, XtRInt, sizeof(int),
     offset(romit), XtRImmediate, (XtPointer) 0},
    {XtNcolOmit, XtCOmit, XtRInt, sizeof(int),
     offset(comit), XtRImmediate, (XtPointer) 0},
    {XtNposition, XtCPosition, XtRInt, sizeof(int),
     offset(position), XtRImmediate, (XtPointer) 0},
    {XtNnewRow, XtCNewRow, XtRBoolean, sizeof(Boolean),
     offset(newrow), XtRImmediate, (XtPointer) FALSE},
    {XtNnoBorder, XtCNoBorder, XtRBoolean, sizeof(Boolean),
     offset(noborder), XtRImmediate, (XtPointer) FALSE},
};

#undef offset

/* Semi-public interface functions */
static void         Initialize(Widget /*request */ , Widget /*new */ ,
			  ArgList /*args */ , Cardinal * /*num_args */ );
static Boolean      SetValues(Widget /*old */ , Widget	/*request */
			      ,Widget /*new */ , ArgList /*args */ ,
			      Cardinal * /*num_args */ );
static void         Realize(Widget /*w */ , Mask * /*valueMask */ ,
			    XSetWindowAttributes * /*attributes */ );
static XtGeometryResult QueryGeometry(Widget /*w */ ,
				      XtWidgetGeometry * /*request */ ,
				      XtWidgetGeometry * /*reply */ );
static XtGeometryResult GeometryManager(Widget /*w */ ,
					XtWidgetGeometry * /*request */ ,
					XtWidgetGeometry * /*reply */ );
static void         Resize(Widget /*w */ );
static void         InsertChild(Widget /*w */ );
static void         ChangeManaged(Widget /*w */ );

/* private functions */
static void         SetChildrenSizes(TablerWidget /*self */ );
static void         MoveChildren(TablerWidget /*self */ );
static int          cmp(const void * /*a */ , const void * /*b */ );
static void         Layout(TablerWidget /*self */ , Dimension * /*width */ ,
				 Dimension * /*height */ , int /*set */ );
static Boolean      FindSpace(int * /*array */ , int /*nr */ , int /*nc */ ,
				 int */*wr */ , int */*wc */ , int /*er */ ,
				 int /*ec */ , Boolean /* newrow */);
static Boolean      CheckSpace(int * /*array */ , int /*nr */ , int /*nc */ ,
				 int /*sr */ , int /*sc */ ,
				 int /*rsz */ , int /*csz */ );
static void         ClearSpace(int * /*array */ , int /*nr */ , int /*nc */ ,
				 int /*sr */ , int /*sc */ ,
				 int /*rsz */ , int /*csz */ );


#define SuperClass ((ConstraintWidgetClass)&constraintClassRec)

TablerClassRec      tablerClassRec =
{
    {	/* core class fields */
	(WidgetClass) SuperClass,	/* superclass          */
	"Tabler",			/* class name          */
	sizeof(TablerRec),		/* size                */
	XpwInitializeWidgetSet,		/* class_initialize    */
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
	QueryGeometry,			/* query_geometry      */
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
	sizeof(TablerConstraintsRec),	/* constraint_size     */
	NULL,				/* initialize          */
	NULL,				/* destroy             */
	NULL,				/* set_values          */
	NULL				/* extension           */
    },
    {   /* Tabler Class fields. */
        0
    }
};

WidgetClass         tablerWidgetClass = (WidgetClass) & tablerClassRec;

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
    TablerWidget        self = (TablerWidget) new;

    self->tabler.recursively_called = False;
    self->tabler.num_children = 0;
    self->tabler.next_child = 0;
    if (self->core.width == 0)
	self->core.width = 1;
    if (self->core.height == 0)
	self->core.height = 1;
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
    TablerWidget        old_self = (TablerWidget) old;
    TablerWidget        new_self = (TablerWidget) new;

   /* If anything changed, relayout */
    if ((old_self->tabler.nrows != new_self->tabler.nrows) ||
	(old_self->tabler.ncols != new_self->tabler.ncols) ||
	(old_self->tabler.row_spacing != new_self->tabler.row_spacing) ||
	(old_self->tabler.col_spacing != new_self->tabler.col_spacing) ||
      (old_self->tabler.side_spacing != new_self->tabler.side_spacing)) {
       /* Relayout children */
	ChangeManaged(new);
    }
    return FALSE;
}

static void
Realize(w, valueMask, attributes)
	Widget              w;
	Mask               *valueMask;
	XSetWindowAttributes *attributes;
{
    TablerWidget        self = (TablerWidget) w;
    Widget             *childP;

    (*SuperClass->core_class.realize) (w, valueMask, attributes);

    ForAllChildren(self, childP)
	XtRealizeWidget(*childP);

   /* Set children to there prefered sizes */
    SetChildrenSizes(self);
   /* Relayout and move children */
    Layout(self, &self->core.width, &self->core.height, TRUE);
    MoveChildren(self);
}

/*
 * Calculate preferred size, given constraining box, caching it in the widget.
 */

static              XtGeometryResult
QueryGeometry(w, intended, return_val)
	Widget              w;
	XtWidgetGeometry   *intended, *return_val;
{
    TablerWidget        self = (TablerWidget) w;
    Dimension           width, height;
    XtGeometryResult    ret_val = XtGeometryYes;
    XtGeometryMask      mode = intended->request_mode;

    return_val->request_mode = 0;

   /* Compute required size */
    SetChildrenSizes(self);
    Layout(self, &width, &height, FALSE);

    if (width == 0)
	width = 1;
    if (height == 0)
	height = 1;

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
	    ret_val = XtGeometryNo;
    }
    return (ret_val);
}

/* The Geometry Manager only allows changes after Realize if
 * allow_resize is True in the constraints record.  
 * 
 * For vertically tabler widgets:
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
    TablerWidget        self = (TablerWidget) XtParent(w);
    XtGeometryMask      mask = request->request_mode;
    TablerConstraints   child = ChildInfo(w);
    Dimension           old_width, old_height;
    Dimension           nwidth, nheight;
    XtGeometryResult    result;
    XtWidgetGeometry    prequest, preply;
    Boolean             almost = FALSE;

   /* If widget not realized don't allow for change.  */
    if (!XtIsRealized(w) || !child->allow_resize)
	return XtGeometryNo;

   /* Save old infomation. */
    old_width = child->pref_width;
    old_height = child->pref_height;
    if (mask & CWWidth)
	child->pref_width = request->width;
    if (mask & CWHeight)
	child->pref_height = request->height;

    nwidth = 0;
    nheight = 0;
    Layout(self, &nwidth, &nheight, FALSE);

   /* If we were able to do it in same size, just do it */
    if (nwidth == self->core.width && nheight == self->core.height) {
	if (mask & XtCWQueryOnly) {
	    child->pref_width = old_width;
	    child->pref_height = old_height;
	    return XtGeometryYes;
	}
	Layout(self, &self->core.width, &self->core.height, TRUE);
	MoveChildren(self);
	return XtGeometryDone;
    }
   /* Don't all change if would require resize */
    if (!self->tabler.refiguremode) {
	child->pref_width = old_width;
	child->pref_height = old_height;
	return XtGeometryNo;
    }
   /* If size difference try and preform a resize */
    prequest.request_mode = CWWidth | CWHeight | XtCWQueryOnly;
    prequest.width = nwidth;
    prequest.height = nheight;
    result = XtMakeGeometryRequest((Widget) self, &prequest, &preply);

   /* If we did not match requested or this was just a Query */
    almost = prequest.width > preply.width;
    almost |= prequest.height > preply.height;

    if ((mask & XtCWQueryOnly) || almost) {
       /* Return Almost and restore old locations */
	reply->width = child->width;
	reply->height = child->height;
	reply->request_mode = CWWidth | CWHeight;
	child->width = old_width;
	child->height = old_height;
	if (almost)
	    return XtGeometryAlmost;
	return XtGeometryYes;
    }
   /* If we matched, then actualy move the children */
    prequest.request_mode = CWWidth | CWHeight;
    (void) XtMakeGeometryRequest((Widget) self, &prequest, &preply);
    Layout(self, &self->core.width, &self->core.height, TRUE);
    MoveChildren(self);
    return XtGeometryDone;
}

/*
 * Resize widget
 */

static void
Resize(w)
	Widget              w;
{
    TablerWidget        self = (TablerWidget) w;

   /* Move children to there new places */
    Layout(self, &self->core.width, &self->core.height, TRUE);
    MoveChildren(self);
}

static void
InsertChild(w)
	Widget              w;
{
    TablerConstraints   child = ChildInfo(w);
    TablerWidget        self = (TablerWidget) XtParent(w);

   /* insert the child widget in the composite children list with the */
   /* superclass insert_child routine.                                */
    (*SuperClass->composite_class.insert_child) (w);

    self->tabler.next_child++;

    if (child->position == 0)
	child->position = self->tabler.next_child;

    child->x_loc = 0;
    child->y_loc = 0;
    child->width = w->core.width;
    child->height = w->core.height;
    child->pref_width = 0;
    child->pref_height = 0;
}

static void
ChangeManaged(w)
	Widget              w;
{
    TablerWidget        self = (TablerWidget) w;
    Dimension           width, height;
    XtWidgetGeometry    prequest, preply;
    Widget             *childP;

   /* Don't allow us to be called recursivly */
    if (self->tabler.recursively_called++)
	return;

   /* Compute number of managed children */
    self->tabler.num_children = 0;
    ForAllChildren(self, childP)
	if (XtIsManaged(*childP))
	      self->tabler.num_children++;

   /* Set children to prefered sizes */
    SetChildrenSizes(self);
    width = height = 0;
    Layout(self, &width, &height, FALSE);
   /* If we were able to do it in same size, just do it */
    if (width != self->core.width || height != self->core.height) {

       /* If size difference try and preform a resize */
	prequest.request_mode = CWWidth | CWHeight;
	prequest.width = width;
	prequest.height = height;
	if (XtMakeGeometryRequest((Widget) self, &prequest, &preply)
	    == XtGeometryAlmost) {
	    if (preply.request_mode & CWWidth)
		width = preply.width;
	    if (preply.request_mode & CWHeight)
		height = preply.height;
	    (void) XtMakeGeometryRequest((Widget) self, &preply, &preply);
	}
    }
   /* If we are realized, move children */
    if (XtIsRealized(w)) {
	Layout(self, &self->core.width, &self->core.height, TRUE);
	MoveChildren(self);
    }
    self->tabler.recursively_called = FALSE;
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
SetChildrenSizes(self)
	TablerWidget        self;
{
    Widget             *childP;
    XtWidgetGeometry    request, reply;

    ForAllChildren(self, childP) {
	TablerConstraints   child = ChildInfo(*childP);

	if (!XtIsManaged(*childP))
	    continue;

	child = ChildInfo(*childP);
	request.request_mode = CWWidth | CWHeight;
	request.width = (*childP)->core.width;
	request.height = (*childP)->core.height;
	if (XtQueryGeometry(*childP, &request, &reply) == XtGeometryAlmost) {
	    child->pref_width = (reply.request_mode & CWWidth) ?
		reply.width : (*childP)->core.width;
	    child->pref_height = (reply.request_mode & CWHeight) ?
		reply.height : (*childP)->core.height;

	} else {
	    child->pref_width = (*childP)->core.width;
	    child->pref_height = (*childP)->core.height;
	}
    }
}

/*
 * Move children to their new home.
 */
static void
MoveChildren(self)
	TablerWidget        self;
{
    Widget             *childP;

    ForAllChildren(self, childP) {
	TablerConstraints   child = ChildInfo(*childP);

	if (XtIsManaged(*childP))
	    XtConfigureWidget(*childP, child->x_loc, child->y_loc,
	      child->width, child->height, (*childP)->core.border_width);
    }
}

/*
 * Compare to children.
 */
static int
cmp(a, b)
	const void         *a;
	const void         *b;
{
    TablerConstraints   ca = *(TablerConstraints *) a;
    TablerConstraints   cb = *(TablerConstraints *) b;

    if (ca->row < 0 && cb->row >= 0)
	return 1;
    if (ca->row >= 0 && cb->row < 0)
	return -1;
    if (ca->row != cb->row)
	return (ca->row - cb->row);
    if (ca->col < 0 && cb->col >= 0)
	return 1;
    if (ca->col >= 0 && cb->col < 0)
	return -1;
    if (ca->col != cb->col)
	return (cb->col - cb->col);
    return (cb->position - cb->position);
}

/*
 * Compute locations of children.
 */
static void
Layout(self, width, height, set)
	TablerWidget        self;
	Dimension          *width;
	Dimension          *height;
	Boolean             set;
{
    Widget             *childP;
    int                 nrows = 0;	/* Number of rows. */
    int                 er = 0, ec;	/* Extra rows */
    int                 ncols = 0;	/* NUmber of cols */
    int                 tot = 0;	/* Number of children */
    Dimension          *rheight, wh;	/* Height of each row. */
    Dimension          *cwidth, ww;	/* Width of each column. */
    int                 wr, wc;
    TablerConstraints   child, *carray, *cp;
    int                *fillmat;
    int                 children;
    int                 i, j, x;

    if (!self->tabler.refiguremode)
	return;

    carray = (TablerConstraints *) XtMalloc(sizeof(TablerConstraints) *
					    (self->tabler.next_child + 1));
    cp = carray;
   /* Take into account spans and omits */
    ForAllChildren(self, childP) {

	if (!XtIsManaged(*childP))
	    continue;

	child = ChildInfo(*childP);
	*cp++ = child;
	tot++;

	if (child->row < 0 && child->newrow)
	    child->col = 0;
	if (child->row >= 0 || child->rspan != 0 || child->romit != 0) {
	    int                 nr;

	    nr = child->row + child->rspan + child->romit;
	    if (nr > nrows)
		nrows = nr;
	}
	if (child->col >= 0 || child->cspan != 0 || child->comit != 0) {
	    int                 nc;

	    nc = ((child->newrow) ? 1 : child->col) + child->cspan +
							 child->comit;
	    if (nc > ncols)
		ncols = nc;
	}
	if (child->row < 0 && child->newrow)
	    er++;
	/* Compute total space taken by child */
	x = (child->cspan + child->comit);
	tot += (child->rspan + child->romit) * ((x == 0) ? 1 : x);
	if (set) {
	   child->crow = -1;
	   child->ccol = -1;
	}
    }

   /* Sort children by row/column/position */
    children = cp - carray;
    qsort((void *) carray, children, sizeof(TablerConstraints *), cmp);

   /* Find max row & column, and number of children */
   /* Compute number of rows and columns needed. */
   /* Favor square layouts if no sizes given */
    nrows += er;
   /* Fill from default */
    if (nrows == 0 || nrows < self->tabler.nrows)
	nrows = self->tabler.nrows;
    if (ncols == 0 || ncols < self->tabler.ncols)
	ncols = self->tabler.ncols;
    if (nrows == 0 && ncols == 0)
	ncols = DEFAULT_COLS;
    if (nrows == 0)
	nrows = tot / ncols;
    if (ncols == 0)
	ncols = tot / nrows;
    while ((nrows * ncols) < tot)
	nrows++;

   /* Compute width and height needed */
    rheight = (Dimension *) XtMalloc((nrows + 1) * sizeof(Dimension));
    cwidth = (Dimension *) XtMalloc((ncols + 1) * sizeof(Dimension));
    fillmat = (int *) XtMalloc(((nrows + 1) * (ncols + 1)) * sizeof(int));
    for (i = 0; i < nrows; rheight[i++] = 0)
	for (j = 0; j < ncols; j++)
	    fillmat[(i * ncols) + j] = ncols - j;

    for (i = 0; i < ncols; cwidth[i++] = 0) ;

   /* Place sorted children */
    for (cp = carray, x = 0; x < children; x++, cp++) {
	Boolean             newrow;

	child = *cp;
	wr = wc = -1;
	if (child->row >= 0)
	    wr = child->row;
	if (child->col >= 0)
	    wc = child->col;
	if (wr == -1 && wc == -1) {
	    wr = (child->position - 1) / nrows;
	    wc = (child->position - 1) % ncols;
	}
       /* Add widths to each row & col widget spans */
	er = ((child->rspan == 0) ? 1 : child->rspan) + child->romit;
	ec = ((child->cspan == 0) ? 1 : child->cspan) + child->comit;
       /* Make sure rows and cols in range */
	wc = ((wc < 0) ? 0 : (((wc + ec) > ncols) ? (ncols - ec) : wc));
	wr = ((wr < 0) ? 0 : (((wr + er) > nrows) ? (nrows - er) : wr));
       /* Find a spot to fit this child */
	newrow = child->newrow;
	child->crow = -1;
	for (;;) {
	   /* Search for place to put child */
	    i = wr; j = wc;
	   /* Try and place child */ 
	    if (FindSpace(fillmat, nrows, ncols, &i, &j, er, ec, newrow)) {
		child->crow = i;
		child->ccol = j;
		ClearSpace(fillmat, nrows, ncols, i, j, er, ec);
		break;
	    }
	    if (newrow && wc != 0) {
		wc = 0;
		wr++;
		continue;
	    }
	   /* Could not place it... try with column of zero */
	    if (wc != 0) {
		wc = 0;
		continue;
	    }
	   /* Could not place it... try with row of zero */
	    if (wr != 0) {
		wr = 0;
		continue;
	    }
	    if (newrow) {
		newrow = FALSE;
		continue;
	    }
	    if (er != 1 || ec != 1) {
		er = 1;
		ec = 1;
		continue;
	    }
	    child->crow = 0;
	    child->ccol = 0;
	    break;
	}
	wr = child->crow;
	wc = child->ccol;
	er = (child->rspan == 0) ? 1 : child->rspan;
	ec = (child->cspan == 0) ? 1 : child->cspan;
	for (i = 0; i < er; i++) {
	    if (rheight[wr + i] < (child->pref_height / er))
		rheight[wr + i] = child->pref_height / er;
	}
	for (i = 0; i < ec; i++) {
	    if (cwidth[wc + i] < (child->pref_width / ec))
		cwidth[wc + i] = child->pref_width / ec;
	}
    }

    XtFree((XtPointer) fillmat);	/* Done with it. */
    XtFree((XtPointer) carray);
   /* If seting Compute positions and sizes of children */
    for (i = 0, wh = 0; i < nrows; wh += rheight[i++]);
    wh += ((nrows - 1) * self->tabler.row_spacing) +
	(2 * self->tabler.side_spacing);
    for (i = 0, ww = 0; i < ncols; ww += cwidth[i++]);
    ww += ((ncols - 1) * self->tabler.col_spacing) +
	(2 * self->tabler.side_spacing);

   /* If we are not setting, there  is nothing more to do. */
    if (!set) {
	XtFree((XtPointer) rheight);
	XtFree((XtPointer) cwidth);
	*width = ww;
	*height = wh;
	return;
    }
   /* If the sizes are not equal distribute the diffrences evenly over
    * rows and columns.
    */

    if (*width != 0 && *width != ww) {
	int                 adjust = (*width - ww) / ncols;

	for (i = 0; i < ncols; i++)
	    if ((cwidth[i] += adjust) < 1)
		cwidth[i] = 1;
    }
    if (*height != 0 && *height != wh) {
	int                 adjust = (*height - wh) / nrows;

	for (i = 0; i < nrows; i++)
	    if ((rheight[i] += adjust) < 1)
		rheight[i] = 1;
    }
   /* Now position children and set their sizes */
    ForAllChildren(self, childP) {
	int                 x, y;
	Dimension           w, h;
	int		    rs = self->tabler.row_spacing;
	int		    cs = self->tabler.col_spacing;

	if (!XtIsManaged(*childP))
	    continue;

	child = ChildInfo(*childP);
	er = (child->rspan == 0) ? 1 : child->rspan;
	ec = (child->cspan == 0) ? 1 : child->cspan;
	for (y = i = 0; i < child->crow; y += rheight[i++]) ;
	for (h = 0; i < (child->crow + er); h += rheight[i++]) ;
	for (x = i = 0; i < child->ccol; x += cwidth[i++]) ;
	for (w = 0; i < (child->ccol + ec); w += cwidth[i++]) ;

	x += cs * child->ccol;
	y += rs * child->crow;
	w += cs * (ec - 1);
	h += rs * (er - 1);
	if (child->noborder) {
	    y -= cs / 2;
	    w += cs;
	    x -= rs / 2;
	    h += rs;
	}
	child->x_loc = x + self->tabler.side_spacing;
	child->y_loc = y + self->tabler.side_spacing;
	child->width = (child->resize_to_pref) ? child->pref_width : w;
	child->height = (child->resize_to_pref) ? child->pref_height : h;
    }

    XtFree((XtPointer) rheight);
    XtFree((XtPointer) cwidth);
}

/*
 * Search forward to find a spot.
 */
static	Boolean
FindSpace(array, nr, nc, wr, wc, er, ec, newrow)
	int		*array;
	int		nr, nc;
	int		*wr, *wc;
	int		er, ec;
	Boolean		newrow;
{
	int	i,j;
	Boolean	ok = FALSE;

    for (i = *wr; i <= (nr-er); i++) {
	for (j = *wc; j <= (nc-ec); j++) {
	    if ((ok = CheckSpace(array, nr, nc, i, j, er, ec)) || newrow)
		break;
	}
	if (ok) {
		*wr = i;
		*wc = j;
		return TRUE;
	}
	*wc = 0;
     }
     return FALSE;
}

/*
 * See if space in area for child.
 */
static              Boolean
CheckSpace(array, nr, nc, sr, sc, rsz, csz)
	int                *array;
	int                 nr, nc;
	int                 sr, sc;
	int                 rsz, csz;
{
    int                 i, x;

    if ((sr + rsz) > nr || (sc + csz) > nc) 
	return FALSE;

    for (i = 0; (i + sr) < nr && i < rsz; i++)
	if ((x = array[((sr + i) * nc) + sc]) < csz || x < 0) 
	    return FALSE;
    return TRUE;
}

/*
 * Clear out area where child will be.
 */
static void
ClearSpace(array, nr, nc, sr, sc, rsz, csz)
	int                *array;
	int                 nr, nc;
	int                 sr, sc;
	int                 rsz, csz;
{
    int                 i, j;

    for (i = 0; (i + sr) < nr && i < rsz; i++) {
	int rbase = (i + sr) * nc + (sc+csz)-1;
	for (j = (sc+csz)-1; j >= 0; j--, rbase--) {
	  /* Anything after sc we are using */
	    if(j >= sc)
	        array[rbase] = -1;
	    else {
	  /* We need to stop updating when we find a used location */
	         if (array[rbase] < 0)
			break;
	         array[rbase] = sc - j;
	    }
		  
	}
    }
}

/************************************************************
 *
 * Public routines. 
 *
 ************************************************************/

/* 
 * Allows a flag to be set the will inhibit the tabler widgets relayout routine.
 */

void
XpwTablerSetRefigureMode(w, mode)
	Widget              w;
	Boolean             mode;

{
    ((TablerWidget) w)->tabler.refiguremode = mode;
}

/*
 * Returns the number of children in the tabler widget.
 */

int
XpwTablerGetNumSub(w)
	Widget              w;

{
    return ((TablerWidget) w)->tabler.num_children;
}

/*
 * Allows a flag to be set that determines if the tabler widget will allow
 * geometry requests from this child
 */

void
XpwTablerAllowResize(widget, allow_resize)
	Widget              widget;
	Boolean             allow_resize;

{
    ChildInfo(widget)->allow_resize = allow_resize;
}
