/*
 * Frame widget.
 *
 * Draw a label with a border around it.
 * 
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
 * $Log: Frame.c,v $
 * Revision 1.3  1997/11/01 06:39:02  rich
 * Added support for allowing child to be smaller than parent.
 * Cleaned up geometry management code.
 * Label is placed at Label height down.
 *
 * Revision 1.2  1997/10/12 05:13:48  rich
 * Removed calling child expose.
 * Ask the child what size it wants to be.
 * Don't bother to make subwindows.
 *
 * Revision 1.1  1997/10/04 05:04:15  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: Frame.c,v 1.3 1997/11/01 06:39:02 rich Beta $";

#endif

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Misc.h>
#include <X11/Shell.h>
#include <X11/CoreP.h>
#include "XpwInit.h"
#include "FrameP.h"
#include "threeDdrawP.h"

/* Semi Public Functions */

/* Frame.c */
static void         DoLayout(FrameWidget /*self */ , int /*width */ ,
			 int /*height */ , Dimension * /*reply_width */ ,
		     Dimension * /*reply_height */ , int /*position */ );
static XtGeometryResult QueryGeometry(Widget /*w */ ,
				      XtWidgetGeometry * /*constraint */ ,
				      XtWidgetGeometry * /*preferred */ );
static void         Resize(Widget /*w */ );
static Boolean      TryNewLayout(FrameWidget /*self */ );
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
static void         Redisplay(Widget /*wid */ , XEvent * /*event */ ,
			      Region /*region */ );

/* Composite class extension record */
static CompositeClassExtensionRec extension_rec =
{
    NULL,			/* Next_extension */
    NULLQUARK,			/* record_type */
    XtCompositeExtensionVersion,	/* Version */
    sizeof(CompositeClassExtensionRec),		/* record_size */
    TRUE,			/* accepts_objects */
};

/* Our resources */
#define offset(field) XtOffsetOf(FrameRec, frame.field)
static XtResource   resources[] =
{
    /* Frame resources */
    {XtNlabelOnTop, XtCLabelOnTop, XtRBoolean, sizeof(Boolean),
     offset(labelontop), XtRImmediate, (XtPointer) TRUE},
    {XtNoutline, XtCOutline, XtRBoolean, sizeof(Boolean),
     offset(outline), XtRImmediate, (XtPointer) TRUE},
    {XtNframeWidth, XtCframeWidth, XtRDimension, sizeof(Dimension),
     offset(frame_width), XtRImmediate, (XtPointer) 10},
    {XtNoutlineType, XtCFrameType, XtRFrameType, sizeof(FrameType),
     offset(threeO.frameType), XtRImmediate, (XtPointer) XfGrooved},
    {XtNoutlineRaised, XtCOutlineRaised, XtRBoolean, sizeof(Boolean),
     offset(threeO.raised), XtRImmediate, (XtPointer) FALSE},
    {XtNusePreferedSize, XtCUsePreferedSize, XtRBoolean, sizeof(Boolean),
     offset(usePrefered), XtRImmediate, (XtPointer) FALSE},

    /* Label resources */
    LabelResources
    /* ThreeD resouces */
    threeDresources
};

#define SuperClass ((WidgetClass) &compositeClassRec)

FrameClassRec       frameClassRec =
{
    {	/* core_class part */
	(WidgetClass) SuperClass,	/* superclass            */
	"Frame",			/* class_name            */
	sizeof(FrameRec),		/* widget_size           */
	XpwInitializeWidgetSet,		/* class_initialize      */
	ClassPartInitialize,		/* class_part_initialize */
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
	NULL,				/* tm_table              */
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
    {	/* frame_class part */
	0,
    },
};

WidgetClass         frameWidgetClass = (WidgetClass) & frameClassRec;

/************************************************************
 *
 * Semi-Public interface functions
 *
 ************************************************************/

static void
ClassPartInitialize(class)
	WidgetClass         class;
{
    FrameWidgetClass    c = (FrameWidgetClass) class;
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
    FrameWidget         newself = (FrameWidget) new;
    FrameType           outtype = newself->frame.threeO.frameType;
    Boolean             outraise = newself->frame.threeO.raised;

    _XpwLabelInit(new, &(newself->frame.label), newself->core.background_pixel,
		  newself->core.depth);
    _XpwThreeDInit(new, &newself->frame.threeD, newself->core.background_pixel);
    memcpy(&(newself->frame.threeO), &(newself->frame.threeD),
	   sizeof(_XpmThreeDFrame));
    newself->frame.threeO.frameType = outtype;
    newself->frame.threeO.raised = outraise;
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
    FrameWidget         self = (FrameWidget) new;
    FrameWidget         old_self = (FrameWidget) current;
    Boolean             ret_val = FALSE;

    if (_XpwLabelSetValues(current, new, &(old_self->frame.label),
			&(self->frame.label), new->core.background_pixel,
			   new->core.depth)) {
	Dimension	w, h;
	w = h = 0;
    	_XpwLabelDefaultSize((Widget) self, &(self->frame.label), &w, &h);
    	self->frame.label_height = h;
	ret_val = TRUE;
    }

    if ((self->frame.frame_width != old_self->frame.frame_width) ||
	(self->frame.threeD.shadow_width != old_self->frame.threeD.shadow_width))
	ret_val = TRUE;

    if (ret_val)
	Resize(new);

    if ((self->core.sensitive != old_self->core.sensitive) ||
	(self->frame.labelontop != old_self->frame.labelontop) ||
	(self->frame.outline != old_self->frame.outline) ||
    (self->frame.threeO.frameType != old_self->frame.threeO.frameType) ||
	(self->frame.threeO.raised != old_self->frame.threeO.raised))
	ret_val = TRUE;
    if (_XpmThreeDSetValues(new, &old_self->frame.threeD,
		      &self->frame.threeD, new->core.background_pixel)) {
	FrameType           outtype;
	Boolean             outraised;

	outtype = self->frame.threeO.frameType;
	outraised = self->frame.threeO.raised;
	memcpy(&(self->frame.threeO), &(old_self->frame.threeD),
	       sizeof(_XpmThreeDFrame));
	self->frame.threeO.frameType = outtype;
	self->frame.threeO.raised = outraised;
	ret_val = TRUE;
    }
    return (ret_val);
}

/*
 * Calculate preferred size, given constraining box, caching it in the widget.
 */

static              XtGeometryResult
QueryGeometry(w, constraint, preferred)
	Widget              w;
	XtWidgetGeometry   *constraint, *preferred;
{
    FrameWidget         self = (FrameWidget) w;
    Dimension           width, height;

    DoLayout(self, 0, 0, &width, &height, FALSE);

    preferred->request_mode = CWWidth | CWHeight;
    preferred->width = width;
    preferred->height = height;

    if (constraint->request_mode == (CWWidth | CWHeight)
	&& constraint->width == width
	&& constraint->height == height)
	return XtGeometryYes;
    else
	return XtGeometryAlmost;
}

/*
 *  Resize, just relayout the children for new size.
 */

static void
Resize(w)
	Widget              w;
{
    FrameWidget         self = (FrameWidget) w;
    Dimension           junk;

    DoLayout(self, self->core.width, self->core.height, &junk, &junk, TRUE);
}

/*
 * Geometry Manager: Attempt to honor any child resize request.
 */

/*ARGSUSED */
static              XtGeometryResult
GeometryManager(w, request, reply)
	Widget              w;
	XtWidgetGeometry   *request;
	XtWidgetGeometry   *reply;	/* RETURN */

{
    Dimension           width, height, borderWidth;
    FrameWidget         self = (FrameWidget)XtParent(w);

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

	if (TryNewLayout(self)) {
	    Dimension           junk;
	   /* Fits in existing or new space, relayout */
	    DoLayout(self, self->core.width, self->core.height, &junk, &junk, TRUE);
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
    (void) TryNewLayout((FrameWidget) w);
    Resize(w);
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
    FrameWidget         self = (FrameWidget) wid;
    Display            *dpy = XtDisplayOfObject(wid);
    Window              win = XtWindowOfObject(wid);
    Dimension           s = self->frame.threeD.shadow_width;
    Dimension           f = self->frame.frame_width;
    Dimension           f2 = f / 2;
    Dimension           h = self->core.height;
    Dimension           w = self->core.width;
    Dimension		lh = self->frame.label_height;
    Dimension           x_loc, y_loc;

    if (region == NULL)
	XClearWindow(dpy, win);

    x_loc = s + f + 2;
    y_loc = (self->frame.labelontop) ? (s + f) : (h - lh - s - s);
   /* Draw the label */
    _XpwLabelDraw(wid, &(self->frame.label), event, region, x_loc, y_loc,
		  w - 2 * (s + f), lh, TRUE);
   /* Draw shadows around main window */
    _XpwThreeDDrawShadow(wid, event, region, &(self->frame.threeD), 0, 0, w, h, 0);

    if (self->frame.outline) {
	_XpwThreeDDrawShadow(wid, event, region, &(self->frame.threeO),
			     f2, f2, w - f, h - f, 0);
    }
}

/*
 * Destroy an resources we allocated.
 */
static void
Destroy(w)
	Widget              w;
{
    FrameWidget         self = (FrameWidget) w;

    _XpwLabelDestroy(w, &(self->frame.label));
    _XpwThreeDDestroyShadow(w, &(self->frame.threeD));
}

/************************************************************
 *
 * Private functions
 *
 ************************************************************/

/*
 * Private Geometry management functions.
 *
 * DoLayout: Figure out how big the label and child will be.
 * if position is true, we move them.
 */
static void
DoLayout(self, width, height, reply_width, reply_height, position)
	FrameWidget         self;
	Dimension           width, height;
	Dimension          *reply_width, *reply_height;		/* bounding box */
	Boolean             position;	/* actually reposition the windows? */
{
    Dimension           w, h;	/* Width and height needed for widgit */
    Dimension           bw, fw;
    Position            cx, cy;
    Dimension		cw, ch;	/* X and y needed for child */
    Dimension           s = self->frame.threeD.shadow_width + self->frame.frame_width;
    XtWidgetGeometry    request, reply;

    _XpwLabelDefaultSize((Widget) self, &(self->frame.label), &w, &h);
    self->frame.label_height = h;
    if (self->composite.num_children != 0) {
        Widget              child;

	child = self->composite.children[0];
	bw = child->core.border_width;
	cw = child->core.width;
	ch = child->core.height;
       /* If we are not positioning ask child how big it needs */
	if (!position) {
	    request.request_mode = CWHeight | CWWidth;
	    request.width = cw;
	    request.height = ch;
	    if (XtQueryGeometry(child, &request, &reply) == XtGeometryAlmost) {
		if (reply.request_mode & CWWidth)
		    cw = reply.width;
		if (reply.request_mode & CWHeight)
		    ch = reply.height;
	    }
	}
	fw = 2 * (s + bw);
	if (cw > w)
	    w = cw;
       /* Make child as big as we are */
	if ((self->frame.usePrefered == False || cw == 0) && width != 0) {
	        cw = width - fw;
	}
	if ((self->frame.usePrefered == False || ch == 0) && height != 0) {
		Dimension hh = h + fw;
	        ch = height - hh;
	}
	if (width == 0)
		width = w + fw;
	if (height == 0)
		height = h + ch + fw;
	/* Do this long way to get around gcc bug */
	cx = height - h;
	cx = cx - fw;
	cy = s + ((self->frame.labelontop)?h:0) + (cx - ch) / 2;
	cx = width - fw;
	cx = s + (cx - cw) / 2;
	if (position) 
	    XtConfigureWidget(child, (Position)cx, (Position)cy, 
			(Dimension)cw, (Dimension)ch, bw);
        w += fw;
        h += fw + ch;
    } else {
	w += 2 * s;
	h += 2 * s;
	if (width != 0)		/* Make child as big as we are */
	    w = width;
	if (height != 0)
	    h = height;
    }
    if (width > w)
	w = width;
    if (height > h)
	h = height;
    *reply_width = w;
    *reply_height = h;
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
	FrameWidget         self;
{
    Dimension           preferred_width, preferred_height;
    Dimension           proposed_width, proposed_height;
    int			iterations;

    DoLayout(self, 0, 0, &preferred_width, &preferred_height, FALSE);

    if ((self->core.width == preferred_width) &&
	(self->core.height == preferred_height)) 
	return (TRUE);
    
   /* let's see if our parent will go for a new size. */
    proposed_width = preferred_width;
    proposed_height = preferred_height;
    for(iterations=0; iterations < 10; iterations++) {
        switch (XtMakeResizeRequest((Widget) self,
				     proposed_width, proposed_height,
				    &proposed_width, &proposed_height)) {
        case XtGeometryYes:
	    return (TRUE);

        case XtGeometryNo:
           /* protect from malicious parents who change their minds */
            if (iterations > 0)
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
            }

	    break;

        case XtGeometryDone:
        default:
	    break;
        }
    }
    return (FALSE);
}
