/*
 * Arrow widget.
 *
 * Draw a arrow.
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
 * $Log: Arrow.c,v $
 * Revision 1.1  1997/10/04 18:59:34  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: Arrow.c,v 1.1 1997/10/04 18:59:34 rich Exp rich $";
#endif

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Misc.h>
#include <X11/Shell.h>
#include "XpwInit.h"
#include <X11/CoreP.h>
#include "ArrowP.h"

/* Semi Public Functions */
static XtGeometryResult QueryGeometry(Widget /*w */,
				 XtWidgetGeometry * /*intended */,
				 XtWidgetGeometry * /*return_val */);
static void         ClassInitialize(void);
static void         Initialize(Widget /*request */, Widget /*new */,
				 ArgList /*args */, Cardinal */*num_args */ );
static Boolean      SetValues(Widget /*current */, Widget /*request */ ,
				 Widget /*new */, ArgList /*args */,
				 Cardinal * /*num_args */ );
static void         Destroy(Widget /*w */ );
static void         CreateGCs(Widget /*w */ );
static void         Redisplay(Widget /*wid */ , XEvent * /*event */ ,
				 Region /*region */ );

/* Our resources */
#define offset(field) XtOffsetOf(ArrowRec, arrow.field)
static XtResource   resources[] =
{
    /* ThreeD resouces */
    threeDresources
    /* Arrow Stuff */
    {XtNcallback, XtCCallback, XtRCallback, sizeof(XtPointer),
     offset(callbacks), XtRCallback, (XtPointer) NULL},
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
     offset(foreground), XtRString, XtDefaultForeground},
    {XtNarrowType, XtCShapeType, XtRShapeType, sizeof(ShapeType),
     offset(direction), XtRImmediate, (XtPointer) XaUp},
    {XtNdelayTime, XtCDelayTime, XtRInt, sizeof(int),
     offset(delayTime), XtRImmediate, (XtPointer) 0},
    {XtNrepeatTime, XtCRepeatTime, XtRInt, sizeof(int),
     offset(repeatTime), XtRImmediate, (XtPointer) 0},
    {XtNborderWidth, XtCBorderWidth, XtRDimension, sizeof(Dimension),
     XtOffsetOf(ArrowRec, core.border_width), XtRImmediate, (XtPointer) 0}
};

/* ACTION procedures */
static void         Set(Widget /*w */, XEvent * /*event */,
				 String */*params */, Cardinal */*num_params */);
static void         Unset(Widget /*w */, XEvent */*event */,
				 String */*params */, Cardinal */*num_params */);
static void         Reset(Widget /*w */, XEvent * /*event */,
				 String */*params */, Cardinal */*num_params */);
static void         Notify(Widget /*w */, XEvent * /*event */,
				 String */*params */, Cardinal */*num_params */);
static void	    RepeatTime(XtPointer /* client_data */,
				 XtIntervalId * /* timerid */);

/* Actions */
static XtActionsRec actionsList[] =
{
    {"set", Set},
    {"notify", Notify},
    {"reset", Reset},
    {"unset", Unset},
};

/* Translations */
static char         defaultTranslations[] =
"<LeaveWindow>:     reset()         \n\
    <Btn1Down>:     set()           \n\
      <Btn1Up>:     notify() unset()";

#define SuperClass ((WidgetClass) &widgetClassRec)

ArrowClassRec       arrowClassRec =
{
    {	/* core_class part */
	(WidgetClass) SuperClass,	/* superclass            */
	"Arrow",			/* class_name            */
	sizeof(ArrowRec),		/* widget_size           */
	XpwInitializeWidgetSet,		/* class_initialize      */
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
	TRUE,				/* compres_motion        */
	TRUE,				/* compress_exposure     */
	TRUE,				/* compress_enterleave   */
	FALSE,				/* visible_interest      */
	Destroy,			/* destroy               */
	NULL,				/* resize                */
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
    {	/* arrow_class part */
	0,
    },
};

WidgetClass         arrowWidgetClass = (WidgetClass) & arrowClassRec;

/************************************************************
 *
 * Semi Public interface functions
 *
 ************************************************************/

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
    ArrowWidget         nself = (ArrowWidget) new;
    Dimension           border;

    _XpwThreeDInit(new, &nself->arrow.threeD, nself->core.background_pixel);

    border = nself->arrow.threeD.shadow_width;
    if (nself->core.width == 0)
	    nself->core.width = 4 + (2 * border);
    if (nself->core.height == 0)
	    nself->core.height = 4 + (2 * border);

    CreateGCs(new);
    nself->arrow.set = FALSE;
    nself->arrow.timer = (XtIntervalId) NULL;
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
    ArrowWidget         self = (ArrowWidget) new;
    ArrowWidget         old_self = (ArrowWidget) current;
    Boolean             ret_val = FALSE;

    if (self->core.sensitive != old_self->core.sensitive) {
	self->arrow.set = FALSE;
	ret_val = TRUE;
    }
    if (_XpmThreeDSetValues(new, &old_self->arrow.threeD,
		      &self->arrow.threeD, new->core.background_pixel)) {
	ret_val = TRUE;
    }
    if ((old_self->arrow.foreground != self->arrow.foreground) ||
      (old_self->core.background_pixel != self->core.background_pixel)) {
       /* Release old GC's before creating new ones */
	XtReleaseGC(current, self->arrow.inverse_GC);
	XtReleaseGC(current, self->arrow.norm_GC);
	XtReleaseGC(current, self->arrow.norm_gray_GC);

	CreateGCs(new);
	ret_val = TRUE;
    }
    if (self->arrow.direction != old_self->arrow.direction)
	ret_val = TRUE;
    return (ret_val);
}

/*
 * Calculate preferred size, given constraining box, caching it in the widget.
 */

static              XtGeometryResult
QueryGeometry(w, intended, return_val)
	Widget              w;
	XtWidgetGeometry   *intended, *return_val;
{
    ArrowWidget         self = (ArrowWidget) w;
    Dimension           width, height;
    XtGeometryResult    ret_val = XtGeometryYes;
    XtGeometryMask      mode = intended->request_mode;
    Dimension           border;

    border = self->arrow.threeD.shadow_width;
    width = 4 + (2 * border);
    height = 4 + (2 * border);

    if (((mode & CWWidth) && (intended->width < width)) ||
	!(mode & CWWidth)) {
	return_val->request_mode |= CWWidth;
	return_val->width = width;
	ret_val = XtGeometryAlmost;
    }
    if (((mode & CWHeight) && (intended->height < height)) ||
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
 * Create the GC's for drawing.
 */
static void
CreateGCs(w)
	Widget              w;
{
    ArrowWidget         self = (ArrowWidget) w;
    XGCValues           values;
    XtGCMask            mask;

    values.foreground = self->arrow.foreground;
    values.background = self->core.background_pixel;
    mask = GCForeground | GCBackground;
    self->arrow.norm_GC = XtGetGC((Widget) self, mask, &values);

    values.background = self->arrow.foreground;
    values.foreground = self->core.background_pixel;
    self->arrow.inverse_GC = XtGetGC((Widget) self, mask, &values);

    values.foreground = self->arrow.foreground;
    values.background = self->core.background_pixel;
    values.fill_style = FillTiled;
    values.tile = XmuCreateStippledPixmap(XtScreenOfObject(w),
		     self->arrow.foreground, self->core.background_pixel,
					      self->core.depth);
    mask |= GCTile | GCFillStyle;
    self->arrow.norm_gray_GC = XtGetGC((Widget) self, mask, &values);
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
    ArrowWidget         self = (ArrowWidget) wid;
    Display            *dpy = XtDisplayOfObject(wid);
    Window              win = XtWindowOfObject(wid);
    Dimension           h = self->core.height;
    Dimension           w = self->core.width;
    GC                  cent;

    XClearWindow(dpy, win);
   /* Figure out what colors to make arrow */
    if (self->arrow.set) 
	cent = self->arrow.inverse_GC;
    else 
	cent = self->arrow.norm_GC;

    if (!XtIsSensitive(wid))
	cent = self->arrow.norm_gray_GC;
    _XpwThreeDDrawShape(wid, event, region, &(self->arrow.threeD), cent, 
		self->arrow.direction, 0, 0, w, h, self->arrow.set);
}

/*
 * Destroy an resources we allocated.
 */
static void
Destroy(w)
	Widget              w;
{
    ArrowWidget         self = (ArrowWidget) w;

    /* Remove any timers that may be pending. */
    if (self->arrow.timer != (XtIntervalId) NULL) {
        XtRemoveTimeOut(self->arrow.timer);
        self->arrow.timer = (XtIntervalId) NULL;
    }

    _XpwThreeDDestroyShadow(w, &(self->arrow.threeD));
    XtReleaseGC(w, self->arrow.inverse_GC);
    XtReleaseGC(w, self->arrow.norm_GC);
    XtReleaseGC(w, self->arrow.norm_gray_GC);
}

/************************************************************
 *
 * ACTION procedures
 *
 ************************************************************/

/* ARGSUSED */
static void
Set(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    ArrowWidget         self = (ArrowWidget) w;
    XtAppContext        context = XtWidgetToApplicationContext(w);


    if (self->arrow.set)
	return;

    self->arrow.set = TRUE;

    if (!XtIsRealized(w))
	return;

    /* Install timers if needed */
    if (self->arrow.delayTime != 0) 
        self->arrow.timer = XtAppAddTimeOut(context, 1000*self->arrow.delayTime,
                                            RepeatTime, (XtPointer) self);
    if (self->arrow.delayTime == 0 && self->arrow.repeatTime != 0) 
        self->arrow.timer = XtAppAddTimeOut(context, 1000*self->arrow.repeatTime,
                                           RepeatTime, (XtPointer) self);
    Redisplay(w, event, (Region) NULL);
}

/* ARGSUSED */
static void
Unset(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;
{
    ArrowWidget         self = (ArrowWidget) w;

    if (!self->arrow.set)
	return;

    self->arrow.set = FALSE;

    if (self->arrow.timer != (XtIntervalId) NULL) {
        XtRemoveTimeOut(self->arrow.timer);
        self->arrow.timer = (XtIntervalId) NULL;
    }

    if (XtIsRealized(w))
        Redisplay(w, event, (Region) NULL);
}

/* ARGSUSED */
static void
Reset(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    ArrowWidget         self = (ArrowWidget) w;

    if (self->arrow.set) 
	Unset(w, event, params, num_params);
}

/* ARGSUSED */
static void
Notify(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    ArrowWidget         self = (ArrowWidget) w;

   /* check to be sure state is still Set so that user can cancel
    * the action (e.g. by moving outside the window, in the default
    * bindings.
    */
    if (self->arrow.set)
	XtCallCallbackList(w, self->arrow.callbacks, (XtPointer) NULL);
}

/*
 * Handle Time out Events.
 */
static void
RepeatTime(client_data, timerid)
        XtPointer           client_data;
        XtIntervalId       *timerid;
{
    ArrowWidget         self = (ArrowWidget) client_data;
    XtAppContext        context = XtWidgetToApplicationContext((Widget)self);

    self->arrow.timer = (XtIntervalId) NULL;
    /* Set up a new timer */
    if (self->arrow.repeatTime != 0)
        self->arrow.timer = XtAppAddTimeOut(context, 1000*self->arrow.repeatTime,
                                           RepeatTime, (XtPointer) self);

    Notify((Widget)self, NULL, NULL, NULL);
}


