/*
 * XCdplay: Plays a cd.
 *
 * Code to handle main interface panel.
 *
 * $Log: $
 *
 */

#ifndef lint
static char        *rcsid = "$Id: $";
#endif

/* System stuff */
#include <stdio.h>
#include <errno.h>
#include <ctype.h>

/* Include all the widget stuff we need */
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Converters.h>
#include <Xpw/Clue.h>
#include <Xpw/Dialog.h>
#include <Xpw/List.h>
#include <Xpw/TextLine.h>
#include <Xpw/RowCol.h>
#include <Xpw/Label.h>
#include <Xpw/Command.h>
#include <Xpw/BarButton.h>
#include <Xpw/Slider.h>
#include <Xpw/Select.h>
#include <Xpw/ComboBox.h>
#include <Xpw/Tabler.h>
#include <Xpw/MenuBar.h>
#include <Xpw/PopupMenu.h>
#include <Xpw/PmeEntry.h>
#include <Xpw/PmeLine.h>
#include <Xpw/PmeSelect.h>

/* Local includes */
#include "xcdplay.h"
#include "string.h"
#include "player.h"
#include "playlist.h"
#include "tracklist.h"
#include "mainwindow.h"
#include "bitmaps.h"

/*
 * Build main player window.
 */
MainWindow::MainWindow(Widget top, XtAppContext        app_con)
{
    Arg                 args[5];
    Widget		w, w1, w2, menu;
    Pixmap              image;
    int                 i;
	toplevel = top;
	context = app_con;

   /* Build widget tree. */
    manager = XtCreateManagedWidget("manager", rowColWidgetClass, toplevel,
			NULL, 0);
    menu = XtCreateManagedWidget("menubar", menubarWidgetClass, manager,
			NULL, 0);
    /* File menu */
    XtSetArg(args[0], XtNmenuName, "filemenu");
    w = XtCreateManagedWidget("file", barbuttonWidgetClass, menu, args, 1);
    w = XtCreatePopupShell("filemenu", popupMenuWidgetClass, menu, NULL, 0);
    w1 = XtCreateManagedWidget("play", pmeEntryObjectClass, w, NULL, 0);
    XtAddCallback(w1, XtNcallback, &MainWindow::play, (XtPointer)this);
    w1 = XtCreateWidget("stop", pmeEntryObjectClass, w, NULL, 0);
    XtAddCallback(w1, XtNcallback, &MainWindow::stop, (XtPointer)this);
    w1 = XtCreateManagedWidget("edittrack", pmeEntryObjectClass, w, NULL, 0);
    XtAddCallback(w1, XtNcallback, &MainWindow::editTrack, (XtPointer)this);
    w1 = XtCreateManagedWidget("editplay", pmeEntryObjectClass, w, NULL, 0);
    XtAddCallback(w1, XtNcallback, &MainWindow::editPlay, (XtPointer)this);
    w1 = XtCreateManagedWidget("eject", pmeEntryObjectClass, w, NULL, 0);
    XtAddCallback(w1, XtNcallback, &MainWindow::eject, (XtPointer)this);
    XtCreateManagedWidget("mline", pmeLineObjectClass, w, NULL, 0);
    w1 = XtCreateManagedWidget("options", pmeEntryObjectClass, w, NULL, 0);
    XtAddCallback(w1, XtNcallback, &MainWindow::options, (XtPointer)this);
    w1 = XtCreateManagedWidget("exit", pmeEntryObjectClass, w, NULL, 0);
    XtAddCallback(w1, XtNcallback, app_quit, NULL);

    /* Display control menu */
    XtSetArg(args[0], XtNmenuName, "dispmenu");
    w = XtCreateManagedWidget("windows", barbuttonWidgetClass, menu, args, 1);
    w = XtCreatePopupShell("dispmenu", popupMenuWidgetClass, menu, NULL, 0);
    XtSetArg(args[0], XtNstate, TRUE);
    w1 = XtCreateManagedWidget("toolbar", pmeSelectObjectClass, w, args, 1);
    XtAddCallback(w1, XtNcallback, &MainWindow::hideshow, (XtPointer)this);
    w1 = XtCreateManagedWidget("status", pmeSelectObjectClass, w, args, 1);
    XtAddCallback(w1, XtNcallback, &MainWindow::hideshow, (XtPointer)this);
    w1 = XtCreateManagedWidget("volume", pmeSelectObjectClass, w, args, 1);
    XtAddCallback(w1, XtNcallback, &MainWindow::hideshow, (XtPointer)this);
    w1 = XtCreateManagedWidget("title", pmeSelectObjectClass, w, args, 1);
    XtAddCallback(w1, XtNcallback, &MainWindow::hideshow, (XtPointer)this);
    w1 = XtCreateManagedWidget("track", pmeSelectObjectClass, w, args, 1);
    XtAddCallback(w1, XtNcallback, &MainWindow::hideshow, (XtPointer)this);
    w1 = XtCreateManagedWidget("infobar", pmeSelectObjectClass, w, args, 1);
    XtAddCallback(w1, XtNcallback, &MainWindow::hideshow, (XtPointer)this);

    /* Options */
    XtSetArg(args[0], XtNmenuName, "optionmenu");
    w = XtCreateManagedWidget("option", barbuttonWidgetClass, menu, args, 1);
    w = XtCreatePopupShell("optionmenu", popupMenuWidgetClass, menu, NULL, 0);
    w1 = XtCreateManagedWidget("random", pmeSelectObjectClass, w, NULL, 0);
    XtAddCallback(w1, XtNcallback, &MainWindow::setplaymode, (XtPointer)this);
    w1 = XtCreateManagedWidget("intro", pmeSelectObjectClass, w, NULL, 0);
    XtAddCallback(w1, XtNcallback, &MainWindow::setintromode, (XtPointer)this);
    w1 = XtCreateManagedWidget("repeat", pmeSelectObjectClass, w, NULL, 0);
    XtAddCallback(w1, XtNcallback, &MainWindow::setrepeatmode, (XtPointer)this);
    XtCreateManagedWidget("mline", pmeLineObjectClass, w, NULL, 0);
    w1 = XtCreateManagedWidget("elapsed", pmeSelectObjectClass, w, NULL, 0);
    XtSetArg(args[0], XtNradioGroup, w1);
    XtSetArg(args[1], XtNradioData, (XtPointer)Elapsed);
    XtSetArg(args[2], XtNallowNone, False);
    XtSetValues(w1, args, 3);
    XtAddCallback(w1, XtNcallback, &MainWindow::settimemode, (XtPointer)this);
    menutime = w1;
    XtSetArg(args[1], XtNradioData, (XtPointer)Remaining);
    w1 = XtCreateManagedWidget("remaining", pmeSelectObjectClass, w, args, 3);
    XtAddCallback(w1, XtNcallback, &MainWindow::settimemode, (XtPointer)this);
    XtSetArg(args[1], XtNradioData, (XtPointer)Disc);
    w1 = XtCreateManagedWidget("disc", pmeSelectObjectClass, w, args, 3);
    XtAddCallback(w1, XtNcallback, &MainWindow::settimemode, (XtPointer)this);
    XtSetArg(args[1], XtNradioData, (XtPointer)Total);
    w1 = XtCreateManagedWidget("total", pmeSelectObjectClass, w, args, 3);
    XtAddCallback(w1, XtNcallback, &MainWindow::settimemode, (XtPointer)this);

    /* Toolbar */
    XtSetArg(args[0], XtNorientation, XtorientHorizontal);
    toolbar = XtCreateManagedWidget("toolbar", rowColWidgetClass, manager, args, 1);
    XtSetArg(args[0], XtNbitmap, Bitmaps[toolExit]);
    XtSetArg(args[1], XtNbitmapMask, Bitmask[toolExit]);
    w = XtCreateManagedWidget("toolExit", commandWidgetClass, toolbar, args, 2);
    XtAddCallback(w, XtNcallback, app_quit, NULL);
    XtSetArg(args[0], XtNbitmap, Bitmaps[toolEject]);
    XtSetArg(args[1], XtNbitmapMask, Bitmask[toolEject]);
    w = XtCreateManagedWidget("toolEject", commandWidgetClass, toolbar, args, 2);
    XtAddCallback(w, XtNcallback, &MainWindow::eject, (XtPointer)this);
    XtSetArg(args[0], XtNbitmap, Bitmaps[toolPlay]);
    XtSetArg(args[1], XtNbitmapMask, Bitmask[toolPlay]);
    w = XtCreateManagedWidget("toolPlay", commandWidgetClass, toolbar, args, 2);
    XtAddCallback(w, XtNcallback, &MainWindow::play, (XtPointer)this);
    XtSetArg(args[0], XtNbitmap, Bitmaps[toolStop]);
    XtSetArg(args[1], XtNbitmapMask, Bitmask[toolStop]);
    w = XtCreateWidget("toolStop", commandWidgetClass, toolbar, args, 2);
    XtAddCallback(w, XtNcallback, &MainWindow::stop, (XtPointer)this);
    XtSetArg(args[0], XtNlabel, " ");
    XtCreateManagedWidget("blank", labelWidgetClass, toolbar, args, 1);
    XtSetArg(args[0], XtNbitmap, Bitmaps[toolRandom]);
    XtSetArg(args[1], XtNbitmapMask, Bitmask[toolRandom]);
    w = XtCreateManagedWidget("toolRandom", selectWidgetClass, toolbar, args, 2);
    XtAddCallback(w, XtNcallback, &MainWindow::setplaymode, (XtPointer)this);
    XtSetArg(args[0], XtNbitmap, Bitmaps[toolRepeat]);
    XtSetArg(args[1], XtNbitmapMask, Bitmask[toolRepeat]);
    w = XtCreateManagedWidget("toolRepeat", selectWidgetClass, toolbar, args, 2);
    XtAddCallback(w, XtNcallback, &MainWindow::setrepeatmode, (XtPointer)this);
    XtSetArg(args[0], XtNbitmap, Bitmaps[toolIntro]);
    XtSetArg(args[1], XtNbitmapMask, Bitmask[toolIntro]);
    w = XtCreateManagedWidget("toolIntro", selectWidgetClass, toolbar, args, 2);
    XtAddCallback(w, XtNcallback, &MainWindow::setintromode, (XtPointer)this);
    XtSetArg(args[0], XtNlabel, " ");
    XtCreateManagedWidget("blank", labelWidgetClass, toolbar, args, 1);
    w = XtCreateManagedWidget("toolElapsed", selectWidgetClass, toolbar, NULL, 0);
    XtSetArg(args[0], XtNradioGroup, w);
    XtSetArg(args[1], XtNradioData, (XtPointer)Elapsed);
    XtSetArg(args[2], XtNallowNone, False);
    XtSetArg(args[3], XtNbitmap, Bitmaps[toolElapsed]);
    XtSetArg(args[4], XtNbitmapMask, Bitmask[toolElapsed]);
    XtSetValues(w, args, 5);
    XtAddCallback(w, XtNcallback, &MainWindow::settimemode, (XtPointer)this);
    tooltime = w;
    XtSetArg(args[1], XtNradioData, (XtPointer)Remaining);
    XtSetArg(args[3], XtNbitmap, Bitmaps[toolRemaining]);
    XtSetArg(args[4], XtNbitmapMask, Bitmask[toolRemaining]);
    w = XtCreateManagedWidget("toolRemaining", selectWidgetClass, toolbar, args, 5);
    XtAddCallback(w, XtNcallback, &MainWindow::settimemode, (XtPointer)this);
    XtSetArg(args[1], XtNradioData, (XtPointer)Disc);
    XtSetArg(args[3], XtNbitmap, Bitmaps[toolDisc]);
    XtSetArg(args[4], XtNbitmapMask, Bitmask[toolDisc]);
    w = XtCreateManagedWidget("toolDisc", selectWidgetClass, toolbar, args, 5);
    XtAddCallback(w, XtNcallback, &MainWindow::settimemode, (XtPointer)this);
    XtSetArg(args[1], XtNradioData, (XtPointer)Total);
    XtSetArg(args[3], XtNbitmap, Bitmaps[toolTotal]);
    XtSetArg(args[4], XtNbitmapMask, Bitmask[toolTotal]);
    w = XtCreateManagedWidget("toolTotal", selectWidgetClass, toolbar, args, 5);
    XtAddCallback(w, XtNcallback, &MainWindow::settimemode, (XtPointer)this);

   /* Status window, and cd control functions */
    XtSetArg(args[0], XtNorientation, XtorientHorizontal);
    status = XtCreateManagedWidget("status", rowColWidgetClass, manager, args, 1);
    XtCreateManagedWidget("mainwindow", labelWidgetClass, status, NULL, 0);
    XtSetArg(args[0], XtNbitmap, Bitmaps[playbut]);
    w1 = XtCreateManagedWidget("f1", tablerWidgetClass, status, NULL, 0);
    w2 = XtCreateManagedWidget("play", commandWidgetClass, w1, args, 1);
    XtAddCallback(w2, XtNcallback, &MainWindow::play, (XtPointer)this);
    XtSetArg(args[0], XtNbitmap, Bitmaps[stopbut]);
    w2 = XtCreateWidget("stop", commandWidgetClass, w1, args, 1);
    XtAddCallback(w2, XtNcallback, &MainWindow::stop, (XtPointer)this);
    XtSetArg(args[0], XtNbitmap, Bitmaps[pausebits]); 
    pausebut = XtCreateManagedWidget("pause", selectWidgetClass, w1, args, 1);
    XtAddCallback(pausebut, XtNcallback, &MainWindow::pause, (XtPointer)this);
    XtSetArg(args[0], XtNbitmap, Bitmaps[skipback]);
    w2 = XtCreateManagedWidget("skipback", commandWidgetClass, w1, args, 1);
    XtAddCallback(w2, XtNcallback, &MainWindow::skipb, (XtPointer)this);
    XtSetArg(args[0], XtNbitmap, Bitmaps[fastback]);
    w2 = XtCreateManagedWidget("fastback", commandWidgetClass, w1, args, 1);
    XtAddCallback(w2, XtNcallback, &MainWindow::fb, (XtPointer)this);
    XtSetArg(args[0], XtNbitmap, Bitmaps[fastforw]);
    w2 = XtCreateManagedWidget("fastforw", commandWidgetClass, w1, args, 1);
    XtAddCallback(w2, XtNcallback, &MainWindow::ff, (XtPointer)this);
    XtSetArg(args[0], XtNbitmap, Bitmaps[skipforw]);
    w2 = XtCreateManagedWidget("skipforw", commandWidgetClass, w1, args, 1);
    XtAddCallback(w2, XtNcallback, &MainWindow::skipf, (XtPointer)this);

    /* Volume control */
    XtSetArg(args[0], XtNorientation, XtorientHorizontal);
    volume = XtCreateManagedWidget("volume", sliderWidgetClass, manager, args, 1);
    XtAddCallback(volume, XtNcallback, &MainWindow::setvolume, (XtPointer)this);

    /* Disc label */
    XtSetArg(args[0], XtNlabel, "No CD");
    title = XtCreateManagedWidget("title", labelWidgetClass, manager, args, 1);

    /* Track Selector */
    track = XtCreateManagedWidget("track", comboBoxWidgetClass, manager, NULL, 0);
    XtAddCallback(track, XtNcallback, &MainWindow::playtrack, (XtPointer)this);

    /* Information bar */
    XtSetArg(args[0], XtNorientation, XtorientHorizontal);
    infobar = XtCreateManagedWidget("infobar", rowColWidgetClass, manager, args, 1);
    XtCreateManagedWidget("runtime", labelWidgetClass, infobar, NULL, 0);
    XtCreateManagedWidget("totaltime", labelWidgetClass, infobar, NULL, 0);

    /* Set up rest of object */
    settimemode(Elapsed);

    curtrack = -1;
    setstate(CDStop);
    playing = 0;
    intro = 0;
    repeat = 0;
    tlist = NULL;
    dialog = NULL;
    changed = 0;

    /* Set options */
    UpdateOptions();

#ifndef linux
    /* Make sure volume slide is correct for SUN's */
    player.setvolume(100);
    XpwSliderSetPosition(volume, 100);
#endif

    /* Make sure controls are updated */
    timer = XtAppAddTimeOut(context, 10, Update, (XtPointer)this);
}

/* Remove timer and destroy widget tree */
MainWindow::~MainWindow()
{
    XtRemoveTimeOut(timer);
    delete tlist;
    if (dialog != NULL)
        XtDestroyWidget(dialog);
    XtDestroyWidget(manager);
}

/*
 * Widget Callbacks
 */

/* Handle hide/show functions */
void
MainWindow::hideshow(Widget w, XtPointer client_data, XtPointer call_data)
{
    MainWindow * mw = (MainWindow *) client_data;

    mw->hideshow(XtName(w), (int)call_data);
}

/* Play cd. Use play mode to determin track order */
void
MainWindow::play(Widget w, XtPointer client_data, XtPointer call_data)
{
    MainWindow * mw = (MainWindow *) client_data;

    mw->playing = 1;
   /* Restart timer for quick restart */
    XtRemoveTimeOut(mw->timer);
    mw->timer = XtAppAddTimeOut(mw->context, 10, Update, (XtPointer)mw);
}

/* Play cd. Use play mode to determin track order */
void
MainWindow::playtrack(Widget w, XtPointer client_data, XtPointer call_data)
{
    MainWindow * mw = (MainWindow *) client_data;
    XpwComboBoxReturnStruct *sel = (XpwComboBoxReturnStruct *)call_data;
    Widget	wid;
    Arg		arg[1];

   /* Make sure we drop out of random mode */
    XtSetArg(arg[0], XtNstate, 0);
    if ((wid = XtNameToWidget(mw->manager, "menubar")) != NULL &&
        (wid = XtNameToWidget(wid, "optionmenu")) != NULL &&
        (wid = XtNameToWidget(wid, "random")) != NULL) 
	XtSetValues(wid, arg, 1);
    
    if ((wid = XtNameToWidget(mw->manager, "toolbar")) != NULL &&
        (wid = XtNameToWidget(wid, "toolRandom")) != NULL)
	XtSetValues(wid, arg, 1);
   
   /* Reset and point at requested track */
    mw->list.simplelist(sel->index+1);
    mw->playing = 1;
   /* Restart timer for quick restart */
    XtRemoveTimeOut(mw->timer);
    mw->timer = XtAppAddTimeOut(mw->context, 10, Update, (XtPointer)mw);
}


/* Stop the CD player */
void
MainWindow::stop(Widget w, XtPointer client_data, XtPointer call_data)
{
    MainWindow * mw = (MainWindow *) client_data;

    mw->playing = 0;
    mw->list.restart(-1);
    /* Stop the CD player */
    mw->player.stop();
}

/* Handle pause control */
void
MainWindow::pause(Widget w, XtPointer client_data, XtPointer call_data)
{
    MainWindow * mw = (MainWindow *) client_data;

    if (int(call_data) == 0) 
	mw->player.resume();
    else
	mw->player.pause();
}

void
MainWindow::skipf(Widget w, XtPointer client_data, XtPointer call_data)
{
    MainWindow * mw = (MainWindow *) client_data;

    int trk = mw->list.getnexttrk();

    if (mw->playing && trk != 0)
	mw->player.play_cd(trk, 0, (mw->intro)? resources.intro_time: 0);
    mw->curtrack = trk;
}

void
MainWindow::fb(Widget w, XtPointer client_data, XtPointer call_data)
{
    MainWindow * mw = (MainWindow *) client_data;
    int trk, tt;

    if (mw->playing) {
         (void)mw->player.getposition(trk, tt);
	 tt -= resources.skip_time;
	 if (tt < 0) 
		tt = 0;
  	 mw->player.play_cd(trk, tt, 0);
    }
}

void
MainWindow::ff(Widget w, XtPointer client_data, XtPointer call_data)
{
    MainWindow * mw = (MainWindow *) client_data;
    int trk, tt;

    if (mw->playing) {
         (void)mw->player.getposition(trk, tt);
	 tt += resources.skip_time;
	 if (tt >= mw->player.gettracklength(trk)) {
		tt = 0;
		trk = mw->list.getnexttrk();
	 }
  	 mw->player.play_cd(trk, tt, 0);
    }
}

void
MainWindow::skipb(Widget w, XtPointer client_data, XtPointer call_data)
{
    MainWindow * mw = (MainWindow *) client_data;
    int trk, curtrack, tt = 0;


    if (mw->playing) 
	(void)mw->player.getposition(trk, tt);
    
    if (tt < 10)
	trk = mw->list.getprevtrk();
    if (mw->playing && trk != 0)
	mw->player.play_cd(trk, 0, (mw->intro)? resources.intro_time: 0);
    mw->curtrack = trk;
}

void
MainWindow::editPlay(Widget w, XtPointer client_data, XtPointer call_data)
{
    MainWindow * mw = (MainWindow *) client_data;

    if (mw->tlist != NULL)
    	mw->tlist->editPlaylist();
}

void
MainWindow::editTrack(Widget w, XtPointer client_data, XtPointer call_data)
{
    MainWindow * mw = (MainWindow *) client_data;

    if (mw->tlist != NULL)
    	mw->tlist->editTrack();
}

void
MainWindow::eject(Widget w, XtPointer client_data, XtPointer call_data)
{
    MainWindow * mw = (MainWindow *) client_data;

    mw->playing = 0;
    mw->player.stop();
    mw->player.eject();
}

void
MainWindow::setvolume(Widget w, XtPointer client_data, XtPointer call_data)
{
    MainWindow * mw = (MainWindow *) client_data;

    mw->player.setvolume((int)call_data);
}

void
MainWindow::setplaymode(Widget w, XtPointer client_data, XtPointer call_data)
{
    MainWindow * mw = (MainWindow *) client_data;
    Widget	wid;
    Boolean	ns = (int)call_data;
    Arg		arg[1];

    XtSetArg(arg[0], XtNstate, ns);
    if ((wid = XtNameToWidget(mw->manager, "menubar")) != NULL &&
        (wid = XtNameToWidget(wid, "optionmenu")) != NULL &&
        (wid = XtNameToWidget(wid, "random")) != NULL) 
	XtSetValues(wid, arg, 1);
    
    if ((wid = XtNameToWidget(mw->manager, "toolbar")) != NULL &&
        (wid = XtNameToWidget(wid, "toolRandom")) != NULL)
	XtSetValues(wid, arg, 1);
   
    mw->list.setrandom((int)call_data, (mw->playing) ? mw->curtrack : -1);
    mw->curtrack = mw->list.peeknexttrk();
}

void
MainWindow::setintromode(Widget w, XtPointer client_data, XtPointer call_data)
{
    MainWindow * mw = (MainWindow *) client_data;
    Widget	wid;
    Boolean	ns = (int)call_data;
    Arg		arg[1];

    XtSetArg(arg[0], XtNstate, ns);
    if ((wid = XtNameToWidget(mw->manager, "menubar")) != NULL &&
        (wid = XtNameToWidget(wid, "optionmenu")) != NULL &&
        (wid = XtNameToWidget(wid, "intro")) != NULL) 
	XtSetValues(wid, arg, 1);
    
    if ((wid = XtNameToWidget(mw->manager, "toolbar")) != NULL &&
        (wid = XtNameToWidget(wid, "toolIntro")) != NULL)
	XtSetValues(wid, arg, 1);
   
    mw->intro = ns;
}

void
MainWindow::setrepeatmode(Widget w, XtPointer client_data, XtPointer call_data)
{
    MainWindow * mw = (MainWindow *) client_data;
    Widget	wid;
    Boolean	ns = (int)call_data;
    Arg		arg[1];

    XtSetArg(arg[0], XtNstate, ns);
    if ((wid = XtNameToWidget(mw->manager, "menubar")) != NULL &&
        (wid = XtNameToWidget(wid, "optionmenu")) != NULL &&
        (wid = XtNameToWidget(wid, "repeat")) != NULL) 
	XtSetValues(wid, arg, 1);
    
    if ((wid = XtNameToWidget(mw->manager, "toolbar")) != NULL &&
        (wid = XtNameToWidget(wid, "toolRepeat")) != NULL)
	XtSetValues(wid, arg, 1);
   
    mw->repeat = ns;
}


void
MainWindow::settimemode(Widget w, XtPointer client_data, XtPointer call_data)
{
    MainWindow * mw = (MainWindow *) client_data;
    Arg		arg[1];
    int		tm;

    if (((int)call_data) != 0) {
   /* Get new timemode */
        XtSetArg(arg[0], XtNradioData, &tm);
        XtGetValues(w, arg, 1);
        mw->settimemode(Timemode(tm));
    }
}

void
MainWindow::options(Widget w, XtPointer client_data, XtPointer call_data)
{
    MainWindow * mw = (MainWindow *) client_data;

    mw->editOptions();
}

/*
 * Load the contents of all controls.
 */
void 
MainWindow::Update(XtPointer client_data, XtIntervalId *timerid)
{
    MainWindow * mw = (MainWindow *) client_data;

    mw->Update();
}


void
MainWindow::UpdateOptions()
{
    player.setdevice(resources.cd_device); 

    /* Manage windows as needed */
    hideshow("toolbar", resources.show_toolbar);
    hideshow("status", resources.show_status);
    hideshow("volume", resources.show_volume);
    hideshow("title", resources.show_title);
    hideshow("track", resources.show_track);
    hideshow("infobar", resources.show_infobar);
}

/* Build the track list */
void
MainWindow::buildtracklist()
{
    int		ntrks = player.gettracks();
    int		*tlens = new int [ntrks + 1];

    /* Grab frame info */
    for(int i = 0; i < ntrks; i++) {
	tlens[i] = player.getframe(i);
    }

    delete tlist;
    tlist = new TrackList(player.getdiscid(), player.getlength(), ntrks,
			toplevel, tlens);

    updatetracklist();
}

/* Update the track list */
void
MainWindow::updatetracklist()
{
    int		i;
    int	        ntrks = player.gettracks();
    char	buffer[20];
    char	*p;
    Arg		arg[1];
    Widget	wid;


    list.setinfo(ntrks, tlist->getplayorder());
    if (!playing)
	list.restart(-1);

    char	**newlist = new char * [ntrks+1];
    for(i = 0; i < ntrks; i++) {
	Str s = tlist->getname(i);
	p = s.cstring();
	if (*p == '\0') {
	   sprintf(buffer, "Track %d", i+1);
	   p = buffer;
	}
	if (player.getdata(i)) {
	   p = "--Data--";
	   list.setignore(i);
	}
	newlist[i] = new char[strlen(p)+1];
	strcpy(newlist[i], p);
	list.settracklen(i, player.gettracklength(i));
    }
    newlist[i] = NULL;
    XpwComboBoxNew(track);
    XpwComboBoxAddItems(track, newlist, 0);
    for (i = 0; i < ntrks; i++) {
	if (list.getignore(i))
	   XtSetSensitive(XpwComboBoxGetMenu(track, i), False);
	delete [] newlist[i];
    }

    if (curtrack > 0)
	XpwComboBoxSetItem(track, curtrack - 1);
    if ((wid = XtNameToWidget(infobar, "totaltime")) != NULL) {
        if (curtrack < 0) {
	    buffer[0] = ' ';
	    buffer[1] = '\0';
        } else {
	    int	tm = player.getlength();
	    sprintf(buffer, "%2d:%02d", tm / 60, tm % 60);
        }
        XtSetArg(arg[0], XtNlabel, buffer);
        XtSetValues(wid, arg, 1);
    }
    if ((p = tlist->gettitle()) != NULL && *p != '\0') {
	XtSetArg(arg[0], XtNlabel, p);
	XtSetValues(title, arg, 1);
	XtSetArg(arg[0], XtNtitle, p);
	XtSetValues(toplevel, arg, 1);
    } else {
	XtSetArg(arg[0], XtNlabel, "Unknown Title");
	XtSetValues(title, arg, 1);
	XtSetArg(arg[0], XtNtitle, ProgramName);
	XtSetValues(toplevel, arg, 1);
    }
}


/* Change state of widgets to reflect state of drive */
void
MainWindow::setstate(State newstate)
{
    Widget	stop_set[3], play_set[3], wid;
    Arg		arg[1];
    int		s = 0, p = 0;
    int		i;

    /* Do nothing if in same state */
    if (newstate == cdstate)
	return;
    /* Wander over widget tree to flip all play/stop buttons */
    if ((wid = XtNameToWidget(manager, "status")) != NULL &&
	(wid = XtNameToWidget(wid, "f1")) != NULL) {
        stop_set[s] = XtNameToWidget(wid, "stop");
        play_set[s] = XtNameToWidget(wid, "play");
	s++;
    }
    if ((wid = XtNameToWidget(manager, "menubar")) != NULL &&
        (wid = XtNameToWidget(wid, "filemenu")) != NULL)  {
        stop_set[s] = XtNameToWidget(wid, "stop");
        play_set[s] = XtNameToWidget(wid, "play");
	s++;
    }
    if ((wid = XtNameToWidget(manager, "toolbar")) != NULL) {
        stop_set[s] = XtNameToWidget(wid, "toolStop");
        play_set[s] = XtNameToWidget(wid, "toolPlay");
	s++;
    }

    /* Make sure correct ones are showing */
    if (newstate == CDStop) {
	for(s--; s>=0 ; s--)
	    XtChangeManagedSet(&stop_set[s], 1, NULL, NULL, &play_set[s], 1);
        if (cdstate == CDPause)
	    XpwSelectUnsetCurrent(pausebut);
	XtSetArg(arg[0], XtNiconName, "Stopped");
	XtSetValues(toplevel, arg, 1);
    } else {
	char	buffer[10];
	if (cdstate == CDStop) {
	    for(s--; s>=0 ; s--)
	        XtChangeManagedSet(&play_set[s], 1, NULL, NULL, &stop_set[s], 1);
	}
	sprintf(buffer, "Track %d", curtrack);
	XtSetArg(arg[0], XtNiconName, buffer);
	if (newstate == CDPause) {
	    XtSetArg(arg[0], XtNiconName, "Paused");
	    XpwSelectSetCurrent(pausebut, NULL);
	}
	XtSetValues(toplevel, arg, 1);
    }
     cdstate = newstate;
}


/* Handle hide/show functions */
void
MainWindow::hideshow(String name, int state)
{
    Widget	wid;

    if ((wid = XtNameToWidget(manager, "menubar")) != NULL &&
        (wid = XtNameToWidget(wid, "dispmenu")) != NULL && 
        (wid = XtNameToWidget(wid, name)) != NULL) {
	Arg arg[1];
	Boolean ns = -1;

	XtSetArg(arg[0], XtNstate, &ns);
	XtGetValues(wid, arg, 1);
	if (ns != state) {
	    if (state)
		XpwPmeSelectSetCurrent(wid, NULL);
	    else
		XpwPmeSelectUnsetCurrent(wid);
	} 
    }

    /* Now handle the main function */
    if ((wid = XtNameToWidget(manager, name)) != NULL) {
	if (!state) 
	    XtUnmanageChild(wid);
	else
	    XtManageChild(wid);
    }
}

void
MainWindow::settimemode(Timemode tm)
{
    timemode = tm;
    XpwPmeSelectSetCurrent(menutime, (XtPointer)tm);
    XpwSelectSetCurrent(tooltime, (XtPointer)tm);
}

/*
 * Load the contents of all controls.
 */
void 
MainWindow::Update()
{
    int 	tt;
    static int	lasttime = -1000;
    char	buffer[100];
    Widget	wid;
    Arg		arg[1];
    State	newstate = CDStop;
    int		dotime = 0;
    static int	lasttrack = -1;
    Cdmode	mode;
    int		rate = resources.poll_rate;

     switch(mode = player.getposition(curtrack, tt)) {
     case Empty:
	curtrack = 1;
	sprintf(buffer, "<00> --:--");
	break;
     case New:
	buildtracklist();
        list.restart(-1);
	curtrack = -1;
	lasttrack = -2;
	XtSetArg(arg[0], XtNiconName, "Stopped");
	XtSetValues(toplevel, arg, 1);
	strcpy(buffer, "<--> --:--");
	if (resources.autoplay)
	    playing = 1;
	else
	    curtrack = list.peeknexttrk();	
     case Valid:
     case Stop:
	if (playing) {
            int trk = list.getnexttrk();

	    if (trk == 0 && repeat) {
	        list.restart(-1);
                trk = list.getnexttrk();
	    }
	    /* Move to next track if more to go */
            if (trk != 0) {
            	player.play_cd(trk, 0, (intro)?resources.intro_time: 0);
		curtrack = trk;
		newstate = CDPlay;
		tt = 0;
		dotime = 1;
	    } else {
		playing = 0;
	        list.restart(-1);
		curtrack = list.peeknexttrk();
	    }
	} 
        sprintf(buffer, "<%02d> 00:00", curtrack);
	if (newstate == CDStop)
   	    rate = resources.stop_rate;
	break;
     case Play:
        newstate = CDPlay;
	if (playing == 0 && curtrack > 0)
	   list.setcurtrk(curtrack);
	playing = 1;		/* Make sure state gets set correctly */
	dotime = 1;
	break;
     case Pause:
        newstate = CDPause;
	dotime = 1;
	break;
     case Eject:
     case Mount:
	strcpy(buffer, "<--> --:--");
	curtrack = -1;
   	rate = resources.empty_rate;
        XpwComboBoxNew(track);
	XtSetArg(arg[0], XtNiconName, "Empty");
	XtSetValues(toplevel, arg, 1);
	XtSetArg(arg[0], XtNtitle, ProgramName);
	XtSetValues(toplevel, arg, 1);
        XtSetArg(arg[0], XtNlabel, "No CD");
        XtSetValues(title, arg, 1);
	XpwComboBoxNew(track);
	delete tlist;
	tlist = NULL;
	break;
    }
    setstate(newstate);
    if (tlist != NULL && tlist->chkupded()) 
	updatetracklist();
	
    if (dotime && lasttime != tt) {
	char xmode = ' ';
	int trk = curtrack;
	int sec, min;

        switch(timemode) {
	case Elapsed:
		xmode = ' ';
		sec = tt;
		break;
	case Remaining:
		sec = player.gettracklength(curtrack-1) - tt;
		xmode = '-';
		break;
	case Disc:
		sec = list.timeplayed(curtrack) + tt;
		break;
	case Total:
		list.totaltime(sec, trk);
		break;
	}
	min = sec / 60;
	if ((sec = sec % 60) < 0)
	   sec = -sec;
	if (timemode == Disc)
	    sprintf(buffer, "    %c%2d:%02d%c", xmode, min, sec, 
				(newstate==CDPause)?'P':' ');
	else
	    sprintf(buffer, "<%02d>%c%2d:%02d%c", trk, xmode, min, sec, 
				(newstate==CDPause)?'P':' ');
    }

    if ((wid = XtNameToWidget(status, "mainwindow")) != NULL && lasttime != tt) {
        XtSetArg(arg[0], XtNlabel, buffer);
        XtSetValues(wid, arg, 1);
    }

    if ((wid = XtNameToWidget(infobar, "runtime")) != NULL && lasttime != tt) {
	int sec = tt % 60;
	if (sec < 0)
		sec = -sec;
	sprintf(buffer, "%02d:%02d", tt / 60, sec);
	XtSetArg(arg[0], XtNlabel, buffer);
	XtSetValues(wid, arg, 1);
    }

    lasttime = tt;

    if (curtrack != lasttrack) {
	XpwComboBoxSetItem(track, curtrack - 1);
        if (playing) {
	    sprintf(buffer, "Track %d", curtrack);
	    XtSetArg(arg[0], XtNiconName, buffer);
	    XtSetValues(toplevel, arg, 1);
	}
	lasttrack = curtrack;
    } 


#ifdef linux
    XpwSliderSetPosition(volume, player.getvolume());
#endif
    timer = XtAppAddTimeOut(context, rate, Update, (XtPointer)this);
}


/*
 * Make a dialog box with supported options.
 * We use a very specific nameing convention so we don't require hundreds
 * of individual callbacks. If the type is a Boolean, we build a select
 * widget and name it after the resource, otherwise we build a rowCol
 * with name of the resource + frame, and place a label with the name of
 * the resource and a textline with the name value. All textline widgets
 * are placed into the dialog focus group.
 */
void
MainWindow::editOptions()
{
    Widget      w1, tbl;
    char        **newlist;
    int         i;
    Arg         arg[2];

    if (dialog != NULL)
        return;
    dialog = XpwDialogCreateDialog(toplevel, "EditOptions", NULL, 0);
    XtUnmanageChild(XpwDialogGetChild(dialog, XpwDialog_Help_Button));
    XtAddCallback(XpwDialogGetChild(dialog, XpwDialog_Cancel_Button),
        XtNcallback, &MainWindow::dialogdone, (XtPointer)this);
    XtAddCallback(XpwDialogGetChild(dialog, XpwDialog_Ok_Button),
        XtNcallback, &MainWindow::editok, (XtPointer)this);
    w1 = XtCreateManagedWidget("apply", commandWidgetClass, dialog, NULL, 0);
    XtAddCallback(w1, XtNcallback, &MainWindow::editapply, (XtPointer)this);
    w1 = XtCreateManagedWidget("reset", commandWidgetClass, dialog, NULL, 0);
    XtAddCallback(w1, XtNcallback, &MainWindow::editreset, (XtPointer)this);
    tbl = XtCreateWidget("frame", tablerWidgetClass, dialog, NULL, 0);
    for(i = 0; default_resources[i].name != NULL; i++) {
	DefRes *rp = &default_resources[i];
	
	/* Don't add RC path. */
	if (strcmp(rp->name, "rcPath") == 0)
	    continue;
	if (strcmp(rp->type, XtRBoolean) == 0) {
    	    XtCreateManagedWidget(rp->name, selectWidgetClass, tbl, NULL, 0);
	}
	if (strcmp(rp->type, XtRInt) == 0) {
    	    Widget w;
    	    XtSetArg(arg[0], XtNorientation, XtorientHorizontal);
	    w = XtCreateManagedWidget(rp->name, rowColWidgetClass, tbl, arg, 1);
	    XtCreateManagedWidget("label", labelWidgetClass, w, NULL, 0);
    	    XtSetArg(arg[0], XtNtraversalOn, TRUE);
    	    XtSetArg(arg[1], XtNfocusGroup, dialog);
	    w1 = XtCreateManagedWidget("value", textLineWidgetClass, w, arg, 2);
    	    XtAddCallback(w1, XtNcallback, &MainWindow::checkint,
			 (XtPointer)this);
	}
	if (strcmp(rp->type, XtRString) == 0) {
    	    Widget w;
    	    XtSetArg(arg[0], XtNorientation, XtorientHorizontal);
	    w = XtCreateManagedWidget(rp->name, rowColWidgetClass, tbl, arg, 1);
	    XtCreateManagedWidget("label", labelWidgetClass, w, NULL, 0);
    	    XtSetArg(arg[0], XtNtraversalOn, TRUE);
    	    XtSetArg(arg[1], XtNfocusGroup, dialog);
	    w1 = XtCreateManagedWidget("value", textLineWidgetClass, w, arg, 2);
    	    XtAddCallback(w1, XtNcallback, &MainWindow::checkpath,
			 (XtPointer)this);
	}
    }
    /* Load defaults */
    editreset(dialog, (XtPointer)this, NULL);
    XtRealizeWidget(tbl);
    XtManageChild(tbl);
    XtManageChild(dialog);
}

void
MainWindow::editapply(Widget w, XtPointer client_data, XtPointer call_data)
{
    MainWindow    * mw = (MainWindow *)client_data;
    Widget	    wid = mw->dialog;
    Widget	    sw = XtNameToWidget(wid, "frame");
    Widget	    aw;
    char	    buffer[50];
	
    if (sw == NULL)
	return;
    for(int i = 0; default_resources[i].name != NULL; i++) {
	DefRes *rp = &default_resources[i];

	/* Find the widget */
	if ((aw = XtNameToWidget(sw, rp->name)) != NULL &&
	    (w = XtNameToWidget(aw, "value")) != NULL)
	    aw = w;
	if (aw == NULL)
	    continue;
	if (strcmp(rp->type, XtRBoolean) == 0) {
	    Boolean	ns;
	    Arg 	arg[1];

	    XtSetArg(arg[0], XtNstate, &ns);
	    XtGetValues(aw, arg, 1);
	    if (ns != *(Boolean *)rp->offset)
	        mw->changed = 1;
	    *(Boolean *)rp->offset = ns;
	} else if (strcmp(rp->type, XtRInt) == 0) {
	    int  ns;
	    char *nv, *p;

	    nv = XpwTextLineGetString(aw);
	    for(p = nv; *p == ' '; p++);
	    for(ns = 0; isdigit(*p); p++)
	        ns = (ns * 10) + (*p - '0');
	    XtFree(nv);
	    if (ns != *(int *)rp->offset)
	        mw->changed = 1;
	    *(int *)rp->offset = ns;
	} else if (strcmp(rp->type, XtRString) == 0) {
	    char *nv, *p, *p1;

	    nv = XpwTextLineGetString(aw);
	    for(p = nv; *p == ' '; p++);
	    for(p1 = p; *p1 != ' ' && *p1 != '\0'; p1++);
	    *p1 = '\0';
	    if (strcmp(*(char **)rp->offset, p) != 0) {
	        mw->changed = 1;
	        XtFree(*(char **)rp->offset);
	        *(char **)rp->offset = (char *)XtNewString(p);
	    }
	    XtFree(nv);
	}
    }
    mw->UpdateOptions();
}

void
MainWindow::editreset(Widget w, XtPointer client_data, XtPointer call_data)
{
    MainWindow    * mw = (MainWindow *)client_data;
    Widget	    wid = mw->dialog;
    Widget	    sw = XtNameToWidget(wid, "frame");
	
    if (sw == NULL)
	return;
    for(int i = 0; default_resources[i].name != NULL; i++) {
	DefRes     *rp = &default_resources[i];
        Widget	    aw;

	/* Find the widget */
	if ((aw = XtNameToWidget(sw, rp->name)) != NULL &&
	    (w = XtNameToWidget(aw, "value")) != NULL)
            aw = w;
        if (aw == NULL)
	    continue;
	if (strcmp(rp->type, XtRBoolean) == 0) {
            char	    buffer[50];
	    Arg 	    arg[1];

	    XtSetArg(arg[0], XtNstate, *(Boolean *)rp->offset);
	    XtSetValues(aw, arg, 1);
	} else if (strcmp(rp->type, XtRInt) == 0) {
	    char	buffer[20];
	    int 	v = *(int *)rp->offset;

	    sprintf(buffer, "%d", v);
	    XpwTextLineSetString(aw, buffer);
	} else if (strcmp(rp->type, XtRString) == 0) {
	    char	*p = *(char **)rp->offset;

	    XpwTextLineSetString(aw, p);
	}
    }
}

void
MainWindow::editsave(Widget parent)
{
    Widget	w;

    if (changed) {
        w = XpwDialogCreateQuestion(parent, "SaveOptions", NULL, 0);
        XtUnmanageChild(XpwDialogGetChild(w, XpwDialog_Help_Button));
        XtAddCallback(XpwDialogGetChild(w, XpwDialog_Ok_Button),
            XtNcallback, &MainWindow::saveok, (XtPointer)this);
        XpwDialogDisplay(w);
        XtDestroyWidget(w);
   }
}

void
MainWindow::editok(Widget w, XtPointer client_data, XtPointer call_data)
{
    MainWindow    * mw = (MainWindow *)client_data;

   /* Collect any changes */
    editapply(w, client_data, call_data);

   /* Build question dialog box */
    mw->editsave(mw->dialog);
    XtUnmanageChild(mw->dialog);
    XtDestroyWidget(mw->dialog);
    mw->dialog = NULL;
} 

void
MainWindow::saveok(Widget w, XtPointer client_data, XtPointer call_data)
{
    MainWindow    * mw = (MainWindow *)client_data;
    WriteRCFile();
}

void
MainWindow::noticedone(Widget w, XtPointer client_data, XtPointer call_data)
{
    XtDestroyWidget(w);
}

void
MainWindow::checkint(Widget w, XtPointer client_data, XtPointer call_data)
{
    MainWindow    * mw = (MainWindow *)client_data;
    char *nv, *p;

    nv = XpwTextLineGetString(w);
    for(p = nv; *p == ' '; p++);
    for(; isdigit(*p); p++);
    for(; *p == ' '; p++);
    if (*p != '\0') {
        Arg	arg[1];
        char	*str = NULL;
        Widget	wid;

        /* Pick up the parents name */
        XtSetArg(arg[0], XtNlabel, &str);
        if ((wid = XtNameToWidget(XtParent(w), "label")) != NULL)
            XtGetValues(wid, arg, 1);
	
        if (str == NULL)
            str = XtName(XtParent(w));
        char *msg = new char [strlen(str) + 100];
        sprintf(msg, "Attempt to set \"%s\" to a invalid number", str);
        XtSetArg(arg[0], XtNmessage, msg);
        wid = XpwDialogCreateInfo(mw->toplevel, "EntryError", arg, 1);
        XtUnmanageChild(XpwDialogGetChild(wid, XpwDialog_Help_Button));
        XtUnmanageChild(XpwDialogGetChild(wid, XpwDialog_Cancel_Button));
        XtAddCallback(XpwDialogGetChild(wid, XpwDialog_Ok_Button),
            XtNcallback, &MainWindow::noticedone, (XtPointer)NULL);
        XtManageChild(wid);
    }
    XtFree(nv);
}

void
MainWindow::checkpath(Widget w, XtPointer client_data, XtPointer call_data)
{
    MainWindow    * mw = (MainWindow *)client_data;
    char *nv, *p, *p1;

    nv = XpwTextLineGetString(w);
    for(p = nv; *p == ' '; p++);
    for(p1 = p; *p1 != ' ' && *p1 != '\0'; p1++);
    *p1 = '\0';
    if (access(p, R_OK) < 0) {
        Arg	arg[1];
        Widget	wid;

        char *msg = new char [strlen(p) + 100];
        sprintf(msg, "Path \"%s\": %s", p, strerror(errno));
        XtSetArg(arg[0], XtNmessage, msg);
        wid = XpwDialogCreateInfo(mw->toplevel, "EntryError", arg, 1);
        XtUnmanageChild(XpwDialogGetChild(wid, XpwDialog_Help_Button));
        XtUnmanageChild(XpwDialogGetChild(wid, XpwDialog_Cancel_Button));
        XtAddCallback(XpwDialogGetChild(wid, XpwDialog_Ok_Button),
        XtNcallback, &MainWindow::noticedone, (XtPointer)NULL);
        XtManageChild(wid);
    }
    XtFree(nv);
}

void
MainWindow::dialogdone(Widget w, XtPointer client_data, XtPointer call_data)
{
    MainWindow    * mw = (MainWindow *)client_data;
    if (mw->dialog != NULL) {
        XtDestroyWidget(mw->dialog);
        mw->dialog = NULL;
    }
}

