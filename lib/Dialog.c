/*
 * Dialog Widget.
 *
 * While not realy a widget, the dialog widget provides a simple way to display
 * a popup dialog window.
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
 * $Log: Dialog.c,v $
 * Revision 1.3  1998/01/24 20:43:27  rich
 * Fixed GeometryManager so it works correctly.
 * Don't give child any size hints, let it fit itself correctly.
 * Size and gap need to be int so gap can be negative.
 * Call changemanaged to relayout the children before popup.
 *
 * Revision 1.2  1997/12/06 04:14:49  rich
 * Added color images for default Icons.
 * Allow control over type of grab.
 * Support for IconMask.
 *
 * Revision 1.1  1997/11/28 19:56:42  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: Dialog.c,v 1.3 1998/01/24 20:43:27 rich Exp $";

#endif

#include <X11/Xlib.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xosdefs.h>
#include <X11/Xos.h>
#include <X11/Shell.h>
#include <X11/Xmu/Misc.h>
#include <X11/Xmu/CharSet.h>
#include <X11/Xmu/Converters.h>
#include "Cardinals.h"
#include "XpwInit.h"
#include "DialogShell.h"
#include "RowCol.h"
#include "DialogP.h"
#include "Label.h"
#include "Line.h"
#include "Command.h"
#include "TextLine.h"
#include "MenuBar.h"
#include "focus.h"

#include <stdio.h>

/* Semi Public function definitions. */
static void         ClassInitialize(void);
static void         Initialize(Widget /*request */ , Widget /*new */ ,
			  ArgList /*args */ , Cardinal * /*num_args */ );
static Boolean      SetValues(Widget /*current */ , Widget /*request */ ,
			      Widget /*new */ , ArgList /*args */ ,
			      Cardinal * /*num_args */ );
static XtGeometryResult QueryGeometry(Widget /*w */ ,
				      XtWidgetGeometry * /*constraint */ ,
				      XtWidgetGeometry * /*preferred */ );
static void         Resize(Widget /*w */ );
static void         Redisplay(Widget /*wid */ , XEvent * /*event */ ,
			      Region /*region */ );
static XtGeometryResult GeometryManager(Widget /*w */ ,
					XtWidgetGeometry * /*request */ ,
					XtWidgetGeometry * /*reply */ );
static void         ChangeManaged(Widget /*w */ );
static Cardinal     InsertPosition(Widget /*w */ );
static void         InsertChild(Widget /*w */ );
static void         Destroy(Widget /*w */ );

/* Private functions */
static void         ComputeSize(DialogWidget /* self */ ,
				Dimension * /* width */ ,
				Dimension * /* height */ );
static void         Layout(DialogWidget /* self */ ,
			   Dimension /* width */ ,
			   Dimension /* height */ );
static int          FindWidget(DialogWidget /*self */ , int /* start */ ,
			       int /* dir */ , Widget /* w */ );
static Widget       MakeShell(Widget /* parent */ , String /* name */ ,
			      String /* cname */ , Arg * /* args */ ,
			      int /* count */ , Pixmap /* icon */ ,
			      Pixmap /* icon_mask */ );
static Pixmap	    MakePixmap(Widget /*parent*/, char **/*data*/,
			      char */*data_bit*/, int /*width*/,
			      int /*height*/, Pixmap */*mask*/);

/* Action functions */
static void         ButtonCall(Widget /*w */ , XtPointer /*client_data */ ,
			       XtPointer /*call_data */ );
static void         DestroyCall(Widget /*w */ , XtPointer /*client_data */ ,
				XtPointer /*call_data */ );
static void         Dismiss(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         Popup(Widget /*w */ , XtPointer /*client_data */ ,
			  XtPointer /*call_data */ );
static void         MoveFocus(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         Notify(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         Iconify(Widget /* w */ , XtPointer /* client_data */ ,
		       XEvent * /* event */ , Boolean * /* dispatch */ );

/* type converters */
Boolean             cvtStringToButtonType(Display * /*dpy */ ,
					  XrmValuePtr /*args */ ,
					  Cardinal * /*num_args */ ,
					  XrmValuePtr /*from */ ,
					  XrmValuePtr /*to */ ,
				       XtPointer * /*converter_data */ );
Boolean             cvtButtonTypeToString(Display * /*dpy */ ,
					  XrmValuePtr /*args */ ,
					  Cardinal * /*num_args */ ,
					  XrmValuePtr /*from */ ,
					  XrmValuePtr /*to */ ,
				       XtPointer * /*converter_data */ );
Boolean             cvtStringToDialogType(Display * /*dpy */ ,
					  XrmValuePtr /*args */ ,
					  Cardinal * /*num_args */ ,
					  XrmValuePtr /*from */ ,
					  XrmValuePtr /*to */ ,
				       XtPointer * /*converter_data */ );
Boolean             cvtDialogTypeToString(Display * /*dpy */ ,
					  XrmValuePtr /*args */ ,
					  Cardinal * /*num_args */ ,
					  XrmValuePtr /*from */ ,
					  XrmValuePtr /*to */ ,
				       XtPointer * /*converter_data */ );
extern Boolean      cvtStringToPackingType(Display * /*dpy */ ,
					   XrmValuePtr /*args */ ,
					   Cardinal * /*num_args */ ,
					   XrmValuePtr /*from */ ,
					   XrmValuePtr /*to */ ,
				       XtPointer * /*converter_data */ );
extern Boolean      cvtPackingTypeToString(Display * /*dpy */ ,
					   XrmValuePtr /*args */ ,
					   Cardinal * /*num_args */ ,
					   XrmValuePtr /*from */ ,
					   XrmValuePtr /*to */ ,
				       XtPointer * /*converter_data */ );

static XtActionsRec actions[] =
{
    {"Dismiss", Dismiss},
    {"MoveFocus", MoveFocus},
    {"Notify", Notify},
};

/* Default Translation table.  */
static char         defaultTranslations[] =
"<Key>Return:      Notify()\n\
       <Key>Tab:      MoveFocus(Next)\n\
     <Key>Right:      MoveFocus(Next)\n\
      <Key>Left:      MoveFocus(Prev)\n\
      <Key>Home:      MoveFocus(Home)\n\
       <Key>End:      MoveFocus(End)\n\
<Message>WM_PROTOCOLS: Dismiss()";

/* Initialization of defaults */

#define offset(field) XtOffsetOf(DialogRec, dialog.field)

static XtResource   resources[] =
{
    {XtNicon, XtCBitmap, XtRBitmap, sizeof(Pixmap),
     offset(icon), XtRImmediate, (XtPointer) None},
    {XtNiconMask, XtCBitmap, XtRBitmap, sizeof(Pixmap),
     offset(icon_mask), XtRImmediate, (XtPointer) None},
    {XtNmessage, XtCLabel, XtRString, sizeof(String),
     offset(message), XtRImmediate, (XtPointer) NULL},
    {XtNdefaultButton, XtCDefaultButton, XtRButtonType, sizeof(int),
     offset(defbut), XtRImmediate, (XtPointer) XpwDialog_Ok_Button},
    {XtNdialogType, XtCDialogType, XtRDialogType, sizeof(DialogType),
     offset(dialog_type), XtRImmediate, (XtPointer) XpwDialog_Modal},
    {XtNminimizeButtons, XtCMinimizeButtons, XtRBoolean, sizeof(Boolean),
     offset(minimize), XtRImmediate, (XtPointer) TRUE},
    {XtNautoUnmanage, XtCAutoUnmanage, XtRBoolean, sizeof(Boolean),
     offset(autoUnmanage), XtRImmediate, (XtPointer) TRUE},
    {XtNactiveBorder, XtCSpacing, XtRDimension, sizeof(Dimension),
     offset(active_border), XtRImmediate, (XtPointer) 4},
    {XtNspacing, XtCSpacing, XtRDimension, sizeof(Dimension),
     offset(spacing), XtRImmediate, (XtPointer) 2},
    {XtNpacking, XtCPackingType, XtRPackingType, sizeof(XpwPackingType),
     offset(packing), XtRImmediate, (XtPointer) XpwFill},
    {XtNjustify, XtCJustify, XtRJustify, sizeof(XtJustify),
     offset(justify), XtRImmediate, (XtPointer) XtJustifyLeft},
    {XtNokLabelString, XtCLabel, XtRString, sizeof(String),
     offset(ok_str), XtRImmediate, (XtPointer) NULL},
    {XtNcancelLabelString, XtCLabel, XtRString, sizeof(String),
     offset(cancel_str), XtRImmediate, (XtPointer) NULL},
    {XtNhelpLabelString, XtCLabel, XtRString, sizeof(String),
     offset(help_str), XtRImmediate, (XtPointer) NULL},
    {XtNokCallback, XtCCallback, XtRCallback, sizeof(XtPointer),
     offset(ok_callbacks), XtRCallback, (XtPointer) NULL},
    {XtNcancelCallback, XtCCallback, XtRCallback, sizeof(XtPointer),
     offset(cancel_callbacks), XtRCallback, (XtPointer) NULL},
    {XtNhelpCallback, XtCCallback, XtRCallback, sizeof(XtPointer),
     offset(help_callbacks), XtRCallback, (XtPointer) NULL},
    /* ThreeD resouces */
    threeDresources
};

#undef offset

#define ForAllChildren(self, childP) \
  for ( (childP) = (self)->composite.children ; \
        (childP) < (self)->composite.children + (self)->composite.num_children ; \
        (childP)++ )
#define ChildInfo(w)    ((DialogConstraints)(w)->core.constraints)

#define SuperClass (&constraintClassRec)
DialogClassRec      dialogClassRec =
{
    {	/* core fields */
	(WidgetClass) SuperClass,	/* superclass               */
	"Dialog",			/* class_name               */
	sizeof(DialogRec),		/* widget_size              */
	ClassInitialize,		/* class_initialize         */
	NULL,				/* class_part_initialize    */
	FALSE,				/* class_inited             */
	Initialize,			/* initialize               */
	NULL,				/* initialize_hook          */
	XtInheritRealize,		/* realize                  */
	actions,			/* actions                  */
	XtNumber(actions),		/* num_actions              */
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
	GeometryManager,		/* geometry_manager         */
	ChangeManaged,			/* change_managed           */
	InsertChild,			/* insert_child             */
	XtInheritDeleteChild,		/* delete_child             */
	NULL				/* extension                */
    },
    {	/* constraint class fields */
	NULL,				/* subresources             */
	ZERO,				/* subresource_count        */
	sizeof(DialogConstraintsRec),	/* constraint_size          */
	NULL,				/* initialize               */
	NULL,				/* destroy                  */
	NULL,				/* set_values               */
	NULL				/* extension                */
    },
    {	/* DialogClass Fields */
	0				/* extension                */
    }
};

WidgetClass         dialogWidgetClass = (WidgetClass) & dialogClassRec;

/************************************************************
 *
 * Semi-Public Functions.
 *
 ************************************************************/

/* 
 * Add required type converters.
 */

static void
ClassInitialize()
{
    XpwInitializeWidgetSet();
    XtAddConverter(XtRString, XtRJustify, XmuCvtStringToJustify,
		   (XtConvertArgList) NULL, (Cardinal) 0);
    XtSetTypeConverter(XtRString, XtRButtonType, cvtStringToButtonType,
		       NULL, 0, XtCacheNone, NULL);
    XtSetTypeConverter(XtRButtonType, XtRString, cvtButtonTypeToString,
		       NULL, 0, XtCacheNone, NULL);
    XtSetTypeConverter(XtRString, XtRDialogType, cvtStringToDialogType,
		       NULL, 0, XtCacheNone, NULL);
    XtSetTypeConverter(XtRDialogType, XtRString, cvtDialogTypeToString,
		       NULL, 0, XtCacheNone, NULL);
    XtSetTypeConverter(XtRString, XtRPackingType, cvtStringToPackingType,
		       NULL, 0, XtCacheNone, NULL);
    XtSetTypeConverter(XtRPackingType, XtRString, cvtPackingTypeToString,
		       NULL, 0, XtCacheNone, NULL);
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
    DialogWidget        nself = (DialogWidget) new;
    Dimension           width, height;
    int                 na;
    Widget              cw;
    Arg                 arg[4];

    nself->dialog.active_child = -1;
   /* Initialize threeD shadow */
    _XpwThreeDInit(new, &nself->dialog.threeD, nself->core.background_pixel);

   /* Build children */
    na = 0;
    if (nself->dialog.message != NULL) {
	XtSetArg(arg[na], XtNlabel, nself->dialog.message);
	na++;
    }
    if (nself->dialog.icon != (Pixmap) NULL) {
	int                 x, y;
	Window              root;
	unsigned int        w, h, bw, depth;
	char                buf[200];

       /* Make sure label is not obscured by image */
	if (!XGetGeometry(XtDisplayOfObject(new),
			  nself->dialog.icon, &root,
			  &x, &y, &w, &h, &bw, &depth)) {
	    strcpy(buf, "Xpw Dialog for object: Could not get icon");
	    strcat(buf, " Bitmap geometry information \"");
	    strcat(buf, XtName(new));
	    strcat(buf, "\"");
	    XtAppError(XtWidgetToApplicationContext(new), buf);
	}
	XtSetArg(arg[na], XtNleftMargin, w + 4);
	na++;
	XtSetArg(arg[na], XtNleftBitmap, nself->dialog.icon);
	na++;
	if (nself->dialog.icon_mask != (Pixmap) NULL) {
	    XtSetArg(arg[na], XtNleftBitmapMask, nself->dialog.icon_mask);
	    na++;
	}
    }
    nself->dialog.label = XtCreateManagedWidget("message", labelWidgetClass,
						new, arg, na);
    nself->dialog.line = XtCreateManagedWidget("line", lineWidgetClass, new,
					       NULL, 0);
    na = 0;
    if (nself->dialog.ok_str != NULL) {
	XtSetArg(arg[na], XtNlabel, nself->dialog.ok_str);
	na++;
    }
    nself->dialog.ok_button = XtCreateManagedWidget("ok", commandWidgetClass,
						    new, arg, na);
    na = 0;
    if (nself->dialog.cancel_str != NULL) {
	XtSetArg(arg[na], XtNlabel, nself->dialog.cancel_str);
	na++;
    }
    nself->dialog.cancel_button = XtCreateManagedWidget("cancel",
				       commandWidgetClass, new, arg, na);
    na = 0;
    if (nself->dialog.help_str != NULL) {
	XtSetArg(arg[na], XtNlabel, nself->dialog.help_str);
	na++;
    }
    nself->dialog.help_button = XtCreateManagedWidget("help",
				       commandWidgetClass, new, arg, na);

    XtAddCallback(XtParent(new), XtNpopupCallback, Popup, (XtPointer) new);
   /* Flag we have not had a button press */
    nself->dialog.used = 0;

   /* Compute default layout */
    ComputeSize(nself, &width, &height);
    if (nself->core.width == 0)
	nself->core.width = width;
    if (nself->core.height == 0)
	nself->core.height = height;

   /* Set active child */
    if ((cw = XpwDialogGetChild(new, nself->dialog.defbut)) != NULL)
	nself->dialog.active_child = FindWidget(nself, 0, 1, cw);

   /* Set insert position routine */
    nself->composite.insert_position = InsertPosition;

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
    DialogWidget        new_self = (DialogWidget) new;
    DialogWidget        old_self = (DialogWidget) current;
    Arg                 arg[3];
    Boolean             ret_val = False;

    if (new_self->dialog.message != old_self->dialog.message) {
	XtSetArg(arg[0], XtNlabel, new_self->dialog.message);
	XtSetValues(new_self->dialog.label, arg, 1);
    }
    if (new_self->dialog.icon != old_self->dialog.icon) {
	int                 x, y, a;
	Window              root;
	unsigned int        w, h, bw, depth;
	char                buf[200];

	if (!XGetGeometry(XtDisplayOfObject(new),
			  new_self->dialog.icon, &root,
			  &x, &y, &w, &h, &bw, &depth)) {
	    strcpy(buf, "Xpw Dialog for object: Could not get icon");
	    strcat(buf, " Bitmap geometry information \"");
	    strcat(buf, XtName(new));
	    strcat(buf, "\"");
	    XtAppError(XtWidgetToApplicationContext(new), buf);
	}
	XtSetArg(arg[0], XtNleftMargin, w + 4);
	XtSetArg(arg[1], XtNleftBitmap, new_self->dialog.icon);
	a = 2;
	if (new_self->dialog.icon_mask != old_self->dialog.icon_mask) {
	    XtSetArg(arg[a], XtNleftBitmapMask, new_self->dialog.icon_mask);
	    a++;
	}
	XtSetValues(new_self->dialog.label, arg, a);
    }
    if (new_self->dialog.ok_str != old_self->dialog.ok_str) {
	XtSetArg(arg[0], XtNlabel, new_self->dialog.ok_str);
	XtSetValues(new_self->dialog.ok_button, arg, 1);
    }
    if (new_self->dialog.cancel_str != old_self->dialog.cancel_str) {
	XtSetArg(arg[0], XtNlabel, new_self->dialog.cancel_str);
	XtSetValues(new_self->dialog.cancel_button, arg, 1);
    }
    if (new_self->dialog.help_str != old_self->dialog.help_str) {
	XtSetArg(arg[0], XtNlabel, new_self->dialog.help_str);
	XtSetValues(new_self->dialog.help_button, arg, 1);
    }
    if (_XpmThreeDSetValues(new, &old_self->dialog.threeD,
		   &new_self->dialog.threeD, new->core.background_pixel))
	ret_val = True;
    if (new_self->dialog.minimize != old_self->dialog.minimize ||
	new_self->dialog.packing != old_self->dialog.packing ||
	new_self->dialog.justify != old_self->dialog.justify) {
	Layout(new_self, new_self->core.width, new_self->core.height);
	ret_val = True;
    }
    if (new_self->dialog.defbut != old_self->dialog.defbut) {
	Widget              cw;

       /* Set active child */
	if ((cw = XpwDialogGetChild(new, new_self->dialog.defbut)) != NULL)
	    new_self->dialog.active_child = FindWidget(new_self, 0, 1, cw);
	else
	    new_self->dialog.active_child = -1;
	ret_val = True;
    }
    return ret_val;
}

/*
 * Calculate preferred size, given constraining box, caching it in the widget.
 */

static              XtGeometryResult
QueryGeometry(w, constraint, preferred)
	Widget              w;
	XtWidgetGeometry   *constraint, *preferred;
{
    DialogWidget        self = (DialogWidget) w;
    Dimension           width, height;

    ComputeSize(self, &width, &height);

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
 * Place holder for Resize events.
 */
static void
Resize(w)
	Widget              w;
{
    DialogWidget        self = (DialogWidget) w;

    Layout(self, self->core.width, self->core.height);
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
    DialogWidget        self = (DialogWidget) wid;
    DialogConstraints   child = NULL;
    Widget              cw;
    Dimension           w;
    Dimension           a = self->dialog.active_border;

    if (self->dialog.active_child == -1)
	return;
    cw = self->composite.children[self->dialog.active_child];
    if (!XtIsManaged(cw))
	return;
    child = ChildInfo(cw);
    w = (self->dialog.minimize) ? child->width : self->dialog.button_width;
   /* Draw shadows around main window */
    _XpwThreeDDrawShadow(wid, event, region, &(self->dialog.threeD),
			 child->y_loc - a, child->x_loc - a,
			 w + (2 * a), child->height + (2 * a), 1);

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
    Dimension           proposed_width, proposed_height;
    Dimension           returned_width, returned_height;
    Dimension           old_width, old_height;
    DialogWidget        self = (DialogWidget) XtParent(w);
    DialogConstraints   child = ChildInfo(w);
    XtGeometryResult	result;
    Dimension           wh, ww, bh, bw, mbw;
    Dimension           ch, cw;
    Dimension           brd = 2 * self->dialog.spacing;
    Widget             *childP;
    int                 num;

   /* Position request always denied */
    if ((request->request_mode & CWX && request->x != w->core.x) ||
	(request->request_mode & CWY && request->y != w->core.y))
	return (XtGeometryNo);

   /* Size changes must see if the new size can be accomodated */
    if ((request->request_mode & (CWWidth | CWHeight)) == 0)
	return (XtGeometryNo);

    old_width = child->width;
    old_height = child->height;
   /* Make all three fields in the request valid */
    if ((request->request_mode & CWWidth))
	child->width = request->width;
    if ((request->request_mode & CWHeight))
	child->height = request->height;

   /* Recompute width and height */
    wh = 0;
    ww = 0;
    bh = 0;
    bw = 0;
    mbw = 0;
    num = 0;
   /* Calculate size of the Children */
    ForAllChildren(self, childP) {
	Widget              w = *childP;
	DialogConstraints   child = ChildInfo(w);

	if (!XtIsManaged(w))
	    continue;

	cw = child->width;
	ch = child->height;
	if (XtIsSubclass(w, commandWidgetClass)) {
	    ch += (2 * self->dialog.active_border) + brd;
	    cw += (2 * self->dialog.active_border) + brd;
	    num++;
	    if (ch > bh)
		bh = ch;
	    bw += cw;
	    if (cw > mbw)
		mbw = cw;
	} else {
	    if (cw > ww)
		ww = cw;
	    wh += ch + brd;
	}
    }

    if (!self->dialog.minimize)
	bw = num * mbw;

    proposed_height = wh + bh + brd;
    proposed_width = ((bw > ww) ? bw : ww) + brd;

    result = XtGeometryNo;
    switch (XtMakeResizeRequest((Widget) self,
				    proposed_width, proposed_height,
				    &returned_width, &returned_height)) {
    case XtGeometryYes:
	   result = XtGeometryYes;
	   break;

    case XtGeometryNo:
	   /* protect from malicious parents who change their minds */
	    if ((returned_width <= self->core.width) &&
		(returned_height <= self->core.height)) {
 	        result = XtGeometryYes;
	    }
	    break;

    case XtGeometryAlmost:
	    if (proposed_height >= returned_height &&
		proposed_width >= returned_width) {

	       /*
	        * Take it, and assume the parent knows what it is doing.
	        *
	        * The parent must accept this since it was returned in
	        * almost.
	        *
	        */
		(void) XtMakeResizeRequest((Widget) self,
					 returned_width, returned_height,
				      &returned_width, &returned_height);
		result = XtGeometryYes;
	    }
	    break;

    case XtGeometryDone:
    default:
	    break;
    }

    if (result == XtGeometryYes) {
       /* Save info about buttons for later */
        self->dialog.button_height = bh - (2 * self->dialog.active_border + brd);
        self->dialog.button_width = mbw - (2 * self->dialog.active_border + brd);
        Layout(self, self->core.width, self->core.height);
    } else {
	child->width = old_width;
	child->height = old_height;
    }
    return result;
}

static void
ChangeManaged(w)
	Widget              w;
{
    DialogWidget        self = (DialogWidget) w;
    XtWidgetGeometry    request, reply;
    XtGeometryResult    result;
    Dimension           width, height;

   /* Compute size of managed children */
    ComputeSize(self, &width, &height);

    if (width != self->core.width || height != self->core.height) {
	request.request_mode = CWWidth | CWHeight;
	request.width = width;
	request.height = height;

	result = XtMakeGeometryRequest((Widget) self, &request, &reply);
	if (result == XtGeometryAlmost) {
	   /* Almost got it, request it since we know we can get it */
	    result = XtMakeGeometryRequest((Widget) self, &reply, &reply);
	}
    }
    Layout(self, self->core.width, self->core.height);
}

/*
 * Compute where children are to be inserted.
 */
static              Cardinal
InsertPosition(w)
	Widget              w;
{
    DialogWidget        self = (DialogWidget) XtParent(w);
    Widget              cw;	/* Insert before */

    if (XtIsSubclass(w, menubarWidgetClass))
	cw = self->dialog.label;
    else if (XtIsSubclass(w, commandWidgetClass))
	cw = self->dialog.cancel_button;
    else
	cw = self->dialog.line;
    return FindWidget(self, 0, 1, cw);
}

/*
 * Handle insertion of children.
 */
static void
InsertChild(w)
	Widget              w;
{
    DialogWidget        self = (DialogWidget) XtParent(w);
    Widget              aw = NULL;;

   /* Save old active widget */
    if (self->dialog.active_child >= 0)
	aw = self->composite.children[self->dialog.active_child];

   /* superclass insert_child routine.                                */
    (*SuperClass->composite_class.insert_child) (w);

   /* If commandWidget or textLineWidget, install a callback */
    if (XtIsSubclass(w, commandWidgetClass) ||
	XtIsSubclass(w, textLineWidgetClass)) {
       /* If the child is a commandWidgetClass, add a callback */
	XtAddCallback(w, XtNcallback, ButtonCall, (XtPointer) self);
    }
   /* Find it again */
    if (aw != NULL)
	self->dialog.active_child = FindWidget(self, 0, 1, aw);
}

/*
 * Destroy an resources we allocated.
 */
static void
Destroy(w)
	Widget              w;
{
    DialogWidget        self = (DialogWidget) w;
    Widget              shell = XtParent(w);
    Widget              parent = XtParent(shell);

   /* Remove the event handler */
    XtRemoveEventHandler(parent, StructureNotifyMask, False, Iconify, shell);
    _XpwThreeDDestroyShadow(w, &(self->dialog.threeD));
}

/************************************************************
 *
 * Private functions
 *
 ************************************************************/

/*
 * Layout children based on what they are.
 */
static void
ComputeSize(self, width, height)
	DialogWidget        self;
	Dimension          *width;
	Dimension          *height;
{
    Dimension           wh, ww, bh, bw, mbw;
    Dimension           ch, cw;
    Dimension           brd = 2 * self->dialog.spacing;
    Widget             *childP;
    int                 num;
    XtWidgetGeometry    request, reply;

    wh = 0;
    ww = 0;
    bh = 0;
    bw = 0;
    mbw = 0;
    num = 0;
   /* Calculate size of the Children */
    ForAllChildren(self, childP) {
	Widget              w = *childP;
	DialogConstraints   child = ChildInfo(w);

	if (!XtIsManaged(w))
	    continue;
	request.request_mode = CWWidth | CWHeight | XtCWQueryOnly;
	cw = request.width = 0/*w->core.width*/;
	ch = request.height = 0/*w->core.height*/;
	if (XtQueryGeometry(w, &request, &reply) != XtGeometryNo) {
	    if (reply.request_mode & CWWidth)
		cw = reply.width;
	    if (reply.request_mode & CWHeight)
		ch = reply.height;
	}
	if (cw == 0)
	    cw = w->core.width;
	if (ch == 0)
	    ch = w->core.height;
	child->width = cw;
	child->height = ch;
	if (XtIsSubclass(w, commandWidgetClass)) {
	    ch += (2 * self->dialog.active_border) + brd;
	    cw += (2 * self->dialog.active_border) + brd;
	    num++;
	    if (ch > bh)
		bh = ch;
	    bw += cw;
	    if (cw > mbw)
		mbw = cw;
	} else {
	    if (cw > ww)
		ww = cw;
	    wh += ch + brd;
	}
    }

    if (!self->dialog.minimize)
	bw = num * mbw;

    *height = wh + bh + brd;
    if (bw > ww)
	*width = bw + brd;
    else
	*width = ww + brd;
   /* Save info about buttons for later */
    self->dialog.button_height = bh - (2 * self->dialog.active_border + brd);
    self->dialog.button_width = mbw - (2 * self->dialog.active_border + brd);
}

/*
 * Layout children based on what they are.
 */
static void
Layout(self, width, height)
	DialogWidget        self;
	Dimension           width;
	Dimension           height;
{
    Dimension           brd = self->dialog.spacing;
    DialogConstraints   child;
    Position            x_loc, y_loc;
    Widget             *childP;
    int                 i;
    int   	        size, gap;

    x_loc = brd;
    y_loc = brd;
    brd += brd;
    width -= brd;

   /* Next Place all widgets, other then line and buttons */
    ForAllChildren(self, childP) {
	Widget              w = *childP;

	if (!XtIsManaged(w) || *childP == self->dialog.line 
	    || XtIsSubclass(w, commandWidgetClass))
	    continue;
	child = ChildInfo(w);
	XtConfigureWidget(w, y_loc, x_loc, width, child->height, 0);
	child->x_loc = x_loc;
	child->y_loc = y_loc;
	x_loc += child->height + brd;
    }
   /* Place line, make sure it is full width */
    if (XtIsManaged(self->dialog.line)) {
	child = ChildInfo(self->dialog.line);
	XtConfigureWidget(self->dialog.line, 0, x_loc, width + brd,
			  child->height, 0);
	child->x_loc = x_loc;
	child->y_loc = 0;
	x_loc += child->height + brd;
    }
    x_loc += self->dialog.active_border;
    y_loc += self->dialog.active_border;

   /* Compute size of command buttons */
    size = 0;
    i = 0;
    ForAllChildren(self, childP) {
	Widget              w = *childP;

	if (!XtIsManaged(w) || !XtIsSubclass(w, commandWidgetClass))
	    continue;
	child = ChildInfo(w);
	size += (self->dialog.minimize) ? child->width :
	    self->dialog.button_width;
	i++;
    }

    size += (i) * (2 * self->dialog.active_border);

    if (size < width && self->dialog.packing == XpwEven && i > 2)
	gap = (width - size) / (i - 1);
    else if (size < width && self->dialog.packing == XpwEven && i == 2)
	gap = width - size;
    else
	gap = 0;

    gap += 2 * self->dialog.active_border;

    y_loc += self->dialog.active_border;

    switch (self->dialog.justify) {
    case XtJustifyCenter:
	if (self->dialog.packing == XpwEven && i > 0) 
	    gap = ((width - size) / i);
	y_loc += (width - (size + gap * (i - 1))) / 2;
	break;
    case XtJustifyRight:
	if (self->dialog.packing != XpwEven)
	    y_loc += width - (size + gap * (i - 1));
	break;
    case XtJustifyLeft:
    default:
	break;
    }
  
   /* Now fill in button bar */
    ForAllChildren(self, childP) {
	Widget              w = *childP;
	Dimension           cw;

	if (!XtIsManaged(w) || !XtIsSubclass(w, commandWidgetClass))
	    continue;
	child = ChildInfo(w);
	cw = (self->dialog.minimize) ? child->width : self->dialog.button_width;
	XtConfigureWidget(w, y_loc, x_loc, cw, self->dialog.button_height, 0);
	child->x_loc = x_loc;
	child->y_loc = y_loc;
	y_loc += cw + gap;
    }
}

/*
 * Finded next button.
 */
static int
FindWidget(self, start, dir, w)
	DialogWidget        self;
	int                 start;
	int                 dir;
	Widget              w;
{
    int                 num = self->composite.num_children;
    int                 act = start;
    Widget              cw;

    act += dir;
    for (cw = NULL; act != start; act += dir) {
	if (act < 0)
	    act = num - 1;
	if (act >= num)
	    act = 0;
	cw = self->composite.children[act];
	if (!XtIsManaged(cw))
	    continue;
	if (w != NULL && w == cw)
	    return act;
	if (w == NULL && XtIsSubclass(cw, commandWidgetClass))
	    return act;
    }
    return -1;
}

/*
 * All the dirty work of building a dialog popup.
 */
static              Widget
MakeShell(parent, name, cname, args, count, icon, icon_mask)
	Widget              parent;
	String              name;
	String              cname;
	Arg                *args;
	int                 count;
	Pixmap              icon;
	Pixmap              icon_mask;
{
    Widget              shell, dialog;
    Display            *dpy;
    Arg                 largs[3];
    Atom                wm_delete_window;

   /* Build popup */
    XtSetArg(largs[0], XtNwindowGroup, XtWindow(parent));
    XtSetArg(largs[1], XtNtransientFor, parent);
    XtSetArg(largs[2], XtNallowShellResize, TRUE);
    shell = XtCreatePopupShell(name, dialogShellWidgetClass, parent,
			       largs, 3);
    dialog = XtCreateWidget(cname, dialogWidgetClass, shell, args, count);
   /* Overridde icon */
    if (icon != (Pixmap) NULL) {
	int	na = 0;

	XtSetArg(largs[na], XtNicon, icon);
	na++;
        if (icon_mask != (Pixmap) NULL) {
	    XtSetArg(largs[na], XtNiconMask, icon_mask);
	    na++;
        }
        XtSetValues(dialog, largs, na);
    }
   /* Make sure shell gets destroyed when dialog goes away. */
    XtAddCallback(dialog, XtNdestroyCallback, DestroyCall, (XtPointer) shell);
    XtRealizeWidget(shell);

   /* Catch dismiss button */
    dpy = XtDisplay(shell);
    wm_delete_window = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    (void) XSetWMProtocols(dpy, XtWindow(shell), &wm_delete_window, 1);

   /* Also catch iconify events */
    XtAddEventHandler(parent, StructureNotifyMask, False, Iconify, shell);
    return dialog;
}

/* Color table for builtin images */
static XColor       clist[10] =
{
    {0x0000, 0x0000, 0x0000, 0x0000, 0, 0},
    {0x0000, 0xffff, 0x0000, 0x0000, 0, 0},
    {0x0000, 0x8000, 0x0000, 0x8000, 0, 0},
    {0x0000, 0x8000, 0x8000, 0x8000, 0, 0},
    {0x0000, 0xffff, 0x0000, 0xffff, 0, 0},
    {0x0000, 0xffff, 0xffff, 0xffff, 0, 0},
    {0x0000, 0x8000, 0x8000, 0x0000, 0, 0},
    {0x0000, 0xffff, 0xffff, 0x0000, 0, 0},
    {0x0000, 0x0000, 0x0000, 0x8000, 0, 0},
    {0x0000, 0xc000, 0xc000, 0xc000, 0, 0}
};

/*
 * Make a Pixmap from asascii representation.
 */
static              Pixmap
MakePixmap(parent, data, data_bit, width, height, mask)
	Widget              parent;
	char              **data;
	char               *data_bit;
	int                 width;
	int                 height;
	Pixmap             *mask;
{
    Display            *dpy = XtDisplay(parent);
    int                 scn = DefaultScreen(dpy);
    Visual             *visual = XDefaultVisual(dpy, scn);
    Colormap            cmap = DefaultColormap(dpy, scn);
    unsigned int        depth = XDefaultDepth(dpy, scn);
    XImage             *img;
    Pixmap              result;
    char               *mask_data;
    int                 i, j;
    int			bitmap_pad;
    GC                  gc;
    XGCValues           values;

    if (depth == 1) {
	if (mask != NULL)
	    *mask = (Pixmap) NULL;
	return XCreateBitmapFromData(dpy, DefaultRootWindow(dpy),
			 (char *) data_bit, width, height);
    }
    if (depth > 16)
        bitmap_pad = 32;
    else if (depth > 8)
        bitmap_pad = 16;
    else
        bitmap_pad = 8;

    for (i = 0; i < (sizeof(clist) / sizeof(XColor)); i++)
	XAllocColor(dpy, cmap, &clist[i]);
   /* Make mask bitmap */
    mask_data = XtMalloc(i = ((width / 8) * height));
    for (; i >= 0; mask_data[--i] = 0) ;
   /* Make image */
    img = XCreateImage(dpy, visual, depth, ZPixmap, 0, 0,
		       width, height, bitmap_pad, 0);
    img->data = (char *)XtMalloc(img->bytes_per_line * height);

   /* Make image and mask */
    for (i = 0; i < height; i++) {
	char               *line = data[i];

	for (j = 0; j < width; j++) {
	    int                 pix = line[j] - '0';

	    if (pix != 0) {
		mask_data[((i * width) + j) / 8] |= 1 << (j & 0x7);
		pix--;
	    }
	    XPutPixel(img, j, i, clist[pix].pixel);
	}
    }
   /* Build mask if one was requested */
    if (mask != NULL)
	*mask = XCreateBitmapFromData(dpy, DefaultRootWindow(dpy),
			 (char *) mask_data, width, height);
   /* Make the Pixmap */
    result = XCreatePixmap(dpy, DefaultRootWindow(dpy), width, height, depth);

    values.foreground = 1;
    values.background = 0;
    gc = XCreateGC(dpy, result, GCForeground | GCBackground, &values);

    XPutImage(dpy, result, gc, img, 0, 0, 0, 0, width, height);

   /* Free stuff we don't need */
    XFreeGC(dpy, gc);
    XDestroyImage(img);
    XtFree(mask_data);
    return result;
}

/************************************************************
 *
 * ACTION procedures
 *
 ************************************************************/

/*
 * Popup event.
 */
static void
Popup(w, client_data, call_data)
	Widget              w;
	XtPointer           client_data;
	XtPointer           call_data;
{
    Widget              self = (Widget) client_data;
    Widget              parent = XtParent(w);
    Arg                 largs[4];
    Position            x, y;
    Dimension           ph, pw, sh, sw, ww, wh;

   /* Now center in child and make sure fully visable */
    ChangeManaged((Widget)self);

   /* Pickup info on parent */
    XtSetArg(largs[0], XtNwidth, &pw);
    XtSetArg(largs[1], XtNheight, &ph);
    XtGetValues(parent, largs, 2);

   /* Next translate the cordinates from widget to screen */
    XtTranslateCoords(parent, 0, 0, &x, &y);

   /* Next get shell size */
    sw = self->core.width;
    sh = self->core.height;
    x += (pw - sw) / 2;
    if (x < 0)
	x = 0;
    y += (ph - sh) / 2;
    if (y < 0)
	y = 0;
    ww = WidthOfScreen(XtScreen(parent));
    if ((x + sw) > ww) {
	x = ww - sw;
    }
    wh = HeightOfScreen(XtScreen(parent));
    if ((y + sh) > wh) {
	y = wh - sh;
    }
    XtSetArg(largs[0], XtNx, x);
    XtSetArg(largs[1], XtNy, y);
    XtSetArg(largs[2], XtNwidth, sw);
    XtSetArg(largs[3], XtNheight, sh);
    XtSetValues(w, largs, 4);
}

/*
 * Button event.
 */
static void
ButtonCall(w, client_data, call_data)
	Widget              w;
	XtPointer           client_data;
	XtPointer           call_data;
{
    DialogWidget        self = (DialogWidget) client_data;
    int                 what = 0;

    if (w == self->dialog.ok_button) {
	what = XpwDialog_Ok_Button;
	XtCallCallbackList((Widget) self, self->dialog.ok_callbacks,
			   (XtPointer) what);
    } else if (w == self->dialog.cancel_button) {
	what = XpwDialog_Cancel_Button;
	XtCallCallbackList((Widget) self, self->dialog.cancel_callbacks,
			   (XtPointer) what);
    } else if (w == self->dialog.help_button) {
	what = XpwDialog_Help_Button;
	XtCallCallbackList((Widget) self, self->dialog.help_callbacks,
			   (XtPointer) what);
    }
    self->dialog.used = what;

   /* Pop down now */
    if (what != XpwDialog_Help_Button && self->dialog.autoUnmanage)
	XtUnmanageChild((Widget) self);
}

/*
 * Destroy.
 */
static void
DestroyCall(w, client_data, call_data)
	Widget              w;
	XtPointer           client_data;
	XtPointer           call_data;
{
    XtDestroyWidget((Widget) client_data);
}

/* Dismiss() - ACTION
 *
 * Calls cancel or ok callback when window is dismissed by window manager.
 */

/* ARGSUSED */
static void
Dismiss(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    DialogWidget        self = (DialogWidget) w;
    Widget              cw;

    if (event->type != ClientMessage)
	return;

   /* Try cancel first */
    cw = self->dialog.cancel_button;
    if (XtIsManaged(cw)) {
	XtCallCallbacks(cw, XtNcallback, (XtPointer) NULL);
	return;
    }
   /* Cancel has been turned off. try Ok. */
    cw = self->dialog.ok_button;
    if (XtIsManaged(cw)) {
	XtCallCallbacks(cw, XtNcallback, (XtPointer) NULL);
	return;
    }
   /* Nothing. Punt and just unmanage window */
    XtUnmanageChild(w);
}

/*
 * Move to next widget.
 */

/* ARGSUSED */
static void
MoveFocus(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    DialogWidget        self = (DialogWidget) w;
    int                 act = self->dialog.active_child;
    DialogConstraints   child = NULL;
    Widget              cw;
    Dimension           a = self->dialog.active_border;

    if (*num_params != (Cardinal) 1) {
	XtWarning("Invalid number of parameters to MoveFocus action");
	return;
    }
    if (act != -1)
	cw = self->composite.children[self->dialog.active_child];
    else {
	cw = self->dialog.ok_button;
	act = FindWidget(self, 0, 1, cw);
    }

   /* Remove old shadow */
    if (XtIsManaged(cw)) {
	Dimension           width;

	child = ChildInfo(cw);
	width = (self->dialog.minimize) ? child->width :
	    self->dialog.button_width;
       /* Draw shadows around main window */
	_XpwThreeDEraseShadow(w, NULL, NULL, &(self->dialog.threeD),
			      child->y_loc - a, child->x_loc - a,
			      width + (2 * a), child->height + (2 * a));
    }
    switch (params[0][0]) {
    case 'n':
    case 'N':
	act = FindWidget(self, act, 1, NULL);
	break;
    case 'p':
    case 'P':
	act = FindWidget(self, act, -1, NULL);
	break;;
    case 'h':
    case 'H':
	act = FindWidget(self, 0, 1, self->dialog.ok_button);
	if (act < 0)
	    act = FindWidget(self, 0, 1, self->dialog.cancel_button);
	if (act < 0)
	    act = FindWidget(self, 0, 1, self->dialog.help_button);
	if (act < 0)
	    act = FindWidget(self, 0, 1, NULL);
	break;
    case 'e':
    case 'E':
	act = FindWidget(self, 0, 1, self->dialog.help_button);
	if (act < 0)
	    act = FindWidget(self, 0, 1, self->dialog.cancel_button);
	if (act < 0)
	    act = FindWidget(self, 0, 1, self->dialog.ok_button);
	if (act < 0)
	    act = FindWidget(self, 0, -1, NULL);
	break;
    default:
	return;
    }
    if (cw != NULL)
	self->dialog.active_child = act;
    Redisplay(w, NULL, NULL);
}

/* Notify() - ACTION
 *
 * Notifies the user that a button has been pressed, and
 * calls the callback.
 */

/* ARGSUSED */
static void
Notify(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    DialogWidget        self = (DialogWidget) w;
    Widget              cw;

   /* See if we have a active button */
    if (self->dialog.active_child == -1)
	return;
    cw = self->composite.children[self->dialog.active_child];
    if (XtIsManaged(cw))
	XtCallCallbacks(cw, XtNcallback, (XtPointer) NULL);
}

/*
 * Handle iconify events.
 */
static void
Iconify(w, client_data, event, dispatch)
	Widget              w;
	XtPointer           client_data;
	XEvent             *event;
	Boolean            *dispatch;
{
    Widget              shell = (Widget) client_data;
    Arg                 arg[1];

    if (event->type == UnmapNotify) {
	XtSetArg(arg[0], XtNiconic, True);
	XtSetValues(shell, arg, 1);
    } else if (event->type == MapNotify) {
	XtSetArg(arg[0], XtNiconic, False);
	XtSetValues(shell, arg, 1);
    }
}

/************************************************************
 *
 * Type converters.
 *
 ************************************************************/

Boolean
cvtStringToButtonType(dpy, args, num_args, from, to, converter_data)
	Display            *dpy;
	XrmValuePtr         args;
	Cardinal           *num_args;
	XrmValuePtr         from;
	XrmValuePtr         to;
	XtPointer          *converter_data;
{
    String              s = (String) from->addr;
    static int          result;

    if (*num_args != 0) {
	XtAppErrorMsg(XtDisplayToApplicationContext(dpy),
		      "cvtStringToButtonType", "wrongParameters",
		      "XtToolkitError",
		  "String to packing type conversion needs no arguments",
		      (String *) NULL, (Cardinal *) NULL);
	return FALSE;
    }
    if (XmuCompareISOLatin1(s, "ok") == 0)
	result = XpwDialog_Ok_Button;
    else if (XmuCompareISOLatin1(s, "cancel") == 0)
	result = XpwDialog_Cancel_Button;
    else if (XmuCompareISOLatin1(s, "help") == 0)
	result = XpwDialog_Help_Button;
    else if (XmuCompareISOLatin1(s, "none") == 0)
	result = 0;
    else {
	XtDisplayStringConversionWarning(dpy, s, XtRButtonType);
	return FALSE;
    }

    if (to->addr != NULL) {
	if (to->size < sizeof(int)) {
	    to->size = sizeof(int);

	    return FALSE;
	}
	*(int *) (to->addr) = result;
    } else
	(int *) to->addr = &result;
    to->size = sizeof(int);

    return TRUE;
}

Boolean
cvtButtonTypeToString(dpy, args, num_args, from, to, converter_data)
	Display            *dpy;
	XrmValuePtr         args;
	Cardinal           *num_args;
	XrmValuePtr         from;
	XrmValuePtr         to;
	XtPointer          *converter_data;
{
    static String       result;

    if (*num_args != 0) {
	XtAppErrorMsg(XtDisplayToApplicationContext(dpy),
		      "cvtButtonTypeToString", "wrongParameters",
		      "XtToolkitError",
		  "packing type to String conversion needs no arguments",
		      (String *) NULL, (Cardinal *) NULL);
	return FALSE;
    }
    switch ((int) from->addr) {
    case XpwDialog_Ok_Button:
	result = "ok";
    case XpwDialog_Help_Button:
	result = "help";
    case XpwDialog_Cancel_Button:
	result = "cancel";
    case 0:
	result = 0;
    default:
	XtError("Illegal ButtonType");
	return FALSE;
    }
    if (to->addr != NULL) {
	if (to->size < sizeof(String)) {
	    to->size = sizeof(String);
	    return FALSE;
	}
	*(String *) (to->addr) = result;
    } else
	(String *) to->addr = &result;
    to->size = sizeof(String);
    return TRUE;
}

Boolean
cvtStringToDialogType(dpy, args, num_args, from, to, converter_data)
	Display            *dpy;
	XrmValuePtr         args;
	Cardinal           *num_args;
	XrmValuePtr         from;
	XrmValuePtr         to;
	XtPointer          *converter_data;
{
    String              s = (String) from->addr;
    static int          result;

    if (*num_args != 0) {
	XtAppErrorMsg(XtDisplayToApplicationContext(dpy),
		      "cvtStringToDialogType", "wrongParameters",
		      "XtToolkitError",
		  "String to packing type conversion needs no arguments",
		      (String *) NULL, (Cardinal *) NULL);
	return FALSE;
    }

    if (XmuCompareISOLatin1(s, "modeless") == 0)
	result = XpwDialog_Modeless;
    else if (XmuCompareISOLatin1(s, "modal") == 0)
	result = XpwDialog_Modal;
    else if (XmuCompareISOLatin1(s, "dialog") == 0)
	result = XpwDialog_Dialog;
    else {
	XtDisplayStringConversionWarning(dpy, s, XtRDialogType);
	return FALSE;
    }

    if (to->addr != NULL) {
	if (to->size < sizeof(int)) {
	    to->size = sizeof(int);

	    return FALSE;
	}
	*(int *) (to->addr) = result;
    } else
	(int *) to->addr = &result;
    to->size = sizeof(int);

    return TRUE;
}

Boolean
cvtDialogTypeToString(dpy, args, num_args, from, to, converter_data)
	Display            *dpy;
	XrmValuePtr         args;
	Cardinal           *num_args;
	XrmValuePtr         from;
	XrmValuePtr         to;
	XtPointer          *converter_data;
{
    static String       result;

    if (*num_args != 0) {
	XtAppErrorMsg(XtDisplayToApplicationContext(dpy),
		      "cvtButtonTypeToString", "wrongParameters",
		      "XtToolkitError",
		  "packing type to String conversion needs no arguments",
		      (String *) NULL, (Cardinal *) NULL);
	return FALSE;
    }
    switch ((int) from->addr) {
    case XpwDialog_Modeless:
	result = "modeless";
    case XpwDialog_Modal:
	result = "modal";
    case XpwDialog_Dialog:
	result = "dialog";
    default:
	XtError("Illegal DialogType");
	return FALSE;
    }
    if (to->addr != NULL) {
	if (to->size < sizeof(String)) {
	    to->size = sizeof(String);
	    return FALSE;
	}
	*(String *) (to->addr) = result;
    } else
	(String *) to->addr = &result;
    to->size = sizeof(String);
    return TRUE;
}
/************************************************************
 *
 * Built in bitmaps.
 *
 ************************************************************/

#define icon_width 32
#define icon_height 32

static unsigned char notice_bits[] =
{
  0x00, 0xe0, 0x01, 0x00, 0x00, 0xb0, 0x02, 0x00, 0x00, 0x58, 0x05, 0x00,
  0x00, 0xa8, 0x06, 0x00, 0x00, 0x58, 0x05, 0x00, 0x00, 0xa8, 0x06, 0x00,
  0x00, 0x58, 0x05, 0x00, 0x00, 0xb8, 0x06, 0x00, 0x00, 0x50, 0x03, 0x00,
  0x00, 0xb0, 0x02, 0x00, 0x00, 0x50, 0x03, 0x00, 0x00, 0xb0, 0x02, 0x00,
  0x00, 0x50, 0x03, 0x00, 0x00, 0xb0, 0x02, 0x00, 0x00, 0x50, 0x03, 0x00,
  0x00, 0xb0, 0x03, 0x00, 0x00, 0x60, 0x01, 0x00, 0x00, 0xa0, 0x01, 0x00,
  0x00, 0x60, 0x01, 0x00, 0x00, 0xa0, 0x01, 0x00, 0x00, 0xe0, 0x01, 0x00,
  0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xc0, 0x01, 0x00, 0x00, 0xa0, 0x02, 0x00, 0x00, 0x50, 0x05, 0x00,
  0x00, 0xb0, 0x06, 0x00, 0x00, 0x50, 0x05, 0x00, 0x00, 0xa0, 0x02, 0x00,
  0x00, 0xc0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};

static char *notice_data[] =
{
    "00000000000000011100000000000000",
    "00000000000000155310000000000000",
    "00000000000001554531000000000000",
    "00000000000001545531000000000000",
    "00000000000001565531000000000000",
    "00000000000001565531000000000000",
    "00000000000001565531000000000000",
    "00000000000001565531000000000000",
    "00000000000001565531000000000000",
    "00000000000001565531000000000000",
    "00000000000001565531000000000000",
    "00000000000001565531000000000000",
    "00000000000001565531000000000000",
    "00000000000001565531000000000000",
    "00000000000001565531000000000000",
    "00000000000001565531000000000000",
    "00000000000001565531000000000000",
    "00000000000001545531000000000000",
    "00000000000001555531000000000000",
    "00000000000001555531000000000000",
    "00000000000001555531000000000000",
    "00000000000001355531000000000000",
    "00000000000000133310000000000000",
    "00000000000000011100000000000000",
    "00000000000000000000000000000000",
    "00000000000000011100000000000000",
    "00000000000000155310000000000000",
    "00000000000001564531000000000000",
    "00000000000001545531000000000000",
    "00000000000001355531000000000000",
    "00000000000000133310000000000000",
    "00000000000000011100000000000000"
};

static unsigned char error_bits[] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x03, 0x00, 0x00, 0xf8, 0x0f, 0x00,
  0x00, 0x5c, 0x35, 0x00, 0x00, 0xaf, 0xea, 0x00, 0x80, 0xd5, 0x57, 0x01,
  0xc0, 0x3a, 0xb8, 0x02, 0x60, 0x05, 0x40, 0x05, 0xb0, 0x02, 0x80, 0x0a,
  0x70, 0x01, 0x00, 0x0d, 0xb8, 0x00, 0x00, 0x1a, 0xd8, 0x00, 0x00, 0x16,
  0xbc, 0x00, 0x00, 0x3a, 0xdc, 0xff, 0xff, 0x37, 0xac, 0xff, 0xff, 0x2b,
  0x5c, 0x55, 0x55, 0x35, 0xac, 0xaa, 0xaa, 0x2a, 0xd8, 0xff, 0xff, 0x37,
  0xb8, 0x00, 0x00, 0x3b, 0xd0, 0x00, 0x00, 0x17, 0xb0, 0x00, 0x00, 0x1b,
  0x60, 0x01, 0x80, 0x0d, 0xa0, 0x02, 0xc0, 0x0a, 0x40, 0x05, 0x78, 0x05,
  0x80, 0x3a, 0xbf, 0x02, 0x00, 0xd5, 0x57, 0x01, 0x00, 0xae, 0xea, 0x00,
  0x00, 0x58, 0x35, 0x00, 0x00, 0xe0, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* XPM */
static char *error_data[] = {
"00000000000000000000000000000000",
"00000000000000000000000000000000",
"00000000000000000000000000000000",
"00000000000002222220000000000000",
"00000000000222222222200000000000",
"00000000022222222222222000000000",
"00000000222221111112222200000000",
"00000002222110000001222220000000",
"00000022222000000000022222000000",
"00000222222200000000002222100000",
"00000222222220000000000222200000",
"00002221022222000000000022210000",
"00002221002222200000000022210000",
"00022210000222220000000002221000",
"00022210000022222000000002221000",
"00022210000002222200000002221000",
"00022210000000222220000002221000",
"00022210000000022222000002221000",
"00022210000000002222200002221000",
"00002221000000000222220022210000",
"00002222000000000022222022110000",
"00000222200000000002222222100000",
"00000222220000000000222221100000",
"00000022222000000000022211000000",
"00000002222220000002222110000000",
"00000000222222222222221100000000",
"00000000012222222222211000000000",
"00000000000112222221100000000000",
"00000000000001111110000000000000",
"00000000000000000000000000000000",
"00000000000000000000000000000000",
"00000000000000000000000000000000",
};

static unsigned char info_bits[] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x50, 0x01, 0x00,
  0x00, 0xa8, 0x02, 0x00, 0x00, 0x58, 0x03, 0x00, 0x00, 0xa8, 0x02, 0x00,
  0x00, 0x50, 0x01, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xf8, 0x03, 0x00, 0x00, 0xac, 0x06, 0x00, 0x00, 0x52, 0x05, 0x00,
  0x00, 0xbe, 0x06, 0x00, 0x00, 0x50, 0x05, 0x00, 0x00, 0xb0, 0x06, 0x00,
  0x00, 0x50, 0x05, 0x00, 0x00, 0xb0, 0x06, 0x00, 0x00, 0x50, 0x05, 0x00,
  0x00, 0xb0, 0x06, 0x00, 0x00, 0x50, 0x05, 0x00, 0x00, 0xb0, 0x06, 0x00,
  0x00, 0x50, 0x05, 0x00, 0x00, 0xb0, 0x06, 0x00, 0x00, 0x50, 0x05, 0x00,
  0x00, 0xb0, 0x06, 0x00, 0x00, 0x58, 0x0d, 0x00, 0x00, 0xaf, 0x7a, 0x00,
  0x00, 0x55, 0x55, 0x00, 0x00, 0xab, 0x6a, 0x00, 0x00, 0x55, 0x55, 0x00,
  0x00, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00};

static char *info_data[] = {
"00000000000000000000000000000000",
"00000000000000000000000000000000",
"00000000000000000000000000000000",
"00000000000000000000000000000000",
"00000000000009999999000000000000",
"00000000000999999999990000000000",
"00000000099999466A49999900000000",
"00000000999999666669999990000000",
"00000009999994666664999999000000",
"00000099999994666669999999900000",
"00000099999999466649999999900000",
"00000999999999944499999999990000",
"00000999999944444449999999990000",
"0000999999994A666669999999999000",
"00009999999999466669999999999000",
"00009999999999466669999999999000",
"00009999999999466669999999999000",
"00009999999999466669999999999000",
"00009999999999466669999999999000",
"00009999999999466669999999999000",
"00000999999999466669999999990000",
"00000999999999466669999999990000",
"00000099999999466669999999900000",
"00000099999999466669999999900000",
"0000000999994466666A449999000000",
"0000000099994A44444A499990000000",
"00000000099999999999999900000000",
"00000000000999999999990000000000",
"00000000000009999999000000000000",
"00000000000000000000000000000000",
"00000000000000000000000000000000",
"00000000000000000000000000000000",
};

static unsigned char quest_bits[] =
{
  0x00, 0xfc, 0x7f, 0x00, 0x00, 0xaa, 0xaa, 0x00, 0x00, 0xfd, 0x5f, 0x01,
  0x00, 0x0f, 0xa0, 0x02, 0x00, 0x05, 0x40, 0x03, 0x00, 0x07, 0x80, 0x02,
  0x00, 0x05, 0x80, 0x03, 0x00, 0x02, 0x80, 0x02, 0x00, 0x00, 0x80, 0x03,
  0x00, 0x00, 0x80, 0x02, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x80, 0x02,
  0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0xe0, 0x02, 0x00, 0x00, 0x50, 0x01,
  0x00, 0x00, 0xac, 0x00, 0x00, 0x80, 0x77, 0x00, 0x00, 0x40, 0x0d, 0x00,
  0x00, 0xa0, 0x07, 0x00, 0x00, 0x60, 0x01, 0x00, 0x00, 0xa0, 0x01, 0x00,
  0x00, 0x60, 0x01, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x01, 0x00, 0x00, 0xa0, 0x02, 0x00,
  0x00, 0x50, 0x05, 0x00, 0x00, 0xb0, 0x06, 0x00, 0x00, 0x50, 0x05, 0x00,
  0x00, 0xa0, 0x02, 0x00, 0x00, 0xc0, 0x01, 0x00};

static char *quest_data[] = {
"00000000000011111110000000000000",
"00000000001188888801100000000000",
"00000000118866666888011000000000",
"00000001886668888888807100000000",
"00000018866888888888880710000000",
"00000018668807777708888010000000",
"00000186688071111170888871000000",
"00000186680710000017888871000000",
"00000188887100000001888871000000",
"00000108807100000001888871000000",
"00000017771000000001888871000000",
"00000001110000000001888871000000",
"00000000000000000118888071000000",
"00000000000000011888888710000000",
"00000000000000188688887710000000",
"00000000000001886888077100000000",
"00000000000018668807711000000000",
"00000000000018688071100000000000",
"00000000000186680710000000000000",
"00000000000186687100000000000000",
"00000000000188887100000000000000",
"00000000000108807100000000000000",
"00000000000017771000000000000000",
"00000000000001110000000000000000",
"00000000000000000000000000000000",
"00000000000001110000000000000000",
"00000000000018801000000000000000",
"00000000000186687100000000000000",
"00000000000186687100000000000000",
"00000000000108807100000000000000",
"00000000000017771000000000000000",
"00000000000001110000000000000000",
};

static unsigned char work_bits[] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xc0, 0xff, 0xff, 0x01, 0x20, 0x00, 0x00, 0x02, 0x20, 0x00, 0x00, 0x02,
  0x40, 0x00, 0x00, 0x01, 0x80, 0x80, 0x80, 0x00, 0x00, 0x41, 0x41, 0x00,
  0x00, 0xa2, 0x22, 0x00, 0x00, 0x54, 0x15, 0x00, 0x00, 0xa8, 0x0a, 0x00,
  0x00, 0x50, 0x05, 0x00, 0x00, 0xa0, 0x02, 0x00, 0x00, 0x40, 0x01, 0x00,
  0x00, 0xc0, 0x01, 0x00, 0x00, 0x40, 0x01, 0x00, 0x00, 0x20, 0x02, 0x00,
  0x00, 0x10, 0x04, 0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x84, 0x10, 0x00,
  0x00, 0x42, 0x21, 0x00, 0x00, 0xa1, 0x42, 0x00, 0x80, 0x50, 0x85, 0x00,
  0x40, 0xa8, 0x0a, 0x01, 0x20, 0x54, 0x15, 0x02, 0x20, 0xaa, 0x2a, 0x02,
  0xc0, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static char *work_data[] = {
"00000001111111111111111111000000",
"00000010006660000000444440100000",
"00000001106011111111114011000000",
"00000000010000000000000100000000",
"00000000010066000004400100000000",
"00000000010066000004400100000000",
"00000000010066000004400100000000",
"00000000010066000004400100000000",
"00000000010066000004400100000000",
"00000000010066000004400100000000",
"00000000010066000004400100000000",
"00000000001006880070001000000000",
"00000000000110677700110000000000",
"00000000000001067001000000000000",
"00000000000000100010000000000000",
"00000000000000010100000000000000",
"00000000000000010100000000000000",
"00000000000000100010000000000000",
"00000000000001000001000000000000",
"00000000000110067000110000000000",
"00000000001000687700001000000000",
"00000000010078877744700100000000",
"00000000010778877744770100000000",
"00000000010778877744770100000000",
"00000000010778877744770100000000",
"00000000010778877744770100000000",
"00000000010778877744770100000000",
"00000000010778877744770100000000",
"00000000010078877744700100000000",
"00000001106044111111114011000000",
"00000010006660000044444400100000",
"00000001111111111111111111000000"
};

/************************************************************
 *
 * Public interface functions
 *
 ************************************************************/

/*
 * Make a new notice dialog.
 */
Widget
XpwDialogCreateNotice(parent, name, args, count)
	Widget              parent;
	String              name;
	Arg                *args;
	int                 count;
{
    static Pixmap       notice = (Pixmap) NULL;
    static Pixmap       notice_mask = (Pixmap) NULL;

    if (notice == (Pixmap) NULL)
       notice = MakePixmap(parent, notice_data, notice_bits,
			icon_width, icon_height, &notice_mask);

    return MakeShell(parent, name, "notice", args, count, notice, notice_mask);
}

/*
 * Make a new error dialog.
 */
Widget
XpwDialogCreateError(parent, name, args, count)
	Widget              parent;
	String              name;
	Arg                *args;
	int                 count;
{
    static Pixmap       error = (Pixmap) NULL;
    static Pixmap       error_mask = (Pixmap) NULL;

    if (error == (Pixmap) NULL)
       error = MakePixmap(parent, error_data, error_bits,
			icon_width, icon_height, &error_mask);

    return MakeShell(parent, name, "error", args, count, error, error_mask);
}

/*
 * Make a new info dialog.
 */
Widget
XpwDialogCreateInfo(parent, name, args, count)
	Widget              parent;
	String              name;
	Arg                *args;
	int                 count;
{
    static Pixmap       info = (Pixmap) NULL;
    static Pixmap       info_mask = (Pixmap) NULL;

    if (info == (Pixmap) NULL)
        info = MakePixmap(parent, info_data, info_bits,
			icon_width, icon_height, &info_mask);

    return MakeShell(parent, name, "info", args, count, info, info_mask);
}

/*
 * Make a new question dialog.
 */
Widget
XpwDialogCreateQuestion(parent, name, args, count)
	Widget              parent;
	String              name;
	Arg                *args;
	int                 count;
{
    static Pixmap       quest = (Pixmap) NULL;
    static Pixmap       quest_mask = (Pixmap) NULL;

    if (quest == (Pixmap) NULL)
        quest = MakePixmap(parent, quest_data, quest_bits,
			icon_width, icon_height, &quest_mask);

    return MakeShell(parent, name, "question", args, count, quest, quest_mask);
}

/*
 * Make a new work in progresss dialog.
 */
Widget
XpwDialogCreateWork(parent, name, args, count)
	Widget              parent;
	String              name;
	Arg                *args;
	int                 count;
{
    static Pixmap       work = (Pixmap) NULL;
    static Pixmap       work_mask = (Pixmap) NULL;

    if (work == (Pixmap) NULL)
        work = MakePixmap(parent, work_data, work_bits,
			icon_width, icon_height, &work_mask);

    return MakeShell(parent, name, "work", args, count, work, work_mask);
}

/*
 * Make a new dialog.
 */
Widget
XpwDialogCreateDialog(parent, name, args, count)
	Widget              parent;
	String              name;
	Arg                *args;
	int                 count;
{
    return MakeShell(parent, name, "dialog", args, count, (Pixmap) NULL,
				 (Pixmap) NULL);
}

/*
 * Show a dialog.
 */
void
XpwDialogShow(dialog)
	Widget              dialog;
{
    XtManageChild(dialog);	/* Put it up */
}

/*
 * Popup a dialog box and wait for a button to be pressed.
 */
int
XpwDialogDisplay(dialog)
	Widget              dialog;
{
    DialogWidget        self = (DialogWidget) dialog;
    XtAppContext        context = XtWidgetToApplicationContext(dialog);

    self->dialog.used = 0;
    XtManageChild(dialog);	/* Put it up */
    while (self->dialog.used == 0 || self->dialog.used == XpwDialog_Help_Button)
	XtAppProcessEvent(context, XtIMAll);	/* Process events */
    return self->dialog.used;
}

/*
 * Return widget based on parameter.
 */
Widget
XpwDialogGetChild(dialog, what)
	Widget              dialog;
	int                 what;
{
    DialogWidget        self = (DialogWidget) dialog;

    switch (what) {
    case XpwDialog_Ok_Button:
	return self->dialog.ok_button;
    case XpwDialog_Cancel_Button:
	return self->dialog.cancel_button;
    case XpwDialog_Help_Button:
	return self->dialog.help_button;
    case XpwDialog_Label:
	return self->dialog.label;
    }
    return NULL;
}
