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
 * $Id: MenuButtonP.h,v 1.2 1997/10/15 05:42:56 rich Exp rich $
 *
 * $Log: MenuButtonP.h,v $
 * Revision 1.2  1997/10/15 05:42:56  rich
 * Added justification to pulldown menus.
 *
 * Revision 1.1  1997/10/04 05:06:55  rich
 * Initial revision
 *
 * 
 */

#ifndef _MenuButtonP_h
#define _MenuButtonP_h
#include "CommandP.h"
#include "MenuButton.h"

/*********************************************************************
 *
 * MenuButton Widget Private Data
 *
 *********************************************************************/

/* New fields for the MenuButton widget class record */
typedef struct _MenuButtonClassPart {
    int                foo;	/* Null record entry */
} MenuButtonClassPart;

/* Full class record declaration */
typedef struct _MenuButtonClassRec {
    CoreClassPart       core_class;
    CommandClassPart	comand_class;
    MenuButtonClassPart menubutton_class;
} MenuButtonClassRec;

extern MenuButtonClassRec menubuttonClassRec;

/* New fields for the MenuButton widget record */
typedef struct {
   /* resources */
    String              menu_name;	/* Sub Menu... */
    XtJustify           menu_justify;   /* Justification for the popup. */
} MenuButtonPart;

/* Full instance record declaration */
typedef struct _MenuButtonRec {
    CorePart            core;
    CommandPart		command;
    MenuButtonPart      menubutton;
} MenuButtonRec;


#endif /* _MenuButtonP_h */
