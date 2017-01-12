/*
 * ViewPort widget.
 *
 * Provide scrollbar support for child.
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
 * $Log: ViewPort.c,v $
 * Revision 1.2  1997/11/01 06:39:09  rich
 * Removed unused variables.
 *
 * Revision 1.1  1997/10/19 01:42:08  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: ViewPort.c,v 1.2 1997/11/01 06:39:09 rich Beta $";

#endif

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Misc.h>
#include <X11/Shell.h>
#include <X11/CoreP.h>
#include "XpwInit.h"
#include "ViewPortP.h"
#include "Scroller.h"

/* Our resources */
#define offset(field) XtOffsetOf(ViewPortRec, viewport.field)
static XtResource   resources[] =
{
    {XtNscrollOnTop, XtCScrollOnTop, XtRBoolean, sizeof(Boolean),
     offset(v_scroll_ontop), XtRImmediate, (XtPointer) FALSE},
    {XtNscrollOnLeft, XtCScrollOnLeft, XtRBoolean, sizeof(Boolean),
     offset(h_scroll_onleft), XtRImmediate, (XtPointer) FALSE},
    {XtNvScroll, XtCVScroll, XtRBoolean, sizeof(Boolean),
     offset(use_v_scroll), XtRImmediate, (XtPointer) FALSE},
    {XtNhScroll, XtCHScroll, XtRBoolean, sizeof(Boolean),
     offset(use_h_scroll), XtRImmediate, (XtPointer) FALSE},
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
     offset(foreground), XtRString, XtDefaultForeground},
    {XtNtopColor, XtCTopColor, XtRPixel, sizeof(Pixel),
     offset(topcolor), XtRString, XtDefaultBackground},
    {XtNscrollBorder, XtCScrollBorder, XtRDimension, sizeof(Dimension),
     offset(scrollborder), XtRImmediate, (XtPointer) 4},

    /* ThreeD resouces */
    threeDresources

};

#undef offset

/* Semi Public Functions */
static void         Initialize(Widget /*request */ , Widget /*new */ ,
			  ArgList /*args */ , Cardinal * /*num_args */ );
static Boolean      SetValues(Widget /*current */ , Widget /*request */ ,
			      Widget /*new */ , ArgList /*args */ ,
			      Cardinal * /*num_args */ );
static void         Realize(Widget /*w */ , Mask * /*valueMask */ ,
			    XSetWindowAttributes * /*attributes */ );
static XtGeometryResult GeometryManager(Widget /*wid */ ,
					XtWidgetGeometry * /*request */ ,
					XtWidgetGeometry * /*reply */ );
static void         InsertChild(Widget /*w */ );
static void         DeleteChild(Widget /*w */ );
static XtGeometryResult QueryGeometry(Widget /*w */ ,
				      XtWidgetGeometry * /*intended */ ,
				    XtWidgetGeometry * /*return_val */ );
static void         Resize(Widget /*w */ );
static void         Redisplay(Widget /*wid */ , XEvent * /*event */ ,
			      Region /*region */ );
static void         Destroy(Widget /*w */ );

/* Private Functions */

static void         Layout(ViewPortWidget /*self */ , Dimension * /*width */ ,
			   Dimension * /*height */ , int /*set */ );
static void         HandleScroll(Widget /*wid */ , XtPointer /*client_data */ ,
				 XtPointer /*call_data */ );
static void         MoveChild(ViewPortWidget /*self */ , Position /*nxoff */ ,
			      Position /*nyoff */ );
static void         CreateGCs(Widget /*w */ );
static void         DestroyGCs(Widget /*w */ );

#define SuperClass ((CompositeWidgetClass) &compositeClassRec)

ViewPortClassRec    viewportClassRec =
{
    {	/* core_class part */
	(WidgetClass) SuperClass,	/* superclass            */
	"ViewPort",			/* class_name            */
	sizeof(ViewPortRec),		/* widget_size           */
	XpwInitializeWidgetSet,		/* class_initialize      */
	NULL,				/* class_part_initialize */
	FALSE,				/* class_inited          */
	Initialize,			/* initialize            */
	NULL,				/* initialize_hook       */
	Realize,			/* realize               */
	NULL,				/* actions               */
	0,				/* num_actions           */
	resources,			/* resources             */
	XtNumber(resources),		/* num_resources         */
	NULLQUARK,			/* xrm_class             */
	TRUE,				/* compres_motion        */
	TRUE,				/* compress_exposure     */
	TRUE,				/* compress_enterleave   */
	TRUE,				/* visible_interest      */
	Destroy,			/* destroy               */
	Resize,				/* resize                */
	Redisplay,			/* expose                */
	SetValues,			/* set_values            */
	NULL,				/* set_values_hook       */
	XtInheritSetValuesAlmost,	/* set_values_almost     */
	NULL,				/* get_values+hook       */
	XtInheritAcceptFocus,		/* accept_focus          */
	XtVersion,			/* version               */
	NULL,				/* callback_private      */
	NULL,				/* tm_table              */
	QueryGeometry,			/* query_geometry        */
	XtInheritDisplayAccelerator,	/* display_acceleator    */
	NULL				/* extension             */
    },
    {	/* composite part */
	GeometryManager,		/* geometry_manager      */
	XtInheritChangeManaged,		/* change_managed        */
	InsertChild,			/* insert_child          */
	DeleteChild,			/* delete_child          */
	NULL				/* extension             */
    },
    {	/* viewport_class part */
	0,
    },
};

WidgetClass         viewportWidgetClass = (WidgetClass) & viewportClassRec;

/************************************************************
 *
 * Semi-Public interface functions
 *
 ************************************************************/

/* 
 * Initialize the viewport class.
 */

/* ARGSUSED */
static void
Initialize(request, new, args, num_args)
	Widget              request, new;
	ArgList             args;
	Cardinal           *num_args;
{
    ViewPortWidget      nself = (ViewPortWidget) new;
    Arg                 arg[2];

    nself->viewport.h_scrollbar = NULL;
    nself->viewport.v_scrollbar = NULL;
    nself->viewport.child = NULL;

   /* Set up 3D stuff */
    _XpwThreeDInit(new, &nself->viewport.threeD, nself->core.background_pixel);
    CreateGCs(new);

   /* Initialize width and height */
    if (nself->core.width == 0)
	nself->core.width = 100;
    if (nself->core.height == 0)
	nself->core.height = 100;

    nself->viewport.setup = TRUE;
    if (nself->viewport.use_h_scroll) {
	XtSetArg(arg[0], XtNorientation, XtorientVertical);
	nself->viewport.h_scrollbar = XtCreateManagedWidget(
		 "hscroll", scrollerWidgetClass, (Widget) nself, arg, 1);

	XtAddCallback(nself->viewport.h_scrollbar,
		      XtNcallback, HandleScroll, nself);
    }
    if (nself->viewport.use_v_scroll) {
	XtSetArg(arg[0], XtNorientation, XtorientHorizontal);
	nself->viewport.v_scrollbar = XtCreateManagedWidget(
		 "vscroll", scrollerWidgetClass, (Widget) nself, arg, 1);
	XtAddCallback(nself->viewport.v_scrollbar,
		      XtNcallback, HandleScroll, nself);
    }
    XtSetArg(arg[0], XtNheight, nself->core.height);
    XtSetArg(arg[1], XtNwidth, nself->core.width);
    nself->viewport.clip = XtCreateManagedWidget(
			    "clip", widgetClass, (Widget) nself, arg, 2);
    nself->viewport.setup = FALSE;
}

/*
 * SetValues: Figure out if we need to redisplay becuase of the changes,
 * also allocate any new GC's as required.
 */

/* ARGSUSED */
static              Boolean
SetValues(current, request, new, args, num_args)
	Widget              current, request, new;
	ArgList             args;
	Cardinal           *num_args;
{
    ViewPortWidget      self = (ViewPortWidget) new;
    ViewPortWidget      old_self = (ViewPortWidget) current;
    Boolean             ret_val = FALSE;
    Boolean             relayout = FALSE;

    if (self->core.sensitive != old_self->core.sensitive)
	ret_val = TRUE;
    if (_XpmThreeDSetValues(new, &old_self->viewport.threeD,
		   &self->viewport.threeD, new->core.background_pixel)) {
	_XpwThreeDDestroyShadow(current, &old_self->viewport.threeD);
	_XpwThreeDAllocateShadowGC(new, &self->viewport.threeD,
				   new->core.background_pixel);
	ret_val = TRUE;
    }
    if ((old_self->viewport.v_scroll_ontop != self->viewport.v_scroll_ontop) ||
	(old_self->viewport.h_scroll_onleft != self->viewport.h_scroll_onleft)) {
	relayout = TRUE;
    }
    if ((old_self->core.background_pixel != self->core.background_pixel) ||
	(old_self->viewport.topcolor != self->viewport.topcolor)) {
	DestroyGCs(current);
	CreateGCs(new);
	ret_val = TRUE;
    }
    if (relayout) {
	Layout(self, &self->core.width, &self->core.height, TRUE);
	ret_val = TRUE;
    }
    return (ret_val);
}

static void
Realize(w, valueMask, attributes)
	Widget              w;
	Mask               *valueMask;
	XSetWindowAttributes *attributes;
{
    ViewPortWidget      self = (ViewPortWidget) w;

    (*SuperClass->core_class.realize) (w, valueMask, attributes);

    if (self->viewport.h_scrollbar != NULL)
	XtRealizeWidget(self->viewport.h_scrollbar);
    if (self->viewport.v_scrollbar != NULL)
	XtRealizeWidget(self->viewport.v_scrollbar);
    if (self->viewport.child != NULL)
	XtRealizeWidget(self->viewport.child);
    XtRealizeWidget(self->viewport.clip);

    if (self->viewport.child != (Widget) NULL) {
	XtMoveWidget(self->viewport.child, (Position) 0, (Position) 0);
	XReparentWindow(XtDisplay(w), XtWindow(self->viewport.child),
			XtWindow(self->viewport.clip),
			(Position) 0, (Position) 0);
	XtMapWidget(self->viewport.child);
    }
   /* Layout widgets */
    Layout(self, &self->core.width, &self->core.height, TRUE);
    MoveChild(self, 0, 0);
}

/* 
 * GeometryManager, allow any request, just update scrollbars.
 */

static              XtGeometryResult
GeometryManager(wid, request, reply)
	Widget              wid;
	XtWidgetGeometry   *request, *reply;
{
    ViewPortWidget      self = (ViewPortWidget) XtParent(wid);
    XtGeometryMask      mask = request->request_mode;
    XtGeometryResult    result;
    XtWidgetGeometry    prequest, preply;
    Dimension           nh, nw;
    Dimension           s = self->viewport.threeD.shadow_width;
    Dimension           h = self->core.height - 2 * s;
    Dimension           w = self->core.width - 2 * s;

    if (self->viewport.child == NULL || self->viewport.child != wid)
	return XtGeometryNo;
   /* Figure out new size. */
    nw = (mask & CWWidth) ? request->width : self->viewport.child->core.width;
    nh = (mask & CWHeight) ? request->height : self->viewport.child->core.height;

   /* Compute our size. */
    if (self->viewport.v_scrollbar != NULL)
	w -= self->viewport.v_scrollbar->core.width + self->viewport.scrollborder;
    if (self->viewport.h_scrollbar != NULL)
	h -= self->viewport.h_scrollbar->core.height + self->viewport.scrollborder;

   /* If size is larger and we have no scrollbar or size is smaller then
    * current window and shrinking child, ask to resize parent to fit.
    * No matter what the resize returns we will accept it.
    */
    if ((nw > w && self->viewport.h_scrollbar == NULL) ||
	(nh > h && self->viewport.v_scrollbar == NULL) ||
	(nw < w && nw < self->viewport.child->core.width) ||
	(nh < h && nh < self->viewport.child->core.height)) {
	prequest.width = nw + 2 * s;
	prequest.height = nh + 2 * s;
	prequest.request_mode = XtCWQueryOnly;
	if (prequest.width != self->core.width)
	    prequest.request_mode |= CWWidth;
	if (prequest.height != self->core.height)
	    prequest.request_mode |= CWHeight;
	result = XtMakeGeometryRequest((Widget) self, &prequest, &preply);
       /* If it is query then we can always fit it so return ok. */
	if (result != XtGeometryNo) {
	   /* We almost got it... use what we did get */
	    if ((mask & XtCWQueryOnly) == 0) {
		prequest.width = preply.width;
		prequest.height = preply.height;
		prequest.request_mode = 0;
		if (prequest.width != self->core.width)
		    prequest.request_mode |= CWWidth;
		if (prequest.height != self->core.height)
		    prequest.request_mode |= CWHeight;

		XtMakeGeometryRequest((Widget) self, &prequest, &preply);
	    }
	   /* Set ourself to correct size */
	    if (preply.request_mode & CWWidth)
		w = preply.width - 2 * s;
	    if (preply.request_mode & CWHeight)
		h = preply.height - 2 * s;
	}
    }
   /* If smaller then self... force to fit */
    if (nw < w)
	nw = w;
    if (nh < h)
	nh = h;
    if (mask & XtCWQueryOnly) {
	result = XtGeometryYes;
	reply->width = nw;
	reply->height = nh;
	reply->request_mode = CWWidth | CWHeight;
	if ((reply->width != request->width) ||
	    (reply->height != request->height))
	    result = XtGeometryAlmost;
	return result;
    }
   /* Set child to requested size */
    XtResizeWidget(wid, nw, nh, 0);
   /* Now relayout ourselfs */
    Layout(self, &self->core.width, &self->core.height, TRUE);
    MoveChild(self, self->viewport.xoff, self->viewport.yoff);
    return XtGeometryDone;

}

static void
InsertChild(w)
	Widget              w;
{
    ViewPortWidget      self = (ViewPortWidget) XtParent(w);
    Arg                 arg[1];

    if (self->viewport.setup || self->viewport.child == NULL)
       /* insert the child widget in the composite children list with the */
       /* superclass insert_child routine.                                */
	(*SuperClass->composite_class.insert_child) (w);

    if (!self->viewport.setup && self->viewport.child != NULL)
	XtAppError(XtWidgetToApplicationContext(w),
		   "Trying to add a second child to viewport");
    if (!self->viewport.setup && self->viewport.child == NULL) {
	self->viewport.child = w;

       /* Make sure we have no border showing */
	XtSetArg(arg[0], XtNborderWidth, 0);
	XtSetValues(w, arg, 1);
	if (XtIsRealized(XtParent(w))) {
	    XtMoveWidget(self->viewport.child, (Position) 0, (Position) 0);
	    XReparentWindow(XtDisplay(w), XtWindow(self->viewport.child),
			    XtWindow(self->viewport.clip),
			    (Position) 0, (Position) 0);
	    XtMapWidget(self->viewport.child);
	}
    }
}

static void
DeleteChild(w)
	Widget              w;
{
    ViewPortWidget      self = (ViewPortWidget) XtParent(w);

   /* Clear child if this is the child */
    if (self->viewport.child == w)
	self->viewport.child = NULL;
   /* delete the child widget in the composite children list with the */
   /* superclass delete_child routine.                                */
    (*SuperClass->composite_class.delete_child) (w);
}

/*
 * Calculate preferred size, given constraining box, caching it in the widget.
 */

static              XtGeometryResult
QueryGeometry(w, intended, return_val)
	Widget              w;
	XtWidgetGeometry   *intended, *return_val;
{
    ViewPortWidget      self = (ViewPortWidget) w;
    Dimension           width, height;
    XtGeometryResult    ret_val = XtGeometryYes;
    XtGeometryMask      mode = intended->request_mode;

   /* Compute the default size */
    width = 0;
    height = 0;
    Layout(self, &width, &height, FALSE);

    if (((mode & CWWidth) && (intended->width < width)) || !(mode & CWWidth)) {
	return_val->request_mode |= CWWidth;
	return_val->width = width;
	ret_val = XtGeometryAlmost;
    }
    if (((mode & CWHeight) && (intended->height < height)) || !(mode & CWHeight)) {
	return_val->request_mode |= CWHeight;
	return_val->height = height;
	ret_val = XtGeometryAlmost;
    }
    if (ret_val == XtGeometryAlmost) {
	mode = return_val->request_mode;

	if (((mode & CWWidth) && (width == self->core.width)) &&
	    ((mode & CWHeight) && (height == self->core.height))) {
	    return (XtGeometryNo);
	}
    }
    return (ret_val);
}

/*
 * Handle resizes.
 */
static void
Resize(w)
	Widget              w;
{
    ViewPortWidget      self = (ViewPortWidget) w;
    Dimension           width = self->core.width;
    Dimension           height = self->core.height;

    Layout(self, &width, &height, TRUE);
   /* Move child to top corner */
    MoveChild(self, 0, 0);
}

/*
 * Redisplay whole widget.
 */

static void
Redisplay(wid, event, region)
	Widget              wid;
	XEvent             *event;
	Region              region;
{
    ViewPortWidget      self = (ViewPortWidget) wid;
    int                 x_loc, y_loc;
    Display            *dpy = XtDisplayOfObject(wid);
    Window              win = XtWindowOfObject(wid);
    Dimension           sb = self->viewport.scrollborder;
    Dimension           h, w;
    GC                  gc = self->viewport.top_gc;

    if (!XtIsRealized(wid))
	return;

    if (region == NULL)
	XClearWindow(dpy, win);

   /* Draw shadows around main window */
    x_loc = y_loc = 0;
    h = self->core.height;
    w = self->core.width;
    if (self->viewport.v_scrollbar != NULL) {
	int                 sh = self->viewport.v_scrollbar->core.height + sb;

	if (self->viewport.v_scroll_ontop) {
	    XFillRectangle(dpy, win, gc, 0, 0, w, sh);
	    y_loc += sh;
	} else {
	    XFillRectangle(dpy, win, gc, 0, h - sh, w, sh);
	}
    }
    if (self->viewport.h_scrollbar != NULL) {
	int                 sw = self->viewport.h_scrollbar->core.width + sb;

	if (self->viewport.h_scroll_onleft) {
	    XFillRectangle(dpy, win, gc, 0, 0, sw, h);
	    x_loc += sw;
	} else {
	    XFillRectangle(dpy, win, gc, w - sw, 0, sw, h);
	}
	w -= sw;
    }
    if (self->viewport.v_scrollbar != NULL) 
	h -= self->viewport.v_scrollbar->core.height + sb;

    _XpwThreeDDrawShadow(wid, event, region, &(self->viewport.threeD),
		 x_loc, y_loc, w, h, TRUE);
}

/*
 * Destroy an resources we allocated.
 */
static void
Destroy(w)
	Widget              w;
{
    ViewPortWidget      self = (ViewPortWidget) w;

    DestroyGCs(w);
    _XpwThreeDDestroyShadow(w, &(self->viewport.threeD));
}

/************************************************************
 *
 * Private functions
 *
 ************************************************************/

/*
 * Layout, return desired width and height. If set is TRUE make scrollbars
 * as needed.
 *
 */

static void
Layout(self, width, height, set)
	ViewPortWidget      self;
	Dimension          *width;
	Dimension          *height;
	int                 set;
{
    Dimension           w = *width;
    Dimension           h = *height;
    Dimension           cx, cy, cw, ch;		/* Clip window size */
    Dimension           sb = self->viewport.scrollborder;
    Dimension           s = self->viewport.threeD.shadow_width;
    Boolean             needwscroll = self->viewport.use_h_scroll;
    Boolean             needhscroll = self->viewport.use_v_scroll;
    Arg                 arg[1];

   /* Grab current settings if doing prefered size */
    if (self->viewport.child != NULL) {
	if (w == 0)
	    w = self->viewport.child->core.width;
	if (h == 0)
	    h = self->viewport.child->core.height;
    }
    if (w == 0 || (self->core.width < w && set))
	w = self->core.width - 2 * s;
    if (h == 0 || (self->core.height < h && set))
	h = self->core.height - 2 * s;

    if (self->viewport.child != NULL) {
       /* Fit into space given! */
	if (w < self->viewport.child->core.width) {
	    needwscroll = TRUE;
	}
	if (h < self->viewport.child->core.height) {
	    needhscroll = TRUE;
	}
    }
   /* If were doing it for real then make the scroll bars if needed */
    if (set) {
	int                 sh, sw;
	int                 th, tw;
	int                 lsize, tsize;

      redo:
	cw = self->core.width - 2 * s;
	ch = self->core.height - 2 * s;
	cx = 0;
	cy = 0;
	self->viewport.setup = TRUE;
	if (needhscroll) {
	    if (self->viewport.h_scrollbar == NULL) {
		XtSetArg(arg[0], XtNorientation, XtorientVertical);
		self->viewport.h_scrollbar = XtCreateManagedWidget(
		  "hscroll", scrollerWidgetClass, (Widget) self, arg, 1);

		XtAddCallback(self->viewport.h_scrollbar,
			      XtNcallback, HandleScroll, self);
		XtRealizeWidget(self->viewport.h_scrollbar);
	    }
	    lsize = self->viewport.h_scrollbar->core.width;
	} else {
	    if (self->viewport.h_scrollbar != NULL) {
		XtDestroyWidget(self->viewport.h_scrollbar);
		self->viewport.h_scrollbar = NULL;
	    }
	    lsize = 0;
	}
	if (needwscroll) {
	    if (self->viewport.v_scrollbar == NULL) {
		XtSetArg(arg[0], XtNorientation, XtorientHorizontal);
		self->viewport.v_scrollbar = XtCreateManagedWidget(
		  "vscroll", scrollerWidgetClass, (Widget) self, arg, 1);
		XtAddCallback(self->viewport.v_scrollbar,
			      XtNcallback, HandleScroll, self);
		XtRealizeWidget(self->viewport.v_scrollbar);
	    }
	    tsize = self->viewport.v_scrollbar->core.height;
	} else {
	    if (self->viewport.v_scrollbar != NULL) {
		XtDestroyWidget(self->viewport.v_scrollbar);
		self->viewport.v_scrollbar = NULL;
	    }
	    tsize = 0;
	}
	self->viewport.setup = FALSE;
       /* See if we overflowed the other direction */
	if (self->viewport.child != NULL &&
	    (w - lsize) < self->viewport.child->core.width &&
	    self->viewport.v_scrollbar == NULL) {
	    needwscroll = TRUE;
	    goto redo;
	}
	if (self->viewport.child != NULL &&
	    (h - tsize) < self->viewport.child->core.height &&
	    self->viewport.h_scrollbar == NULL) {
	    needhscroll = TRUE;
	    goto redo;
	}
	if (self->viewport.h_scrollbar != NULL) {

	   /* Adjust sizes of bars */
	    sw = lsize;
	    sh = h - tsize;
	    XtConfigureWidget(self->viewport.h_scrollbar,
			 ((self->viewport.h_scroll_onleft) ? 0 : w - sw),
			   ((self->viewport.v_scroll_ontop) ? tsize : 0),
			      lsize, (h - tsize),
			  self->viewport.h_scrollbar->core.border_width);
	   /* Total height */
	    th = (self->viewport.child != NULL) ?
		self->viewport.child->core.height : sh;
	   /* Remove undisplayable parts */
	    sh -= s * 2;
	    if (self->viewport.v_scrollbar != NULL)
		sh -= sb;
	   /* Set it */
	    XpwScrollerSetThumb(self->viewport.h_scrollbar, th, sh);
	   /* Adjust clip child */
	    cw -= sw + sb;
	    if (self->viewport.h_scroll_onleft)
		cx += sw + sb;
	}
	if (self->viewport.v_scrollbar != NULL) {

	   /* Adjust sizes of bars */
	    sw = w - lsize;
	    sh = tsize;
	    XtConfigureWidget(self->viewport.v_scrollbar,
			  ((self->viewport.h_scroll_onleft) ? lsize : 0),
			  ((self->viewport.v_scroll_ontop) ? 0 : h - sh),
			      (w - lsize), tsize,
			  self->viewport.v_scrollbar->core.border_width);
	   /* Total Width */
	    tw = (self->viewport.child != NULL) ?
		self->viewport.child->core.width : sh;
	   /* Remove undisplayable parts */
	    sw -= s * 2;
	    if (self->viewport.h_scrollbar != NULL)
		sw -= sb;
	   /* Set it */
	    XpwScrollerSetThumb(self->viewport.v_scrollbar, tw, sw);
	   /* Adjust clip child */
	    ch -= sh + sb;
	    if (self->viewport.v_scroll_ontop)
		cy = sh + sb;
	}
       /* Make sure clip does not overwrite boarders */
	XtConfigureWidget(self->viewport.clip, cx + s, cy + s, cw, ch, 0);
    } else {
       /* Return desired sizes */
	*width = w + 2 * s;
	*height = h + 2 * s;
    }
}

/*
 * Handle scrollbar motion.
 */
static void
HandleScroll(wid, client_data, call_data)
	Widget              wid;
	XtPointer           client_data;
	XtPointer           call_data;
{
    ViewPortWidget      self = (ViewPortWidget) client_data;
    Position            place = (int) call_data;
    Position            nxoff = self->viewport.xoff;
    Position            nyoff = self->viewport.yoff;

   /* Now do the scroll */
    if (wid == self->viewport.h_scrollbar)
	nyoff = place;
    if (wid == self->viewport.v_scrollbar)
	nxoff = place;
    MoveChild(self, nxoff, nyoff);
}

/*
 * Move child window.
 */
static void
MoveChild(self, nxoff, nyoff)
	ViewPortWidget      self;
	Position            nxoff;
	Position            nyoff;
{
    Dimension           s = self->viewport.threeD.shadow_width;
    Dimension           width, height;
    Dimension           h = self->core.height - 2 * s;
    Dimension           w = self->core.width - 2 * s;

   /* If no child, just set to origin */
    if (self->viewport.child == NULL) {
	if (self->viewport.h_scrollbar != NULL)
	    XpwScrollerSetPosition(self->viewport.h_scrollbar, 0);
	if (self->viewport.v_scrollbar != NULL)
	    XpwScrollerSetPosition(self->viewport.v_scrollbar, 0);
	self->viewport.xoff = 0;
	self->viewport.yoff = 0;
	return;
    }
   /* Make sure whole window is showing if no scroller */
    if (self->viewport.h_scrollbar == NULL)
	nyoff = 0;
    else
	w -= self->viewport.h_scrollbar->core.width + self->viewport.scrollborder;
    if (self->viewport.v_scrollbar == NULL)
	nxoff = 0;
    else
	h -= self->viewport.v_scrollbar->core.height + self->viewport.scrollborder;

   /* Range check */
    width = self->viewport.child->core.width;
    height = self->viewport.child->core.height;

    if ((nxoff + w) > width)
	nxoff = width - w;
    if ((nyoff + h) > height)
	nyoff = height - h;
    if (nxoff < 0)
	nxoff = 0;
    if (nyoff < 0)
	nyoff = 0;

   /* Move the window only if there was a change. */
    if (nxoff == self->viewport.xoff && nyoff == self->viewport.yoff)
	return;
    if (self->viewport.h_scrollbar != NULL && nyoff == self->viewport.yoff)
	XpwScrollerSetPosition(self->viewport.h_scrollbar, nyoff);
    if (self->viewport.v_scrollbar != NULL && nxoff == self->viewport.xoff)
	XpwScrollerSetPosition(self->viewport.v_scrollbar, nxoff);
    self->viewport.xoff = nxoff;
    self->viewport.yoff = nyoff;

    XtMoveWidget(self->viewport.child, s - nxoff, s - nyoff);
}

/* 
 * CreateGCs for viewport widget.
 */

static void
CreateGCs(w)
	Widget              w;
{
    ViewPortWidget      self = (ViewPortWidget) w;
    XGCValues           values;
    XtGCMask            mask;

    values.foreground = self->viewport.topcolor;
    values.background = self->core.background_pixel;
    mask = GCForeground | GCBackground;

    self->viewport.top_gc = XtGetGC(w, mask, &values);

}

/*
 * DestroyGC's we created.
 */
static void
DestroyGCs(w)
	Widget              w;
{
    ViewPortWidget      self = (ViewPortWidget) w;

    XtReleaseGC(w, self->viewport.top_gc);
}

/************************************************************
 *
 * Public interface functions
 *
 ************************************************************/

void
XpwViewportSetLocation(w, xoff, yoff)
	Widget              w;
	double		    xoff;
	double		    yoff;
{
    ViewPortWidget      self = (ViewPortWidget) w;

    if (self->viewport.child != NULL)
        MoveChild(self,
		 (int)(xoff * (double)self->core.width), 
		 (int)(yoff * (double)self->core.height));
}

void
XpwViewportSetCoorinates(w, xoff, yoff)
	Widget              w;
	Position	    xoff;
	Position	    yoff;
{
    ViewPortWidget      self = (ViewPortWidget) w;

    MoveChild(self, xoff, yoff);
}


