/*
 * Vendor widget.
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
 * $Log: Vendor.c,v $
 * Revision 1.1  1997/11/29 01:17:56  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: Vendor.c,v 1.1 1997/11/29 01:17:56 rich Exp rich $";
#endif

/* $XConsortium: Vendor.c,v 1.27 94/04/17 20:13:25 kaleb Exp $ */

/***********************************************************

Copyright (c) 1987, 1988, 1994  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.

Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/* $XFree86: xc/lib/Xpw/Vendor.c,v 1.1.1.1.12.2 1997/07/19 07:07:11 dawes Exp $ */

/*
 * This is a copy of Xt/Vendor.c with an additional ClassInitialize
 * procedure to register Xmu resource type converters, and all the
 * monkey business associated with input methods...
 *
 */

/* Make sure all wm properties can make it out of the resource manager */

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/ShellP.h>
#include <X11/VendorP.h>
#include <X11/Xmu/Converters.h>
#include <X11/Xmu/Atoms.h>
#include <X11/Xmu/Editres.h>
#include <X11/Xmu/ExtAgent.h>

/* The following two headers are for the input method. */

#include "VendorEP.h"
#include "XInputP.h"
#include "focus.h"

static XtResource   resources[] =
{
    {XtNinput, XtCInput, XtRBool, sizeof(Bool),
     XtOffsetOf(VendorShellRec, wm.wm_hints.input),
     XtRImmediate, (XtPointer) True}
};

/***************************************************************************
 *
 * Vendor shell class record
 *
 ***************************************************************************/

#ifdef __EMX__
/* to fix the EditRes problem because of wrong linker semantics */
extern WidgetClass  vendorShellWidgetClass;	/* from Xt/Vendor.c */
extern VendorShellClassRec _XpwVendorShellClassRec;
extern void         _XpwFixupVendorShell();
unsigned long
_DLL_InitTerm(unsigned long mod, unsigned long flag)
{
    switch (flag) {
    case 0:			/*called on init */
	_CRT_init();
	vendorShellWidgetClass = (WidgetClass) (&_XpwVendorShellClassRec);
	_XpwFixupVendorShell();
	return 1;
    case 1:			/*called on exit */
	return 1;
    default:
	return 0;
    }
}
#define vendorShellClassRec _XpwVendorShellClassRec

#endif

/* Global definitions */
static Boolean      XpwCvtCompoundTextToString(Display * /*dpy */ ,
					       XrmValuePtr /*args */ ,
					       Cardinal * /*num_args */ ,
					       XrmValue * /*fromVal */ ,
					       XrmValue * /*toVal */ ,
					     XtPointer * /*cvt_data */ );
static void         XpwVendorShellClassInitialize(void);
static void         XpwVendorShellClassPartInit(WidgetClass /*class */ );
static void         XpwVendorShellInitialize(Widget /*req */ ,
				    Widget /*new */ , ArgList /*args */ ,
					     Cardinal * /*num_args */ );
static Boolean      XpwVendorShellSetValues(Widget /*old */ ,
				      Widget /*ref */ , Widget /*new */ ,
			  ArgList /*args */ , Cardinal * /*num_args */ );
static void         Realize(Widget /*wid */ , Mask * /*vmask */ ,
			    XSetWindowAttributes * /*attr */ );
static void         XpwVendorShellExtClassInitialize(void);
static void         XpwVendorShellExtInitialize(Widget /*req */ ,
				    Widget /*new */ , ArgList /*args */ ,
					      Cardinal * /*num_args */ );
static void         XpwVendorShellExtDestroy(Widget /*w */ );
static Boolean      XpwVendorShellExtSetValues(Widget /*old */ ,
				      Widget /*ref */ , Widget /*new */ ,
			  ArgList /*args */ , Cardinal * /*num_args */ );
void                XpwVendorShellExtResize(Widget /*w */ );
static XtGeometryResult GeometryManager(Widget /*wid */ ,
					XtWidgetGeometry * /*request */ ,
					XtWidgetGeometry * /*reply */ );
static void         ChangeManaged(Widget /*wid */ );

static CompositeClassExtensionRec vendorCompositeExt =
{
    /* next_extension     */ NULL,
    /* record_type        */ NULLQUARK,
    /* version            */ XtCompositeExtensionVersion,
    /* record_size        */ sizeof(CompositeClassExtensionRec),
    /* accepts_objects    */ TRUE,
    /* allows_change_managed_set */ FALSE
};

#define SuperClass (&wmShellClassRec)
VendorShellClassRec vendorShellClassRec =
{
    {
	(WidgetClass) SuperClass,	/* superclass          */
	"VendorShell",			/* class_name          */
	sizeof(VendorShellRec),		/* size                */
	XpwVendorShellClassInitialize,	/* class_initialize    */
	XpwVendorShellClassPartInit,	/* class_part_init     */
	FALSE,				/* Class init'ed ?     */
	XpwVendorShellInitialize,	/* initialize          */
	NULL,				/* initialize_hook     */
	Realize,			/* realize             */
	NULL,				/* actions             */
	0,				/* num_actions         */
	resources,			/* resources           */
	XtNumber(resources),		/* resource_count      */
	NULLQUARK,			/* xrm_class           */
	FALSE,				/* compress_motion     */
	TRUE,				/* compress_exposure   */
	FALSE,				/* compress_enterleave */
	FALSE,				/* visible_interest    */
	NULL,				/* destroy             */
	XpwVendorShellExtResize,	/* resize              */
	NULL,				/* expose              */
	XpwVendorShellSetValues,	/* set_values          */
	NULL,				/* set_values_hook     */
	XtInheritSetValuesAlmost,	/* set_values_almost   */
	NULL,				/* get_values_hook     */
	NULL,				/* accept_focus        */
	XtVersion,			/* intrinsics version  */
	NULL,				/* callback offsets    */
	NULL,				/* tm_table            */
	NULL,				/* query_geometry      */
	NULL,				/* display_accelerator */
	NULL				/* extension           */
    },
    {
	GeometryManager,		/* geometry_manager    */
	ChangeManaged,			/* change_managed      */
	XtInheritInsertChild,		/* insert_child        */
	XtInheritDeleteChild,		/* delete_child        */
	(XtPointer) & vendorCompositeExt /* extension          */
    },
    {
	NULL				/* extension           */
    },
    {
	NULL				/* extension           */
    },
    {
	NULL				/* extension           */
    }
};

#ifndef __EMX__
WidgetClass         vendorShellWidgetClass = (WidgetClass) (&vendorShellClassRec);

#endif

/***************************************************************************
 *
 * The following section is for the Vendor shell Extension class record
 *
 ***************************************************************************/

static XtResource   ext_resources[] =
{
    {XtNinputMethod, XtCInputMethod, XtRString, sizeof(String),
     XtOffsetOf(XpwVendorShellExtRec, vendor_ext.im.input_method),
     XtRString, (XtPointer) NULL},
    {XtNpreeditType, XtCPreeditType, XtRString, sizeof(String),
     XtOffsetOf(XpwVendorShellExtRec, vendor_ext.im.preedit_type),
     XtRString, (XtPointer) "OverTheSpot,OffTheSpot,Root"},
    {XtNopenIm, XtCOpenIm, XtRBoolean, sizeof(Boolean),
     XtOffsetOf(XpwVendorShellExtRec, vendor_ext.im.open_im),
     XtRImmediate, (XtPointer) TRUE},
    {XtNsharedIc, XtCSharedIc, XtRBoolean, sizeof(Boolean),
     XtOffsetOf(XpwVendorShellExtRec, vendor_ext.ic.shared_ic),
     XtRImmediate, (XtPointer) FALSE}
};

XpwVendorShellExtClassRec
xpwvendorShellExtClassRec = {
    {
	(WidgetClass) & objectClassRec,		/* superclass            */
	    "VendorShellExt",			/* class_name            */
	    sizeof(XpwVendorShellExtRec),	/* size                  */
	    XpwVendorShellExtClassInitialize,	/* class_initialize      */
	    NULL,				/* class_part_initialize */
	    FALSE,				/* Class init'ed ?       */
	    XpwVendorShellExtInitialize,	/* initialize            */
	    NULL,				/* initialize_hook       */
	    NULL,				/* pad                   */
	    NULL,				/* pad                   */
	    0,					/* pad                   */
	    ext_resources,			/* resources             */
	    XtNumber(ext_resources),		/* resource_count        */
	    NULLQUARK,				/* xrm_class             */
	    FALSE,				/* pad                   */
	    FALSE,				/* pad                   */
	    FALSE,				/* pad                   */
	    FALSE,				/* pad                   */
	    XpwVendorShellExtDestroy,		/* destroy               */
	    NULL,				/* pad                   */
	    NULL,				/* pad                   */
	    XpwVendorShellExtSetValues,		/* set_values            */
	    NULL,				/* set_values_hook       */
	    NULL,				/* pad                   */
	    NULL,				/* get_values_hook       */
	    NULL,				/* pad                   */
	    XtVersion,				/* version               */
	    NULL,				/* callback_offsets      */
	    NULL,				/* pad                   */
	    NULL,				/* pad                   */
	    NULL,				/* pad                   */
	    NULL				/* extension             */
    },
    {
	NULL					/* extension             */
    }
};

WidgetClass         xpwvendorShellExtWidgetClass = (WidgetClass) (&xpwvendorShellExtClassRec);

/*ARGSUSED */
static              Boolean
XpwCvtCompoundTextToString(dpy, args, num_args, fromVal, toVal, cvt_data)
	Display            *dpy;
	XrmValuePtr         args;
	Cardinal           *num_args;
	XrmValue           *fromVal;
	XrmValue           *toVal;
	XtPointer          *cvt_data;
{
    XTextProperty       prop;
    char              **list;
    int                 count;
    static char        *mbs = NULL;
    int                 len;

    prop.value = (unsigned char *) fromVal->addr;
    prop.encoding = XA_COMPOUND_TEXT(dpy);
    prop.format = 8;
    prop.nitems = fromVal->size;

    if (XmbTextPropertyToTextList(dpy, &prop, &list, &count) < Success) {
	XtAppWarningMsg(XtDisplayToApplicationContext(dpy),
		    "converter", "XmbTextPropertyToTextList", "XpwError",
			"conversion from CT to MB failed.", NULL, 0);
	return False;
    }
    len = strlen(*list);
    toVal->size = len;
    mbs = XtRealloc(mbs, len + 1); /* keep buffer because no one call free :( */
    strcpy(mbs, *list);
    XFreeStringList(list);
    toVal->addr = (XtPointer) mbs;
    return True;
}

static void
XpwVendorShellClassInitialize()
{
    static XtConvertArgRec screenConvertArg[] =
    {
     {XtWidgetBaseOffset, (XtPointer) XtOffsetOf(WidgetRec, core.screen),
      sizeof(Screen *)}
    };

    XtAddConverter(XtRString, XtRCursor, XmuCvtStringToCursor,
		   screenConvertArg, XtNumber(screenConvertArg));

    XtAddConverter(XtRString, XtRBitmap, XmuCvtStringToBitmap,
		   screenConvertArg, XtNumber(screenConvertArg));

    XtSetTypeConverter("CompoundText", XtRString, XpwCvtCompoundTextToString,
		       NULL, 0, XtCacheNone, NULL);
}

static void
XpwVendorShellClassPartInit(class)
	WidgetClass         class;
{
    CompositeClassExtension ext;
    VendorShellWidgetClass vsclass = (VendorShellWidgetClass) class;

    if ((ext = (CompositeClassExtension)
	 XtGetClassExtension(class,
			     XtOffsetOf(CompositeClassRec,
					composite_class.extension),
			     NULLQUARK, 1L, (Cardinal) 0)) == NULL) {
	ext = (CompositeClassExtension) XtNew(CompositeClassExtensionRec);
	if (ext != NULL) {
	    ext->next_extension = vsclass->composite_class.extension;
	    ext->record_type = NULLQUARK;
	    ext->version = XtCompositeExtensionVersion;
	    ext->record_size = sizeof(CompositeClassExtensionRec);
	    ext->accepts_objects = TRUE;
	    ext->allows_change_managed_set = FALSE;
	    vsclass->composite_class.extension = (XtPointer) ext;
	}
    }
}

#if defined(__osf__) || defined(__EMX__)
/* stupid OSF/1 shared libraries have the wrong semantics */
/* symbols do not get resolved external to the shared library */
void
_XpwFixupVendorShell()
{
    transientShellWidgetClass->core_class.superclass =
	(WidgetClass) & vendorShellClassRec;
    topLevelShellWidgetClass->core_class.superclass =
	(WidgetClass) & vendorShellClassRec;
}

#endif

/* ARGSUSED */
static void
XpwVendorShellInitialize(req, new, args, num_args)
	Widget              req, new;
	ArgList             args;
	Cardinal           *num_args;
{
    XtAddEventHandler(new, (EventMask) 0, TRUE, _XEditResCheckMessages, NULL);
    XtAddEventHandler(new, (EventMask) 0, TRUE, XmuRegisterExternalAgent, NULL);
    XtCreateWidget("shellext", xpwvendorShellExtWidgetClass,
		   new, args, *num_args);
}

/* ARGSUSED */
static              Boolean
XpwVendorShellSetValues(old, ref, new, args, num_args)
	Widget              old, ref, new;
	ArgList             args;
	Cardinal           *num_args;

{
    return FALSE;
}

static void
Realize(wid, vmask, attr)
	Widget              wid;
	Mask               *vmask;
	XSetWindowAttributes *attr;
{
    WidgetClass         super = wmShellWidgetClass;

   /* Make my superclass do all the dirty work */

    (*super->core_class.realize) (wid, vmask, attr);
    _XpwImRealize(wid);
}

static void
XpwVendorShellExtClassInitialize()
{
}

/* ARGSUSED */
static void
XpwVendorShellExtInitialize(req, new, args, num_args)
	Widget              req, new;
	ArgList             args;
	Cardinal           *num_args;
{
    _XpwImInitialize(new->core.parent, new);
}

/* ARGSUSED */
static void
XpwVendorShellExtDestroy(w)
	Widget              w;
{
    _XpwImDestroy(w->core.parent, w);
}

/* ARGSUSED */
static              Boolean
XpwVendorShellExtSetValues(old, ref, new, args, num_args)
	Widget              old, ref, new;
	ArgList             args;
	Cardinal           *num_args;
{
    return FALSE;
}

void
XpwVendorShellExtResize(w)
	Widget              w;
{
    ShellWidget         sw = (ShellWidget) w;
    Widget              childwid;
    int                 i;
    Dimension           core_height;

    _XpwImResizeVendorShell(w);
    core_height = _XpwImGetShellHeight(w);
    for (i = 0; i < sw->composite.num_children; i++) {
	if (XtIsManaged(sw->composite.children[i])) {
	    childwid = sw->composite.children[i];
	    XtResizeWidget(childwid, sw->core.width, core_height,
			   childwid->core.border_width);
	}
    }
}

/*ARGSUSED */
static              XtGeometryResult
GeometryManager(wid, request, reply)
	Widget              wid;
	XtWidgetGeometry   *request;
	XtWidgetGeometry   *reply;
{
    ShellWidget         shell = (ShellWidget) (wid->core.parent);
    XtWidgetGeometry    my_request;

    if (shell->shell.allow_shell_resize == FALSE && XtIsRealized(wid))
	return (XtGeometryNo);

    if (request->request_mode & (CWX | CWY))
	return (XtGeometryNo);

   /* %%% worry about XtCWQueryOnly */
    my_request.request_mode = 0;
    if (request->request_mode & CWWidth) {
	my_request.width = request->width;
	my_request.request_mode |= CWWidth;
    }
    if (request->request_mode & CWHeight) {
	my_request.height = request->height
	    + _XpwImGetImAreaHeight(wid);
	my_request.request_mode |= CWHeight;
    }
    if (request->request_mode & CWBorderWidth) {
	my_request.border_width = request->border_width;
	my_request.request_mode |= CWBorderWidth;
    }
    if (XtMakeGeometryRequest((Widget) shell, &my_request, NULL)
	== XtGeometryYes) {
       /* assert: if (request->request_mode & CWWidth) then
        *                  shell->core.width == request->width
        * assert: if (request->request_mode & CWHeight) then
        *            shell->core.height == request->height
        *
        * so, whatever the WM sized us to (if the Shell requested
        * only one of the two) is now the correct child size
        */

	wid->core.width = shell->core.width;
	wid->core.height = shell->core.height;
	if (request->request_mode & CWBorderWidth) {
	    wid->core.x = wid->core.y = -request->border_width;
	}
	_XpwImCallVendorShellExtResize(wid);
	return XtGeometryYes;
    } else
	return XtGeometryNo;
}

static void
ChangeManaged(wid)
	Widget              wid;
{
    ShellWidget         w = (ShellWidget) wid;
    Widget             *childP;
    int                 i;

    (*SuperClass->composite_class.change_managed) (wid);
    for (i = w->composite.num_children, childP = w->composite.children;
	 i; i--, childP++) {
	if (XtIsManaged(*childP)) {
	    _XpwSetFocus(*childP, wid);
	    break;
	}
    }
}
