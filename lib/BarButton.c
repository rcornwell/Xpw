/*
 * Menu Bar button widget.
 *
 * This is the menu bar button widget, it does not have a window
 * of it's own. It's purpose is to display the label for the parent.
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
 * $Log: BarButton.c,v $
 * Revision 1.2  1997/10/05 02:25:17  rich
 * Make sure ident line is in object file.
 *
 * Revision 1.1  1997/10/04 05:00:21  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: BarButton.c,v 1.2 1997/10/05 02:25:17 rich Exp rich $";
#endif

#include <X11/Xlib.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xosdefs.h>
#include <X11/Xos.h>
#include <X11/Xmu/Drawing.h>
#include "XpwInit.h"
#include "BarButtonP.h"
#include "MenuBar.h"
#include "Clue.h"
#include "Cardinals.h"

#include <stdio.h>

/* Initialization of defaults */

#define offset(field) XtOffsetOf(BarButtonRec, bar_button.field)

static XtResource   resources[] =
{
    LabelResources
    {XtNcallback, XtCCallback, XtRCallback, sizeof(XtPointer),
     offset(callbacks), XtRCallback, (XtPointer) NULL},
    {XtNborderWidth, XtCBorderWidth, XtRDimension, sizeof(Dimension),
     XtOffsetOf(BarButtonRec, core.border_width), XtRImmediate, (XtPointer) 0},
    {XtNmenuName, XtCMenuName, XtRString, sizeof(String),
     offset(menu_name), XtRString, (XtPointer) NULL},
    {XtNshadowWidth, XtCShadowWidth, XtRDimension, sizeof(Dimension),
     offset(shadow_width), XtRImmediate, (XtPointer) 2},
    {XtNclue, XtCLabel, XtRString, sizeof(String),
     offset(clue), XtRImmediate, (XtPointer) NULL}
};

#undef offset

/* Semi Public function definitions. */
static void         ClassInitialize(void);
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

/* Actions */
static void         Enable(Widget /*w */, XEvent * /*event */,
				 String */*params */, Cardinal */*num_params */);
static void         Disable(Widget /*w */, XEvent * /*event */,
				 String */*params */, Cardinal */*num_params */);
static void         Notify(Widget /*w */, XEvent * /*event */,
				 String */*params */, Cardinal */*num_params */);
static void         Arm(Widget /*w */ , XEvent * /*event */,
				 String */*params */, Cardinal */*num_params */);
static void         Disarm(Widget /*w */ , XEvent * /*event */,
				 String */*params */, Cardinal */*num_params */);

static XtActionsRec actionsList[] =
{
    {"highlight", Enable},
    {"unhighlight", Disable},
    {"notify", Notify},
    {"arm", Arm},
    {"disarm", Disarm},
};

static char         defaultTranslations[] =
       "<EnterWindow>:  arm()\n\
        <LeaveWindow>:  disarm()\n\
              <BtnUp>:  notify() unhighlight()\n\
           <Btn1Down>:  highlight() disarm()\n\
 Button1<EnterWindow>:  highlight()\n";

#define superclass (&widgetClassRec)
BarButtonClassRec   barbuttonClassRec =
{
    {	/* core fields */
	(WidgetClass) superclass,	/* superclass               */
	"BarButton",			/* class_name               */
	sizeof(BarButtonRec),		/* widget_size              */
	ClassInitialize,		/* class_initialize         */
	NULL,				/* class_part_initialize    */
	FALSE,				/* class_inited             */
	Initialize,			/* initialize               */
	NULL,				/* initialize_hook          */
	XtInheritRealize,		/* realize                  */
	actionsList,			/* actions                  */
	XtNumber(actionsList),		/* num_actions              */
	resources,			/* resources                */
	XtNumber(resources),		/* num_resources            */
	NULLQUARK,			/* xrm_class                */
	TRUE,				/* compress_motion          */
	TRUE,				/* compress_exposure        */
	TRUE,				/* compress_enterleave      */
	FALSE,				/* visible_interest         */
	Destroy,			/* destroy                  */
	NULL,				/* resize                   */
	Redisplay,			/* expose                   */
	SetValues,			/* set_values               */
	NULL,				/* set_values_hook          */
	XtInheritSetValuesAlmost,	/* set_values_almost        */
	NULL,				/* get_values_hook          */
	NULL,				/* accept_focus             */
	XtVersion,			/* version                  */
	NULL,				/* callback_private         */
	defaultTranslations,		/* tm_table                 */
	QueryGeometry,			/* query_geometry           */
	XtInheritDisplayAccelerator,	/* display_accelerator      */
	NULL				/* extension                */
    },
    {	/* Bar ButtonClass Fields */
	Enable,				/* highlight               */
	Disable,			/* unhighlight             */
	Notify,				/* notify                  */
	NULL				/* extension          	   */
    }
};

WidgetClass         barbuttonWidgetClass = (WidgetClass) & barbuttonClassRec;

/************************************************************
 *
 * Semi-Public Functions.
 *
 ************************************************************/

/*
 * ClassInitialize: Register a grab for popups.
 */

static void
ClassInitialize()
{
    XpwInitializeWidgetSet();
    XtRegisterGrabAction(Enable, True,
		    (unsigned int) (ButtonPressMask | ButtonReleaseMask),
			 GrabModeAsync, GrabModeAsync);
}

/*
 * Initialize: Set widget default values.
 */

/* ARGSUSED */
static void
Initialize(request, new, args, num_args)
	Widget              request, new;
	ArgList             args;
	Cardinal           *num_args;
{
    BarButton           self = (BarButton) new;

    self->core.border_width = 0;
    _XpwLabelInit(new, &(self->bar_button.label),
		  self->core.background_pixel, self->core.depth);
    _XpwLabelDefaultSize(new, &(self->bar_button.label),
			 &(self->core.width), &(self->core.height));
    self->core.width += 2 * self->bar_button.shadow_width;
    self->core.height += 2 * self->bar_button.shadow_width;
    if (self->bar_button.menu_name != NULL) {
	self->bar_button.menu_name = XtNewString(self->bar_button.menu_name);
    }
}

/*
 * SetValues: Check if the requested changes mean we need to redraw.
 */

/* ARGSUSED */
static              Boolean
SetValues(current, request, new, args, num_args)
	Widget              current, request, new;
	ArgList             args;
	Cardinal           *num_args;
{
    BarButton           self = (BarButton) new;
    BarButton           old_self = (BarButton) current;
    Boolean             ret_val = FALSE;

    if (self->bar_button.menu_name != old_self->bar_button.menu_name) {
	XtFree(old_self->bar_button.menu_name);
	self->bar_button.menu_name = XtNewString(self->bar_button.menu_name);
    }
    if (_XpwLabelSetValues(current, new, &(old_self->bar_button.label),
		  &(self->bar_button.label), self->core.background_pixel,
			   self->core.depth)) {
	ret_val = TRUE;
    }
    if (self->bar_button.shadow_width != old_self->bar_button.shadow_width)
	ret_val = TRUE;
    if (ret_val) {
	_XpwLabelDefaultSize(new, &(self->bar_button.label),
			     &(self->core.width), &(self->core.height));
	self->core.width += 2 * self->bar_button.shadow_width;
	self->core.height += 2 * self->bar_button.shadow_width;
    }
    return (ret_val);
}


/*
 * QueryGeometry: Returns the preferred geometry for this widget.
 * Just return the height and width of the label plus the margins.
 */

static              XtGeometryResult
QueryGeometry(w, intended, return_val)
	Widget              w;
	XtWidgetGeometry   *intended, *return_val;
{
    BarButton           self = (BarButton) w;
    Dimension           width, height;
    XtGeometryResult    ret_val = XtGeometryYes;
    XtGeometryMask      mode = intended->request_mode;

    _XpwLabelDefaultSize(w, &(self->bar_button.label), &width, &height);
    width += 2 * self->bar_button.shadow_width;
    height += 2 * self->bar_button.shadow_width;

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
 * Redisplay the contents of the widget.
 */

/* ARGSUSED */
static void
Redisplay(w, event, region)
	Widget              w;
	XEvent             *event;
	Region              region;
{
    BarButton           self = (BarButton) w;
    int			s = self->bar_button.shadow_width;

    _XpwLabelDraw(w, &(self->bar_button.label), event, region, s, s,
	 self->core.width - (2 * s), self->core.height - (2 * s), FALSE);
}

/*
 * Destroy: Called when widget is remove, cleanup resourses.
 */

static void
Destroy(w)
	Widget              w;
{
    BarButton           self = (BarButton) w;

    _XpwLabelDestroy(w, &(self->bar_button.label));
}

/************************************************************
 *
 * ACTION procedures
 *
 ************************************************************/

/*
 * Enable: if we have a menu associated with us, request our parent to
 * pop it up.
 */
static void
Enable(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    BarButton           self = (BarButton) w;

    if (self->bar_button.menu_name != NULL)
	_XpwMenuPopupEntry(XtParent(w), w);
}

/*
 * Disable: if we have a menu associated with us, request our parent to
 * pop it up.
 */
static void
Disable(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    BarButton           self = (BarButton) w;

    if (self->bar_button.menu_name != NULL)
	_XpwMenuPopdownEntry(XtParent(w));
}

/*
 * Notify: Preform any callbacks requested.
 */

static void
Notify(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    XtCallCallbacks(w, XtNcallback, (XtPointer) NULL);
}


/* ARGSUSED */
static void
Arm(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    BarButton           self = (BarButton) w;

    _XpwArmClue(w, self->bar_button.clue);
}

/* ARGSUSED */
static void
Disarm(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    _XpwDisArmClue(w);
}
