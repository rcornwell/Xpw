/*
 * XpwInitialization.
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
 * $Log:$
 */

#include <X11/Intrinsic.h>
#include <X11/Vendor.h>
#include "XpwInit.h"

#ifndef lint
char rcsid[] = "$Id:$";
#endif

void 
XpwInitializeWidgetSet()
{
    static int          firsttime = 1;

    if (firsttime) {
	firsttime = 0;
	_XpwLabelClassInit();
	_XpwThreeDClassInit();
#if 0
	XtInitializeWidgetClass(vendorShellWidgetClass);
#endif
    }
}
