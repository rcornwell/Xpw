/*
 * Label widget.
 *
 * Draw a label.
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
 * $Log: Label.c,v $
 * Revision 1.1  1997/10/04 05:04:39  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: Label.c,v 1.1 1997/10/04 05:04:39 rich Beta $";

#endif

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Misc.h>
#include <X11/Shell.h>
#include <X11/CoreP.h>
#include "XpwInit.h"
#include "LabelP.h"
#include "Clue.h"

/* Semi Public Functions */

/* Label.c */
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
#define offset(field) XtOffsetOf(LabelRec, label.field)
static XtResource   resources[] =
{
    /* Label resources */
    LabelResources
    /* ThreeD resouces */
    threeDresources
    {XtNclue, XtCLabel, XtRString, sizeof(String),
     offset(clue), XtRImmediate, (XtPointer) NULL}
};

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

LabelClassRec       labelClassRec =
{
    {		/* core_class part */
	(WidgetClass) SuperClass,	/* superclass            */
	"Label",			/* class_name            */
	sizeof(LabelRec),		/* widget_size           */
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
    {				/* label_class part */
	0,
    },
};

WidgetClass         labelWidgetClass = (WidgetClass) & labelClassRec;

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
    LabelWidget         nself = (LabelWidget) new;

    _XpwLabelInit(new, &(nself->label.label), nself->core.background_pixel,
		  nself->core.depth);
    _XpwThreeDInit(new, &nself->label.threeD, nself->core.background_pixel);
    _XpwLabelDefaultSize(new, &(nself->label.label),
			 &(nself->core.width), &(nself->core.height));
    nself->core.width += 2 * nself->label.threeD.shadow_width;
    nself->core.height += 2 * nself->label.threeD.shadow_width;

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
    LabelWidget         self = (LabelWidget) new;
    LabelWidget         old_self = (LabelWidget) current;
    Boolean             ret_val = FALSE;

    if (_XpwLabelSetValues(current, new, &(old_self->label.label),
			&(self->label.label), new->core.background_pixel,
			   new->core.depth)) {
	_XpwThreeDDestroyShadow(current, &old_self->label.threeD);
	_XpwThreeDAllocateShadowGC(new, &self->label.threeD,
				   new->core.background_pixel);
	ret_val = TRUE;
    }
    if (self->core.sensitive != old_self->core.sensitive)
	ret_val = TRUE;
    if (_XpmThreeDSetValues(new, &old_self->label.threeD,
		      &self->label.threeD, new->core.background_pixel)) 
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
    LabelWidget         self = (LabelWidget) w;
    Dimension           width, height;
    XtGeometryResult    ret_val = XtGeometryYes;
    XtGeometryMask      mode = intended->request_mode;

    _XpwLabelDefaultSize(w, &(self->label.label), &width, &height);
    width += 2 * self->label.threeD.shadow_width;
    height += 2 * self->label.threeD.shadow_width;

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
    LabelWidget         self = (LabelWidget) wid;
    Display            *dpy = XtDisplayOfObject(wid);
    Window              win = XtWindowOfObject(wid);
    Dimension           s = self->label.threeD.shadow_width;
    Dimension           h = self->core.height;
    Dimension           w = self->core.width;

    if(region == NULL)
        XClearWindow(dpy, win); 

   /* Draw shadows around main window */
    _XpwThreeDDrawShadow(wid, event, region, &(self->label.threeD), 0, 0, w, h,
			 0);

   /* Draw the label */
    _XpwLabelDraw(wid, &(self->label.label), event, region, s, s,
		  w - (2 * s), h - (2 * s), FALSE);
}

/*
 * Destroy an resources we allocated.
 */
static void
Destroy(w)
	Widget              w;
{
    LabelWidget         self = (LabelWidget) w;

    _XpwLabelDestroy(w, &(self->label.label));
    _XpwThreeDDestroyShadow(w, &(self->label.threeD));
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
    LabelWidget         self = (LabelWidget) w;

    _XpwArmClue(w, self->label.clue);
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
