/*
 * PopupMenuP.h - Private Header file for PopupMenu widget.
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
 */

/*
 * $Id$
 *
 * $Log:$
 *
 */

#ifndef _PopupMenuP_h
#define _PopupMenuP_h

#include "PopupMenu.h"
#include "PmeEntryP.h"
#include "threeDdrawP.h"
#include <X11/ShellP.h>

typedef struct {
    XtPointer           extension;	/* For future needs. */
} PopupMenuClassPart;

typedef struct _PopupMenuClassRec {
    CoreClassPart       core_class;
    CompositeClassPart  composite_class;
    ShellClassPart      shell_class;
    OverrideShellClassPart override_shell_class;
    PopupMenuClassPart  simpleMenu_class;
} PopupMenuClassRec;

extern PopupMenuClassRec popupMenuClassRec;

typedef struct _PopupMenuPart {

   /* resources */

    String              label_string;	/* The string for the label or NULL. */
    PmeEntryObject      label;	/* If label_string is non-NULL then this is
				 * the label widget. */
    WidgetClass         label_class;	/* Widget Class of the menu label object. */

    Dimension           top_margin;	/* Top and bottom margins. */
    Dimension           bottom_margin;
    Dimension           row_height;	/* height of each row (menu entry) */

    Cursor              cursor;	/* The menu's cursor. */
    PmeEntryObject      popup_entry;	/* The entry to position the cursor on for
					 * when using XawPositionPopupMenu. */
    Boolean             menu_on_screen;		/* Force the menus to be fully on the screen. */
    int                 backing_store;	/* What type of backing store to use. */

   _XpmThreeDFrame	threeD;
   /* private state */

    Boolean             recursive_set_values;	/* contain a possible infinite loop. */

    Boolean             menu_width;	/* If true then force width to remain 
					 * core.width */
    Boolean             menu_height;	/* Just like menu_width, but for height. */

    PmeEntryObject      entry_set;	/* The entry that is currently set or
					 * highlighted. */
    XtIntervalId        entry_timer;	/* Start a timeout when a cascade menu is
					 * selected, popup when timer runs out */
    Widget              current_popup;	/* Currently popped up cascade menu */

} PopupMenuPart;

typedef struct _PopupMenuRec {
    CorePart            core;
    CompositePart       composite;
    ShellPart           shell;
    OverrideShellPart   override;
    PopupMenuPart       popup_menu;
} PopupMenuRec;

#endif /* _PopupMenuP_h */
