/*
 * Combo Box widget.
 *
 * The ComboBox widget maintains a pulldown menu of the elements of the
 * combobox associated with it.
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
 *
 * $Log: ComboBox.c,v $
 * Revision 1.1  1997/11/28 19:55:48  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: ComboBox.c,v 1.1 1997/11/28 19:55:48 rich Exp rich $";

#endif

#include <X11/Xlib.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xosdefs.h>
#include <X11/Xos.h>
#include <X11/Xmu/Drawing.h>
#include "XpwInit.h"
#include "ComboBoxP.h"
#include "Clue.h"
#include "PopupMenu.h"
#include "PmeEntry.h"
#include "MenuArrow.h"
#include "threeDdrawP.h"
#include "Cardinals.h"

#include <stdio.h>

/* Initialization of defaults */

#define offset(field) XtOffsetOf(ComboBoxRec, combobox.field)

static XtResource   resources[] =
{
    {XtNitemList, XtCItemList, XtRItemList, sizeof(String *),
     offset(list), XtRImmediate, (XtPointer) NULL},
    {XtNclue, XtCLabel, XtRString, sizeof(String),
     offset(clue), XtRImmediate, (XtPointer) NULL},
    {XtNcallback, XtCCallback, XtRCallback, sizeof(XtPointer),
     offset(callbacks), XtRCallback, (XtPointer) NULL},
    {XtNonRight, XtCOnRight, XtRBoolean, sizeof(Boolean),
     offset(onright), XtRImmediate, (XtPointer) TRUE},

    /* Label resources */
    LabelResources
    /* ThreeD resouces */
    threeDresources
};

#undef offset

/* Semi Public function definitions. */
static void         Initialize(Widget /*request */ , Widget /*new */ ,
			  ArgList /*args */ , Cardinal * /*num_args */ );
static XtGeometryResult QueryGeometry(Widget /*w */ ,
				      XtWidgetGeometry * /*intended */ ,
				    XtWidgetGeometry * /*return_val */ );
static Boolean      SetValues(Widget /*current */ , Widget /*request */ ,
			      Widget /*new */ , ArgList /*args */ ,
			      Cardinal * /*num_args */ );
static void         Resize(Widget /*wid */ );
static void         Redisplay(Widget /*wid */ , XEvent * /*event */ ,
			      Region /*region */ );
static void         Destroy(Widget /*w */ );

/* Private functions */
static void         GrowList(ComboBoxWidget /*self */ , int /*size */ ,
				 int /*ip */ );
static void         FreeList(ComboBoxWidget /*self */ );
static Dimension    CalculateLongest(ComboBoxWidget /*self */ );
static void         MakePulldowns(ComboBoxWidget /*self */ );
static void         CopyList(ComboBoxWidget /*self */ );
static void         SelectItem(Widget /*w */ , XtPointer /*client_data */ ,
			       XtPointer /*call_data */ );

/* Actions  */
static void         Enter(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         Leave(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );

static XtActionsRec actionsList[] =
{
    {"enter", Enter},
    {"leave", Leave},
};

static char         defaultTranslations[] =
"<EnterWindow>:	enter()\n\
 <LeaveWindow>: leave()\n";

#define superclass (&compositeClassRec)
ComboBoxClassRec    comboBoxClassRec =
{
    {	/* core fields */
	(WidgetClass) superclass,	/* superclass               */
	"ComboBox",			/* class_name               */
	sizeof(ComboBoxRec),		/* widget_size              */
	XpwInitializeWidgetSet,		/* class_initialize         */
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
	Resize,				/* resize                   */
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
    {	/* composite part */
	XtInheritGeometryManager,	/* geometry_manager         */
	XtInheritChangeManaged,		/* change_managed           */
	XtInheritInsertChild,		/* insert_child             */
	XtInheritDeleteChild,		/* delete_child             */
	NULL				/* extension                */
    },
    {	/* ComboBoxClass Fields */
	0				/* extension               */
    }
};

WidgetClass         comboBoxWidgetClass = (WidgetClass) & comboBoxClassRec;

/************************************************************
 *
 * Semi-Public Functions.
 *
 ************************************************************/

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
    ComboBoxWidget            nself = (ComboBoxWidget) new;
    Arg			arg[2];
    Dimension		w, h;

   /* Build list */
    if (nself->combobox.list == NULL) {
	nself->combobox.list = (String *) XtMalloc(2 * sizeof(String *));
	nself->combobox.list[0] = XtNewString(XtName(new));
	nself->combobox.list[1] = NULL;
	nself->combobox.nitems = 1;
    } else
	CopyList(nself);

    nself->combobox.select = 0;
    nself->combobox.label.label = nself->combobox.list[0];
   /* Initialize label */
    _XpwLabelInit(new, &(nself->combobox.label), nself->core.background_pixel,
		  nself->core.depth);
    _XpwThreeDInit(new, &nself->combobox.threeD, nself->core.background_pixel);
    _XpwLabelDefaultSize(new, &(nself->combobox.label), &w, &h);
    w = CalculateLongest(nself);
    w += (nself->core.height != 0)? nself->core.height: h;
    w += 2 * nself->combobox.threeD.shadow_width;
    h += 2 * nself->combobox.threeD.shadow_width;
    if (nself->core.height == 0)
	nself->core.height = h;
    if (nself->core.width == 0)
	nself->core.width = w;

   /* Build children */
    XtSetArg(arg[0], XtNmenuName, "pulldown");
    XtSetArg(arg[1], XtNalignToParent, TRUE);
    nself->combobox.arrow = XtCreateManagedWidget("arrow", menuarrowWidgetClass,
							 new, arg, 2);
    XtSetArg(arg[0], XtNwidth, nself->core.width);

    nself->combobox.menu = XtCreatePopupShell("pulldown", popupMenuWidgetClass,
							 new, arg, 1);
    MakePulldowns(nself);
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
    ComboBoxWidget            new_self = (ComboBoxWidget) new;
    ComboBoxWidget            old_self = (ComboBoxWidget) current;
    Boolean             ret_val = FALSE;

    if (new_self->combobox.list != old_self->combobox.list) {
	FreeList(old_self);
	new_self->combobox.wlist = NULL;
	CopyList(new_self);
    	new_self->combobox.select = 0;
    	new_self->combobox.label.label = new_self->combobox.list[0];
	MakePulldowns(new_self);
    }
    if (_XpwLabelSetValues(current, new, &(old_self->combobox.label),
		 &(new_self->combobox.label), new->core.background_pixel,
			   new->core.depth)) {
	_XpwThreeDDestroyShadow(current, &old_self->combobox.threeD);
	_XpwThreeDAllocateShadowGC(new, &new_self->combobox.threeD,
				   new->core.background_pixel);
	ret_val = TRUE;
    }
    if (new_self->core.sensitive != old_self->core.sensitive)
	ret_val = TRUE;
    if (_XpmThreeDSetValues(new, &old_self->combobox.threeD,
		 &new_self->combobox.threeD, new->core.background_pixel))
	ret_val = TRUE;
    if (new_self->combobox.onright != old_self->combobox.onright) {
	Dimension           h = new_self->core.height;
	Dimension           w = new_self->core.width;
	Dimension           s = new_self->combobox.threeD.shadow_width;
	Dimension           x;

	x = (new_self->combobox.onright) ? (w - h - s) : s;
	h -= 2 * (s + 2);
	XtConfigureWidget(new_self->combobox.arrow, x, s + 2, h, h, 0);
	ret_val = TRUE;
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
    ComboBoxWidget            self = (ComboBoxWidget) w;
    Dimension           width, height;
    XtGeometryResult    ret_val = XtGeometryYes;
    XtGeometryMask      mode = intended->request_mode;

    _XpwLabelDefaultSize(w, &(self->combobox.label), &width, &height);
    width = CalculateLongest(self) + height +
		 2 * self->combobox.threeD.shadow_width;
    height += 2 * self->combobox.threeD.shadow_width;

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
 * Resize.
 */

static void
Resize(wid)
	Widget              wid;
{
    ComboBoxWidget            self = (ComboBoxWidget) wid;
    Dimension           h = self->core.height;
    Dimension           w = self->core.width;
    Dimension           s = self->combobox.threeD.shadow_width;
    Dimension           x;
    Arg			arg[1];

    x = (self->combobox.onright) ? (w - h - s) : s;
    h -= 2 * (s + 2);
    XtConfigureWidget(self->combobox.arrow, x, s + 2, h, h, 0);
    XtSetArg(arg[0], XtNwidth, w);
    XtSetValues(self->combobox.menu, arg, 1);
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
    ComboBoxWidget            self = (ComboBoxWidget) wid;
    Display            *dpy = XtDisplayOfObject(wid);
    Window              win = XtWindowOfObject(wid);
    Dimension           s = self->combobox.threeD.shadow_width;
    Dimension           h = self->core.height;
    Dimension           w = self->core.width;
    Dimension           x;

    if (region == NULL)
	XClearWindow(dpy, win);

   /* Draw shadows around main window */
    _XpwThreeDDrawShadow(wid, event, region, &(self->combobox.threeD), 0, 0,
			 w, h, 1);

    x = (self->combobox.onright) ? s : h + s;
    w -= h;
   /* Draw the combobox */
    _XpwLabelDraw(wid, &(self->combobox.label), event, region, x, s,
		  w - (2 * s), h - (2 * s), FALSE);
}

/*
 * Destroy an resources we allocated.
 */
static void
Destroy(w)
	Widget              w;
{
    ComboBoxWidget            self = (ComboBoxWidget) w;
    int			      i;

   /* Can't call FreeLists since the children are already gone */
    for (i = self->combobox.nitems; i >= 0; i--) {
	if (self->combobox.list != NULL &&
	    self->combobox.list[i] != NULL)
	    XtFree(self->combobox.list[i]);
    }
    _XpwLabelDestroy(w, &(self->combobox.label));
    _XpwThreeDDestroyShadow(w, &(self->combobox.threeD));
}

/************************************************************
 *
 * Private functions
 *
 ************************************************************/

/*
 * Add new items to a list.
 */
static void
GrowList(self, size, ip)
	ComboBoxWidget            self;
	int                 size;
	int                 ip;
{
    int                 i, j = self->combobox.nitems + size + 1;

   /* Grow arrays */
    self->combobox.list = (String *) XtRealloc((XtPointer) self->combobox.list,
					       j * sizeof(String));
    self->combobox.wlist = (Widget *) XtRealloc((XtPointer) self->combobox.wlist,
						j * sizeof(Widget));
   /* Move old items to make space */
    for (i = self->combobox.nitems, j--; i >= ip; i--, j--) {
	self->combobox.list[j] = self->combobox.list[i];
	self->combobox.wlist[j] = self->combobox.wlist[i];
    }
   /* Make new ones as empty */
    for (i = ip, j = size; j > 0; j--, i++) {
	self->combobox.list[i] = NULL;
	self->combobox.wlist[i] = NULL;
    }
   /* Adjust size */
    self->combobox.nitems += size;
}

/*
 * Remove all enties in a list.
 */
static void
FreeList(self)
	ComboBoxWidget            self;
{
    int                 i;

    for (i = self->combobox.nitems; i >= 0; i--) {
	if (self->combobox.list != NULL &&
	    self->combobox.list[i] != NULL)
	    XtFree(self->combobox.list[i]);
	if (self->combobox.wlist != NULL &&
	    self->combobox.wlist[i] != NULL)
	    XtDestroyWidget(self->combobox.wlist[i]);
    }
    if (self->combobox.list != NULL)
	XtFree((XtPointer) self->combobox.list);
    self->combobox.list = NULL;
    if (self->combobox.wlist != NULL)
	XtFree((XtPointer) self->combobox.wlist);
    self->combobox.wlist = NULL;
    self->combobox.nitems = 0;
}

/*
 * Find max height and width of list. Note this code knows about inside of
 * label structure.
 */
static Dimension
CalculateLongest(self)
	ComboBoxWidget	self;
{
    int                 longest, i, len;
    String              *lp;

    longest = 0;

    lp = self->combobox.list;
   /* Scan list elements computing lenght of each */
    for (i = 0; i < self->combobox.nitems; i++) {
        len = strlen(*lp);
        if (self->combobox.label.international)
            len = XmbTextEscapement(self->combobox.label.fontset, *lp, len);
        else
            len = XTextWidth(self->combobox.label.font, *lp, len);

        if (len > longest)
            longest = len;
    }
    return longest;

}

/*
 * Make sure there is a menu entry for every item in list.
 */
static void
MakePulldowns(self)
	ComboBoxWidget            self;
{
    int                 i;
    Arg                 arg[1];

   /* If widget list does not exist, create one */
    if (self->combobox.wlist == NULL) {
	self->combobox.wlist = (Widget *) XtMalloc(sizeof(Widget) *
					    (self->combobox.nitems + 1));
	for (i = 0; i <= self->combobox.nitems; i++)
	    self->combobox.wlist[i] = NULL;
    }
   /* Now create menu popups. */
    for (i = 0; i < self->combobox.nitems; i++) {
	if (self->combobox.list[i] == NULL)
	    continue;
	if (self->combobox.wlist[i] == NULL) {
	    Widget              w;

	    w = XtCreateManagedWidget("item", pmeEntryObjectClass,
				      self->combobox.menu, NULL, 0);
	    XtAddCallback(w, XtNcallback, SelectItem, (XtPointer) i);
	    self->combobox.wlist[i] = w;
	}
	XtSetArg(arg[0], XtNlabel, self->combobox.list[i]);
	XtSetValues(self->combobox.wlist[i], arg, 1);
    }
}

/*
 * Make a copy of the list, so application can free up space
 */
static void
CopyList(self)
	ComboBoxWidget            self;
{

    int                 i;
    String             *nlist;

    if (self->combobox.list == NULL) {
	self->combobox.nitems = 0;
	return;
    }
   /* Figure out how big to make it first */
    for (i = 0; self->combobox.list[i] != NULL; i++) ;
    self->combobox.nitems = i;
    nlist = (String *) XtMalloc((i + 1) * sizeof(String));
    nlist[i + 1] = NULL;
    for (; i >= 0; i--)
	nlist[i] = XtNewString(self->combobox.list[i]);
    self->combobox.list = nlist;
}

/************************************************************
 *
 * ACTION procedures
 *
 ************************************************************/

/*
 * Select an item from the list.
 */
static void
SelectItem(w, client_data, call_data)
	Widget              w;
	XtPointer           client_data;
	XtPointer           call_data;
{
    ComboBoxWidget            self = (ComboBoxWidget) XtParent(XtParent(w));
    int                 sel = (int) client_data;
    XpwComboBoxReturnStruct rs;

    if (sel >= 0 && sel < self->combobox.nitems) {
        self->combobox.select = sel;
    	self->combobox.label.label = self->combobox.list[sel];
    }
    Redisplay((Widget)self, NULL, NULL);

    rs.index = self->combobox.select;
    rs.string = self->combobox.list[rs.index];
    XtCallCallbacks((Widget)self, XtNcallback, (XtPointer) & rs);
}

static void
Enter(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    ComboBoxWidget            self = (ComboBoxWidget) w;

    _XpwArmClue(w, self->combobox.clue);
}

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
 * Reset the combobox.
 */
void
XpwComboBoxNew(w)
	Widget              w;
{
    ComboBoxWidget            self = (ComboBoxWidget) w;

   /* Free all elements from old combobox */
    if (self->combobox.list != NULL)
	FreeList(self);

    self->combobox.list = (String *) XtMalloc(sizeof(String *));
    self->combobox.list[0] = NULL;
    self->combobox.wlist = (Widget *) XtMalloc(sizeof(Widget *));
    self->combobox.wlist[0] = NULL;
    self->combobox.label.label = "";
    self->combobox.select = -1;
}

/*
 * Add an item to a combobox.
 * an after of -1 indicates at the end of the combobox.
 */
void
XpwComboBoxAddItem(w, item, after)
	Widget              w;
	String              item;
	int                 after;
{
    ComboBoxWidget            self = (ComboBoxWidget) w;

   /* Now make room */
    if (after == -1)
	after = self->combobox.nitems;

    GrowList(self, 1, after);
    self->combobox.list[after] = XtNewString(item);
    self->combobox.wlist[after] = NULL;
    MakePulldowns(self);
}

/*
 * Add items to a combobox.
 * an after of -1 indicates at the end of the combobox.
 */
void
XpwComboBoxAddItems(w, items, after)
	Widget              w;
	String             *items;
	int                 after;
{
    ComboBoxWidget            self = (ComboBoxWidget) w;
    int                 i;
    int                 size;

   /* Size the new items */
    for (size = 0; items[size] != NULL; size++) ;

   /* Now make room */
    if (after == -1)
	after = self->combobox.nitems;
    GrowList(self, size, after);
    for (i = 0; i < size; i++, after++) {
	self->combobox.list[after] = XtNewString(items[i]);
	self->combobox.wlist[after] = NULL;
    }
    MakePulldowns(self);
}

/*
 * Remove items from the combobox.
 */
void
XpwComboBoxDeleteItems(w, from, to)
	Widget              w;
	int                 from;
	int                 to;
{
    ComboBoxWidget            self = (ComboBoxWidget) w;
    int                 size;

    if (from < 0 || from > self->combobox.nitems)
	return;
    if (to < 0 || to > self->combobox.nitems)
	return;
    if (to < from)
	return;
    size = self->combobox.nitems - to + 1;
   /* Move old items to make space */
    for (; size >= 0; size--) {
	if (self->combobox.list[from] != NULL)
	    XtFree(self->combobox.list[from]);
	self->combobox.list[from] = self->combobox.list[to];
	if (self->combobox.wlist[from] != NULL)
	    XtDestroyWidget(self->combobox.wlist[from]);
	self->combobox.wlist[from] = self->combobox.wlist[to];
	from++;
	to++;
    }
}

/*
 * Replace an item in the combobox.
 */
void
XpwComboBoxReplaceItem(w, item, index)
	Widget              w;
	String              item;
	int                 index;
{
    ComboBoxWidget            self = (ComboBoxWidget) w;
    String             *lp;

    if (index < 0 || index > self->combobox.nitems)
	return;
    lp = &self->combobox.list[index];
    if (*lp != NULL)
	XtFree(*lp);
    *lp = XtNewString(item);
    MakePulldowns(self);
}

/*
 * Replace items in the combobox.
 */
void
XpwComboBoxReplaceItems(w, items, index)
	Widget              w;
	String             *items;
	int                 index;
{
    ComboBoxWidget            self = (ComboBoxWidget) w;
    int                 i;
    int                 size;
    String             *lp;

   /* Size the new items */
    for (size = 0; items[size] != NULL; size++) ;

   /* Make sure there is space */
    if (index < 0 || index > self->combobox.nitems)
	return;
   /* Truncate the replacement to the size available */
    if ((index + size) > self->combobox.nitems)
	size = self->combobox.nitems - index;
   /* Copy data. */
    lp = &self->combobox.list[index];
    for (i = 0; i < size; i++, lp++) {
	if (*lp != NULL)
	    XtFree(*lp);
	*lp = XtNewString(items[i]);
    }
    MakePulldowns(self);
}

/*
 * Mark an item.
 * An index of -1 indicates the last selected item.
 */
void
XpwComboBoxSetItem(w, index)
	Widget              w;
	int                 index;
{
    ComboBoxWidget            self = (ComboBoxWidget) w;

   /* Make sure there is space */
    if (index < 0 || index > self->combobox.nitems)
	return;
    if (self->combobox.select != index) {
	self->combobox.select = index;
	self->combobox.label.label = self->combobox.list[index];
	Redisplay(w, NULL, NULL);
    }
}

/*
 * Return currently selected item.
 */
void
XpwComboBoxGetSelected(w, rs)
	Widget              w;
	XpwComboBoxReturnStruct *rs;
{
    ComboBoxWidget            self = (ComboBoxWidget) w;

    rs->index = self->combobox.select;
    if (self->combobox.select >= 0 &&
			 self->combobox.select < self->combobox.nitems)
	rs->string = self->combobox.list[self->combobox.select];
    else
	rs->string = NULL;
}

/*
 * Return menu Widget for item.
 */
Widget
XpwComboBoxGetMenu(w, index)
	Widget              w;
	int                 index;
{
    ComboBoxWidget            self = (ComboBoxWidget) w;

   /* Make sure there is space */
    if (index < 0 || index > self->combobox.nitems)
	return NULL;
    return self->combobox.wlist[index];
}



