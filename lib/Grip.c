/*
 * Grip widget.
 *
 * Draw a box and do callbacks.
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
 * $Log: Grip.c,v $
 * Revision 1.1  1997/10/12 05:18:35  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: Grip.c,v 1.1 1997/10/12 05:18:35 rich Exp rich $";

#endif

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Misc.h>
#include <X11/Shell.h>
#include <X11/CoreP.h>
#include "XpwInit.h"
#include "GripP.h"
#include "Clue.h"

/* Semi Public Functions */

/* Grip.c */
static void         Initialize(Widget /*request */ , Widget /*new */ ,
			  ArgList /*args */ , Cardinal * /*num_args */ );
static Boolean      SetValues(Widget /*current */ , Widget /*request */ ,
			      Widget /*new */ , ArgList /*args */ ,
			      Cardinal * /*num_args */ );
static void         Destroy(Widget /*w */ );
static void         Redisplay(Widget /*wid */ , XEvent * /*event */ ,
			      Region /*region */ );

/* Our resources */
#define coffset(field) XtOffsetOf(GripRec, field)
#define offset(field) XtOffsetOf(GripRec, grip.field)
static XtResource   resources[] =
{
    /* Grip resources */
    {XtNwidth, XtCWidth, XtRDimension, sizeof(Dimension),
     coffset(core.width), XtRImmediate, (XtPointer) DEFAULT_GRIP_SIZE},
    {XtNheight, XtCHeight, XtRDimension, sizeof(Dimension),
     coffset(core.height), XtRImmediate, (XtPointer) DEFAULT_GRIP_SIZE},
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
     coffset(core.background_pixel), XtRString, XtDefaultBackground},
    {XtNborderWidth, XtCBorderWidth, XtRDimension, sizeof(Dimension),
     coffset(core.border_width), XtRImmediate, (XtPointer) 0},
    {XtNcallback, XtCCallback, XtRCallback, sizeof(XtPointer),
     offset(grip_action), XtRCallback, NULL},
    {XtNclue, XtCLabel, XtRString, sizeof(String),
     offset(clue), XtRImmediate, (XtPointer) NULL},

    /* ThreeD resouces */
    threeDresources
};

/* Actions */
static void         GripAction(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         Enter(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         Leave(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );

static XtActionsRec actionsList[] =
{
    {"GripAction", GripAction},
    {"Enter", Enter},
    {"Leave", Leave},
};

#define SuperClass ((WidgetClass) &widgetClassRec)

GripClassRec        gripClassRec =
{
    {	/* core_class part */
	(WidgetClass) SuperClass,	/* superclass            */
	"Grip",				/* class_name            */
	sizeof(GripRec),		/* widget_size           */
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
	NULL,				/* tm_table              */
	XtInheritQueryGeometry,		/* query_geometry        */
	XtInheritDisplayAccelerator,	/* display_acceleator    */
	NULL				/* extension             */
    },
    {	/* grip_class part */
	0,
    },
};

WidgetClass         gripWidgetClass = (WidgetClass) & gripClassRec;

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
    GripWidget          nself = (GripWidget) new;

    _XpwThreeDInit(new, &nself->grip.threeD, nself->core.background_pixel);
    nself->core.width += 2 * nself->grip.threeD.shadow_width;
    nself->core.height += 2 * nself->grip.threeD.shadow_width;

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
    GripWidget          self = (GripWidget) new;
    GripWidget          old_self = (GripWidget) current;
    Boolean             ret_val = FALSE;

    if (self->core.sensitive != old_self->core.sensitive)
	ret_val = TRUE;
    if (_XpmThreeDSetValues(new, &old_self->grip.threeD,
			 &self->grip.threeD, new->core.background_pixel))
	ret_val = TRUE;
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
    GripWidget          self = (GripWidget) wid;
    Display            *dpy = XtDisplayOfObject(wid);
    Window              win = XtWindowOfObject(wid);

    XClearWindow(dpy, win);

   /* Draw shadows around main window */
    _XpwThreeDDrawShadow(wid, event, region, &(self->grip.threeD), 0, 0,
		 self->core.width, self->core.height, 0);
}

/*
 * Destroy an resources we allocated.
 */
static void
Destroy(w)
	Widget              w;
{
    GripWidget          self = (GripWidget) w;

    _XpwThreeDDestroyShadow(w, &(self->grip.threeD));
}

/************************************************************
 *
 * ACTION procedures
 *
 ************************************************************/

/* ARGSUSED */
static void
GripAction(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    XpwGripCallDataRec  call_data;

    call_data.event = event;
    call_data.params = params;
    call_data.num_params = *num_params;

    XtCallCallbacks(w, XtNcallback, (XtPointer) & call_data);
}

/* ARGSUSED */
static void
Enter(w, event, params, num_params)
        Widget              w;
        XEvent             *event;
        String             *params;     /* unused */
        Cardinal           *num_params;         /* unused */
{
    GripWidget         self = (GripWidget) w;

    _XpwArmClue(w, self->grip.clue);
}

/* ARGSUSED */
static void
Leave(w, event, params, num_params)
        Widget              w;
        XEvent             *event;
        String             *params;     /* unused */
        Cardinal           *num_params;         /* unused */
{
    _XpwDisArmClue(w);
}

