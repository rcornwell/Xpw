/*
 * Clue Widget.
 *
 * Pops up a clue box after a timeout period. Not realy a free standing
 * widget, more of a place holder. Each widget needs to define a clue 
 * structure in it's private space. There should be one Clue widget defined
 * for each context, if none can be found no popups will occure.
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
 * $Log: Clue.c,v $
 * Revision 1.2  1997/10/05 02:17:28  rich
 * Don't pop up clue unless we realy have one.
 * Make sure that if a clue is disarmed nothing will get shown.
 * Popup clue a bit below the widget.
 *
 * Revision 1.1  1997/10/04 05:02:12  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: Clue.c,v 1.2 1997/10/05 02:17:28 rich Exp rich $";
#endif

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Misc.h>
#include <X11/Shell.h>
#include <X11/CoreP.h>
#include "XpwInit.h"
#include "ClueP.h"

/* Semi Public Functions */
static void         Initialize(Widget /*request */, Widget /*new */,
				 ArgList /*args */, Cardinal * /*num_args */);
static Boolean      SetValues(Widget /*current */, Widget /*request */,
				 Widget /*new */, ArgList /*args */,
				 Cardinal * /*num_args */ );
static void         Destroy(Widget /*w */ );
static void         PopupClue(XtPointer /*client_data */ ,
				 XtIntervalId * /*timerid */ );
static ClueWidget   FindClueWidgetFor(Widget /*w */ );

/* Our resources */
#define offset(field) XtOffsetOf(ClueRec, clue.field)
static XtResource   resources[] =
{
    /* Label resources */
    LabelResources
    /* Popup resources */
    {XtNtimeOut, XtCtimeOut, XtRInt, sizeof(int),
     offset(timeout), XtRImmediate, (XtPointer) 500},
};

#define SuperClass ((WidgetClass) &overrideShellClassRec)

ClueClassRec        clueClassRec =
{
    {	/* core_class part */
	(WidgetClass) SuperClass,	/* superclass            */
	"Clue",				/* class_name            */
	sizeof(ClueRec),		/* widget_size           */
	NULL,				/* class_initialize      */
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
	FALSE,				/* visible_interest      */
	Destroy,			/* destroy               */
	XtInheritResize,		/* resize                */
	XtInheritExpose,		/* expose                */
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
    {
	XtInheritGeometryManager,	/* geometry_manager   */
	XtInheritChangeManaged,		/* change_managed     */
	XtInheritInsertChild,		/* insert_child       */
	XtInheritDeleteChild,		/* delete_child       */
	NULL				/* extension          */
    },
    {	/* Shell extension    */
	 NULL
    },
    {	/* Override extension */
	 NULL
    },
    {	/* clue_class part */
	0,
    },
};

WidgetClass         clueWidgetClass = (WidgetClass) & clueClassRec;

struct clue_list {
    XtAppContext        context;
    Widget              clue_widget;
    struct clue_list   *next;
};

static struct clue_list *cluelist = (struct clue_list *) NULL;

/************************************************************
 *
 * Semi-Public interface functions
 *
 ************************************************************/

/*
 * Initailize Clue Widget.
 */

/* ARGSUSED */
static void
Initialize(request, new, args, num_args)
	Widget              request, new;
	ArgList             args;
	Cardinal           *num_args;
{
    ClueWidget          nself = (ClueWidget) new;
    XtAppContext        context = XtWidgetToApplicationContext(new);
    struct clue_list   *cp, *lcp;

   /* Find out if this is a new or old widget */
    if (cluelist == NULL) {
       /* Nothing in list, must be new */
	cp = cluelist = XtNew(struct clue_list);

	cp->next = (struct clue_list *) NULL;
	cp->context = context;
	cp->clue_widget = new;
    } else {
       /* List not empty, look down it first */
	for (cp = lcp = cluelist; cp != NULL; cp = cp->next)
	    if (cp->context == context)
		goto found;
	    else
		lcp = cp;
       /* Not found, tac on end */
	cp = XtNew(struct clue_list);

	lcp->next = cp;
	cp->next = (struct clue_list *) NULL;
	cp->context = context;
	cp->clue_widget = new;
    }
  found:
   /* If not the same widget, remove the old on */
    if (cp->clue_widget != new) {
	if (cp->clue_widget != NULL)
	    XtDestroyWidget(cp->clue_widget);
	cp->clue_widget = new;
    }
    _XpwLabelInit(new, &(nself->clue.label), nself->core.background_pixel,
		  nself->core.depth);

    nself->clue.timer = (XtIntervalId) NULL;
    nself->clue.showing = FALSE;
}				/* Initialize */

/*
 * SetValues: 
 */

/* ARGSUSED */
static              Boolean
SetValues(current, request, new, args, num_args)
	Widget              current, request, new;
	ArgList             args;
	Cardinal           *num_args;
{
    ClueWidget          self = (ClueWidget) new;
    ClueWidget          old_self = (ClueWidget) current;

    return (_XpwLabelSetValues(current, new, &(old_self->clue.label),
		       &(self->clue.label), new->core.background_pixel,
		       new->core.depth));
}

/*
 * Destroy any resources we allocated.
 */
static void
Destroy(w)
	Widget              w;
{
    ClueWidget          self = (ClueWidget) w;
    struct clue_list   *cp, *lcp;

   /* Clean up label part first */
    _XpwLabelDestroy(w, &(self->clue.label));
    if (self->clue.timer != (XtIntervalId) NULL)
	XtRemoveTimeOut(self->clue.timer);
    if (self->clue.showing)
	XtPopdown((Widget) self);
   /* Look down the list to find ourselfs and remove us */
    for (cp = lcp = cluelist; cp != NULL; cp = cp->next) {
	if (cp->clue_widget == w) {
	   /* Unlink it */
	    if (lcp == cluelist && cp->next == NULL)
		cluelist = NULL;
	    else
		lcp->next = cp->next;
	    XtFree((XtPointer) cp);
	} else
	    lcp = cp;
    }
   /* Not found, something wrong.
    * But don't worry about it for the moment, can't do anything about
    * it anyway.
    */
}

/************************************************************
 *
 * Private Functions.
 *
 ************************************************************/

/*
 * Redisplay the parent and all children.
 */

static void
PopupClue(client_data, timerid)
	XtPointer           client_data;
	XtIntervalId       *timerid;
{
    ClueWidget          self = (ClueWidget) client_data;
    Position            loc_x, loc_y;
    Widget              wid = self->clue.active;
    Arg                 arglist[1];
    Dimension           h, cw, ch;

    self->clue.timer = (XtIntervalId) NULL;
   /* If no widget active, then do nothing */
    if (wid == NULL || self->clue.label.label == NULL)
	return;
   /* Get widget size */
    XtSetArg(arglist[0], XtNheight, &h);
    XtGetValues(wid, arglist, 1);
   /* Translate point to screen coords */
    XtTranslateCoords(wid, 10, h+10, &loc_x, &loc_y);

   /* Compute required size */
    _XpwLabelDefaultSize(wid, &(self->clue.label), &cw, &ch);

   /* Size and position new window */
    XtResizeWidget((Widget) self, cw + 2, ch + 2, self->core.border_width);
    XtMoveWidget((Widget) self, loc_x, loc_y);
    XtPopup((Widget) self, XtGrabNone);
    self->clue.showing = TRUE;
   /* Draw the label */
    _XpwLabelDraw((Widget) self, &(self->clue.label), NULL, NULL, 1, 1, cw,
		  ch, TRUE);
}

/*
 * Locate the clue assiocated with this widget.
 */
static ClueWidget 
FindClueWidgetFor(w)
	Widget              w;
{
    XtAppContext        context = XtWidgetToApplicationContext(w);
    struct clue_list   *cp;

   /* Look down the list to find clue widget for this context */
    for (cp = cluelist; cp != NULL; cp = cp->next)
	if (cp->context == context)
	    return (ClueWidget) cp->clue_widget;
    return NULL;
}

/************************************************************
 *
 * Public interface functions
 *
 ************************************************************/

/*
 * Arm a clue to popup for widget w.
 */
void 
_XpwArmClue(w, clue)
	Widget              w;
	char               *clue;
{
    ClueWidget          self;
    XtAppContext        context = XtWidgetToApplicationContext(w);

    if (!XtIsSensitive(w))
	return;
    if ((self = FindClueWidgetFor(w)) != (ClueWidget) NULL) {
	if (self->clue.timer != (XtIntervalId) NULL)
	    XtRemoveTimeOut(self->clue.timer);
	self->clue.timer = (XtIntervalId) NULL;
	if (self->clue.showing)
	    XtPopdown((Widget) self);
	self->clue.showing = FALSE;
	self->clue.active = w;
	self->clue.label.label = clue;
	if (self->clue.label.label != NULL)
	    self->clue.timer = XtAppAddTimeOut(context, self->clue.timeout,
					    PopupClue, (XtPointer) self);
    }
}

/*
 * Cancel a clue popup, for widget w.
 */
void 
_XpwDisArmClue(w)
	Widget              w;
{
    ClueWidget          self;

    if ((self = FindClueWidgetFor(w)) != (ClueWidget) NULL) {
       /* Make sure someone else is not in our place */
	if (self->clue.active != w)
	    return;
	if (self->clue.timer != (XtIntervalId) NULL)
	    XtRemoveTimeOut(self->clue.timer);
	if (self->clue.showing)
	    XtPopdown((Widget) self);
	self->clue.showing = FALSE;
	self->clue.timer = (XtIntervalId) NULL;
	self->clue.active = (Widget) NULL;
	self->clue.label.label = NULL;
    }
}

/*
 * Hold off a clue timeout.
 */
void 
_XpwHoldClue(w)
	Widget              w;
{
    ClueWidget          self;

    if ((self = FindClueWidgetFor(w)) != (ClueWidget) NULL) {
       /* Make sure someone else is not in our place */
	if (self->clue.active != w)
	    return;
	if (self->clue.timer != (XtIntervalId) NULL)
	    XtRemoveTimeOut(self->clue.timer);
	self->clue.timer = (XtIntervalId) NULL;
    }
}
