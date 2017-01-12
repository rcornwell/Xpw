/*
 * Slider widget.
 *
 * Draw a moveable slider. Unlike scroller thumb is of fixed size and there
 * is no arrows.
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
 * $Log: Slider.c,v $
 * Revision 1.3  1997/12/06 04:14:50  rich
 * Thickness is a dimension not a integer.
 *
 * Revision 1.2  1997/11/01 06:39:08  rich
 * Make sure slider does not work when set insensitve.
 * Thumb resource no longer used.
 * Fixed up some errors in geometry query code.
 * Added IsVert to clean up code.
 * General code cleanup.
 * RedrawThumb no longer accepts region and event.
 *
 * Revision 1.1  1997/10/08 04:08:42  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: Slider.c,v 1.3 1997/12/06 04:14:50 rich Beta $";
#endif

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Drawing.h>
#include <X11/Xmu/Misc.h>
#include <X11/Shell.h>
#include <X11/CoreP.h>
#include "XpwInit.h"
#include "SliderP.h"
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
static void         Realize(Widget /*w */ , Mask * /*valueMask */ ,
			    XSetWindowAttributes * /*attributes */ );
static void         Resize(Widget /*w */ );
static void         Redisplay(Widget /*wid */ , XEvent * /*event */ ,
			      Region /*region */ );
static void         Destroy(Widget /*w */ );

/* Private Functions */
static void         CreateGCs(Widget /*w */ );
static void         DestroyGCs(Widget /*w */ );
static void         RedrawThumb(Widget /*wid */ );
/* Actions */
static void         MoveThumb(Widget /*w */ , XEvent * /*event */ ,
			     String * /*params */ , Cardinal * /*num_params */ );
static void         Notify(Widget /*w */ , XEvent * /*event */ ,
			     String * /*params */ , Cardinal * /*num_params */ );
static void         Arm(Widget /*w */ , XEvent * /*event */ ,
			     String * /*params */ , Cardinal * /*num_params */ );
static void         DisArm(Widget /*w */ , XEvent * /*event */ ,
			     String * /*params */ , Cardinal * /*num_params */ );

#define IsVert(w)       ( (w)->slider.orientation == XtorientVertical )

/* Slider.c */

/* Our resources */
#define offset(field) XtOffsetOf(SliderRec, slider.field)
static XtResource   resources[] =
{
    {XtNmax, XtCMax, XtRInt, sizeof(int),
     offset(max), XtRImmediate, (XtPointer) 100},
    {XtNmin, XtCMin, XtRInt, sizeof(int),
     offset(min), XtRImmediate, (XtPointer) 0},
    {XtNscrollVCursor, XtCCursor, XtRCursor, sizeof(Cursor),
     offset(verCursor), XtRString, "sb_v_double_arrow"},
    {XtNscrollHCursor, XtCCursor, XtRCursor, sizeof(Cursor),
     offset(horCursor), XtRString, "sb_h_double_arrow"},
    {XtNposition, XtCPosition, XtRInt, sizeof(int),
     offset(position), XtRImmediate, (XtPointer) 0},
    {XtNthickness, XtCThickness, XtRDimension, sizeof(Dimension),
     offset(thickness), XtRImmediate, (XtPointer) 14},
    {XtNorientation, XtCOrientation, XtROrientation, sizeof(XtOrientation),
     offset(orientation), XtRImmediate, (XtPointer) XtorientVertical},
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
     offset(foreground), XtRString, XtDefaultForeground},
    {XtNcallback, XtCCallback, XtRCallback, sizeof(XtPointer),
     offset(callbacks), XtRCallback, (XtPointer) NULL},
    {XtNborderWidth, XtCBorderWidth, XtRDimension, sizeof(Dimension),
     XtOffsetOf(SliderRec, core.border_width), XtRImmediate, (XtPointer) 0},
    {XtNclue, XtCLabel, XtRString, sizeof(String),
     offset(clue), XtRImmediate, (XtPointer) 0},
    /* ThreeD resouces */
    threeDresources

};

#undef offset


static XtActionsRec actionsSlider[] =
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
     <BtnDown>:    DisArm() MoveThumb() \n\
   <BtnMotion>:    MoveThumb() Notify() \n\
     <BtnUp>:      Notify()";

#define SuperClass ((WidgetClass) &widgetClassRec)

SliderClassRec      sliderClassRec =
{
    {	/* core_class part */
	(WidgetClass) SuperClass,	/* superclass            */
	"Slider",			/* class_name            */
	sizeof(SliderRec),		/* widget_size           */
	ClassInitialize,		/* class_initialize      */
	NULL,				/* class_part_initialize */
	FALSE,				/* class_inited          */
	Initialize,			/* initialize            */
	NULL,				/* initialize_hook       */
	Realize,			/* realize               */
	actionsSlider,			/* actions               */
	XtNumber(actionsSlider),	/* num_actions           */
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
    {	/* slider_class part */
	0,
    },
};

WidgetClass         sliderWidgetClass = (WidgetClass) & sliderClassRec;

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
    SliderWidget        nself = (SliderWidget) new;
    Dimension           w, h;
    Dimension           s = nself->slider.threeD.shadow_width;

   /* Make sure thumb gets redrawn */
    nself->slider.t_top = nself->slider.t_bottom = -1;
    nself->slider.oposition = nself->slider.position - 1;

    _XpwThreeDInit(new, &nself->slider.threeD, nself->core.background_pixel);
    CreateGCs(new);

   /* Make sure we are not too small */
    if (IsVert(nself)) {
        h = nself->slider.thickness;
        w = 0;
    } else {
        w = nself->slider.thickness;
        h = 0;
    }
    w += 2 * s + nself->slider.thickness;
    h += 2 * s + nself->slider.thickness;
    if (nself->core.width == 0)
	nself->core.width = w;
    if (nself->core.height == 0)
	nself->core.height = h;

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
    SliderWidget        self = (SliderWidget) new;
    SliderWidget        old_self = (SliderWidget) current;
    Boolean             ret_val = FALSE;

    _XpwThreeDDestroyShadow(current, &old_self->slider.threeD);
    _XpwThreeDAllocateShadowGC(new, &self->slider.threeD,
			       new->core.background_pixel);

    if (self->core.sensitive != old_self->core.sensitive) 
	ret_val = TRUE;

    if (_XpmThreeDSetValues(new, &old_self->slider.threeD,
		     &self->slider.threeD, new->core.background_pixel)) {
	ret_val = TRUE;
    }
    if ((old_self->slider.foreground != self->slider.foreground) ||
      (old_self->core.background_pixel != self->core.background_pixel)) {
	DestroyGCs(current);
	CreateGCs(new);

	ret_val = TRUE;
    }
    if ((old_self->slider.position != self->slider.position) ||
	(old_self->slider.min != self->slider.min) ||
	(old_self->slider.max != self->slider.max)) {
	self->slider.oposition = self->slider.position - 1;
	ret_val = TRUE;
    }
    return (ret_val);
}

/*
 * Set the cursor based on orientation.
 */
static void 
Realize(w, valueMask, attributes)
	Widget              w;
	Mask               *valueMask;
	XSetWindowAttributes *attributes;
{
    SliderWidget        self = (SliderWidget) w;
    Cursor              cursor;

   /* Do actual work of creating window.  */
    (*sliderWidgetClass->core_class.superclass->core_class.realize)
	(w, valueMask, attributes);

   /* Now set the cursor */
    cursor = IsVert(self) ? self->slider.verCursor : self->slider.horCursor;
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
    SliderWidget        self = (SliderWidget) w;
    Dimension           width, height;
    Dimension           s = self->slider.threeD.shadow_width;
    XtGeometryResult    ret_val = XtGeometryYes;
    XtGeometryMask      mode = intended->request_mode;
    int			min = self->slider.min;
    int			max = self->slider.max;
    int			h;

   /* Compute the default size */
    width = 0;
    height = 0;
    
    h = (min > max)? (min - max): (max - min);
    if (h < 0)
	h = -h;
    if (IsVert(self))
	height = h;
    else 
	width = h;

    width += 2 * s + self->slider.thickness;
    height += 2 * s + self->slider.thickness;

    if (((mode & CWWidth) && (intended->width != width)) || !(mode & CWWidth)) {
	return_val->request_mode |= CWWidth;
	return_val->width = width;
	ret_val = XtGeometryAlmost;
    }
    if (((mode & CWHeight) && (intended->height != height)) || !(mode & CWHeight)) {
	return_val->request_mode |= CWHeight;
	return_val->height = height;
	ret_val = XtGeometryAlmost;
    }
    if (ret_val != XtGeometryAlmost) {
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
    SliderWidget        self = (SliderWidget) wid;
    Display            *dpy = XtDisplayOfObject(wid);
    Window              win = XtWindowOfObject(wid);

    if (!XtIsRealized(wid))
	return;

    if (region == NULL) 
	XClearWindow(dpy, win);

    self->slider.t_top = -1;
    _XpwThreeDDrawShadow(wid, event, region, &(self->slider.threeD), 0, 0,
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
    SliderWidget        self = (SliderWidget) w;

    DestroyGCs(w);
    _XpwThreeDDestroyShadow(w, &(self->slider.threeD));
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
    SliderWidget        self = (SliderWidget) w;
    XGCValues           values;
    XtGCMask            mask;

    values.foreground = self->slider.foreground;
    values.background = self->core.background_pixel;
    values.graphics_exposures = FALSE;
    mask = GCForeground | GCBackground | GCGraphicsExposures;

    self->slider.norm_gc = XtGetGC(w, mask, &values);

    values.fill_style = FillTiled;
    values.tile = XmuCreateStippledPixmap(XtScreenOfObject(w),
		    self->slider.foreground, self->core.background_pixel,
					  self->core.depth);
    values.graphics_exposures = FALSE;
    mask |= GCTile | GCFillStyle;
    self->slider.gray_gc = XtGetGC(w, mask, &values);
}

/*
 * Release old GC's.
 */
static void
DestroyGCs(w)
	Widget              w;
{
    SliderWidget        self = (SliderWidget) w;

    XtReleaseGC(w, self->slider.norm_gc);
    XtReleaseGC(w, self->slider.gray_gc);
}

/*
 * Redraw the thumb.
 */

static void
RedrawThumb(wid)
	Widget              wid;
{
    SliderWidget        self = (SliderWidget) wid;
    Display            *dpy = XtDisplayOfObject(wid);
    Window              win = XtWindowOfObject(wid);
    Dimension           s = self->slider.threeD.shadow_width;
    Dimension           h = self->core.height - 2 * s;
    Dimension           w = self->core.width - 2 * s;
    float               pos;
    int                 x_loc, y_loc, sw, sh;
    int                 cx_loc, cy_loc, csw, csh;
    int                 ox_loc, oy_loc, osw, osh;
    GC                  gc;

    if (!XtIsRealized(wid))
	return;

   /* Set the context based on sensitivity setting */
    if (XtIsSensitive(wid) && XtIsSensitive(XtParent(wid)))
	gc = self->slider.norm_gc;
    else
	gc = self->slider.gray_gc;

   /* Force position into range */
    if (self->slider.position < self->slider.min)
	self->slider.position = self->slider.min;
    if (self->slider.position > self->slider.max)
	self->slider.position = self->slider.max;
   /* Compute new top and bottom of thumb */
    pos = ((float) self->slider.position) /
	((float) (self->slider.max - self->slider.min));

   /* Keep values in range */
    if (pos > 1.0)
	pos = 1.0;

    if (IsVert(self)) {
	x_loc = s;
	sw = w;
	y_loc = (int) (pos * (float) (h - self->slider.thickness));
	sh = self->slider.thickness;
       /* Make sure it stays in range */
	if (y_loc < 0)
	    y_loc = 0;
	if ((y_loc + sh) > h) {
	    y_loc = h - sh;
	}
	y_loc += s;
	cx_loc = x_loc;
	cy_loc = y_loc + self->slider.thickness / 2 - 2;
	csh = 4;
	csw = sw;
	ox_loc = s;
	oy_loc = self->slider.t_top;
	osw = w;
	osh = self->slider.t_bottom;
	self->slider.t_top = y_loc;
	self->slider.t_bottom = sh;
    } else {
	y_loc = s;
	sh = h;
	x_loc = (int) (pos * (float) (w - self->slider.thickness));
	sw = self->slider.thickness;
       /* Make sure it stays in range */
	if (x_loc < 0)
	    x_loc = 0;
	if ((x_loc + sw) > w)
	    x_loc = w - sw;
	x_loc += s;
	cx_loc = x_loc + self->slider.thickness / 2 - 2;
	cy_loc = y_loc;
	csh = sh;
	csw = 4;
	ox_loc = self->slider.t_top;
	oy_loc = s;
	osw = self->slider.t_bottom;
	osh = h;
	self->slider.t_top = x_loc;
	self->slider.t_bottom = sw;
    }

    if (x_loc != ox_loc || y_loc != oy_loc || sw != osw || sh != osh) {
       /* Decide if we need to remove old one */
	if (ox_loc >= 0 && oy_loc >= 0)
	    XClearArea(dpy, win, ox_loc, oy_loc, osw, osh, FALSE);
       /* Draw new one */
	XFillRectangle(dpy, win, gc, x_loc + s, y_loc + s,
				 sw - 2 * s, sh - 2 * s);
        if (XtIsSensitive(wid) && XtIsSensitive(XtParent(wid)))
	    _XpwThreeDDrawShadow(wid, NULL, NULL, &(self->slider.threeD),
			     cx_loc, cy_loc, csw, csh, TRUE);
	_XpwThreeDDrawShadow(wid, NULL, NULL, &(self->slider.threeD),
			     x_loc, y_loc, sw, sh, FALSE);

    }
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
    SliderWidget        self = (SliderWidget) w;
    int                 point;
    Dimension           s = self->slider.threeD.shadow_width;
    Dimension           total;

    if (!XtIsRealized(w))
	return;

    if ((!XtIsSensitive(w)) || (!XtIsSensitive(XtParent(w))))
	return;

    if (IsVert(self)) {
	point = event->xbutton.y - s - (self->slider.thickness / 2);
	total = self->core.height - 2 * s - self->slider.thickness;
    } else {
	point = event->xbutton.x - s - (self->slider.thickness / 2);
	total = self->core.width - 2 * s - self->slider.thickness;
    }
    self->slider.position = self->slider.min + (int) (
			((float) (self->slider.max - self->slider.min)) *
				  (((float) (point)) / ((float) total)));
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
    SliderWidget        self = (SliderWidget) w;

    if ((!XtIsSensitive(w)) || (!XtIsSensitive(XtParent(w))))
	return;

    if (self->slider.oposition != self->slider.position) {
	XtCallCallbackList(w, self->slider.callbacks,
			   (XtPointer) self->slider.position);
	self->slider.oposition = self->slider.position;
    }
}

/* ARGSUSED */
static void
Arm(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    SliderWidget        self = (SliderWidget) w;

    _XpwArmClue(w, self->slider.clue);
}

/* ARGSUSED */
static void
DisArm(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    _XpwDisArmClue(w);
}

/************************************************************
 *
 * Public interface functions
 *
 ************************************************************/

/*
 * Set slider thumb size and showing.
 */
void
XpwSliderSetMinMax(w, min, max)
	Widget              w;
	int                 min;
	int                 max;
{
    SliderWidget        self = (SliderWidget) w;

    self->slider.min = min;
    self->slider.max = max;
    RedrawThumb(w);
}

/*
 * Move the thumb.
 */
void
XpwSliderSetPosition(w, pos)
	Widget              w;
	int                 pos;
{
    SliderWidget        self = (SliderWidget) w;

    if (self->slider.position != pos) {
	self->slider.position = pos;
	RedrawThumb(w);
    }
}
