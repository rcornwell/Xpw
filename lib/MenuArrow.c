/*
 * Menu arrow widget.
 *
 * This is the menu arrow widget. It is a subclass of ArrowWidget.
 * It's purpose is for when you want to put a popup menu with a simple shape.
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
 * $Log: MenuArrow.c,v $
 * Revision 1.2  1997/11/01 06:39:04  rich
 * Changed justify to menuJustify to avoid conflict with label.
 *
 * Revision 1.1  1997/10/15 05:42:22  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: MenuArrow.c,v 1.2 1997/11/01 06:39:04 rich Beta rich $";

#endif

#include <X11/Xlib.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xosdefs.h>
#include <X11/Xos.h>
#include <X11/Xmu/Drawing.h>
#include "XpwInit.h"
#include "MenuArrowP.h"
#include "Cardinals.h"

#include <stdio.h>

/* Initialization of defaults */

#define offset(field) XtOffsetOf(MenuArrowRec, menuarrow.field)

static XtResource   resources[] =
{
    {XtNmenuName, XtCMenuName, XtRString, sizeof(String),
     offset(menu_name), XtRString, (XtPointer) NULL},
    {XtNmenuJustify, XtCJustify, XtRJustify, sizeof(XtJustify),
     offset(menu_justify), XtRImmediate, (XtPointer) XtJustifyRight}, 
    {XtNalignToParent, XtCAlignToParent, XtRBoolean, sizeof(Boolean),
     offset(align_parent), XtRImmediate, (XtPointer) False},
};

#undef offset

/* Semi Public function definitions. */
static void         ClassInitialize(void);
static void         Initialize(Widget /*request */ , Widget /*new */ ,
				 ArgList /*args */ , Cardinal * /*num_args */ );
static Boolean      SetValues(Widget /*current */ , Widget /*request */ ,
				 Widget /*new */ , ArgList /*args */ ,
				 Cardinal * /*num_args */ );
static void         Destroy(Widget /*w */ );

/* Actions  */
static void         Popup(Widget /*w */, XEvent * /*event */,
                                 String * /*params */, Cardinal * /*num_params */);
static void         Popdown(Widget /*w */, XEvent * /*event */,
                                 String * /*params */, Cardinal * /*num_params */);

static XtActionsRec actionsList[] =
{
    {"PopupMenu", Popup},
    {"PopdownMenu", Popdown},
};

static char         defaultTranslations[] =
       "      <BtnUp>:	notify() unset()\n\
           <Btn1Down>:	set() PopupMenu()\n\
 	<LeaveWindow>:  reset()\n";

#define superclass (&arrowClassRec)
MenuArrowClassRec  menuarrowClassRec =
{
    {	/* core fields */
	(WidgetClass) superclass,	/* superclass               */
	"MenuArrow",			/* class_name               */
	sizeof(MenuArrowRec),		/* widget_size              */
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
	XtInheritExpose,		/* expose                   */
	SetValues,			/* set_values               */
	NULL,				/* set_values_hook          */
	XtInheritSetValuesAlmost,	/* set_values_almost        */
	NULL,				/* get_values_hook          */
	NULL,				/* accept_focus             */
	XtVersion,			/* version                  */
	NULL,				/* callback_private         */
	defaultTranslations,		/* tm_table                 */
	XtInheritQueryGeometry,		/* query_geometry           */
	XtInheritDisplayAccelerator,	/* display_accelerator      */
	NULL				/* extension                */
    },
    { /* Arrow Class Fields */
	0,
    },
    { /* Menu ArrowClass Fields */
	0,				/* extension 		   */
    }
};

WidgetClass         menuarrowWidgetClass = (WidgetClass) & menuarrowClassRec;

/************************************************************
 *
 * Semi-Public Functions.
 *
 ************************************************************/

/*
 * ClassInitialize: Register grab for mouse.
 */

static void
ClassInitialize()
{
    XpwInitializeWidgetSet();
    XtRegisterGrabAction(Popup, True,
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
    MenuArrow           self = (MenuArrow) new;

    if (self->menuarrow.menu_name != NULL) {
	self->menuarrow.menu_name = XtNewString(self->menuarrow.menu_name);
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
    MenuArrow           self = (MenuArrow) new;
    MenuArrow           old_self = (MenuArrow) current;

    if (self->menuarrow.menu_name != old_self->menuarrow.menu_name) {
	XtFree(old_self->menuarrow.menu_name);
	self->menuarrow.menu_name = XtNewString(self->menuarrow.menu_name);
    }
    return (FALSE);
}

/*
 * Destroy an resources we allocated.
 */
static void
Destroy(w)
	Widget              w;
{
    MenuArrow           self = (MenuArrow) w;

    if (self->menuarrow.menu_name != NULL)
	XtFree(self->menuarrow.menu_name);
}

/************************************************************
 *
 * ACTION procedures
 *
 ************************************************************/

/*
 * Popup: if we have a menu associated with us, request our parent to
 * pop it up.
 */
static void
Popup(w, event, params, num_params)
	Widget              w;
        XEvent             *event;
        String             *params;
        Cardinal           *num_params;
{
    MenuArrow           self = (MenuArrow) w;
    Widget              menu = NULL, temp;
    Arg                 arglist[2];
    int                 menu_x, menu_y, menu_width, menu_height;
    Position            arrow_x, arrow_y;

    if (self->menuarrow.menu_name == NULL) 
	return;

   /* Find the widget to popup */
    for (temp = w; temp != NULL; temp = XtParent(temp)) {
        if ((menu = XtNameToWidget(temp, self->menuarrow.menu_name)) != NULL)
            break;
    }

    if (menu == NULL)
        return;
   /* Not realized, realize it first */
    if (!XtIsRealized(menu))
        XtRealizeWidget(menu);

   /* Figure out where to place it when it is shown */
    menu_width = menu->core.width + 2 * menu->core.border_width;
    menu_height = menu->core.height + 2 * menu->core.border_width;

    if (self->menuarrow.align_parent) 
    	XtTranslateCoords(XtParent(w), 0, 0, &arrow_x, &arrow_y);
    else
    	XtTranslateCoords(w, 0, 0, &arrow_x, &arrow_y);
    menu_x = arrow_x;
    switch (self->menuarrow.menu_justify) {
    case XtJustifyCenter:
        menu_x += (self->core.width - menu_width) / 2;
        break;
    case XtJustifyRight:
        break;
    case XtJustifyLeft:
    default:
	menu_x -= menu_width - self->core.width;
        break;
    }

    if (self->menuarrow.align_parent) 
        menu_y = arrow_y + XtParent(w)->core.height;
    else
        menu_y = arrow_y + self->core.height;

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
    XtPopupSpringLoaded(menu);

}


/*
 * Popdown: if we have a menu associated with us, request our parent to
 * pop it up.
 */
static void
Popdown(w, event, params, num_params)
	Widget              w;
        XEvent             *event;
        String             *params;
        Cardinal           *num_params;
{
    MenuArrow           self = (MenuArrow) w;
    Widget              menu = NULL, temp;

    if (self->menuarrow.menu_name == NULL) 
	return;

   /* Find the widget to popup */
    for (temp = w; temp != NULL; temp = XtParent(temp)) {
        if ((menu = XtNameToWidget(temp, self->menuarrow.menu_name)) != NULL)
            break;
    }

    if (menu == NULL)
        return;
   /* If realized, pop it down */
    if (XtIsRealized(menu)) 
	    XtPopdown(menu);

}


