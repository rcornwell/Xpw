/*
 * PmeEntry.c 
 *
 *   Popup Menu Entry, displays a label on a popup menu. Includes popup clues,
 *   and cascaded menus.
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
 * $Log: PmeEntry.c,v $
 * Revision 1.4  1997/11/01 06:39:05  rich
 * Removed unused variables.
 *
 * Revision 1.3  1997/10/05 02:25:17  rich
 * Make sure ident line is in object file.
 *
 * Revision 1.2  1997/10/05 02:17:28  rich
 * Moved clue handling into widget.
 * Widget also can return menu_name.
 *
 * Revision 1.1  1997/10/04 05:07:44  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char         rcsid[] = "$Id: PmeEntry.c,v 1.4 1997/11/01 06:39:05 rich Beta $";

#endif

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xos.h>

#include <X11/Xmu/Drawing.h>

#include "XpwInit.h"
#include "PopupMenu.h"
#include "PmeEntryP.h"
#include "Clue.h"

#include <stdio.h>

#define ONE_HUNDRED 100

#define offset(field) XtOffsetOf(PmeEntryRec, pme_entry.field)

static XtResource   resources[] =
{
    LabelResources

    /* Pme Entry Resources */
    {XtNcallback, XtCCallback, XtRCallback, sizeof(XtPointer),
     offset(callbacks), XtRCallback, (XtPointer) NULL},
    {XtNmenuName, XtCMenuName, XtRString, sizeof(String),
     offset(menu_name), XtRString, NULL},
    {XtNclue, XtCLabel, XtRString, sizeof(String),
     offset(clue), XtRImmediate, (XtPointer) NULL}
};

#undef offset

/*
 * Semi Public function definitions. 
 */
static void         Initialize(Widget /*request */ , Widget /*new */ ,
				 ArgList /*args */ , Cardinal * /*num_args */ );
static void         Destroy(Widget /*w */ );
static void         Redisplay(Widget /*w */ , XEvent * /*event */ ,
					 Region /*region */ );
static Boolean      SetValues(Widget /*current */ , Widget /*request */ ,
					 Widget /*new */ , ArgList /*args */ ,
					 Cardinal * /*num_args */ );
static XtGeometryResult QueryGeometry(Widget /*w */ ,
					 XtWidgetGeometry * /*intended */ ,
					 XtWidgetGeometry * /*return_val */ );

/* Action Procs */
static void         Notify(Widget /*w */ );
static void         Highlight(Widget /*w */ );
static void         Unhighlight(Widget /*w */ );
static char	*   GetMenuName(Widget /*w */ );

#define superclass (&rectObjClassRec)
PmeEntryClassRec    pmeEntryClassRec =
{
    {
	(WidgetClass) superclass,	/* superclass            */
	"PmeEntry",			/* class_name            */
	sizeof(PmeEntryRec),		/* size                  */
	XpwInitializeWidgetSet,		/* class_initializer     */
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
    {
     /* PmeEntryClass Fields */
	Notify,				/* notify                */
    	Highlight,			/* highlight		 */
        Unhighlight,			/* unhighlight		 */
    	GetMenuName,			/* getmenuname		 */
	NULL				/* extension             */
    }
};

WidgetClass         pmeEntryObjectClass = (WidgetClass) & pmeEntryClassRec;

/************************************************************
 *
 * Semi-Public Functions.
 *
 ************************************************************/

/*
 *  Initialize the Widget;
 */

/* ARGSUSED */
static void
Initialize(request, new, args, num_args)
	Widget              request, new;
	ArgList             args;
	Cardinal           *num_args;
{
    PmeEntryObject      self = (PmeEntryObject) new;

    _XpwLabelInit(new, &(self->pme_entry.label),
		  XtParent(new)->core.background_pixel,
		  XtParent(new)->core.depth);
    _XpwLabelDefaultSize(new, &(self->pme_entry.label),
		    &(self->rectangle.width), &(self->rectangle.height));
}

/* 
 * SetValues widget resources.
 */

/* ARGSUSED */
static              Boolean
SetValues(current, request, new, args, num_args)
	Widget              current, request, new;
	ArgList             args;
	Cardinal           *num_args;
{
    PmeEntryObject      self = (PmeEntryObject) new;
    PmeEntryObject      old_self = (PmeEntryObject) current;
    Boolean             ret_val = FALSE;

    if (_XpwLabelSetValues(current, new, &(old_self->pme_entry.label),
			   &(self->pme_entry.label),
			   new->core.background_pixel, new->core.depth))
	ret_val = TRUE;
    if (self->rectangle.sensitive != old_self->rectangle.sensitive)
	ret_val = TRUE;

    if (ret_val) {
	_XpwLabelDefaultSize(new, &(self->pme_entry.label),
				 &(self->rectangle.width),
			     &(self->rectangle.height));
    }
    return (ret_val);
}

/* 
 * Return Prefered widget size.
 *
 * Just return the height and width of the label plus the margins.
 */

static              XtGeometryResult
QueryGeometry(w, intended, return_val)
	Widget              w;
	XtWidgetGeometry   *intended, *return_val;
{
    PmeEntryObject      self = (PmeEntryObject) w;
    Dimension           width, height;
    XtGeometryResult    ret_val = XtGeometryYes;
    XtGeometryMask      mode = intended->request_mode;

    _XpwLabelDefaultSize(w, &(self->pme_entry.label), &width, &height);

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

	if (((mode & CWWidth) && (width == self->rectangle.width)) &&
	    ((mode & CWHeight) && (height == self->rectangle.height)))
	    return (XtGeometryNo);
    }
    return (ret_val);
}

/* 
 * Redisplay the widget.
 */

/* ARGSUSED */
static void
Redisplay(w, event, region)
	Widget              w;
	XEvent             *event;
	Region              region;
{
    PmeEntryObject      self = (PmeEntryObject) w;

    _XpwLabelDraw(w, &self->pme_entry.label, event, region,
		  self->rectangle.x, self->rectangle.y,
		  self->rectangle.width, self->rectangle.height, FALSE);
}

/* 
 *  Destroy resources used.
 */

static void
Destroy(w)
	Widget              w;
{
    PmeEntryObject      self = (PmeEntryObject) w;

    _XpwLabelDestroy(w, &(self->pme_entry.label));
}

/************************************************************
 *
 * ACTION procedures.
 *
 ************************************************************/


/*
 * Preform callbacks.
 */
static void
Notify(w)
	Widget              w;
{
    XtCallCallbacks(w, XtNcallback, (XtPointer) NULL);
}

/*
 * Trigger Clue popup if we have any.
 */
static void
Highlight(w)
	Widget		    w;
{
    PmeEntryObject      self = (PmeEntryObject) w;

    _XpwArmClue(w, self->pme_entry.clue);
}

/*
 * Disarm any clue Popops we may have.
 */
static void
Unhighlight(w)
        Widget              w;
{
    _XpwDisArmClue(w);
}

/*
 * Disarm any clue Popops we may have.
 */
static char *
GetMenuName(w)
        Widget              w;
{
    PmeEntryObject      self = (PmeEntryObject) w;

    return self->pme_entry.menu_name;
}
