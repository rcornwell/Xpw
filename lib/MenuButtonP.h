/*
 * Menu button widget.
 *
 * This is the menu button widget. It is a subclass of Command. It is used
 * when you don't want to use a menubar for pull down menus.
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
 */

/* 
 * MenuButtonP.h - Private definitions for MenuButton object
 *
 * $Id$
 *
 * $Log:$
 * 
 */

#ifndef _MenuButtonP_h
#define _MenuButtonP_h

/***********************************************************************
 *
 * Private Data
 *
 ***********************************************************************/

#include "CommandP.h"
#include "MenuButton.h"

/************************************************************
 *
 * New fields for the MenuButton Object class record.
 *
 ************************************************************/
typedef struct _MenuButtonClassPart {
    XtPointer           extension;
} MenuButtonClassPart;

/* Full class record declaration */
typedef struct _MenuButtonClassRec {
    CoreClassPart       core_class;
    CommandClassPart	comand_class;
    MenuButtonClassPart menubutton_class;
} MenuButtonClassRec;

extern MenuButtonClassRec menubuttonClassRec;

/* New fields for the MenuButton Object record */
typedef struct {
   /* resources */
    String              menu_name;	/* Sub Menu... used by parent! */
} MenuButtonPart;

/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _MenuButtonRec {
    CorePart            core;
    CommandPart		command;
    MenuButtonPart      menubutton;
} MenuButtonRec;

/************************************************************
 *
 * Private declarations.
 *
 ************************************************************/

#endif /* _MenuButtonP_h */
