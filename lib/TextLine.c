/*
 * TextLine widget.
 *
 * Simple widget for collecting text input.
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
 *
 * $Log: TextLine.c,v $
 * Revision 1.2  1997/11/01 06:39:09  rich
 * Removed unused variables.
 *
 * Revision 1.1  1997/10/25 22:15:47  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: TextLine.c,v 1.2 1997/11/01 06:39:09 rich Beta rich $";

#endif

#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Xmu/Xmu.h>
#include <X11/Xmu/Drawing.h>

#include "XpwInit.h"
#include "TextLineP.h"
#include "Clue.h"
#include "focus.h"

static void         Initialize(Widget /*junk */ , Widget /*new */ ,
				 ArgList /*args */ , Cardinal * /*num_args */ );
static Boolean      SetValues(Widget /*current */ , Widget /*request */ ,
				 Widget /*new */ , ArgList /*args */ ,
				 Cardinal * /*num_args */ );
static XtGeometryResult QueryGeometry(Widget /*w */ ,
				 XtWidgetGeometry * /*intended */ ,
				 XtWidgetGeometry * /*requested */ );
static void         Redisplay(Widget /*wid */ , XEvent * /*event */ ,
				 Region /*reg */ );
static void         Resize(Widget /*w */ );
static void         Destroy(Widget /*w */ );

/* Private Procedures */
static void         DrawText(TextLineWidget /*self */ , Display * /*dpy */ ,
				 Window /*win */ , GC /*gc */ ,
				 int /*x_loc */ , int /*y_loc */ ,
				 String /*str */ , int /*len */ );
static int          TextLen(TextLineWidget /*self */ , String /*str */ ,
				 int /*len */ );
static void         AdjustPoint(TextLineWidget /*self */ , int /*newpoint */ );
static void         GrowBuffer(TextLineWidget /*self */ , int /*num */ );
static void         ShrinkBuffer(TextLineWidget /*self */ , int /*num */ );
static void         EraseCursor(Widget /*w */ );
static void         DrawCursor(Widget /*w */ );
static int          DeleteHighlight(TextLineWidget /*self */ );
static void         ClearHighlight(TextLineWidget /*self */ );
static int          OffsetToChar(TextLineWidget /*self */ , int /*offset */ );
static int          CharToOffset(TextLineWidget /*self */ , int /*pos */ );
static Boolean      ConvertSelection(Widget /*w */ , Atom * /*selection */ ,
				 Atom * /*target */ , Atom * /*type */ ,
				 XtPointer * /*value */ ,
				 unsigned long * /*length */ , int * /*format */ );
static void         LoseSelection(Widget /*w */ , Atom * /*selection */ );
static void         RequestSelection(Widget /*w */ , XtPointer /*client */ ,
				 Atom * /*selection */ , Atom * /*type */ ,
				 XtPointer /*value */ ,
				 unsigned long * /*length */ , int * /*format */ );
static void         CreateGCs(Widget /*w */ );
static void         DestroyGCs(Widget /*w */ );

/* Actions */
static void         ArmClue(Widget /*w */ , XEvent * /*event */ ,
			     String * /*params */ , Cardinal * /*num_params */ );
static void         DisArmClue(Widget /*w */ , XEvent * /*event */ ,
			     String * /*params */ , Cardinal * /*num_params */ );
static void         KeyFocusIn(Widget /*w */ , XEvent * /*event */ ,
			     String * /*params */ , Cardinal * /*num_params */ );
static void         KeyFocusOut(Widget /*w */ , XEvent * /*event */ ,
			     String * /*params */ , Cardinal * /*num_params */ );
static void         MoveFocus(Widget /*w */ , XEvent * /*event */ ,
			     String * /*params */ , Cardinal * /*num_params */ );
static void         Notify(Widget /*w */ , XEvent * /*event */ ,
			     String * /*params */ , Cardinal * /*num_params */ );
static void         InsertChar(Widget /*w */ , XEvent * /*event */ ,
			     String * /*params */ , Cardinal * /*num_params */ );
static void         DeleteChar(Widget /*w */ , XEvent * /*event */ ,
			     String * /*params */ , Cardinal * /*num_params */ );
static void         DeleteNextChar(Widget /*w */ , XEvent * /*event */ ,
			     String * /*params */ , Cardinal * /*num_params */ );
static void         RightChar(Widget /*w */ , XEvent * /*event */ ,
			     String * /*params */ , Cardinal * /*num_params */ );
static void         LeftChar(Widget /*w */ , XEvent * /*event */ ,
			     String * /*params */ , Cardinal * /*num_params */ );
static void         EndOfLine(Widget /*w */ , XEvent * /*event */ ,
			     String * /*params */ , Cardinal * /*num_params */ );
static void         BeginOfLine(Widget /*w */ , XEvent * /*event */ ,
			     String * /*params */ , Cardinal * /*num_params */ );
static void         StartSelect(Widget /*w */ , XEvent * /*event */ ,
			     String * /*params */ , Cardinal * /*num_params */ );
static void         ExtendSelect(Widget /*w */ , XEvent * /*event */ ,
			     String * /*params */ , Cardinal * /*num_params */ );
static void         EndSelect(Widget /*w */ , XEvent * /*event */ ,
			     String * /*params */ , Cardinal * /*num_params */ );
static void         SelectWord(Widget /*w */ , XEvent * /*event */ ,
			     String * /*params */ , Cardinal * /*num_params */ );
static void         SelectLine(Widget /*w */ , XEvent * /*event */ ,
			     String * /*params */ , Cardinal * /*num_params */ );
static void         InsertSelect(Widget /*w */ , XEvent * /*event */ ,
			     String * /*params */ , Cardinal * /*num_params */ );
static void         DeleteToEOL(Widget /*w */ , XEvent * /*event */ ,
			     String * /*params */ , Cardinal * /*num_params */ );
static void         DeleteWord(Widget /*w */ , XEvent * /*event */ ,
			     String * /*params */ , Cardinal * /*num_params */ );

static XtActionsRec actions[] =
{
    {"ArmClue", ArmClue},
    {"DisArmClue", DisArmClue},
    {"KeyFocusIn", KeyFocusIn},
    {"KeyFocusOut", KeyFocusOut},
    {"MoveFocus", MoveFocus},
    {"Notify", Notify},
    {"InsertChar", InsertChar},
    {"DeleteChar", DeleteChar},
    {"DeleteNextChar", DeleteNextChar},
    {"RightChar", RightChar},
    {"LeftChar", LeftChar},
    {"EndOfLine", EndOfLine},
    {"BeginOfLine", BeginOfLine},
    {"StartSelect", StartSelect},
    {"ExtendSelect", ExtendSelect},
    {"EndSelect", EndSelect},
    {"SelectWord", SelectWord},
    {"SelectLine", SelectLine},
    {"InsertSelect", InsertSelect},
    {"DeleteToEOL", DeleteToEOL},
    {"DeleteWord", DeleteWord},
};

/* Default Translation table.  */
static char         defaultTranslations[] =
 "<EnterWindow>:      ArmClue()\n\
  <LeaveWindow>:      DisArmClue()\n\
     <FocusOut>:      KeyFocusOut()\n\
      <FocusIn>:      KeyFocusIn()\n\
     <Key>Right:      RightChar()\n\
      <Key>Left:      LeftChar()\n\
    <Key>Delete:      DeleteChar()\n\
 <Key>BackSpace:      DeleteChar()\n\
    <Key>Return:      Notify()\n\
       <Key>Tab:      Notify() MoveFocus(Next)\n\
 Shift<Key>Down:      MoveFocus(Next)\n\
   Shift<Key>Up:      MoveFocus(Prev)\n\
 Shift<Key>Home:      MoveFocus(Home)\n\
  Shift<Key>End:      MoveFocus(End)\n\
     Ctrl<Key>A:      BeginOfLine()\n\
     Ctrl<Key>D:      DeleteNextChar()\n\
     Ctrl<Key>F:      RightChar()\n\
     Ctrl<Key>B:      LeftChar()\n\
     Ctrl<Key>U:      BeginOfLine() DeleteToEOL()\n\
     Ctrl<Key>E:      EndOfLine()\n\
     Ctrl<Key>W:      DeleteWord()\n\
     Ctrl<Key>K:      DeleteToEOL()\n\
      <Key>Home:      BeginOfLine()\n\
   <Key>KP_Home:      BeginOfLine()\n\
       <Key>End:      EndOfLine()\n\
    <Key>KP_End:      EndOfLine()\n\
          <Key>:      InsertChar()\n\
     <Btn1Down>:      StartSelect()\n\
   <Btn1Motion>:      ExtendSelect()\n\
       <Btn1Up>:      EndSelect()\n\
    <Btn1Up>(2):      SelectWord()\n\
    <Btn1Up>(3):      SelectLine()\n\
     <Btn2Down>:      InsertSelect()\n";

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

#define offset(field) XtOffset(TextLineWidget, text.field)

static XtResource   resources[] =
{
    {XtNstring, XtCString, XtRString, sizeof(String),
     offset(string), XtRImmediate, (XtPointer) NULL},
    {XtNinternational, XtCInternational, XtRBoolean, sizeof(Boolean),
     offset(international), XtRImmediate, (XtPointer) FALSE},
    {XtNvertSpace, XtCVertSpace, XtRInt, sizeof(int),
     offset(vert_space), XtRImmediate, (XtPointer) 25},
    {XtNleftMargin, XtCHorizontalMargins, XtRDimension, sizeof(Dimension),
     offset(left_margin), XtRImmediate, (XtPointer) 4},
    {XtNrightMargin, XtCHorizontalMargins, XtRDimension, sizeof(Dimension),
     offset(right_margin), XtRImmediate, (XtPointer) 4},
    {XtNcenterAmount, XtCHorizontalMargins, XtRDimension, sizeof(Dimension),
     offset(centeramount), XtRImmediate, (XtPointer) 75},
    {XtNcforeground, XtCForeground, XtRPixel, sizeof(Pixel),
     offset(cforeground), XtRString, XtDefaultForeground},
    {XtNaforeground, XtCForeground, XtRPixel, sizeof(Pixel),
     offset(aforeground), XtRString, XtDefaultBackground},
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
     offset(foreground), XtRString, XtDefaultForeground},
    {XtNfont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
     offset(font), XtRString, XtDefaultFont},
    {XtNfontSet, XtCFontSet, XtRFontSet, sizeof(XFontSet),
     offset(fontset), XtRString, XtDefaultFontSet},
    {XtNeditable, XtCEditable, XtRBoolean, sizeof(Boolean),
     offset(editable), XtRImmediate, (XtPointer) TRUE},
    {XtNdoArrows, XtCDoArrows, XtRBoolean, sizeof(Boolean),
     offset(doArrows), XtRImmediate, (XtPointer) TRUE},
    {XtNpassMode, XtCPassMode, XtRBoolean, sizeof(Boolean),
     offset(passmode), XtRImmediate, (XtPointer) FALSE},
    {XtNcallback, XtCCallback, XtRCallback, sizeof(XtPointer),
     offset(callbacks), XtRCallback, (XtPointer) NULL},
    {XtNfocusGroup, XtCFocusGroup, XtRWidget, sizeof(Widget),
     offset(focusGroup), XtRImmediate, (XtPointer) NULL},
    {XtNtraversalOn, XtCTraversalOn, XtRBoolean, sizeof(Boolean),
     offset(traversal), XtRImmediate, (XtPointer) False},
    /* ThreeD resouces */
    threeDresources
    {XtNclue, XtCLabel, XtRString, sizeof(String),
     offset(clue), XtRImmediate, (XtPointer) NULL}
};

#define superclass (&widgetClassRec)

TextLineClassRec    textLineClassRec =
{
    {	/* core_class fields */
	(WidgetClass) superclass,	/* superclass               */
	"TextLine",			/* class_name               */
	sizeof(TextLineRec),		/* widget_size              */
	XpwInitializeWidgetSet,		/* class_initialize         */
	NULL,				/* class_part_initialize    */
	FALSE,				/* class_inited             */
	Initialize,			/* initialize               */
	NULL,				/* initialize_hook          */
	XtInheritRealize,		/* realize                  */
	actions,			/* actions                  */
	XtNumber(actions),		/* num_actions              */
	resources,			/* resources                */
	XtNumber(resources),		/* num_resources            */
	NULLQUARK,			/* xrm_class                */
	TRUE,				/* compress_motion          */
	FALSE,				/* compress_exposure        */
	TRUE,				/* compress_enterleave      */
	FALSE,				/* visible_interest         */
	Destroy,			/* destroy                  */
	Resize,				/* resize                   */
	Redisplay,			/* expose                   */
	SetValues,			/* set_values               */
	NULL,				/* set_values_hook          */
	XtInheritSetValuesAlmost,	/* set_values_almost        */
	NULL,				/* get_values_hook          */
	NULL,				/* accept_focus             */
	XtVersion,			/* version                  */
	NULL,				/* callback_private         */
	defaultTranslations,		/* tm_table                 */
	QueryGeometry,			/* query_geometry           */
    },
    {	/* TextLine class fields initialization */
	0				/* not used                 */
    },
};

WidgetClass         textLineWidgetClass = (WidgetClass) & textLineClassRec;

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
    TextLineWidget      nself = (TextLineWidget) new;
    int                 font_ascent = 0, font_descent = 0;
    XFontSetExtents    *ext = XExtentsOfFontSet(nself->text.fontset);
    Dimension           width, height;
    String              str;
    int                 len;

   /* Build a defualt buffer */
    if (nself->text.string == NULL)
	nself->text.string = XtName(new);
    nself->text.string = XtNewString(nself->text.string);
    nself->text.stringsize = strlen(nself->text.string);
    nself->text.insert_point = 0;
    nself->text.textstart = 0;
    nself->text.coffset = 0;
    nself->text.cursor_x = 0;
    nself->text.highstart = 0;
    nself->text.highend = 0;
    nself->text.right_arrow = FALSE;
    nself->text.left_arrow = FALSE;
    nself->text.select_text = NULL;

    CreateGCs(new);
    _XpwThreeDInit(new, &nself->text.threeD, nself->core.background_pixel);
    width = 0;

    width = nself->text.left_margin + nself->text.right_margin;
    if (nself->text.international == True) {
	font_ascent = abs(ext->max_ink_extent.y);
	font_descent = ext->max_ink_extent.height - font_ascent;
    } else {			/*else, compute size from font like R5 */
	font_ascent = nself->text.font->max_bounds.ascent;
	font_descent = nself->text.font->max_bounds.descent;
    }
    height = font_ascent + font_descent;

    str = nself->text.string;
    len = nself->text.stringsize;
   /* Next compute width of first part of string */
    width += TextLen(nself, str, len);
    height = ((int) height * (100 + nself->text.vert_space)) / 100;
    width += 2 * nself->text.threeD.shadow_width;
    height += 2 * nself->text.threeD.shadow_width;

    if (nself->core.width == 0)
	nself->core.width = width;
    if (nself->core.height == 0)
	nself->core.height = height;
    if (nself->text.focusGroup != NULL)
	_XpwAddFocusGroup(new, nself->text.focusGroup);
    nself->text.infocus = False;
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
    TextLineWidget      self = (TextLineWidget) new;
    TextLineWidget      old_self = (TextLineWidget) current;

    if (((old_self->text.font != self->text.font) &&
	 (old_self->text.international == False)) ||
	(old_self->text.aforeground != self->text.aforeground) ||
	(old_self->text.cforeground != self->text.cforeground) ||
	(old_self->text.foreground != self->text.foreground)) {
	DestroyGCs(current);
	CreateGCs(new);
	ret_val = TRUE;
    }
    if (old_self->text.string != self->text.string) {
	XtFree(old_self->text.string);
	self->text.string = XtNewString(self->text.string);
	self->text.stringsize = strlen(self->text.string);
	ClearHighlight(self);
	self->text.cursor_x = self->text.coffset = self->text.textstart = 0;
	self->text.insert_point = 0;
    }
    if ((old_self->text.fontset != self->text.fontset) &&
	(old_self->text.international == True))
       /* DONT changes the GCs, because the fontset is not in them. */
	ret_val = TRUE;

    if (_XpmThreeDSetValues(new, &old_self->text.threeD,
			 &self->text.threeD, new->core.background_pixel))
	ret_val = TRUE;

    if ((old_self->text.passmode != self->text.passmode))
	ret_val = TRUE;
    if (old_self->text.focusGroup != self->text.focusGroup) {
	if (old_self->text.focusGroup != NULL)
	    _XpwClearFocusGroup(current, old_self->text.focusGroup);

	if (self->text.focusGroup != NULL)
	    _XpwAddFocusGroup(new, self->text.focusGroup);
    }
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
    TextLineWidget      self = (TextLineWidget) w;
    int                 font_ascent = 0, font_descent = 0;
    XFontSetExtents    *ext = XExtentsOfFontSet(self->text.fontset);
    Dimension           width, height;
    XtGeometryResult    ret_val = XtGeometryYes;
    XtGeometryMask      mode = intended->request_mode;
    String              str;
    int                 len;

    width = 0;

    width = self->text.left_margin + self->text.right_margin;
    if (self->text.international == True) {
	font_ascent = abs(ext->max_ink_extent.y);
	font_descent = ext->max_ink_extent.height - font_ascent;
    } else {			/*else, compute size from font like R5 */
	font_ascent = self->text.font->max_bounds.ascent;
	font_descent = self->text.font->max_bounds.descent;
    }
    height = font_ascent + font_descent;

    str = self->text.string;
    len = self->text.stringsize;
    width += TextLen(self, str, len);

    height = ((int) height * (100 + self->text.vert_space)) / 100;
    width += 4 + 2 * self->text.threeD.shadow_width;
    height += 4 + 2 * self->text.threeD.shadow_width;

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
    Redisplay(w, NULL, NULL);
}

#define abs(x)	(((x)<0)?-(x):(x))
/*
 * Repaints the widget window on expose events.
 */

/* ARGSUSED */
static void
Redisplay(wid, event, reg)
	Widget              wid;
	XEvent             *event;
	Region              reg;
{
    TextLineWidget      self = (TextLineWidget) wid;
    GC                  gc, rgc, dgc, arrow;
    int                 font_ascent = 0, font_descent = 0;
    Position		x_loc, y_loc;
    XFontSetExtents    *ext = XExtentsOfFontSet(self->text.fontset);
    Display            *dpy = XtDisplayOfObject(wid);
    Window              win = XtWindowOfObject(wid);
    Dimension           s = self->text.threeD.shadow_width;
    Dimension           h = self->core.height;
    Dimension           w = self->core.width;
    int                 t_width;
    String              str;
    int                 len;
    int                 tstart;
    int                 hstart;
    int                 hend;
    XRectangle          cliparea;

    if (!XtIsRealized(wid))
	return;

   /* Set the context based on sensitivity setting */
    if (XtIsSensitive(wid) && XtIsSensitive(XtParent(wid))) {
	gc = self->text.norm_gc;
	rgc = self->text.rev_gc;
	dgc = self->text.norm_gc;
	arrow = self->text.arrow_gc;
    } else {
	gc = self->text.norm_gray_gc;
	rgc = self->text.rev_gray_gc;
	dgc = self->text.norm_gray_gc;
	arrow = self->text.norm_gray_gc;
    }

    x_loc = self->text.left_margin + s;
    if (self->text.international == True) {
	font_ascent = abs(ext->max_ink_extent.y);
	font_descent = ext->max_ink_extent.height - font_ascent;
    } else {			/*else, compute size from font like R5 */
	font_ascent = self->text.font->max_bounds.ascent;
	font_descent = self->text.font->max_bounds.descent;
    }
    y_loc = s + ((int) h - (font_ascent + font_descent)) / 2 + font_ascent;

    cliparea.x = x_loc;
    cliparea.y = s;
    cliparea.width = w - 2 * s - self->text.left_margin - self->text.right_margin;
    cliparea.height = h - 2 * s;
    XSetClipRectangles(dpy, gc, 0, 0, &cliparea, 1, YXBanded);
   /* First clear the whole window */
    XClearArea(dpy, win, s, s, w, h, FALSE);

   /* Draw shadows around main window */
    _XpwThreeDDrawShadow(wid, event, reg, &(self->text.threeD), 0, 0, w, h, TRUE);

   /* Draw the arrows */
    if (self->text.left_arrow) {
	_XpwThreeDDrawShape(wid, NULL, NULL, &(self->text.threeD), arrow,
			    XaLeft, s, s, h - s * 2, h - s * 2, TRUE);
	cliparea.x += h;
	cliparea.width -= h;
	x_loc += h;
    }
    if (self->text.right_arrow) {
	_XpwThreeDDrawShape(wid, NULL, NULL, &(self->text.threeD), arrow,
			  XaRight, w - h, s, h - 2 * s, h - s * s, TRUE);
	cliparea.width -= h;
    }
    XSetClipRectangles(dpy, gc, 0, 0, &cliparea, 1, YXBanded);
    XSetClipRectangles(dpy, rgc, 0, 0, &cliparea, 1, YXBanded);
    XSetClipRectangles(dpy, dgc, 0, 0, &cliparea, 1, YXBanded);

    tstart = self->text.textstart;
    str = &self->text.string[tstart];
    len = self->text.stringsize - tstart;
    hstart = self->text.highstart - tstart;
    hend = self->text.highend - tstart;
   /* Check if cursor at very right */
    x_loc++;

   /* Check if we have to draw a highlight */
    if (hstart != hend && hstart < len && hend > 0) {
	int                 temp;

       /* Draw up to hstart */
	if (hstart > 0) {
	    DrawText(self, dpy, win, gc, x_loc, y_loc, str, hstart);
	   /* Adjust pointers */
	    x_loc += TextLen(self, str, hstart);
	   /* Check if next draw will be outside window */
	    if (x_loc > w)
		return;
	    str += hstart;
	    len -= hstart;
	    hend -= hstart;
	}
       /* Grab smaller of two */
	temp = (hend < len) ? hend : len;
	t_width = TextLen(self, str, temp);
       /* Draw background */
	XFillRectangle(dpy, win, dgc, x_loc, s, t_width, h - 2 * s);
	DrawText(self, dpy, win, rgc, x_loc, y_loc, str, temp);
	str += temp;
	len -= temp;
	x_loc += t_width;
    }
   /* Draw whatever is left. */
    if (len > 0 && x_loc < w)
	DrawText(self, dpy, win, gc, x_loc, y_loc, str, len);
    DrawCursor(wid);
}

static void
Destroy(w)
	Widget              w;
{
    TextLineWidget      self = (TextLineWidget) w;

    DestroyGCs(w);
    XtFree(self->text.string);
    XtFree(self->text.select_text);
    _XpwThreeDDestroyShadow(w, &(self->text.threeD));
}

/************************************************************
 *
 * Private Functions.
 *
 ************************************************************/
/* Some short cuts for some repeated text */
static void 
DrawText(self, dpy, win, gc, x_loc, y_loc, str, len)
	TextLineWidget      self;
	Display            *dpy;
	Window              win;
	GC                  gc;
	int                 x_loc;
	int                 y_loc;
	String              str;
	int                 len;
{

    if (self->text.passmode) {
	int                 i;
	int                 t_width = TextLen(self, "*", 1);

	for (i = 0; i <= len; i++) {
	    if (self->text.international == True)
		XmbDrawString(dpy, win, self->text.fontset, gc, x_loc, y_loc, "*", len);
	    else
		XDrawString(dpy, win, gc, x_loc, y_loc, "*", 1);
	    x_loc += t_width;
	}
    } else {
	if (self->text.international == True)
	    XmbDrawString(dpy, win, self->text.fontset, gc, x_loc, y_loc, str, len);
	else
	    XDrawString(dpy, win, gc, x_loc, y_loc, str, len);
    }
}

static int 
TextLen(self, str, len)
	TextLineWidget      self;
	String              str;
	int                 len;
{
    int                 rep = 1;

    if (self->text.passmode) {
	str = "*";
	rep = len;
	len = 1;
    }
    return (rep * ((self->text.international == True) ?
		   XmbTextEscapement(self->text.fontset, str, len) :
		   XTextWidth(self->text.font, str, len)));
}

/*
 * Adjust cursor to center of window if it drifted out of window.
 */
static int
ReCenterCursor(w)
	Widget              w;
{
    TextLineWidget      self = (TextLineWidget) w;
    Dimension           s = self->text.threeD.shadow_width;
    Dimension           h = self->core.height;
    Dimension           wd = self->core.width;
    Dimension           lside, rside;
    int                 nc = 0;
    char               *str = self->text.string;
    int                 i;
    int                 t_width = 0;
    int                 redraw = FALSE;

    lside = s + self->text.left_margin;
    if (self->text.left_arrow)
	lside += h;
    i = lside + self->text.cursor_x;
    rside = wd - s - self->text.right_margin;
    if (self->text.right_arrow)
	rside -= h;

   /* Compute new cursor point */
    if (i > (rside)) {
	nc = wd - (((rside - lside) * self->text.centeramount) / 100);
    } else if (i < lside) {
	nc = ((rside - lside) * self->text.centeramount) / 100;
    }
   /* Walk backwords from insert point till we have at least nc chars */
    if (nc != 0) {
	for (i = self->text.insert_point - 1; i >= 0; i--) {
	    t_width += TextLen(self, &str[i], 1);
	    if (t_width >= nc)
		break;
	}
       /* readjust all pointers now */
	if (self->text.textstart != i)
	    redraw = TRUE;
	self->text.cursor_x = t_width;
	self->text.textstart = i;
	if (i > 0)
	    t_width = TextLen(self, str, i);
	else {
	    t_width = 0;
	    self->text.textstart = 0;
	}
	self->text.coffset = t_width;
    } else {
	t_width = self->text.coffset;
    }
    if (self->text.doArrows) {
	Boolean             narrow;

	if (t_width > 0) {
	    narrow = TRUE;
	} else
	    narrow = FALSE;
	if (self->text.left_arrow != narrow) {
	    self->text.left_arrow = narrow;
	    redraw = TRUE;
	}
	str = &self->text.string[self->text.insert_point];
	t_width = TextLen(self, str, self->text.stringsize - self->text.insert_point);
	if ((t_width + self->text.cursor_x) > rside) {
	    narrow = TRUE;
	} else
	    narrow = FALSE;
	if (self->text.right_arrow != narrow) {
	    self->text.right_arrow = narrow;
	    redraw = TRUE;
	}
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
AdjustPoint(self, newpoint)
	TextLineWidget      self;
	int                 newpoint;
{
    if (newpoint < 0)
	newpoint = 0;

    if (newpoint > self->text.stringsize)
	newpoint = self->text.stringsize;

    self->text.insert_point = newpoint;
}

/* GrowBuffer
 *
 * Grows the buffer if there is not enough room for num chars.
 */
static void
GrowBuffer(self, num)
	TextLineWidget      self;
	int                 num;
{
    int                 ip = self->text.insert_point;
    int                 sz = self->text.stringsize;
    int                 n;
    char               *p, *q;

    self->text.string = XtRealloc(self->text.string, sz + num + 1);
   /* Compute old and new end. */
    p = &self->text.string[sz];
    q = &self->text.string[sz + num];
   /* Copy backwards */
    for (n = self->text.stringsize - ip; n >= 0; n--)
	*q-- = *p--;
    self->text.stringsize += num;
}

/* ShrinkBuffer
 *
 * Shrinks the buffer to remove num chars after the insert point.
 */
static void
ShrinkBuffer(self, num)
	TextLineWidget      self;
	int                 num;
{
    int                 ip = self->text.insert_point;
    int                 n;
    char               *p, *q;

   /* Compute old and new end. */
    p = &self->text.string[ip + num];
    q = &self->text.string[ip];
   /* Copy back half */
    for (n = self->text.stringsize - ip - num; n > 0; n--)
	*q++ = *p++;
    self->text.stringsize -= num;
    self->text.string = XtRealloc(self->text.string,
				  self->text.stringsize + 1);
}

/*
 * Erase the cursor.
 */
static void
EraseCursor(w)
	Widget              w;
{
    TextLineWidget      self = (TextLineWidget) w;
    GC                  gc;
    Display            *dpy = XtDisplayOfObject(w);
    Window              win = XtWindowOfObject(w);
    Dimension           s = self->text.threeD.shadow_width;
    Dimension           h = self->core.height;
    XPoint              pt[6];
    int                 pos;

   /* Set the context based on sensitivity setting */
    if (XtIsSensitive(w) && XtIsSensitive(XtParent(w))
	&& self->text.cursor_x >= 0) {
	gc = self->text.erase_gc;
	pos = self->text.left_margin + s + self->text.cursor_x;
	if (self->text.left_arrow)
	    pos += h;
	if (self->text.infocus) {
	    pt[0].x = pos - 2, pt[0].y = s;
	    pt[1].x = pos, pt[1].y = s + 2;
	    pt[2].x = pos + 2, pt[2].y = s;
	    XDrawLines(dpy, win, gc, pt, 3, CoordModeOrigin);
	    XDrawLine(dpy, win, gc, pos, s + 2, pos, h - s - 3);
	}
	pt[0].x = pos - 2, pt[0].y = h - s - 1;
	pt[1].x = pos, pt[1].y = h - s - 3;
	pt[2].x = pos + 2, pt[2].y = h - s - 1;
	XDrawLines(dpy, win, gc, pt, 3, CoordModeOrigin);
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
    TextLineWidget      self = (TextLineWidget) w;
    GC                  gc;
    Display            *dpy = XtDisplayOfObject(w);
    Window              win = XtWindowOfObject(w);
    Dimension           s = self->text.threeD.shadow_width;
    Dimension           h = self->core.height;
    XPoint              pt[6];
    int                 pos;

   /* Set the context based on sensitivity setting */
    if (XtIsSensitive(w) && XtIsSensitive(XtParent(w))
	&& self->text.cursor_x >= 0) {
	gc = self->text.cursor_gc;
	pos = self->text.left_margin + s + self->text.cursor_x;
	if (self->text.left_arrow)
	    pos += h;
	if (self->text.infocus) {
	    pt[0].x = pos - 2, pt[0].y = s;
	    pt[1].x = pos, pt[1].y = s + 2;
	    pt[2].x = pos + 2, pt[2].y = s;
	    XDrawLines(dpy, win, gc, pt, 3, CoordModeOrigin);
	    XDrawLine(dpy, win, gc, pos, s + 2, pos, h - s - 3);
	}
	pt[0].x = pos - 2, pt[0].y = h - s - 1;
	pt[1].x = pos, pt[1].y = h - s - 3;
	pt[2].x = pos + 2, pt[2].y = h - s - 1;
	XDrawLines(dpy, win, gc, pt, 3, CoordModeOrigin);
    }
}

/*
 * DeleteHighlight.
 *
 * Removed highlighted text from buffer.
 */
static int
DeleteHighlight(self)
	TextLineWidget      self;
{
    int                 len;

    if (self->text.highstart == self->text.highend)
	return FALSE;
    len = self->text.highend - self->text.highstart;
    AdjustPoint(self, self->text.highstart);
    ShrinkBuffer(self, len);
    self->text.highend = self->text.highstart = -1;
    return TRUE;
}

/* ClearHighlight
 *
 * Clears the selected text.
 */
static void
ClearHighlight(self)
	TextLineWidget      self;
{
    self->text.highstart = self->text.highend = -1;
}

/* OffsetToChar()
 *
 * Translate a offset to a character position.
 */
static int
OffsetToChar(self, offset)
	TextLineWidget      self;
	int                 offset;
{
    int                 ip = self->text.insert_point;
    String              str;
    Dimension           s = self->text.threeD.shadow_width;
    int                 len, dist;
    int                 cp;
    int                 t_width;
    int                 i;

   /* Adjust offset for margin and shadow */
    offset -= self->text.left_margin + s;
    if (self->text.left_arrow)
	offset -= self->core.height;
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
	str = &self->text.string[ip];
	len = self->text.stringsize - ip;
    } else {
	str = self->text.string;
	len = ip;
	ip = 0;
    }
   /* Compute lenght of full string */
    t_width = TextLen(self, str, len);
   /* Check if after end of string */
    if (offset > t_width)
	return ip + len;
    for (dist = i = 0; i < len; i++) {
	t_width = TextLen(self, &str[i], 1);
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
CharToOffset(self, pos)
	TextLineWidget      self;
	int                 pos;
{
    if (pos < 0)
	pos = 0;
    if (pos > self->text.stringsize)
	pos = self->text.stringsize + 1;
   /* Compute length of part before insert pointer */
    return TextLen(self, self->text.string, pos) - self->text.coffset;

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
    TextLineWidget      self = (TextLineWidget) w;
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
    TextLineWidget      self = (TextLineWidget) w;

    ClearHighlight(self);
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
    TextLineWidget      self = (TextLineWidget) w;
    char               *np = (char *) value;
    int                 len = (int) *length;
    char               *p;

    if ((np != NULL) && (len != 0)) {

       /* Remove any highlights */
	ClearHighlight(self);
       /* Point cursor at correct point. */
	AdjustPoint(self, (int) client);
       /* Grow buffer if need be */
	GrowBuffer(self, len);
       /* Copy text into buffer. */
	p = &self->text.string[self->text.insert_point];
       /* Adjust offsets */
	self->text.highstart = self->text.insert_point;
	self->text.insert_point += len;
	self->text.highend = self->text.insert_point;
	while (len-- > 0) {
	    *p++ = *np++;
	}
       /* Adjust cursor */
	self->text.cursor_x = CharToOffset(self, self->text.insert_point);
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
    TextLineWidget      self = (TextLineWidget) w;
    XGCValues           values;
    XtGCMask            mask, mask_i18n;

    values.foreground = self->text.foreground;
    values.background = self->core.background_pixel;
    values.font = self->text.font->fid;
    values.graphics_exposures = FALSE;
    mask = GCForeground | GCBackground | GCGraphicsExposures | GCFont;
    mask_i18n = GCForeground | GCBackground | GCGraphicsExposures;

    self->text.norm_gc = XtAllocateGC(w, 0, mask_i18n, &values, GCFont, 0);
    XSetFont(XtDisplay(w), self->text.norm_gc, self->text.font->fid);

    values.foreground = self->core.background_pixel;
    values.background = self->text.foreground;
    values.font = self->text.font->fid;
    values.graphics_exposures = FALSE;
    values.clip_mask = None;
    mask = GCForeground | GCBackground | GCGraphicsExposures | GCFont;
    mask_i18n = GCForeground | GCBackground | GCGraphicsExposures;

    self->text.rev_gc = XtAllocateGC(w, 0, mask_i18n, &values, GCFont, 0);
    XSetFont(XtDisplay(w), self->text.rev_gc, self->text.font->fid);

    values.foreground = self->text.foreground;
    values.background = self->core.background_pixel;
    values.fill_style = FillTiled;
    values.tile = XmuCreateStippledPixmap(XtScreenOfObject(w),
					  self->text.foreground,
					  self->core.background_pixel,
					  self->core.depth);
    values.graphics_exposures = FALSE;
    mask_i18n |= GCTile | GCFillStyle;
    self->text.norm_gray_gc = XtAllocateGC(w, 0, mask_i18n, &values, GCFont, 0);
    XSetFont(XtDisplay(w), self->text.norm_gray_gc, self->text.font->fid);

    values.background = self->text.foreground;
    values.foreground = self->core.background_pixel;
    values.fill_style = FillTiled;
    values.tile = XmuCreateStippledPixmap(XtScreenOfObject(w),
					  self->core.background_pixel,
					  self->text.foreground,
					  self->core.depth);
    values.graphics_exposures = FALSE;
    mask_i18n |= GCTile | GCFillStyle;
    self->text.rev_gray_gc = XtAllocateGC(w, 0, mask_i18n, &values, GCFont, 0);
    XSetFont(XtDisplay(w), self->text.rev_gray_gc, self->text.font->fid);

    values.foreground = self->text.cforeground;
    values.background = self->core.background_pixel;
    values.graphics_exposures = FALSE;
    mask = GCForeground | GCBackground | GCGraphicsExposures;

    self->text.cursor_gc = XtGetGC(w, mask, &values);

    values.foreground = self->text.aforeground;
    values.background = self->core.background_pixel;
    values.graphics_exposures = FALSE;
    mask = GCForeground | GCBackground | GCGraphicsExposures;

    self->text.arrow_gc = XtGetGC(w, mask, &values);

    values.foreground = self->core.background_pixel;
    values.background = self->text.foreground;
    values.graphics_exposures = FALSE;
    mask = GCForeground | GCBackground | GCGraphicsExposures;

    self->text.erase_gc = XtGetGC(w, mask, &values);

}

/*  
 * Removes all gc's for the text widget.
 */

static void
DestroyGCs(w)
	Widget              w;
{
    TextLineWidget      self = (TextLineWidget) w;

    XtReleaseGC(w, self->text.norm_gc);
    XtReleaseGC(w, self->text.norm_gray_gc);
    XtReleaseGC(w, self->text.rev_gc);
    XtReleaseGC(w, self->text.rev_gray_gc);
    XtReleaseGC(w, self->text.arrow_gc);
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
    TextLineWidget      self = (TextLineWidget) w;

    if (self->text.focusGroup == NULL) {
	EraseCursor(w);
	self->text.infocus = TRUE;
	DrawCursor(w);
    }
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
    TextLineWidget      self = (TextLineWidget) w;

    if (self->text.focusGroup == NULL) {
	EraseCursor(w);
	self->text.infocus = FALSE;
	DrawCursor(w);
    }
    _XpwDisArmClue(w);
}

/*
 * Aquire keyboard focus.
 */

/* ARGSUSED */
static void
KeyFocusIn(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    TextLineWidget      self = (TextLineWidget) w;

    if (self->text.focusGroup != NULL &&
	XtGetKeyboardFocusWidget(self->text.focusGroup) == w) {
	EraseCursor(w);
	self->text.infocus = TRUE;
	DrawCursor(w);
    }
}

/*
 * Loose Keyboard Focus.
 */

/* ARGSUSED */
static void
KeyFocusOut(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    TextLineWidget      self = (TextLineWidget) w;

    EraseCursor(w);
    self->text.infocus = FALSE;
    DrawCursor(w);
}

/*
 * Move to next widget.
 */

/* ARGSUSED */
static void
MoveFocus(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    TextLineWidget      self = (TextLineWidget) w;
    XpwFocusDirection   dir;

    if (*num_params != (Cardinal) 1) {
	XtWarning("Invalid number of parameters to MoveFocus action");
	return;
    }
    if (self->text.focusGroup == NULL)
	return;

    switch (params[0][0]) {
    case 'n':
    case 'N':
	dir = XpwFocusNext;
	break;
    case 'p':
    case 'P':
	dir = XpwFocusPrev;
	break;
    case 'h':
    case 'H':
	dir = XpwFocusHome;
	break;
    case 'e':
    case 'E':
	dir = XpwFocusEnd;
	break;
    default:
	return;
    }
    XpwShiftFocus(w, dir);
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
    TextLineWidget      self = (TextLineWidget) w;
    XpwTextLineReturnStruct ret;

    _XpwDisArmClue(w);
   /* Move to end of line */
    EndOfLine(w, event, params, num_params);
   /* Remove cursor */
    EraseCursor(w);
   /* Adjust buffer size to hold contents and default gap */
   /* Call back to process. */
    ret.reason = 0;
    ret.event = event;
    ret.string = XtNewString(self->text.string);
    XtCallCallbackList(w, self->text.callbacks, (XtPointer) & ret);
   /* Free storage */
    XtFree(ret.string);
    DrawCursor(w);
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
    TextLineWidget      self = (TextLineWidget) w;
    int                 len, i;
    char                buf[10], *p;
    int                 ip = self->text.insert_point;
    int                 t_width;

    _XpwDisArmClue(w);
   /* Check if editable */
    if (!self->text.editable)
	return;

    len = XLookupString((XKeyEvent *) event, buf, sizeof(buf), NULL, NULL);
    if (len > 0) {
       /* remove cursor */
	EraseCursor(w);
       /* If there is a selection, remove old chars first */
	DeleteHighlight(self);
       /* Compute size of new text */
	t_width = TextLen(self, buf, len);
	self->text.cursor_x += t_width;
       /* Grow buffer if need be */
	GrowBuffer(self, len);
       /* Copy text into buffer. */
	p = &self->text.string[ip];
       /* Adjust offsets */
	self->text.insert_point += len;
	for (i = 0; len > 0; i++, len--) {
	    *p++ = buf[i];
	}
       /* Check if display needs to reposition */
	if (ReCenterCursor(w) == FALSE)
	    Redisplay(w, NULL, NULL);
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
    TextLineWidget      self = (TextLineWidget) w;
    String              str;

    _XpwDisArmClue(w);
   /* Check if editable */
    if (!self->text.editable)
	return;

   /* Remove cursor */
    EraseCursor(w);
   /* If there is a selection, remove all chars from selected range */
    if (!DeleteHighlight(self)) {

       /* Adjust insert and gap size */
	if (self->text.insert_point > 0) {
	   /* Compute size of new text */
	    self->text.insert_point--;
	    str = &self->text.string[self->text.insert_point];
	   /* Compute size of char to remove. */
	    self->text.cursor_x -= TextLen(self, str, 1);
	    ShrinkBuffer(self, 1);
	}
    }
   /* Draw cursor */
    if (ReCenterCursor(w) == FALSE)
	Redisplay(w, NULL, NULL);
}

/* DeleteNextChar() - ACTION
 *
 * Delete next character from the buffer.
 */
/* ARGSUSED */
static void
DeleteNextChar(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    TextLineWidget      self = (TextLineWidget) w;

    _XpwDisArmClue(w);
   /* Check if editable */
    if (!self->text.editable)
	return;

   /* Remove cursor */
    EraseCursor(w);
   /* If there is a selection, remove all chars from selected range */
    if (!DeleteHighlight(self)) {

       /* Adjust insert and gap size */
	if (self->text.insert_point < self->text.stringsize)
	    ShrinkBuffer(self, 1);
    }
   /* Draw cursor */
    if (ReCenterCursor(w) == FALSE)
	Redisplay(w, NULL, NULL);
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
    TextLineWidget      self = (TextLineWidget) w;
    int                 t_width;
    String              str;

    _XpwDisArmClue(w);
   /* Remove cursor */
    EraseCursor(w);
   /* If there is a selection, clear selection and mark for full redraw */
    ClearHighlight(self);
   /* Adjust insert pointer */
    if (self->text.insert_point < self->text.stringsize) {
	str = &self->text.string[self->text.insert_point];
	t_width = TextLen(self, str, 1);
	self->text.cursor_x += TextLen(self, str, 1);
	AdjustPoint(self, self->text.insert_point + 1);
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
    TextLineWidget      self = (TextLineWidget) w;
    int                 t_width;
    String              str;

    _XpwDisArmClue(w);
   /* Remove cursor */
    EraseCursor(w);
   /* If there is a selection, clear selection and mark for full redraw */
    ClearHighlight(self);
   /* Adjust insert pointer */
    if (self->text.insert_point > 0) {
	str = &self->text.string[self->text.insert_point];
	t_width = TextLen(self, str, 1);
	AdjustPoint(self, self->text.insert_point - 1);
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
    TextLineWidget      self = (TextLineWidget) w;

    _XpwDisArmClue(w);
   /* Remove cursor */
    EraseCursor(w);
   /* If there is a selection, clear selection and mark for full redraw */
    ClearHighlight(self);
   /* Adjust insert pointer */
    AdjustPoint(self, self->text.stringsize);
   /* Check if display needs to reposition */
    self->text.cursor_x = CharToOffset(self, self->text.insert_point);
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
    TextLineWidget      self = (TextLineWidget) w;

    _XpwDisArmClue(w);
   /* Remove cursor */
    EraseCursor(w);
   /* If there is a selection, clear selection and mark for full redraw */
    ClearHighlight(self);
   /* Adjust insert pointer */
    AdjustPoint(self, 0);
   /* Quick way, just reset cursor */
    self->text.cursor_x = 0;
    self->text.coffset = 0;
    self->text.textstart = 0;
   /* Check if display needs to reposition */
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
    TextLineWidget      self = (TextLineWidget) w;
    int                 nip;
    Boolean             needredraw = FALSE;

    _XpwDisArmClue(w);
    if (event == NULL)
	return;
   /* Remove cursor */
    EraseCursor(w);
   /* Grab focus */
    if (self->text.focusGroup != NULL)
	_XpwSetFocus(w, self->text.focusGroup);
   /* If there is a selection, clear selection and mark for full redraw */
    if (self->text.highstart != self->text.highend)
	needredraw = TRUE;
   /* Compute charater offset of mouse pointer */
    nip = OffsetToChar(self, event->xbutton.x);
    AdjustPoint(self, nip);
    self->text.cursor_x = CharToOffset(self, nip);
    self->text.highstart = self->text.highend = nip;
    self->text.select_start = self->text.select_end = nip;
    if (needredraw)
	Redisplay(w, NULL, NULL);
    else
        DrawCursor(w);
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
    TextLineWidget      self = (TextLineWidget) w;
    int                 nip;

    _XpwDisArmClue(w);
    if (event == NULL)
	return;
   /* Compute character offset of mouse pointer */
    nip = OffsetToChar(self, event->xbutton.x);
   /* If mouse has moved at less than one character, leave */
   /* Redisplay new selected characters */
    if (self->text.select_end != nip) {
	self->text.select_end = nip;
	if (self->text.select_start < self->text.select_end) {
	    self->text.highstart = self->text.select_start;
	    self->text.highend = self->text.select_end;
	} else {
	    self->text.highstart = self->text.select_end;
	    self->text.highend = self->text.select_start;
	}
	AdjustPoint(self, nip);
       /* Update cursor */
	self->text.cursor_x = CharToOffset(self, nip);
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
    TextLineWidget      self = (TextLineWidget) w;
    int                 nip;
    int                 len;
    char               *str;

    _XpwDisArmClue(w);
    if (event == NULL)
	return;
   /* Remove cursor if shown */
   /* Compute character offset of mouse pointer */
    nip = OffsetToChar(self, event->xbutton.x);
   /* Redraw cursor */
    if (self->text.select_end != nip) {
       /* Readjust gap */
	AdjustPoint(self, nip);
	self->text.cursor_x = CharToOffset(self, nip);
	self->text.select_end = nip;
	if (ReCenterCursor(w) == FALSE)
	    Redisplay(w, NULL, NULL);
    }
    len = self->text.highend - self->text.highstart;
    if (len > 0 && !self->text.passmode) {
	self->text.select_size = len;
       /* Remove the old one */
	if (self->text.select_text)
	    XtFree(self->text.select_text);
	self->text.select_text = XtMalloc(len + 1);
	str = &self->text.string[self->text.highstart];
	strncpy(self->text.select_text, str, len);

	XtOwnSelection(w, XA_PRIMARY, event->xbutton.time,
		       ConvertSelection, LoseSelection, NULL);
	XChangeProperty(XtDisplay(w), DefaultRootWindow(XtDisplay(w)),
			XA_CUT_BUFFER0, XA_STRING, 8, PropModeReplace,
			(unsigned char *) self->text.select_text, len);

    }
    DrawCursor(w);
}

/* SelectWord() - ACTION
 *
 * Select current word.
 */
/* ARGSUSED */
static void
SelectWord(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    TextLineWidget      self = (TextLineWidget) w;
    int                 nip;
    int                 len;
    char               *str;

    _XpwDisArmClue(w);
    if (event == NULL)
	return;
    if (self->text.passmode) {
	SelectLine(w, event, params, num_params);
	return;
    }
   /* Remove cursor if shown */
   /* Compute character offset of mouse pointer */
    nip = OffsetToChar(self, event->xbutton.x);
   /* Redraw cursor */
   /* Readjust gap */
    AdjustPoint(self, nip);
    self->text.cursor_x = CharToOffset(self, nip);
   /* Find start of current word. */
    str = &self->text.string[self->text.insert_point];
    self->text.highstart = self->text.insert_point;
    for (; self->text.highstart >= 0 && *str == ' '; self->text.highstart--, str--) ;
    for (; self->text.highstart >= 0 && *str != ' '; self->text.highstart--, str--) ;
    self->text.highstart++;
    str++;
    self->text.highend = self->text.select_start = self->text.highstart;
   /* Find end of current word */
    for (; self->text.highend < self->text.stringsize && *str != ' '; self->text.highend++, str++) ;
    self->text.select_end = self->text.highend;
    if (ReCenterCursor(w) == FALSE)
	Redisplay(w, NULL, NULL);
    len = self->text.highend - self->text.highstart;
    if (len > 0) {
	self->text.select_size = len;
       /* Remove the old one */
	if (self->text.select_text)
	    XtFree(self->text.select_text);
	self->text.select_text = XtMalloc(len + 1);
	str = &self->text.string[self->text.highstart];
	strncpy(self->text.select_text, str, len);

	XtOwnSelection(w, XA_PRIMARY, event->xbutton.time,
		       ConvertSelection, LoseSelection, NULL);
	XChangeProperty(XtDisplay(w), DefaultRootWindow(XtDisplay(w)),
			XA_CUT_BUFFER0, XA_STRING, 8, PropModeReplace,
			(unsigned char *) self->text.select_text, len);

    }
    DrawCursor(w);
}

/* SelectLine() - ACTION
 *
 * Select whole buffer.
 */
/* ARGSUSED */
static void
SelectLine(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    TextLineWidget      self = (TextLineWidget) w;
    int                 nip;
    int                 len;
    char               *str;

    _XpwDisArmClue(w);
    if (event == NULL)
	return;
   /* Remove cursor if shown */
   /* Compute character offset of mouse pointer */
    nip = OffsetToChar(self, event->xbutton.x);
   /* Redraw cursor */
   /* Readjust gap */
    AdjustPoint(self, nip);
    self->text.cursor_x = CharToOffset(self, nip);
    self->text.highstart = 0;
    self->text.highend = self->text.stringsize;
    if (ReCenterCursor(w) == FALSE)
	Redisplay(w, NULL, NULL);
    self->text.select_start = 0;
    self->text.select_end = self->text.stringsize;
    len = self->text.highend - self->text.highstart;
    if (len > 0 && !self->text.passmode) {
	self->text.select_size = len;
       /* Remove the old one */
	if (self->text.select_text)
	    XtFree(self->text.select_text);
	self->text.select_text = XtMalloc(len + 1);
	str = &self->text.string[self->text.highstart];
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
    TextLineWidget      self = (TextLineWidget) w;
    int                 nip;

    _XpwDisArmClue(w);
   /* Check if editable */
    if (!self->text.editable)
	return;

   /* Remove cursor */
    EraseCursor(w);
   /* Compute charater offset of mouse pointer */
    nip = OffsetToChar(self, event->xbutton.x);
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
    TextLineWidget      self = (TextLineWidget) w;

    _XpwDisArmClue(w);
   /* Check if editable */
    if (!self->text.editable)
	return;

   /* Remove cursor */
    EraseCursor(w);
   /* If there is a selection, clear selection and mark for full redraw */
    ClearHighlight(self);
   /* Grow gap to include remainder of the buffer */
    if (self->text.insert_point >= 0)
	ShrinkBuffer(self, self->text.stringsize - self->text.insert_point);
   /* Redraw cursor */
    if (ReCenterCursor(w) == FALSE)
	Redisplay(w, NULL, NULL);
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
    TextLineWidget      self = (TextLineWidget) w;
    int                 len, left;
    char               *p;

    _XpwDisArmClue(w);
   /* Check if editable */
    if (!self->text.editable)
	return;

   /* Remove cursor */
    EraseCursor(w);
   /* If there is a selection, clear selection and mark for full redraw */
    ClearHighlight(self);
   /* Grow gap to include next word */
    if (self->text.insert_point >= 0) {
	len = 0;
	p = &self->text.string[self->text.insert_point];
       /* Grab all leading spaces */
	for (left = self->text.stringsize - self->text.insert_point;
	     left > 0 && *p == ' '; left--, p++, len++) ;
       /* Now grab the word */
	for (; left > 0 && *p != ' ' && *p != '\0'; left--, p++, len++) ;
	ShrinkBuffer(self, len);
    }
   /* Decide if display needs to be readjusted */
    if (ReCenterCursor(w) == FALSE)
	Redisplay(w, NULL, NULL);
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
XpwTextLineGetEditable(w)
	Widget              w;
{
    TextLineWidget      self = (TextLineWidget) w;

    return self->text.editable;
}

void
XpwTextLineSetEditable(w, editable)
	Widget              w;
	Boolean             editable;
{
    TextLineWidget      self = (TextLineWidget) w;

    self->text.editable = editable;
}

/*
 * Return the insert point.
 */
int
XpwTextLineGetInsertionPosition(w)
	Widget              w;
{
    TextLineWidget      self = (TextLineWidget) w;

    return self->text.insert_point;
}

/*
 * Set the insert point.
 */
void
XpwTextLineSetInsertionPosition(w, pos)
	Widget              w;
	int                 pos;
{
    TextLineWidget      self = (TextLineWidget) w;

    ClearHighlight(self);

   /* Move to insert Point */
    AdjustPoint(self, pos);

   /* Reposition cursor */
    self->text.cursor_x = CharToOffset(self, self->text.insert_point);
    if (ReCenterCursor(w) == FALSE)
	Redisplay(w, NULL, NULL);
}

/*
 * Return the contents of the text buffer. It is users responsablity to
 * free the string when done.
 */
char               *
XpwTextLineGetString(w)
	Widget              w;
{
    TextLineWidget      self = (TextLineWidget) w;

    return XtNewString(self->text.string);
}

/*
 * Set the buffer to string.
 */
void
XpwTextLineSetString(w, str)
	Widget              w;
	char               *str;
{
    TextLineWidget      self = (TextLineWidget) w;

    if (str == NULL)
	return;

    ClearHighlight(self);

   /* Remove all storage */
    XtFree(self->text.string);
   /* Build free new buffer */
    self->text.string = XtNewString(str);
    self->text.stringsize = strlen(str);
    self->text.insert_point = self->text.stringsize;

   /* Reposition cursor */
    self->text.cursor_x = CharToOffset(self, self->text.insert_point);
    if (ReCenterCursor(w) == FALSE)
	Redisplay(w, NULL, NULL);
}

/*
 * Insert string at the insert point.
 */
void
XpwTextLineInsert(w, pos, str)
	Widget              w;
	int                 pos;
	char               *str;
{
    TextLineWidget      self = (TextLineWidget) w;
    int                 len;
    char               *p;

    ClearHighlight(self);

   /* Move to insert Point */
    AdjustPoint(self, pos);

   /* Sanity Check */
    if (str != NULL && (len = strlen(str)) > 0) {
       /* Grow buffer if need be */
	GrowBuffer(self, len);
       /* Copy text into buffer. */
	p = &self->text.string[self->text.insert_point];
       /* Adjust offsets */
	self->text.insert_point += len;
	while (len-- > 0)
	    *p++ = *str++;
    }
   /* Reposition cursor */
    self->text.cursor_x = CharToOffset(self, self->text.insert_point);
    if (ReCenterCursor(w) == FALSE)
	Redisplay(w, NULL, NULL);
}

/*
 * Replace a chunk of text.
 */
void
XpwTextLineReplace(w, first, last, str)
	Widget              w;
	int                 first;
	int                 last;
	char               *str;
{
    TextLineWidget      self = (TextLineWidget) w;
    int                 len;

    if (first > last)
	return;

    ClearHighlight(self);
   /* Move to insert point */
    AdjustPoint(self, first);

    if (last > self->text.stringsize)
	last = self->text.stringsize;

    len = last - first;
    if (len > 0)
	ShrinkBuffer(self, len);
    if (str != NULL && (len = strlen(str)) > 0) {
	char               *p;

       /* Grow buffer if need be */
	GrowBuffer(self, len);
       /* Copy text into buffer. */
	p = &self->text.string[self->text.insert_point];
       /* Adjust offsets */
	self->text.insert_point += len;
	while (len-- > 0)
	    *p++ = *str++;

    }
   /* Reposition cursor */
    self->text.cursor_x = CharToOffset(self, self->text.insert_point);
    if (ReCenterCursor(w) == FALSE)
	Redisplay(w, NULL, NULL);
}

/*
 * Set the highlight and move it to the cut buffer.
 */
void
XpwTextLineSetSelection(w, start, end)
	Widget              w;
	int                 start;
	int                 end;
{
    TextLineWidget      self = (TextLineWidget) w;
    int                 len;
    Time                t;

    ClearHighlight(self);

    if (start < 0 || start == end)
	return;

    if (end > self->text.stringsize)
	end = self->text.stringsize;

    self->text.highstart = self->text.select_start = start;
    self->text.highend = self->text.select_end = start;
    AdjustPoint(self, end);
    self->text.cursor_x = CharToOffset(self, end);
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
	str = &self->text.string[self->text.highstart];
	strncpy(self->text.select_text, str, len);

	time(&t);
	XtOwnSelection(w, XA_PRIMARY, t,
		       ConvertSelection, LoseSelection, NULL);
	XChangeProperty(XtDisplay(w), DefaultRootWindow(XtDisplay(w)),
			XA_CUT_BUFFER0, XA_STRING, 8, PropModeReplace,
			(unsigned char *) self->text.select_text, len);

    }
}
