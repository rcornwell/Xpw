/*
 * SimpleText widget.
 *
 * SimpleText display and edit of ascii text in up to 4 fonts.
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
 *
 */

#ifndef lint
static char        *rcsid = "$Id$";

#endif

#include <stdio.h>
#include <ctype.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Xmu/Xmu.h>
#include <X11/Xmu/Drawing.h>

#include "XpwInit.h"
#include "SimpleTextP.h"
#include "Scroller.h"
#include "Clue.h"

/* Some short cuts for some repeated text */
#define TextLen(self, str, len, fnt) \
    ((self->text.international == True)? \
	XmbTextEscapement(self->text.fontset[fnt], str, len) : \
	XTextWidth(self->text.font[fnt], str, len))

#define TextHigh(self, fnt) \
    ((self->text.international == True)? \
        ((XExtentsOfFontSet(self->text.fontset[fnt]))->max_ink_extent.height) \
	: self->text.font[fnt]->max_bounds.ascent + \
		self->text.font[fnt]->max_bounds.descent)
            


static void         Initialize(Widget /*junk */ , Widget /*new */ , ArgList /*args */ , Cardinal * /*num_args */ );
static void         Redisplay(Widget /*wid */ , XEvent * /*event */ , Region /*region */ );
static XtGeometryResult QueryGeometry(Widget /*w */ , XtWidgetGeometry * /*intended */ , XtWidgetGeometry * /*requested */ );
static void         Resize(Widget /*w */ );
static void	    BuildLineTable(SimpleTextWidget /*self*/);
static void	    Layout(SimpleTextWidget /*self*/, Dimension */*width*/, Dimension */*height*/, int /*set*/);
static void	    HandleScroll(Widget /*wid*/, XtPointer /*client_data*/, XtPointer /*call_data*/);
static void   	    MoveWindow(Widget /*wid*/, int /*nxoff*/, int /*nyoff*/);
static int	    ReCenterCursor(Widget /*w*/);
static void         AdjustPoint(Widget /*w */ , int /*newpoint */ );
static void         GrowBuffer(Widget /*w */ , int /*num */ );
static void         EraseCursor(Widget /*w */ );
static void         DrawCursor(Widget /*w */ );
static int          DeleteHighlight(Widget /*w */ );
static void         ClearHighlight(Widget /*w */ );
static int          OffsetToChar(Widget /*w */ , int /*offset */ );
static int          CharToOffset(Widget /*w */ , int /*pos */ );
static Boolean      ConvertSelection(Widget /*w */ , Atom * /*selection */ , Atom * /*target */ , Atom * /*type */ , XtPointer * /*value */ , unsigned long * /*length */ , int * /*format */ );
static void         LoseSelection(Widget /*w */ , Atom * /*selection */ );
static void         RequestSelection(Widget /*w */ , XtPointer /*client */ , Atom * /*selection */ , Atom * /*type */ , XtPointer /*value */ , unsigned long * /*length */ , int * /*format */ );
static Boolean      SetValues(Widget /*current */ , Widget /*request */ , Widget /*new */ , ArgList /*args */ , Cardinal * /*num_args */ );
static void         Destroy(Widget /*w */ );
static void         CreateGCs(Widget /*w */ );
static void         DestroyGCs(Widget /*w */ );

#define INSERTBUFSIZ 32

/* Actions */
static void         ArmClue(Widget /*w */ , XEvent * /*event */ , String * /*params */ , Cardinal * /*num_params */ );
static void         DisArmClue(Widget /*w */ , XEvent * /*event */ , String * /*params */ , Cardinal * /*num_params */ );
static void         Notify(Widget /*w */ , XEvent * /*event */ , String * /*params */ , Cardinal * /*num_params */ );
static void         InsertChar(Widget /*w */ , XEvent * /*event */ , String * /*params */ , Cardinal * /*num_params */ );
static void         DeleteChar(Widget /*w */ , XEvent * /*event */ , String * /*params */ , Cardinal * /*num_params */ );
static void         RightChar(Widget /*w */ , XEvent * /*event */ , String * /*params */ , Cardinal * /*num_params */ );
static void         LeftChar(Widget /*w */ , XEvent * /*event */ , String * /*params */ , Cardinal * /*num_params */ );
static void         EndOfLine(Widget /*w */ , XEvent * /*event */ , String * /*params */ , Cardinal * /*num_params */ );
static void         BeginOfLine(Widget /*w */ , XEvent * /*event */ , String * /*params */ , Cardinal * /*num_params */ );
static void         StartSelect(Widget /*w */ , XEvent * /*event */ , String * /*params */ , Cardinal * /*num_params */ );
static void         ExtendSelect(Widget /*w */ , XEvent * /*event */ , String * /*params */ , Cardinal * /*num_params */ );
static void         EndSelect(Widget /*w */ , XEvent * /*event */ , String * /*params */ , Cardinal * /*num_params */ );
static void         InsertSelect(Widget /*w */ , XEvent * /*event */ , String * /*params */ , Cardinal * /*num_params */ );
static void         DeleteToEOL(Widget /*w */ , XEvent * /*event */ , String * /*params */ , Cardinal * /*num_params */ );
static void         DeleteWord(Widget /*w */ , XEvent * /*event */ , String * /*params */ , Cardinal * /*num_params */ );

static XtActionsRec actions[] =
{
    {"ArmClue", ArmClue},
    {"DisArmClue", DisArmClue},
    {"Notify", Notify},
    {"InsertChar", InsertChar},
    {"DeleteChar", DeleteChar},
    {"RightChar", RightChar},
    {"LeftChar", LeftChar},
    {"EndOfLine", EndOfLine},
    {"BeginOfLine", BeginOfLine},
    {"StartSelect", StartSelect},
    {"ExtendSelect", ExtendSelect},
    {"EndSelect", EndSelect},
    {"InsertSelect", InsertSelect},
    {"DeleteToEOL", DeleteToEOL},
    {"DeleteWord", DeleteWord},
};

/* Default Translation table.  */
static char         defaultTranslations[] =
"<EnterWindow>:     ArmClue()        \n\
 <LeaveWindow>:     DisArmClue()      \n\
 <Key>Right:    RightChar()\n\
 <Key>Left:     LeftChar()\n\
 <Key>Delete:   DeleteChar()\n\
 <Key>BackSpace:        DeleteChar()\n\
 <Key>Return:   Notify()\n\
 Ctrl<Key>A:  BeginOfLine()\n\
 Ctrl<Key>F:  RightChar()\n\
 Ctrl<Key>B:  LeftChar()\n\
 Ctrl<Key>U:  BeginOfLine() DeleteToEOL()\n\
 Ctrl<Key>E:  EndOfLine()\n\
 Ctrl<Key>W:  DeleteWord()\n\
 Ctrl<Key>K:  DeleteToEOL()\n\
<Key>Home:      BeginOfLine() \n\
:<Key>KP_Home:  BeginOfLine() \n\
<Key>End:       EndOfLine() \n\
:<Key>KP_End:   EndOfLine() \n\
 <Key>:         InsertChar()\n\
 Shift<Btn1Down>:     \n\
 <Btn1Down>:    StartSelect()\n\
 <Btn1Motion>:  ExtendSelect()\n\
 <Btn1Up>:      EndSelect()\n\
 <Btn2Down>:    InsertSelect()\n";

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

#define offset(field) XtOffset(SimpleTextWidget, text.field)

static XtResource   resources[] =
{
    {XtNscrollOnTop, XtCScrollOnTop, XtRBoolean, sizeof(Boolean),
     offset(v_scroll_ontop), XtRImmediate, (XtPointer) FALSE},
    {XtNscrollOnLeft, XtCScrollOnLeft, XtRBoolean, sizeof(Boolean),
     offset(h_scroll_onleft), XtRImmediate, (XtPointer) FALSE},
    {XtNvScroll, XtCVScroll, XtRBoolean, sizeof(Boolean),
     offset(use_v_scroll), XtRImmediate, (XtPointer) FALSE},
    {XtNhScroll, XtCHScroll, XtRBoolean, sizeof(Boolean),
     offset(use_h_scroll), XtRImmediate, (XtPointer) FALSE},
    {XtNscrollBorder, XtCScrollBorder, XtRInt, sizeof(int),
     offset(scrollborder), XtRImmediate, (XtPointer) 4},
    {XtNinternational, XtCInternational, XtRBoolean, sizeof(Boolean),
     offset(international), XtRImmediate, (XtPointer) FALSE},
    {XtNcforeground, XtCForeground, XtRPixel, sizeof(Pixel),
     offset(cforeground), XtRString, XtDefaultForeground},
    {XtNtopColor, XtCTopColor, XtRPixel, sizeof(Pixel),
     offset(topcolor), XtRString, XtDefaultBackground},
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
     offset(foreground), XtRString, XtDefaultForeground},
    {XtNfont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
     offset(font), XtRString, XtDefaultFont},
    {XtNfont1, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
     offset(font[1]), XtRString, XtDefaultFont},
    {XtNfont2, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
     offset(font[2]), XtRString, XtDefaultFont},
    {XtNfont3, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
     offset(font[3]), XtRString, XtDefaultFont},
    {XtNfontSet, XtCFontSet, XtRFontSet, sizeof(XFontSet),
     offset(fontset), XtRString, XtDefaultFontSet},
    {XtNfontSet1, XtCFontSet, XtRFontSet, sizeof(XFontSet),
     offset(fontset[1]), XtRString, XtDefaultFontSet},
    {XtNfontSet2, XtCFontSet, XtRFontSet, sizeof(XFontSet),
     offset(fontset[2]), XtRString, XtDefaultFontSet},
    {XtNfontSet3, XtCFontSet, XtRFontSet, sizeof(XFontSet),
     offset(fontset[3]), XtRString, XtDefaultFontSet},
    {XtNgapSize, XtCGapSize, XtRInt, sizeof(int),
     offset(gapsize), XtRImmediate, (XtPointer) 32},
    {XtNeditable, XtCEditable, XtRBoolean, sizeof(Boolean),
     offset(editable), XtRImmediate, (XtPointer) TRUE},
    {XtNcallback, XtCCallback, XtRCallback, sizeof(XtPointer),
     offset(callbacks), XtRCallback, (XtPointer) NULL},
    /* ThreeD resouces */
    threeDresources
    {XtNclue, XtCLabel, XtRString, sizeof(String),
     offset(clue), XtRImmediate, (XtPointer) NULL}
};

#define superclass (&compositeClassRec)

SimpleTextClassRec    simpleTextClassRec =
{
    { /* core_class fields */
    (WidgetClass) superclass,	/* superclass               */
    "SimpleText",		/* class_name               */
    sizeof(SimpleTextRec),	/* widget_size              */
    XpwInitializeWidgetSet,	/* class_initialize         */
    NULL,			/* class_part_initialize    */
    FALSE,			/* class_inited             */
    Initialize,			/* initialize               */
    NULL,			/* initialize_hook          */
    XtInheritRealize,		/* realize                  */
    actions,			/* actions                  */
    XtNumber(actions),		/* num_actions              */
    resources,			/* resources                */
    XtNumber(resources),	/* num_resources            */
    NULLQUARK,			/* xrm_class                */
    TRUE,			/* compress_motion          */
    FALSE,			/* compress_exposure        */
    TRUE,			/* compress_enterleave      */
    FALSE,			/* visible_interest         */
    Destroy,			/* destroy                  */
    Resize,			/* resize                   */
    Redisplay,			/* expose                   */
    SetValues,			/* set_values               */
    NULL,			/* set_values_hook          */
    XtInheritSetValuesAlmost,	/* set_values_almost        */
    NULL,			/* get_values_hook          */
    NULL,			/* accept_focus             */
    XtVersion,			/* version                  */
    NULL,			/* callback_private         */
    defaultTranslations,	/* tm_table                 */
    QueryGeometry,		/* query_geometry           */
    },
    {   /* composite part */
        XtInheritGeometryManager,       /* geometry_manager   */
        XtInheritChangeManaged,         /* change_managed     */
        XtInheritInsertChild,           /* insert_child       */
        XtInheritDeleteChild,           /* delete_child       */
        NULL                            /* extension          */
    },
    { /* SimpleText class fields initialization */
    0		/* not used                 */
    },
};

WidgetClass         simpleTextWidgetClass = (WidgetClass) & simpleTextClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

/* 
 * Initialize widget.
 */

/* ARGSUSED */
static void
Initialize(junk, new, args, num_args)
	Widget              junk, new;
	ArgList             args;
	Cardinal           *num_args;
{
    SimpleTextWidget      nself = (SimpleTextWidget) new;
    int                 font_ascent = 0, font_descent = 0, x_loc, y_loc;
    XFontSetExtents    *ext = XExtentsOfFontSet(nself->text.fontset[0]);
    Dimension           width, height;
    String              str;
    int                 len;

   /* Build a defualt buffer */
    nself->text.stringsize = strlen(XtName(new));
    if (nself->text.gapsize == 0)
	nself->text.gapsize = INSERTBUFSIZ;
    nself->text.cgap_size = nself->text.gapsize;
    nself->text.textbuffer = XtMalloc(nself->text.stringsize + nself->text.gapsize+1);
    nself->text.insert_point = 0;
    nself->text.textstart = 0;
    nself->text.highstart = 0;
    nself->text.highend = 0;
    strcpy(&nself->text.textbuffer[nself->text.cgap_size], XtName(new));
    nself->text.larray = NULL;
    nself->text.xoff = 0;
    nself->text.yoff = 0;
    nself->text.v_scrollbar = NULL;
    nself->text.h_scrollbar = NULL;
    BuildLineTable(nself);

    CreateGCs(new);
    _XpwThreeDInit(new, &nself->text.threeD, nself->core.background_pixel);
    width = 0;
    height = 0;

    Layout(nself, &width, &height, FALSE);
    width += 2 * nself->text.threeD.shadow_width;
    height += 2 * nself->text.threeD.shadow_width;

    if (nself->core.width == 0)
        nself->core.width = width;
    if (nself->core.height == 0)
        nself->core.height = height;
}

/*
 * Set specified arguments into widget
 */

static              Boolean
SetValues(current, request, new, args, num_args)
	Widget              current, request, new;
	ArgList             args;
	Cardinal           *num_args;
{
    Boolean             ret_val = FALSE;
    SimpleTextWidget      self = (SimpleTextWidget) new;
    SimpleTextWidget      old_self = (SimpleTextWidget) current;

    if (((old_self->text.font != self->text.font) &&
	 (old_self->text.international == False)) ||
	(old_self->text.cforeground != self->text.cforeground) ||
	(old_self->text.foreground != self->text.foreground)) {
	DestroyGCs(current);
	CreateGCs(new);
	ret_val = TRUE;
    }
    if ((old_self->text.fontset != self->text.fontset) &&
	(old_self->text.international == True))
       /* DONT changes the GCs, because the fontset is not in them. */
	ret_val = TRUE;

    if (_XpmThreeDSetValues(new, &old_self->text.threeD,
                    &self->text.threeD, new->core.background_pixel)) 
        ret_val = TRUE;

    return (ret_val);
}

/*
 * This tells the parent what size we would like to be
 * given certain constraints.
 */

static              XtGeometryResult
QueryGeometry(w, intended, requested)
	Widget              w;
	XtWidgetGeometry   *intended, *requested;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    Dimension           width, height;
    XtGeometryResult    ret_val = XtGeometryYes;
    XtGeometryMask      mode = intended->request_mode;
    String              str;
    int                 len;

    width = self->text.mwidth  + 2 * self->text.threeD.shadow_width;
    height = self->text.mheight + 2 * self->text.threeD.shadow_width;

    if (((mode & CWWidth) && (intended->width != width)) ||
	!(mode & CWWidth)) {
	requested->request_mode |= CWWidth;
	requested->width = width;
	ret_val = XtGeometryAlmost;
    }
    if (((mode & CWHeight) && (intended->height != height)) ||
	!(mode & CWHeight)) {
	requested->request_mode |= CWHeight;
	requested->height = height;
	ret_val = XtGeometryAlmost;
    }
    if (ret_val == XtGeometryAlmost) {
	mode = requested->request_mode;

	if (((mode & CWWidth) && (width == self->core.width)) &&
	    ((mode & CWHeight) && (height == self->core.height)))
	    return (XtGeometryNo);
    }
    return (ret_val);
}


/*
 * Resize request.
 */

static void
Resize(w)
	Widget              w;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    Dimension           width, height;

    width = self->core.width;
    height = self->core.height;
    Layout(self, &width, &height, TRUE);
    Redisplay(w, NULL, NULL);
}

#define abs(x)	(((x)<0)?-(x):(x))
/* Redisplay()
 *
 * Repaints the widget window on expose events.
 */

/* ARGSUSED */
static void
Redisplay(wid, event, region)
	Widget              wid;
	XEvent             *event;
	Region              region;
{
    SimpleTextWidget      self = (SimpleTextWidget) wid;
    GC                  gc, rgc, dgc, arrow;
    int                 font_ascent = 0, font_descent = 0, x_loc, y_loc;
    XFontSetExtents    *ext = XExtentsOfFontSet(self->text.fontset[0]);
    Display            *dpy = XtDisplayOfObject(wid);
    Window              win = XtWindowOfObject(wid);
    Dimension           s = self->text.threeD.shadow_width;
    Dimension           h = self->core.height;
    Dimension           w = self->core.width;
    int                 width, t_width;
    int                 tstart;
    int                 hstart;
    int                 hend, cnt;
    XRectangle          cliparea;
    XGCValues           values;
    _XpwST_Line		*lp;

    if (!XtIsRealized(wid))
	return;

    cliparea.x = cliparea.y = s;
    if (self->text.v_scrollbar != NULL) {
        h -= self->text.v_scrollbar->core.height +
            self->text.scrollborder;
        if (self->text.v_scroll_ontop) {
            cliparea.y += self->text.v_scrollbar->core.height +
                self->text.scrollborder;
        }
    }
    if (self->text.h_scrollbar != NULL) {
        w -= self->text.h_scrollbar->core.width +
            self->text.scrollborder;
        if (self->text.h_scroll_onleft) {
            cliparea.x += self->text.h_scrollbar->core.width +
                self->text.scrollborder;
        }
    }
    cliparea.width = w;
    cliparea.height = h;

    if (region == NULL)
	XClearWindow(dpy, win);

    for(lp = self->text.larray, cnt = self->text.nlarray ; cnt > 0; cnt--, lp++) {
	if (lp->len == 0)
		continue;

	x_loc = lp->x_off + self->text.xoff;
	y_loc = lp->y_off + self->text.yoff;

	/* Check if outside window */
	if ((x_loc + lp->width) < 0 || x_loc > w)
		continue;
	if ((y_loc + lp->height) < 0 || y_loc > h)
		continue;
	/* Now see if this we cleared */
	if (region != NULL && XRectInRegion(region, x_loc, y_loc, lp->width,
				lp->height) == RectangleOut)
		continue;

	/* Need to draw it */
         if (self->core.sensitive) 
             gc = (lp->rev) ? self->text.rev_gc : self->text.norm_gc;
         else 
             gc = (lp->rev) ? self->text.rev_gray_gc : self->text.norm_gray_gc;
 
	 XSetClipRectangles(dpy, gc, 0, 0, &cliparea, 1, YXBanded);

	 if (lp->rev) {
		GC	xgc = (self->core.sensitive)?self->text.norm_gc:
				self->text.norm_gray_gc;
		
	 XSetClipRectangles(dpy, xgc, 0, 0, &cliparea, 1, YXBanded);
	XFillRectangle(dpy, win, xgc, x_loc, y_loc - lp->height, lp->width, lp->height);
	} else {
                int                 x = x_loc;
                int                 y = y_loc - lp->height;
                int                 w = lp->width;
                int                 h = lp->height;

               /* Need to do our own cliping */
                if (x < cliparea.x) {
                    w -= cliparea.x - x;
                    x = cliparea.x;
                }
                if ((x + w) > cliparea.width)
                    w -= (x + w) - cliparea.width;

                if (y < cliparea.y) {
                    h -= cliparea.y - y;
                    y = cliparea.y;
                }
                if ((y + h) > cliparea.height)
                    h -= (y + h) - cliparea.height;
               /* Now clear it */
                XClearArea(dpy, win, x, y, w, h, FALSE);
	    }
           /* Draw the string */
            if (self->text.international == True)
                XmbDrawString(dpy, win, self->text.fontset[lp->font], gc,
                    x_loc, y_loc + lp->height, lp->str, lp->len);
            else {
                values.font = self->text.font[lp->font]->fid;
                XChangeGC(dpy, gc, GCFont, &values);
                XDrawString(dpy, win, gc, x_loc, y_loc + lp->height, lp->str, lp->len);
            }

 	    if (lp->under) 
		XDrawLine(dpy, win, gc, x_loc, y_loc, x_loc + lp->width, y_loc);
	}

   /* Draw shadows around main window */
    x_loc = y_loc = 0;
    h = self->core.height;
    w = self->core.width;
    if (self->text.v_scrollbar != NULL) {
        int                 sh = self->text.v_scrollbar->core.height
        + self->text.scrollborder;

        if (self->text.v_scroll_ontop) {
            XFillRectangle(dpy, win, self->text.top_gc, 0, 0, w, sh);
            y_loc += sh;
        } else {
            XFillRectangle(dpy, win, self->text.top_gc, 0, h - sh, w, sh);
        }
    }
    if (self->text.h_scrollbar != NULL) {
        int                 sw = self->text.h_scrollbar->core.width
        + self->text.scrollborder;

        if (self->text.h_scroll_onleft) {
            XFillRectangle(dpy, win, self->text.top_gc, 0, 0, sw, h);
            x_loc += sw;
        } else {
            XFillRectangle(dpy, win, self->text.top_gc, w - sw, 0, sw, h);
        }
        w -= sw;
    }
    if (self->text.v_scrollbar != NULL) {
        h -= self->text.v_scrollbar->core.height + self->text.scrollborder;
    }
 
   /* Draw shadows around main window */
    _XpwThreeDDrawShadow(wid, event, region, &(self->text.threeD), x_loc, x_loc,
			 w, h, TRUE);

}

static void
Destroy(w)
        Widget              w;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;

    DestroyGCs(w);
    XtFree(self->text.textbuffer);
    XtFree(self->text.select_text);
	XtFree(self->text.larray);
    _XpwThreeDDestroyShadow(w, &(self->text.threeD));
}

/************************************************************
 *
 * Private Functions.
 *
 ************************************************************/

/*
 * Build the line table for the string.
 */
static void
BuildLineTable(self)
        SimpleTextWidget   self;
{
	int	ip = self->text.insert_point;
	int	len, olen, rowhieght, maxwidth;
	int	count = 2;
	_XpwST_Line	*lp;
	int	attr = 0;
	int	x, y;
	int	adjust = 0;
	char    *str;

	/* Scan strings looking for break charactures and count */
	str = self->text.textbuffer;
	for (len = 0; len < self->text.stringsize; len++) {
		if (len == ip)
			str += self->text.gapsize;
		if (*str == '\n' || *str == '\t' || *str == '\033')
			count++;
		str++;
	}

 	/* Allocate line break arrays */
	self->text.nlarray = count;
	self->text.larray = XtRealloc(self->text.larray, count * sizeof(_XpwST_Line));
	lp = self->text.larray;
	/* Rescan strings this time building arrays */
	str = self->text.textbuffer;
	lp->str = str;
	lp->off = str - self->text.textbuffer;
	olen = 0;
	rowhieght = self->text.font[0];
	maxwidth = 0;
	x = 0;
	y = 0;
	lp->x_off = x;
	lp->y_off = y;
	count = 0;
	for(len = 0; len < self->text.stringsize; len++) {
		if (len != ip && *str != '\n' && *str != '\t' && *str != '\033') {
			str++;
			olen++;
			continue;
		}

		/* Special and we have some chars, collect them */
	/* We also break at insertpoint to make insertion/deletetion easier */
		if ((lp->len = olen) != 0 || len == ip) {
			lp->font = attr&0x3;
			lp->height = TextHigh(self, lp->font);
			lp->width = TextLen(self, lp->str, olen, lp->font);
			lp->under = (attr&0x4)?1:0;
			lp->rev = (attr&0x8)?1:0;
			lp->link = (attr&0x10)?1:0;
			x += lp->width;
			if (lp->height > rowhieght)
				rowhieght = lp->height;
			lp++;
			count ++;
			lp->x_off = x;
			lp->y_off = y;
			if (len == ip)  {
				str += self->text.gapsize;
				self->text.attr = attr;
			}
			lp->str = str;
			lp->off = str - self->text.textbuffer;
			olen = 0;
		}
			

		switch(*str++) {
		case '\n':
			if (maxwidth > x)
				maxwidth = x;
			x = 0;
			y += rowhieght;
			rowhieght = self->text.font[0];
			break;

		case '\t':
			x = (1 + (x/self->text.tablen)) * self->text.tablen; 
			break;
		case '\033':
			len++;
			switch (*str++) {
			case 'u': attr &= ~0x4; break;
			case 'U': attr |= 0x4; break;
			case 'l': attr &= ~0x10; break;
			case 'L': attr |= 0x10; break;
			case 'r': attr &= ~0x8; break;
			case 'R': attr |= 0x8; break;
			case 'n':
			case 'N': attr &= ~0x3; break;
			case 'b':
			case 'B': attr &= ~0x3; attr |= 1; break;
			case 'i':
			case 'I': attr &= ~0x3; attr |= 2; break;
			case 'e':
			case 'E': attr |= 0x3; break;
			case 'Z':
			case 'z': attr = 0; break;
			}
			break;
		}
	}
	lp->len = ++olen;
	lp->font = attr&0x3;
	lp->height = TextHigh(self, lp->font);
	lp->width = TextLen(self, lp->str, olen, lp->font);
	lp->font = attr&0x3;
	lp->under = (attr&0x4)?1:0;
	lp->rev = (attr&0x8)?1:0;
	lp->link = (attr&0x10)?1:0;
	x += lp->width;
	if (lp->height > rowhieght)
		rowhieght = lp->height;
	count++;
	if (x > 0)
		y += rowhieght;
	self->text.mheight = y;
	self->text.mwidth = maxwidth;
 	/* Allocate line break arrays */
	self->text.nlarray = count;
	self->text.larray = XtRealloc(self->text.larray, count * sizeof(_XpwST_Line));
}

/*
 * Layout, return desired width and height. If set is TRUE, set rows and
 * columns and make scrollbars as needed.
 *
 */

static void
Layout(self, width, height, set)
        SimpleTextWidget    self;
        Dimension          *width;
        Dimension          *height;
        int                 set;
{
    int                 rows = 0;
    int                 cols = 0;
    Dimension           w = *width;
    Dimension           h = *height;
    int                 l;
    int                 rowh;
    Boolean             needwscroll = self->text.use_h_scroll;
    Boolean             needhscroll = self->text.use_v_scroll;

   /* Grab current settings if doing prefered size */
    if (w == 0 && self->core.width != 0)
        w = self->core.width - 2 * self->text.threeD.shadow_width;
    if (h == 0 && self->core.height != 0)
        h = self->core.height - 2 * self->text.threeD.shadow_width;

   /* If were doing it for real then make the scroll bars if needed */
    if (set) {
        int                 sh, sw;
        int                 th, tw;
        int                 lsize, tsize;

      redo:
        if (needhscroll) {
            if (self->text.h_scrollbar == NULL) {
                self->text.h_scrollbar = XtVaCreateManagedWidget(
                           "hscroll", scrollerWidgetClass, (Widget) self,
                                 XtNorientation, XtorientVertical, NULL);

                XtAddCallback(self->text.h_scrollbar,
                              XtNcallback, HandleScroll, self);
                XtRealizeWidget(self->text.h_scrollbar);
            }
            lsize = self->text.h_scrollbar->core.width;
        } else {
            if (self->text.h_scrollbar != NULL) {
                XtDestroyWidget(self->text.h_scrollbar);
                self->text.h_scrollbar = NULL;
            }
            lsize = 0;
        }
        if (needwscroll) {
            if (self->text.v_scrollbar == NULL) {
                self->text.v_scrollbar = XtVaCreateManagedWidget(
                           "vscroll", scrollerWidgetClass, (Widget) self,
                               XtNorientation, XtorientHorizontal, NULL);
                XtAddCallback(self->text.v_scrollbar,
                              XtNcallback, HandleScroll, self);
                XtRealizeWidget(self->text.v_scrollbar);
            }
            tsize = self->text.v_scrollbar->core.height;
        } else {
            if (self->text.v_scrollbar != NULL) {
                XtDestroyWidget(self->text.v_scrollbar);
                self->text.v_scrollbar = NULL;
            }
            tsize = 0;
        }
       /* See if we overflowed the other direction */
        if ((w - lsize) < (l * cols) && self->text.v_scrollbar == NULL) {
            needwscroll = TRUE;
            goto redo;
        }
        if ((h - tsize) < (rowh * rows) && self->text.h_scrollbar == NULL) {
            needhscroll = TRUE;
            goto redo;
        }
        if (self->text.h_scrollbar != NULL) {

           /* Adjust sizes of bars */
            sw = lsize;
            sh = h - tsize;
            XtConfigureWidget(self->text.h_scrollbar,
                              ((self->text.h_scroll_onleft) ? 0 : w - sw),
                              ((self->text.v_scroll_ontop) ? tsize : 0),
                              lsize, (h - tsize),
                              self->text.h_scrollbar->core.border_width);
           /* Total height */
            th = rowh * rows;
           /* Remove undisplayable parts */
            sh -= self->text.threeD.shadow_width * 2;
            if (self->text.v_scrollbar != NULL)
                sh -= self->text.scrollborder;
           /* Set it */
            XpwScrollerSetThumb(self->text.h_scrollbar, th, sh);
        }
        if (self->text.v_scrollbar != NULL) {

           /* Adjust sizes of bars */
            sw = w - lsize;
            sh = tsize;
            XtConfigureWidget(self->text.v_scrollbar,
                              ((self->text.h_scroll_onleft) ? lsize : 0),
                              ((self->text.v_scroll_ontop) ? 0 : h - sh),
                              (w - lsize), tsize,
                              self->text.v_scrollbar->core.border_width);
           /* Total Width */
            tw = l * cols;
           /* Remove undisplayable parts */
            sw -= self->text.threeD.shadow_width * 2;
            if (self->text.h_scrollbar != NULL)
                sw -= self->text.scrollborder;
           /* Set it */
            XpwScrollerSetThumb(self->text.v_scrollbar, tw, sw);
        }
    } else {
       /* Return desired sizes */
        *width = w;
        *height = h;
    }
}

/*
 * Handle scrollbar motion.
 */
static void
HandleScroll(wid, client_data, call_data)
        Widget              wid;
        XtPointer           client_data;
        XtPointer           call_data;
{
    SimpleTextWidget          self = (SimpleTextWidget) client_data;
    int                 place = (int) call_data;
    int                 nxoff = self->text.xoff;
    int                 nyoff = self->text.yoff;

   /* Now do the scroll */
    if (wid == self->text.h_scrollbar)
        nyoff = place;
    if (wid == self->text.v_scrollbar)
        nxoff = place;
   /* Move the window only if there was a change. */
    if (nxoff != self->text.xoff || nyoff != self->text.yoff)
        MoveWindow((Widget) self, nxoff, nyoff);
}
/*
 * Handle scrollbar motion.
 */
static void
MoveWindow(wid, nxoff, nyoff)
        Widget              wid;
        int                 nxoff;
        int                 nyoff;
{
    SimpleTextWidget          self = (SimpleTextWidget) wid;
    int                 coff;
    Display            *dpy = XtDisplayOfObject((Widget) self);
    Window              win = XtWindowOfObject((Widget) self);
    Dimension           s = self->text.threeD.shadow_width;
    Dimension           h = self->core.height - 2 * s;
    Dimension           w = self->core.width - 2 * s;
    int                 dist;
    int                 x, y;
    XRectangle          cliparea;
    GC                  gc;
    Boolean             need_refresh = FALSE;

   /* Build a clip mask */
    cliparea.x = cliparea.y = s;
    x = y = s;
    if (self->text.v_scrollbar != NULL) {
        int                 sh;

        sh = self->text.v_scrollbar->core.height + self->text.scrollborder;
        if (self->text.v_scroll_ontop) {
            cliparea.y += sh;
            y += sh;
        }
        h -= sh;
    }
    if (self->text.h_scrollbar != NULL) {
        int                 sw;

        sw = self->text.h_scrollbar->core.width + self->text.scrollborder;
        if (self->text.h_scroll_onleft) {
            cliparea.x += sw;
            x += sw;
        }
        w -= sw;
    }
    cliparea.width = w;
    cliparea.height = h;

   /* Plane old GC */
    gc = (self->core.sensitive) ? self->text.norm_gc : self->text.norm_gray_gc;

   /* Save Clip Mask */
    XSetClipRectangles(dpy, gc, 0, 0, &cliparea, 1, YXBanded);

   /* Now do the scroll */
    coff = self->text.yoff;
    dist = abs(nyoff - coff);
   /* Check if copy will be any good */
    if (dist > h) {
       /* Nope, just clear whole area */
        XClearWindow(dpy, win);
        need_refresh = TRUE;
    } else {
        if (coff < nyoff)
            XCopyArea(dpy, win, win, gc, x, dist + y, w, h, x, y);
        else
            XCopyArea(dpy, win, win, gc, x, y, w, h, x, dist + y);
        XClearArea(dpy, win, x, (coff < nyoff) ? (h - dist) : x, w, dist, TRUE);
    }
    self->text.yoff = nyoff;
    coff = self->text.xoff;
    dist = abs(nxoff - coff);
   /* Check if copy will be any good */
    if (dist > w) {
       /* Nope, just clear whole area */
        XClearWindow(dpy, win);
        need_refresh = TRUE;
    } else {
        if (coff < nxoff)
            XCopyArea(dpy, win, win, gc, dist + x, y, w, h, x, y);
        else
            XCopyArea(dpy, win, win, gc, x, y, w, h, dist + x, y);
        XClearArea(dpy, win, (coff < nxoff) ? (w - dist) : x, y, dist, h, TRUE);
    }
    self->text.xoff = nxoff;
    if (need_refresh)
        Redisplay((Widget) self, NULL, NULL);
}

/*
 * Adjust cursor to center of window if it drifted out of window.
 */
static int
ReCenterCursor(w)
	Widget              w;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    Dimension           s = self->text.threeD.shadow_width;
    Dimension           h = self->core.height;
    Dimension           wd = self->core.width;
    Dimension           lside, rside;
    int                 nc = 0;
    char               *str = self->text.textbuffer;
    int                 i;
    int                 t_width = 0;
    int                 redraw = FALSE;


    fprintf(stderr, "ReCenterCursor(%s) %d [%d:%d]\n", XtName(w), i, lside, rside);
   /* Walk backwords from insert point till we have at least nc chars */
    if (nc != 0) {
	for (i = self->text.insert_point - 1; i >= 0; i--) {
	    t_width += TextLen(self, &str[i], 1, 0);
	    if (t_width >= nc)
		break;
	}
       /* readjust all pointers now */
	if (self->text.textstart != i)
	    redraw = TRUE;
	self->text.cursor_x = t_width;
	self->text.textstart = i;
	fprintf(stderr, "New cursor at %d offset %d [%d] ", nc, t_width, i);
	if (i > 0) {
	    t_width = TextLen(self, str, i, 0);
	} else
	    t_width = 0;
	self->text.coffset = t_width;
	fprintf(stderr, " offset is %d\n", t_width);
    } else {
	t_width = self->text.coffset;
    }
    if (redraw)
	Redisplay(w, NULL, NULL);
    return redraw;
}

/* AdjustPoint
 *
 * Adjust the insert pointer and gap.
 */
static void
AdjustPoint(w, newpoint)
	Widget              w;
	int                 newpoint;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    int                 ip = self->text.insert_point;
    int                 gap = self->text.cgap_size;

    if (newpoint < 0)
	newpoint = 0;

    if (newpoint > self->text.stringsize)
	newpoint = self->text.stringsize;

    while (ip > newpoint) {
	ip--;
	self->text.textbuffer[ip + gap] = self->text.textbuffer[ip];
    }
    while (ip < newpoint) {
	self->text.textbuffer[ip] = self->text.textbuffer[ip + gap];
	ip++;
    }
    self->text.insert_point = newpoint;
}

/* GrowBuffer
 *
 * Grows the buffer if there is not enough room for num chars.
 */
static void
GrowBuffer(w, num)
	Widget              w;
	int                 num;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    int                 ip = self->text.insert_point;
    int                 sz = self->text.stringsize;
    int                 gap = self->text.cgap_size;
    int                 ngap, n;
    char               *p, *q;

    if (gap > num)
	return;
    ngap = gap + num;
   /* Don't ever grow the gap by less than gapsize */
    if (ngap < self->text.gapsize)
	ngap = self->text.gapsize;
    self->text.textbuffer = XtRealloc(self->text.textbuffer,
				      self->text.stringsize + ngap);
   /* Compute old and new end. */
    p = &self->text.textbuffer[sz + gap];
    q = &self->text.textbuffer[sz + ngap];
    self->text.cgap_size = ngap;
   /* Copy backwards */
    for (n = self->text.stringsize - ip; n > 0; n--)
	*q-- = *p--;
}

/*
 * Erase the cursor.
 */
static void
EraseCursor(w)
	Widget              w;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    GC                  gc;
    Display            *dpy = XtDisplayOfObject(w);
    Window              win = XtWindowOfObject(w);
    Dimension           s = self->text.threeD.shadow_width;
    Dimension           h = self->core.height;
    int                 pos;

    fprintf(stderr, "EraseCursor(%s) at %d\n", XtName(w), self->text.cursor_x);
   /* Set the context based on sensitivity setting */
    if (XtIsSensitive(w) && XtIsSensitive(XtParent(w))
	&& self->text.cursor_x >= 0) {
	gc = self->text.erase_gc;
	pos = self->text.left_margin + s + self->text.cursor_x;
#if 0
	if (self->text.left_arrow)
	    pos += h;
#endif
	XDrawLine(dpy, win, gc, pos, s, pos, h - s - 1);
    }
}

/* DrawCursor
 *
 * Draws the cursor... Adjusts window if cursor would not be shown on the
 * screen.
 */
static void
DrawCursor(w)
	Widget              w;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    GC                  gc;
    Display            *dpy = XtDisplayOfObject(w);
    Window              win = XtWindowOfObject(w);
    Dimension           s = self->text.threeD.shadow_width;
    Dimension           h = self->core.height;
    int                 pos;

    fprintf(stderr, "DrawCursor(%s) at %d", XtName(w), self->text.cursor_x);
   /* Set the context based on sensitivity setting */
    if (XtIsSensitive(w) && XtIsSensitive(XtParent(w))
	&& self->text.cursor_x >= 0) {
	gc = self->text.cursor_gc;
	pos = self->text.left_margin + s + self->text.cursor_x;
#if 0
	if (self->text.left_arrow)
	    pos += h;
#endif
	fprintf(stderr, " real %d", pos);
	XDrawLine(dpy, win, gc, pos, s, pos, h - s - 1);
    }
    fprintf(stderr, "\n");
}

/*
 * DeleteHighlight.
 *
 * Removed highlighted text from buffer.
 */
static int
DeleteHighlight(w)
	Widget              w;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    int                 len;
    char               *str;

    fprintf(stderr, "DeleteHighlight(%s)\n", XtName(w));

    if (self->text.highstart == self->text.highend)
	return FALSE;
    len = self->text.highend - self->text.highstart;
    if (self->text.highstart == self->text.insert_point) {
	fprintf(stderr, "After %d bytes\n", len);
	str = &self->text.textbuffer[self->text.insert_point +
				     self->text.cgap_size];
	self->text.cgap_size += len;
	self->text.stringsize -= len;
    } else if (self->text.highend == self->text.insert_point) {
	int                 t_width;

	fprintf(stderr, "Before %d bytes\n", len);

	str = &self->text.textbuffer[self->text.highstart];
	self->text.cursor_x -= TextLen(self, str, len, 0);
	self->text.insert_point -= len;
	self->text.cgap_size += len;
	self->text.stringsize -= len;
    } else {
	fprintf(stderr, "DeleteHighlight - unable to clear highlight\n");
    }
    self->text.highend = self->text.highstart = 0;
    return TRUE;
}

/* ClearHighlight
 *
 * Clears the selected text.
 */
static void
ClearHighlight(w)
	Widget              w;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;

    self->text.highstart = self->text.highend = -1;
}

/* OffsetToChar()
 *
 * Translate a offset to a character position.
 */
static int
OffsetToChar(w, offset)
	Widget              w;
	int                 offset;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    int                 ip = self->text.insert_point;
    String              str;
    Dimension           s = self->text.threeD.shadow_width;
    int                 len, dist;
    int                 cp;
    int                 t_width;
    int                 c_width;
    int                 i;

   /* Adjust offset for margin and shadow */
    offset -= self->text.left_margin + s;
#if 0
    if (self->text.left_arrow)
	offset -= self->core.height;
#endif
    offset += self->text.coffset;
   /* Check if in range */
    if (offset < 0)
	return 0;
    cp = self->text.cursor_x + self->text.coffset;
   /* Short cut... at insert point */
    if (offset == cp)
	return ip;
    if (offset > cp) {
	offset -= cp;
	str = &self->text.textbuffer[ip + self->text.cgap_size];
	len = self->text.stringsize - ip;
    } else {
	len = ip;
	ip = 0;
	str = self->text.textbuffer;
    }
   /* Compute lenght of full string */
    t_width = TextLen(self, str, len, 0);
   /* Check if after end of string */
    if (offset > t_width)
	return ip + len;
    for (dist = i = 0; i < len; i++) {
	t_width = TextLen(self, &str[i], 1, 0);
	if (offset >= dist && offset < (dist + t_width)) {
	    if (i < len && offset >= (dist + (t_width / 2)))
		i++;
	    break;
	}
	dist += t_width;
    }
    return ip + i;
}

/* CharToOffset()
 *
 * Translate a character position to a offset.
 */
static int
CharToOffset(w, pos)
	Widget              w;
	int                 pos;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    int                 ip = self->text.insert_point;
    String              str = self->text.textbuffer;
    int                 len;
    int                 t_width;

    if (pos < 0)
	pos = 0;
    if (pos > self->text.stringsize)
	pos = self->text.stringsize + 1;
   /* Compute length of part before insert pointer */
    len = (pos < ip) ? pos : ip;
    t_width = TextLen(self, str, len, 0);
    if (pos > ip) {
	len = pos - ip;
	str = &self->text.textbuffer[ip + self->text.cgap_size];
	t_width += TextLen(self, str, len, 0);
    }
    return t_width;

}

/* ARGSUSED */
static              Boolean
ConvertSelection(w, selection, target, type, value, length, format)
	Widget              w;
	Atom               *selection;
	Atom               *target;
	Atom               *type;
	XtPointer          *value;
	unsigned long      *length;
	int                *format;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    XSelectionRequestEvent *req = XtGetSelectionRequest(w, *selection, NULL);

    if (*target == XA_TARGETS(XtDisplay(w))) {
	Atom               *targetP, *std_targets;
	unsigned long       std_length;

	XmuConvertStandardSelection(w, req->time, selection,
				target, type, (XPointer *) & std_targets,
				    &std_length, format);

	*value = XtMalloc((unsigned) sizeof(Atom) * (std_length + 1));
	targetP = *(Atom **) value;
	*length = std_length + 1;
	*targetP++ = XA_STRING;
	memcpy((char *) targetP, (char *) std_targets, sizeof(Atom) * std_length);
	XtFree((char *) std_targets);
	*type = XA_ATOM;
	*format = sizeof(Atom) * 8;
	return True;
    } else if (*target == XA_STRING) {
	*length = (long) self->text.select_size;
	*value = self->text.select_text;
	*type = XA_STRING;
	*format = 8;
	return True;
    }
    return False;
}

/* ARGSUSED */
static void
LoseSelection(w, selection)
	Widget              w;
	Atom               *selection;
{

    ClearHighlight(w);
    Redisplay(w, NULL, NULL);
}

/* ARGSUSED */
static void
RequestSelection(w, client, selection, type, value, length, format)
	Widget              w;
	XtPointer           client;
	Atom               *selection;
	Atom               *type;
	XtPointer           value;
	unsigned long      *length;
	int                *format;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    char               *np = (char *) value;
    int                 len = (int) *length;
    char               *p;

    if ((np != NULL) && (len != 0)) {

/* Remove any highlights */
	ClearHighlight(w);
/* Point cursor at correct point. */
	AdjustPoint(w, (int) client);
       /* Grow buffer if need be */
	GrowBuffer(w, len);
       /* Copy text into buffer. */
	p = &self->text.textbuffer[self->text.insert_point];
       /* Adjust offsets */
	self->text.highstart = self->text.insert_point;
	self->text.insert_point += len;
	self->text.highend = self->text.insert_point;
	self->text.cgap_size -= len;
	self->text.stringsize += len;
	while (len-- > 0) {
	    fprintf(stderr, "Inserting %c\n", *np);
	    *p++ = *np++;
	}
       /* Adjust cursor */
	self->text.cursor_x = CharToOffset(w,
			   self->text.insert_point) - self->text.coffset;
	if (ReCenterCursor(w) == FALSE)
	    Redisplay(w, NULL, NULL);
	DrawCursor(w);
    }
}

/* 
 * Creates all gc's for the simple menu widget.
 */

static void
CreateGCs(w)
	Widget              w;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    XGCValues           values;
    XtGCMask            mask, mask_i18n;

    values.foreground = self->text.foreground;
    values.background = self->core.background_pixel;
    values.font = self->text.font[0]->fid;
    values.graphics_exposures = FALSE;
    mask = GCForeground | GCBackground | GCGraphicsExposures | GCFont;
    mask_i18n = GCForeground | GCBackground | GCGraphicsExposures;

    self->text.norm_gc = XtAllocateGC(w, 0, mask_i18n, &values, GCFont, 0);

    values.foreground = self->core.background_pixel;
    values.background = self->text.foreground;
    values.font = self->text.font[0]->fid;
    values.graphics_exposures = FALSE;
    values.clip_mask = None;
    mask = GCForeground | GCBackground | GCGraphicsExposures | GCFont;
    mask_i18n = GCForeground | GCBackground | GCGraphicsExposures;

    if (self->text.international == True)
	self->text.rev_gc = XtAllocateGC(w, 0, mask_i18n, &values,
					 GCFont, 0);
    else
	self->text.rev_gc = XtGetGC(w, mask, &values);

    values.foreground = self->text.cforeground;
    values.background = self->core.background_pixel;
    values.graphics_exposures = FALSE;
    mask = GCForeground | GCBackground | GCGraphicsExposures;
    mask_i18n = GCForeground | GCBackground | GCGraphicsExposures;

    if (self->text.international == True)
	self->text.cursor_gc = XtAllocateGC(w, 0, mask_i18n, &values,
					    GCFont, 0);
    else
	self->text.cursor_gc = XtGetGC(w, mask, &values);

    values.foreground = self->core.background_pixel;
    values.background = self->text.foreground;
    values.font = self->text.font[0]->fid;
    values.graphics_exposures = FALSE;
    mask = GCForeground | GCBackground | GCGraphicsExposures | GCFont;
    mask_i18n = GCForeground | GCBackground | GCGraphicsExposures;

    if (self->text.international == True)
	self->text.erase_gc = XtAllocateGC(w, 0, mask_i18n, &values,
					   GCFont, 0);
    else
	self->text.erase_gc = XtGetGC(w, mask, &values);

    values.foreground = self->text.foreground;
    values.background = self->core.background_pixel;
    values.fill_style = FillTiled;
    values.tile = XmuCreateStippledPixmap(XtScreenOfObject(w),
					  self->text.foreground,
					  self->core.background_pixel,
					  self->core.depth);
    values.graphics_exposures = FALSE;
    mask |= GCTile | GCFillStyle;
    self->text.norm_gray_gc = XtAllocateGC(w, 0, mask_i18n, &values, GCFont, 0);
    values.background = self->text.foreground;
    values.foreground = self->core.background_pixel;
    values.fill_style = FillTiled;
    values.tile = XmuCreateStippledPixmap(XtScreenOfObject(w),
					  self->core.background_pixel,
					  self->text.foreground,
					  self->core.depth);
    values.graphics_exposures = FALSE;
    mask |= GCTile | GCFillStyle;
    if (self->text.international == True)
	self->text.rev_gray_gc = XtAllocateGC(w, 0, mask_i18n, &values,
					      GCFont, 0);
    else
	self->text.rev_gray_gc = XtGetGC(w, mask, &values);

}

/*  
 * Removes all gc's for the text widget.
 */

static void
DestroyGCs(w)
	Widget              w;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;

    XtReleaseGC(w, self->text.norm_gc);
    XtReleaseGC(w, self->text.norm_gray_gc);
    XtReleaseGC(w, self->text.rev_gc);
    XtReleaseGC(w, self->text.rev_gray_gc);
    XtReleaseGC(w, self->text.cursor_gc);
    XtReleaseGC(w, self->text.erase_gc);
}

/************************************************************
 *
 * ACTION procedures
 *
 ************************************************************/

/*
 * Arm the clue callback.
 */

/* ARGSUSED */
static void
ArmClue(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;

    _XpwArmClue(w, self->text.clue);
}

/*
 * DisArm the clue callback.
 */

/* ARGSUSED */
static void
DisArmClue(w, event, params, num_params)
        Widget              w;
        XEvent             *event;
        String             *params;
        Cardinal           *num_params;
{
    _XpwDisArmClue(w);
}

/* Notify() - ACTION
 *
 * Notifies the user that a button has been pressed, and
 * calls the callback.
 */

/* ARGSUSED */
static void
Notify(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    XpwSimpleTextReturnStruct ret;

    _XpwDisArmClue(w);
   /* Move to end of line */
    EndOfLine(w, event, params, num_params);
   /* Remove cursor */
    EraseCursor(w);
   /* Adjust buffer size to hold contents and default gap */
   /* Call back to process. */
    ret.reason = 0;
    ret.event = event;
    ret.string = XtMalloc(self->text.stringsize + 1);
    strncpy(ret.string, self->text.textbuffer, self->text.stringsize);
    ret.string[self->text.stringsize] = '\0';
	fprintf(stderr, "Notify(%s) = %s\n", XtName(w), ret.string);
    XtCallCallbackList(w, self->text.callbacks, (XtPointer) & ret);
   /* Free storage */
    XtFree(ret.string);
	fprintf(stderr, "Notify done\n", XtName(w));
}

/* InsertChar() - ACTION
 *
 * Insert a character into the buffer.
 */
/* ARGSUSED */
static void
InsertChar(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    int                 len, i;
    char                buf[INSERTBUFSIZ], *p;
    int                 ip = self->text.insert_point;
    int                 need_redraw = FALSE;
    int                 gap = self->text.cgap_size;
    int                 t_width;
    GC                  gc, rgc;
    Display            *dpy = XtDisplayOfObject(w);
    Window              win = XtWindowOfObject(w);

    _XpwDisArmClue(w);
   /* Check if editable */
    if (!self->text.editable)
	return;

    len = XLookupString((XKeyEvent *) event, buf, sizeof(buf), NULL, NULL);
    if (len > 0) {
       /* remove cursor */
	EraseCursor(w);
       /* If there is a selection, remove old chars first */
	DeleteHighlight(w);
       /* Compute size of new text */
	t_width = TextLen(self, buf, len, 0);
	self->text.cursor_x += t_width;
       /* Grow buffer if need be */
	GrowBuffer(w, len);
       /* Copy text into buffer. */
	p = &self->text.textbuffer[ip];
       /* Adjust offsets */
	self->text.insert_point += len;
	self->text.cgap_size -= len;
	self->text.stringsize += len;
	for (i = 0; len > 0; i++, len--) {
	    *p++ = buf[i];
	    fprintf(stderr, "Inserting %c\n", buf[i]);
	}
       /* Check if display needs to reposition */
	if (ReCenterCursor(w) == FALSE)
	    Redisplay(w, NULL, NULL);
	DrawCursor(w);
    }
}

/* DeleteChar() - ACTION
 *
 * Delete a character from the buffer.
 */
/* ARGSUSED */
static void
DeleteChar(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    int                 t_width;
    String              str;

    _XpwDisArmClue(w);
   /* Check if editable */
    if (!self->text.editable)
	return;

   /* Remove cursor */
    EraseCursor(w);
   /* If there is a selection, remove all chars from selected range */
    if (!DeleteHighlight(w)) {

       /* Adjust insert and gap size */
	if (self->text.insert_point >= 0) {
	   /* Compute size of new text */
	    self->text.insert_point--;
	    str = &self->text.textbuffer[self->text.insert_point];
	   /* Compute size of char to remove. */
	    self->text.cursor_x -= TextLen(self, str, 1, 0);
	    self->text.cgap_size++;
	    self->text.stringsize--;
	}
    }
   /* Draw cursor */
    if (ReCenterCursor(w) == FALSE)
	Redisplay(w, NULL, NULL);
    DrawCursor(w);
}

/* RightChar() - ACTION
 *
 * Move Right one char.
 */
/* ARGSUSED */
static void
RightChar(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    int                 t_width;
    String              str;

    fprintf(stderr, "RightChar(%s)\n", XtName(w));
    _XpwDisArmClue(w);
   /* Remove cursor */
    EraseCursor(w);
   /* If there is a selection, clear selection and mark for full redraw */
    ClearHighlight(w);
   /* Adjust insert pointer */
    if (self->text.insert_point < self->text.stringsize) {
	AdjustPoint(w, self->text.insert_point + 1);
	str = &self->text.textbuffer[self->text.insert_point - 1];
	self->text.cursor_x += TextLen(self, str, 1, 0);
    }
   /* Check if display needs to reposition */
   /* Draw cursor */
    if (ReCenterCursor(w) == FALSE)
	DrawCursor(w);
}

/* LeftChar() - ACTION
 *
 * Move left one char.
 */
/* ARGSUSED */
static void
LeftChar(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    int                 t_width;
    String              str;

    fprintf(stderr, "LeftChar(%s)\n", XtName(w));
    _XpwDisArmClue(w);
   /* Remove cursor */
    EraseCursor(w);
   /* If there is a selection, clear selection and mark for full redraw */
    ClearHighlight(w);
   /* Adjust insert pointer */
    if (self->text.insert_point > 0) {
	str = &self->text.textbuffer[self->text.insert_point];
	t_width = TextLen(self, str, 1, 0);
	AdjustPoint(w, self->text.insert_point - 1);
	self->text.cursor_x -= t_width;
    }
   /* Check if display needs to reposition */
   /* Draw cursor */
    if (ReCenterCursor(w) == FALSE)
	DrawCursor(w);
}

/* EndOfLine() - ACTION
 *
 * Move to end of the line.
 */
/* ARGSUSED */
static void
EndOfLine(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;

    _XpwDisArmClue(w);
   /* Remove cursor */
    EraseCursor(w);
   /* If there is a selection, clear selection and mark for full redraw */
    ClearHighlight(w);
   /* Adjust insert pointer */
    AdjustPoint(w, self->text.stringsize/* + 1*/);
   /* Check if display needs to reposition */
    self->text.cursor_x = CharToOffset(w, self->text.insert_point);
   /* Draw cursor */
    if (ReCenterCursor(w) == FALSE)
	DrawCursor(w);

}

/* BeginOfLine() - ACTION
 *
 * Move to begining of the line.
 */
/* ARGSUSED */
static void
BeginOfLine(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;

    _XpwDisArmClue(w);
   /* Remove cursor */
    EraseCursor(w);
   /* If there is a selection, clear selection and mark for full redraw */
    ClearHighlight(w);
   /* Adjust insert pointer */
    AdjustPoint(w, 0);
   /* Check if display needs to reposition */
    self->text.cursor_x = 0;
    self->text.coffset = 0;
   /* Draw cursor */
    if (ReCenterCursor(w) == FALSE)
	DrawCursor(w);

}

/* StartSelect() - ACTION
 *
 * Start a text selection.
 */
/* ARGSUSED */
static void
StartSelect(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    int                 nip;
    Boolean             needredraw = FALSE;

    _XpwDisArmClue(w);
    if (event == NULL)
	return;
    fprintf(stderr, "StartSelection(%s, %d)\n", XtName(w), event->xbutton.x);
   /* Remove cursor */
    EraseCursor(w);
   /* If there is a selection, clear selection and mark for full redraw */
    if (self->text.highstart != self->text.highend)
	needredraw = TRUE;
   /* Compute charater offset of mouse pointer */
    nip = OffsetToChar(w, event->xbutton.x);
    AdjustPoint(w, nip);
	fprintf(stderr, "Char at %d [%c]\n", nip, self->text.textbuffer[nip]);
    self->text.cursor_x = CharToOffset(w, nip) - self->text.coffset;
    self->text.highstart = self->text.highend = nip;
    self->text.select_start = self->text.select_end = nip;
    if (needredraw)
	Redisplay(w, NULL, NULL);
}

/* ExtendSelect() - ACTION
 *
 * Extend a selection of text.
 */
/* ARGSUSED */
static void
ExtendSelect(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    int                 nip;

    _XpwDisArmClue(w);
    if (event == NULL)
	return;
    fprintf(stderr, "ExtendSelection(%s, %d)\n", XtName(w), event->xbutton.x);
   /* Compute character offset of mouse pointer */
    nip = OffsetToChar(w, event->xbutton.x);
   /* If mouse has moved at less than one character, leave */
   /* Redisplay new selected characters */
    fprintf(stderr, "Cursor at %d\n", nip);
    if (self->text.select_end != nip) {
	self->text.select_end = nip;
	if (self->text.select_start < self->text.select_end) {
	    self->text.highstart = self->text.select_start;
	    self->text.highend = self->text.select_end;
	} else {
	    self->text.highstart = self->text.select_end;
	    self->text.highend = self->text.select_start;
	}
	AdjustPoint(w, nip);
       /* Update cursor */
	self->text.cursor_x = CharToOffset(w, nip) - self->text.coffset;
	if (ReCenterCursor(w) == FALSE)
	    Redisplay(w, NULL, NULL);
    }
}

/* EndSelect() - ACTION
 *
 * End a selection.
 */
/* ARGSUSED */
static void
EndSelect(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    int                 nip;
    int                 len;
    char               *str;

    _XpwDisArmClue(w);
    if (event == NULL)
	return;
    fprintf(stderr, "EndSelection(%s, %d)\n", XtName(w), event->xbutton.x);
   /* Remove cursor if shown */
   /* Compute character offset of mouse pointer */
    nip = OffsetToChar(w, event->xbutton.x);
    fprintf(stderr, "Cursor at %d\n", nip);
   /* Redraw cursor */
    if (self->text.select_end != nip) {
       /* Readjust gap */
	AdjustPoint(w, nip);
	self->text.cursor_x = CharToOffset(w, nip) - self->text.coffset;
	self->text.select_end = nip;
	if (ReCenterCursor(w) == FALSE)
	    Redisplay(w, NULL, NULL);
    }
    len = self->text.highend - self->text.highstart;
    if (len > 0) {
	self->text.select_size = len;
       /* Remove the old one */
	if (self->text.select_text)
	    XtFree(self->text.select_text);
	self->text.select_text = XtMalloc(len + 1);
	if (self->text.highstart == self->text.select_start)
	   /* After IP */
	    str = &self->text.textbuffer[self->text.highstart];
	else
	    str = &self->text.textbuffer[self->text.insert_point +
					 self->text.cgap_size];
	strncpy(self->text.select_text, str, len);

	XtOwnSelection(w, XA_PRIMARY, event->xbutton.time,
		       ConvertSelection, LoseSelection, NULL);
	XChangeProperty(XtDisplay(w), DefaultRootWindow(XtDisplay(w)),
			XA_CUT_BUFFER0, XA_STRING, 8, PropModeReplace,
			(unsigned char *) self->text.select_text, len);

    }
    DrawCursor(w);
}

/* InsertSelect() - ACTION
 *
 * Insert selection buffer into text buffer.
 */
/* ARGSUSED */
static void
InsertSelect(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    int                 nip;

    _XpwDisArmClue(w);
   /* Check if editable */
    if (!self->text.editable)
	return;

   /* Remove cursor */
    EraseCursor(w);
   /* Compute charater offset of mouse pointer */
    nip = OffsetToChar(w, event->xbutton.x);
   /* Collect selection and determin size */
    XtGetSelectionValue(w, XA_PRIMARY, XA_STRING, RequestSelection,
			(XtPointer) nip, event->xbutton.time);
}



/* DeleteToEOL() - ACTION
 *
 * Delete to end of line.
 */
/* ARGSUSED */
static void
DeleteToEOL(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;

    _XpwDisArmClue(w);
   /* Check if editable */
    if (!self->text.editable)
	return;

   /* Remove cursor */
    EraseCursor(w);
   /* If there is a selection, clear selection and mark for full redraw */
    ClearHighlight(w);
   /* Grow gap to include remainder of the buffer */
    if (self->text.insert_point >= 0) {
	self->text.insert_point;
	self->text.cgap_size += self->text.stringsize - self->text.insert_point;
	self->text.stringsize = self->text.insert_point;
    }
   /* Clear from cursor point till end of window */
   /* Decide if display needs to be readjusted */
   /* Redraw cursor */
    if (ReCenterCursor(w) == FALSE)
	Redisplay(w, NULL, NULL);
    DrawCursor(w);
}

/* DeleteWord() - ACTION
 *
 * Delete next word.
 */
/* ARGSUSED */
static void
DeleteWord(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    int                 len, left;
    char               *p;

    _XpwDisArmClue(w);
   /* Check if editable */
    if (!self->text.editable)
	return;

   /* Remove cursor */
    EraseCursor(w);
   /* If there is a selection, clear selection and mark for full redraw */
    ClearHighlight(w);
   /* Grow gap to include next word */
    if (self->text.insert_point >= 0) {
	len = 0;
	p = &self->text.textbuffer[self->text.insert_point + self->text.cgap_size];
	for (left = self->text.stringsize - self->text.insert_point; left > 0; left--) {
	    if (*p++ == ' ')
		break;
	    len++;
	}
	self->text.cgap_size += len;
	self->text.stringsize -= len;
    }
   /* Compute length of removed word */
   /* Copy display over removed part */
   /* Clear for cursor to end of window */
   /* Decide if display needs to be readjusted */
    if (ReCenterCursor(w) == FALSE)
	Redisplay(w, NULL, NULL);
   /* Redraw cursor */
    DrawCursor(w);
}

/************************************************************
 *
 * Public interface functions
 *
 ************************************************************/

/*
 * Return if field is editable.
 */
Boolean 
XpwSimpleTextGetEditable(w)
	Widget              w;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;

    return self->text.editable;
}

void 
XpwSimpleTextSetEditable(w, editable)
	Widget              w;
	Boolean             editable;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;

    self->text.editable = editable;
}

/*
 * Return the insert point.
 */
int 
XpwSimpleTextGetInsertionPosition(w)
	Widget              w;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;

    return self->text.insert_point;
}

/*
 * Set the insert point.
 */
void 
XpwSimpleTextSetInsertionPosition(w, pos)
	Widget              w;
	int                 pos;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    int                 len;
    char               *p;

    ClearHighlight(w);

   /* Move to insert Point */
    AdjustPoint(w, pos);

   /* Reposition cursor */
    self->text.cursor_x = CharToOffset(w, self->text.insert_point)
	- self->text.coffset;
    if (ReCenterCursor(w) == FALSE)
	Redisplay(w, NULL, NULL);
}

/*
 * Return the contents of the text buffer. It is users responsablity to
 * free the string when done.
 */
char               *
XpwSimpleTextGetString(w)
	Widget              w;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    char               *str;
    int                 len;

   /* Allocate Space */
    str = XtMalloc(self->text.stringsize + 1);

   /* Copy first half of string */
    strncpy(str, self->text.textbuffer, self->text.insert_point);

   /* Now finish it off */
    strncpy(&str[self->text.insert_point],
	    &self->text.textbuffer[self->text.insert_point + self->text.cgap_size],
	    self->text.stringsize - self->text.insert_point);

   /* Make sure line is terminated */
    str[self->text.stringsize] = '\0';
    return str;
}

/*
 * Set the buffer to string.
 */
void 
XpwSimpleTextSetString(w, str)
	Widget              w;
	char               *str;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    int                 len;

    if (str == NULL)
	return;

    ClearHighlight(w);

   /* Remove all storage */
    XtFree(self->text.textbuffer);
   /* Build free new buffer */
    self->text.stringsize = strlen(str);
    self->text.cgap_size = self->text.gapsize;
    self->text.textbuffer = XtMalloc(self->text.stringsize + self->text.gapsize + 1);
    self->text.insert_point = self->text.stringsize;
    strcpy(self->text.textbuffer, str);

   /* Reposition cursor */
    self->text.cursor_x = CharToOffset(w, self->text.insert_point)
	- self->text.coffset;
    if (ReCenterCursor(w) == FALSE)
	Redisplay(w, NULL, NULL);
}

/*
 * Insert string at the insert point.
 */
void 
XpwSimpleTextInsert(w, pos, str)
	Widget              w;
	int                 pos;
	char               *str;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    int                 len;
    char               *p;

    ClearHighlight(w);

   /* Move to insert Point */
    AdjustPoint(w, pos);

   /* Sanity Check */
    if (str != NULL && (len = strlen(str)) > 0) {
       /* Grow buffer if need be */
	GrowBuffer(w, len);
       /* Copy text into buffer. */
	p = &self->text.textbuffer[self->text.insert_point];
       /* Adjust offsets */
	self->text.insert_point += len;
	self->text.cgap_size -= len;
	self->text.stringsize += len;
	while (len-- > 0)
	    *p++ = *str++;
    }
   /* Reposition cursor */
    self->text.cursor_x = CharToOffset(w, self->text.insert_point)
	- self->text.coffset;
    if (ReCenterCursor(w) == FALSE)
	Redisplay(w, NULL, NULL);
}

/*
 * Replace a chunk of text.
 */
void 
XpwSimpleTextReplace(w, first, last, str)
	Widget              w;
	int                 first;
	int                 last;
	char               *str;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    int                 len;

    if (first > last)
	return;

    ClearHighlight(w);
   /* Move to insert point */
    AdjustPoint(w, first);

    if (last > self->text.stringsize)
	last = self->text.stringsize;

    len = last - first;
    if (len > 0) {
	self->text.cgap_size -= len;
	self->text.stringsize -= len;
    }
    if (str != NULL && (len = strlen(str)) > 0) {
	char               *p;

       /* Grow buffer if need be */
	GrowBuffer(w, len);
       /* Copy text into buffer. */
	p = &self->text.textbuffer[self->text.insert_point];
       /* Adjust offsets */
	self->text.insert_point += len;
	self->text.cgap_size -= len;
	self->text.stringsize += len;
	while (len-- > 0)
	    *p++ = *str++;

    }
   /* Reposition cursor */
    self->text.cursor_x = CharToOffset(w, self->text.insert_point)
	- self->text.coffset;
    if (ReCenterCursor(w) == FALSE)
	Redisplay(w, NULL, NULL);
}

/*
 * Set the highlight and move it to the cut buffer.
 */
void 
XpwSimpleTextSetSelection(w, start, end)
	Widget              w;
	int                 start;
	int                 end;
{
    SimpleTextWidget      self = (SimpleTextWidget) w;
    int                 len;
    Time                t;

    ClearHighlight(w);

    if (start < 0 || start == end)
	return;

    if (end > self->text.stringsize)
	end = self->text.stringsize;

    self->text.highstart = self->text.select_start = start;
    self->text.highend = self->text.select_end = start;
    AdjustPoint(w, end);
    self->text.cursor_x = CharToOffset(w, end) - self->text.coffset;
    if (ReCenterCursor(w) == FALSE)
	Redisplay(w, NULL, NULL);
    len = self->text.highend - self->text.highstart;
    if (len > 0) {
	char               *str;

	self->text.select_size = len;
       /* Remove the old one */
	if (self->text.select_text)
	    XtFree(self->text.select_text);
	self->text.select_text = XtMalloc(len + 1);
	if (self->text.highstart == self->text.select_start)
	   /* After IP */
	    str = &self->text.textbuffer[self->text.highstart];
	else
	    str = &self->text.textbuffer[self->text.insert_point +
					 self->text.cgap_size];
	strncpy(self->text.select_text, str, len);

	time(&t);
	XtOwnSelection(w, XA_PRIMARY, t,
		       ConvertSelection, LoseSelection, NULL);
	XChangeProperty(XtDisplay(w), DefaultRootWindow(XtDisplay(w)),
			XA_CUT_BUFFER0, XA_STRING, 8, PropModeReplace,
			(unsigned char *) self->text.select_text, len);

    }
}
