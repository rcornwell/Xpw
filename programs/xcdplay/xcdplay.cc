/*
 * XCdplay: Plays a cd.
 *
 * $Log: xcdplay.cc,v $
 * Revision 1.1  1997/12/16 05:48:46  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: xcdplay.cc,v 1.1 1997/12/16 05:48:46 rich Exp rich $";
#endif

/* System stuff */
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

/* Include all the widget stuff we need */
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <Xpw/Dialog.h>
#include <Xpw/Clue.h>
#include "xcdplay.h"
#include "string.h"
#include "player.h"
#include "playlist.h"
#include "tracklist.h"
#include "mainwindow.h"
#include "bitmaps.h"

static XrmOptionDescRec options[] =
{
    {"-device", ".cdDevice", XrmoptionSepArg, NULL},
    {"-poll", ".pollRate", XrmoptionSepArg, NULL},
};

#define Offset(field) (XtOffsetOf(XCdplayResources, field))

static XtResource   my_resources[] =
{
    {"rcPath", XtCString, XtRString, sizeof(String),
     Offset(rcpath), XtRImmediate, (XtPointer) "$HOME/.xcdplay"},
#ifdef linux
    {"cdDevice", XtCString, XtRString, sizeof(String),
     Offset(cd_device), XtRImmediate, (XtPointer) "/dev/cdrom"},
    {"mixDevice", XtCString, XtRString, sizeof(String),
     Offset(mix_device), XtRImmediate, (XtPointer) "/dev/mixer"},
#else
#if sunos
    {"cdDevice", XtCString, XtRString, sizeof(String),
     Offset(cd_device), XtRImmediate, (XtPointer) "/dev/rsr0"},
#else
    {"cdDevice", XtCString, XtRString, sizeof(String),
     Offset(cd_device), XtRImmediate, (XtPointer) "/vol/dev/aliases/cdrom0"},
#endif 
#endif
    {"cddbPath", XtCString, XtRString, sizeof(String),
     Offset(cddb_path), XtRImmediate, (XtPointer) "$HOME/.cddb"},
    {"showToolbar", XtCBoolean, XtRBoolean, sizeof(Boolean),
     Offset(show_toolbar), XtRImmediate, (XtPointer) False},
    {"showStatus", XtCBoolean, XtRBoolean, sizeof(Boolean),
     Offset(show_status), XtRImmediate, (XtPointer) TRUE},
    {"showVolume", XtCBoolean, XtRBoolean, sizeof(Boolean),
     Offset(show_volume), XtRImmediate, (XtPointer) False},
    {"showTitle", XtCBoolean, XtRBoolean, sizeof(Boolean),
     Offset(show_title), XtRImmediate, (XtPointer) TRUE},
    {"showTrack", XtCBoolean, XtRBoolean, sizeof(Boolean),
     Offset(show_track), XtRImmediate, (XtPointer) TRUE},
    {"showInfobar", XtCBoolean, XtRBoolean, sizeof(Boolean),
     Offset(show_infobar), XtRImmediate, (XtPointer) False},
    {"autoPlay", XtCBoolean, XtRBoolean, sizeof(Boolean),
     Offset(autoplay), XtRImmediate, (XtPointer) False},
    {"pollRate", "PollRate", XtRInt, sizeof(int),
     Offset(poll_rate), XtRImmediate, (XtPointer) 250},
    {"stopRate", "StopRate", XtRInt, sizeof(int),
     Offset(stop_rate), XtRImmediate, (XtPointer) 3000},
    {"emptyRate", "EmptyRate", XtRInt, sizeof(int),
     Offset(empty_rate), XtRImmediate, (XtPointer) 6000},
    {"skipTime", "SkipTime", XtRInt, sizeof(int),
     Offset(skip_time), XtRImmediate, (XtPointer) 20},
    {"introTime", "IntroTime", XtRInt, sizeof(int),
     Offset(intro_time), XtRImmediate, (XtPointer) 30},
    {"showClues", "ShowClues", XtRBoolean, sizeof(Boolean),
     Offset(show_clues), XtRImmediate, (XtPointer) TRUE},
};

#undef Offset

XCdplayResources resources;
DefRes *default_resources;

static void         usage(void);
void                main(int argc, char **argv );
static void         quit(Widget w, XEvent *event ,
			 String *params , Cardinal *num_params );
static Boolean      Quit_Proc(XtPointer client_data);
static void	    MkDefault();
static void	    removechild(Widget, XtPointer, XtPointer);
static void 	    mkcddb(Widget, XtPointer, XtPointer);
static Boolean      CvtBooleanToString(Display *dpy, XrmValue *args,
	    		 Cardinal *num_args, XrmValue *from, XrmValue *to,
			 XtPointer *convert_data);
static Boolean      CvtIntToString(Display *dpy, XrmValue *args,
	    		 Cardinal *num_args, XrmValue *from, XrmValue *to,
			 XtPointer *convert_data);

/*
 * Define the bare minimum here to make appliation look correct.
 */
static String       fallback_resources[] =
{
   /* Global Stuff */
    "XCdplay.allowShellResize:		          true",
    "XCdplay*Select.switchSize:		          0",
    "XCdplay*Select.switchShape:	          none",
    "XCdplay*status.packmode:		          fill",
    "XCdplay*toolbar*resizeToPreferred:           true",
    "XCdplay*toolbar*blank.shadowWidth:           0",
    "XCdplay*toolbar*blank.leftMargin:            1",
    "XCdplay*toolbar*blank.rightMargin:           1",
    "XCdplay*toolbar*blank.width:                 4",
    "XCdplay*toolbar*blank.resizeToPreferred:     false",
    "XCdplay*status*resizeToPreferred:     	  true",
    "XCdplay*status.mainwindow.resizeToPreferred: false",
    "XCdplay*status.mainwindow.justify:           center",
    "XCdplay*status*mainwindow.raised:            false",
    "XCdplay*status*mainwindow.resizeToPreferred: false",
    "XCdplay*status.f1.rows:    	          2",
    "XCdplay*status.f1.cols:    	          4",
    "XCdplay*status.f1.*.justify:	          center",
    "XCdplay*status.f1.play.resizeToPreferred:    false",
    "XCdplay*status.f1.play.colSpan:	          3",
    "XCdplay*status.f1.play.row:		  0",
    "XCdplay*status.f1.play.col:		  0",
    "XCdplay*status.f1.stop.resizeToPreferred:    false",
    "XCdplay*status.f1.stop.colSpan:	          3",
    "XCdplay*status.f1.stop.row:		  0",
    "XCdplay*status.f1.stop.col:		  0",
    "XCdplay*status.f1.pause.row:		  0",
    "XCdplay*status.f1.pause.col:		  3",
    "XCdplay*status.f1.pause.resizeToPreferred:   false",
    "XCdplay*status.f1.skipback.row:	          1",
    "XCdplay*status.f1.skipback.col:	          0",
    "XCdplay*status.f1.fastback.row:	          1",
    "XCdplay*status.f1.fastback.col:	          1",
    "XCdplay*status.f1.fastforw.row:	          1",
    "XCdplay*status.f1.fastforw.col:	          2",
    "XCdplay*status.f1.skipforw.row:	          1",
    "XCdplay*status.f1.skipforw.col:	          3",
    "XCdplay*track.arrow.arrowType:	          down",
    "XCdplay*track.pulldown.label:		  Select Track",
    "XCdplay*infobar.packmode: 			  fill",
    "XCdplay*infobar.allowResize:	          true",
    "XCdplay*infobar.packing:		          fill",
    "XCdplay*infobar*justify:		          center",
    "XCdplay*infobar*raised:		          false",
    "XCdplay*infobar*resizeToPreferred:	          false",
   /* Track editor */
    "XCdplay*SaveQuest*Label.shadowWidth:	  0",
    "XCdplay*SaveQuest.dialog.dialogType:	  modeless",
    "XCdplay*EditTrack*Label.shadowWidth:	  0",
    "XCdplay*EditTrack.dialog.dialogType:	  modeless",
    "XCdplay*EditTrack.dialog.autoUnmanage:	  false",
    "XCdplay*Dialog.justify:                      center",
    "XCdplay*Dialog.packing:                      even",
    "XCdplay*EditTrack*tracklist.defaultColumns:  1",
    "XCdplay*EditTrack*tracklist.height:  	  100",
    "XCdplay*EditTrack*tracklist.onlyOne:	  true",
    "XCdplay*EditPlayList.dialog.dialogType:	  modeless",
    "XCdplay*EditPlayList.dialog.autoUnmanage:	  false",
    "XCdplay*EditPlayList*tracklist.alwaysNotify: true",
    "XCdplay*EditPlayList*tracklist.row:	  0",
    "XCdplay*EditPlayList*tracklist.rowSpan:	  6", 
    "XCdplay*EditPlayList*tracklist.col:	  0",
    "XCdplay*EditPlayList*tracklist.defaultColumns: 1",
    "XCdplay*EditPlayList*tracklist.height:	  100",
    "XCdplay*EditPlayList*tracklist.width:	  150",
    "XCdplay*EditPlayList*playlist.alwaysNotify:  true",
    "XCdplay*EditPlayList*playlist.row:		  0",
    "XCdplay*EditPlayList*playlist.rowSpan:	  6", 
    "XCdplay*EditPlayList*playlist.col:		  2",
    "XCdplay*EditPlayList*playlist.defaultColumns: 1",
    "XCdplay*EditPlayList*playlist.height:	  100",
    "XCdplay*EditPlayList*playlist.width:	  150",
    "XCdplay*EditPlayList*manager.cols:		  4",
    "XCdplay*EditPlayList*manager.rows:		  6",
    "XCdplay*EditPlayList*clear.col:		  1",
    "XCdplay*EditPlayList*clear.row:		  0",
    "XCdplay*EditPlayList*addall.col:		  1",
    "XCdplay*EditPlayList*addall.row:		  1",
    "XCdplay*EditPlayList*add.col:		  1",
    "XCdplay*EditPlayList*add.row:		  2",
    "XCdplay*EditPlayList*remove.col:		  1",
    "XCdplay*EditPlayList*remove.row:		  3",
    "XCdplay*EditPlayList*delete.col:		  1",
    "XCdplay*EditPlayList*delete.row:		  4",
    "XCdplay*EditPlayList*undelete.col:		  1",
    "XCdplay*EditPlayList*undelete.row:		  5",
    "XCdplay*EditPlayList*top.col:		  3",
    "XCdplay*EditPlayList*top.row:		  0",
    "XCdplay*EditPlayList*up.col:		  3",
    "XCdplay*EditPlayList*up.row:		  1",
    "XCdplay*EditPlayList*down.col:		  3",
    "XCdplay*EditPlayList*down.row:		  4",
    "XCdplay*EditPlayList*bot.col:		  3",
    "XCdplay*EditPlayList*bot.row:		  5",
   /* Options editor */
    "XCdplay*EditOptions*Select.rightButton:	  false",
    "XCdplay*EditOptions*Select.shadowWidth:	  0",
    "XCdplay*EditOptions*RowCol.borderWidth:	  0",
    "XCdplay*EditOptions*TextLine.resizeToPreferred: false",
    "XCdplay*EditOptions.dialog.dialogType:	  modeless",
    "XCdplay*EditOptions.dialog.autoUnmanage:	  false",
    "XCdplay*EditOptions*frame.cols:		  2",
    "XCdplay*EditOptions*frame.rows:		  8",
    "XCdplay*EditOptions*cdDevice*col:		  1",
    "XCdplay*EditOptions*cdDevice*row:		  0",
    "XCdplay*EditOptions*mixDevice*col:		  1",
    "XCdplay*EditOptions*mixDevice*row:		  6",
    "XCdplay*EditOptions*cddbPath*col:		  1",
    "XCdplay*EditOptions*cddbPath*row:		  1",
    "XCdplay*EditOptions*showToolbar.col:	  0",
    "XCdplay*EditOptions*showToolbar.row:	  0",
    "XCdplay*EditOptions*showStatus.col:	  0",
    "XCdplay*EditOptions*showStatus.row:	  1",
    "XCdplay*EditOptions*showVolume.col:	  0",
    "XCdplay*EditOptions*showVolume.row:	  2",
    "XCdplay*EditOptions*showTitle.col:		  0",
    "XCdplay*EditOptions*showTitle.row:		  3",
    "XCdplay*EditOptions*showTrack.col:		  0",
    "XCdplay*EditOptions*showTrack.row:		  4",
    "XCdplay*EditOptions*showInfobar.col:	  0",
    "XCdplay*EditOptions*showInfobar.row:	  5",
    "XCdplay*EditOptions*pollRate.col:		  1",
    "XCdplay*EditOptions*pollRate.row:		  2",
    "XCdplay*EditOptions*stopRate.col:		  1",
    "XCdplay*EditOptions*stopRate.row:		  3",
    "XCdplay*EditOptions*emptyRate.col:		  1",
    "XCdplay*EditOptions*emptyRate.row:		  4",
    "XCdplay*EditOptions*introTime.col:		  0",
    "XCdplay*EditOptions*introTime.row:		  6",
    "XCdplay*EditOptions*skipTime.col:		  0",
    "XCdplay*EditOptions*skipTime.row:		  7",
    "XCdplay*EditOptions*autoPlay.col:		  1",
    "XCdplay*EditOptions*autoPlay.row:		  5",
    "XCdplay*SaveOptions*Label.shadowWidth:	  0",
    "XCdplay*SaveOptions.dialog.dialogType:	  modeless",
    NULL
};

static XtActionsRec xcdplay_actions[] = {
    {"quit", quit},
};

XtAppContext        app_con;
Atom                wm_delete_window;
char               *ProgramName;
Boolean	           iconic;
Display            *dpy;
MainWindow	 * mainwindow;
Widget		   toplevel;


static void 
usage()
{
    fprintf(stderr, "usage:  %s [-options ...]\n\n", ProgramName);
    fprintf(stderr, "where options include:\n");
    fprintf(stderr,
	    "    -display dpy            X server on which to display\n");
    fprintf(stderr,
	    "    -geometry geom          size and location of window\n");
    fprintf(stderr,
	    "    -fn font                font to use in label\n");
    fprintf(stderr,
	    "    -device name            Name of mixer device\n");
    fprintf(stderr,
	    "    -poll mseconds          interval between updates\n");
    fprintf(stderr,
	    "    -bg color               background color\n");
    fprintf(stderr,
	    "    -fg color               foreground color\n");
    exit(0);
}

void 
main(int argc, char **argv)
{
    Window	        win;
    Arg                 args[5];
    Pixmap              image;
    int                 i;
    char		*p;

    ProgramName = argv[0];
    MkDefault();

    toplevel = XtAppInitialize(&app_con, "XCdplay", options, XtNumber(options),
	        &argc, argv, fallback_resources, NULL, (Cardinal) 0);
    if (argc != 1)
	usage();

    XtGetApplicationResources(toplevel, (XtPointer) & resources,
	    	      my_resources, XtNumber(my_resources),
	    	      NULL, (Cardinal) 0);
    XtAppAddActions(app_con, xcdplay_actions, XtNumber(xcdplay_actions));

   /* Add missing type converters */
    XtAppSetTypeConverter(app_con, XtRBoolean, XtRString,
	        CvtBooleanToString, NULL, 0, XtCacheNone, NULL);
    XtAppSetTypeConverter(app_con, XtRInt, XtRString,
	        CvtIntToString, NULL, 0, XtCacheNone, NULL);

    dpy = XtDisplay(toplevel);
   /* Make sure we have a icon */
    mkbitmaps(dpy);
    image = None;
    XtSetArg(args[0], XtNiconPixmap, &image);
    XtSetArg(args[1], XtNiconic, &iconic);
    XtGetValues(toplevel, args, 2);
    if (image == None) {
	XtSetArg(args[0], XtNiconPixmap, Bitmaps[icon]);
	XtSetArg(args[1], XtNiconMask, Bitmask[icon]);
	XtSetValues(toplevel, args, 2);
    }

   /* Add Clue widget */
    XtCreatePopupShell("CluePopup", clueWidgetClass, toplevel, NULL, 0);

   /* Tranlate database path */
    resources.rcpath = ExpandPath((char *)XtNewString(resources.rcpath));

   /* Let user override defaults */
    ReadRCFile();

    resources.cd_device = ExpandPath((char *)XtNewString(resources.cd_device));
#ifdef linux
    resources.mix_device = ExpandPath((char *)XtNewString(resources.mix_device));
#endif
    resources.cddb_path = ExpandPath((char *)XtNewString(resources.cddb_path));

    mainwindow = new MainWindow(toplevel, app_con);

    XtRealizeWidget(toplevel);

   /* This is a hack so that f.delete will do something useful. */
    XtOverrideTranslations(toplevel,
	       XtParseTranslationTable("<Message>WM_PROTOCOLS: quit()"));

    wm_delete_window = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    (void) XSetWMProtocols(dpy, XtWindow(toplevel), &wm_delete_window, 1);

    XtAppMainLoop(app_con);
}

char *
ExpandPath(char *path)
{
    char	 *p = path;
    int		 i;

    while ((p = strchr(p, '$')) != NULL) {
	char *p1, *r;
	int off = p - path;
	/* Find end of string */
	for(p1 = p+1; isalnum(*p1); *p1++);
	/* Build temp place to hold it */
	i = p1 - p;
	char *t = new char [i+1];
	strncpy(t, p+1, i - 1);
	t[i - 1] = '\0';
	/* Look up name */
	if ((r = getenv(t)) != NULL) {
    	    /* Build place to hold new full string */
    	    char *px = (char *)XtMalloc(strlen(path) + strlen(r) + 1 - i);
    	    if (off > 0)
    	    	strncpy(px, path, off);
    	    else
    		*px = '\0';
    	    strcat(px, r);
    	    strcat(px, p1);
    	    XtFree(path);
    	    path = px;
    	    p = &path[off];
	} else
    	    p++;
   } 

   /* Now process ~ expansion */
   p = path;
   while ((p = strchr(p, '~')) != NULL) {
	char  *r;
	int off = p - path;
	/* Build temp place to hold it */
	i = 1;
	/* Look up name */
	if ((r = getenv("HOME")) != NULL) {
    	    /* Build place to hold new full string */
    	    char *px = (char *)XtMalloc(strlen(path) + strlen(r) + 1 - i);
    	    if (off > 0)
    	    	strncpy(px, path, off);
    	    else
    		*px = '\0';
    	    strcat(px, r);
    	    strcat(px, p + 1);
    	    XtFree(path);
    	    path = px;
    	    p = &path[off];
	} else
    	    p++;
    } 
    return path;
}


void
MkDefault()
{
    int	i;

    default_resources = new DefRes [ XtNumber(my_resources) + 1 ];
    for (i = 0; i < XtNumber(my_resources); i++) {
        default_resources[i].name = my_resources[i].resource_name;
        default_resources[i].type = my_resources[i].resource_type;
        default_resources[i].size = my_resources[i].resource_size;
        default_resources[i].offset = ((char *)&resources) +
    				     my_resources[i].resource_offset;
        default_resources[i].def_addr = my_resources[i].default_addr;
    }
    default_resources[i].name = NULL;
}

void
ReadRCFile()
{
    char    			buffer[100], *p, *p1, *p2;
    FILE    			*rcfile;
    DefRes      		*rp;
    XtPointer			xp;
    XrmValue        		src, dst;
    int     			i;

    /* Open it */
    if ((rcfile = fopen(resources.rcpath, "r")) == NULL)
        return; /* Could not read it, either not there or error,
	              will worry about it later */
    while(fgets(buffer, sizeof(buffer), rcfile) != NULL) {
	p = &buffer[strlen(buffer)-1];
       /* Remove trailing blanks, and white space */
        while (p != buffer && (*p == '\n' || *p == ' ' || *p == '\t'))
            *p-- = '\0';
       /* Kill comments */
        if ((p = strchr(buffer, '#')) != NULL)
            *p = '\0';
       /* Skip leading spaces */
        for(p = buffer; *p == ' ' || *p == '\t'; p++);
       /* Anything left? */
        if (*p == '\0')
            continue;
       /* Yup.....scan to end of word */
        for(p1 = p; *p1 != ' ' && *p1 != '\t' && *p1 != '\0'; p1++);
       /* Now scan to start of next word */
        for(*p1++ = '\0'; *p1 == ' ' || *p1 == '\t'; p1++);
       /* Unquote it if there is any */
        p2 = &p1[strlen(p1)-1];
	if ((*p1 == '\'' || *p1 == '"' ) && *p1 == *p2) {
	   *p2 = '\0'; p1++;       /* Remove it */
	}
       /* Ok.. now p points to option and p1 points to value */
       /* Scan options list looking for it */
	for(i = 0; i< XtNumber(my_resources); i++) {
	    rp = &default_resources[i];
	    if (strcasecmp(rp->name, p) != 0) 
	       continue;
	    xp = (XtPointer)rp->offset;
	   /* Check if it is a string */
	    if (strcmp(rp->type, XtRString) == 0) {
	      *(char **)xp = (char *)XtNewString(p1);
	    } else {
	      /* Convert it */
	        src.size = strlen(p1)+1;
	        src.addr = (XPointer) p1;
	        dst.size = rp->size;
	        dst.addr = (XPointer) xp;
	        XtConvertAndStore(toplevel, XtRString, &src, rp->type, &dst);
	    }
	    break;
	}
	if (i == XtNumber(my_resources)) 
	   fprintf(stderr, "Option: %s not found\n", p);
    }
    fclose(rcfile);
}

void
WriteRCFile()
{
    char		*rcpath = new char [strlen(resources.rcpath) + 8];
    char 		 buffer[100], *p;
    FILE    		*rcfile;
    Arg     		 arg[1];
    DefRes  		*rp;
    String  		 comment;
    XrmValue             src, dst;
    int     		 i, j;
    Boolean		 same;
    XtPointer		 xp;

    /* Make it unique */
    sprintf(rcpath, "%s.%7d", resources.rcpath, getpid());
    /* Open it */
    if ((i = open(rcpath, O_CREAT|O_WRONLY, 0600)) < 0) {
        app_error("Unable to create new rcfile");
        return;
    }
    if ((rcfile = fdopen(i, "w")) == NULL) {
        app_error("Unable to open new rcfile");
        return;    /* Could not write it */
    }

    fprintf(rcfile, "# xcdplay Options file.\n#\n");
    for(i = 0; i< XtNumber(my_resources); i++) {
        rp = &default_resources[i];
        xp = (XtPointer)rp->offset;
       /* Check if it is a string */
        if (strcmp(rp->type, XtRString) == 0) {
            char *cp = *(char **)xp;
            /* Check if same as default */
            if (cp != NULL && rp->def_addr != NULL) {
    	        char *p = ExpandPath((char *)XtNewString((char *)rp->def_addr));
                if (strcmp(cp, p) == 0)
                   fprintf(rcfile, "###");
    	        XtFree(p);
    	    }
	    dst.addr = (XPointer)((cp == NULL)?"":cp);
	} else {
	    /* Check if same as default */
	    if (memcmp(xp, &rp->def_addr, rp->size) == 0) 
	         fprintf(rcfile, "###");
	    /* Convert it */
	    src.size = rp->size;
	    src.addr = (XPointer) xp;
	    dst.size = sizeof(buffer);
	    dst.addr = (XPointer) buffer;
	    XtConvertAndStore(toplevel, rp->type, &src, XtRString, &dst);
        }
        fprintf(rcfile, "%s %s\n", rp->name, (char *)dst.addr);
    }
    fclose(rcfile);
    /* Expand the path */
    rename(rcpath, resources.rcpath);
}

/*
 * Handle delete correctly.
 */
static void 
quit(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    if (event->type == ClientMessage &&
        event->xclient.data.l[0] != wm_delete_window) {
       XBell(dpy, 0);
       return;
    }
    delete mainwindow;
    exit(0);
}

void
app_quit(Widget w, XtPointer client_data, XtPointer call_data)
{
    delete mainwindow;
    exit(0);
}

/*
 * Quit application.
 */
static Boolean
Quit_Proc(XtPointer client_data)
{
     exit(0);
}

static void
removechild(Widget w, XtPointer client_data, XtPointer call_data)
{
    XtDestroyWidget(w);
}


/*
 * Check if cddb directory exists.
 * popup dialog if it does not exist.
 */
void
checkdb()
{
    char		*nstr = new char [strlen(resources.cddb_path) + 100];
    Arg			arg[1];
    Widget		dialog;

    if (access(resources.cddb_path, F_OK) == 0) 
	return;
    strcpy(nstr, resources.cddb_path);
    strcat(nstr, "\nDoes not exist, create it?");
    XtSetArg(arg[0], XtNmessage, nstr);
    dialog = XpwDialogCreateQuestion(toplevel, "Question", arg, 1);
    XtUnmanageChild(XpwDialogGetChild(dialog, XpwDialog_Help_Button));
    XtAddCallback(XpwDialogGetChild(dialog, XpwDialog_Ok_Button), XtNcallback,
		 mkcddb, NULL);
    XtAddCallback(XpwDialogGetChild(dialog, XpwDialog_Ok_Button), XtNcallback,
		 removechild, NULL);
    XtAddCallback(XpwDialogGetChild(dialog, XpwDialog_Cancel_Button), XtNcallback,
		 removechild, NULL);
    XtManageChild(dialog);
}
/*
 * Error popup.
 */
void
app_error(char *msg)
{
    char		*emsg = strerror(errno);
    char		*nstr = new char [strlen(msg) + strlen(emsg) + 3];
    Arg			arg[1];
    Widget		dialog;

    strcpy(nstr, msg);
    strcat(nstr, ": ");
    strcat(nstr, emsg);
    XtSetArg(arg[0], XtNmessage, nstr);
    dialog = XpwDialogCreateNotice(toplevel, "Notice", arg, 1);
    XtUnmanageChild(XpwDialogGetChild(dialog, XpwDialog_Help_Button));
    XtUnmanageChild(XpwDialogGetChild(dialog, XpwDialog_Cancel_Button));
    XtAddCallback(XpwDialogGetChild(dialog, XpwDialog_Ok_Button), XtNcallback,
		 removechild, NULL);
    XtManageChild(dialog);
}

/*
 * Fatal Error popup.
 */
void
app_fatal(char *msg)
{
    char		*emsg = strerror(errno);
    char		*nstr = new char [strlen(msg) + strlen(emsg) +
    				  strlen(ProgramName) + 10];
    Arg			arg[1];
    Widget		dialog;

    sprintf(nstr, "%s: %s\nQuit %s?", msg, emsg, ProgramName);
    XtSetArg(arg[0], XtNmessage, nstr);
    dialog = XpwDialogCreateError(toplevel, "Error", arg, 1);
    XtUnmanageChild(XpwDialogGetChild(dialog, XpwDialog_Help_Button));
    if (XpwDialogDisplay(dialog) == XpwDialog_Ok_Button)
       app_quit(NULL, NULL, NULL);
    XtDestroyWidget(dialog);
}
   
/*
 * Make cddb directory.
 */
static void
mkcddb(Widget w, XtPointer client_data, XtPointer call_data)
{
	mkdir(resources.cddb_path, 0700);
}

/*
 * A few basic type converters.
 */
static Boolean
CvtBooleanToString(Display *dpy, XrmValue *args, Cardinal *num_args,
     XrmValue *from, XrmValue *to, XtPointer *convert_data)
{

    Boolean		 value = *(Boolean *)from->addr;
    Boolean		 convert_failed = False;
    static char          buffer[50];
    int   		 i;

    strcpy(buffer, (value == TRUE) ? "True" : "False");

    if (to->addr == NULL)
	to->addr = (XPointer)&buffer;
    else {
	if (to->size < strlen(buffer) )
	    convert_failed = TRUE;
	else
	    strcpy((char *)(to->addr), buffer);
    }
    to->size = strlen(buffer);
    return (!convert_failed);
}

static Boolean
CvtIntToString(Display *dpy, XrmValue *args, Cardinal *num_args,
     XrmValue *from, XrmValue *to, XtPointer *convert_data)
{

    int			value = *(int *)from->addr;
    Boolean		convert_failed = False;
    static  char	buffer[50];
    int    		i;


    sprintf(buffer, "%d", value);

    if (to->addr == NULL)
	 to->addr = (XPointer)&buffer;
    else {
	 if (to->size < strlen(buffer) )
	    convert_failed = TRUE;
	 else
	    strcpy((char *)(to->addr), buffer);
    }
    to->size = strlen(buffer);
    return (!convert_failed);
}

