/*
 * Chart widget.
 *
 * Draws a strip chart.
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
static char        *rcsid = "$Id: $";

#endif

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Misc.h>
#include <X11/Shell.h>
#include <X11/CoreP.h>
#include "XpwInit.h"
#include "ChartP.h"
#include "Clue.h"

/* Semi Public Functions */
static void         Initialize(Widget /*request */ , Widget /*new */ ,
				 ArgList /*args */ , Cardinal * /*num_args */ );
static XtGeometryResult QueryGeometry(Widget /*w */ ,
				 XtWidgetGeometry * /*intended */ ,
				 XtWidgetGeometry * /*return_val */ );
static Boolean      SetValues(Widget /*current */ , Widget /*request */ ,
				 Widget /*new */ , ArgList /*args */ ,
				 Cardinal * /*num_args */ );
static void         Destroy(Widget /*w */ );
static void         Redisplay(Widget /*wid */ , XEvent * /*event */ ,
				 Region /*region */ );

/* Private Functions */
static void         GetData(XtPointer /*client_data */ ,
				 XtIntervalId * /*timerid */ );
static void         AddData(ChartWidget /*self */ , double /*value */ );
static void         RedrawChart(ChartWidget /*self */ );
static void         ScrollData(ChartWidget /*self */ );
static void         CreateGCs(Widget /*w */ );
static void         DestroyGCs(Widget /*w */ );

/* Action Procedures. */
static void         Enter(Widget /*w */ , XEvent * /*event */ ,
				 String * /*params */ , Cardinal * /*num_params */ );
static void         Leave(Widget /*w */ , XEvent * /*event */ ,
				 String * /*params */ , Cardinal * /*num_params */ );

/* Our resources */
#define offset(field) XtOffsetOf(ChartRec, chart.field)
static XtResource   resources[] =
{
    {XtNinterval, XtCInterval, XtRInt, sizeof(int),
     offset(interval), XtRImmediate, (XtPointer) 10},
    {XtNminScale, XtCScale, XtRInt, sizeof(int),
     offset(minScale), XtRImmediate, (XtPointer) 1},
    {XtNdatacolor, XtCForeground, XtRPixel, sizeof(Pixel),
     offset(data_color), XtRString, XtDefaultForeground},
    {XtNscalecolor, XtCForeground, XtRPixel, sizeof(Pixel),
     offset(scale_color), XtRString, XtDefaultForeground},
    {XtNgetValue, XtCCallback, XtRCallback, sizeof(XtPointer),
     offset(getData), XtRImmediate, (XtPointer) NULL},
    {XtNscrollAmount, XtCScrollAmount, XtRInt, sizeof(int),
     offset(scrollAmount), XtRImmediate, (XtPointer) 10},

    /* Label resources */
    LabelResources
    /* ThreeD resouces */
    threeDresources
    {XtNclue, XtCLabel, XtRString, sizeof(String),
     offset(clue), XtRImmediate, (XtPointer) NULL}
};

static XtActionsRec actionsList[] =
{
    {"enter", Enter},
    {"leave", Leave},
};

/* Translations */
static char         defaultTranslations[] =
"<EnterWindow>:     enter()        \n\
 <LeaveWindow>:     leave()        ";

#define SuperClass ((WidgetClass) &widgetClassRec)

ChartClassRec       chartClassRec =
{
    {	/* core_class part */
	(WidgetClass) SuperClass,	/* superclass            */
	"Chart",			/* class_name            */
	sizeof(ChartRec),		/* widget_size           */
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
	TRUE,				/* visible_interest      */
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
    {	/* chart_class part */
	0,
    },
};

WidgetClass         chartWidgetClass = (WidgetClass) & chartClassRec;

/************************************************************
 *
 * Semi-Public interface functions
 *
 ************************************************************/

/* ARGSUSED */
static void
Initialize(request, new, args, num_args)
	Widget              request, new;
	ArgList             args;
	Cardinal           *num_args;
{
    ChartWidget         nself = (ChartWidget) new;
    XtAppContext        context = XtWidgetToApplicationContext(new);
    Dimension           height, width;
    Dimension           s = 2 * nself->chart.threeD.shadow_width;
    int                 i;

    if (nself->chart.label.label == NULL)
	nself->chart.label.label = XtName(new);
    else
	nself->chart.label.label = XtNewString(nself->chart.label.label);

    _XpwLabelInit(new, &(nself->chart.label), nself->core.background_pixel,
		  nself->core.depth);
    _XpwThreeDInit(new, &nself->chart.threeD, nself->core.background_pixel);
    _XpwLabelDefaultSize(new, &(nself->chart.label), &width, &height);
    nself->chart.label_height = height;
    width += s;
    height += s;
    if (width < 100)
	width = 100;
    if (height < 100)
	height = 100;
    if (nself->core.width == 0)
	nself->core.width = width;
    if (nself->core.height == 0)
	nself->core.height = height;
    CreateGCs(new);
    for (i = 0; i < (sizeof(nself->chart.data) / sizeof(double));

	 nself->chart.data[i++] = 0.0) ;
    nself->chart.nextpoint = 0;
    nself->chart.scale = nself->chart.minScale;
    nself->chart.maxvalue = 0.0;
    if (nself->chart.interval > 0)
	nself->chart.timer = XtAppAddTimeOut(context,
					 nself->chart.interval * 1000,
				         GetData, (XtPointer) nself);
    else
	nself->chart.timer = (XtIntervalId) NULL;
}

/*
 * Calculate preferred size, given constraining box, caching it in the widget.
 */

static              XtGeometryResult
QueryGeometry(w, intended, return_val)
	Widget              w;
	XtWidgetGeometry   *intended, *return_val;
{
    ChartWidget         self = (ChartWidget) w;
    Dimension           width, height;
    XtGeometryResult    ret_val = XtGeometryYes;
    XtGeometryMask      mode = intended->request_mode;

    _XpwLabelDefaultSize(w, &(self->chart.label), &width, &height);
    width += 2 * self->chart.threeD.shadow_width;
    height += 2 * self->chart.threeD.shadow_width;
    height += 4 * self->chart.minScale;

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
    ChartWidget         self = (ChartWidget) new;
    ChartWidget         old_self = (ChartWidget) current;
    Boolean             ret_val = FALSE;

    if (_XpwLabelSetValues(current, new, &(old_self->chart.label),
			&(self->chart.label), new->core.background_pixel,
			   new->core.depth)) {
	Dimension           height, width;

	_XpwThreeDDestroyShadow(current, &old_self->chart.threeD);
	_XpwThreeDAllocateShadowGC(new, &self->chart.threeD,
				   new->core.background_pixel);
	_XpwLabelDefaultSize(new, &(self->chart.label), &width, &height);
	self->chart.label_height = height;
	ret_val = TRUE;
    }
    if (self->core.sensitive != old_self->core.sensitive)
	ret_val = TRUE;
    if (_XpmThreeDSetValues(new, &old_self->chart.threeD,
		      &self->chart.threeD, new->core.background_pixel)) {
	ret_val = TRUE;
    }
    if (self->chart.minScale != old_self->chart.minScale)
	ret_val = TRUE;
    if ((self->chart.data_color != old_self->chart.data_color) ||
	(self->chart.scale_color != old_self->chart.scale_color)) {
	DestroyGCs(current);
	CreateGCs(new);
	ret_val = TRUE;
    }
    if (self->chart.interval != old_self->chart.interval) {
	XtAppContext        context = XtWidgetToApplicationContext(new);

	if (old_self->chart.interval > 0)
	    XtRemoveTimeOut(old_self->chart.timer);
	if (self->chart.interval > 0)
	    self->chart.timer = XtAppAddTimeOut(context,
					      self->chart.interval * 1000,
					      GetData, (XtPointer) self);
	else
	    self->chart.timer = (XtIntervalId) NULL;
    }
    return (ret_val);
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
    ChartWidget         self = (ChartWidget) wid;
    Display            *dpy = XtDisplayOfObject(wid);
    Window              win = XtWindowOfObject(wid);
    Dimension           s = self->chart.threeD.shadow_width;
    Dimension           h = self->core.height;
    Dimension           w = self->core.width;

    if (region == NULL)
	XClearWindow(dpy, win);

   /* Draw the label */
    _XpwLabelDraw(wid, &(self->chart.label), event, region, s, s,
		  w - (2 * s), self->chart.label_height - (2 * s), 0);

   /* Draw the chart */
    RedrawChart(self);

   /* Draw shadows around main window */
    _XpwThreeDDrawShadow(wid, event, region, &(self->chart.threeD), 0, 0, w, h,
			 TRUE);
}

/*
 * Destroy an resources we allocated.
 */
static void
Destroy(w)
	Widget              w;
{
    ChartWidget         self = (ChartWidget) w;

    if (self->chart.interval > 0)
	XtRemoveTimeOut(self->chart.timer);
    _XpwLabelDestroy(w, &(self->chart.label));
    _XpwThreeDDestroyShadow(w, &(self->chart.threeD));
    DestroyGCs(w);
}

/************************************************************
 *
 * Private Functions.
 *
 ************************************************************/

/*
 * Call the application to get next data point.
 */
static void
GetData(client_data, timerid)
	XtPointer           client_data;
	XtIntervalId       *timerid;
{
    ChartWidget         self = (ChartWidget) client_data;
    XtAppContext        context = XtWidgetToApplicationContext((Widget) self);
    double              value;

   /* Install new timer */
    if (self->chart.interval > 0)
	self->chart.timer = XtAppAddTimeOut(context, self->chart.interval * 1000,
					    GetData, (XtPointer) self);
    else
	self->chart.timer = (XtIntervalId) NULL;

    if (self->chart.getData == NULL)
	return;
    XtCallCallbackList((Widget) self, self->chart.getData, (XtPointer) & value);
    AddData(self, value);

}

/*
 * Add a datapoint to the array and update display.
 */
static void
AddData(self, value)
	ChartWidget         self;
	double              value;
{
    Display            *dpy = XtDisplayOfObject((Widget) self);
    Window              win = XtWindowOfObject((Widget) self);
    Dimension           s = self->chart.threeD.shadow_width;
    Dimension           ch = self->core.height;
    Dimension           lh = self->chart.label_height;
    Dimension           h = ch - lh - 2 * s;
    Dimension           w = self->core.width - 2 * s;
    int                 x,y, scale;

    if (self->chart.nextpoint >= w)
	ScrollData(self);
   /* Update maxvalue */
    self->chart.data[self->chart.nextpoint++] = value;
    if (value > self->chart.maxvalue) {
	self->chart.maxvalue = value;
	if (self->chart.maxvalue > self->chart.scale) {
	    RedrawChart(self);
	    return;
	}
    }
    if (!XtIsRealized((Widget) self))
	return;
    x = self->chart.nextpoint-1;
    y = h - (int) (value * ((float) h / (float) self->chart.scale));
    XClearArea(dpy, win, x + s, s + lh, 1, y, FALSE);
    XFillRectangle(dpy, win, self->chart.data_gc, x + s, lh + y + s, 1, h - y);
    scale = (int) ((float) h / (float) self->chart.scale);
    for (y = scale + s; y < h; y += scale)
	XDrawPoint(dpy, win, self->chart.lines_gc, x + s, lh + y);
    XFlush(dpy);
}

/*
 * Redraw the whole window. Called on Redisplay or scale change.
 */
static void
RedrawChart(self)
	ChartWidget         self;
{
    Display            *dpy = XtDisplayOfObject((Widget) self);
    Window              win = XtWindowOfObject((Widget) self);
    Dimension           s = self->chart.threeD.shadow_width;
    Dimension           ch = self->core.height;
    Dimension           lh = self->chart.label_height;
    Dimension           h = ch - lh - 2 * s;
    Dimension           w = self->core.width - 2 * s;
    int                 x, y;
    float               scale;

    self->chart.scale = ((int) self->chart.maxvalue) + 1;
    if (self->chart.scale < self->chart.minScale)
	self->chart.scale = self->chart.minScale;
    if (!XtIsRealized((Widget) self))
	return;
    XClearArea(dpy, win, s, s + self->chart.label_height, w, h, FALSE);
    scale = (float) h / (float) self->chart.scale;
    for (x = 0; x <= self->chart.nextpoint; x++) {
	if ((y = h - (int) (self->chart.data[x] * scale)) > 0)
	    XFillRectangle(dpy, win, self->chart.data_gc, x + s, 
			lh + y + s, 1, h - y);
    }
    for (y = (int) scale + s; y < h; y += (int) scale)
	XDrawLine(dpy, win, self->chart.lines_gc, s, lh + y, w+1, lh + y);
    XFlush(dpy);
}

/*
 * Move the window right scroll amount. Also recalculate the maxvalue.
 */
static void
ScrollData(self)
	ChartWidget         self;
{
    Display            *dpy = XtDisplayOfObject((Widget) self);
    Window              win = XtWindowOfObject((Widget) self);
    Dimension           s = self->chart.threeD.shadow_width;
    Dimension           lh = self->chart.label_height;
    Dimension           h = self->core.height - lh - 2 * s;
    Dimension           w = self->core.width - 2 * s;
    double             *src, *dst, value;
    float               scale;
    int                 n, x, y;

    dst = &self->chart.data[0];
    src = &self->chart.data[self->chart.nextpoint - w +
			    self->chart.scrollAmount];
    self->chart.maxvalue = 0.0;
    for (n = w - self->chart.scrollAmount; n >= 0; n--) {
	if ((value = *src++) > self->chart.maxvalue)
	    self->chart.maxvalue = value;
	*dst++ = value;
    }
    self->chart.nextpoint = w - self->chart.scrollAmount;
    if (!XtIsRealized((Widget) self))
	return;
   /* If the data height shrunk, redraw it */
    if (self->chart.maxvalue < self->chart.scale) {
	RedrawChart(self);
	return;
    }
   /* Copy the data */
    x = w - self->chart.scrollAmount;
    y = lh + s;
    XCopyArea(dpy, win, win, self->chart.data_gc,
 	      s+self->chart.scrollAmount, y, x+1, h, s, y);
    XClearArea(dpy, win, x+s, y, self->chart.scrollAmount, h, FALSE);
    scale = (float) h / (float) self->chart.scale;
    for (y = (int) scale + s; y < h; y += (int) scale)
	XDrawLine(dpy, win, self->chart.lines_gc, x, lh + y, w+1, lh + y);

}

/*
 * Create GC's we need.
 */
static void
CreateGCs(w)
	Widget              w;
{
    ChartWidget         self = (ChartWidget) w;
    XGCValues           values;
    XtGCMask            mask;

    values.foreground = self->chart.data_color;
    values.background = self->core.background_pixel;
    mask = GCForeground | GCBackground;

    self->chart.data_gc = XtGetGC(w, mask, &values);
    values.foreground = self->chart.scale_color;
    self->chart.lines_gc = XtGetGC(w, mask, &values);
}

/*
 * Remove GC's we don't need.
 */
static void
DestroyGCs(w)
	Widget              w;
{
    ChartWidget         self = (ChartWidget) w;

    XtReleaseGC(w, self->chart.data_gc);
    XtReleaseGC(w, self->chart.lines_gc);
}

/************************************************************
 *
 * ACTION procedures
 *
 ************************************************************/

/* ARGSUSED */
static void
Enter(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    ChartWidget         self = (ChartWidget) w;

    _XpwArmClue(w, self->chart.clue);
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

/************************************************************
 *
 * Public interface functions
 *
 ************************************************************/

/* 
 * Add a data point to the widget.
 */

void
XpwChartAddData(w, value)
	Widget              w;
	double              value;
{
    ChartWidget         self = (ChartWidget) w;

    AddData(self, value);
}
