/*
 * Led widget.
 *
 * Draw a led.
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
 * $Log: Led.c,v $
 * Revision 1.1  1997/10/04 05:04:39  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: Led.c,v 1.1 1997/10/04 05:04:39 rich Exp $";

#endif

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Misc.h>
#include <X11/Shell.h>
#include <X11/CoreP.h>
#include "XpwInit.h"
#include "LedP.h"
#include "Clue.h"

/* Semi Public Functions */

/* Led.c */
static XtGeometryResult QueryGeometry(Widget /*w */ ,
				 XtWidgetGeometry * /*intended */ ,
				 XtWidgetGeometry * /*return_val */ );
static void         Initialize(Widget /*request */ , Widget /*new */ ,
				 ArgList /*args */ , Cardinal * /*num_args */ );
static Boolean      SetValues(Widget /*current */ , Widget /*request */ ,
				 Widget /*new */ , ArgList /*args */ ,
				 Cardinal * /*num_args */ );
static void         Destroy(Widget /*w */ );
static void         Redisplay(Widget /*wid */ , XEvent * /*event */ ,
				 Region /*region */ );

/* Our resources */
#define offset(field) XtOffsetOf(LedRec, led.field)
static XtResource   resources[] =
{
    /* Led resources */
    {XtNlabel, XtCLabel, XtRString, sizeof(String),
     labelOffset(label), XtRString, NULL},
    {XtNformat, XtCFormat, XtRString, sizeof(String),
     labelOffset(label), XtRString, "D.D.:D.D."},
    {XtNjustify, XtCJustify, XtRJustify, sizeof(XtJustify),
     labelOffset(justify), XtRImmediate, (XtPointer) XtJustifyLeft},
    {XtNleftMargin, XtCHorizontalMargins, XtRDimension, sizeof(Dimension),
     labelOffset(left_margin), XtRImmediate, (XtPointer) 4},
    {XtNrightMargin, XtCHorizontalMargins, XtRDimension, sizeof(Dimension),
     labelOffset(right_margin), XtRImmediate, (XtPointer) 4},
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
     labelOffset(foreground), XtRString, XtDefaultForeground}, 
    {XtNoffcolor, XtCOffColor, XtRPixel, sizeof(Pixel),
     labelOffset(foreground), XtRString, XtDefaultBackground},
    {XtNblinkRate, XtCBlinkRate, XtRInt, sizeof(Int),
     labelOffset(blinkRate), XtRImmediate, (XtPointer) 0},
    /* ThreeD resouces */
    threeDresources
    {XtNclue, XtCLed, XtRString, sizeof(String),
     offset(clue), XtRImmediate, (XtPointer) NULL}
};

/*	segments */

/* A = */
	6,19
	7,18
	12,18
	13,19
	12,20
	6,20
/* B = */
	10,11
	11,12
	13,17
	12,18
	11,17
	 9,11
/* C = */
	8,2
	9,3
	11,8
	10,9
	9,8
	7,3
/* D = */
	1,1
	2,0
	7,0
	8,1
	7,2
	2,2
/* E = */
	1,2
	2,3
	4,8
	3,9
	2,8
	0,1
/* F = */
	4,11
	5,12
	7,17
	6,18
	5,17
	3,11
/* G = */
	3,10
	4,9
	9,9
	10,10
	9,11
	4,11


/* Actions */
static void         Enter(Widget /*w */, XEvent * /*event */,
				 String * /*params */, Cardinal * /*num_params */);
static void         Leave(Widget /*w */, XEvent * /*event */,
				 String * /*params */, Cardinal * /*num_params */);

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

LedClassRec       ledClassRec =
{
    {		/* core_class part */
	(WidgetClass) SuperClass,	/* superclass            */
	"Led",			/* class_name            */
	sizeof(LedRec),		/* widget_size           */
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
    {				/* led_class part */
	0,
    },
};

WidgetClass         ledWidgetClass = (WidgetClass) & ledClassRec;

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
    LedWidget         nself = (LedWidget) new;

    _XpwLedInit(new, &(nself->led.led), nself->core.background_pixel,
		  nself->core.depth);
    _XpwThreeDInit(new, &nself->led.threeD, nself->core.background_pixel);
    _XpwLedDefaultSize(new, &(nself->led.led),
			 &(nself->core.width), &(nself->core.height));
    nself->core.width += 2 * nself->led.threeD.shadow_width;
    nself->core.height += 2 * nself->led.threeD.shadow_width;

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
    LedWidget         self = (LedWidget) new;
    LedWidget         old_self = (LedWidget) current;
    Boolean             ret_val = FALSE;

    if (_XpwLedSetValues(current, new, &(old_self->led.led),
			&(self->led.led), new->core.background_pixel,
			   new->core.depth)) {
	_XpwThreeDDestroyShadow(current, &old_self->led.threeD);
	_XpwThreeDAllocateShadowGC(new, &self->led.threeD,
				   new->core.background_pixel);
	ret_val = TRUE;
    }
    if (self->core.sensitive != old_self->core.sensitive)
	ret_val = TRUE;
    if (_XpmThreeDSetValues(new, &old_self->led.threeD,
		      &self->led.threeD, new->core.background_pixel)) 
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
    LedWidget         self = (LedWidget) w;
    Dimension           width, height;
    XtGeometryResult    ret_val = XtGeometryYes;
    XtGeometryMask      mode = intended->request_mode;

    _XpwLedDefaultSize(w, &(self->led.led), &width, &height);
    width += 2 * self->led.threeD.shadow_width;
    height += 2 * self->led.threeD.shadow_width;

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
 * Redisplay the parent and all children.
 */

static void
Redisplay(wid, event, region)
	Widget              wid;
	XEvent             *event;
	Region              region;
{
    LedWidget         self = (LedWidget) wid;
    Display            *dpy = XtDisplayOfObject(wid);
    Window              win = XtWindowOfObject(wid);
    Dimension           s = self->led.threeD.shadow_width;
    Dimension           h = self->core.height;
    Dimension           w = self->core.width;

    if(region == NULL)
        XClearWindow(dpy, win); 

   /* Draw shadows around main window */
    _XpwThreeDDrawShadow(wid, event, region, &(self->led.threeD), 0, 0, w, h,
			 0);

   /* Draw the led */
    _XpwLedDraw(wid, &(self->led.led), event, region, s, s,
		  w - (2 * s), h - (2 * s), FALSE);
}

/*
 * Destroy an resources we allocated.
 */
static void
Destroy(w)
	Widget              w;
{
    LedWidget         self = (LedWidget) w;

    _XpwLedDestroy(w, &(self->led.led));
    _XpwThreeDDestroyShadow(w, &(self->led.threeD));
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
    LedWidget         self = (LedWidget) w;

    _XpwArmClue(w, self->led.clue);
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
