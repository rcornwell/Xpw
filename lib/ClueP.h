/*
 * Clue widget.
 *
 * Pop up a help box for a widget.
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
 */

/*
 * $Id$
 *
 * $Log:$
 *
 */

#ifndef _ClueP_H_
#define _ClueP_H_
#include "labelP.h"
#include "Clue.h"
#include <X11/ShellP.h>

typedef struct {
/* methods */
    int                 foo;	/* Null record entry */
/* class variables */
} ClueClassPart;

typedef struct _ClueClassRec {
    CoreClassPart       core_class;
    CompositeClassPart  composite_class;
    ShellClassPart      shell_class;
    OverrideShellClassPart override_shell_class;
    ClueClassPart       clue_class;
} ClueClassRec;

typedef struct {
   /* resources */
    _XpwLabel		label;		/* Label gadget to display clues in */
    XtIntervalId	timer;		/* Timer for popup's */
    int			timeout;	/* How long to wait before showing popup */
    Widget		active;		/* Widget to popup for */
    Boolean		showing;	/* Is it currently popped up? */
} CluePart;

typedef struct _ClueRec {
    CorePart            core;
    CompositePart       composite;
    ShellPart           shell;
    OverrideShellPart   override;
    CluePart            clue;
} ClueRec;

extern ClueClassRec clueClassRec;

#endif /* _ClueP_H_ */
