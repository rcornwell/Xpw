/*
 * Scroller widget.
 *
 * Display scroller and allow for multiple selections.
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
 * $Log: Scroller.c,v $
 * Revision 1.3  1997/10/15 05:43:17  rich
 * Removed destroy for arrow widgets.
 *
 * Revision 1.2  1997/10/08 04:09:32  rich
 * Make sure the thumb is never bigger then window.
 * Fixed return information for arrows. Should not be possible to pass end of
 *  scrolled item.
 * When setting scroller externaly, callback results should be same.
 * Make sure we copy the background and foreground colors to the arrows.
 * General code cleanup.
 * Added cursors.
 * Added Clue support.
 *
 * Revision 1.1  1997/10/04 05:09:08  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: Scroller.c,v 1.3 1997/10/15 05:43:17 rich Exp rich $";

#endif

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Misc.h>
#include <X11/Xmu/Drawing.h>
#include <X11/Shell.h>
#include <X11/CoreP.h>
#include "XpwInit.h"
#include "ScrollerP.h"
#include "Arrow.h"
#include "Clue.h"

/* Semi Public Functions */
static XtGeometryResult QueryGeometry(Widget /*w */ ,
				 XtWidgetGeometry * /*intended */ ,
				 XtWidgetGeometry * /*return_val */ );
static void         ClassInitialize(void);
static void         Initialize(Widget /*request */ , Widget /*new */ ,
				 ArgList /*args */ , Cardinal * /*num_args */ );
static Boolean      SetValues(Widget /*current */ , Widget /*request */ ,
				 Widget /*new */ , ArgList /*args */ ,
				 Cardinal * /*num_args */ );
static void         Realize (Widget /*w*/, Mask */*valueMask*/,
                            XSetWindowAttributes */*attributes*/);
static void         Destroy(Widget /*w */ );
static void         Resize(Widget /*w */ );
static void         Redisplay(Widget /*wid */ , XEvent * /*event */ , Region /*region */ );
/* Private Functions */
static void         CreateGCs(Widget /*w */ );
static void         DestroyGCs(Widget /*w */ );
static void         HandleScroll(Widget /*wid */ , XtPointer /*client_data */ ,
				 XtPointer /*call_data */ );
static void         RedrawThumb(Widget /*wid */ );
/* Actions */
static void         MoveThumb(Widget /*w */ , XEvent * /*event */ ,
			      String * /*params */ , Cardinal * /*num_params */ );
static void         Notify(Widget /*w */ , XEvent * /*event */ ,
			      String * /*params */ , Cardinal * /*num_params */ );
static void         Arm(Widget /*w */, XEvent * /*event */,
                              String * /*params */, Cardinal * /*num_params */);
static void         DisArm(Widget /*w */, XEvent * /*event */,
                              String * /*params */, Cardinal * /*num_params */);

#define IsVert(w)       ( (w)->scroller.orientation == XtorientVertical )

/* Scroller.c */

/* Our resources */
#define offset(field) XtOffsetOf(ScrollerRec, scroller.field)
static XtResource   resources[] =
{
    {XtNuseArrows, XtCUseArrows, XtRBoolean, sizeof(Boolean),
     offset(usearrows), XtRImmediate, (XtPointer) TRUE},
    {XtNminimumThumb, XtCMinimumThumb, XtRDimension, sizeof(Dimension),
     offset(min_thumb), XtRImmediate, (XtPointer) 7},
    {XtNshown, XtCShown, XtRInt, sizeof(int),
     offset(tshown), XtRImmediate, (XtPointer) 0},
    {XtNlength, XtCLength, XtRInt, sizeof(int),
     offset(tlenght), XtRImmediate, (XtPointer) 1},
    {XtNposition, XtCPosition, XtRInt, sizeof(int),
     offset(position), XtRImmediate, (XtPointer) 0},
    {XtNthickness, XtCThickness, XtRInt, sizeof(int),
     offset(thickness), XtRImmediate, (XtPointer) 14},
    {XtNorientation, XtCOrientation, XtROrientation, sizeof(XtOrientation),
     offset(orientation), XtRImmediate, (XtPointer) XtorientVertical},
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
     offset(foreground), XtRString, XtDefaultForeground},
    {XtNcallback, XtCCallback, XtRCallback, sizeof(XtPointer),
     offset(callbacks), XtRCallback, (XtPointer) NULL},
    {XtNborderWidth, XtCBorderWidth, XtRDimension, sizeof(Dimension),
     XtOffsetOf(ScrollerRec, core.border_width), XtRImmediate, (XtPointer) 0},
    {XtNscrollVCursor, XtCCursor, XtRCursor, sizeof(Cursor),
     offset(verCursor), XtRString, "sb_v_double_arrow"},
    {XtNscrollHCursor, XtCCursor, XtRCursor, sizeof(Cursor),
     offset(horCursor), XtRString, "sb_h_double_arrow"},
    /* Override Arrow Timeouts */
    {XtNdelayTime, XtCDelayTime, XtRInt, sizeof(int),
     offset(delayTime), XtRImmediate, (XtPointer) 5},
    {XtNrepeatTime, XtCRepeatTime, XtRInt, sizeof(int),
     offset(repeatTime), XtRImmediate, (XtPointer) 1},
    {XtNclue, XtCLabel, XtRString, sizeof(String),
     offset(clue), XtRImmediate, (XtPointer) 0},

    /* ThreeD resouces */
    threeDresources

};

#undef offset

static XtActionsRec actionsScroller[] =
{
    {"Arm", Arm},
    {"DisArm", DisArm},
    {"MoveThumb", MoveThumb},
    {"Notify", Notify},
};

/* Translations */
static char         defaultTranslations[] =
"<EnterWindow>:    Arm()   \n\
 <LeaveWindow>:    DisArm()   \n\
    <Btn1Down>:    DisArm() MoveThumb(Forward) \n\
    <Btn2Down>:    DisArm() MoveThumb(Mouse) \n\
    <Btn3Down>:    DisArm() MoveThumb(Backward) \n\
  <Btn2Motion>:    MoveThumb(Mouse) Notify() \n\
       <BtnUp>:    Notify()";

#define SuperClass ((WidgetClass) &compositeClassRec)

ScrollerClassRec    scrollerClassRec =
{
    {	/* core_class part */
	(WidgetClass) SuperClass,	/* superclass            */
	"Scroller",			/* class_name            */
	sizeof(ScrollerRec),		/* widget_size           */
	ClassInitialize,		/* class_initialize      */
	NULL,				/* class_part_initialize */
	FALSE,				/* class_inited          */
	Initialize,			/* initialize            */
	NULL,				/* initialize_hook       */
	Realize,			/* realize               */
	actionsScroller,		/* actions               */
	XtNumber(actionsScroller),	/* num_actions           */
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
	defaultTranslations,		/* tm_table              */
	QueryGeometry,			/* query_geometry        */
	XtInheritDisplayAccelerator,	/* display_acceleator    */
	NULL				/* extension             */
    },
    {	/* composite part */
        XtInheritGeometryManager,	/* geometry_manager      */
        XtInheritChangeManaged,		/* change_managed        */
        XtInheritInsertChild,		/* insert_child          */
        XtInheritDeleteChild,		/* delete_child          */
    	NULL, 				/* extension             */ 
    },
    {	/* scroller_class part */
	0,
    },
};

WidgetClass         scrollerWidgetClass = (WidgetClass) & scrollerClassRec;

/************************************************************
 *
 * Semi-Public interface functions
 *
 ************************************************************/

/*
 * ClassInitialize: Add in a type converter for Justify argument.
 */

static void
ClassInitialize()
{
    XpwInitializeWidgetSet();
    XtAddConverter(XtRString, XtROrientation, XmuCvtStringToOrientation,
		   (XtConvertArgList) NULL, (Cardinal) 0);
}

/*
 * Initialize the class.
 */

/* ARGSUSED */
static void
Initialize(request, new, args, num_args)
	Widget              request, new;
	ArgList             args;
	Cardinal           *num_args;
{
    ScrollerWidget      nself = (ScrollerWidget) new;
    Dimension           w, h;
    Dimension           s = nself->scroller.threeD.shadow_width;
    Arg			arglist[5];

   /* Make sure thumb gets redrawn */
    nself->scroller.t_top = nself->scroller.t_bottom = -1;

   /* make arrows if we need them */
    if (nself->scroller.usearrows) {

	XtSetArg(arglist[0], XtNarrowType, IsVert(nself) ? XaUp : XaLeft);
	XtSetArg(arglist[1], XtNforeground, nself->scroller.foreground);
	XtSetArg(arglist[2], XtNbackground, nself->core.background_pixel);
	XtSetArg(arglist[3], XtNdelayTime, nself->scroller.delayTime);
	XtSetArg(arglist[4], XtNrepeatTime, nself->scroller.repeatTime);
	nself->scroller.tr_arrow = XtCreateManagedWidget( "tr_arrow",
				arrowWidgetClass, (Widget) nself, arglist, 5);
	XtSetArg(arglist[0], XtNarrowType, IsVert(nself) ? XaDown : XaRight);
	nself->scroller.bl_arrow = XtCreateManagedWidget( "bl_arrow",
				arrowWidgetClass, (Widget) nself, arglist, 5);
	XtAddCallback(nself->scroller.tr_arrow,
		      XtNcallback, HandleScroll, (XtPointer) nself);

	XtAddCallback(nself->scroller.bl_arrow,
		      XtNcallback, HandleScroll, (XtPointer) nself);
    }
    _XpwThreeDInit(new, &nself->scroller.threeD, nself->core.background_pixel);
    CreateGCs(new);

   /* Make sure we are not too small */
    w = nself->core.width;
    h = nself->core.height;
    if (w == 0 || h == 0) {
	if (nself->scroller.orientation == XtorientHorizontal) {
	    w = 2 * nself->scroller.min_thumb;
	    h = nself->scroller.thickness;
	    if (nself->scroller.tr_arrow != NULL) {
		w += nself->scroller.tr_arrow->core.width;
		w += nself->scroller.bl_arrow->core.width;
		if (nself->scroller.tr_arrow->core.height > h)
		    h = nself->scroller.tr_arrow->core.height;
		if (nself->scroller.bl_arrow->core.height > h)
		    h = nself->scroller.bl_arrow->core.height;
	    }
	} else {
	    h = 2 * nself->scroller.min_thumb;
	    w = nself->scroller.thickness;
	    if (nself->scroller.tr_arrow != NULL) {
		h += nself->scroller.tr_arrow->core.height;
		h += nself->scroller.bl_arrow->core.height;
		if (nself->scroller.tr_arrow->core.width > w)
		    w = nself->scroller.tr_arrow->core.width;
		if (nself->scroller.bl_arrow->core.width > w)
		    w = nself->scroller.bl_arrow->core.width;
	    }
	}
	w += 2 * s;
	h += 2 * s;
    }
    if (nself->core.width == 0)
	nself->core.width = w;
    if (nself->core.height == 0)
	nself->core.height = h;

}				/* Initialize */

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
    ScrollerWidget      self = (ScrollerWidget) new;
    ScrollerWidget      old_self = (ScrollerWidget) current;
    Boolean             ret_val = FALSE;
    Boolean             re_size = FALSE;
    Arg                 arglist[5];

    _XpwThreeDDestroyShadow(current, &old_self->scroller.threeD);
    _XpwThreeDAllocateShadowGC(new, &self->scroller.threeD,
			       new->core.background_pixel);

    if (self->core.sensitive != old_self->core.sensitive)
	ret_val = TRUE;
    if (_XpmThreeDSetValues(new, &old_self->scroller.threeD,
		   &self->scroller.threeD, new->core.background_pixel)) {
	ret_val = TRUE;
    }
   /* make arrows if we need them */
    if (self->scroller.usearrows != old_self->scroller.usearrows) {
	if (self->scroller.usearrows) {
	    XtSetArg(arglist[0], XtNarrowType, IsVert(self) ? XaUp : XaLeft);
	    XtSetArg(arglist[1], XtNforeground, self->scroller.foreground);
	    XtSetArg(arglist[2], XtNbackground, self->core.background_pixel);
	    XtSetArg(arglist[3], XtNdelayTime, self->scroller.delayTime);
	    XtSetArg(arglist[4], XtNrepeatTime, self->scroller.repeatTime);
	    self->scroller.tr_arrow = XtCreateManagedWidget( "tr_arrow",
				arrowWidgetClass, (Widget) self, arglist, 5);
	    XtSetArg(arglist[0], XtNarrowType, IsVert(self) ? XaDown : XaRight);
	    self->scroller.bl_arrow = XtCreateManagedWidget( "bl_arrow",
				arrowWidgetClass, (Widget) self, arglist, 5);
	    XtAddCallback(self->scroller.tr_arrow,
			  XtNcallback, HandleScroll, (XtPointer) self);
	    XtAddCallback(self->scroller.bl_arrow,
			  XtNcallback, HandleScroll, (XtPointer) self);
	    XtRealizeWidget(self->scroller.tr_arrow);
	    XtRealizeWidget(self->scroller.bl_arrow);
	} else {
	    if (self->scroller.tr_arrow)
		XtDestroyWidget(self->scroller.tr_arrow);
	    self->scroller.tr_arrow = NULL;
	    if (self->scroller.bl_arrow)
		XtDestroyWidget(self->scroller.bl_arrow);
	    self->scroller.bl_arrow = NULL;
	}
	re_size = TRUE;
	ret_val = TRUE;
    }
    if ((old_self->scroller.foreground != self->scroller.foreground) ||
      (old_self->core.background_pixel != self->core.background_pixel)) {
	DestroyGCs(current);
	CreateGCs(new);

       /* Set Arrow colors if needed */
	if (self->scroller.tr_arrow) {
	    XtSetArg(arglist[0], XtNforeground, self->scroller.foreground);
	    XtSetArg(arglist[1], XtNbackground, self->core.background_pixel);
	    XtSetValues(self->scroller.tr_arrow, arglist, 2);
	    XtSetValues(self->scroller.bl_arrow, arglist, 2);
	}
	ret_val = TRUE;
    }
    if ((old_self->scroller.delayTime != self->scroller.delayTime) ||
	(old_self->scroller.repeatTime != self->scroller.repeatTime)) {
	if (self->scroller.tr_arrow) {
	    XtSetArg(arglist[0], XtNdelayTime, self->scroller.delayTime);
	    XtSetArg(arglist[1], XtNrepeatTime, self->scroller.repeatTime);
	    XtSetValues(self->scroller.tr_arrow, arglist, 2);
	    XtSetValues(self->scroller.bl_arrow, arglist, 2);
	}
    }
    if ((old_self->scroller.position != self->scroller.position) ||
	(old_self->scroller.tlenght != self->scroller.tlenght) ||
	(old_self->scroller.tshown != self->scroller.tshown))
	ret_val = TRUE;
    if ((old_self->core.width != self->core.width) ||
	(old_self->core.height != self->core.height) || re_size)
	Resize(new);

    return (ret_val);
}

/*
 * Set the cursor based on orientation.
 */
static void Realize (w, valueMask, attributes)
    Widget w;
    Mask *valueMask;
    XSetWindowAttributes *attributes;
{
    ScrollerWidget self = (ScrollerWidget) w;
    Cursor       cursor;

    /* Do actual work of creating window.  */
    (*scrollerWidgetClass->core_class.superclass->core_class.realize)
        (w, valueMask, attributes);

    /* Now set the cursor */
    cursor = IsVert(self) ?  self->scroller.verCursor : self->scroller.horCursor;
    XDefineCursor(XtDisplay(w), XtWindow(w), cursor);
}

/*
 * Calculate preferred size, given constraining box, caching it in the widget.
 */

static              XtGeometryResult
QueryGeometry(w, intended, return_val)
	Widget              w;
	XtWidgetGeometry   *intended, *return_val;
{
    ScrollerWidget      self = (ScrollerWidget) w;
    Dimension           width, height;
    Dimension           s = self->scroller.threeD.shadow_width;
    XtGeometryResult    ret_val = XtGeometryYes;
    XtGeometryMask      mode = intended->request_mode;

   /* Compute the default size */
    if (IsVert(self)) {
	height = 2 * self->scroller.min_thumb;
	width = self->scroller.thickness;
	if (self->scroller.tr_arrow != NULL) {
	    height += self->scroller.tr_arrow->core.height;
	    height += self->scroller.bl_arrow->core.height;
	    if (self->scroller.tr_arrow->core.width > width)
		width = self->scroller.tr_arrow->core.width;
	    if (self->scroller.bl_arrow->core.width > width)
		width = self->scroller.bl_arrow->core.width;
	}
    } else {
	width = 2 * self->scroller.min_thumb;
	height = self->scroller.thickness;
	if (self->scroller.tr_arrow != NULL) {
	    width += self->scroller.tr_arrow->core.width;
	    width += self->scroller.bl_arrow->core.width;
	    if (self->scroller.tr_arrow->core.height > height)
		height = self->scroller.tr_arrow->core.height;
	    if (self->scroller.bl_arrow->core.height > height)
		height = self->scroller.bl_arrow->core.height;
	}
    }

    width += 2 * s;
    height += 2 * s;

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
    ScrollerWidget      self = (ScrollerWidget) w;
    Dimension           s = self->scroller.threeD.shadow_width;
    Dimension           width = self->core.width - s * 2;
    Dimension           height = self->core.height - s * 2;
    Dimension           arrows;

    arrows = (IsVert(self)) ? width : height;
/* Move arrows if they exist */
    if (self->scroller.tr_arrow) {
	XtMoveWidget(self->scroller.tr_arrow, s, s);
	XtResizeWidget(self->scroller.tr_arrow, arrows, arrows, 0);
	if (IsVert(self)) 
	    XtMoveWidget(self->scroller.bl_arrow,
		       s, height - self->scroller.bl_arrow->core.height);
	else
	    XtMoveWidget(self->scroller.bl_arrow,
			 width - self->scroller.bl_arrow->core.width, s);
	XtResizeWidget(self->scroller.bl_arrow, arrows, arrows, 0);
    }
    Redisplay(w, NULL, NULL);
}

/*
 * Redisplay the parent and all children.
 */

static void
Redisplay(wid, event, region)
	Widget              wid;
	XEvent             *event;
	Region              region;
{
    ScrollerWidget      self = (ScrollerWidget) wid;
    Display            *dpy = XtDisplayOfObject(wid);
    Window              win = XtWindowOfObject(wid);

    if (!XtIsRealized(wid))
	return;

    if (region == NULL) 
	XClearWindow(dpy, win);

    self->scroller.t_top = -1;
    _XpwThreeDDrawShadow(wid, event, region, &(self->scroller.threeD), 0, 0,
			 self->core.width, self->core.height, TRUE);
    RedrawThumb(wid);
}

/*
 * Destroy an resources we allocated.
 */
static void
Destroy(w)
	Widget              w;
{
    ScrollerWidget      self = (ScrollerWidget) w;

    DestroyGCs(w);
    _XpwThreeDDestroyShadow(w, &(self->scroller.threeD));
}

/************************************************************
 *
 * Private functions
 *
 ************************************************************/
/* 
 * Create GC's we need to.
 */

static void
CreateGCs(w)
	Widget              w;
{
    ScrollerWidget      self = (ScrollerWidget) w;
    XGCValues           values;
    XtGCMask            mask;

    values.foreground = self->scroller.foreground;
    values.background = self->core.background_pixel;
    values.graphics_exposures = FALSE;
    mask = GCForeground | GCBackground | GCGraphicsExposures;

    self->scroller.norm_gc = XtGetGC(w, mask, &values);

    values.fill_style = FillTiled;
    values.tile = XmuCreateStippledPixmap(XtScreenOfObject(w),
					  self->scroller.foreground, self->core.background_pixel, self->core.depth);
    values.graphics_exposures = FALSE;
    mask |= GCTile | GCFillStyle;
    self->scroller.gray_gc = XtGetGC(w, mask, &values);
}

/*
 * Release old GC's.
 */
static void
DestroyGCs(w)
	Widget              w;
{
    ScrollerWidget      self = (ScrollerWidget) w;

    XtReleaseGC(w, self->scroller.norm_gc);
    XtReleaseGC(w, self->scroller.gray_gc);
}

/*
 * Redraw the thumb.
 */

static void
RedrawThumb(wid)
	Widget              wid;
{
    ScrollerWidget      self = (ScrollerWidget) wid;
    Display            *dpy = XtDisplayOfObject(wid);
    Window              win = XtWindowOfObject(wid);
    Dimension           s = self->scroller.threeD.shadow_width;
    Dimension           h = self->core.height - 2 * s;
    Dimension           w = self->core.width - 2 * s;
    float               size;
    float               pos;
    int                 x_loc, y_loc, sw, sh;
    int                 ox_loc, oy_loc, osw, osh;
    GC                  gc;

    if (!XtIsRealized(wid))
	return;

   /* Set the context based on sensitivity setting */
    if (XtIsSensitive(wid) && XtIsSensitive(XtParent(wid)))
	gc = self->scroller.norm_gc;
    else
	gc = self->scroller.gray_gc;

   /* Force position into range */
    if (self->scroller.position < 0)
	self->scroller.position = 0;
    if (self->scroller.position > self->scroller.tlenght)
	self->scroller.position = self->scroller.tlenght;
   /* Compute new top and bottom of thumb */
    size = ((float) self->scroller.tshown) / ((float) self->scroller.tlenght);
    pos = ((float) self->scroller.position) / ((float) self->scroller.tlenght);

   /* Keep values in range */
    if (size > 1.0)
	size = 1.0;
    if (pos > 1.0)
	pos = 1.0;

    if (IsVert(self)) {
	x_loc = s;
	sw = w;
	if (self->scroller.tr_arrow)
	    h -= self->scroller.tr_arrow->core.height +
		self->scroller.bl_arrow->core.height;
	y_loc = (int) (pos * (float) h);
	sh = (int) (size * (float) h);
	if (sh < self->scroller.min_thumb)
	    sh = self->scroller.min_thumb;
	y_loc -= sh / 2;
       /* Make sure it stays in range */
	if (y_loc < 0)
	    y_loc = 0;
	if ((y_loc + sh) > h) {
	    y_loc = h - sh;
	}
	if (self->scroller.tr_arrow)
	    y_loc += self->scroller.tr_arrow->core.height;
	y_loc += s;
	ox_loc = s;
	oy_loc = self->scroller.t_top;
	osw = w;
	osh = self->scroller.t_bottom;
	self->scroller.t_top = y_loc;
	self->scroller.t_bottom = sh;
     } else {
	y_loc = s;
	sh = h;
	if (self->scroller.tr_arrow)
	    w -= self->scroller.tr_arrow->core.width +
		self->scroller.bl_arrow->core.width;
	x_loc = (int) (pos * (float) w);
	sw = (int) (size * (float) w);
	if (sw < self->scroller.min_thumb)
	    sw = self->scroller.min_thumb;
	x_loc -= sw / 2;
       /* Make sure it stays in range */
	if (x_loc < 0)
	    x_loc = 0;
	if ((x_loc + sw) > w)
	    x_loc = w - sw;
	if (self->scroller.tr_arrow)
	    x_loc += self->scroller.tr_arrow->core.width;
	x_loc += s;
	ox_loc = self->scroller.t_top;
	oy_loc = s;
	osw = self->scroller.t_bottom;
	osh = h;
	self->scroller.t_top = x_loc;
	self->scroller.t_bottom = sw;
    } 

    if (x_loc != ox_loc || y_loc != oy_loc || sw != osw || sh != osh) {
   /* Decide if we need to remove old one */
	if (ox_loc >= 0 && oy_loc >= 0)
	    XClearArea(dpy, win, ox_loc, oy_loc, osw, osh, FALSE);
   /* Draw new one */
	_XpwThreeDDrawShadow(wid, NULL, NULL, &(self->scroller.threeD), x_loc, y_loc,
			     sw, sh, 0);
	XFillRectangle(dpy, win, gc, x_loc + s, y_loc + s, sw - 2 * s, sh - 2 * s);

    }
}

/*
 * Handle arrow widgets.
 */
static void
HandleScroll(wid, client_data, call_data)
	Widget              wid;
	XtPointer           client_data;
	XtPointer           call_data;
{
    ScrollerWidget      self = (ScrollerWidget) client_data;

    if ((!XtIsSensitive(wid)) || (!XtIsSensitive(XtParent(wid))))
        return;

    if (wid == self->scroller.tr_arrow)
	self->scroller.position -= self->scroller.tshown;
    else
	self->scroller.position += self->scroller.tshown;
   /* Range check */
    if (self->scroller.position < 0)
	self->scroller.position = 0;
    if (self->scroller.position > self->scroller.tlenght)
	self->scroller.position = self->scroller.tlenght;
    if (XtIsRealized((Widget) self))
	RedrawThumb((Widget) self);
    Notify((Widget) self, NULL, NULL, NULL);
}


/************************************************************
 *
 * ACTION procedures
 *
 ************************************************************/

/*
 * Move Thumb
 */
static void
MoveThumb(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    ScrollerWidget      self = (ScrollerWidget) w;
    int                 point;
    Dimension           s = self->scroller.threeD.shadow_width;
    Dimension           total;

    if (*num_params == (Cardinal) 0) {
	XtWarning("Too few parameters passed to MoveThumb action table.");
	return;
    }
    if (*num_params != (Cardinal) 1) {
	XtWarning("Too many parameters passed to MoveThumb action table.");
	return;
    }

    if ((!XtIsSensitive(w)) || (!XtIsSensitive(XtParent(w))))
        return;

    if (IsVert(self)) {
	point = event->xbutton.y;
	total = self->core.height - 2 * s;
	if (self->scroller.tr_arrow) {
	    point -= self->scroller.tr_arrow->core.height;
	    total -= self->scroller.tr_arrow->core.height +
		self->scroller.bl_arrow->core.height;
	}
    } else {
	point = event->xbutton.x;
	total = self->core.width - 2 * s;
	if (self->scroller.tr_arrow) {
	    point -= self->scroller.tr_arrow->core.width;
	    total -= self->scroller.tr_arrow->core.width +
		self->scroller.bl_arrow->core.width;
	}
    } 

    switch (params[0][0]) {
	int                 amount;

    case 'm':
    case 'M':
	self->scroller.position = (int) (
	    ((float) self->scroller.tlenght) * (((float) (point)) /
					 ((float) total)));

	break;
    case 'f':
    case 'F':
	amount = (int) (
	   ((float) self->scroller.tshown) * (((float) (total - point)) /
					 ((float) total)));
	if (amount == 0)
	    amount = 1;
	self->scroller.position -= amount;
	break;

    case 'b':
    case 'B':
	amount = (int) (
			   ((float) self->scroller.tshown) * (((float) (point)) / ((float) total)));
	if (amount == 0)
	    amount = 1;
	self->scroller.position += amount;
	break;
    }
   /* Range check */
    if (self->scroller.position < 0)
	self->scroller.position = 0;
    if (self->scroller.position > self->scroller.tlenght)
	self->scroller.position = self->scroller.tlenght;
    if (XtIsRealized(w))
	RedrawThumb(w);
}

/*
 * Report position of top of thumb.
 */

/* ARGSUSED */
static void
Notify(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    ScrollerWidget      self = (ScrollerWidget) w;
    int                 pos;

    if ((!XtIsSensitive(w)) || (!XtIsSensitive(XtParent(w))))
        return;

    pos = self->scroller.position - (self->scroller.tshown / 2);
    if (pos < 0)
	pos = 0;
    if ((pos + (self->scroller.tshown)) > self->scroller.tlenght)
	pos = self->scroller.tlenght - self->scroller.tshown;
    XtCallCallbackList(w, self->scroller.callbacks, (XtPointer) pos);

}

/* ARGSUSED */
static void
Arm(w, event, params, num_params)
        Widget              w;
        XEvent             *event;
        String             *params;     /* unused */
        Cardinal           *num_params;         /* unused */
{
    ScrollerWidget     self = (ScrollerWidget) w;

    _XpwArmClue(w, self->scroller.clue);
}

/* ARGSUSED */
static void
DisArm(w, event, params, num_params)
        Widget              w;
        XEvent             *event;
        String             *params;     /* unused */
        Cardinal           *num_params;         /* unused */
{
    _XpwDisArmClue(w);
}

/************************************************************
 *
 * Public interface functions
 *
 ************************************************************/


/*
 * Set scroller thumb size and showing.
 */
void
XpwScrollerSetThumb(w, length, shown)
	Widget              w;
	int                 length;
	int                 shown;
{
    ScrollerWidget      self = (ScrollerWidget) w;

    if (length > 0)
	self->scroller.tlenght = length;
    if (shown >= 0)
	self->scroller.tshown = shown;
    if (self->scroller.position > self->scroller.tlenght)
	self->scroller.position = self->scroller.tlenght;
    RedrawThumb(w);
}

/*
 * Move the thumb.
 */
void
XpwScrollerSetPosition(w, pos)
	Widget              w;
	int                 pos;
{
    ScrollerWidget      self = (ScrollerWidget) w;

    if (pos < 0)
	return;
    self->scroller.position = pos + (self->scroller.tshown / 2);
    if (self->scroller.position > self->scroller.tlenght)
	self->scroller.position = self->scroller.tlenght;
    RedrawThumb(w);
}
