/*
 * PopupMenu.c 
 *
 *    PopupMenu realizes a popup menu, handling positioning and other functions
 *   for it's children.
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
 * $Log: PopupMenu.c,v $
 * Revision 1.2  1997/10/05 02:17:29  rich
 * Make sure after a resize all children are set to correct width.
 * Use class calls to talk to children.
 *
 * Revision 1.1  1997/10/04 05:08:42  rich
 * Initial revision
 *
 *
 */

#ifndef line
static char         rcsid[] = "$Id: PopupMenu.c,v 1.2 1997/10/05 02:17:29 rich Exp rich $";

#endif

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include "XpwInit.h"
#include "PopupMenuP.h"
#include "PmeEntry.h"
#include "Clue.h"

#include <X11/Xmu/Initer.h>
#include <X11/Xmu/CharSet.h>

#define streq(a, b)        ( strcmp((a), (b)) == 0 )

#define offset(field) XtOffsetOf(PopupMenuRec, popup_menu.field)

static XtResource   resources[] =
{

/*
 * Label Resources.
 */

    {XtNlabel, XtCLabel, XtRString, sizeof(String),
     offset(label_string), XtRString, NULL},
    {XtNlabelClass, XtCLabelClass, XtRPointer, sizeof(WidgetClass),
     offset(label_class), XtRImmediate, (XtPointer) NULL},

/*
 * Layout Resources.
 */

    {XtNrowHeight, XtCRowHeight, XtRDimension, sizeof(Dimension),
     offset(row_height), XtRImmediate, (XtPointer) 0},
    {XtNtopMargin, XtCVerticalMargins, XtRDimension, sizeof(Dimension),
     offset(top_margin), XtRImmediate, (XtPointer) 0},
    {XtNbottomMargin, XtCVerticalMargins, XtRDimension, sizeof(Dimension),
     offset(bottom_margin), XtRImmediate, (XtPointer) 0},

/*
 * Misc. Resources
 */

  {XtNallowShellResize, XtCAllowShellResize, XtRBoolean, sizeof(Boolean),
   XtOffsetOf(PopupMenuRec, shell.allow_shell_resize), XtRImmediate, (XtPointer) TRUE},
  {XtNcursor, XtCCursor, XtRCursor, sizeof(Cursor),
   offset(cursor), XtRImmediate, (XtPointer) None},
  {XtNmenuOnScreen, XtCMenuOnScreen, XtRBoolean, sizeof(Boolean),
   offset(menu_on_screen), XtRImmediate, (XtPointer) TRUE},
  {XtNpopupOnEntry, XtCPopupOnEntry, XtRWidget, sizeof(Widget),
   offset(popup_entry), XtRWidget, NULL},
  {XtNbackingStore, XtCBackingStore, XtRBackingStore, sizeof(int),
   offset(backing_store), XtRImmediate, (XtPointer)(Always + WhenMapped + NotUseful)},

/*
 * 3D resources.
 */
    threeDresources
};

#undef offset

/* Semi Public function definitions. */
static void         ClassInitialize(void);
static void         ClassPartInitialize(WidgetClass /*wc */ );
static void         Initialize(Widget /*request */ , Widget /*new */ ,
				 ArgList /*args */ , Cardinal * /*num_args */ );
static void         Destroy(Widget /*gw */ );
static void         Redisplay(Widget /*w */ , XEvent * /*event */ ,
				 Region /*region */ );
static void         Realize(Widget /*w */ , XtValueMask * /*mask */ ,
				 XSetWindowAttributes * /*attrs */ );
static void         Resize(Widget /*w */ );
static Boolean      SetValues(Widget /*current */ , Widget /*request */ ,
				 Widget /*new */ , ArgList /*args */ , Cardinal * /*num_args */ );
static Boolean      SetValuesHook(Widget /*w */ , ArgList /*arglist */ ,
				 Cardinal * /*num_args */ );
static XtGeometryResult GeometryManager(Widget /*w */ ,
				 XtWidgetGeometry * /*request */ ,
				 XtWidgetGeometry * /*reply */ );
static void         ChangeManaged(Widget /*w */ );

/* Private Function Definitions.  */
static void         MakeSetValuesRequest(Widget /*w */ , int /*width */ ,
				 int /*height */ );
static Dimension    GetMenuWidth(Widget /*w */ , Widget /*w_ent */ );
static Dimension    GetMenuHeight(Widget /*w */ );
static PmeEntryObject GetEventEntry(Widget /*w */ , XEvent * /*event */ );
static void         Popup_Timeout(XtPointer /*client_data */ ,
				 XtIntervalId * /*timerid */ );
static void         DoPopdown(Widget /*w */ , XtPointer /*junk */ ,
				 XtPointer /*garbage */ );
static void         DoPopup(Widget /*w */ );
static void         MaybePopup(Widget /*w */ , XEvent * /*event */ ,
				 String * /*params */ , Cardinal * /*num_params */ );
static void         CreateLabel(Widget /*w */ );
static void         Layout(Widget /*w */ , Dimension * /*width_ret */ ,
				 Dimension * /*height_ret */ );
static void         AddPositionAction(XtAppContext /*app_con */ ,
				 XPointer /*data */ );
static Widget       FindMenu(Widget /*widget */ , String /*name */ );
static void         PositionMenu(Widget /*w */ , XPoint * /*location */ );
static void         MoveMenu(Widget /*w */ , int /*x */ , int /*y */ );
static void         ChangeCursorOnGrab(Widget /*w */ , XtPointer /*junk */ ,
				 XtPointer /*garbage */ );

/* Action Routine Definitions */
static void         PositionMenuAction(Widget /*w */ , XEvent * /*event */ ,
				 String * /*params */ ,
				 Cardinal * /*num_params */ );
static void         Unhighlight(Widget /*w */ , XEvent * /*event */ ,
				 String * /*params */ , Cardinal * /*num_params */ );
static void         Highlight(Widget /*w */ , XEvent * /*event */ ,
				 String * /*params */ , Cardinal * /*num_params */ );
static void         Notify(Widget /*w */ , XEvent * /*event */ ,
				 String * /*params */ , Cardinal * /*num_params */ );


static XtActionsRec actionsList[] =
{
    {"notify", Notify},
    {"highlight", Highlight},
    {"unhighlight", Unhighlight},
    {"MaybePopup", MaybePopup},
};

static char         defaultTranslations[] =
 ":<EnterWindow>:     highlight()             \n\
  :<LeaveWindow>:     unhighlight()            \n\
    :<BtnMotion>:     highlight()  MaybePopup() \n\
        :<BtnUp>:     MenuPopdown() notify() unhighlight()";

static char         cascadeTranslations[] =
":<EnterWindow>:     highlight()             \n\
 :<LeaveWindow>:     unhighlight() MenuPopdown() \n\
   :<BtnMotion>:     highlight()  MaybePopup()     \n\
       :<BtnUp>:     MenuPopdown() notify() unhighlight()";

static CompositeClassExtensionRec extension_rec =
{
    NULL,				/* next_extension */
    NULLQUARK,				/* record_type */
    XtCompositeExtensionVersion,	/* version */
    sizeof(CompositeClassExtensionRec),	/* record_size */
    TRUE,				/* accepts_objects */
};

#define superclass (&overrideShellClassRec)

PopupMenuClassRec   popupMenuClassRec =
{
    {
	(WidgetClass) superclass,	/* superclass            */
	"PopupMenu",			/* class_name            */
	sizeof(PopupMenuRec),		/* size                  */
	ClassInitialize,		/* class_initialize      */
	ClassPartInitialize,		/* class_part_initialize */
	FALSE,				/* Class init'ed         */
	Initialize,			/* initialize            */
	NULL,				/* initialize_hook       */
	Realize,			/* realize               */
	actionsList,			/* actions               */
	XtNumber(actionsList),		/* num_actions           */
	resources,			/* resources             */
	XtNumber(resources),		/* resource_count        */
	NULLQUARK,			/* xrm_class             */
	TRUE,				/* compress_motion       */
	TRUE,				/* compress_exposure     */
	TRUE,				/* compress_enterleave   */
	FALSE,				/* visible_interest      */
	Destroy,			/* destroy               */
	Resize,				/* resize                */
	Redisplay,			/* expose                */
	SetValues,			/* set_values            */
	SetValuesHook,			/* set_values_hook       */
	XtInheritSetValuesAlmost,	/* set_values_almost     */
	NULL,				/* get_values_hook       */
	NULL,				/* accept_focus          */
	XtVersion,			/* intrinsics version    */
	NULL,				/* callback offsets      */
	defaultTranslations,		/* tm_table              */
	NULL,				/* query_geometry        */
	NULL,				/* display_accelerator   */
	NULL				/* extension             */
    },
    {
	GeometryManager,		/* geometry_manager      */
	ChangeManaged,			/* change_managed        */
	XtInheritInsertChild,		/* insert_child          */
	XtInheritDeleteChild,		/* delete_child          */
	NULL				/* extension             */
    },
    {
	NULL				/* Shell extension       */
    },
    {
	NULL				/* Override extension    */
    },
    {
	NULL				/* Popup Menu extension  */
    }
};

WidgetClass         popupMenuWidgetClass = (WidgetClass) & popupMenuClassRec;

#define ForAllChildren(self, childP) \
  for ( (childP) = (PmeEntryObject *) (self)->composite.children ; \
        (childP) < (PmeEntryObject *) ( (self)->composite.children + \
				 (self)->composite.num_children ) ; \
        (childP)++ )

/************************************************************
 *
 * Semi-Public Functions.
 *
 ************************************************************/

/*
 * Initialize the Class.
 */

static void
ClassInitialize()
{
    XpwInitializeWidgetSet();
    XtAddConverter(XtRString, XtRBackingStore, XmuCvtStringToBackingStore,
		   (XtConvertArgList) NULL, (Cardinal) 0);
    XmuAddInitializer(AddPositionAction, NULL);
}

/* 
 * ClassPartInitialize: Make sure extention record is set up.
 */

static void
ClassPartInitialize(wc)
	WidgetClass         wc;
{
    PopupMenuWidgetClass selfc = (PopupMenuWidgetClass) wc;

   /* Make sure that our subclass gets the extension rec too.  */
    extension_rec.next_extension = selfc->composite_class.extension;
    selfc->composite_class.extension = (XtPointer) & extension_rec;
}

/*
 * Initialize the Widget
 */

/* ARGSUSED */
static void
Initialize(request, new, args, num_args)
	Widget              request, new;
	ArgList             args;
	Cardinal           *num_args;
{
    PopupMenuWidget     self = (PopupMenuWidget) new;
    static XtTranslations trans = NULL;

    /* Add translation buffer */
    if (trans == NULL)
	trans = XtParseTranslationTable(cascadeTranslations);

   /* Assume that if our parent is a popup menu that we are a cascade */
    if (XtClass(XtParent(new)) == popupMenuWidgetClass)
	XtOverrideTranslations(new, trans);

    self->popup_menu.entry_timer = (XtIntervalId) NULL;
    self->popup_menu.current_popup = (XtIntervalId) NULL;
    XmuCallInitializers(XtWidgetToApplicationContext(new));

    if (self->popup_menu.label_class == NULL)
	self->popup_menu.label_class = pmeEntryObjectClass;

    self->popup_menu.label = NULL;
    self->popup_menu.entry_set = NULL;
    self->popup_menu.recursive_set_values = FALSE;

    if (self->popup_menu.label_string != NULL)
	CreateLabel(new);

    self->popup_menu.menu_width = TRUE;

    if (self->core.width == 0) {
	self->popup_menu.menu_width = FALSE;
	self->core.width = GetMenuWidth(new, (Widget) NULL);
    }
    self->popup_menu.menu_height = TRUE;

    if (self->core.height == 0) {
	self->popup_menu.menu_height = FALSE;
	self->core.height = GetMenuHeight(new);
    }
    _XpwThreeDInit(new, &self->popup_menu.threeD, self->core.background_pixel);

    /* Add a popup_callback routine for changing the cursor.  */
    XtAddCallback(new, XtNpopupCallback, ChangeCursorOnGrab, (XtPointer) NULL);
}

/*
 * Realize widget, install cursor if needed.
 */

static void
Realize(w, mask, attrs)
	Widget              w;
	XtValueMask        *mask;
	XSetWindowAttributes *attrs;
{
    PopupMenuWidget     self = (PopupMenuWidget) w;

    attrs->cursor = self->popup_menu.cursor;
    *mask |= CWCursor;
    if ((self->popup_menu.backing_store == Always) ||
	(self->popup_menu.backing_store == NotUseful) ||
	(self->popup_menu.backing_store == WhenMapped)) {
	*mask |= CWBackingStore;
	attrs->backing_store = self->popup_menu.backing_store;
    } else
	*mask &= ~CWBackingStore;

    (*superclass->core_class.realize) (w, mask, attrs);
}

/*
 * Set widget resources.
 */

/* ARGSUSED */
static              Boolean
SetValues(current, request, new, args, num_args)
	Widget              current, request, new;
	ArgList             args;
	Cardinal           *num_args;
{
    PopupMenuWidget     self_old = (PopupMenuWidget) current;
    PopupMenuWidget     self_new = (PopupMenuWidget) new;
    Boolean             ret_val = FALSE, layout = FALSE;

    if (!XtIsRealized(current))
	return (FALSE);

    if (!self_new->popup_menu.recursive_set_values) {
	if (self_new->core.width != self_old->core.width) {
	    self_new->popup_menu.menu_width = (self_new->core.width != 0);
	    layout = TRUE;
	}
	if (self_new->core.height != self_old->core.height) {
	    self_new->popup_menu.menu_height = (self_new->core.height != 0);
	    layout = TRUE;
	}
    }
    if (self_old->popup_menu.cursor != self_new->popup_menu.cursor)
	XDefineCursor(XtDisplay(new),
		      XtWindow(new), self_new->popup_menu.cursor);

    if (self_old->popup_menu.label_string != self_new->popup_menu.label_string)
	if (self_new->popup_menu.label_string == NULL)	/* Destroy. */
	    XtDestroyWidget((Widget) self_old->popup_menu.label);
	else if (self_old->popup_menu.label_string == NULL)	/* Create. */
	    CreateLabel(new);
	else {			/* Change. */
	    Arg                 arglist[1];

	    XtSetArg(arglist[0], XtNlabel, self_new->popup_menu.label_string);
	    XtSetValues((Widget) self_new->popup_menu.label, arglist, 1);
	}
    if (self_old->popup_menu.label_class != self_new->popup_menu.label_class)
	XtAppWarning(XtWidgetToApplicationContext(new),
		     "No Dynamic class change of the PopupMenu Label.");

    if ((self_old->popup_menu.top_margin != self_new->popup_menu.top_margin) ||
	(self_old->popup_menu.bottom_margin != self_new->popup_menu.bottom_margin)) {
	layout = TRUE;
	ret_val = TRUE;
    }
    if (_XpmThreeDSetValues(new, &self_old->popup_menu.threeD,
	  &self_new->popup_menu.threeD, self_new->core.background_pixel))
	ret_val = TRUE;

    if (layout)
	Layout(new, (Dimension *) NULL, (Dimension *) NULL);

    return (ret_val);
}

/* 
 * If the user actually passed a width and height to the widget
 * then this MUST be used, rather than our newly calculated width and
 * height.
 */

static              Boolean
SetValuesHook(w, arglist, num_args)
	Widget              w;
	ArgList             arglist;
	Cardinal           *num_args;
{
    Cardinal            i;
    Dimension           width, height;

    width = w->core.width;
    height = w->core.height;

    for (i = 0; i < *num_args; i++) {
	if (streq(arglist[i].name, XtNwidth))
	    width = (Dimension) arglist[i].value;
	if (streq(arglist[i].name, XtNheight))
	    height = (Dimension) arglist[i].value;
    }

    if ((width != w->core.width) || (height != w->core.height))
	MakeSetValuesRequest(w, width, height);
    return (FALSE);
}

/*
 *  Change size of widget.
 */

static void
Resize(w)
	Widget              w;
{
    PopupMenuWidget     self = (PopupMenuWidget) w;
    PmeEntryObject     *entry;

    if (!XtIsRealized(w))
	return;

    ForAllChildren(self, entry)	/* reset width of all entries. */
	if (XtIsManaged((Widget) * entry))
	(*entry)->rectangle.width = self->core.width;

    Redisplay(w, (XEvent *) NULL, (Region) NULL);
}


/*
 * Handle children resizing.
 */

static              XtGeometryResult
GeometryManager(w, request, reply)
	Widget              w;
	XtWidgetGeometry   *request, *reply;
{
    PopupMenuWidget     self = (PopupMenuWidget) XtParent(w);
    PmeEntryObject      entry = (PmeEntryObject) w;
    XtGeometryMask      mode = request->request_mode;
    XtGeometryResult    answer;
    Dimension           old_height, old_width;

    if (!(mode & CWWidth) && !(mode & CWHeight))
	return (XtGeometryNo);

    reply->width = request->width;
    reply->height = request->height;

    old_width = entry->rectangle.width;
    old_height = entry->rectangle.height;

    Layout(w, &(reply->width), &(reply->height));

    if ((reply->width == request->width) &&
	(reply->height == request->height)) {

	if (mode & XtCWQueryOnly) {	/* Actually perform the layout. */
	    entry->rectangle.width = old_width;
	    entry->rectangle.height = old_height;
	} else {
	    Layout((Widget) self, (Dimension *) NULL, (Dimension *) NULL);
	}
	answer = XtGeometryDone;
    } else {
	entry->rectangle.width = old_width;
	entry->rectangle.height = old_height;

	if (((reply->width == request->width) && !(mode & CWHeight)) ||
	    ((reply->height == request->height) && !(mode & CWWidth)) ||
	    ((reply->width == request->width) &&
	     (reply->height == request->height)))
	    answer = XtGeometryNo;
	else {
	    answer = XtGeometryAlmost;
	    reply->request_mode = 0;
	    if (reply->width != request->width)
		reply->request_mode |= CWWidth;
	    if (reply->height != request->height)
		reply->request_mode |= CWHeight;
	}
    }
    return (answer);
}

/* 
 * Handle children changeing managment.
 */

static void
ChangeManaged(w)
	Widget              w;
{
    PopupMenuWidget     self = (PopupMenuWidget) w;
    Dimension           s = self->popup_menu.threeD.shadow_width;
    PmeEntryObject     *entry;

    Layout(w, (Dimension *) NULL, (Dimension *) NULL);

    ForAllChildren(self, entry)	/* reset width of all entries. */
	if (XtIsManaged((Widget) * entry))
	(*entry)->rectangle.width = self->core.width - 4 * s;

}

/*
 *  Redisplay widget.
 */

/* ARGSUSED */
static void
Redisplay(w, event, region)
	Widget              w;
	XEvent             *event;
	Region              region;
{
    PopupMenuWidget     self = (PopupMenuWidget) w;
    PmeEntryObject     *entry;
    PmeEntryObjectClass class;

    if (region == NULL)
	XClearWindow(XtDisplay(w), XtWindow(w));
    _XpwThreeDDrawShadow(w, event, NULL, &self->popup_menu.threeD,
		       0, 0, self->core.width, self->core.height, FALSE);
   /*
    * Check and Paint each of the entries - including the label.
    */

    ForAllChildren(self, entry) {
	if (!XtIsManaged((Widget) * entry))
	    continue;

	class = (PmeEntryObjectClass) (*entry)->object.widget_class;

	if (class->rect_class.expose != NULL)
	    (class->rect_class.expose) ((Widget) * entry, event, region);
    }
}


/*
 *  Destroy resources used by Widget.
 */
static void
Destroy(gw)
	Widget              gw;
{
    PopupMenuWidget     w = (PopupMenuWidget) gw;

    _XpwThreeDDestroyShadow(gw, &w->popup_menu.threeD);
}

/************************************************************
 *
 * Private Functions.
 *
 ************************************************************/

/*
 * Pop the cascaded menus down when the parent popsdown.
 */
static void
DoPopdown(w, junk, garbage)
	Widget              w;
	XtPointer           junk;
	XtPointer           garbage;
{
    PopupMenuWidget     self = (PopupMenuWidget) w;

    if (self->popup_menu.current_popup != NULL) {
	XtPopdown(self->popup_menu.current_popup);
	self->popup_menu.current_popup = NULL;
    }
    XtRemoveCallback(w, XtNpopdownCallback, DoPopdown, (XtPointer) NULL);
}

/*
 * Timer expired on a cascade, pop it up.
 */
static void
Popup_Timeout(client_data, timerid)
	XtPointer           client_data;
	XtIntervalId       *timerid;
{
    PopupMenuWidget     self = (PopupMenuWidget) client_data;
    Widget              w = (Widget) self;

    self->popup_menu.entry_timer = (XtIntervalId) NULL;
    DoPopup(w);

}


/*
 * Actual work of poping up a menu.
 */
static void
DoPopup(w)
	Widget              w;
{
    PopupMenuWidget     self = (PopupMenuWidget) w;
    PmeEntryObject      entry = self->popup_menu.entry_set;
    Widget              child = NULL;
    Arg                 arglist[2];
    Position            x, y;
    PmeEntryObjectClass class;
    char	       *menu;

   /* Make sure we have a submenu! */
    if (entry == NULL)
	return;

    class = (PmeEntryObjectClass) entry->object.widget_class;
    menu = (class->pme_entry_class.getmenuname) ((Widget) entry);
    if (menu == NULL)
	return;

   /* Ok... find pointer to actual widget to popup! */
    for (; w != NULL; w = XtParent(w)) {
	if ((child = XtNameToWidget(w, menu)) != NULL)
	    break;
    }
   /* Make sure we found a widget */
    if (child == NULL)
	return;
   /* We have motion, kill the timer */
    if (self->popup_menu.entry_timer != (XtIntervalId) NULL) {
	XtRemoveTimeOut(self->popup_menu.entry_timer);
	self->popup_menu.entry_timer = (XtIntervalId) NULL;
    }
   /* Not realized, realize it first */
    if (!XtIsRealized(child))
	XtRealizeWidget(child);

   /* Next translate the cordinates from widget to screen */
    XtTranslateCoords(w, entry->rectangle.x + entry->rectangle.width +
		      self->popup_menu.threeD.shadow_width,
		      entry->rectangle.y + (entry->rectangle.height / 2),
		      &x, &y);
   /* Figure out where to place it when it is shown */
    y = y - (child->core.height / 2);

   /* Make sure it is still all on the screen */
    if (x >= 0) {
	int                 scr_width = WidthOfScreen(XtScreen(child));

	if (x + child->core.width > scr_width)
	    x = scr_width - child->core.width;
    }
    if (x < 0)
	x = 0;

    if (y >= 0) {
	int                 scr_height = HeightOfScreen(XtScreen(child));

	if (y + child->core.height > scr_height)
	    y = scr_height - child->core.height;
    }
    if (y < 0)
	y = 0;

   /* Mark it as active */
    self->popup_menu.current_popup = child;
    XtSetArg(arglist[0], XtNx, x);
    XtSetArg(arglist[1], XtNy, y);
    XtSetValues(child, arglist, 2);
    XtPopup(child, XtGrabNone);
   /* Make sure cascade menus pop down with parent */
    XtAddCallback(w, XtNpopdownCallback, DoPopdown, (XtPointer) NULL);
}

/* 
 * Creates the label object and makes sure it is the first child in
 * in the list.
 */

static void
CreateLabel(w)
	Widget              w;
{
    PopupMenuWidget     self = (PopupMenuWidget) w;
    Widget             *child, *next_child;
    int                 i;
    Arg                 args[2];

    if ((self->popup_menu.label_string == NULL) ||
	(self->popup_menu.label != NULL)) {
	char                error_buf[BUFSIZ];

	(void) sprintf(error_buf, "Xpw Popup Menu Widget: %s or %s, %s",
		       "label string is NULL", "label already exists",
		       "no label is being created.");
	XtAppWarning(XtWidgetToApplicationContext(w), error_buf);
	return;
    }
    XtSetArg(args[0], XtNlabel, self->popup_menu.label_string);
    XtSetArg(args[1], XtNjustify, XtJustifyCenter);
    self->popup_menu.label = (PmeEntryObject)
	XtCreateManagedWidget("menuLabel",
			      self->popup_menu.label_class, w, args, 2);

    next_child = NULL;
    for (child = self->composite.children + self->composite.num_children,
	 i = self->composite.num_children; i > 0; i--, child--) {
	if (next_child != NULL)
	    *next_child = *child;
	next_child = child;
    }
    *child = (Widget) self->popup_menu.label;
}

/*
 * Lay out children.
 *
 * if width == NULL || height == NULL then it assumes the you do not care
 * about the return values, and just want a relayout.
 *
 * if this is not the case then it will set width_ret and height_ret
 * to be width and height that the child would get if it were layed out
 * at this time.
 *
 * +++ "w" can be the popup menu widget or any of its object children.
 */

static void
Layout(w, width_ret, height_ret)
	Widget              w;
	Dimension          *width_ret, *height_ret;
{
    PmeEntryObject      current_child, *child;
    PopupMenuWidget     self;
    Dimension           width, height;
    Boolean             do_layout = ((height_ret == NULL) || (width_ret == NULL));
    Boolean             allow_change_size;

    height = 0;

    if (XtIsSubclass(w, popupMenuWidgetClass)) {
	self = (PopupMenuWidget) w;
	current_child = NULL;
    } else {
	self = (PopupMenuWidget) XtParent(w);
	current_child = (PmeEntryObject) w;
    }

    allow_change_size = (!XtIsRealized((Widget) self) ||
			 (self->shell.allow_shell_resize));

    if (self->popup_menu.menu_height)
	height = self->core.height;
    else if (do_layout) {
	height = self->popup_menu.top_margin + self->popup_menu.threeD.shadow_width;
	ForAllChildren(self, child) {
	    if (!XtIsManaged((Widget) * child))
		continue;

	    if ((self->popup_menu.row_height != 0) &&
		(*child != self->popup_menu.label))
		(*child)->rectangle.height = self->popup_menu.row_height
		    + 2 * self->popup_menu.threeD.shadow_width;

	    (*child)->rectangle.y = height + self->popup_menu.threeD.shadow_width;
	    (*child)->rectangle.x = 2 * self->popup_menu.threeD.shadow_width;
	    height += (*child)->rectangle.height +
		2 * self->popup_menu.threeD.shadow_width;
	}
	height += self->popup_menu.bottom_margin +
	    self->popup_menu.threeD.shadow_width;
    } else {
	if ((self->popup_menu.row_height != 0) &&
	    (current_child != self->popup_menu.label))
	    height = self->popup_menu.row_height
		+ 2 * self->popup_menu.threeD.shadow_width;
    }

    if (self->popup_menu.menu_width)
	width = self->core.width;
    else if (allow_change_size)
	width = GetMenuWidth((Widget) self, (Widget) current_child)
	    + 4 * self->popup_menu.threeD.shadow_width;
    else
	width = self->core.width;

    if (do_layout) {
	ForAllChildren(self, child)
	    if (XtIsManaged((Widget) * child))
	    (*child)->rectangle.width = width -
		4 * self->popup_menu.threeD.shadow_width;

	if (allow_change_size)
	    MakeSetValuesRequest((Widget) self, width, height);
    } else {
	*width_ret = width + 2 * self->popup_menu.threeD.shadow_width;
	if (height != 0)
	    *height_ret = height + 2 * self->popup_menu.threeD.shadow_width;
    }
}

/*
 *  Adds the XpwPositionPopupMenu action to the global action list for
 *  this appcon.
 */

/* ARGSUSED */
static void
AddPositionAction(app_con, data)
	XtAppContext        app_con;
	XPointer            data;
{
    static XtActionsRec pos_action[] =
    {
	{"XpwPositionPopupMenu", PositionMenuAction},
    };

    XtAppAddActions(app_con, pos_action, XtNumber(pos_action));
}

/*
 *  Find the menu give a name and reference widget.
 */

static              Widget
FindMenu(widget, name)
	Widget              widget;
	String              name;
{
    Widget              w, menu;

    for (w = widget; w != NULL; w = XtParent(w))
	if ((menu = XtNameToWidget(w, name)) != NULL)
	    return (menu);
    return (NULL);
}

/* 
 *  Places the menu
 */

static void
PositionMenu(w, location)
	Widget              w;
	XPoint             *location;
{
    PopupMenuWidget     self = (PopupMenuWidget) w;
    PmeEntryObject      child;
    XPoint              t_point;

    if (location == NULL) {
	Window              junk1, junk2;
	int                 root_x, root_y, junkX, junkY;
	unsigned int        junkM;

	location = &t_point;
	if (XQueryPointer(XtDisplay(w), XtWindow(w), &junk1, &junk2,
		    &root_x, &root_y, &junkX, &junkY, &junkM) == FALSE) {

	    XtAppWarning(XtWidgetToApplicationContext(w),
			 "Xpw Popup Menu Widget: Could not find location of mouse pointer");
	    return;
	}
	location->x = (short) root_x;
	location->y = (short) root_y;
    }
   /*
    * The width will not be correct unless it is realized.
    */

    XtRealizeWidget(w);

    location->x -= (Position) w->core.width / 2;

    if (self->popup_menu.popup_entry == NULL)
	child = self->popup_menu.label;
    else
	child = self->popup_menu.popup_entry;

    if (child != NULL)
	location->y -= child->rectangle.y + child->rectangle.height / 2;

    MoveMenu(w, (Position) location->x, (Position) location->y);
}

/*  
 *   Actually moves the menu, may force it to to be fully visable if
 *     menu_on_screen is TRUE.
 */

static void
MoveMenu(w, x, y)
	Widget              w;
	Position            x, y;
{
    Arg                 arglist[2];
    PopupMenuWidget     self = (PopupMenuWidget) w;

    if (self->popup_menu.menu_on_screen) {
	int                 width = w->core.width + 2 * w->core.border_width +
	2 * self->popup_menu.threeD.shadow_width;
	int                 height = w->core.height + 2 * w->core.border_width +
	2 * self->popup_menu.threeD.shadow_width;

	if (x >= 0) {
	    int                 scr_width = WidthOfScreen(XtScreen(w));

	    if (x + width > scr_width)
		x = scr_width - width;
	}
	if (x < 0)
	    x = 0;

	if (y >= 0) {
	    int                 scr_height = HeightOfScreen(XtScreen(w));

	    if (y + height > scr_height)
		y = scr_height - height;
	}
	if (y < 0)
	    y = 0;
    }
    XtSetArg(arglist[0], XtNx, x);
    XtSetArg(arglist[1], XtNy, y);
    XtSetValues(w, arglist, 2);
}

/*
 *  Changes the cursor on the active grab to the one specified in out resource
 *     list.
 */

/* ARGSUSED */
static void
ChangeCursorOnGrab(w, junk, garbage)
	Widget              w;
	XtPointer           junk, garbage;
{
    PopupMenuWidget     self = (PopupMenuWidget) w;

   /*
    * The event mask here is what is currently in the MIT implementation.
    * There really needs to be a way to get the value of the mask out
    * of the toolkit (CDP 5/26/89).
    */

    XChangeActivePointerGrab(XtDisplay(w), ButtonPressMask | ButtonReleaseMask,
			     self->popup_menu.cursor,
			     XtLastTimestampProcessed(XtDisplay(w)));
}

/*
 * Makes a (possibly recursive) call to SetValues,
 * I take great pains to not go into an infinite loop.
 */

static void
MakeSetValuesRequest(w, width, height)
	Widget              w;
	Dimension           width, height;
{
    PopupMenuWidget     self = (PopupMenuWidget) w;
    Arg                 arglist[2];

    if (!self->popup_menu.recursive_set_values) {
	if ((self->core.width != width) || (self->core.height != height)) {
	    self->popup_menu.recursive_set_values = TRUE;
	    XtSetArg(arglist[0], XtNwidth, width);
	    XtSetArg(arglist[1], XtNheight, height);
	    XtSetValues(w, arglist, 2);
	} else if (XtIsRealized((Widget) self))
	    Redisplay((Widget) self, (XEvent *) NULL, (Region) NULL);
    }
    self->popup_menu.recursive_set_values = FALSE;
}

/*
 * Compute width of menu we are going to pop up.
 */

static              Dimension
GetMenuWidth(w, w_ent)
	Widget              w, w_ent;
{
    PmeEntryObject      cur_child = (PmeEntryObject) w_ent;
    PopupMenuWidget     self = (PopupMenuWidget) w;
    Dimension           width, widest = (Dimension) 0;
    PmeEntryObject     *child;

    if (self->popup_menu.menu_width)
	return (self->core.width);

    ForAllChildren(self, child) {
	XtWidgetGeometry    preferred;

	if (!XtIsManaged((Widget) * child))
	    continue;

	if (*child != cur_child) {
	    XtQueryGeometry((Widget) * child, (XtWidgetGeometry *) NULL, &preferred);

	    if (preferred.request_mode & CWWidth)
		width = preferred.width;
	    else
		width = (*child)->rectangle.width;
	} else
	    width = (*child)->rectangle.width;

	if (width > widest)
	    widest = width;
    }

    return (widest);
}

/* 
 * Compute height of menu.
 */

static              Dimension
GetMenuHeight(w)
	Widget              w;
{
    PopupMenuWidget     self = (PopupMenuWidget) w;
    PmeEntryObject     *child;
    Dimension           height;

    if (self->popup_menu.menu_height)
	return (self->core.height);

    height = self->popup_menu.top_margin + self->popup_menu.bottom_margin
	+ 2 * self->popup_menu.threeD.shadow_width;

    if (self->popup_menu.row_height == 0) {
	ForAllChildren(self, child)
	    if (XtIsManaged((Widget) * child))
	    height += (*child)->rectangle.height
		+ 2 * self->popup_menu.threeD.shadow_width;
    } else
	height += self->popup_menu.row_height * self->composite.num_children;

    return (height);
}

/* 
 * Gets an child given an event that has X and Y coords.
 */

static              PmeEntryObject
GetEventEntry(w, event)
	Widget              w;
	XEvent             *event;
{
    Position            x_loc = 0, y_loc = 0;
    PopupMenuWidget     self = (PopupMenuWidget) w;
    PmeEntryObject     *child;

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
		   "Unknown event type in GetEventEntry().");
	break;
    }

    if ((x_loc < 0) || (x_loc >= (int) self->core.width) || (y_loc < 0) ||
	(y_loc >= (int) self->core.height))
	return (NULL);

    ForAllChildren(self, child) {
	if (!XtIsManaged((Widget) * child))
	    continue;

	if (((*child)->rectangle.y < y_loc) &&
	    ((*child)->rectangle.y + (int) (*child)->rectangle.height > y_loc))
	    if (*child == self->popup_menu.label)
		return (NULL);	/* cannot select the label. */
	    else
		return (*child);
    }

    return (NULL);
}

/************************************************************
 *
 * ACTION procedures.
 *
 ************************************************************/

/*
 * Determine mouse position for popup.
 */

/* ARGSUSED */
static void
PositionMenuAction(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    Widget              menu;
    XPoint              loc;

    if (*num_params != 1) {
	char                error_buf[BUFSIZ];

	(void) sprintf(error_buf, "%s %s",
	  "Xpw - PopupMenuWidget: position menu action expects only one",
		       "parameter which is the name of the menu.");
	XtAppWarning(XtWidgetToApplicationContext(w), error_buf);
	return;
    }
    if ((menu = FindMenu(w, params[0])) == NULL) {
	char                error_buf[BUFSIZ];

	(void) sprintf(error_buf, "%s '%s'",
	"Xpw - PopupMenuWidget: could not find menu named: ", params[0]);
	XtAppWarning(XtWidgetToApplicationContext(w), error_buf);
	return;
    }
    switch (event->type) {
    case ButtonPress:
    case ButtonRelease:
	loc.x = event->xbutton.x_root;
	loc.y = event->xbutton.y_root;
	PositionMenu(menu, &loc);
	break;
    case EnterNotify:
    case LeaveNotify:
	loc.x = event->xcrossing.x_root;
	loc.y = event->xcrossing.y_root;
	PositionMenu(menu, &loc);
	break;
    case MotionNotify:
	loc.x = event->xmotion.x_root;
	loc.y = event->xmotion.y_root;
	PositionMenu(menu, &loc);
	break;
    default:
	PositionMenu(menu, (XPoint *) NULL);
	break;
    }
}

/*
 * Unhighlight last selected widget.
 */

/* ARGSUSED */
static void
Unhighlight(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    PopupMenuWidget     self = (PopupMenuWidget) w;
    PmeEntryObject      entry = self->popup_menu.entry_set;
    Dimension           s = self->popup_menu.threeD.shadow_width;
    PmeEntryObjectClass class;

    if (entry == NULL)
	return;

    _XpwThreeDEraseShadow(w, event, NULL, &self->popup_menu.threeD,
			  entry->rectangle.x - s, entry->rectangle.y - s,
	entry->rectangle.width + 2 * s, entry->rectangle.height + 2 * s);
    if (self->popup_menu.entry_timer != (XtIntervalId) NULL) {
	XtRemoveTimeOut(self->popup_menu.entry_timer);
	self->popup_menu.entry_timer = (XtIntervalId) NULL;
    }
    self->popup_menu.entry_set = NULL;
    class = (PmeEntryObjectClass) entry->object.widget_class;
    (class->pme_entry_class.unhighlight) ((Widget) entry);
}

/* 
 * Highlight widget mouse is over.
 */

/* ARGSUSED */
static void
Highlight(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    PopupMenuWidget     self = (PopupMenuWidget) w;
    PmeEntryObject      entry;
    Dimension           s = self->popup_menu.threeD.shadow_width;
    PmeEntryObjectClass class;

    if (!XtIsSensitive(w))
	return;

    entry = GetEventEntry(w, event);

    if (entry == self->popup_menu.entry_set)
	return;

    Unhighlight(w, event, params, num_params);

    if (entry == NULL)
	return;

    if (!XtIsSensitive((Widget) entry)) {
	self->popup_menu.entry_set = NULL;
	return;
    }

    class = (PmeEntryObjectClass) entry->object.widget_class;

    _XpwThreeDDrawShadow(w, event, NULL, &self->popup_menu.threeD,
			 entry->rectangle.x - s, entry->rectangle.y - s,
	 entry->rectangle.width + 2 * s, entry->rectangle.height + 2 * s,
			 FALSE);
    self->popup_menu.entry_set = entry;
    DoPopdown(w, NULL, NULL);
    if ((class->pme_entry_class.getmenuname) ((Widget) entry) != NULL &&
	self->popup_menu.entry_timer == (XtIntervalId) NULL) {
	self->popup_menu.entry_timer = XtAppAddTimeOut(
			XtWidgetToApplicationContext((Widget) self), 750,
					Popup_Timeout, (XtPointer) self);
    }
    (class->pme_entry_class.highlight) ((Widget) entry);
}

/*
 * Handle Notify event.
 */

/* ARGSUSED */
static void
Notify(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    PopupMenuWidget     self = (PopupMenuWidget) w;
    PmeEntryObject      entry = self->popup_menu.entry_set;
    PmeEntryObjectClass class;

    if ((entry == NULL) || !XtIsSensitive((Widget) entry))
	return;

    class = (PmeEntryObjectClass) entry->object.widget_class;
    (class->pme_entry_class.notify) ((Widget) entry);
}

/*
 * Popup a cascade if mouse is past midway point.
 */
/* ARGSUSED */
static void
MaybePopup(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    PopupMenuWidget     self = (PopupMenuWidget) w;
    int                 x;

   /* If nothing selected or not a motion event, forget it */
    if (event == NULL || event->type != MotionNotify)
	return;

    x = event->xmotion.x;

   /* Now make sure we are moving in the right side of the menu */
    if (x > self->core.width / 2)
	 DoPopup(w);

}

