/*
 * MainWindow class.
 *
 * Handles the bulk of the user interface to CD player.
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
 * $Id: mainwindow.h,v 1.2 1998/01/26 01:02:01 rich Exp rich $
 *
 * $Log: mainwindow.h,v $
 * Revision 1.2  1998/01/26 01:02:01  rich
 * Support for stacker drives.
 *
 * Revision 1.1  1997/12/16 05:48:46  rich
 * Initial revision
 *
 *
 */

enum Timemode {Elapsed=1, Remaining, Disc, Total};
enum State    {CDStop, CDPlay, CDPause};

class MainWindow
{
   private:
     XtAppContext context;
     Widget	toplevel;	/* Toplevel of display */
     Widget	manager;	/* Main Manager */
     Widget	drives;		/* Drives menu */
     Widget	toolbar;	/* Toolbar */
     Widget	status;		/* Make status function. */
     Widget	volume;		/* Volume control */
     Widget     title;		/* CD Label */
     Widget	track;		/* Track information */
     Widget	infobar;	/* Status bar */
     Widget	pausebut;
     Widget	toolplay;	/* Radio groups to update */
     Widget	menuplay;
     Widget	dialog;
     Widget	drivegrp;
     Timemode	timemode;
     Widget	tooltime;
     Widget	menutime;
     XtIntervalId	timer;	/* Update Timer */
     Player	player;		/* Cd Player object */
     PlayList   list;
     TrackList	*tlist[MAXDRIVES+1]; /* List of track names */
     int	curdrive;	/* Current drive */
     int	curtrack;	/* Current track */
     State	cdstate;
     int	playing;	/* Currently playing */
     int 	repeat;		/* Repeat mode */
     int 	intro;		/* Intro mode */
     int	changed;
   public:
     MainWindow(Widget top, XtAppContext app_con);
     ~MainWindow();

   private:
	/* static callback functions */
	static void hideshow(Widget, XtPointer, XtPointer);
	static void play(Widget, XtPointer, XtPointer);
	static void playtrack(Widget, XtPointer, XtPointer);
	static void stop(Widget, XtPointer, XtPointer);
	static void pause(Widget, XtPointer, XtPointer);
	static void skipb(Widget, XtPointer, XtPointer);
	static void skipf(Widget, XtPointer, XtPointer);
	static void fb(Widget, XtPointer, XtPointer);
	static void ff(Widget, XtPointer, XtPointer);
	static void eject(Widget, XtPointer, XtPointer);
	static void options(Widget, XtPointer, XtPointer);
	static void settimemode(Widget, XtPointer, XtPointer);
	static void setplaymode(Widget, XtPointer, XtPointer);
	static void setintromode(Widget, XtPointer, XtPointer);
	static void setrepeatmode(Widget, XtPointer, XtPointer);
	static void setvolume(Widget, XtPointer, XtPointer);
	static void editTrack(Widget, XtPointer, XtPointer);
	static void Update(XtPointer, XtIntervalId *);
	static void editapply(Widget, XtPointer, XtPointer);
	static void editreset(Widget, XtPointer, XtPointer);
	static void editok(Widget, XtPointer, XtPointer);
	static void saveok(Widget, XtPointer, XtPointer);
	static void noticedone(Widget, XtPointer, XtPointer);
	static void checkint(Widget, XtPointer, XtPointer);
	static void checkpath(Widget, XtPointer, XtPointer);
	static void dialogdone(Widget, XtPointer, XtPointer);

        /* General methods */
	void UpdateOptions();
        void buildtracklist();
        void updatetracklist();
	void hideshow(char *, int);
	void settimemode(Timemode);
	void setstate(State);
	void setdrive(int);
	void Update();
	void editOptions();
	void editsave(Widget);
};
