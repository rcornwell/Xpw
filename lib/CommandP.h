/*
 * Command widget.
 *
 * Command button widget.
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

#ifndef _CommandP_H_
#define _CommandP_H_
#include "Command.h"
#include "labelP.h"
#include "threeDdrawP.h"

typedef enum {
  HighlightNone,                /* Do not highlight. */
  HighlightWhenUnset,           /* Highlight only when unset, this is
                                   to preserve current command widget 
                                   functionality. */
  HighlightAlways               /* Always highlight, lets the toggle widget
                                   and other subclasses do the right thing. */
} XtCommandHighlight;

typedef struct {
/* methods */
    int                 foo;	/* Null record entry */
/* class variables */
} CommandClassPart;

typedef struct _CommandClassRec {
    CoreClassPart       core_class;
    CompositeClassPart  composite_class;
    CommandClassPart      command_class;
} CommandClassRec;

typedef struct {
   /* resources */
    _XpwLabel		label;	/* Label to display */

   /* Shadow info */
    _XpmThreeDFrame	threeD; /* ThreeD highlight part */

   /* Command Stuff */
    Dimension   highlight_thickness; /* Highlight thickness */
    XtCallbackList callbacks;

    /* private state */
    Pixmap              gray_pixmap;
    GC                  normal_GC;
    GC                  inverse_GC;
    Boolean             set;
    XtCommandHighlight  highlighted;
    String		clue;
} CommandPart;

typedef struct _CommandRec {
    CorePart            core;
    CommandPart         command;
} CommandRec;

extern CommandClassRec commandClassRec;

#endif /* _CommandP_H_ */
