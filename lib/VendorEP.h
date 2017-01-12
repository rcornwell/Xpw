/*
 *
 * Vendor Shell. Taken from X11R6.3.
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
 * $Id: VendorEP.h,v 1.1 1997/11/29 01:17:56 rich Exp $
 *
 * $Log: VendorEP.h,v $
 * Revision 1.1  1997/11/29 01:17:56  rich
 * Initial revision
 *
 *
 */

/* $XConsortium: VendorEP.h,v 1.2 94/04/17 20:13:25 kaleb Exp $ */

/*
 * Copyright 1991 by OMRON Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of OMRON not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  OMRON makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * OMRON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * OMRON BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE. 
 *
 *      Author: Seiji Kuwari    OMRON Corporation
 *                              kuwa@omron.co.jp
 *                              kuwa%omron.co.jp@uunet.uu.net
 */

/*
 * 
 * Copyright (c) 1994  X Consortium
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * Except as contained in this notice, the name of the X Consortium shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from the X Consortium.
 * 
 */

#ifndef _VendorEP_h
#define _VendorEP_h

#include "XInputP.h"

typedef struct {
    XtPointer           extension;
} XpwVendorShellExtClassPart;

typedef struct _VendorShellExtClassRec {
    ObjectClassPart     object_class;
    XpwVendorShellExtClassPart vendor_shell_ext_class;
} XpwVendorShellExtClassRec;

typedef struct {
    Widget              parent;
    XpwImPart           im;
    XpwIcPart           ic;
} XpwVendorShellExtPart;

typedef struct XpwVendorShellExtRec {
    ObjectPart          object;
    XpwVendorShellExtPart vendor_ext;
} XpwVendorShellExtRec, *XpwVendorShellExtWidget;

#endif /* _VendorEP_h */
