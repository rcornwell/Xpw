/*
 * List widget.
 *
 * Manage a list of items.
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
 * $Log: List.c,v $
 * Revision 1.4  1998/01/24 20:43:27  rich
 * Don't realize scrollbars if we are not realized.
 * Fixed errors in XpwListDeleteItems.
 * When replacing items if string is same, don't update.
 * XpwGetItem now returns text string, if pointer is non-null.
 *
 * Revision 1.3  1997/11/01 06:39:03  rich
 * Removed unused variables.
 *
 * Revision 1.2  1997/10/15 05:52:03  rich
 * Don't need to destroy scrollbars, composite does it for us.
 *
 * Revision 1.1  1997/10/09 02:40:35  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: List.c,v 1.4 1998/01/24 20:43:27 rich Exp $";

#endif

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Misc.h>
#include <X11/Xmu/Drawing.h>
#include <X11/Shell.h>
#include <X11/CoreP.h>
#include "XpwInit.h"
#include "ListP.h"
#include "Clue.h"
#include "Scroller.h"

#define abs(x)  (((x)<0)?-(x):(x))

/* Our resources */
#define offset(field) XtOffsetOf(ListRec, list.field)
static XtResource   resources[] =
{
    {XtNonlyOne, XtCOnlyOne, XtRBoolean, sizeof(Boolean),
     offset(only_one), XtRImmediate, (XtPointer) FALSE},
    {XtNalwaysNotify, XtCAlwaysNotify, XtRBoolean, sizeof(Boolean),
     offset(always_notify), XtRImmediate, (XtPointer) FALSE},
    {XtNforceColumns, XtCForceColumns, XtRBoolean, sizeof(Boolean),
     offset(forcedcols), XtRImmediate, (XtPointer) FALSE},
    {XtNdefaultColumns, XtCColumns, XtRInt, sizeof(int),
     offset(ncols), XtRImmediate, (XtPointer) 2},
    {XtNlongest, XtCLongest, XtRInt, sizeof(int),
     offset(longest), XtRImmediate, (XtPointer) 0},
    {XtNcolumnSpacing, XtCSpacing, XtRInt, sizeof(int),
     offset(colspace), XtRImmediate, (XtPointer) 6},
    {XtNscrollOnTop, XtCScrollOnTop, XtRBoolean, sizeof(Boolean),
     offset(v_scroll_ontop), XtRImmediate, (XtPointer) FALSE},
    {XtNscrollOnLeft, XtCScrollOnLeft, XtRBoolean, sizeof(Boolean),
     offset(h_scroll_onleft), XtRImmediate, (XtPointer) FALSE},
    {XtNvScroll, XtCVScroll, XtRBoolean, sizeof(Boolean),
     offset(use_v_scroll), XtRImmediate, (XtPointer) FALSE},
    {XtNhScroll, XtCHScroll, XtRBoolean, sizeof(Boolean),
     offset(use_h_scroll), XtRImmediate, (XtPointer) FALSE},
    {XtNrowSpacing, XtCSpacing, XtRInt, sizeof(int),
     offset(rowspace), XtRImmediate, (XtPointer) 6},
    {XtNinternational, XtCInternational, XtRBoolean, sizeof(Boolean),
     offset(international), XtRImmediate, (XtPointer) FALSE},
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
     offset(foreground), XtRString, XtDefaultForeground},
    {XtNtopColor, XtCTopColor, XtRPixel, sizeof(Pixel),
     offset(topcolor), XtRString, XtDefaultBackground},
    {XtNmarkColor, XtCMarkColor, XtRPixel, sizeof(Pixel),
     offset(markColor), XtRString, XtDefaultForeground},
    {XtNscrollBorder, XtCScrollBorder, XtRInt, sizeof(int),
     offset(scrollborder), XtRImmediate, (XtPointer) 4},
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
    {XtNcolor1, XtCListColor, XtRPixel, sizeof(Pixel),
     offset(color[1]), XtRString, XtDefaultForeground},
    {XtNcolor2, XtCListColor, XtRPixel, sizeof(Pixel),
     offset(color[2]), XtRString, XtDefaultForeground},
    {XtNcolor3, XtCListColor, XtRPixel, sizeof(Pixel),
     offset(color[3]), XtRString, XtDefaultForeground},
    {XtNcolor4, XtCListColor, XtRPixel, sizeof(Pixel),
     offset(color[4]), XtRString, XtDefaultForeground},
    {XtNcolor5, XtCListColor, XtRPixel, sizeof(Pixel),
     offset(color[5]), XtRString, XtDefaultForeground},
    {XtNcolor6, XtCListColor, XtRPixel, sizeof(Pixel),
     offset(color[6]), XtRString, XtDefaultForeground},
    {XtNcolor7, XtCListColor, XtRPixel, sizeof(Pixel),
     offset(color[7]), XtRString, XtDefaultForeground},
    {XtNmarkShape, XtCShapeType, XtRShapeType, sizeof(ShapeType),
     offset(markShape), XtRImmediate, (XtPointer) XaNone},
    {XtNmarkSize, XtCMarkSize, XtRDimension, sizeof(Dimension),
     offset(markSize), XtRImmediate, (XtPointer) 0},
    {XtNclue, XtCLabel, XtRString, sizeof(String),
     offset(clue), XtRImmediate, (XtPointer) NULL},
    {XtNcallback, XtCCallback, XtRCallback, sizeof(XtPointer),
     offset(callbacks), XtRCallback, (XtPointer) NULL},
    {XtNmultiCallback, XtCCallback, XtRCallback, sizeof(XtPointer),
     offset(multicallbacks), XtRCallback, (XtPointer) NULL},

    /* ThreeD resouces */
    threeDresources

};

#undef offset

/* Semi Public Functions */

static void         Initialize(Widget /*request */ , Widget /*new */ ,
			  ArgList /*args */ , Cardinal * /*num_args */ );
static Boolean      SetValues(Widget /*current */ , Widget /*request */ ,
			      Widget /*new */ , ArgList /*args */ ,
			      Cardinal * /*num_args */ );
static XtGeometryResult QueryGeometry(Widget /*w */ ,
				      XtWidgetGeometry * /*intended */ ,
				    XtWidgetGeometry * /*return_val */ );
static void         Resize(Widget /*w */ );
static void         Redisplay(Widget /*wid */ , XEvent * /*event */ ,
			      Region /*region */ );
static void         Destroy(Widget /*w */ );

/* Private Functions */
static void         CalculateLongest(ListWidget /*self */ );
static void         GrowList(ListWidget /*self */ , int /*size */ ,
			     int /*ip */ );
static void         Layout(ListWidget /*self */ , Dimension * /*width */ ,
			   Dimension * /*height */ , int /*set */ );
static void         HandleScroll(Widget /*wid */ , XtPointer /*client_data */ ,
				 XtPointer /*call_data */ );
static int          GetItem(Widget /* w */ , XEvent * /* event */ );
static void         CenterItem(Widget /*wid */ , int /*item */ );
static void	    UpdateRange(Widget /*wid */ , int /*nitem */ , char /*mode*/);
static void         MoveWindow(Widget /*wid */ , int /*nxoff */ , int /*nyoff */ );
static void         RedisplayItem(ListWidget /*self */ , int /*item */ );
static void         CreateGCs(Widget /*w */ );
static void         DestroyGCs(Widget /*w */ );

/* Actions */
static void         Toggle(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         Unset(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         Set(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         Notify(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         Enter(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         Leave(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         MoveMouse(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         PageDown(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         PageUp(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         DownLine(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         UpLine(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         RightCol(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         LeftCol(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         EndRow(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );
static void         HomeRow(Widget /*w */ , XEvent * /*event */ ,
		     String * /*params */ , Cardinal * /*num_params */ );

static XtActionsRec actionsList[] =
{
    {"Enter", Enter},
    {"Leave", Leave},
    {"Set", Set},
    {"Notify", Notify},
    {"Unset", Unset},
    {"Toggle", Toggle},
    {"MoveMouse", MoveMouse},
    {"PageDown", PageDown},
    {"PageUp", PageUp},
    {"DownLine", DownLine},
    {"UpLine", UpLine},
    {"RightCol", RightCol},
    {"LeftCol", LeftCol},
    {"EndRow", EndRow},
    {"HomeRow", HomeRow},
};

/* Translations */
static char         defaultTranslations[] =
"<EnterWindow>:     Enter()\n\
 <LeaveWindow>:     Leave()\n\
    <Btn1Down>:     Toggle()\n\
  <Btn1Motion>:     MoveMouse(Toggle)\n\
    <Btn2Down>:     Unset()\n\
  <Btn2Motion>:     MoveMouse(Unset)\n\
    <Btn3Down>:     Set()\n\
  <Btn3Motion>:     MoveMouse(Set)\n\
       <BtnUp>:     Notify()\n\
     <Key>Down:     DownLine()\n\
       <Key>Up:     UpLine()\n\
    <Key>Right:     RightCol()\n\
     <Key>Left:     LeftCol()\n\
  <Key>Page_Up:     PageUp()\n\
<Key>Page_Down:     PageDown()\n\
     <Key>Home:     HomeRow()\n\
      <Key>End:     EndRow()\n\
";

#define SuperClass ((WidgetClass) &compositeClassRec)

ListClassRec        listClassRec =
{
    {	/* core_class part */
	(WidgetClass) SuperClass,	/* superclass            */
	"List",				/* class_name            */
	sizeof(ListRec),		/* widget_size           */
	XpwInitializeWidgetSet,		/* class_initialize      */
	NULL,				/* class_part_initialize */
	FALSE,				/* class_inited          */
	Initialize,			/* initialize            */
	NULL,				/* initialize_hook       */
	XtInheritRealize,		/* realize               */
	actionsList,			/* actions               */
	XtNumber(actionsList),		/* num_actions           */
	resources,			/* resources             */
	XtNumber(resources),		/* num_resources         */
	NULLQUARK,			/* xrm_class             */
	TRUE,				/* compres_motion        */
	TRUE,				/* compress_exposure     */
	TRUE,				/* compress_enterleave   */
	TRUE,				/* visible_interest      */
	Destroy,			/* destroy               */
	Resize,				/* resize                */
	Redisplay,			/* expose                */
	SetValues,			/* set_values            */
	NULL,				/* set_values_hook       */
	XtInheritSetValuesAlmost,	/* set_values_almost     */
	NULL,				/* get_values+hook       */
	XtInheritAcceptFocus,		/* accept_focus          */
	XtVersion,			/* version               */
	NULL,				/* callback_private      */
	defaultTranslations,		/* tm_table              */
	QueryGeometry,			/* query_geometry        */
	XtInheritDisplayAccelerator,	/* display_acceleator    */
	NULL				/* extension             */
    },
    {	/* composite part */
	XtInheritGeometryManager,	/* geometry_manager   */
	XtInheritChangeManaged,		/* change_managed     */
	XtInheritInsertChild,		/* insert_child       */
	XtInheritDeleteChild,		/* delete_child       */
	NULL				/* extension          */
    },
    {	/* list_class part */
	0,
    },
};

WidgetClass         listWidgetClass = (WidgetClass) & listClassRec;

/* Freedoms mask */
#define LK_WIDTH	0x01	/* Lock width */
#define LK_HEIGHT	0x02	/* Lock height */
#define LK_ROWS		0x04	/* Lock rows */
#define LK_COLS		0x08	/* Lock cols */
#define LK_ITEMS	0x10	/* Lock items */
#define LK_LONGEST	0x20	/* Lock longest */

/************************************************************
 *
 * Semi-Public interface functions
 *
 ************************************************************/

/* 
 * Initialize the list class.
 */

/* ARGSUSED */
static void
Initialize(request, new, args, num_args)
	Widget              request, new;
	ArgList             args;
	Cardinal           *num_args;
{
    ListWidget          nself = (ListWidget) new;
    String             *ls, lb[2];
    Dimension           w, h;

   /* Clear values based on freedoms mask */
    nself->list.freedoms = 0;
    if (nself->list.forcedcols)
	nself->list.freedoms |= LK_COLS;
    if (nself->list.longest != 0)
	nself->list.freedoms |= LK_LONGEST;
    if (nself->core.width != 0)
	nself->list.freedoms |= LK_WIDTH;
    if (nself->core.height != 0)
	nself->list.freedoms |= LK_HEIGHT;
    nself->list.h_scrollbar = NULL;
    nself->list.v_scrollbar = NULL;
    nself->list.do_redisplay = FALSE;
    nself->list.first_item = -1;
    nself->list.last_item = -1;
    nself->list.color[0] = nself->list.foreground;

   /* Make a empty list */
    ls = &lb[0];
    lb[0] = XtName(new);
    lb[1] = NULL;
    nself->list.nrows = 0;
    nself->list.nitems = 0;
    nself->list.selected = -1;
    nself->list.list = NULL;
    nself->list.list_attr = NULL;
    XpwListAddItems(new, ls, -1, 0, FALSE, FALSE);

   /* Set up 3D stuff */
    _XpwThreeDInit(new, &nself->list.threeD, nself->core.background_pixel);
    CreateGCs(new);

   /* Initialize width and height */
    w = nself->core.width;
    h = nself->core.height;
    if (w == 0 || h == 0) {
	Layout(nself, &w, &h, FALSE);
	nself->core.width = w + 2 * nself->list.threeD.shadow_width;
	nself->core.height = h + 2 * nself->list.threeD.shadow_width;
    }
    nself->list.do_redisplay = TRUE;

}

/*
 * SetValues: Figure out if we need to redisplay becuase of the changes,
 * also allocate any new GC's as required.
 */

/* ARGSUSED */
static              Boolean
SetValues(current, request, new, args, num_args)
	Widget              current, request, new;
	ArgList             args;
	Cardinal           *num_args;
{
    ListWidget          self = (ListWidget) new;
    ListWidget          old_self = (ListWidget) current;
    int                 i;
    Boolean             ret_val = FALSE;
    Boolean             relayout = FALSE;

    if (self->core.sensitive != old_self->core.sensitive)
	ret_val = TRUE;
    if (_XpmThreeDSetValues(new, &old_self->list.threeD,
		       &self->list.threeD, new->core.background_pixel)) {
	_XpwThreeDDestroyShadow(current, &old_self->list.threeD);
	_XpwThreeDAllocateShadowGC(new, &self->list.threeD,
				   new->core.background_pixel);
	ret_val = TRUE;
    }
    if (old_self->list.longest != self->list.longest) {
	if (self->list.longest == 0)
	    self->list.freedoms |= LK_LONGEST;
	else
	    self->list.freedoms &= ~LK_LONGEST;
	relayout = TRUE;
	ret_val = TRUE;
    }
    if ((old_self->list.forcedcols != self->list.forcedcols)) {
	if (self->list.forcedcols == 0)
	    self->list.freedoms |= LK_COLS;
	else
	    self->list.freedoms &= ~LK_COLS;
	relayout = TRUE;
	ret_val = TRUE;
    }
    if ((old_self->list.markShape != self->list.markShape) ||
	(old_self->list.markSize != self->list.markSize)) {
	relayout = TRUE;
	ret_val = TRUE;
    }
    if ((old_self->list.ncols != self->list.ncols) ||
	(old_self->list.nrows != self->list.nrows)) {
       /* Move back to home if we changed number of columns or rows */
	self->list.xoff = 0;
	if (self->list.h_scrollbar != NULL)
	    XpwScrollerSetPosition(self->list.h_scrollbar, 0);
	self->list.yoff = 0;
	if (self->list.v_scrollbar != NULL)
	    XpwScrollerSetPosition(self->list.v_scrollbar, 0);
    }
    if ((old_self->list.ncols != self->list.ncols) ||
	(old_self->list.nrows != self->list.nrows) ||
	(old_self->list.colspace != self->list.colspace) ||
	(old_self->list.rowspace != self->list.rowspace) ||
	(old_self->list.v_scroll_ontop != self->list.v_scroll_ontop) ||
	(old_self->list.h_scroll_onleft != self->list.h_scroll_onleft)) {
	relayout = TRUE;
    }
    if ((old_self->list.foreground != self->list.foreground) ||
      (old_self->core.background_pixel != self->core.background_pixel) ||
	(old_self->list.topcolor != self->list.topcolor) ||
	(old_self->list.markColor != self->list.markColor)) {
	self->list.color[0] = self->list.foreground;
	DestroyGCs(current);
	CreateGCs(new);
	ret_val = TRUE;
    }
   /* Check to see if any colors changed */
    if (!ret_val) {
	for (i = 1; i <= 7; i++)
	    if (old_self->list.color[i] != self->list.color[i])
		ret_val = TRUE;
    }
    if (relayout) {
	CalculateLongest(self);
	Layout(self, &self->core.width, &self->core.height, TRUE);
	ret_val = TRUE;
    }
    return (ret_val);
}

/*
 * Calculate preferred size, given constraining box, caching it in the widget.
 */

static              XtGeometryResult
QueryGeometry(w, intended, return_val)
	Widget              w;
	XtWidgetGeometry   *intended, *return_val;
{
    ListWidget          self = (ListWidget) w;
    Dimension           width, height;
    XtGeometryResult    ret_val = XtGeometryYes;
    XtGeometryMask      mode = intended->request_mode;

   /* Compute the default size */
    width = 0;
    height = 0;
    Layout(self, &width, &height, FALSE);
    width += 2 * self->list.threeD.shadow_width;
    height += 2 * self->list.threeD.shadow_width;

    if (((mode & CWWidth) && (intended->width < width)) || !(mode & CWWidth)) {
	return_val->request_mode |= CWWidth;
	return_val->width = width;
	ret_val = XtGeometryAlmost;
    }
    if (((mode & CWHeight) && (intended->height < height)) || !(mode & CWHeight)) {
	return_val->request_mode |= CWHeight;
	return_val->height = height;
	ret_val = XtGeometryAlmost;
    }
    if (ret_val == XtGeometryAlmost) {
	mode = return_val->request_mode;

	if (((mode & CWWidth) && (width == self->core.width)) &&
	    ((mode & CWHeight) && (height == self->core.height))) {
	    return (XtGeometryNo);
	}
    }
    return (ret_val);
}

/*
 * Handle resizes.
 */
static void
Resize(w)
	Widget              w;
{
    ListWidget          self = (ListWidget) w;
    Dimension           width = self->core.width;
    Dimension           height = self->core.height;

    Layout(self, &width, &height, TRUE);
   /* Move back to home if we changed number of columns or rows */
    self->list.xoff = 0;
    if (self->list.h_scrollbar != NULL)
	XpwScrollerSetPosition(self->list.h_scrollbar, 0);
    self->list.yoff = 0;
    if (self->list.v_scrollbar != NULL)
	XpwScrollerSetPosition(self->list.v_scrollbar, 0);
}

#define inrange(x, l, h)	(((x)<(l))?(l):(((x)>(h))?(h):(x)))

/*
 * Redisplay whole widget.
 */

static void
Redisplay(wid, event, region)
	Widget              wid;
	XEvent             *event;
	Region              region;
{
    ListWidget          self = (ListWidget) wid;
    int                 font_ascent = 0, font_descent = 0, x_loc, y_loc;
    Display            *dpy = XtDisplayOfObject(wid);
    Window              win = XtWindowOfObject(wid);
    XFontSetExtents    *ext;
    Dimension           s = self->list.threeD.shadow_width;
    Dimension           h = self->core.height - 2 * s;
    Dimension           w = self->core.width - 2 * s;
    int                 sr, sc;	/* Starting row and column */
    int                 er, ec;	/* Ending row and column */
    int                 i, j, l;
    int                 rowh;
    int                 tstart = 0;
    GC                  gc;
    XGCValues           values;
    XRectangle          cliparea;

    if (!XtIsRealized(wid))
	return;

    if (self->list.markShape != XaNone && self->list.markSize != 0)
	tstart = self->list.markSize;

    cliparea.x = cliparea.y = s;
    if (self->list.v_scrollbar != NULL) {
	h -= self->list.v_scrollbar->core.height +
	    self->list.scrollborder;
	if (self->list.v_scroll_ontop) {
	    cliparea.y += self->list.v_scrollbar->core.height +
		self->list.scrollborder;
	}
    }
    if (self->list.h_scrollbar != NULL) {
	w -= self->list.h_scrollbar->core.width +
	    self->list.scrollborder;
	if (self->list.h_scroll_onleft) {
	    cliparea.x += self->list.h_scrollbar->core.width +
		self->list.scrollborder;
	}
    }
    cliparea.width = w;
    cliparea.height = h;

    if (region == NULL)
	XClearWindow(dpy, win);

    rowh = self->list.tallest + self->list.rowspace;
    l = self->list.longest + self->list.colspace;
   /* Figure what needs to be redrawn */
    sc = self->list.xoff / l;
    sr = self->list.yoff / rowh;
    ec = 1 + (w + self->list.xoff) / l;
    er = 1 + (h + self->list.yoff) / rowh;

    sc = inrange(sc, 0, self->list.ncols);
    ec = inrange(ec, 0, self->list.ncols);
    sr = inrange(sr, 0, self->list.nrows);
    er = inrange(er, 0, self->list.nrows);

    if (sc == ec)
	ec++;
    if (sr == er)
	er++;
    for (i = sr; i <= er; i++) {
	for (j = sc; j < ec; j++) {
	    int                 len;
	    String              str;
	    int                 x;
	    _XpwList_attr       *la;

	    if (i > self->list.nrows || j > self->list.ncols)
		continue;
	    y_loc = (i * rowh - self->list.yoff) + cliparea.y;
	    x_loc = (j * l - self->list.xoff) + cliparea.x;

	   /* Check of we need to draw this */
	    if (region != NULL && XRectInRegion(region, x_loc, y_loc,
			       self->list.longest, rowh) == RectangleOut)
		continue;

	   /* Set up to draw it */
	    x = (i * self->list.ncols) + j;
	    if (x < 0 || x >= self->list.nitems) {
		str = "";
		la = &self->list.list_attr[self->list.nitems];
	    } else {
		str = self->list.list[x];
		la = &self->list.list_attr[x];
	    }
	    len = strlen(str);
	    if (self->list.international == True) {
		ext = XExtentsOfFontSet(self->list.fontset[la->font]);
		font_ascent = abs(ext->max_ink_extent.y);
		font_descent = ext->max_ink_extent.height - font_ascent;
	    } else {		/*else, compute size from font like R5 */
		font_ascent = self->list.font[la->font]->max_bounds.ascent;
		font_descent = self->list.font[la->font]->max_bounds.descent;
	    }

	    if (self->core.sensitive) {
		gc = (la->selected) ? self->list.rev_gc :
		    self->list.norm_gc;
	    } else {
		gc = (la->selected) ? self->list.rev_gray_gc :
		    self->list.gray_gc;
	    }

	    y_loc += ((int) rowh - (font_ascent + font_descent)) / 2 +
		font_ascent;
	    XSetClipRectangles(dpy, gc, 0, 0, &cliparea, 1, YXBanded);

	   /* Now fix the background */
	    if (la->selected) {
		GC                  xgc = (self->core.sensitive) ? self->list.norm_gc :
		self->list.gray_gc;

		XSetClipRectangles(dpy, xgc, 0, 0, &cliparea, 1, YXBanded);
		XFillRectangle(dpy, win, xgc,
			     x_loc, y_loc - (font_ascent + font_descent),
		       self->list.longest, (font_ascent + font_descent));
	    } else {
		int                 x = x_loc;
		int                 y = y_loc - (font_ascent + font_descent);
		int                 w = self->list.longest;
		int                 h = (font_ascent + font_descent);

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
		values.foreground = self->list.color[la->color];
		XChangeGC(dpy, gc, GCForeground, &values);
	    }

	   /* Draw the string */
	    if (self->list.international == True)
		XmbDrawString(dpy, win, self->list.fontset[la->font], gc,
			      x_loc + (self->list.colspace / 2) + tstart, y_loc, str, len);
	    else {
		values.font = self->list.font[la->font]->fid;
		XChangeGC(dpy, gc, GCFont, &values);
		XDrawString(dpy, win, gc,
		       x_loc + (self->list.colspace / 2) + tstart, y_loc,
			    str, len);
	    }

	    if (!la->selected) {
		values.foreground = self->list.foreground;
		XChangeGC(dpy, gc, GCForeground, &values);
	    }
	   /* Add modifiers */
	    if (self->list.international)
		len = XmbTextEscapement(self->list.fontset[la->font],
					str, len);
	    else
		len = XTextWidth(self->list.font[la->font], str, len);
	    if (la->underline)
		XDrawLine(dpy, win, gc, x_loc + tstart, y_loc, x_loc + tstart + len, y_loc);
	    if (la->crossout) {
		int                 offset = (font_ascent + font_descent) / 2;

		XDrawLine(dpy, win, gc, x_loc + tstart, y_loc - offset,
			  x_loc + tstart + len, y_loc - offset);
	    }
	    if (la->outline)
		XDrawRectangle(dpy, win, gc,
			       x_loc - 1,
			       y_loc - rowh + 2,
			       self->list.longest + 2,
			       rowh + 1);
	    if (self->list.markShape != XaNone &&
		self->list.markSize != 0 && la->shape)
		_XpwThreeDDrawShape(wid, event, region,
				&(self->list.threeD), self->list.mark_gc,
		self->list.markShape, x_loc, y_loc - self->list.markSize,
		 self->list.markSize, self->list.markSize, la->selected);

	}
    }

   /* Draw shadows around main window */
    x_loc = y_loc = 0;
    h = self->core.height;
    w = self->core.width;
    if (self->list.v_scrollbar != NULL) {
	int                 sh = self->list.v_scrollbar->core.height
	+ self->list.scrollborder;

	if (self->list.v_scroll_ontop) {
	    XFillRectangle(dpy, win, self->list.top_gc, 0, 0, w, sh);
	    y_loc += sh;
	} else {
	    XFillRectangle(dpy, win, self->list.top_gc, 0, h - sh, w, sh);
	}
    }
    if (self->list.h_scrollbar != NULL) {
	int                 sw = self->list.h_scrollbar->core.width
	+ self->list.scrollborder;

	if (self->list.h_scroll_onleft) {
	    XFillRectangle(dpy, win, self->list.top_gc, 0, 0, sw, h);
	    x_loc += sw;
	} else {
	    XFillRectangle(dpy, win, self->list.top_gc, w - sw, 0, sw, h);
	}
	w -= sw;
    }
    if (self->list.v_scrollbar != NULL) {
	h -= self->list.v_scrollbar->core.height + self->list.scrollborder;
    }
    _XpwThreeDDrawShadow(wid, event, region, &(self->list.threeD), x_loc, y_loc,
			 w, h, TRUE);
}

/*
 * Destroy an resources we allocated.
 */
static void
Destroy(w)
	Widget              w;
{
    ListWidget          self = (ListWidget) w;

    DestroyGCs(w);
   /* Free all elements from old list */
    if (self->list.list != NULL) {
	int                 i;
	String		    *lp;

	for (i = 0, lp = self->list.list; i < self->list.nitems; i++, lp++)
	    if (*lp != NULL)
		XtFree(*lp);
	XtFree((XtPointer) self->list.list);
    }
    if (self->list.list_attr != NULL)
	XtFree((XtPointer) self->list.list_attr);
    _XpwThreeDDestroyShadow(w, &(self->list.threeD));
}

/************************************************************
 *
 * Private functions
 *
 ************************************************************/

/*
 * Calculate the longest member of the list.
 */
static void
CalculateLongest(self)
	ListWidget          self;
{
    int                 longest, i, len, font, tallest, rowh;
    _XpwList_attr       *la;
    String		*lp;

    longest = 0;
    tallest = 0;

   /* Calculate the hieght of each font */
    for (font = 0; font < 4; font++) {
	if (self->list.international == True) {
	    XFontSetExtents    *ext = XExtentsOfFontSet(
					       self->list.fontset[font]);

	    rowh = ext->max_ink_extent.height;
	} else {
	    rowh = (self->list.font[font]->max_bounds.ascent +
		    self->list.font[font]->max_bounds.descent);
	}
	if (rowh > tallest)
	    tallest = rowh;
    }
    if (self->list.markSize == 0)
	self->list.markSize = tallest;
    else if (self->list.markSize > tallest)
	tallest = self->list.markSize;

    self->list.tallest = tallest;

   /* Scan list elements computing lenght of each */
    lp = self->list.list;
    la = self->list.list_attr;
    for (i = 0; i < self->list.nitems; i++, lp++, la++) {
	len = strlen(*lp);
	font = la->font;
	if (self->list.international)
	    len = XmbTextEscapement(self->list.fontset[font], *lp, len);
	else
	    len = XTextWidth(self->list.font[font], *lp, len);

	if (len > longest)
	    longest = len;
    }
    if (self->list.markShape != XaNone && self->list.markSize != 0)
	longest += self->list.markSize;
    self->list.longest = longest;

}

/*
 * Add new items to a list.
 */
static void
GrowList(self, size, ip)
	ListWidget          self;
	int                 size;
	int                 ip;
{
    int                 i, j;

   /* Grow arrays */
    self->list.list = (String *) XtRealloc((XtPointer) self->list.list,
		      (self->list.nitems + size + 1) * sizeof(String *));
    self->list.list_attr = (_XpwList_attr *) XtRealloc(
					(XtPointer) self->list.list_attr,
		  (self->list.nitems + size + 1) * sizeof(_XpwList_attr));
   /* Move old items to make space */
    j = self->list.nitems + size;
    for (i = self->list.nitems; i >= ip; i--, j--) {
	self->list.list[j] = self->list.list[i];
	self->list.list_attr[j] = self->list.list_attr[i];
    }
   /* Make new ones as empty */
    for (i = ip, j = size; j > 0; j--, i++)
	self->list.list[i] = NULL;
   /* Adjust size */
    self->list.nitems += size;
}

/*
 * Layout, return desired width and height. If set is TRUE, set rows and
 * columns and make scrollbars as needed.
 *
 * Work the following: First figure out how many rows and columns we need
 *                       based on the setting of nrows & ncols.
 *                     Next determine how big the window would need to be
 *                       to hold that size list.
 *                     
 */

static void
Layout(self, width, height, set)
	ListWidget          self;
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
    Boolean             needwscroll = self->list.use_h_scroll;
    Boolean             needhscroll = self->list.use_v_scroll;

   /* Grab current settings if doing prefered size */
    if (w == 0 && self->core.width != 0)
	w = self->core.width - 2 * self->list.threeD.shadow_width;
    if (h == 0 && self->core.height != 0)
	h = self->core.height - 2 * self->list.threeD.shadow_width;
    rowh = self->list.tallest + self->list.rowspace;
    l = self->list.longest + self->list.colspace;

   /* Figure out how many rows and columns we need to represent this list */
    if ((self->list.freedoms & (LK_COLS | LK_ROWS)) == (LK_COLS | LK_ROWS)
	&& self->list.nrows != 0 && self->list.ncols != 0) {
	cols = self->list.ncols;
	rows = self->list.nrows;
       /* Sanity check, if more items, grow number of rows */
	if ((rows * cols) < self->list.nitems)
	    rows = self->list.nitems / self->list.ncols;
    } else if (self->list.freedoms & LK_COLS && self->list.ncols != 0) {
	cols = self->list.ncols;
	rows = self->list.nitems / self->list.ncols;
    } else if (self->list.freedoms & LK_ROWS && self->list.nrows != 0) {
	rows = self->list.nrows;
	cols = self->list.nitems / self->list.nrows;
    } else {
	if (self->list.ncols != 0) {
	    cols = self->list.ncols;
	    rows = self->list.nitems / cols;
	} else if (self->list.nrows != 0) {
	    rows = self->list.nrows;
	    cols = self->list.nitems / rows;
	} else {		/* Default is long and thin */
	    cols = 1;
	    rows = self->list.nitems;
	}
    }
   /* Now figure out how big this will be in pixels */
    if (w == 0 && h == 0) {
	w = cols * l;
	h = rows * rowh;
    } else if (w != 0 && h == 0) {
	if (cols > (w / l) &&
	    (self->list.freedoms & LK_COLS) == 0)
	    cols = (w / l);
	if (cols == 0)
	    cols = 1;
	if ((self->list.freedoms & LK_ROWS) == 0)
	    rows = self->list.nitems / cols;
	else if (rows > (self->list.nitems / cols))
	    needhscroll = TRUE;
	h = rows * rowh;
    } else if (w == 0 && h != 0) {
	if (rows > (h / rowh) &&
	    (self->list.freedoms & LK_ROWS) == 0)
	    rows = h / rowh;
	if (rows == 0)
	    rows = 1;
	if ((self->list.freedoms & LK_COLS) == 0)
	    cols = self->list.nitems / rows;
	else if (cols > (self->list.nitems / rows))
	    needwscroll = TRUE;
	w = cols * l;
    } else {			/* w != 0 && h != 0 */
       /* Fit into space given! */
	if (w < (l * cols)) {
	    needwscroll = TRUE;
	}
	if (h < (rowh * rows)) {
	    needhscroll = TRUE;
	}
    }
   /* If not force, save these for later */
    if ((self->list.freedoms & LK_COLS) == 0)
	self->list.ncols = cols;
    if ((self->list.freedoms & LK_ROWS) == 0)
	self->list.nrows = rows;
   /* If were doing it for real then make the scroll bars if needed */
    if (set) {
	int                 sh, sw;
	int                 th, tw;
	int                 lsize, tsize;

      redo:
	if (needhscroll) {
	    if (self->list.h_scrollbar == NULL) {
		self->list.h_scrollbar = XtVaCreateManagedWidget(
			   "hscroll", scrollerWidgetClass, (Widget) self,
				 XtNorientation, XtorientVertical, NULL);

		XtAddCallback(self->list.h_scrollbar,
			      XtNcallback, HandleScroll, self);
		if (XtIsRealized((Widget)self))
		    XtRealizeWidget(self->list.h_scrollbar);
	    }
	    lsize = self->list.h_scrollbar->core.width;
	} else {
	    if (self->list.h_scrollbar != NULL) {
		XtDestroyWidget(self->list.h_scrollbar);
		self->list.h_scrollbar = NULL;
	    }
	    lsize = 0;
	}
	if (needwscroll) {
	    if (self->list.v_scrollbar == NULL) {
		self->list.v_scrollbar = XtVaCreateManagedWidget(
			   "vscroll", scrollerWidgetClass, (Widget) self,
			       XtNorientation, XtorientHorizontal, NULL);
		XtAddCallback(self->list.v_scrollbar,
			      XtNcallback, HandleScroll, self);
		if (XtIsRealized((Widget)self))
		    XtRealizeWidget(self->list.v_scrollbar);
	    }
	    tsize = self->list.v_scrollbar->core.height;
	} else {
	    if (self->list.v_scrollbar != NULL) {
		XtDestroyWidget(self->list.v_scrollbar);
		self->list.v_scrollbar = NULL;
	    }
	    tsize = 0;
	}
       /* See if we overflowed the other direction */
	if ((w - lsize) < (l * cols) && self->list.v_scrollbar == NULL) {
	    needwscroll = TRUE;
	    goto redo;
	}
	if ((h - tsize) < (rowh * rows) && self->list.h_scrollbar == NULL) {
	    needhscroll = TRUE;
	    goto redo;
	}
	if (self->list.h_scrollbar != NULL) {

	   /* Adjust sizes of bars */
	    sw = lsize;
	    sh = h - tsize;
	    XtConfigureWidget(self->list.h_scrollbar,
			      ((self->list.h_scroll_onleft) ? 0 : w - sw),
			      ((self->list.v_scroll_ontop) ? tsize : 0),
			      lsize, (h - tsize),
			      self->list.h_scrollbar->core.border_width);
	   /* Total height */
	    th = rowh * rows;
	   /* Remove undisplayable parts */
	    sh -= self->list.threeD.shadow_width * 2;
	    if (self->list.v_scrollbar != NULL)
		sh -= self->list.scrollborder;
	   /* Set it */
	    XpwScrollerSetThumb(self->list.h_scrollbar, th, sh);
	}
	if (self->list.v_scrollbar != NULL) {

	   /* Adjust sizes of bars */
	    sw = w - lsize;
	    sh = tsize;
	    XtConfigureWidget(self->list.v_scrollbar,
			      ((self->list.h_scroll_onleft) ? lsize : 0),
			      ((self->list.v_scroll_ontop) ? 0 : h - sh),
			      (w - lsize), tsize,
			      self->list.v_scrollbar->core.border_width);
	   /* Total Width */
	    tw = l * cols;
	   /* Remove undisplayable parts */
	    sw -= self->list.threeD.shadow_width * 2;
	    if (self->list.h_scrollbar != NULL)
		sw -= self->list.scrollborder;
	   /* Set it */
	    XpwScrollerSetThumb(self->list.v_scrollbar, tw, sw);
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
    ListWidget          self = (ListWidget) client_data;
    int                 place = (int) call_data;
    int                 nxoff = self->list.xoff;
    int                 nyoff = self->list.yoff;
    int                 rowh, l;

   /* Size of rows */
    rowh = self->list.tallest + self->list.rowspace;
    l = self->list.longest + self->list.colspace;

   /* Now do the scroll */
    if (wid == self->list.h_scrollbar)
	nyoff = place;
    if (wid == self->list.v_scrollbar)
	nxoff = place;
   /* Move the window only if there was a change. */
    if (nxoff != self->list.xoff || nyoff != self->list.yoff)
	MoveWindow((Widget) self, nxoff, nyoff);
}

/*
 * Convert a mouse event into a item index.
 */
/* ARGSUSED */
static int
GetItem(w, event)
	Widget              w;
	XEvent             *event;
{
    ListWidget          self = (ListWidget) w;
    Dimension           s = self->list.threeD.shadow_width;
    int                 x_loc, y_loc;
    int                 rowh, l;
    int                 item;

   /* If outside window return -1 */
    if (event->xbutton.x < s || event->xbutton.x > self->core.width ||
	event->xbutton.y < s || event->xbutton.y > self->core.height)
	return -1;

   /* Compute coordents to top of screen */
    x_loc = self->list.xoff - s;
    if (self->list.v_scroll_ontop && self->list.v_scrollbar != NULL)
	x_loc -= self->list.v_scrollbar->core.height + self->list.scrollborder;
    y_loc = self->list.yoff - s;
    if (self->list.h_scroll_onleft && self->list.h_scrollbar != NULL)
	x_loc -= self->list.h_scrollbar->core.height + self->list.scrollborder;

   /* Add in mouse hit */
    x_loc += event->xbutton.x;
    y_loc += event->xbutton.y;

   /* Compute size of cells */
    rowh = self->list.tallest + self->list.rowspace;
    l = self->list.longest + self->list.colspace;

   /* Convert into item column */
    y_loc = y_loc / rowh;
    x_loc = x_loc / l;
    item = (y_loc * self->list.ncols) + x_loc;
    if (item >= 0 && item < self->list.nitems)
	return item;
    else
	return -1;
}

/*
 * Center the display on a item.
 */
static void
CenterItem(wid, item)
	Widget              wid;
	int                 item;
{
    ListWidget          self = (ListWidget) wid;
    int                 nxoff, nyoff;
    Dimension           s = self->list.threeD.shadow_width;
    Dimension           h = self->core.height - 2 * s;
    Dimension           w = self->core.width - 2 * s;
    int                 i, j, x_loc, y_loc;
    int                 rowh, l;

    if (!XtIsRealized(wid))
	return;
   /* Convert item into x,y offset */
    if (item < 0 || item >= self->list.nitems)
	return;

   /* Build a clip mask */
    if (self->list.v_scrollbar != NULL) {
	h -= self->list.v_scrollbar->core.height + self->list.scrollborder;
    }
    if (self->list.h_scrollbar != NULL) {
	w -= self->list.h_scrollbar->core.width + self->list.scrollborder;
    }
    i = item / self->list.ncols;
    j = item % self->list.ncols;

    rowh = self->list.tallest + self->list.rowspace;
    l = self->list.longest + self->list.colspace;
    y_loc = ((i * rowh) - self->list.yoff);
    x_loc = ((j * l) - self->list.xoff);
   /* Check if ah_eady fully shown */
    if ((y_loc >= 0 && (y_loc + rowh) <= h) &&
	(x_loc >= 0 && (x_loc + l) <= w))
	return;
   /* Compute new xoff and yoff */
    if (x_loc < 0 || (x_loc + l) > w) {
	int                 max;

	nxoff = (j * l) - (w / 2);
	if (nxoff < 0)
	    nxoff = 0;
	if ((max = w / l) == 0)
	    max = 1;
       /* Check if would not show full screen */
	if (((j + max) > self->list.ncols))
	    nxoff = (self->list.ncols - max) * l;
    } else
	nxoff = self->list.xoff;

    if (y_loc < 0 || (y_loc + rowh) > h) {
	int                 max;

	nyoff = (i * rowh) - (h / 2);
	if (nyoff < 0) {
	    nyoff = 0;
	}
	if ((max = h / rowh) == 0)
	    max = 1;
       /* Check if would not show full screen */
	if (((i + max) > self->list.nrows))
	    nyoff = (self->list.nrows - max) * rowh;
    } else
	nyoff = self->list.yoff;

   /* Adjust the scrollbars if any */
    if (self->list.h_scrollbar != NULL)
	XpwScrollerSetPosition(self->list.h_scrollbar, nyoff);
    if (self->list.v_scrollbar != NULL)
	XpwScrollerSetPosition(self->list.v_scrollbar, nxoff);
   /* Now reposition the window */
    MoveWindow((Widget) self, nxoff, nyoff);

}

#define min(a, b)	(((a) < (b)) ? (a) : (b))
#define max(a, b)	(((a) < (b)) ? (b) : (a))
/*
 * Update block.
 */
static void
UpdateRange(wid, nitem, mode)
	Widget              wid;
	int                 nitem;
	char                mode;
{
    ListWidget          self = (ListWidget) wid;
    int                 fr, fc, lr, lc, nr, nc;
    int                 sr, sc, er, ec;
    int                 si, ei;
    Boolean             prev;

   /* Crude solution, but will work. 
    * We start at smallest item and go to the largest item.
    * if the item is in the range first to new we apply function.
    * otherwise we set back to original. At any point if we change
    * a value redisplay the item.
    */

   /* Convert pointer to row/col */
    fr = self->list.first_item / self->list.ncols;
    fc = self->list.first_item % self->list.ncols;
    lr = self->list.last_item / self->list.ncols;
    lc = self->list.last_item % self->list.ncols;
    nr = nitem / self->list.ncols;
    nc = nitem % self->list.ncols;

   /* Find extents of range */
    sr = min(min(fr, lr), nr);
    sc = min(min(fc, lc), nc);
    er = max(max(fr, lr), nr);
    ec = max(max(fc, lc), nc);
    lr = max(fr, nr);
    lc = max(fc, nc);
    fr = min(fr, nr);
    fc = min(fc, nc);

    si = (sr * self->list.ncols) + sc;
    ei = (er * self->list.ncols) + ec;

    for (; si <= ei; si++) {
	int                 r, c;

       /* Convert to row col */
	r = si / self->list.ncols;
	c = si % self->list.ncols;
	prev = self->list.list_attr[si].selected;

       /* In range to operate on? */
	if (r >= fr && r <= lr && c >= fc && c <= lc) {
	    switch (mode) {
	    case 'T':
		self->list.list_attr[si].selected =
				 !self->list.list_attr[si].oldstate;
		break;
	    case 'U':
		self->list.list_attr[si].selected = FALSE;
		break;
	    case 'S':
		self->list.list_attr[si].selected = TRUE;
		break;
	    case 'O':
		self->list.list_attr[si].selected =
				 self->list.list_attr[si].oldstate;
		break;
	    }
	} else {
	   /* Set it back */
	    self->list.list_attr[si].selected =
				 self->list.list_attr[si].oldstate;
	}
	if (self->list.list_attr[si].selected != prev) {
	    RedisplayItem(self, si);
	}
    }

    self->list.last_item = nitem;
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
    ListWidget          self = (ListWidget) wid;
    int                 coff;
    int                 rowh, l;
    Display            *dpy = XtDisplayOfObject((Widget) self);
    Window              win = XtWindowOfObject((Widget) self);
    Dimension           s = self->list.threeD.shadow_width;
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
    if (self->list.v_scrollbar != NULL) {
	int                 sh;

	sh = self->list.v_scrollbar->core.height + self->list.scrollborder;
	if (self->list.v_scroll_ontop) {
	    cliparea.y += sh;
	    y += sh;
	}
	h -= sh;
    }
    if (self->list.h_scrollbar != NULL) {
	int                 sw;

	sw = self->list.h_scrollbar->core.width + self->list.scrollborder;
	if (self->list.h_scroll_onleft) {
	    cliparea.x += sw;
	    x += sw;
	}
	w -= sw;
    }
    cliparea.width = w;
    cliparea.height = h;

   /* Plane old GC */
    gc = (self->core.sensitive) ? self->list.norm_gc : self->list.gray_gc;

   /* Save Clip Mask */
    XSetClipRectangles(dpy, gc, 0, 0, &cliparea, 1, YXBanded);

   /* Size of rows */
    rowh = self->list.tallest + self->list.rowspace;
    l = self->list.longest + self->list.colspace;
   /* Now do the scroll */
    coff = self->list.yoff;
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
    self->list.yoff = nyoff;
    coff = self->list.xoff;
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
    self->list.xoff = nxoff;
    if (need_refresh)
	Redisplay((Widget) self, NULL, NULL);
}

/*
 * If selected item is showing in screen, do a clear area to force a
 * redisplay.
 */
static void
RedisplayItem(self, item)
	ListWidget          self;
	int                 item;
{
    int                 i, j, l;
    int                 rowh;
    int                 x_loc, y_loc;
    Display            *dpy = XtDisplayOfObject((Widget) self);
    Window              win = XtWindowOfObject((Widget) self);
    Dimension           s = self->list.threeD.shadow_width;

   /* Should we redisplay it */
    if (!XtIsRealized((Widget) self) || !self->list.do_redisplay ||
        item < 0 || item >= self->list.nitems)
	return;

   /* Convert to screen location */
    i = item / self->list.ncols;
    j = item % self->list.ncols;

    rowh = self->list.tallest + self->list.rowspace;
    l = self->list.longest + self->list.colspace;
    y_loc = (i * rowh - self->list.yoff) + s;
    x_loc = (j * l - self->list.xoff) + s;
    if (self->list.v_scrollbar != NULL && self->list.v_scroll_ontop) 
        y_loc += self->list.v_scrollbar->core.height + self->list.scrollborder;
   
    if (self->list.h_scrollbar != NULL && self->list.h_scroll_onleft) 
	x_loc += self->list.h_scrollbar->core.width + self->list.scrollborder;
    
   /* Check if out of range */
    if ((y_loc < 0 && (y_loc + rowh) < 0) || (y_loc > self->core.height))
	return;
    if ((x_loc < 0 && (x_loc + l) < 0) || (x_loc > self->core.width))
	return;

   /* Just clear area and let expose handle it */
    XClearArea(dpy, win, x_loc, y_loc, l, rowh, TRUE);
}

/* 
 * CreateGCs for list widget.
 */

static void
CreateGCs(w)
	Widget              w;
{
    ListWidget          self = (ListWidget) w;
    XGCValues           values;
    XtGCMask            mask;

    values.foreground = self->list.foreground;
    values.background = self->core.background_pixel;
    values.font = self->list.font[0]->fid;
    values.graphics_exposures = FALSE;
    mask = GCForeground | GCBackground | GCGraphicsExposures | GCFont;

    self->list.norm_gc = XtAllocateGC(w, 0, mask, &values, GCForeground | GCFont, 0);

    values.foreground = self->core.background_pixel;
    values.background = self->list.foreground;
    values.font = self->list.font[0]->fid;
    values.graphics_exposures = FALSE;
    mask = GCForeground | GCBackground | GCGraphicsExposures | GCFont;

    self->list.rev_gc = XtAllocateGC(w, 0, mask, &values, GCForeground | GCFont, 0);

    values.fill_style = FillTiled;
    values.tile = XmuCreateStippledPixmap(XtScreenOfObject(w),
    self->list.foreground, self->core.background_pixel, self->core.depth);
    values.graphics_exposures = FALSE;
    mask |= GCTile | GCFillStyle;
    self->list.gray_gc = XtAllocateGC(w, 0, mask, &values, GCForeground | GCFont, 0);

    values.fill_style = FillTiled;
    values.tile = XmuCreateStippledPixmap(XtScreenOfObject(w),
    self->core.background_pixel, self->list.foreground, self->core.depth);
    values.graphics_exposures = FALSE;
    mask |= GCTile | GCFillStyle;
    self->list.rev_gray_gc = XtAllocateGC(w, 0, mask, &values, GCForeground | GCFont, 0);

    values.foreground = self->list.topcolor;
    values.background = self->core.background_pixel;
    mask = GCForeground | GCBackground;

    self->list.top_gc = XtGetGC(w, mask, &values);

    values.foreground = self->list.markColor;
    values.background = self->core.background_pixel;
    mask = GCForeground | GCBackground;

    self->list.mark_gc = XtGetGC(w, mask, &values);
}

/*
 * DestroyGC's we created.
 */
static void
DestroyGCs(w)
	Widget              w;
{
    ListWidget          self = (ListWidget) w;

    XtReleaseGC(w, self->list.norm_gc);
    XtReleaseGC(w, self->list.gray_gc);
    XtReleaseGC(w, self->list.rev_gc);
    XtReleaseGC(w, self->list.rev_gray_gc);
    XtReleaseGC(w, self->list.top_gc);
    XtReleaseGC(w, self->list.mark_gc);
}

/************************************************************
 *
 * ACTION procedures
 *
 ************************************************************/

/* ARGSUSED */
static void
Toggle(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    ListWidget          self = (ListWidget) w;
    int                 item;

    _XpwDisArmClue(w);

    if ((item = GetItem(w, event)) >= 0) {
	self->list.first_item = item;
	self->list.last_item = item;
	XpwListToggleItem(w, item, TRUE);
    }
}

static void
Unset(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    ListWidget          self = (ListWidget) w;
    int                 item;

    _XpwDisArmClue(w);

    if ((item = GetItem(w, event)) >= 0) {
	self->list.first_item = item;
	self->list.last_item = item;
	XpwListClearItem(w, item, TRUE);
    }
}

static void
Set(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    ListWidget          self = (ListWidget) w;
    int                 item;

    _XpwDisArmClue(w);

    if ((item = GetItem(w, event)) >= 0) {
	self->list.first_item = item;
	self->list.last_item = item;
	XpwListSetItem(w, item, TRUE);
    }
}

static void
MoveMouse(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    ListWidget          self = (ListWidget) w;
    int                 item;

    if (*num_params == (Cardinal) 0) {
	XtWarning("Too few parameters passed to MoveMouse action table.");
	return;
    }
    if (*num_params != (Cardinal) 1) {
	XtWarning("Too many parameters passed to MoveMouse action table.");
	return;
    }
    if ((item = GetItem(w, event)) >= 0) {
       /* If we are still at same item just leave */
	if (self->list.last_item == item)
	    return;
	if (self->list.only_one && self->list.first_item != item) {
	    int	oitem = self->list.first_item;

	   /* Reset old one */
	    if (self->list.list_attr[oitem].selected != 
			self->list.list_attr[oitem].oldstate) {
	    	self->list.list_attr[oitem].selected = 
			self->list.list_attr[oitem].oldstate;
		RedisplayItem(self, oitem);
	    }
	   /* Fix new one */
	    switch (params[0][0]) {
	    case 'T':
		self->list.list_attr[item].selected =
				 !self->list.list_attr[item].oldstate;
		break;
	    case 'U':
		self->list.list_attr[item].selected = FALSE;
		break;
	    case 'S':
		self->list.list_attr[item].selected = TRUE;
		break;
	    }
           /* Redisplay if changed */
	    if (self->list.list_attr[item].selected != 
			self->list.list_attr[item].oldstate) 
		RedisplayItem(self, item);
	    self->list.first_item = item;
	    self->list.last_item = item;
	} else 
	    UpdateRange(w, item, params[0][0]);
    }
}

/* ARGSUSED */
static void
Notify(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    ListWidget          self = (ListWidget) w;
    int                 item;
    XpwListReturnStruct ret_value;

    if ((item = GetItem(w, event)) < 0) {
	UpdateRange(w, self->list.last_item, 'O');
	return;
    }
   /* Check if doing multi callback */
    if (self->list.last_item != self->list.first_item) {
	XpwListReturnStruct *rp, *srp;
        _XpwList_attr        *la;
	String		    *lp;
	int                 i;
	int                 count = 0;

       /* Count number of items */
	la = self->list.list_attr;
	for (i = 0; i < self->list.nitems; i++, la++) {
	    if ((la->selected == TRUE && la->oldstate == FALSE) ||
		(self->list.always_notify && la->selected != la->oldstate))
		count++;
	}

       /* Build List */
	srp = (XpwListReturnStruct *) XtMalloc(
			      sizeof(XpwListReturnStruct) * (count + 1));
	la = self->list.list_attr;
	lp = self->list.list;
	rp = srp;
	for (i = 0; i < self->list.nitems; i++, la++, lp++) {
	    if ((la->selected == TRUE && la->oldstate == FALSE) ||
		(self->list.always_notify && la->selected != la->oldstate)) {
		rp->string = *lp;
		rp->index = i;
		rp++;
	    }
	    la->oldstate = la->selected;
	}
       /* Make sure list is terminated */
	rp->string = NULL;
	rp->index = -1;

	XtCallCallbacks(w, XtNmultiCallback, (XtPointer) srp);

       /* Free space since we are done */
	XtFree((XtPointer) srp);
    } else {
	self->list.list_attr[item].oldstate = self->list.list_attr[item].selected;
	if (item >= 0 &&
	    (self->list.always_notify || self->list.list_attr[item].selected)) {
	    ret_value.index = item;
	    ret_value.string = self->list.list[item];
	    XtCallCallbacks(w, XtNcallback, (XtPointer) & ret_value);
	}
    }

}

/* ARGSUSED */
static void
Enter(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    ListWidget          self = (ListWidget) w;

    _XpwArmClue(w, self->list.clue);
}

/* ARGSUSED */
static void
Leave(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    _XpwDisArmClue(w);
}

static void
PageDown(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    ListWidget          self = (ListWidget) w;
    int                 nyoff = self->list.yoff;
    int                 h, th;

   /* Do nothing if we have no scroll bar */
    if (self->list.h_scrollbar == NULL)
	return;

   /* Remove undisplayable parts */
    h = self->core.height - self->list.threeD.shadow_width * 2;
    if (self->list.v_scrollbar != NULL)
	h -= self->list.scrollborder + self->list.v_scrollbar->core.height;

   /* Total height */
    th = (self->list.tallest + self->list.rowspace) * self->list.nrows;
   /* Size of rows */
    nyoff += h;
   /* Don't move past end of list */
    if ((nyoff + h) > th)
	nyoff = th - h;
    if (nyoff < 0)
	nyoff = 0;
    if (nyoff != self->list.yoff) {
       /* Move the window. */
	MoveWindow((Widget) self, self->list.xoff, nyoff);
       /* Adjust the scrollbars if any */
	if (self->list.h_scrollbar != NULL)
	    XpwScrollerSetPosition(self->list.h_scrollbar, nyoff);
    }
}

static void
PageUp(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    ListWidget          self = (ListWidget) w;
    int                 nyoff = self->list.yoff;
    int                 h;

   /* Do nothing if we have no scroll bar */
    if (self->list.h_scrollbar == NULL)
	return;

   /* Remove undisplayable parts */
    h = self->core.height - self->list.threeD.shadow_width * 2;
    if (self->list.v_scrollbar != NULL)
	h -= self->list.scrollborder + self->list.v_scrollbar->core.height;

   /* Size of rows */
    nyoff -= h;
   /* Don't move past end of list */
    if (nyoff < 0)
	nyoff = 0;
    if (nyoff != self->list.yoff) {
       /* Move the window. */
	MoveWindow((Widget) self, self->list.xoff, nyoff);
       /* Adjust the scrollbars if any */
	if (self->list.h_scrollbar != NULL)
	    XpwScrollerSetPosition(self->list.h_scrollbar, nyoff);
    }
}

static void
DownLine(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    ListWidget          self = (ListWidget) w;
    int                 nyoff = self->list.yoff;
    int                 h, th;

   /* Do nothing if we have no scroll bar */
    if (self->list.h_scrollbar == NULL)
	return;

   /* Remove undisplayable parts */
    h = self->core.height - self->list.threeD.shadow_width * 2;
    if (self->list.v_scrollbar != NULL)
	h -= self->list.scrollborder + self->list.v_scrollbar->core.height;

   /* Total height */
    th = (self->list.tallest + self->list.rowspace) * self->list.nrows;
   /* Size of rows */
    nyoff += self->list.tallest + self->list.rowspace;
   /* Don't move past end of list */
    if ((nyoff + h) > th)
	nyoff = th - h;
    if (nyoff < 0)
	nyoff = 0;
    if (nyoff != self->list.yoff) {
       /* Move the window. */
	MoveWindow((Widget) self, self->list.xoff, nyoff);
       /* Adjust the scrollbars if any */
	if (self->list.h_scrollbar != NULL)
	    XpwScrollerSetPosition(self->list.h_scrollbar, nyoff);
    }
}

static void
UpLine(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    ListWidget          self = (ListWidget) w;
    int                 nyoff = self->list.yoff;
    int                 h;

   /* Do nothing if we have no scroll bar */
    if (self->list.h_scrollbar == NULL)
	return;

   /* Remove undisplayable parts */
    h = self->core.height - self->list.threeD.shadow_width * 2;
    if (self->list.v_scrollbar != NULL)
	h -= self->list.scrollborder + self->list.v_scrollbar->core.height;

   /* Size of rows */
    nyoff -= self->list.tallest + self->list.rowspace;
   /* Don't move past end of list */
    if (nyoff < 0)
	nyoff = 0;
    if (nyoff != self->list.yoff) {
       /* Move the window. */
	MoveWindow((Widget) self, self->list.xoff, nyoff);
       /* Adjust the scrollbars if any */
	if (self->list.h_scrollbar != NULL)
	    XpwScrollerSetPosition(self->list.h_scrollbar, nyoff);
    }
}

static void
LeftCol(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    ListWidget          self = (ListWidget) w;
    int                 nxoff = self->list.xoff;
    int                 ww;

   /* Do nothing if we have no scroll bar */
    if (self->list.v_scrollbar == NULL)
	return;

   /* Remove undisplayable parts */
    ww = self->core.width - self->list.threeD.shadow_width * 2;
    if (self->list.h_scrollbar != NULL)
	ww -= self->list.scrollborder + self->list.h_scrollbar->core.height;

   /* Move 1/2 a column size */
    nxoff -= self->list.longest / 2;
   /* Don't move past end of list */
    if (nxoff < 0)
	nxoff = 0;
    if (nxoff != self->list.xoff) {
       /* Move the window. */
	MoveWindow((Widget) self, nxoff, self->list.yoff);
       /* Adjust the scrollbars if any */
	if (self->list.v_scrollbar != NULL)
	    XpwScrollerSetPosition(self->list.v_scrollbar, nxoff);
    }
}

static void
RightCol(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    ListWidget          self = (ListWidget) w;
    int                 nxoff = self->list.xoff;
    int                 ww, tw;

   /* Do nothing if we have no scroll bar */
    if (self->list.v_scrollbar == NULL)
	return;

   /* Remove undisplayable parts */
    ww = self->core.width - self->list.threeD.shadow_width * 2;
    if (self->list.h_scrollbar != NULL)
	ww -= self->list.scrollborder + self->list.h_scrollbar->core.height;

    tw = (self->list.longest) * self->list.ncols;
   /* Size of rows */
    nxoff += self->list.longest / 2;
   /* Don't move past end of list */
    if ((nxoff + ww) > tw)
	nxoff = ww - tw;	/*return; */
    if (nxoff < 0)
	nxoff = 0;
    if (nxoff != self->list.xoff) {
       /* Move the window. */
	MoveWindow((Widget) self, nxoff, self->list.yoff);
       /* Adjust the scrollbars if any */
	if (self->list.v_scrollbar != NULL)
	    XpwScrollerSetPosition(self->list.v_scrollbar, nxoff);
    }
}

static void
HomeRow(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    ListWidget          self = (ListWidget) w;

    if (self->list.yoff == 0 && self->list.xoff == 0)
	return;
   /* Move the window. */
    MoveWindow((Widget) self, 0, 0);
   /* Adjust the scrollbars if any */
    if (self->list.h_scrollbar != NULL)
	XpwScrollerSetPosition(self->list.h_scrollbar, 0);
    if (self->list.v_scrollbar != NULL)
	XpwScrollerSetPosition(self->list.v_scrollbar, 0);
}

static void
EndRow(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;	/* unused */
	Cardinal           *num_params;		/* unused */
{
    ListWidget          self = (ListWidget) w;
    int                 nyoff = self->list.yoff;
    int                 h, th;

   /* Do nothing if we have no scroll bar */
    if (self->list.h_scrollbar == NULL)
	return;

   /* Remove undisplayable parts */
    h = self->core.height - self->list.threeD.shadow_width * 2;
    if (self->list.v_scrollbar != NULL)
	h -= self->list.scrollborder + self->list.v_scrollbar->core.height;

   /* Total height */
    th = (self->list.tallest + self->list.rowspace) * self->list.nrows;
   /* Size of rows */
    nyoff = th - h;
    if (nyoff < 0)
	nyoff = 0;
    if (nyoff != self->list.yoff) {
       /* Move the window. */
	MoveWindow((Widget) self, self->list.xoff, nyoff);
       /* Adjust the scrollbars if any */
	if (self->list.h_scrollbar != NULL)
	    XpwScrollerSetPosition(self->list.h_scrollbar, nyoff);
    }
}

/************************************************************
 *
 * Public interface functions
 *
 ************************************************************/

/*
 * Enable displaying of data.
 */
void
XpwListEnableDisplay(w)
	Widget              w;
{
    ListWidget          self = (ListWidget) w;

    if (!self->list.do_redisplay) {
	Dimension           width = self->core.width;
	Dimension           height = self->core.height;

	self->list.do_redisplay = TRUE;
       /* Relayout widget since lots could have changed */
	Layout(self, &width, &height, TRUE);
       /* Readjust */
	if ((self->list.freedoms & LK_LONGEST) == 0)
	    CalculateLongest(self);
	Redisplay(w, NULL, NULL);
    }
}

/*
 * Disable displaying of data.
 */
void
XpwListDisableDisplay(w)
	Widget              w;
{
    ListWidget          self = (ListWidget) w;

    self->list.do_redisplay = FALSE;
}

/*
 * Reset the list.
 */
void
XpwListNew(w)
	Widget              w;
{
    ListWidget          self = (ListWidget) w;

   /* Free all elements from old list */
    if (self->list.list != NULL) {
	int                 i;

	for (i = 0; i < self->list.nitems; i++)
	    if (self->list.list[i] != NULL)
		XtFree(self->list.list[i]);
    }
    if (self->list.list == NULL)
	self->list.list = (String *) XtMalloc(sizeof(String *));
    else
	self->list.list = (String *) XtRealloc((XtPointer) self->list.list, sizeof(String *));
    self->list.list[0] = NULL;
    if (self->list.list_attr == NULL)
	self->list.list_attr = (_XpwList_attr *) XtMalloc(sizeof(_XpwList_attr));
    else
	self->list.list_attr = (_XpwList_attr *) XtRealloc(
					(XtPointer) self->list.list_attr,
						   sizeof(_XpwList_attr));
    if ((self->list.freedoms & LK_LONGEST) == 0)
	self->list.longest = 0;
    self->list.nitems = 0;
    if (self->list.do_redisplay && XtIsRealized(w)) {
	Dimension           width = self->core.width;
	Dimension           height = self->core.height;

	Layout(self, &width, &height, TRUE);
	Redisplay(w, NULL, NULL);
    }
}

/*
 * Add an item to a list.
 * an after of -1 indicates at the end of the list.
 */
void
XpwListAddItem(w, item, after, font, attr, color)
	Widget              w;
	String              item;
	int                 after;
	int                 font;
	int                 attr;
	int                 color;
{
    ListWidget          self = (ListWidget) w;

   /* Now make room */
    if (after == -1)
	after = self->list.nitems;
    GrowList(self, 1, after);
    self->list.list[after] = XtNewString(item);
    self->list.list_attr[after].selected = FALSE;
    self->list.list_attr[after].oldstate = FALSE;
    self->list.list_attr[after].outline = (attr & XpwListOutline) ? 1 : 0;
    self->list.list_attr[after].underline = (attr & XpwListUnderline) ? 1 : 0;
    self->list.list_attr[after].crossout = (attr & XpwListCrossout) ? 1 : 0;
    self->list.list_attr[after].shape = (attr & XpwListShape) ? 1 : 0;
    self->list.list_attr[after].font = font;
    self->list.list_attr[after].color = color;
    if ((self->list.freedoms & LK_LONGEST) == 0)
	CalculateLongest(self);
    if (self->list.do_redisplay && XtIsRealized(w)) {
	Dimension           width = self->core.width;
	Dimension           height = self->core.height;

	Layout(self, &width, &height, TRUE);
       /* Readjust */
	Redisplay(w, NULL, NULL);
    }
}

/*
 * Add items to a list.
 * an after of -1 indicates at the end of the list.
 */
void
XpwListAddItems(w, items, after, font, attr, color)
	Widget              w;
	String             *items;
	int                 after;
	int                 font;
	int                 attr;
	int                 color;
{
    ListWidget          self = (ListWidget) w;
    int                 i;
    int                 size;

   /* Size the new items */
    for (size = 0; items[size] != NULL; size++) ;

   /* Now make room */
    if (after == -1)
	after = self->list.nitems;
    GrowList(self, size, after);
    for (i = 0; i < size; i++, after++) {
	self->list.list[after] = XtNewString(items[i]);
	self->list.list_attr[after].selected = FALSE;
	self->list.list_attr[after].oldstate = FALSE;
	self->list.list_attr[after].outline = (attr & XpwListOutline) ? 1 : 0;
	self->list.list_attr[after].underline = (attr & XpwListUnderline) ? 1 : 0;
	self->list.list_attr[after].crossout = (attr & XpwListCrossout) ? 1 : 0;
	self->list.list_attr[after].shape = (attr & XpwListShape) ? 1 : 0;
	self->list.list_attr[after].font = font;
	self->list.list_attr[after].color = color;
    }
    if ((self->list.freedoms & LK_LONGEST) == 0)
	CalculateLongest(self);
    if (self->list.do_redisplay && XtIsRealized(w)) {
	Dimension           width = self->core.width;
	Dimension           height = self->core.height;

       /* Readjust */
	Layout(self, &width, &height, TRUE);
	Redisplay(w, NULL, NULL);
    }
}

/*
 * Remove items from the list.
 */
void
XpwListDeleteItems(w, from, to)
	Widget              w;
	int                 from;
	int                 to;
{
    ListWidget          self = (ListWidget) w;
    int                 i;

    if (from < 0 || from > self->list.nitems)
	return;
    if (to < 0 || to > self->list.nitems)
	return;
    if (to < from)
	return;
   /* Remove old items */
    for (i = from; i < to; i++)
	if (self->list.list[i] != NULL)
	    XtFree(self->list.list[i]);
    while (to < self->list.nitems) {
	self->list.list[from] = self->list.list[to];
	self->list.list_attr[from] = self->list.list_attr[to];
	from++;
	to++;
    }
    self->list.nitems = from;
    if ((self->list.freedoms & LK_LONGEST) == 0)
	CalculateLongest(self);
    if (self->list.do_redisplay && XtIsRealized(w)) {
	Dimension           width = self->core.width;
	Dimension           height = self->core.height;

	Layout(self, &width, &height, TRUE);
	CenterItem(w, to);
	Redisplay(w, NULL, NULL);
    }
}

/*
 * Replace an item in the list.
 */
void
XpwListReplaceItem(w, item, index, font, attr, color)
	Widget              w;
	String              item;
	int                 index;
	int                 font;
	int                 attr;
	int                 color;
{
    ListWidget          self = (ListWidget) w;
    _XpwList_attr	*la;
    String		*lp;

    if (index < 0 || index > self->list.nitems)
	return;
    la = &self->list.list_attr[index];
    lp = &self->list.list[index];
    if (item != *lp) {
        if (*lp != NULL)
	    XtFree(*lp);
        *lp = XtNewString(item);
    }
    la->selected = FALSE;
    la->oldstate = FALSE;
    la->outline = (attr & XpwListOutline) ? 1 : 0;
    la->underline = (attr & XpwListUnderline) ? 1 : 0;
    la->crossout = (attr & XpwListCrossout) ? 1 : 0;
    la->shape = (attr & XpwListShape) ? 1 : 0;
    la->font = font;
    la->color = color;
   /* Redisplay */
    if ((self->list.freedoms & LK_LONGEST) == 0)
	CalculateLongest(self);
    if (self->list.do_redisplay && XtIsRealized(w)) {
	Dimension           width = self->core.width;
	Dimension           height = self->core.height;

	Layout(self, &width, &height, TRUE);
	Redisplay(w, NULL, NULL);
    }
}

/*
 * Replace items in the list.
 */
void
XpwListReplaceItems(w, items, index, font, attr, color)
	Widget              w;
	String             *items;
	int                 index;
	int                 font;
	int                 attr;
	int                 color;
{
    ListWidget          self = (ListWidget) w;
    int                 i;
    int                 size;
    _XpwList_attr	*la;
    String		*lp;

   /* Size the new items */
    for (size = 0; items[size] != NULL; size++) ;

   /* Make sure there is space */
    if (index < 0 || index > self->list.nitems)
	return;
   /* Truncate the replacement to the size available */
    if ((index + size) > self->list.nitems)
	size = self->list.nitems - index;
   /* Copy data. */
    la = &self->list.list_attr[index];
    lp = &self->list.list[index];
    for (i = 0; i < size; i++, la++, lp++) {
	if (*lp != items[i]) {
	    if (*lp != NULL)
	        XtFree(*lp);
	    *lp = XtNewString(items[i]);
	}
	la->selected = FALSE;
	la->oldstate = FALSE;
	la->outline = (attr & XpwListOutline) ? 1 : 0;
	la->underline = (attr & XpwListUnderline) ? 1 : 0;
	la->crossout = (attr & XpwListCrossout) ? 1 : 0;
	la->shape = (attr & XpwListShape) ? 1 : 0;
	la->font = font;
	la->color = color;
    }
    if ((self->list.freedoms & LK_LONGEST) == 0)
	CalculateLongest(self);
    if (self->list.do_redisplay && XtIsRealized(w)) {
	Dimension           width = self->core.width;
	Dimension           height = self->core.height;

       /* Redisplay */
	Layout(self, &width, &height, TRUE);
	Redisplay(w, NULL, NULL);
    }
}

/*
 * Mark an item.
 * An index of -1 indicates the last selected item.
 */
void
XpwListSetItem(w, index, adjust)
	Widget              w;
	int                 index;
	Boolean             adjust;
{
    ListWidget          self = (ListWidget) w;

   /* Make sure there is space */
    if (index < 0 || index > self->list.nitems)
	return;
    if (self->list.only_one && self->list.selected != -1
	&& self->list.selected != index) {
	self->list.list_attr[self->list.selected].selected = FALSE;
	self->list.list_attr[self->list.selected].oldstate = FALSE;
	RedisplayItem(self, self->list.selected);
    }
    self->list.list_attr[index].oldstate = self->list.list_attr[index].selected;
    self->list.list_attr[index].selected = TRUE;
    self->list.selected = index;
    if (self->list.list_attr[index].oldstate !=
	self->list.list_attr[index].selected)
	RedisplayItem(self, self->list.selected);
    if (adjust)
	CenterItem(w, index);
}

/*
 * Get state of item.
 * Returns FALSE or the current state of requested item.
 */
Boolean
XpwListGetItem(w, index, rs)
	Widget              w;
	int                 index;
        XpwListReturnStruct *rs;
{
    ListWidget          self = (ListWidget) w;

   /* Make sure there is space */
    if (index < 0 || index > self->list.nitems) {
	if (rs != NULL) {
	    rs->index = -1;
	    rs->string = NULL;
        }
	return False;
    }

    if (rs != NULL) {
    	rs->index = index;
	rs->string = self->list.list[index];
    }

    return self->list.list_attr[index].selected;
}

/*
 * Toggle an item.
 * An index of -1 indicates the last selected item.
 */
void
XpwListToggleItem(w, index, adjust)
	Widget              w;
	int                 index;
	Boolean             adjust;
{
    ListWidget          self = (ListWidget) w;
    _XpwList_attr	*la;

   /* Make sure there is space */
    if (index < 0 || index > self->list.nitems)
	return;
    if (self->list.only_one && self->list.selected != -1
	&& self->list.selected != index) {
        la = &self->list.list_attr[self->list.selected];
	la->selected = FALSE;
	la->oldstate = FALSE;
	RedisplayItem(self, self->list.selected);
    }
    la = &self->list.list_attr[index];
    la->oldstate = la->selected;
    la->selected = !la->selected;
    self->list.selected = (la->selected) ? index : -1;
    RedisplayItem(self, index);
    if (adjust)
	CenterItem(w, index);
}

/*
 * Clear selected items.
 */
void
XpwListClearItem(w, index, adjust)
	Widget              w;
	int                 index;
	Boolean             adjust;
{
    ListWidget          self = (ListWidget) w;
    _XpwList_attr        *la;

   /* Make sure there is space */
    if (index < 0 || index > self->list.nitems)
	return;
    la = &self->list.list_attr[index];
    la->selected = FALSE;
    self->list.selected = -1;
    if (la->oldstate)
	RedisplayItem(self, index);
    if (adjust)
	CenterItem(w, index);
    la->oldstate = la->selected;
}

/*
 * Clear all selected items.
 */
void
XpwListClearAll(w)
	Widget              w;
{
    ListWidget          self = (ListWidget) w;
    _XpwList_attr        *la;
    int                 i;

    la = self->list.list_attr;
    for (i = 0; i < self->list.nitems; i++, la++) {
	if (la->selected) {
	    la->selected = FALSE;
	    la->oldstate = FALSE;
	    RedisplayItem(self, i);
	}
    }
    self->list.selected = -1;
}

/*
 * Return all currently selected items.
 */
int
XpwListGetAllSelected(w, rs)
	Widget              w;
	XpwListReturnStruct **rs;
{
    ListWidget          self = (ListWidget) w;
    XpwListReturnStruct *rp;
    _XpwList_attr        *la;
    String		*lp;
    int                 i;
    int                 count = 0;

   /* Count number of items */
    la = self->list.list_attr;
    for (i = 0; i < self->list.nitems; i++, la++) 
	if (la->selected)
	    count++;

   /* If rs is not set, just return count */
    if (rs == NULL)
	return count;
   /* Build List */
    rp = (XpwListReturnStruct *) XtMalloc(
			      sizeof(XpwListReturnStruct) * (count + 1));
    *rs = rp;
    la = self->list.list_attr;
    lp = self->list.list;
    for (i = 0; i < self->list.nitems; i++, la++, lp++) {
	if (la->selected) {
	    rp->string = *lp;
	    rp->index = i;
	    rp++;
	}
    }
   /* Make sure list is terminated */
    rp->string = NULL;
    rp->index = -1;
    return count;
}

/*
 * Return currently selected item.
 */
void
XpwListGetSelected(w, rs)
	Widget              w;
	XpwListReturnStruct *rs;
{
    ListWidget          self = (ListWidget) w;

    rs->index = self->list.selected;
    if (self->list.selected >= 0 && self->list.selected < self->list.nitems)
	rs->string = self->list.list[self->list.selected];
    else
	rs->string = NULL;
}
