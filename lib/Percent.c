/*
 * Percent widget.
 *
 * Draw a percent bar.
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
 * $Log: Percent.c,v $
 * Revision 1.2  1997/11/01 06:39:05  rich
 * Removed unused variables.
 *
 * Revision 1.1  1997/10/08 04:03:06  rich
 * Initial revision
 *
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: Percent.c,v 1.2 1997/11/01 06:39:05 rich Beta $";

#endif

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Misc.h>
#include <X11/Xmu/Drawing.h>
#include <X11/Shell.h>
#include <X11/CoreP.h>
#include "XpwInit.h"
#include "PercentP.h"
#include "Clue.h"

/* Semi Public Functions */
static void         ClassInitialize();
static void         Initialize(Widget /*request */ , Widget /*new */ ,
			  ArgList /*args */ , Cardinal * /*num_args */ );
static Boolean      SetValues(Widget /*current */ , Widget /*request */ ,
			      Widget /*new */ , ArgList /*args */ ,
			      Cardinal * /*num_args */ );
static XtGeometryResult QueryGeometry(Widget /*w */ ,
				      XtWidgetGeometry * /*intended */ ,
				    XtWidgetGeometry * /*return_val */ );
static void         Redisplay(Widget /*wid */ , XEvent * /*event */ ,
			      Region /*region */ );
static void         Destroy(Widget /*w */ );

/* Private Functions */
static void         CreateGCs(Widget /*w */ );
static void         DestroyGCs(Widget /*w */ );

/* Our resources */
#define offset(field) XtOffsetOf(PercentRec, percent.field)
static XtResource   resources[] =
{
    /* Percent resources */
    {XtNmax, XtCMax, XtRInt, sizeof(int),
     offset(max), XtRImmediate, (XtPointer) 100},
    {XtNmin, XtCMin, XtRInt, sizeof(int),
     offset(min), XtRImmediate, (XtPointer) 0},
    {XtNposition, XtCPosition, XtRInt, sizeof(int),
     offset(position), XtRImmediate, (XtPointer) 0},
    {XtNorientation, XtCOrientation, XtROrientation, sizeof(XtOrientation),
     offset(orientation), XtRImmediate, (XtPointer) XtorientVertical},
    {XtNthickness, XtCThickness, XtRInt, sizeof(int),
     offset(thickness), XtRImmediate, (XtPointer) 14},
    {XtNshowValue, XtCShowValue, XtRBoolean, sizeof(Boolean),
     offset(showValue), XtRImmediate, (XtPointer) TRUE},
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
     offset(foreground), XtRString, XtDefaultForeground},
    {XtNthumb, XtCThumb, XtRPixel, sizeof(Pixel),
     offset(thumb), XtRString, XtDefaultForeground},
    {XtNborderWidth, XtCBorderWidth, XtRDimension, sizeof(Dimension),
     XtOffsetOf(PercentRec, core.border_width), XtRImmediate, (XtPointer) 0},
    {XtNinternational, XtCInternational, XtRBoolean, sizeof(Boolean),
     offset(international), XtRImmediate, (XtPointer) FALSE},
    {XtNvertSpace, XtCVertSpace, XtRInt, sizeof(int),
     offset(vert_space), XtRImmediate, (XtPointer) 25},
    {XtNjustify, XtCJustify, XtRJustify, sizeof(XtJustify),
     offset(justify), XtRImmediate, (XtPointer) XtJustifyLeft},
    {XtNfont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
     offset(font), XtRString, XtDefaultFont},
    {XtNfontSet, XtCFontSet, XtRFontSet, sizeof(XFontSet),
     offset(fontset), XtRString, XtDefaultFontSet},

    /* ThreeD resouces */
    threeDresources
    {XtNclue, XtCLabel, XtRString, sizeof(String),
     offset(clue), XtRImmediate, (XtPointer) NULL}
};

/* Actions */
static void         Enter(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         Leave(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );

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

PercentClassRec     percentClassRec =
{
    {	/* core_class part */
	(WidgetClass) SuperClass,	/* superclass            */
	"Percent",			/* class_name            */
	sizeof(PercentRec),		/* widget_size           */
	ClassInitialize,		/* class_initialize      */
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
    {	/* percent_class part */
	0,
    },
};

WidgetClass         percentWidgetClass = (WidgetClass) & percentClassRec;

/************************************************************
 *
 * Semi-Public interface functions
 *
 ************************************************************/

/*
 * ClassInitialize: Add in a type converter for Justify argument.
 */

static void
ClassInitialize()
{
    XpwInitializeWidgetSet();
    XtAddConverter(XtRString, XtROrientation, XmuCvtStringToOrientation,
		   (XtConvertArgList) NULL, (Cardinal) 0);
    XtAddConverter(XtRString, XtRJustify, XmuCvtStringToJustify,
		   (XtConvertArgList) NULL, (Cardinal) 0);
}

/* ARGSUSED */
static void
Initialize(request, new, args, num_args)
	Widget              request, new;
	ArgList             args;
	Cardinal           *num_args;
{
    PercentWidget       nself = (PercentWidget) new;
    Dimension           width, height;

    _XpwThreeDInit(new, &nself->percent.threeD, nself->core.background_pixel);
    width = height = 0;
    if (nself->percent.showValue) {
	if (nself->percent.international == True) {
	    XFontSetExtents    *ext = XExtentsOfFontSet(nself->percent.fontset);

	    width = XmbTextEscapement(nself->percent.fontset, "100%", 4);
	    height = 2 + ext->max_ink_extent.height;
	} else {
	    width = XTextWidth(nself->percent.font, "100%", 4);
	    height = 2 + (nself->percent.font->max_bounds.ascent +
			  nself->percent.font->max_bounds.descent);
	}
	height = ((int) height * (100 + nself->percent.vert_space)) / 100;
    }
    if (nself->percent.orientation == XtorientHorizontal) {
	if (width < nself->percent.thickness)
	    width = nself->percent.thickness;
	if (height < nself->percent.thickness) ;
	height = nself->percent.thickness;
    } else {
	if (height < nself->percent.thickness)
	    height = nself->percent.thickness;
	if (width < nself->percent.thickness)
	    width = nself->percent.thickness;
    }
    nself->core.width += 2 * nself->percent.threeD.shadow_width;
    nself->core.height += 2 * nself->percent.threeD.shadow_width;
    nself->percent.state = TRUE;

    CreateGCs(new);

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
    PercentWidget       self = (PercentWidget) new;
    PercentWidget       old_self = (PercentWidget) current;
    Boolean             ret_val = FALSE;

    if (((old_self->percent.font != self->percent.font) &&
	 (old_self->percent.international == False)) ||
	(old_self->percent.foreground != self->percent.foreground) ||
	(old_self->percent.thumb != self->percent.thumb)) {
	DestroyGCs(current);
	CreateGCs(new);
	ret_val = TRUE;
    }
    if ((old_self->percent.fontset != self->percent.fontset) &&
	(old_self->percent.international == True))
       /* DONT changes the GCs, because the fontset is not in them. */
	ret_val = TRUE;

    if (old_self->percent.justify != self->percent.justify)
	ret_val = TRUE;

    if (self->core.sensitive != old_self->core.sensitive)
	ret_val = TRUE;
    if (_XpmThreeDSetValues(new, &old_self->percent.threeD,
		      &self->percent.threeD, new->core.background_pixel))
	ret_val = TRUE;
    if (self->core.sensitive != old_self->core.sensitive)
	ret_val = TRUE;
    if ((old_self->percent.position != self->percent.position) ||
	(old_self->percent.min != self->percent.min) ||
	(old_self->percent.max != self->percent.max))
	ret_val = TRUE;
    return (ret_val);
}

#define abs(x)	(((x)<0)?-(x):(x))

/*
 * Calculate preferred size, given constraining box, caching it in the widget.
 */

static              XtGeometryResult
QueryGeometry(w, intended, return_val)
	Widget              w;
	XtWidgetGeometry   *intended, *return_val;
{
    PercentWidget       self = (PercentWidget) w;
    Dimension           width, height;
    XtGeometryResult    ret_val = XtGeometryYes;
    XtGeometryMask      mode = intended->request_mode;

    width = 0;
    height = 0;
    if (self->percent.showValue) {
	if (self->percent.international == True) {
	    XFontSetExtents    *ext = XExtentsOfFontSet(self->percent.fontset);

	    width = XmbTextEscapement(self->percent.fontset, "100%", 4);
	    height += 2 + ext->max_ink_extent.height;
	} else {
	    width = XTextWidth(self->percent.font, "100%", 4);
	    height += 2 + (self->percent.font->max_bounds.ascent +
			   self->percent.font->max_bounds.descent);
	}
	height = ((int) height * (100 + self->percent.vert_space)) / 100;
    }
   /* Compute the default size */
    if (self->percent.orientation == XtorientHorizontal) {
	Dimension           nw;

	nw = abs(self->percent.max - self->percent.min);
	if (nw > width)
	    width = nw;
	if (self->percent.thickness > height)
	    height = self->percent.thickness;
    } else {
	Dimension           nh;

	nh = abs(self->percent.max - self->percent.min);
	if (nh > height)
	    height = nh;
	if (width > self->percent.thickness)
	    width = self->percent.thickness;
    }
    width += 2 * self->percent.threeD.shadow_width;
    height += 2 * self->percent.threeD.shadow_width;

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
    PercentWidget       self = (PercentWidget) wid;
    Display            *dpy = XtDisplayOfObject(wid);
    Window              win = XtWindowOfObject(wid);
    int                 font_ascent = 0, font_descent = 0, x_loc, y_loc;
    XFontSetExtents    *ext = XExtentsOfFontSet(self->percent.fontset);
    GC                  gc;
    Dimension           s = self->percent.threeD.shadow_width;
    Dimension           h = self->core.height;
    Dimension           w = self->core.width;
    float               pos;
    int                 th, t_width;

    if (!XtIsRealized(wid))
	return;

    XClearWindow(dpy, win);

    if (!self->percent.state)
	return;

   /* Draw shadows around main window */
    _XpwThreeDDrawShadow(wid, NULL, NULL, &(self->percent.threeD), 0, 0, w, h,
			 0);
    h -= 2 * s;
    w -= 2 * s;

   /* Set the context based on sensitivity setting */
    if (XtIsSensitive(wid) && XtIsSensitive(XtParent(wid)))
	gc = self->percent.thumb_gc;
    else
	gc = self->percent.thumb_gray_gc;

   /* Force position into range */
    if (self->percent.position < self->percent.min)
	self->percent.position = self->percent.min;
    if (self->percent.position > self->percent.max)
	self->percent.position = self->percent.max;
   /* Compute new top and bottom of thumb */
    pos = ((float) self->percent.position) /
	((float) (self->percent.max - self->percent.min));

   /* Keep values in range */
    if (pos > 1.0)
	pos = 1.0;
    else if (pos < -1.0)
	pos = -1.0;

    if (self->percent.orientation == XtorientHorizontal) {
	x_loc = (int) (pos * (float) w);
	if (x_loc < 0)
	    XFillRectangle(dpy, win, gc, (-x_loc) + s, s, w + x_loc, h);
	else
	    XFillRectangle(dpy, win, gc, s, s, x_loc, h);
    } else {
	y_loc = (int) (pos * (float) h);
	if (y_loc < 0)
	    XFillRectangle(dpy, win, gc, s, (-y_loc) + s, w, h + y_loc);
	else
	    XFillRectangle(dpy, win, gc, s, s, w, y_loc);
    }

/* Set the context based on sensitivity setting */
    if (XtIsSensitive(wid) && XtIsSensitive(XtParent(wid)))
	gc = self->percent.norm_gc;
    else
	gc = self->percent.norm_gray_gc;

    if (self->percent.showValue) {
	char                buffer[4];

	sprintf(buffer, "%3d%%", abs((int) (100.0 * pos)));

	if (self->percent.international == True) {
	    t_width = XmbTextEscapement(self->percent.fontset, buffer, 4);
	    font_ascent = abs(ext->max_ink_extent.y);
	    font_descent = ext->max_ink_extent.height - font_ascent;
	} else {		/*else, compute size from font like R5 */
	    t_width = XTextWidth(self->percent.font, buffer, 4);
	    font_ascent = self->percent.font->max_bounds.ascent;
	    font_descent = self->percent.font->max_bounds.descent;
	}
	th = font_ascent + font_descent;

       /* Justify label */
	switch (self->percent.justify) {
	case XtJustifyCenter:
	    x_loc = (w - t_width) / 2;
	    break;
	case XtJustifyRight:
	    x_loc = w - t_width;
	    break;
	case XtJustifyLeft:
	default:
	    x_loc = 0;
	    break;
	}

       /* this will center the text in the gadget top-to-bottom */
	x_loc += s;
	y_loc = s + th + 2;
	if (self->percent.international == True)
	    XmbDrawString(dpy, win, self->percent.fontset, gc, x_loc, y_loc,
			  buffer, 4);
	else
	    XDrawString(dpy, win, gc, x_loc, y_loc, buffer, 4);
    }
}

/*
 * Destroy an resources we allocated.
 */
static void
Destroy(w)
	Widget              w;
{
    DestroyGCs(w);
}

/************************************************************
 *
 * Private functions
 *
 ************************************************************/

/*
 * Create GC's.
 */
static void
CreateGCs(w)
	Widget              w;
{
    PercentWidget       self = (PercentWidget) w;
    XGCValues           values;
    XtGCMask            mask;

    values.foreground = self->percent.foreground;
    values.background = self->core.background_pixel;
    values.graphics_exposures = FALSE;
    mask = GCForeground | GCBackground | GCGraphicsExposures;

    self->percent.norm_gc = XtGetGC(w, mask, &values);

    values.fill_style = FillTiled;
    values.tile = XmuCreateStippledPixmap(XtScreenOfObject(w),
			  self->percent.foreground, self->core.background_pixel,
			  self->core.depth);
    values.graphics_exposures = FALSE;
    mask |= GCTile | GCFillStyle;
    self->percent.norm_gray_gc = XtGetGC(w, mask, &values);

    values.foreground = self->percent.thumb;
    values.background = self->core.background_pixel;
    values.graphics_exposures = FALSE;
    mask = GCForeground | GCBackground | GCGraphicsExposures;

    if (self->percent.showValue && 
		self->percent.foreground == self->percent.thumb) {
    	values.fill_style = FillTiled;
    	values.tile = XmuCreateStippledPixmap(XtScreenOfObject(w),
     	self->percent.thumb, self->core.background_pixel, self->core.depth);
    	values.graphics_exposures = FALSE;
    	mask |= GCTile | GCFillStyle;
    }

    self->percent.thumb_gc = XtGetGC(w, mask, &values);

    values.fill_style = FillTiled;
    values.tile = XmuCreateStippledPixmap(XtScreenOfObject(w),
     self->percent.thumb, self->core.background_pixel, self->core.depth);
    values.graphics_exposures = FALSE;
    mask |= GCTile | GCFillStyle;
    self->percent.thumb_gray_gc = XtGetGC(w, mask, &values);
    _XpwThreeDAllocateShadowGC(w, &self->percent.threeD,
			       w->core.background_pixel);
}

/*
 * Destroy GC's.
 */
static void
DestroyGCs(w)
	Widget              w;
{
    PercentWidget       self = (PercentWidget) w;

    XtReleaseGC(w, self->percent.norm_gc);
    XtReleaseGC(w, self->percent.norm_gray_gc);
    XtReleaseGC(w, self->percent.thumb_gc);
    XtReleaseGC(w, self->percent.thumb_gray_gc);
    _XpwThreeDDestroyShadow(w, &self->percent.threeD);
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
    PercentWidget       self = (PercentWidget) w;

    _XpwArmClue(w, self->percent.clue);
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

/************************************************************
 *
 * Public interface functions
 *
 ************************************************************/

/*
 * Set percent thumb size and showing.
 */
void
XpwPercentSetMinMax(w, min, max)
	Widget              w;
	int                 min;
	int                 max;
{
    PercentWidget       self = (PercentWidget) w;

    self->percent.min = min;
    self->percent.max = max;
    Redisplay(w, NULL, NULL);
}

/*
 * Move the thumb.
 */
void
XpwPercentSetPosition(w, pos)
	Widget              w;
	int                 pos;
{
    PercentWidget       self = (PercentWidget) w;

    if (self->percent.position != pos) {
        self->percent.position = pos;
        Redisplay(w, NULL, NULL);
    }
}

/*
 * Turn widget on and off.
 */
void
XpwPercentSetState(w, state)
	Widget		    w;
	Boolean		    state;
{
    PercentWidget	self = (PercentWidget) w;

    self->percent.state = state;
    Redisplay(w, NULL, NULL);
}
