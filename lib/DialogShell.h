/*
 * DialogShell Widget.
 *
 * The DialogShell widget adds geometry management and autopopup/down to
 * the TransientShell.
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
 * $Id: DialogShell.h,v 1.1 1997/11/28 19:56:42 rich Exp $
 *
 * $Log: DialogShell.h,v $
 * Revision 1.1  1997/11/28 19:56:42  rich
 * Initial revision
 *
 *
 */

#ifndef _DialogShell_h
#define _DialogShell_h

/* Class record constants */

typedef struct _DialogShellClassRec *DialogShellWidgetClass;

#ifndef SHELL 
extern WidgetClass dialogShellWidgetClass;
#endif

#endif	/* _DialogShell_h */
