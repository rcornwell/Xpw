/*
 * Dialog Shell widget.
 *
 * The DialogShell is a subclass of TransientShell, it adds no resources. 
 * When any of it's children are managed it pop's itself up, it pop's itself
 * down whenever there is no managed children.
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
 * $Log: DialogShell.c,v $
 * Revision 1.1  1997/11/28 19:56:42  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: DialogShell.c,v 1.1 1997/11/28 19:56:42 rich Exp rich $";

#endif

/* Make sure all wm properties can make it out of the resource manager */

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/ShellP.h>
#include "DialogShellP.h"
#include "Dialog.h"
#include "focus.h"

/***************************************************************************
 *
 * DialogShell shell class record
 *
 ***************************************************************************/

static Boolean      SetValues(Widget /*old */ , Widget /*ref */ ,
				 Widget /*new */, ArgList /* args */,
				 Cardinal * /* num_args */ );
static void         Realize(Widget /*w */ , Mask * /*valueMask */ ,
				 XSetWindowAttributes * /*attributes */ );
static void         ChangeManaged(Widget /*w */ );

#define SuperClass (&transientShellClassRec)
DialogShellClassRec dialogShellClassRec =
{
    {	/* Core Class */
	(WidgetClass) SuperClass,	/* superclass     	*/
	"DialogShell",			/* class_name     	*/
	sizeof(DialogShellRec),		/* size           	*/
	NULL,				/* class_initialize     */
	NULL,				/* class_part_init      */
	FALSE,				/* Class init'ed ?      */
	NULL,				/* initialize         	*/
	NULL,				/* initialize_hook      */
	Realize,			/* realize              */
	NULL,				/* actions              */
	0,				/* num_actions    	*/
	NULL,				/* resources      	*/
	0,				/* resource_count       */
	NULLQUARK,			/* xrm_class      	*/
	FALSE,				/* compress_motion      */
	TRUE,				/* compress_exposure  	*/
	FALSE,				/* compress_enterleave 	*/
	FALSE,				/* visible_interest     */
	NULL,				/* destroy              */
	XtInheritResize,		/* resize               */
	NULL,				/* expose               */
	SetValues,			/* set_values     	*/
	NULL,				/* set_values_hook      */
	XtInheritSetValuesAlmost,	/* set_values_almost  	*/
	NULL,				/* get_values_hook      */
	NULL,				/* accept_focus   	*/
	XtVersion,			/* intrinsics version 	*/
	NULL,				/* callback offsets     */
	NULL,				/* tm_table             */
	NULL,				/* query_geometry       */
	NULL,				/* display_accelerator 	*/
	NULL				/* extension      	*/
    },
    {  /* Composite Class */
	XtInheritGeometryManager,	/* geometry_manager     */
	ChangeManaged,			/* change_managed       */
	XtInheritInsertChild,		/* insert_child   	*/
	XtInheritDeleteChild,		/* delete_child   	*/
	NULL				/* extension      	*/
    },
    { /* Shell Class */
	NULL				/* extension      	*/
    },
    { /* WMShell Class */
	NULL				/* extension      	*/
    },
    { /* VendorShell Class */
	NULL				/* extension      	*/
    },
    { /* TransientShell Class */
	NULL				/* extension      	*/
    },
    { /* DialogShell Class */
	NULL				/* extension      	*/
    }
};

WidgetClass         dialogShellWidgetClass = (WidgetClass)(&dialogShellClassRec);

/* ARGSUSED */
static Boolean 
SetValues(old, ref, new, args, num_args)
	Widget              old, ref, new;
	ArgList             args;	/* unused */
	Cardinal           *num_args;	/* unused */
{
    return FALSE;
}

static void
Realize(w, valueMask, attributes)
	Widget              w;
	Mask               *valueMask;
	XSetWindowAttributes *attributes;
{
    DialogShellWidget   self = (DialogShellWidget) w;
    Widget              child;
    int                 i;

   /* Make sure shell has a size */
    for (i = self->composite.num_children - 1; i >= 0; i--) {
	child = self->composite.children[i];

	if (self->core.width == 0)
	    self->core.width = child->core.width;
	if (self->core.height == 0)
	    self->core.height = child->core.height;
    }

   /* Let superclass finish it off */
    (*SuperClass->core_class.realize) (w, valueMask, attributes);
}

static void 
ChangeManaged(w)
	Widget              w;
{
    DialogShellWidget   self = (DialogShellWidget) w;
    Widget              child;
    int                 i;


   /* See if any widgets are managed */
    for (i = self->composite.num_children - 1; i >= 0; i--) {
	child = self->composite.children[i];
	if (XtIsManaged(child)) {
	/* Ask child what kind of grab it wants */
	    DialogType dialog_type = XpwDialog_Modal;
	    Arg		arg[1];
	    XtGrabKind	grab_kind;

	    XtSetArg(arg[0], XtNdialogType, &dialog_type);
	    XtGetValues(child, arg, 1);
	    switch(dialog_type) {
	    case XpwDialog_Modeless:
		grab_kind = XtGrabNone;
		break;
	    default:
	    case XpwDialog_Modal:
		grab_kind = XtGrabExclusive;
		break;
	    case XpwDialog_Dialog:
		grab_kind = XtGrabNonexclusive;
		break;
	    }
	    
	    XtPopup(w, grab_kind); 	/* If so pop us up */
	    _XpwSetFocus(child, w);
	    return;
	}
    }
   /* Nothing managed, pop us down */
    XtPopdown(w);
}
