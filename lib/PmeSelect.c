/*
 * PmeSelection box.
 *
 * Displays a switch and a label. Switchs can be grouped into radio groups.
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
 * $Log: PmeSelect.c,v $
 * Revision 1.1  1997/10/04 22:13:23  rich
 * Initial revision
 *
 *
 */

#ifdef lint
static char        *rcsid = "$Id: PmeSelect.c,v 1.1 1997/10/04 22:13:23 rich Exp $";

#endif

#include <X11/Xlib.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xosdefs.h>
#include <X11/Xos.h>
#include <X11/Xmu/Drawing.h>
#include <X11/Xmu/Converters.h>
#include "XpwInit.h"
#include "Clue.h"
#include "PopupMenu.h"
#include "PmeSelectP.h"

#include <stdio.h>

/* Initialization of defaults */

#define offset(field) XtOffsetOf(PmeSelectRec, pme_select.field)

static XtResource   resources[] =
{
    /* PmeSelector resources */
    {XtNswitchShape, XtCShapeType, XtRShapeType, sizeof(ShapeType),
     offset(switchShape), XtRImmediate, (XtPointer) XaSquare},
    {XtNswitchSize, XtCSwitchSize, XtRDimension, sizeof(Dimension),
     offset(switchSize), XtRImmediate, (XtPointer) 15},
    {XtNswitchShadow, XtCShadowWidth, XtRDimension, sizeof(Dimension),
     offset(switchShadow), XtRImmediate, (XtPointer) 2},
    {XtNcallback, XtCCallback, XtRCallback, sizeof(XtPointer),
     offset(callbacks), XtRCallback, (XtPointer) NULL},
    {XtNstate, XtCState, XtRBoolean, sizeof(Boolean),
     offset(state), XtRImmediate, (XtPointer) FALSE},
    {XtNradioGroup, XtCWidget, XtRWidget, sizeof(Widget),
     offset(widget), XtRWidget, (XtPointer) NULL},
    {XtNradioData, XtCRadioData, XtRPointer, sizeof(XtPointer),
     offset(radio_data), XtRPointer, (XtPointer) NULL},
    {XtNallowNone, XtCAllowNone, XtRBoolean, sizeof(Boolean),
     offset(allownone), XtRImmediate, (XtPointer) TRUE},
    {XtNrightButton, XtCRightButton, XtRBoolean, sizeof(Boolean),
     offset(rightbutton), XtRImmediate, (XtPointer) TRUE},
    {XtNswitchColor, XtCForeground, XtRPixel, sizeof(Pixel),
     offset(switchColor), XtRString, XtDefaultForeground},
    {XtNblankOff, XtCBlankOff, XtRBoolean, sizeof(Boolean),
     offset(blankOff), XtRImmediate, (XtPointer)True},

    /* Label resources */
    LabelResources

    /* ThreeD resouces */
    threeDresources

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

/* Private Function Definitions.  */
static void         CreateGCs(Widget /*w */ );
static void         DestroyGCs(Widget /*w */ );
static void         DrawSwitch(Widget /*w */ );
static RadioGroup  *GetRadioGroup(Widget /*w */ );
static void         CreateRadioGroup(Widget /*w1 */ , Widget /*w2 */ );
static void         AddToRadioGroup(RadioGroup * /*group */ , Widget /*w */ );
static void         TurnOffRadioSiblings(Widget /*w */ );
static void         RemoveFromRadioGroup(Widget /*w */ );
static void         Set(Widget /*w */ );
static void         Unset(Widget /*w */ );

/* Actions */
static void         Toggle(Widget /*w */ );
static void         Arm(Widget /*w */ );
static void         Disarm(Widget /*w */);
static char       * GetMenuName(Widget /*w */);

#define superclass (&rectObjClassRec)
PmeSelectClassRec      pmeSelectClassRec =
{
    {	/* rectangle fields */
	(WidgetClass) superclass,	/* superclass               */
	"PmeSelect",			/* class_name               */
	sizeof(PmeSelectRec),		/* widget_size              */
	ClassInitialize,		/* class_initialize         */
	NULL,				/* class_part_initialize    */
	FALSE,				/* class_inited             */
	Initialize,			/* initialize               */
	NULL,				/* initialize_hook          */
	NULL,				/* realize                  */
	NULL,				/* actions                  */
	0,				/* num_actions              */
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
	NULL,				/* tm_table                 */
	QueryGeometry,			/* query_geometry           */
	NULL,				/* display_accelerator      */
	NULL				/* extension                */
    },
    {	/* PmeSelectClass Fields */
	Toggle,				/* notify                   */
        Arm,                      	/* highlight                */
        Disarm,	                    	/* unhighlight              */
        GetMenuName,                    /* getmenuname              */
	NULL				/* extension                */
    }
};

WidgetClass         pmeSelectObjectClass = (WidgetClass) & pmeSelectClassRec;

/************************************************************
 *
 * Semi-Public Functions.
 *
 ************************************************************/

/*
 * ClassInitialize: Add in a type converter for Justify argument.
 */

static void
ClassInitialize()
{
    static XtConvertArgRec parentCvtArgs[] =
    {
	{XtBaseOffset, (XtPointer) XtOffsetOf(WidgetRec, core.parent),
	 sizeof(Widget)}
    };

    XpwInitializeWidgetSet();
    XtSetTypeConverter(XtRString, XtRWidget, XmuNewCvtStringToWidget,
		     parentCvtArgs, XtNumber(parentCvtArgs), XtCacheNone,
		       (XtDestructor) NULL);
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
    PmeSelectObject        self = (PmeSelectObject) new;
    Screen             *scr = XtScreenOfObject(new);

    _XpwLabelInit(new, &(self->pme_select.label),
		 XtParent(new)->core.background_pixel,
	         XtParent(new)->core.depth);
    _XpwThreeDInit(new, &self->pme_select.threeD, 
		XtParent(new)->core.background_pixel);

    self->pme_select.threeD.shadow_width = self->pme_select.switchShadow;

    CreateGCs(new);

   /* Compute default size */
    _XpwLabelDefaultSize(new, &(self->pme_select.label), &(self->rectangle.width),
					 &(self->rectangle.height));
    self->rectangle.width += self->pme_select.switchSize;
    if (self->rectangle.height < self->pme_select.switchSize)
	self->rectangle.height = self->pme_select.switchSize;
    self->rectangle.width += 2 * self->pme_select.threeD.shadow_width;
    self->rectangle.height += 2 * self->pme_select.threeD.shadow_width;

    self->pme_select.radio_group = NULL;

    if (self->pme_select.radio_data == NULL)
	self->pme_select.radio_data = (XtPointer) XtName(new);

    if (self->pme_select.widget != NULL) {
	if (GetRadioGroup(self->pme_select.widget) == NULL)
	    CreateRadioGroup(new, self->pme_select.widget);
	else
	    AddToRadioGroup(GetRadioGroup(self->pme_select.widget), new);
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
    PmeSelectObject        self = (PmeSelectObject) new;
    PmeSelectObject        oldself = (PmeSelectObject) current;
    Boolean             ret_val = FALSE;

    if (_XpwLabelSetValues(current, new, &(oldself->pme_select.label),
		       &(self->pme_select.label),
		       XtParent(new)->core.background_pixel,
                       XtParent(new)->core.depth)
	|| self->pme_select.switchColor != oldself->pme_select.switchColor) {
	_XpwThreeDDestroyShadow(current, &oldself->pme_select.threeD);
	_XpwThreeDAllocateShadowGC(new, &self->pme_select.threeD,
			XtParent(new)->core.background_pixel);
	self->pme_select.threeD.shadow_width = self->pme_select.switchShadow;
	DestroyGCs(current);
	CreateGCs(new);
	ret_val = TRUE;
    }
    if (self->rectangle.sensitive != oldself->rectangle.sensitive)
	ret_val = TRUE;

    if (oldself->pme_select.widget != self->pme_select.widget) {
	XpwPmeSelectChangeRadioGroup(new, self->pme_select.widget);
	ret_val = FALSE;
    }
    if (oldself->pme_select.state != self->pme_select.state) {
	self->pme_select.state = oldself->pme_select.state;
	Toggle(new);
	ret_val = TRUE;
    }
    if (oldself->pme_select.switchShadow != self->pme_select.switchShadow) {
	self->pme_select.threeD.shadow_width = self->pme_select.switchShadow;
	ret_val = TRUE;
    }
    if ((oldself->pme_select.switchSize != self->pme_select.switchSize) ||
	(oldself->pme_select.switchShape != self->pme_select.switchShape))
	ret_val = TRUE;

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
    PmeSelectObject        self = (PmeSelectObject) w;
    Dimension           width, height;
    XtGeometryResult    ret_val = XtGeometryYes;
    XtGeometryMask      mode = intended->request_mode;

    _XpwLabelDefaultSize(w, &(self->pme_select.label), &width, &height);
    width += self->pme_select.switchSize;
    if (height < self->pme_select.switchSize)
	height = self->pme_select.switchSize;

    width += 2 * self->pme_select.threeD.shadow_width;
    height += 2 * self->pme_select.threeD.shadow_width;

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
 * Redisplay the contents of the widget.
 */

/* ARGSUSED */
static void
Redisplay(w, event, region)
	Widget              w;
	XEvent             *event;
	Region              region;
{
    PmeSelectObject        self = (PmeSelectObject) w;
    Display            *dpy = XtDisplayOfObject(w);
    Window              win = XtWindowOfObject(w);
    GC                  gc;
    int                 x_loc;
    Dimension           s = self->pme_select.threeD.shadow_width;

   /* If the switch is on the left, shift over that far */
    if (self->pme_select.rightbutton)
	x_loc = 0;
    else
	x_loc = self->pme_select.switchSize;

    x_loc += self->rectangle.x;

   /* Draw the label */
    _XpwLabelDraw(w, &(self->pme_select.label), event, region, x_loc,
		  self->rectangle.y,
		  self->rectangle.width - self->pme_select.switchSize - (2 * s),
		  self->rectangle.height - (2 * s), FALSE);

    DrawSwitch(w);

}

/*
 * Destroy: Called when widget is remove, cleanup resourses.
 */

static void
Destroy(w)
	Widget              w;
{
    PmeSelectObject        self = (PmeSelectObject) w;

    RemoveFromRadioGroup(w);
    DestroyGCs(w);
    _XpwLabelDestroy(w, &(self->pme_select.label));
    _XpwThreeDDestroyShadow(w, &(self->pme_select.threeD));
}

/************************************************************
 *
 * Private functions
 *
 ************************************************************/

/* ARGSUSED */
static void
DrawSwitch(w)
	Widget              w;
{
    GC                  gc;
    PmeSelectObject        self = (PmeSelectObject) w;
    Display            *dpy = XtDisplayOfObject(w);
    Window              win = XtWindowOfObject(w);
    int                 width, x_loc, y_loc;
    Dimension           s = self->pme_select.threeD.shadow_width;

   /* Figure out what colors to make arrow */
    if (!self->pme_select.state)
	gc = self->pme_select.inverse_gc;
    else
	gc = self->pme_select.norm_gc;

    if (!XtIsSensitive(w) && !XtIsSensitive(XtParent(w)))
	gc = self->pme_select.label.norm_gray_gc;
   /* Now Draw the bitmaps */
    width = self->pme_select.switchSize;
   /* First figure out where to put it! */
    y_loc = (self->rectangle.height / 2 - self->pme_select.switchSize / 2)
		+ self->rectangle.y;

   /* Compute X cord first. */
    if (self->pme_select.rightbutton)
	x_loc = self->rectangle.width - width;
    else
	x_loc = 0;

    x_loc += self->rectangle.x;
   
    if (self->pme_select.blankOff && !self->pme_select.state)
	XClearArea(dpy, win, x_loc, y_loc, width, width, FALSE);
    else
        _XpwThreeDDrawShape(w, NULL, NULL, &(self->pme_select.threeD), gc,
		    self->pme_select.switchShape, x_loc, y_loc, width, width,
			self->pme_select.state);
}

/*
 * GetRadioGroup: Gets the radio group associated with a give pme_select widget.
 */

static RadioGroup  *
GetRadioGroup(w)
	Widget              w;
{
    PmeSelectObject        self = (PmeSelectObject) w;

    if (self == NULL)
	return (NULL);
    return (self->pme_select.radio_group);
}

/*
 * CreateRadioGroup: Creates a radio group. give two widgets.
 */

static void
CreateRadioGroup(w1, w2)
	Widget              w1, w2;
{
    char                error_buf[BUFSIZ];
    PmeSelectObject        self1 = (PmeSelectObject) w1;
    PmeSelectObject        self2 = (PmeSelectObject) w2;

    if ((self1->pme_select.radio_group != NULL) || (self2->pme_select.radio_group != NULL)) {
	(void) sprintf(error_buf, "%s %s", "PmeSelect Widget Error - Attempting",
	       "to create a new pme_select group, when one already exists.");
	XtWarning(error_buf);
    }
    AddToRadioGroup((RadioGroup *) NULL, w1);
    AddToRadioGroup(GetRadioGroup(w1), w2);
}

/* 
 * AddToRadioGroup: Adds a pme_select to the radio group.
 */

static void
AddToRadioGroup(group, w)
	RadioGroup         *group;
	Widget              w;
{
    PmeSelectObject        self = (PmeSelectObject) w;
    RadioGroup         *local;

    local = (RadioGroup *) XtMalloc(sizeof(RadioGroup));
    local->widget = w;
    self->pme_select.radio_group = local;

    if (group == NULL) {	/* Creating new group. */
	group = local;
	group->next = NULL;
	group->prev = NULL;
	return;
    }
    local->prev = group;	/* Adding to previous group. */
    if ((local->next = group->next) != NULL)
	local->next->prev = local;
    group->next = local;
}

/* 
 * TurnOffRadioSiblings: Deactivates all radio siblings.
 */

static void
TurnOffRadioSiblings(w)
	Widget              w;
{
    RadioGroup         *group;

    if ((group = GetRadioGroup(w)) == NULL)	/* Punt if there is no group */
	return;

   /* Go to the top of the group. */

    for (; group->prev != NULL; group = group->prev) ;

    while (group != NULL) {
	PmeSelectObject        local_tog = (PmeSelectObject) group->widget;

	if (local_tog->pme_select.state) 
	    Unset(group->widget);
	group = group->next;
    }
}

/*
 * RemoveFromRadioGroup: Removes a pme_select from a RadioGroup.
 */

static void
RemoveFromRadioGroup(w)
	Widget              w;
{
    RadioGroup         *group = GetRadioGroup(w);

    if (group != NULL) {
	if (group->prev != NULL)
	    (group->prev)->next = group->next;
	if (group->next != NULL)
	    (group->next)->prev = group->prev;
	XtFree((char *) group);
    }
}

/*
 * CreateGCs: Creates all gc's for the simple menu widget.
 */

static void
CreateGCs(w)
	Widget              w;
{
    PmeSelectObject        self = (PmeSelectObject) w;
    XGCValues           values;
    XtGCMask            mask;

    values.foreground = XtParent(w)->core.background_pixel;
    values.background = self->pme_select.switchColor;
    mask = GCForeground | GCBackground;

    self->pme_select.inverse_gc = XtGetGC(w, mask, &values);

    values.foreground = self->pme_select.switchColor;
    values.background = XtParent(w)->core.background_pixel;
    mask = GCForeground | GCBackground;

    self->pme_select.norm_gc = XtGetGC(w, mask, &values);
}

/*
 * DestroyGCs: Removes all gc's for the simple menu widget.
 */

static void
DestroyGCs(w)
	Widget              w;
{
    PmeSelectObject        self = (PmeSelectObject) w;

    XtReleaseGC(w, self->pme_select.inverse_gc);
    XtReleaseGC(w, self->pme_select.norm_gc);
}

/*
 * Set selected widget.
 */
static void
Set(w)
	Widget              w;
{
    PmeSelectObject        self = (PmeSelectObject) w;
    Boolean             oldstate = self->pme_select.state;

    TurnOffRadioSiblings(w);
    self->pme_select.state = True;
    if (oldstate != self->pme_select.state && XtIsRealized(w))
	DrawSwitch(w);
    XtCallCallbacks(w, XtNcallback, (XtPointer) (int)self->pme_select.state);
}

/*
 * Clear selected widget.
 */
static void
Unset(w)
	Widget              w;
{
    PmeSelectObject        self = (PmeSelectObject) w;
    Boolean             oldstate = self->pme_select.state;

    self->pme_select.state = False;
    if (oldstate != self->pme_select.state && XtIsRealized(w))
	DrawSwitch(w);
    XtCallCallbacks(w, XtNcallback, (XtPointer) (int)self->pme_select.state);
}


/************************************************************
 *
 * ACTION procedures.
 *
 ************************************************************/

/*
 * Toggle: if we have a menu associated with us, request our parent to
 * pop it up.
 */
static void
Toggle(w)
	Widget              w;
{
    PmeSelectObject        self = (PmeSelectObject) w;
    Boolean             oldstate = self->pme_select.state;
						/* Others could clear us */

   /* Only change state if we are not selected */
    if (self->pme_select.state == FALSE)
	TurnOffRadioSiblings(w);
    self->pme_select.state = oldstate;
    if (self->pme_select.allownone)
	self->pme_select.state = !oldstate;
    else
	self->pme_select.state = TRUE;
    if (XtIsRealized(w))
	DrawSwitch(w);
    XtCallCallbacks(w, XtNcallback, (XtPointer) (int)self->pme_select.state);
}

/* ARGSUSED */
static void
Arm(w)
	Widget              w;
{
    PmeSelectObject        self = (PmeSelectObject) w;

    _XpwArmClue(w, self->pme_select.clue);
}

/* ARGSUSED */
static void
Disarm(w)
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

    return NULL;
}

/************************************************************
 *
 * Public interface functions
 *
 ************************************************************/

/* 
 * Puts widget into a given radio group.
 */

void
XpwPmeSelectChangeRadioGroup(w, radio_group)
	Widget              w, radio_group;
{
    PmeSelectObject        self = (PmeSelectObject) w;
    RadioGroup         *group;

    RemoveFromRadioGroup(w);

/*
 * If the pme_select that we are about to add is set then we will 
 * unset all pme_selects in the new radio group.
 */

    if (self->pme_select.state && radio_group != NULL)
	XpwPmeSelectUnsetCurrent(radio_group);

    if (radio_group != NULL)
	if ((group = GetRadioGroup(radio_group)) == NULL)
	    CreateRadioGroup(w, radio_group);
	else
	    AddToRadioGroup(group, w);
}

/*      
 *  Returns the current Radio Group of Widget.
 */

XtPointer
XpwPmeSelectGetCurrent(w)
	Widget              w;
{
    RadioGroup         *group;

    if ((group = GetRadioGroup(w)) == NULL)
	return (NULL);
    for (; group->prev != NULL; group = group->prev) ;

    while (group != NULL) {
	PmeSelectObject        local_sel = (PmeSelectObject) group->widget;

	if (local_sel->pme_select.state)
	    return (local_sel->pme_select.radio_data);
	group = group->next;
    }
    return (NULL);
}

/* 
 * Sets the widget associated with the given radio group.
 */

void
XpwPmeSelectSetCurrent(radio_group, radio_data)
	Widget              radio_group;
	XtPointer           radio_data;
{
    RadioGroup         *group;
    PmeSelectObject        local_sel;

/* Special case of no radio group. */

    if ((group = GetRadioGroup(radio_group)) == NULL) {
	local_sel = (PmeSelectObject) radio_group;
	if ((local_sel->pme_select.radio_data == radio_data))
	    if (!local_sel->pme_select.state) 
		Set((Widget) local_sel);
	return;
    }
/*
 * find top of radio_roup 
 */

    for (; group->prev != NULL; group = group->prev) ;

/*
 * search for matching radio data.
 */

    while (group != NULL) {
	local_sel = (PmeSelectObject) group->widget;
	if ((local_sel->pme_select.radio_data == radio_data)) {
	    if (!local_sel->pme_select.state) /* if not already set. */
		Set((Widget) local_sel);
	    return;		/* found it, done */
	}
	group = group->next;
    }
}

/*
 * Unsets all widgets of a given Radio group.
 */

void
XpwPmeSelectUnsetCurrent(radio_group)
	Widget              radio_group;
{
    PmeSelectObject        local_sel = (PmeSelectObject) radio_group;

   /* Special Case no radio group. */

    if (local_sel->pme_select.state) 
	Unset(radio_group);
    if (GetRadioGroup(radio_group) == NULL)
	return;
    TurnOffRadioSiblings(radio_group);
}
