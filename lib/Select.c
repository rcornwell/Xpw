/*
 * Selection box.
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
 * $Log: Select.c,v $
 * Revision 1.3  1997/11/01 06:39:08  rich
 * Cleaned up switch display handling code.
 * Fixed some errors in geometry query code.
 * Removed unused variables.
 *
 * Revision 1.2  1997/10/05 02:25:17  rich
 * Make sure ident line is in object file.
 *
 * Revision 1.1  1997/10/04 22:13:23  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: Select.c,v 1.3 1997/11/01 06:39:08 rich Beta $";

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
#include "SelectP.h"

#include <stdio.h>

/* Initialization of defaults */

#define offset(field) XtOffsetOf(SelectRec, select.field)

static XtResource   resources[] =
{
    /* Selector resources */
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

    /* Label resources */
    LabelResources

    {XtNborderWidth, XtCBorderWidth, XtRDimension, sizeof(Dimension),
     XtOffsetOf(SelectRec, core.border_width), XtRImmediate, (XtPointer) 0},

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

/* Actions */
static void         Notify(Widget /*w */ , XEvent * /*event */ ,
		           String * /*params */ , Cardinal * /*num_params */ );
static void         Set(Widget /*w */ , XEvent * /*event */ ,
		           String * /*params */ , Cardinal * /*num_params */ );
static void         Unset(Widget /*w */ , XEvent * /*event */ ,
		           String * /*params */ , Cardinal * /*num_params */ );
static void         Toggle(Widget /*w */ , XEvent * /*event */ ,
		           String * /*params */ , Cardinal * /*num_params */ );
static void         Arm(Widget /*w */ , XEvent * /*event */ ,
		           String * /*params */ , Cardinal * /*num_params */ );
static void         Disarm(Widget /*w */ , XEvent * /*event */ ,
		           String * /*params */ , Cardinal * /*num_params */ );

static XtActionsRec actionsList[] =
{
    {"toggle", Toggle},
    {"notify", Notify},
    {"set", Set},
    {"unset", Unset},
    {"arm", Arm},
    {"disarm", Disarm},
};

static char         defaultTranslations[] =
"<BtnDown>:	toggle() notify()disarm()\n\
 <EnterWindow>: arm()\n\
 <LeaveWindow>: disarm()\n";

#define superclass (&widgetClassRec)
SelectClassRec      selectClassRec =
{
    {	/* core fields */
	(WidgetClass) superclass,	/* superclass               */
	"Select",			/* class_name               */
	sizeof(SelectRec),		/* widget_size              */
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
    {	/* SelectClass Fields */
	Set,				/* set                      */
	Unset,				/* unset                    */
	Toggle,				/* toggle                   */
	Notify,				/* notify                   */
	NULL				/* extension                */
    }
};

WidgetClass         selectWidgetClass = (WidgetClass) & selectClassRec;

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
    SelectWidget        self = (SelectWidget) new;
    Dimension		h, w;

    _XpwLabelInit(new, &(self->select.label), self->core.background_pixel,
		  self->core.depth);
    _XpwThreeDInit(new, &self->select.threeD, self->core.background_pixel);

    memcpy(&(self->select.threeS), &(self->select.threeD),
	   sizeof(_XpmThreeDFrame));
    _XpwThreeDInit(new, &self->select.threeS, self->core.background_pixel);
    self->select.threeS.shadow_width = self->select.switchShadow;

    CreateGCs(new);

   /* Compute default size */
    _XpwLabelDefaultSize(new, &(self->select.label), &w, &h);
    w += self->select.switchSize;
    if (h < self->select.switchSize)
	h = self->select.switchSize;
    w += 2 * self->select.threeD.shadow_width;
    h += 2 * self->select.threeD.shadow_width;
    if (self->core.width == 0)
        self->core.width = w;
    if (self->core.height == 0)
        self->core.height = h;

    self->select.radio_group = NULL;

    if (self->select.radio_data == NULL)
	self->select.radio_data = (XtPointer) new->core.name;

    if (self->select.widget != NULL) {
	if (GetRadioGroup(self->select.widget) == NULL)
	    CreateRadioGroup(new, self->select.widget);
	else
	    AddToRadioGroup(GetRadioGroup(self->select.widget), new);
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
    SelectWidget        self = (SelectWidget) new;
    SelectWidget        oldself = (SelectWidget) current;
    Boolean             ret_val = FALSE;

    if (_XpwLabelSetValues(current, new, &(oldself->select.label),
		       &(self->select.label), new->core.background_pixel,
			   new->core.depth)
	|| self->select.switchColor != oldself->select.switchColor) {
	_XpwThreeDDestroyShadow(current, &oldself->select.threeD);
	_XpwThreeDAllocateShadowGC(new, &self->select.threeD,
				   new->core.background_pixel);
	_XpwThreeDDestroyShadow(current, &oldself->select.threeS);
	memcpy(&(self->select.threeS), &(self->select.threeD),
	       sizeof(_XpmThreeDFrame));
	self->select.threeS.shadow_width = self->select.switchShadow;
	_XpwThreeDAllocateShadowGC(new, &self->select.threeS,
				   new->core.background_pixel);
	DestroyGCs(current);
	CreateGCs(new);
	ret_val = TRUE;
    }
    if (self->core.sensitive != oldself->core.sensitive)
	ret_val = TRUE;

    if (oldself->select.widget != self->select.widget) {
	XpwSelectChangeRadioGroup(new, self->select.widget);
	ret_val = FALSE;
    }
    if (oldself->select.state != self->select.state) {
	self->select.state = oldself->select.state;
	Toggle(new, (XEvent *) NULL, (String *) NULL, (Cardinal *) 0);
	ret_val = TRUE;
    }
    if (oldself->select.switchShadow != self->select.switchShadow) {
	self->select.threeS.shadow_width = self->select.switchShadow;
	ret_val = TRUE;
    }
    if ((oldself->select.switchSize != self->select.switchSize) ||
	(oldself->select.switchShape != self->select.switchShape))
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
    SelectWidget        self = (SelectWidget) w;
    Dimension           width, height;
    XtGeometryResult    ret_val = XtGeometryYes;
    XtGeometryMask      mode = intended->request_mode;

    _XpwLabelDefaultSize(w, &(self->select.label), &width, &height);
    width += self->select.switchSize;
    if (height < self->select.switchSize)
	height = self->select.switchSize;

    width += 2 * self->select.threeD.shadow_width;
    height += 2 * self->select.threeD.shadow_width;

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
    if (ret_val != XtGeometryAlmost) {
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
    SelectWidget        self = (SelectWidget) w;
    Display            *dpy = XtDisplayOfObject(w);
    Window              win = XtWindowOfObject(w);
    int                 x_loc;
    Dimension           s = self->select.threeD.shadow_width;

    if (region == NULL)
	XClearWindow(dpy, win);

   /* If the switch is on the left, shift over that far */
    if (self->select.rightbutton)
	x_loc = 0;
    else
	x_loc = self->select.switchSize;

    x_loc += s;

   /* Draw the label */
    _XpwLabelDraw(w, &(self->select.label), event, region, x_loc, s,
		  self->core.width - self->select.switchSize - (2 * s),
		  self->core.height - (2 * s), FALSE);

    DrawSwitch(w);

}

/*
 * Destroy: Called when widget is remove, cleanup resourses.
 */

static void
Destroy(w)
	Widget              w;
{
    SelectWidget        self = (SelectWidget) w;

    RemoveFromRadioGroup(w);
    DestroyGCs(w);
    _XpwLabelDestroy(w, &(self->select.label));
    _XpwThreeDDestroyShadow(w, &(self->select.threeD));
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
    SelectWidget        self = (SelectWidget) w;
    int                 width, x_loc, y_loc;
    Dimension           s = self->select.threeD.shadow_width;

   /* Draw shadows around main window */
    _XpwThreeDDrawShadow(w, NULL, NULL, &(self->select.threeD), 0, 0,
		self->core.width, self->core.height, self->select.state);

   /* Figure out what colors to make arrow */
    if (!self->select.state)
	gc = self->select.inverse_gc;
    else
	gc = self->select.norm_gc;

    if (!XtIsSensitive(w) && !XtIsSensitive(XtParent(w)))
	gc = self->select.label.norm_gray_gc;
   /* Now Draw the bitmaps */
    width = self->select.switchSize;
   /* First figure out where to put it! */
    y_loc = (self->core.height / 2) - (self->select.switchSize / 2);

   /* Compute X cord first. */
    if (self->select.rightbutton)
	x_loc = (int)(self->core.width) - width - 2 * s;
    else
	x_loc =  2 * s;

    _XpwThreeDDrawShape(w, NULL, NULL, &(self->select.threeS), gc,
		    self->select.switchShape, x_loc, y_loc, width, width,
			self->select.state);
}

/*
 * GetRadioGroup: Gets the radio group associated with a give select widget.
 */

static RadioGroup  *
GetRadioGroup(w)
	Widget              w;
{
    SelectWidget        self = (SelectWidget) w;

    if (self == NULL)
	return (NULL);
    return (self->select.radio_group);
}

/*
 * CreateRadioGroup: Creates a radio group. give two widgets.
 */

static void
CreateRadioGroup(w1, w2)
	Widget              w1, w2;
{
    char                error_buf[BUFSIZ];
    SelectWidget        self1 = (SelectWidget) w1;
    SelectWidget        self2 = (SelectWidget) w2;

    if ((self1->select.radio_group != NULL) || (self2->select.radio_group != NULL)) {
	(void) sprintf(error_buf, "%s %s", "Select Widget Error - Attempting",
	       "to create a new select group, when one already exists.");
	XtWarning(error_buf);
    }
    AddToRadioGroup((RadioGroup *) NULL, w1);
    AddToRadioGroup(GetRadioGroup(w1), w2);
}

/* 
 * AddToRadioGroup: Adds a select to the radio group.
 */

static void
AddToRadioGroup(group, w)
	RadioGroup         *group;
	Widget              w;
{
    SelectWidget        self = (SelectWidget) w;
    RadioGroup         *local;

    local = (RadioGroup *) XtMalloc(sizeof(RadioGroup));
    local->widget = w;
    self->select.radio_group = local;

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
    SelectWidgetClass   class = (SelectWidgetClass) w->core.widget_class;

    if ((group = GetRadioGroup(w)) == NULL)	/* Punt if there is no group */
	return;

   /* Go to the top of the group. */

    for (; group->prev != NULL; group = group->prev) ;

    while (group != NULL) {
	SelectWidget        local_tog = (SelectWidget) group->widget;

	if (local_tog->select.state) {
	    class->select_class.Unset(group->widget, NULL, NULL, 0);
	    Notify(group->widget, (XEvent *) NULL, (String *) NULL, (Cardinal *) 0);
	}
	group = group->next;
    }
}

/*
 * RemoveFromRadioGroup: Removes a select from a RadioGroup.
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
    SelectWidget        self = (SelectWidget) w;
    XGCValues           values;
    XtGCMask            mask;

    values.foreground = XtParent(w)->core.background_pixel;
    values.background = self->select.switchColor;
    mask = GCForeground | GCBackground;

    self->select.inverse_gc = XtGetGC(w, mask, &values);

    values.foreground = self->select.switchColor;
    values.background = XtParent(w)->core.background_pixel;
    mask = GCForeground | GCBackground;

    self->select.norm_gc = XtGetGC(w, mask, &values);
}

/*
 * DestroyGCs: Removes all gc's for the simple menu widget.
 */

static void
DestroyGCs(w)
	Widget              w;
{
    SelectWidget        self = (SelectWidget) w;

    XtReleaseGC(w, self->select.inverse_gc);
    XtReleaseGC(w, self->select.norm_gc);
}

/************************************************************
 *
 * ACTION procedures.
 *
 ************************************************************/

static void
Notify(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    SelectWidget        self = (SelectWidget) w;
    int                 state = (int) self->select.state;

    XtCallCallbacks(w, XtNcallback, (XtPointer) state);
}

static void
Set(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    SelectWidget        self = (SelectWidget) w;
    Boolean             oldstate = self->select.state;

    TurnOffRadioSiblings(w);
    self->select.state = True;
    if (oldstate != self->select.state && XtIsRealized(w))
	DrawSwitch(w);
}

static void
Unset(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    SelectWidget        self = (SelectWidget) w;
    Boolean             oldstate = self->select.state;

    self->select.state = False;
    if (oldstate != self->select.state && XtIsRealized(w))
	DrawSwitch(w);
}

/*
 * Toggle: if we have a menu associated with us, request our parent to
 * pop it up.
 */
static void
Toggle(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    SelectWidget        self = (SelectWidget) w;
    Boolean             oldstate = self->select.state;	/* Others could clear us */

   /* Only change state if we are not selected */
    if (self->select.state == FALSE)
	TurnOffRadioSiblings(w);
    self->select.state = oldstate;
    if (self->select.allownone)
	self->select.state = !oldstate;
    else
	self->select.state = TRUE;
    if (XtIsRealized(w))
	DrawSwitch(w);
}

/* ARGSUSED */
static void
Arm(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    SelectWidget        self = (SelectWidget) w;

    _XpwArmClue(w, self->select.clue);
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

/************************************************************
 *
 * Public interface functions
 *
 ************************************************************/

/* 
 * Puts widget into a given radio group.
 */

void
XpwSelectChangeRadioGroup(w, radio_group)
	Widget              w, radio_group;
{
    SelectWidget        self = (SelectWidget) w;
    RadioGroup         *group;

    RemoveFromRadioGroup(w);

/*
 * If the select that we are about to add is set then we will 
 * unset all selects in the new radio group.
 */

    if (self->select.state && radio_group != NULL)
	XpwSelectUnsetCurrent(radio_group);

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
XpwSelectGetCurrent(w)
	Widget              w;
{
    RadioGroup         *group;

    if ((group = GetRadioGroup(w)) == NULL)
	return (NULL);
    for (; group->prev != NULL; group = group->prev) ;

    while (group != NULL) {
	SelectWidget        local_sel = (SelectWidget) group->widget;

	if (local_sel->select.state)
	    return (local_sel->select.radio_data);
	group = group->next;
    }
    return (NULL);
}

/* 
 * Sets the widget associated with the given radio group.
 */

void
XpwSelectSetCurrent(radio_group, radio_data)
	Widget              radio_group;
	XtPointer           radio_data;
{
    RadioGroup         *group;
    SelectWidget        local_sel;

/* Special case of no radio group. */

    if ((group = GetRadioGroup(radio_group)) == NULL) {
	local_sel = (SelectWidget) radio_group;
	if ((local_sel->select.radio_data == radio_data))
	    if (!local_sel->select.state) {
		Set((Widget) local_sel, (XEvent *) NULL, (String *) NULL,
		    (Cardinal *) 0);
		Notify((Widget) local_sel, (XEvent *) NULL, (String *) NULL,
		       (Cardinal *) 0);
	    }
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
	local_sel = (SelectWidget) group->widget;
	if ((local_sel->select.radio_data == radio_data)) {
	    if (!local_sel->select.state) {	/* if not already set. */
		Set((Widget) local_sel, (XEvent *) NULL, (String *) NULL,
		    (Cardinal *) 0);
		Notify((Widget) local_sel, (XEvent *) NULL, (String *) NULL,
		       (Cardinal *) 0);
	    }
	    return;		/* found it, done */
	}
	group = group->next;
    }
}

/*
 * Unsets all widgets of a given Radio group.
 */

void
XpwSelectUnsetCurrent(radio_group)
	Widget              radio_group;
{
    SelectWidgetClass   class;
    SelectWidget        local_sel = (SelectWidget) radio_group;

   /* Special Case no radio group. */

    if (local_sel->select.state) {
	class = (SelectWidgetClass) local_sel->core.widget_class;
	class->select_class.Unset(radio_group, NULL, NULL, 0);
	Notify(radio_group, (XEvent *) NULL, (String *) NULL, (Cardinal *) 0);
    }
    if (GetRadioGroup(radio_group) == NULL)
	return;
    TurnOffRadioSiblings(radio_group);
}
