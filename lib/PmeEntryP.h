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
 * $Id: PmeEntryP.h,v 1.2 1997/10/05 02:17:28 rich Exp rich $
 *
 * $Log: PmeEntryP.h,v $
 * Revision 1.2  1997/10/05 02:17:28  rich
 * Added callbacks into class for retrieving menu_name and doing clue handling.
 *
 * Revision 1.1  1997/10/04 05:07:44  rich
 * Initial revision
 *
 *
 */

#ifndef _PmeEntryP_h
#define _PmeEntryP_h
#include "labelP.h"
#include "PmeEntry.h"

/***********************************************************************
 *
 * PmeEntry object Private Data
 *
 ***********************************************************************/

/* New fields for the PmeEntry widget class record */
typedef struct _PmeEntryClassPart {
    void                (*notify) ();
    void                (*highlight) ();
    void                (*unhighlight) ();
    char	       *(*getmenuname) ();
    XtPointer           extension;
} PmeEntryClassPart;

/* Full class record declaration */
typedef struct _PmeEntryClassRec {
    RectObjClassPart    rect_class;
    PmeEntryClassPart   pme_entry_class;
} PmeEntryClassRec;

extern PmeEntryClassRec pmeEntryClassRec;

/* New fields for the PmeEntry object record */
typedef struct {
   /* resources */
    _XpwLabel		label;
    String              menu_name;	/* Cascade Menu Name */
    String		clue;		/* Pop up clue */
    XtCallbackList      callbacks;	/* The callback list */

} PmeEntryPart;

/* Full instance record declaration */
typedef struct _PmeEntryRec {
    ObjectPart          object;
    RectObjPart         rectangle;
    PmeEntryPart        pme_entry;
} PmeEntryRec;

#endif /* _PmeEntryP_h */
