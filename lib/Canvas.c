/*
 * Canvas widget.
 *
 * Blank widget, with callbacks for expose.
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
 * $Log: Canvas.c,v $
 * Revision 1.1  1997/10/19 01:41:37  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: Canvas.c,v 1.1 1997/10/19 01:41:37 rich Exp rich $";

#endif

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Misc.h>
#include <X11/Xmu/CharSet.h>
#include <X11/Shell.h>
#include <X11/CoreP.h>
#include "XpwInit.h"
#include "CanvasP.h"
#include "Clue.h"

/* Semi Public Functions */
static void         ClassInitialize(void);
static void         Initialize(Widget /*request */ , Widget /*new */ ,
				 ArgList /*args */ , Cardinal * /*num_args */ );
static XtGeometryResult QueryGeometry(Widget /*w */ , 
				XtWidgetGeometry * /*intended */ , 
				XtWidgetGeometry * /*return_val */ );
static void         Resize(Widget /*w */ );
static void         Redisplay(Widget /*w */ , XEvent * /*event */ , 
				Region /*region */ );

/* Type Converters */
Boolean             cvtStringToResizePolicy(Display * /*dpy */ , 
				XrmValuePtr /*args */ , 
				Cardinal * /*num_args */ , 
				XrmValuePtr /*from */ , XrmValuePtr /*to */ , 
				XtPointer * /*converter_data */ );
Boolean             cvtResizePolicyToString(Display * /*dpy */ , 
				XrmValuePtr /*args */ , 
				Cardinal * /*num_args */ , 
				XrmValuePtr /*from */ , XrmValuePtr /*to */ , 
				XtPointer * /*converter_data */ );

/* Actions */
static void         CanvasAction(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         Enter(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         Leave(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );

static XtActionsRec actionsList[] =
{
    {"CanvasAction", CanvasAction},
    {"Enter", Enter},
    {"Leave", Leave},
};

/* Our resources */
#define coffset(field) XtOffsetOf(CanvasRec, field)
#define offset(field) XtOffsetOf(CanvasRec, canvas.field)
static XtResource   resources[] =
{
    /* Canvas resources */
    {XtNwidth, XtCWidth, XtRDimension, sizeof(Dimension),
     coffset(core.width), XtRImmediate, (XtPointer) 100},
    {XtNheight, XtCHeight, XtRDimension, sizeof(Dimension),
     coffset(core.height), XtRImmediate, (XtPointer) 100},
    {XtNactionCallback, XtCCallback, XtRCallback, sizeof(XtPointer),
     offset(canvas_action), XtRCallback, NULL},
    {XtNexposeCallback, XtCCallback, XtRCallback, sizeof(XtPointer),
     offset(expose_call), XtRCallback, NULL},
    {XtNresizeCallback, XtCCallback, XtRCallback, sizeof(XtPointer),
     offset(resize_call), XtRCallback, NULL},
    {XtNresizePolicy, XtCResizePolicy, XtRResizePolicy,
     sizeof(XpwCanvasResizePolicy),
     offset(resizePolicy), XtRImmediate, (XtPointer) XpwResize_Any},
    {XtNclue, XtCLabel, XtRString, sizeof(String),
     offset(clue), XtRImmediate, (XtPointer) NULL},

};

#define SuperClass ((WidgetClass) &widgetClassRec)

CanvasClassRec      canvasClassRec =
{
    {	/* core_class part */
	(WidgetClass) SuperClass,	/* superclass            */
	"Canvas",			/* class_name            */
	sizeof(CanvasRec),		/* widget_size           */
	ClassInitialize,		/* class_initialize      */
	NULL,				/* class_part_initialize */
	FALSE,				/* class_inited          */
	Initialize,			/* initialize            */
	NULL,				/* initialize_hook       */
	XtInheritRealize,		/* realize               */
	actionsList,			/* actions               */
	XtNumber(actionsList),		/* num_actions           */
	resources,			/* resources             */
	XtNumber(resources),		/* num_resources         */
	NULLQUARK,			/* xrm_class             */
	FALSE,				/* compres_motion        */
	FALSE,				/* compress_exposure     */
	TRUE,				/* compress_enterleave   */
	TRUE,				/* visible_interest      */
	NULL,				/* destroy               */
	Resize,				/* resize                */
	Redisplay,			/* expose                */
	NULL,				/* set_values            */
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
    {	/* canvas_class part */
	0,
    },
};

WidgetClass         canvasWidgetClass = (WidgetClass) & canvasClassRec;

/************************************************************
 *
 * Semi-Public interface functions
 *
 ************************************************************/

static void
ClassInitialize()
{
    XtSetTypeConverter(XtRString, XtRResizePolicy, cvtStringToResizePolicy,
		       NULL, 0, XtCacheNone, NULL);
    XtSetTypeConverter(XtRResizePolicy, XtRString, cvtResizePolicyToString,
		       NULL, 0, XtCacheNone, NULL);
}

static void
Initialize(request, new, args, num_args)
	Widget              request, new;
	ArgList             args;
	Cardinal           *num_args;
{
    CanvasWidget        self = (CanvasWidget) new;

    if (self->core.width == 0)
	self->core.width = 100;
    if (self->core.height == 0)
	self->core.height = 100;
}

/*
 * Calculate preferred size, given constraining box, caching it in the widget.
 */

static              XtGeometryResult
QueryGeometry(w, intended, return_val)
	Widget              w;
	XtWidgetGeometry   *intended, *return_val;
{
    CanvasWidget        self = (CanvasWidget) w;
    Dimension           width, height;
    XtGeometryResult    ret_val = XtGeometryYes;
    XtGeometryMask      mode = intended->request_mode;

    width = self->core.width;
    height = self->core.height;

    switch (self->canvas.resizePolicy) {
    case XpwResize_Any:
	break;
    case XpwResize_None:
	width = self->core.width;
	height = self->core.height;
	break;
    case XpwResize_Grow:
	if (width < self->core.width)
	    width = self->core.width;
	if (height < self->core.height)
	    height = self->core.height;
	break;
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
	    ((mode & CWHeight) && (height == self->core.height)))
	    return (XtGeometryNo);
    }
    return (ret_val);
}

/*
 *  Resize, just relayout the children for new size.
 */

static void
Resize(w)
	Widget              w;
{
    XpwCanvasCallbackDataRec call_data;

    call_data.reason = XpwCanvasResize;
    call_data.event = NULL;
    call_data.window = XtWindowOfObject(w);

    XtCallCallbacks(w, XtNresizeCallback, (XtPointer) & call_data);

}

/*
 * Redisplay pass results onto application.
 */

static void
Redisplay(w, event, region)
	Widget              w;
	XEvent             *event;
	Region              region;
{
    XpwCanvasCallbackDataRec call_data;

    call_data.reason = XpwCanvasExpose;
    call_data.event = event;
    call_data.window = XtWindowOfObject(w);

    XtCallCallbacks(w, XtNexposeCallback, (XtPointer) & call_data);
}

/************************************************************
 *
 * Type converters.
 *
 ************************************************************/

Boolean
cvtStringToResizePolicy(dpy, args, num_args, from, to, converter_data)
	Display            *dpy;
	XrmValuePtr         args;
	Cardinal           *num_args;
	XrmValuePtr         from;
	XrmValuePtr         to;
	XtPointer          *converter_data;
{
    String              s = (String) from->addr;
    static XpwCanvasResizePolicy result;

    if (*num_args != 0) {
	XtAppErrorMsg(XtDisplayToApplicationContext(dpy),
		      "cvtStringToResizePolicy", "wrongParameters",
		      "XtToolkitError",
		    "String to frame type conversion needs no arguments",
		      (String *) NULL, (Cardinal *) NULL);
	return FALSE;
    }
    if (XmuCompareISOLatin1(s, "any") == 0)
	result = XpwResize_Any;
    else if (XmuCompareISOLatin1(s, "none") == 0)
	result = XpwResize_None;
    else if (XmuCompareISOLatin1(s, "grow") == 0)
	result = XpwResize_Grow;
    else {
	XtDisplayStringConversionWarning(dpy, s, XtRResizePolicy);
	return FALSE;
    }

    if (to->addr != NULL) {
	if (to->size < sizeof(XpwCanvasResizePolicy)) {
	    to->size = sizeof(XpwCanvasResizePolicy);
	    return FALSE;
	}
	*(XpwCanvasResizePolicy *) (to->addr) = result;
    } else
	(XpwCanvasResizePolicy *) to->addr = &result;
    to->size = sizeof(XpwCanvasResizePolicy);
    return TRUE;
}

Boolean
cvtResizePolicyToString(dpy, args, num_args, from, to, converter_data)
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
		      "cvtResizePolicyToString", "wrongParameters",
		      "XtToolkitError",
		   "Fframe type to String conversion needs no arguments",
		      (String *) NULL, (Cardinal *) NULL);
	return FALSE;
    }
    switch (*(XpwCanvasResizePolicy *) from->addr) {
    case XpwResize_Any:
	result = "any";
    case XpwResize_None:
	result = "none";
    case XpwResize_Grow:
	result = "grow";
    default:
	XtError("Illegal ResizePolicy");
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
 * ACTION procedures
 *
 ************************************************************/

/* ARGSUSED */
static void
CanvasAction(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    XpwCanvasCallbackDataRec call_data;

    call_data.reason = XpwCanvasAction;
    call_data.event = event;
    call_data.params = params;
    call_data.num_params = *num_params;

    XtCallCallbacks(w, XtNactionCallback, (XtPointer) & call_data);
}

/* ARGSUSED */
static void
Enter(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    CanvasWidget        self = (CanvasWidget) w;

    _XpwArmClue(w, self->canvas.clue);
}

/* ARGSUSED */
static void
Leave(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    _XpwDisArmClue(w);
}
