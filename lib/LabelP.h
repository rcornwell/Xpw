/*
 * Label widget.
 *
 * Draw a label with a border around it.
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

#ifndef _LabelP_H_
#define _LabelP_H_
#include "Label.h"
#include "labelP.h"
#include "threeDdrawP.h"

typedef struct {
/* methods */
    int                 foo;	/* Null record entry */
/* class variables */
} LabelClassPart;

typedef struct _LabelClassRec {
    CoreClassPart       core_class;
    LabelClassPart      label_class;
} LabelClassRec;

typedef struct {
   /* resources */
    _XpwLabel		label;

   /* Shadow info */
    _XpmThreeDFrame	threeD;

   String		clue;
} LabelPart;

typedef struct _LabelRec {
    CorePart            core;
    LabelPart           label;
} LabelRec;

extern LabelClassRec labelClassRec;

#endif /* _LabelP_H_ */
