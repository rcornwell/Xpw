/*
 * PmeLine.c - Draw a line. Parent must set our x,y origin correctly.
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
 * $Log: PmeLine.c,v $
 * Revision 1.1  1997/10/04 05:08:13  rich
 * Initial revision
 *
 *
 */

#ifdef lint
static char         rcsid[] = "$Id: PmeLine.c,v 1.1 1997/10/04 05:08:13 rich Exp rich $";

#endif

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include "XpwInit.h"
#include "PmeLineP.h"

#define offset(field) XtOffsetOf(PmeLineRec, pme_line.field)
static XtResource   resources[] =
{
    {XtNlineWidth, XtCLineWidth, XtRDimension, sizeof(Dimension),
     offset(line_width), XtRImmediate, (XtPointer) 1},
    {XtNstipple, XtCStipple, XtRBitmap, sizeof(Pixmap),
     offset(stipple), XtRImmediate, (XtPointer) XtUnspecifiedPixmap},
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
     offset(foreground), XtRString, XtDefaultForeground},
};

#undef offset

/*
 * Function definitions. 
 */
static void         Initialize(Widget /*request */ , Widget /*new */ ,
				 ArgList /*args */ , Cardinal * /*num_args */ );
static void         Notify(Widget /*w */ );
static char       * GetMenuName(Widget /*w */ );
static void         CreateGC(Widget /*w */ );
static void         Destroy(Widget /*w */ );
static void         Redisplay(Widget /*w */ , XEvent * /*event */ ,
				 Region /*region */ );
static Boolean      SetValues(Widget /*current */ , Widget /*request */ ,
				 Widget /*new */ , ArgList /*args */ ,
				 Cardinal * /*num_args */ );
static XtGeometryResult QueryGeometry(Widget /*w */ ,
				 XtWidgetGeometry * /*intended */ ,
				 XtWidgetGeometry * /*return_val */ );

#define SUPERCLASS (&rectObjClassRec)

PmeLineClassRec     pmeLineClassRec =
{
    {
	(WidgetClass) SUPERCLASS,	/* superclass            */
	"PmeLine",			/* class_name            */
	sizeof(PmeLineRec),		/* size                  */
	XpwInitializeWidgetSet,		/* class_initialize      */
	NULL,				/* class_part_initialize */
	FALSE,				/* Class init'ed         */
	Initialize,			/* initialize            */
	NULL,				/* initialize_hook       */
	NULL,				/* realize               */
	NULL,				/* actions               */
	0,				/* num_actions           */
	resources,			/* resources             */
	XtNumber(resources),		/* resource_count        */
	NULLQUARK,			/* xrm_class             */
	FALSE,				/* compress_motion       */
	FALSE,				/* compress_exposure     */
	FALSE,				/* compress_enterleave   */
	FALSE,				/* visible_interest      */
	Destroy,			/* destroy               */
	NULL,				/* resize                */
	Redisplay,			/* expose                */
	SetValues,			/* set_values            */
	NULL,				/* set_values_hook       */
	XtInheritSetValuesAlmost,	/* set_values_almost     */
	NULL,				/* get_values_hook       */
	NULL,				/* accept_focus          */
	XtVersion,			/* intrinsics version    */
	NULL,				/* callback offsets      */
	NULL,				/* tm_table              */
	QueryGeometry,			/* query_geometry        */
	NULL,				/* display_accelerator   */
	NULL				/* extension             */
    },
    { /* PmeLine Fields */
	Notify,				/* notify                */
        Notify,                         /* highlight             */
        Notify,                         /* unhighlight           */
        GetMenuName,                    /* getmenuname           */
	NULL				/* extension             */
    }
};

WidgetClass         pmeLineObjectClass = (WidgetClass) & pmeLineClassRec;

/************************************************************
 *
 * Semi-Public Functions.
 *
 ************************************************************/

/* 
 * Initialize the Widget.
 */

/* ARGSUSED */
static void
Initialize(request, new, args, num_args)
	Widget              request, new;
	ArgList             args;
	Cardinal           *num_args;
{
    PmeLineObject       self = (PmeLineObject) new;

    self->rectangle.border_width = 0;
    if (self->rectangle.height == 0)
	self->rectangle.height = self->pme_line.line_width;

    CreateGC(new);
}

/* 
 * Set widget resources.
 */

/* ARGSUSED */
static              Boolean
SetValues(current, request, new, args, num_args)
	Widget              current, request, new;
	ArgList             args;
	Cardinal           *num_args;
{
    PmeLineObject       self = (PmeLineObject) new;
    PmeLineObject       old_self = (PmeLineObject) current;

    if ((self->pme_line.line_width != old_self->pme_line.line_width) &&
	(self->pme_line.stipple != old_self->pme_line.stipple)) {
	Destroy(current);
	CreateGC(new);
	return (TRUE);
    }
    return (FALSE);
}

/*
 * Return widgets prefered size.
 * Just return the height and a width of 1.
 */

static              XtGeometryResult
QueryGeometry(w, intended, return_val)
	Widget              w;
	XtWidgetGeometry   *intended, *return_val;
{
    PmeLineObject       self = (PmeLineObject) w;
    Dimension           width;
    XtGeometryResult    ret_val = XtGeometryYes;
    XtGeometryMask      mode = intended->request_mode;

    width = 1;			/* we can be really small. */

    if (((mode & CWWidth) && (intended->width != width)) ||
	!(mode & CWWidth)) {
	return_val->request_mode |= CWWidth;
	return_val->width = width;
	mode = return_val->request_mode;

	if ((mode & CWWidth) && (width == self->rectangle.width))
	    return (XtGeometryNo);
	return (XtGeometryAlmost);
    }
    return (ret_val);
}

/*
 * Create GCs for Widget.
 */

static void
CreateGC(w)
	Widget              w;
{
    PmeLineObject       self = (PmeLineObject) w;
    XGCValues           values;
    XtGCMask            mask = GCForeground | GCGraphicsExposures | GCLineWidth;

    values.foreground = self->pme_line.foreground;
    values.graphics_exposures = FALSE;
    values.line_width = self->pme_line.line_width;

    if (self->pme_line.stipple != XtUnspecifiedPixmap) {
	values.stipple = self->pme_line.stipple;
	values.fill_style = FillStippled;
	mask |= GCStipple | GCFillStyle;

	self->pme_line.gc = XCreateGC(XtDisplayOfObject(w),
				 RootWindowOfScreen(XtScreenOfObject(w)),
				      mask, &values);
    } else
	self->pme_line.gc = XtGetGC(w, mask, &values);
}

/* 
 * Redisplay the widget.
 */

/*ARGSUSED */
static void
Redisplay(w, event, region)
	Widget              w;
	XEvent             *event;
	Region              region;
{
    PmeLineObject       self = (PmeLineObject) w;
    int                 y = self->rectangle.y +
    (int) (self->rectangle.height - self->pme_line.line_width) / 2;

    if (self->pme_line.stipple != XtUnspecifiedPixmap)
	XSetTSOrigin(XtDisplayOfObject(w), self->pme_line.gc, 0, y);

    XFillRectangle(XtDisplayOfObject(w), XtWindowOfObject(w),
		   self->pme_line.gc,
		   self->rectangle.x, y,
		   (unsigned int) self->rectangle.width,
		   (unsigned int) self->pme_line.line_width);
}

/*
 * Notify callbacks, this is a place holder for PmeEntry.
 */
static void
Notify(w)
	Widget              w;
{
}

/*
 * Notify callbacks, this is a place holder for PmeEntry.
 */
static char *
GetMenuName(w)
	Widget              w;
{
	return NULL;
}


/*     
 *   Destroy GC's we allocated.
 */

static void
Destroy(w)
	Widget              w;
{
    PmeLineObject       self = (PmeLineObject) w;

    if (self->pme_line.stipple != XtUnspecifiedPixmap)
	XFreeGC(XtDisplayOfObject(w), self->pme_line.gc);
    else
	XtReleaseGC(w, self->pme_line.gc);
}


