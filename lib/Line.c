/*
 * Line widget.
 *
 * Draw a line.
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
 * $Log: Line.c,v $
 * Revision 1.1  1997/10/04 05:05:03  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: Line.c,v 1.1 1997/10/04 05:05:03 rich Exp rich $";

#endif

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Misc.h>
#include <X11/Shell.h>
#include <X11/CoreP.h>
#include "XpwInit.h"
#include "LineP.h"

/* Semi Public Functions */

/* Line.c */
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
#define offset(field) XtOffsetOf(LineRec, line.field)
static XtResource   resources[] =
{
    /* ThreeD resouces */
    threeDresources
};

#define SuperClass ((WidgetClass) &widgetClassRec)

LineClassRec       lineClassRec =
{
    {	/* core_class part */
	(WidgetClass) SuperClass,	/* superclass            */
	"Line",				/* class_name            */
	sizeof(LineRec),		/* widget_size           */
	XpwInitializeWidgetSet,		/* class_initialize      */
	NULL,				/* class_part_initialize */
	FALSE,				/* class_inited          */
	Initialize,			/* initialize            */
	NULL,				/* initialize_hook       */
	XtInheritRealize,		/* realize               */
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
	NULL,				/* resize                */
	Redisplay,			/* expose                */
	SetValues,			/* set_values            */
	NULL,				/* set_values_hook       */
	XtInheritSetValuesAlmost,	/* set_values_almost     */
	NULL,				/* get_values+hook       */
	XtInheritAcceptFocus,		/* accept_focus          */
	XtVersion,			/* version               */
	NULL,				/* callback_private      */
	XtInheritTranslations,		/* tm_table              */
	QueryGeometry,			/* query_geometry        */
	XtInheritDisplayAccelerator,	/* display_acceleator    */
	NULL				/* extension             */
    },
    {	/* line_class part */
	0,
    },
};

WidgetClass         lineWidgetClass = (WidgetClass) & lineClassRec;

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
    LineWidget         nself = (LineWidget) new;

    _XpwThreeDInit(new, &nself->line.threeD, nself->core.background_pixel);
    nself->core.width = 2 * nself->line.threeD.shadow_width;
    nself->core.height = 2 * nself->line.threeD.shadow_width;
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
    LineWidget         self = (LineWidget) new;
    LineWidget         old_self = (LineWidget) current;

    return (_XpmThreeDSetValues(new, &old_self->line.threeD,
		      &self->line.threeD, new->core.background_pixel));
}

/*
 * Calculate preferred size, given constraining box, caching it in the widget.
 */

static              XtGeometryResult
QueryGeometry(w, intended, return_val)
	Widget              w;
	XtWidgetGeometry   *intended, *return_val;
{
    LineWidget         self = (LineWidget) w;
    Dimension           width, height;
    XtGeometryResult    ret_val = XtGeometryYes;
    XtGeometryMask      mode = intended->request_mode;

    width = 2 * self->line.threeD.shadow_width;
    height = 2 * self->line.threeD.shadow_width;

    if (intended->width > intended->height) {
    	if (((mode & CWWidth) && (intended->width < width)) ||
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
    } else {	
        if (((mode & CWWidth) && (intended->width != width)) ||
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
    }
    if (ret_val == XtGeometryAlmost) {
	mode = return_val->request_mode;

	if (((mode & CWWidth) && (width > self->core.width)) &&
	    ((mode & CWHeight) && (height > self->core.height))) 
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
    LineWidget         self = (LineWidget) wid;

   /* Draw shadows around main window */
    _XpwThreeDDrawShadow(wid, event, region, &(self->line.threeD), 0, 0, 
			self->core.width, self->core.height, TRUE);
}

/*
 * Destroy an resources we allocated.
 */
static void
Destroy(w)
	Widget              w;
{
    LineWidget         self = (LineWidget) w;

    _XpwThreeDDestroyShadow(w, &(self->line.threeD));
}


