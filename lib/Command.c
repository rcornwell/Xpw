/*
 * Command widget.
 *
 * Draw a command.
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
 * $Log:$
 *
 */

#ifndef lint
static char        *rcsid = "$Id$";

#endif

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Misc.h>
#include <X11/Shell.h>
#include <X11/CoreP.h>
#include "XpwInit.h"
#include "CommandP.h"
#include "Clue.h"

/* Semi Public Functions */
static XtGeometryResult QueryGeometry(Widget /*w */,
				      XtWidgetGeometry * /*intended */,
				      XtWidgetGeometry * /*return_val */ );
static void         Initialize(Widget /*request */, Widget /*new */,
				     ArgList /*args */, Cardinal * /*num_args */);
static Boolean      SetValues(Widget /*current */, Widget /*request */,
				     Widget /*new */, ArgList /*args */,
			             Cardinal * /*num_args */ );
static void         CreateGCs(Widget /*w */ );
static void         Destroy(Widget /*w */ );
static void         Redisplay(Widget /*wid */, XEvent * /*event */,
				      Region /*region */ );

#define DEFAULT_HIGHLIGHT_THICKNESS 2

/* Our resources */
#define offset(field) XtOffsetOf(CommandRec, command.field)
static XtResource   resources[] =
{
    /* Command resources */
    LabelResources
    /* ThreeD resouces */
    threeDresources
    /* Command Stuff */
    {XtNcallback, XtCCallback, XtRCallback, sizeof(XtPointer),
     offset(callbacks), XtRCallback, (XtPointer) NULL},
    {XtNhighlightThickness, XtCThickness, XtRDimension, sizeof(Dimension),
     offset(highlight_thickness), XtRImmediate, (XtPointer) DEFAULT_HIGHLIGHT_THICKNESS},
    {XtNborderWidth, XtCBorderWidth, XtRDimension, sizeof(Dimension),
     XtOffsetOf(RectObjRec, rectangle.border_width), XtRImmediate, (XtPointer) 0},
    {XtNclue, XtCLabel, XtRString, sizeof(String),
     offset(clue), XtRImmediate, (XtPointer) NULL}
};

/* Actions */
static void         Set(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         Unset(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         Reset(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         Highlight(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         Unhighlight(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         Notify(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static XtActionsRec actionsList[] =
{
    {"set", Set},
    {"notify", Notify},
    {"highlight", Highlight},
    {"reset", Reset},
    {"unset", Unset},
    {"unhighlight", Unhighlight},
};

/* Translations */
static char         defaultTranslations[] =
"<EnterWindow>:     highlight()        \n\
 <LeaveWindow>:     reset() unhighlight() \n\
    <Btn1Down>:     set()              \n\
      <Btn1Up>:     notify() unset()   ";

#define SuperClass ((WidgetClass) &widgetClassRec)

CommandClassRec     commandClassRec =
{
    {	/* core_class part */
	(WidgetClass) SuperClass,	/* superclass            */
	"Command",			/* class_name            */
	sizeof(CommandRec),		/* widget_size           */
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
    {	/* command_class part */
	0,
    },
};

WidgetClass         commandWidgetClass = (WidgetClass) & commandClassRec;

/************************************************************
 *
 * Semi-Public interface functions
 *
 ************************************************************/

/* 
 * Initialize the command class.
 */

/* ARGSUSED */
static void
Initialize(request, new, args, num_args)
	Widget              request, new;
	ArgList             args;
	Cardinal           *num_args;
{
    CommandWidget       nself = (CommandWidget) new;
    Dimension           border;

    _XpwLabelInit(new, &(nself->command.label), nself->core.background_pixel,
		  nself->core.depth);
    _XpwThreeDInit(new, &nself->command.threeD, nself->core.background_pixel);
    _XpwLabelDefaultSize(new, &(nself->command.label),
			 &(nself->core.width), &(nself->core.height));

   /* Configure default size */
    border = nself->command.threeD.shadow_width;
    if (nself->command.highlight_thickness > 1)
	border += nself->command.highlight_thickness;
    else
	border += 1;
    nself->core.width += 2 * border;
    nself->core.height += 2 * border;

   /* Create GC's */
    CreateGCs(new);

    nself->command.set = FALSE;
    nself->command.highlighted = HighlightNone;

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
    CommandWidget       self = (CommandWidget) new;
    CommandWidget       old_self = (CommandWidget) current;
    Boolean             ret_val = FALSE;
    XGCValues           values;

    if (_XpwLabelSetValues(current, new, &(old_self->command.label),
		      &(self->command.label), new->core.background_pixel,
			   new->core.depth)) {
	_XpwThreeDDestroyShadow(current, &old_self->command.threeD);
	_XpwThreeDAllocateShadowGC(new, &self->command.threeD,
				   new->core.background_pixel);
	ret_val = TRUE;
    }
    if (self->core.sensitive != old_self->core.sensitive) {
	self->command.set = FALSE;
	self->command.highlighted = HighlightNone;
	ret_val = TRUE;
    }
    if (_XpmThreeDSetValues(new, &old_self->command.threeD,
		    &self->command.threeD, new->core.background_pixel)) {
	ret_val = TRUE;
    }
    if ((old_self->command.label.foreground != self->command.label.foreground) ||
      (old_self->core.background_pixel != self->core.background_pixel) ||
	(old_self->command.highlight_thickness !=
	 self->command.highlight_thickness) ||
	(old_self->command.label.font != self->command.label.font)) {
       /* Label has release one of these */
	XtReleaseGC(current, self->command.inverse_GC);
	XtReleaseGC(current, self->command.normal_GC);

       /* Build new GC's */
	CreateGCs(new);
    }
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
    CommandWidget       self = (CommandWidget) w;
    Dimension           width, height;
    XtGeometryResult    ret_val = XtGeometryYes;
    XtGeometryMask      mode = intended->request_mode;
    Dimension           border;

    _XpwLabelDefaultSize(w, &(self->command.label), &width, &height);
    border = self->command.threeD.shadow_width;
    if (self->command.highlight_thickness > 1)
	border += self->command.highlight_thickness;
    else
	border += 1;
    width += 2 * border;
    height += 2 * border;

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
 * Create GC's we need.
 */
static void
CreateGCs(w)
	Widget              w;
{
    CommandWidget       self = (CommandWidget) w;
    XGCValues           values;
    XtGCMask            mask;

    values.foreground = self->command.label.foreground;
    values.background = self->core.background_pixel;
    values.cap_style = CapProjecting;
    if (self->command.highlight_thickness > 1)
	values.line_width = self->command.highlight_thickness;
    else
	values.line_width = 0;
    mask = GCForeground | GCBackground | GCLineWidth | GCCapStyle;

    self->command.normal_GC = XtGetGC(w, mask, &values);
    values.background = self->command.label.foreground;
    values.foreground = self->core.background_pixel;
    self->command.inverse_GC = XtGetGC(w, mask, &values);
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
    CommandWidget       self = (CommandWidget) wid;
    Display            *dpy = XtDisplayOfObject(wid);
    Window              win = XtWindowOfObject(wid);
    Dimension           s = self->command.threeD.shadow_width;
    Dimension           h = self->core.height;
    Dimension           w = self->core.width;
    Dimension           l = self->command.highlight_thickness;
    GC			gc;

    if (region == NULL)
	XClearWindow(dpy, win);

   /* Draw shadows around main window */
    _XpwThreeDDrawShadow(wid, event, region, &(self->command.threeD), 0, 0, w, h,
			 self->command.set);

   /* Draw the command */
    _XpwLabelDraw(wid, &(self->command.label), event, region, s + l, s + l,
		  w - (2 * (s + l)), h - (2 * (s + l)), FALSE);

   /* Draw highlight if needed */
    if (self->command.highlighted == HighlightAlways ||
	(self->command.highlighted == HighlightWhenUnset && (!self->command.set)))
       /* wide lines are centered on the path, so indent it */
	gc = self->command.normal_GC;
    else
	gc = self->command.inverse_GC;
    XDrawRectangle(dpy, win, gc, s + (l / 2), s + (l / 2),
				 w - l - 2 * s, h - l - 2 * s);

}

/*
 * Destroy an resources we allocated.
 */
static void
Destroy(w)
	Widget              w;
{
    CommandWidget       self = (CommandWidget) w;

    _XpwLabelDestroy(w, &(self->command.label));
    _XpwThreeDDestroyShadow(w, &(self->command.threeD));
    XtReleaseGC(w, self->command.inverse_GC);
    XtReleaseGC(w, self->command.normal_GC);
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
    CommandWidget       self = (CommandWidget) w;

    _XpwDisArmClue(w);
    if (self->command.set)
	return;

    self->command.set = TRUE;
    if (XtIsRealized(w))
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
    CommandWidget       self = (CommandWidget) w;

    if (!self->command.set)
	return;

    self->command.set = FALSE;
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
    CommandWidget       self = (CommandWidget) w;

    if (self->command.set) {
	self->command.highlighted = HighlightNone;
	Unset(w, event, params, num_params);
    } else
	Unhighlight(w, event, params, num_params);
    _XpwDisArmClue(w);
}

/* ARGSUSED */
static void
Highlight(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    CommandWidget       self = (CommandWidget) w;

    if (*num_params == (Cardinal) 0)
	self->command.highlighted = HighlightWhenUnset;
    else {
	if (*num_params != (Cardinal) 1)
	    XtWarning("Too many parameters passed to highlight action table.");
	switch (params[0][0]) {
	case 'A':
	case 'a':
	    self->command.highlighted = HighlightAlways;
	    break;
	default:
	    self->command.highlighted = HighlightWhenUnset;
	    break;
	}
    }

    if (XtIsRealized(w))
	Redisplay(w, event, NULL);
    _XpwArmClue(w, self->command.clue);
}

/* ARGSUSED */
static void
Unhighlight(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    CommandWidget       self = (CommandWidget) w;

    self->command.highlighted = HighlightNone;
    if (XtIsRealized(w))
	Redisplay(w, event, NULL);
}

/* ARGSUSED */
static void
Notify(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    CommandWidget       self = (CommandWidget) w;

   /* check to be sure state is still Set so that user can cancel
    * the action (e.g. by moving outside the window, in the default
    * bindings.
    */
    if (self->command.set)
	XtCallCallbackList(w, self->command.callbacks, (XtPointer) NULL);
}
