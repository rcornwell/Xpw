/* 
 * PmeP.h - Private definitions for Pme object
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

#ifndef _XawPmeEntryP_h
#define _XawPmeEntryP_h

/***********************************************************************
 *
 * Pme Entry Private Data
 *
 ***********************************************************************/

#include "labelP.h"
#include "PmeEntry.h"

/************************************************************
 *
 * New fields for the Pme Entry class record.
 *
 ************************************************************/

typedef struct _PmeEntryClassPart {
    void                (*notify) ();
    XtPointer           extension;
} PmeEntryClassPart;

/* Full class record declaration */
typedef struct _PmeEntryClassRec {
    RectObjClassPart    rect_class;
    PmeEntryClassPart   pme_entry_class;
} PmeEntryClassRec;

extern PmeEntryClassRec pmeBSBClassRec;

/* New fields for the Pme Entry record */
typedef struct {
   /* resources */
    XtCallbackList      callbacks;	/* The callback list */
    _XpwLabel		label;

    String              menu_name;	/* Cascade Menu Name */
    String		clue;		/* Pop up clue */

} PmeEntryPart;

/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _PmeEntryRec {
    ObjectPart          object;
    RectObjPart         rectangle;
    PmeEntryPart        pme_entry;
} PmeEntryRec;

/************************************************************
 *
 * Private declarations.
 *
 ************************************************************/

#endif /* _XawPmeEntryP_h */
