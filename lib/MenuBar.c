/*
 * MenuBar widget.
 *
 * Draws a bar of buttons. Each can have a pulldown menu. As user moves
 * around the popups track the cursor. The bar also includes geometry
 * management and threeD shadow drawing for it's children.
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

#ifdef lint
static char        *rcsid = "$Id$";

#endif

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Misc.h>
#include <X11/Shell.h>
#include <X11/CoreP.h>
#include "XpwInit.h"
#include "BarButtonP.h"
#include "MenuBarP.h"

/* Semi Public Functions */
static XtGeometryResult QueryGeometry(Widget /*widget */ ,
				 XtWidgetGeometry * /*constraint */ ,
				 XtWidgetGeometry * /*preferred */ );
static void         Resize(Widget /*w */ );
static void         ClassPartInitialize(WidgetClass /*class */ );
static void         Initialize(Widget /*request */ , Widget /*new */ ,
				 ArgList /*args */ , Cardinal * /*num_args */ );
static XtGeometryResult GeometryManager(Widget /*w */ ,
			         XtWidgetGeometry * /*request */ ,
				 XtWidgetGeometry * /*reply */ );
static void         ChangeManaged(Widget /*w */ );
static Boolean      SetValues(Widget /*current */ , Widget /*request */ ,
				 Widget /*new */ , ArgList /*args */ ,
				 Cardinal * /*num_args */ );
static void         Destroy(Widget /*w */ );
static void         Redisplay(Widget /*w */ , XEvent * /*event */ ,
				 Region /*region */ );

/* Private Functions */
static void         DoLayout(MenuBarWidget /*self */ , int /*width */ ,
				 int /*height */ , Dimension * /*reply_width */ ,
				 Dimension * /*reply_height */, int /*position */ );
static Boolean      TryNewLayout(MenuBarWidget /*self */ );

/* Action Procs */
static void         PopdownMenu(Widget /*w */ , XEvent * /*event */ ,
				 String * /*params */, Cardinal * /*num_params */);
static void         Unhighlight(Widget /*w */ , XEvent * /*event */ ,
				 String * /*params */, Cardinal * /*num_params */);
static void         Highlight(Widget /*w */ , XEvent * /*event */ ,
				 String * /*params */, Cardinal * /*num_params */);
static void         Notify(Widget /*w */ , XEvent * /*event */ ,
				 String * /*params */, Cardinal * /*num_params */);

/* Declare the action procs */
static XtActionsRec actionsList[] =
{
    {"closemenu", PopdownMenu},
    {"notify", Notify},
    {"highlight", Highlight},
    {"unhighlight", Unhighlight},
};

static char         defaultTranslations[] =
"<Btn1Up>:      notify() closemenu() unhighlight()\n\
 <Btn1Motion>:  highlight()\n";

/* Composite class extension record */
static CompositeClassExtensionRec extension_rec =
{
    NULL,				/* Next_extension */
    NULLQUARK,				/* record_type */
    XtCompositeExtensionVersion,	/* Version */
    sizeof(CompositeClassExtensionRec),	/* record_size */
    TRUE,				/* accepts_objects */
};

#define offset(field) XtOffsetOf(MenuBarRec, menubar.field)
static XtResource   resources[] =
{
    {XtNhSpace, XtCHSpace, XtRDimension, sizeof(Dimension),
     offset(h_space), XtRImmediate, (XtPointer) 4},
    {XtNvSpace, XtCVSpace, XtRDimension, sizeof(Dimension),
     offset(v_space), XtRImmediate, (XtPointer) 4},
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
     offset(foreground), XtRString, XtDefaultForeground},
    threeDresources
};

#define SuperClass ((WidgetClass) &compositeClassRec)

MenuBarClassRec     menubarClassRec =
{
    {	/* core_class part */
	(WidgetClass) SuperClass,	/* superclass            */
	"MenuBar",			/* class_name            */
	sizeof(MenuBarRec),		/* widget_size           */
	NULL,				/* class_initialize      */
	ClassPartInitialize,		/* class_part_initialize */
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
	XtExposeCompressMultiple,	/* compress_exposure     */
	TRUE,				/* compress_enterleave   */
	TRUE,				/* visible_interest      */
	Destroy,			/* destroy               */
	Resize,				/* resize                */
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
    {	/* composite_class part */
	GeometryManager,
	ChangeManaged,
	XtInheritInsertChild,
	XtInheritDeleteChild,
	NULL
    },
    {	/* menubar_class part */
	0,
    },
};

WidgetClass         menubarWidgetClass = (WidgetClass) & menubarClassRec;

/* This is a handy definition to scan all our children */
#define ForAllChildren(self, childP) \
	for( (childP) =  (BarButton *)(self)->composite.children ; \
	     (childP) <  ((BarButton *) (self)->composite.children + \
			 (self)->composite.num_children) ; \
	     (childP)++ )


/************************************************************
 *
 * Semi-Public interface functions
 *
 ************************************************************/

static void
ClassPartInitialize(class)
	WidgetClass         class;
{
    MenuBarWidgetClass  c = (MenuBarWidgetClass) class;
    MenuBarWidgetClass  super;
    CompositeClassExtensionRec *ext;

    ext = (XtPointer) XtMalloc(sizeof(*ext));
    *ext = extension_rec;
    ext->next_extension = c->composite_class.extension;
    c->composite_class.extension = ext;
}

/* ARGSUSED */
static void
Initialize(request, new, args, num_args)
	Widget              request, new;
	ArgList             args;
	Cardinal           *num_args;
{
    MenuBarWidget       newself = (MenuBarWidget) new;
    XGCValues           values;

    newself->menubar.last_query_mode = CWWidth | CWHeight;
    newself->menubar.last_query_width = newself->menubar.last_query_height = 0;
    newself->menubar.preferred_width = Max(newself->menubar.h_space, 1);
    newself->menubar.preferred_height = Max(newself->menubar.v_space, 1);

    if (newself->core.width == 0)
	newself->core.width = newself->menubar.preferred_width;

    if (newself->core.height == 0)
	newself->core.height = newself->menubar.preferred_height;

#if 0
    values.foreground = newself->menubar.foreground;
    values.background = newself->core.background_pixel;

    newself->menubar.normal_GC = XtGetGC(new,
				 (GCForeground | GCBackground), &values);
#endif
    newself->menubar.current_menu = NULL;
    newself->menubar.current_popup = NULL;

    _XpwThreeDInit(new, &newself->menubar.threeD, newself->core.background_pixel);
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
    MenuBarWidget       self = (MenuBarWidget) new;
    MenuBarWidget       old_self = (MenuBarWidget) current;

    return (_XpmThreeDSetValues(new, &old_self->menubar.threeD,
		     &self->menubar.threeD, self->core.background_pixel));
}


/*
 * Calculate preferred size, given constraining box, caching it in the widget.
 */

static              XtGeometryResult
QueryGeometry(widget, constraint, preferred)
	Widget              widget;
	XtWidgetGeometry   *constraint, *preferred;
{
    MenuBarWidget       self = (MenuBarWidget) widget;
    Dimension           width /*, height */ ;
    Dimension           preferred_width = self->menubar.preferred_width;
    Dimension           preferred_height = self->menubar.preferred_height;

    constraint->request_mode &= CWWidth | CWHeight;

    if (constraint->request_mode == 0)
       /* parent isn't going to change w or h, so nothing to re-compute */
	return XtGeometryYes;

    if (constraint->request_mode == self->menubar.last_query_mode &&
	(!(constraint->request_mode & CWWidth) ||
	 constraint->width == self->menubar.last_query_width) &&
	(!(constraint->request_mode & CWHeight) ||
	 constraint->height == self->menubar.last_query_height)) {
       /* same query; current preferences are still valid */
	preferred->request_mode = CWWidth | CWHeight;
	preferred->width = preferred_width;
	preferred->height = preferred_height;
	if (constraint->request_mode == (CWWidth | CWHeight) &&
	    constraint->width == preferred_width &&
	    constraint->height == preferred_height)
	    return XtGeometryYes;
	else
	    return XtGeometryAlmost;
    }
   /* else gotta do it the long way...
    * I have a preference for tall and narrow, so if my width is
    * constrained, I'll accept it; otherwise, I'll compute the minimum
    * width that will fit me within the height constraint */

    self->menubar.last_query_mode = constraint->request_mode;
    self->menubar.last_query_width = constraint->width;
    self->menubar.last_query_height = constraint->height;

    if (constraint->request_mode & CWWidth)
	width = constraint->width;
    else {			/* if (constraint->request_mode & CWHeight) */
       /* let's see if I can become any narrower */
	width = 0;
	constraint->width = 65535;
    }

   /* height is currently ignored by DoLayout.
    * height = (constraint->request_mode & CWHeight) ? constraint->height
    * : *preferred_height;
    */
    DoLayout(self, width, (Dimension) 0,
	     &preferred_width, &preferred_height, FALSE);

    if (constraint->request_mode & CWHeight &&
	preferred_height > constraint->height) {
       /* find minimum width for this height */
	if (preferred_width > constraint->width) {
	   /* punt; over-constrained */
	} else {
	    width = preferred_width;
	    do {    /* find some width big enough to stay within this height */
		width *= 2;
		if (width > constraint->width)
		    width = constraint->width;
		DoLayout(self, width, 0, &preferred_width, &preferred_height,
			 FALSE);
	    } while (preferred_height > constraint->height &&
		     width < constraint->width);
	    if (width != constraint->width) {
		do {		/* find minimum width */
		    width = preferred_width;
		    DoLayout(self, preferred_width - 1, 0,
			     &preferred_width, &preferred_height, FALSE);
		} while (preferred_height < constraint->height);
	       /* one last time */
		DoLayout(self, width, 0, &preferred_width, &preferred_height,
			 FALSE);
	    }
	}
    }
    preferred->request_mode = CWWidth | CWHeight;
    preferred->width = self->menubar.preferred_width = preferred_width;
    preferred->height = self->menubar.preferred_height = preferred_height;

    if (constraint->request_mode == (CWWidth | CWHeight)
	&& constraint->width == preferred_width
	&& constraint->height == preferred_height)
	return XtGeometryYes;
    else
	return XtGeometryAlmost;
}

/*
 * Actually layout the box
 */

static void
Resize(w)
	Widget              w;
{
    Dimension           junk;

    DoLayout((MenuBarWidget) w, w->core.width, w->core.height, &junk, &junk,
	     TRUE);
}

/*
 * Geometry Manager
 */

/*ARGSUSED */
static              XtGeometryResult
GeometryManager(w, request, reply)
	Widget              w;
	XtWidgetGeometry   *request;
	XtWidgetGeometry   *reply;	/* RETURN */

{
    Dimension           width, height, borderWidth;
    MenuBarWidget       self;

   /* Position request always denied */
    if ((request->request_mode & CWX && request->x != w->core.x) ||
	(request->request_mode & CWY && request->y != w->core.y))
	return (XtGeometryNo);

   /* Size changes must see if the new size can be accomodated */
    if (request->request_mode & (CWWidth | CWHeight | CWBorderWidth)) {

       /* Make all three fields in the request valid */
	if ((request->request_mode & CWWidth) == 0)
	    request->width = w->core.width;
	if ((request->request_mode & CWHeight) == 0)
	    request->height = w->core.height;
	if ((request->request_mode & CWBorderWidth) == 0)
	    request->border_width = w->core.border_width;

       /* Save current size and set to new size */
	width = w->core.width;
	height = w->core.height;
	borderWidth = w->core.border_width;
	w->core.width = request->width;
	w->core.height = request->height;
	w->core.border_width = request->border_width;

       /* Decide if new layout works: (1) new widget is smaller,
        * (2) new widget fits in existing Box, (3) Box can be
        * expanded to allow new widget to fit */

	self = (MenuBarWidget) w->core.parent;

	if (TryNewLayout(self)) {
	   /* Fits in existing or new space, relayout */
	    (*XtClass((Widget) self)->core_class.resize) ((Widget) self);
	    return (XtGeometryYes);
	} else {
	   /* Cannot satisfy request, change back to original geometry */
	    w->core.width = width;
	    w->core.height = height;
	    w->core.border_width = borderWidth;
	    return (XtGeometryNo);
	}
    };				/* if any size changes requested */

   /* Any stacking changes don't make a difference, so allow if that's all */
    return (XtGeometryYes);
}

static void
ChangeManaged(w)
	Widget              w;
{
   /* Reconfigure the box */
    (void) TryNewLayout((MenuBarWidget) w);
    Resize(w);
}

/*
 * Redisplay the parent and all children.
 */

static void
Redisplay(w, event, region)
	Widget              w;
	XEvent             *event;
	Region              region;
{
    MenuBarWidget       self = (MenuBarWidget) w;
    MenuBarWidgetClass  selfclass = (MenuBarWidgetClass) XtClass(w);
    BarButton          *child;

    if (region == NULL)
        XClearWindow(XtDisplay(w), XtWindow(w));
    _XpwThreeDDrawShadow(w, event, region, &self->menubar.threeD, 0, 0,
			 self->core.width, self->core.height, FALSE);
   /* Repaint each child */
    ForAllChildren(self, child) {
	BarButtonClass      class;

	if (!XtIsManaged((Widget) (*child)))
	    continue;

        if (region != NULL)
            switch (XRectInRegion(region, (int) (*child)->core.x,
                                  (int) (*child)->core.y,
                                (unsigned int) (*child)->core.width,
                                (unsigned int) (*child)->core.height)) {
            case RectangleIn:
            case RectanglePart:
                break;
            default:
                continue;
            }

	class = (BarButtonClass) XtClass((Widget) * child);
	if (class->core_class.expose != NULL)
	    (class->core_class.expose) ((Widget) * child, event, region);
    }

}

/*
 * Destroy an resources we allocated.
 */
static void
Destroy(w)
	Widget              w;
{
    MenuBarWidget       self = (MenuBarWidget) w;

    _XpwThreeDDestroyShadow(w, &self->menubar.threeD);
}

/************************************************************
 *
 * Private functions
 *
 ************************************************************/

/*
 * Private Geometry management functions.
 *
 * DoLayout: scan children and figure out how big they are.
 * if position is true, we move them.
 */
static void
DoLayout(self, width, height, reply_width, reply_height, position)
	MenuBarWidget       self;
	Dimension           width, height;
	Dimension          *reply_width, *reply_height;		/* bounding box */
	Boolean             position;	/* actually reposition the windows? */
{
    Cardinal            i;
    Dimension           w, h;	/* Width and height needed for box              */
    Dimension           lw, lh;	/* Width and height needed for current line     */
    Dimension           bw, bh;	/* Width and height needed for current widget   */
    Dimension           h_space;	/* Local copy of self->box.h_space               */
    Widget              widget;	/* Current widget                               */
    int                 num_mapped_children = 0;
    BarButton          *child;

   /* Box width and height */
    h_space = self->menubar.h_space;

    w = 0;
    ForAllChildren(self, child) {
	if (((Widget) * child)->core.width > w)
	    w = ((Widget) * child)->core.width;
    }
    w += h_space;
    if (w > width)
	width = w;
    h = self->menubar.v_space;

   /* Line width and height */
    lh = 0;
    lw = h_space;

    ForAllChildren(self, child) {
	widget = (Widget) * child;
	if (widget->core.managed) {
	    if (widget->core.mapped_when_managed)
		num_mapped_children++;
	   /* Compute widget width */
	    bw = widget->core.width + 2 * widget->core.border_width + h_space;
	    if ((Dimension) (lw + bw) > width) {
		if (lw > h_space) {
		   /* At least one widget on this line, and
		    * can't fit any more.  Start new line if vbox.
		    */
		    AssignMax(w, lw);
		} else if (!position) {
		   /* too narrow for this widget; we'll assume we can grow */
		    DoLayout(self, lw + bw, height, reply_width,
			     reply_height, position);
		    return;
		}
	    }
	    if (position && (lw != (Dimension) widget->core.x ||
			     h != (Dimension) widget->core.y)) {
		if (XtIsRealized(widget) && widget->core.mapped_when_managed)
		    XUnmapWindow(XtDisplay(widget), XtWindow(widget));
		XtMoveWidget(widget, (int) lw, (int) h);
	    }
	    lw += bw;
	    bh = widget->core.height + 2 * widget->core.border_width;
	    AssignMax(lh, bh);
	}			/* if managed */
    }				/* for */

    if (position && XtIsRealized((Widget) self)) {
	if (self->composite.num_children == num_mapped_children)
	    XMapSubwindows(XtDisplay((Widget) self), XtWindow((Widget) self));
	else {
	    int                 i = self->composite.num_children;
	    Widget             *childP = self->composite.children;

	    for (; i > 0; childP++, i--)
		if (XtIsRealized(*childP) && XtIsManaged(*childP) &&
		    (*childP)->core.mapped_when_managed)
		    XtMapWidget(*childP);
	}
    }
   /* Finish off last line */
    if (lw > h_space) {
	AssignMax(w, lw);
	h += lh + self->menubar.v_space;
    }
    *reply_width = Max(w, 1);
    *reply_height = Max(h, 1);
}

/*
 *
 * Try to do a new layout within the current width and height;
 * if that fails try to resize and do it within the box returne
 * by QueryGeometry.
 *
 * TryNewLayout just says if it's possible, and doesn't actually move the kids
 */

static              Boolean
TryNewLayout(self)
	MenuBarWidget       self;
{
    Dimension           preferred_width, preferred_height;
    Dimension           proposed_width, proposed_height;
    int                 iterations;

    DoLayout(self, self->core.width, self->core.height,
	     &preferred_width, &preferred_height, FALSE);

   /* at this point, preferred_width is guaranteed to not be greater
    * than self->core.width unless some child is larger, so there's no
    * point in re-computing another layout */

    if ((self->core.width == preferred_width) &&
	(self->core.height == preferred_height)) {
       /* Same size */
	return (TRUE);
    }
   /* let's see if our parent will go for a new size. */
    iterations = 0;
    proposed_width = preferred_width;
    proposed_height = preferred_height;
    do {
	switch (XtMakeResizeRequest((Widget) self, proposed_width,
		   proposed_height, &proposed_width, &proposed_height)) {
	case XtGeometryYes:
	    return (TRUE);

	case XtGeometryNo:
	    if (iterations > 0)
	       /* protect from malicious parents who change their minds */
		DoLayout(self, self->core.width, self->core.height,
			 &preferred_width, &preferred_height, FALSE);
	    if ((preferred_width <= self->core.width) &&
		(preferred_height <= self->core.height))
		return (TRUE);
	    else
		return (FALSE);

	case XtGeometryAlmost:
	    if (proposed_height >= preferred_height &&
		proposed_width >= preferred_width) {

	       /*
	        * Take it, and assume the parent knows what it is doing.
	        *
	        * The parent must accept this since it was returned in
	        * almost.
	        *
	        */
		(void) XtMakeResizeRequest((Widget) self,
					 proposed_width, proposed_height,
				      &proposed_width, &proposed_height);
		return (TRUE);
	    } else if (proposed_width != preferred_width) {
	       /* recalc bounding box; height might change */
		DoLayout(self, proposed_width, 0,
			 &preferred_width, &preferred_height, FALSE);
		proposed_height = preferred_height;
	    } else {		/* proposed_height != preferred_height */
		XtWidgetGeometry    constraints, reply;

		constraints.request_mode = CWHeight;
		constraints.height = proposed_height;
		(void) QueryGeometry((Widget) self, &constraints, &reply);
		proposed_width = preferred_width;
	    }
	    break;

	case XtGeometryDone:	/* ??? */
	default:
	    break;
	}
	iterations++;
    } while (iterations < 10);
    return (FALSE);
}


/************************************************************
 *
 * ACTION procedures
 *
 ************************************************************/


/*
 * Remove the highlighting from around one of our children.
 */

/* ARGSUSED */
static void
Unhighlight(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    MenuBarWidget       self = (MenuBarWidget) w;
    BarButton           child = (BarButton) self->menubar.current_menu;
    BarButtonClass      class;

    if (child == NULL)
	return;

    _XpwThreeDEraseShadow((Widget) child, event, NULL, &self->menubar.threeD,
			  0, 0, child->core.width, child->core.height);
    self->menubar.current_menu = NULL;

    class = (BarButtonClass) XtClass(child) /*child->object.widget_class */ ;
    (class->bar_class.unhighlight) ((Widget) child);
}


/* ARGSUSED */
static void
Highlight(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    MenuBarWidget       self = (MenuBarWidget) w;
    Position            x_loc = 0, y_loc = 0;
    BarButton           child = NULL;
    BarButton          *cp;
    BarButtonClass      class;

    if (!XtIsSensitive(w))
	return;
/* Figure out which child window the mouse is pointing at.  */

    switch (event->type) {
    case MotionNotify:
	x_loc = event->xmotion.x;
	y_loc = event->xmotion.y;
	break;
    case EnterNotify:
    case LeaveNotify:
	x_loc = event->xcrossing.x;
	y_loc = event->xcrossing.y;
	break;
    case ButtonPress:
    case ButtonRelease:
	x_loc = event->xbutton.x;
	y_loc = event->xbutton.y;
	break;
    default:
	XtAppError(XtWidgetToApplicationContext(w),
		   "Unknown event type in Highlight().");
	break;
    }
    if ((x_loc >= 0) && (x_loc <= (int) self->core.width) && (y_loc >= 0) &&
	(y_loc <= (int) self->core.height)) {
        ForAllChildren(self, cp) {
	    if (!XtIsManaged(*(Widget *) cp))
	        continue;

	    if (((*cp)->core.x < x_loc) &&
	        ((*cp)->core.x + (int) (*cp)->core.width > x_loc)) {
	        child = *cp;
	        break;
 	    }
        }
    }

    if (child == (BarButton) self->menubar.current_menu)
	return;
    Unhighlight(w, event, params, num_params);

    if (child == NULL)
	return;

    if (!XtIsSensitive((Widget) child)) {
	self->menubar.current_menu = NULL;
	return;
    }
    self->menubar.current_menu = (Widget) child;
    _XpwThreeDDrawShadow((Widget) child, event, NULL, &self->menubar.threeD,
		     0, 0, child->core.width, child->core.height, FALSE);
   /* Tell the child he is now up! */
    class = (BarButtonClass) XtClass(child) /*child->object.widget_class */ ;
    (class->bar_class.highlight) ((Widget) child);
}

/*
 * Call back the currently selected child.
 */

/* ARGSUSED */
static void
Notify(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    MenuBarWidget       self = (MenuBarWidget) w;
    BarButton           child = (BarButton) self->menubar.current_menu;
    BarButtonClass      class;

    if (child == NULL)
	return;

    class = (BarButtonClass) XtClass(child);
    (class->bar_class.notify) ((Widget) child);
}

/*
 * All done with the window, pop it down.
 */
static void
PopdownMenu(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    MenuBarWidget       self = (MenuBarWidget) w;

    if (self->menubar.current_popup != NULL) {
	XtPopdown(self->menubar.current_popup);
	XtRemoveGrab(w);
    }
    self->menubar.current_popup = NULL;

    if (self->menubar.current_menu != NULL)
	Unhighlight(w, NULL, NULL, NULL);
}

/************************************************************
 *
 * Public interface functions
 *
 ************************************************************/

/*
 * Child asked us to pop down the window.
 */
/* ARGSUSED */
void
_XpwMenuPopdownEntry(w)
	Widget              w;
{
    PopdownMenu(w, NULL, NULL, NULL);
}

/*
 * Pop up a menu if we need to.
 */
/* ARGSUSED */
void
_XpwMenuPopupEntry(w, menubutton)
	Widget              w;
	Widget              menubutton;
{
    MenuBarWidget       self = (MenuBarWidget) w;
    BarButton           child = (BarButton) menubutton;
    Widget              menu = NULL, temp;
    Arg                 arglist[2];
    int                 menu_x, menu_y, menu_width, menu_height, button_height;
    Position            button_x, button_y;

   /* If no childern selected, do nothing */
    if (child == NULL)
	return;

   /* Find the widget to popup */
    for (temp = w; temp != NULL; temp = XtParent(temp)) {
	if ((menu = XtNameToWidget(temp, child->bar_button.menu_name)) != NULL)
	    break;
    }

   /* If same, ignore, otherwise popdown old and mark us as current */
    if (self->menubar.current_popup == menu)
	return;
    if (self->menubar.current_popup != NULL) {
	XtPopdown(self->menubar.current_popup);
	XtRemoveGrab(w);
    }
    self->menubar.current_popup = NULL;

   /* First shut off the current selection if there is one */
    if (self->menubar.current_menu != menubutton
	&& self->menubar.current_menu != NULL)
	Unhighlight(w, NULL, NULL, NULL);

   /* Next set the popup and current menu */
    self->menubar.current_popup = menu;
    self->menubar.current_menu = (Widget) child;
    _XpwThreeDDrawShadow((Widget) child, NULL, NULL, &self->menubar.threeD,
		     0, 0, child->core.width, child->core.height, FALSE);

    if (menu == NULL)
	return;
   /* Not realized, realize it first */
    if (!XtIsRealized(menu))
	XtRealizeWidget(menu);

   /* Figure out where to place it when it is shown */
    menu_width = menu->core.width + 2 * menu->core.border_width;
    button_height = w->core.height /* + 2 * w->core.border_width */ ;
    menu_height = menu->core.height + 2 * menu->core.border_width;

    XtTranslateCoords(w, child->core.x, child->core.y, &button_x, &button_y);
    menu_x = button_x;
    menu_y = button_y + button_height;

    if (menu_x >= 0) {
	int                 scr_width = WidthOfScreen(XtScreen(menu));

	if (menu_x + menu_width > scr_width)
	    menu_x = scr_width - menu_width;
    }
    if (menu_x < 0)
	menu_x = 0;

    if (menu_y >= 0) {
	int                 scr_height = HeightOfScreen(XtScreen(menu));

	if (menu_y + menu_height > scr_height)
	    menu_y = scr_height - menu_height;
    }
    if (menu_y < 0)
	menu_y = 0;

    XtSetArg(arglist[0], XtNx, menu_x);
    XtSetArg(arglist[1], XtNy, menu_y);
    XtSetValues(menu, arglist, 2);

   /* Grab events so we can watch for moving into other windows */
    XtAddGrab(w, TRUE, TRUE);
    XtPopup(menu, XtGrabNonexclusive);
}


