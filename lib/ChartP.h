/*
 * Chart widget.
 *
 * Draws a strip chart.
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
 * $Id: ChartP.h,v 1.1 1997/10/15 04:57:46 rich Exp rich $
 *
 * $Log: ChartP.h,v $
 * Revision 1.1  1997/10/15 04:57:46  rich
 * Initial revision
 *
 *
 */

#ifndef _ChartP_H_
#define _ChartP_H_
#include "Chart.h"
#include "labelP.h"
#include "threeDdrawP.h"

/*********************************************************************
 *
 * Chart Widget Private Data
 *
 *********************************************************************/

/* New fields for the Chart widget class record */

typedef struct {
    int                 foo;	/* Null record entry */
} ChartClassPart;

/* Full class record declaration */
typedef struct _ChartClassRec {
    CoreClassPart       core_class;
    ChartClassPart      chart_class;
} ChartClassRec;

/* New fields for the Chart widget Record */
typedef struct {
   /* resources */
   int			interval;	/* How often to collect new samples. */
   int			minScale;	/* Min number of lines */
   int			scrollAmount;	/* How many points to slide when
					 * scrolling. */
   Pixel		data_color;
   Pixel		scale_color;
   XtCallbackList	getData;

   _XpwLabel		label;
   String		clue;

   /* Shadow info */
    _XpmThreeDFrame	threeD;
   GC			lines_gc;
   GC			data_gc;
   Dimension		label_height;
   int			nextpoint;	/* Where to insert next point */
   int			scale;		/* Amount to scale data when colected */
   double		maxvalue;	/* Max value in data buffer */
   double		data[1024];	/* Data collected */
   XtIntervalId         timer;          /* Data collection timeout. */

} ChartPart;

/* Full instance record declaration */
typedef struct _ChartRec {
    CorePart            core;
    ChartPart           chart;
} ChartRec;

extern ChartClassRec chartClassRec;

#endif /* _ChartP_H_ */
