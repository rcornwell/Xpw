/*
 * Xmixer: Control sound card volume settings.
 *
 * $Log: xmixer.c,v $
 * Revision 1.1  1997/10/29 05:41:32  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: xmixer.c,v 1.1 1997/10/29 05:41:32 rich Exp rich $";
#endif

/* System stuff */
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>

/* Include all the widget stuff we need */
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <Xpw/Clue.h>
#include <Xpw/RowCol.h>
#include <Xpw/Paned.h>
#include <Xpw/Label.h>
#include <Xpw/Command.h>
#include <Xpw/Arrow.h>
#include <Xpw/Frame.h>
#include <Xpw/Slider.h>
#include <Xpw/Select.h>

#include "bitmaps/aux.xbm"
#include "bitmaps/bass.xbm"
#include "bitmaps/cd.xbm"
#include "bitmaps/headphone.xbm"
#include "bitmaps/hook.xbm"
#include "bitmaps/huh.xbm"
#include "bitmaps/iconify.xbm"
#include "bitmaps/mic.xbm"
#include "bitmaps/mix_play.xbm"
#include "bitmaps/mix_rec.xbm"
#include "bitmaps/mixicon.xbm"
#include "bitmaps/mute.xbm"
#include "bitmaps/pcm.xbm"
#include "bitmaps/pcm2.xbm"
#include "bitmaps/pcsp.xbm"
#include "bitmaps/power.xbm"
#include "bitmaps/record.xbm"
#include "bitmaps/speaker.xbm"
#include "bitmaps/synth.xbm"
#include "bitmaps/tape.xbm"
#include "bitmaps/treble.xbm"
#include "bitmaps/tweeter.xbm"
#include "bitmaps/vol.xbm"
#include "bitmaps/volume.xbm"
#include "bitmaps/wide.xbm"
#include "bitmaps/wideoff.xbm"

typedef struct _XMixerResources {
    String              mixer_device;
    Boolean             show_all;
    int                 poll_rate;
} XMixerResources;

static XrmOptionDescRec options[] =
{
    {"-device", ".mixerDevice", XrmoptionSepArg, NULL},
    {"-poll", ".pollRate", XrmoptionSepArg, NULL},
    {"-show", ".showAll", XrmoptionNoArg, "True"},
    {"-noshow", ".showAll", XrmoptionNoArg, "False"},
};

#define Offset(field) (XtOffsetOf(XMixerResources, field))

static XtResource   my_resources[] =
{
    {"mixerDevice", XtCString, XtRString, sizeof(String),
     Offset(mixer_device), XtRImmediate, (XtPointer) "/dev/mixer"},
    {"showAll", XtCBoolean, XtRBoolean, sizeof(Boolean),
     Offset(show_all), XtRImmediate, (XtPointer) FALSE},
    {"pollRate", "PollRate", XtRInt, sizeof(int),
     Offset(poll_rate), XtRImmediate, (XtPointer) 500},
};

#undef Offset

static XMixerResources resources;

static void         usage(void);
void                main(int /*argc */ , char ** /*argv */ );
static void         quit(Widget /*w */ , XEvent * /*event */ ,
			 String * /*params */ , Cardinal * /*num_params */ );
static void         app_quit(Widget /*w */ , XtPointer /*client_data */ ,
			 XtPointer /*call_data */ );
static void         app_iconify(Widget /*w */ , XtPointer /*client_data */ ,
			 XtPointer /*call_data */ );
static void	    iconify(Widget /*w */, XtPointer /*client_data */,
			 XEvent */*event*/, Boolean */*dispatch*/);
static void         Update(XtPointer /*client_data */ ,
			 XtIntervalId * /*timerid */ );
static void         update_settings(void);
static void         button_call(Widget /*w */ , XtPointer /*client_data */ ,
			 XtPointer /*call_data */ );
static void         balence_call(Widget /*w */ , XtPointer /*client_data */ ,
			 XtPointer /*call_data */ );
static void         slide_call(Widget /*w */ , XtPointer /*client_data */ ,
			 XtPointer /*call_data */ );
static void         rec_call(Widget /*w */ , XtPointer /*client_data */ ,
			 XtPointer /*call_data */ );

/*
 * Define the bare minimum here to make appliation look correct.
 */
static String       fallback_resources[] =
{
   /* Global Stuff */
    "XMixer*Frame.outline:	   false",
    "XMixer*Label.shadowWidth:	   0",
   /* Labels */
    "XMixer*botbar*Slider.width:   20",
    "XMixer*left.label: 	   Left",
    "XMixer*right.label:	   Right",
    "XMixer*loud.label:		   Loudness",
    "XMixer*switch.shadowWidth:	   0",
    "XMixer*holder.shadowWidth:	   0",
    "XMixer*switch.switchSize:	   20",
    "XMixer*switch.switchShape:	   square",
    NULL
};

static XtActionsRec xmixer_actions[] =
{
    {"quit", quit},
};

static XtAppContext        app_con;
static Atom                wm_delete_window;
static char               *ProgramName;
static int                 mixdev;
static int                 mixmask;
static int                 recmask;
static int                 steriomask;
static int                 recsrc;
static Boolean	           iconic;
static int                 balence = 50;
static Widget              bal_slide = NULL;
static XtIntervalId        timer = (XtIntervalId)NULL;
static char	          *devnames[] = SOUND_DEVICE_NAMES;
static Display            *dpy;
static Window	           win;

static struct _mixarray {
    int                 control;
    int                 width;
    int                 height;
    char               *bits;
    Widget              slide;
    Widget              rec;
    int                 value;
} Mixdata[] = {

    { SOUND_MIXER_VOLUME, vol_width, vol_height, vol_bits, NULL, NULL, 0 },
    { SOUND_MIXER_BASS, bass_width, bass_height, bass_bits, NULL, NULL, 0 },
    { SOUND_MIXER_TREBLE, treble_width, treble_height, treble_bits, NULL, NULL, 0 },
    { SOUND_MIXER_SYNTH, synth_width, synth_height, synth_bits, NULL, NULL, 0 },
    { SOUND_MIXER_PCM, pcm_width, pcm_height, pcm_bits, NULL, NULL, 0 },
    { SOUND_MIXER_SPEAKER, pcsp_width, pcsp_height, pcsp_bits, NULL, NULL, 0 },
    { SOUND_MIXER_LINE, aux_width, aux_height, aux_bits, NULL, NULL, 0 },
    { SOUND_MIXER_MIC, mic_width, mic_height, mic_bits, NULL, NULL, 0 },
    { SOUND_MIXER_CD, cd_width, cd_height, cd_bits, NULL, NULL, 0 },
    { SOUND_MIXER_IMIX, tweeter_width, tweeter_height, tweeter_bits, NULL, NULL, 0 },
    { SOUND_MIXER_ALTPCM, pcm2_width, pcm2_height, pcm2_bits, NULL, NULL, 0 },
    { SOUND_MIXER_RECLEV, aux_width, aux_height, aux_bits, NULL, NULL, 0 },
    { SOUND_MIXER_IGAIN, tape_width, tape_height, tape_bits, NULL, NULL, 0 },
    { SOUND_MIXER_OGAIN, headphone_width, headphone_height, headphone_bits, NULL, NULL, 0 },
    { SOUND_MIXER_LINE1, aux_width, aux_height, aux_bits, NULL, NULL, 0 },
    { SOUND_MIXER_LINE2, aux_width, aux_height, aux_bits, NULL, NULL, 0 },
    { SOUND_MIXER_LINE3, aux_width, aux_height, aux_bits, NULL, NULL, 0 },
};


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
	    "    -show                   show all controls\n");
    fprintf(stderr,
	    "    -noshow                 show only valid controls\n");
    fprintf(stderr,
	    "    -bg color               background color\n");
    fprintf(stderr,
	    "    -fg color               foreground color\n");
    exit(0);
}

void 
main(argc, argv)
	int                 argc;
	char              **argv;
{
    Widget              toplevel, topbar, botbar, w;
    Arg                 args[5];
    Pixmap              image;
    int                 i;

    ProgramName = argv[0];

    toplevel = XtAppInitialize(&app_con, "XMixer", options, XtNumber(options),
		    &argc, argv, fallback_resources, NULL, (Cardinal) 0);
    if (argc != 1)
	usage();

    XtGetApplicationResources(toplevel, (XtPointer) & resources,
			      my_resources, XtNumber(my_resources),
			      NULL, (Cardinal) 0);
    XtAppAddActions(app_con, xmixer_actions, XtNumber(xmixer_actions));

    dpy = XtDisplay(toplevel);
    win = DefaultRootWindow(dpy);
   /* Make sure we have a icon */
    image = None;
    XtSetArg(args[0], XtNiconPixmap, &image);
    XtSetArg(args[1], XtNiconic, &iconic);
    XtGetValues(toplevel, args, 2);
    if (image == None) {
	image = XCreateBitmapFromData(dpy,
				  win,
		   (char *) mixicon_bits, mixicon_width, mixicon_height);
	XtSetArg(args[0], XtNiconPixmap, image);
	XtSetValues(toplevel, args, 1);
    }

   /* Add Clue widget */
    XtCreatePopupShell("CluePopup", clueWidgetClass, toplevel, NULL, 0);

   /* Open mixer */
    if ((mixdev = open(resources.mixer_device, O_RDWR)) < 0) {
	fprintf(stderr, "%s: Unable to open mixer device %s\n",
		ProgramName, resources.mixer_device);
	exit(0);
    }
   /* Get device masks */
    if (ioctl(mixdev, SOUND_MIXER_READ_DEVMASK, &mixmask) < 0) {
	fprintf(stderr, "%s: Unable get list of devices\n",
		ProgramName);
	mixmask = 0xffffff;
    }
    if (ioctl(mixdev, SOUND_MIXER_READ_RECMASK, &recmask) < 0) {
	fprintf(stderr, "%s: Unable get list of record devices\n",
		ProgramName);
	recmask = 0;
    }
    if (ioctl(mixdev, SOUND_MIXER_READ_STEREODEVS, &steriomask) < 0) {
	fprintf(stderr, "%s: Unable get list of sterio devices\n",
		ProgramName);
       /* Assume all sterio */
	steriomask = mixmask;
    }

   /* Build widget tree. */
    w = XtCreateManagedWidget("manager", rowColWidgetClass, toplevel, NULL, 0);

    XtSetArg(args[0], XtNorientation, XtorientHorizontal);
    topbar = XtCreateManagedWidget("topbar", rowColWidgetClass, w, args, 1);
    botbar = XtCreateManagedWidget("botbar", rowColWidgetClass, w, args, 1);
    image = XCreateBitmapFromData(dpy, win,
			 (char *) power_bits, power_width, power_height);
    XtSetArg(args[0], XtNlabel, "");
    XtSetArg(args[1], XtNbitmap, image);
    w = XtCreateManagedWidget("power", commandWidgetClass, topbar, args, 2);
    XtAddCallback(w, XtNcallback, app_quit, toplevel);
    image = XCreateBitmapFromData(dpy, win,
		   (char *) iconify_bits, iconify_width, iconify_height);
    XtSetArg(args[1], XtNbitmap, image);
    w = XtCreateManagedWidget("iconify", commandWidgetClass, topbar, args, 2);
    XtAddCallback(w, XtNcallback, app_iconify, toplevel);
    if (mixmask & SOUND_MASK_LOUD || resources.show_all) {
        XtSetArg(args[0], XtNswitchSize, 0);
	w = XtCreateManagedWidget("loud", selectWidgetClass, topbar, args, 1);
	XtAddCallback(w, XtNcallback, button_call, (XtPointer) SOUND_MIXER_LOUD);
	if ((mixmask & SOUND_MASK_LOUD) == 0)
	   XtSetSensitive(w, False);
    }
    if (mixmask & SOUND_MASK_ENHANCE || resources.show_all) {
        image = XCreateBitmapFromData(dpy, win,
			 (char *) wide_bits, wide_width, wide_height);
        XtSetArg(args[0], XtNlabel, "");
        XtSetArg(args[1], XtNbitmap, image);
        XtSetArg(args[2], XtNswitchSize, 0);
	w = XtCreateManagedWidget("seperate", selectWidgetClass, topbar, args, 3);
	XtAddCallback(w, XtNcallback, button_call, (XtPointer) SOUND_MIXER_ENHANCE);
	if ((mixmask & SOUND_MASK_ENHANCE) == 0)
	   XtSetSensitive(w, False);
    }
    if (mixmask & SOUND_MASK_MUTE || resources.show_all) {
        image = XCreateBitmapFromData(dpy, win,
			 (char *) mute_bits, mute_width, mute_height);
        XtSetArg(args[0], XtNlabel, "");
        XtSetArg(args[1], XtNbitmap, image);
        XtSetArg(args[2], XtNswitchSize, 0);
	w = XtCreateManagedWidget("mute", selectWidgetClass, topbar, args, 3);
	XtAddCallback(w, XtNcallback, button_call, (XtPointer) SOUND_MIXER_MUTE);
	if ((mixmask & SOUND_MASK_MUTE) == 0)
	   XtSetSensitive(w, False);
    }
    XtSetArg(args[0], XtNlabel, "");
    w = XtCreateManagedWidget("frame", frameWidgetClass, topbar, args, 1);
    XtSetArg(args[0], XtNorientation, XtorientHorizontal);
    w = XtCreateManagedWidget("manager", rowColWidgetClass, w, args, 1);
    XtCreateManagedWidget("left", labelWidgetClass, w, NULL, 0);
    XtSetArg(args[0], XtNorientation, XtorientHorizontal);
    XtSetArg(args[1], XtNmin, 0);
    XtSetArg(args[2], XtNmax, 100);
    bal_slide = XtCreateManagedWidget("balence", sliderWidgetClass, w, args, 3);
    XtCreateManagedWidget("right", labelWidgetClass, w, NULL, 0);

   /* Create sliders */
    for (i = 0; i < XtNumber(Mixdata); i++) {
	Widget              frame;
	int                 cntl;

	cntl = 1 << Mixdata[i].control;

	/* Check if this should be added */
	if ((mixmask & cntl) == 0 && !resources.show_all)
	    continue;
	if (i == SOUND_MIXER_VOLUME)
	    XtAddCallback(bal_slide, XtNcallback, balence_call, (XtPointer) i);
	/* Install image if one */
	image = None;
	if (Mixdata[i].bits != NULL) 
	    image = XCreateBitmapFromData(dpy, win, (char *) Mixdata[i].bits,
				    Mixdata[i].width, Mixdata[i].height);

	XtSetArg(args[0], XtNlabel, "");
	XtSetArg(args[1], XtNbitmap, image);
	frame = XtCreateManagedWidget(devnames[i], frameWidgetClass, botbar, 
							 args, 2);
	w = XtCreateManagedWidget("manager", rowColWidgetClass, frame, NULL, 0);
	XtSetArg(args[0], XtNmin, 0);
	XtSetArg(args[1], XtNmax, 100);
	Mixdata[i].slide = XtCreateManagedWidget("slider", sliderWidgetClass, w,
							 args, 2);
	XtAddCallback(Mixdata[i].slide, XtNcallback, slide_call, (XtPointer) i);
	XtSetArg(args[0], XtNlabel, "");
	XtSetArg(args[1], XtNstate, ((recsrc & cntl) != 0) ? True : False);
	XtSetArg(args[2], XtNswitchShape, XaNone);
	Mixdata[i].rec = XtCreateManagedWidget(
		 (recmask & cntl)?"switch":"holder", selectWidgetClass, w,
		 args, (recmask & cntl)?2:3);
	XtAddCallback(Mixdata[i].rec, XtNcallback, rec_call, (XtPointer) i);
	if ((mixmask & cntl) == 0) 
	   XtSetSensitive(frame, False);
    }

    XtRealizeWidget(toplevel);

   /* This is a hack so that f.delete will do something useful. */
    XtOverrideTranslations(toplevel,
	       XtParseTranslationTable("<Message>WM_PROTOCOLS: quit()"));

    wm_delete_window = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    (void) XSetWMProtocols(dpy, XtWindow(toplevel), &wm_delete_window, 1);

   /* Turn off timer when iconified */
    XtAddEventHandler(toplevel, StructureNotifyMask, False, iconify, NULL);

   /* Make sure sliders are updated */
    timer = XtAppAddTimeOut(app_con, 10, Update, NULL);

    XtAppMainLoop(app_con);
}

/*
 * Handle delete correctly.
 */
static void 
quit(w, event, params, num_params)
	Widget              w;
	XEvent             *event;
	String             *params;
	Cardinal           *num_params;
{
    if (event->type == ClientMessage &&
	event->xclient.data.l[0] != wm_delete_window) {
	XBell(dpy, 0);
	return;
    }
    XtDestroyApplicationContext(app_con);
    close(mixdev);
    exit(0);
}

/* 
 * Quit button call back.
 */
static void 
app_quit(w, client_data, call_data)
	Widget              w;
	XtPointer           client_data;
	XtPointer           call_data;
{
    XtDestroyApplicationContext(app_con);
    close(mixdev);
    exit(0);
}

/*
 * Iconify button callback.
 */
static void 
app_iconify(w, client_data, call_data)
	Widget              w;
	XtPointer           client_data;
	XtPointer           call_data;
{
    Widget              top = (Widget) client_data;

    XtVaSetValues(top, XtNiconic, True, NULL);
}

/*
 * Handle iconify events.
 */
static void
iconify(w, client_data, event, dispatch)
Widget w;
XtPointer client_data;
XEvent *event;
Boolean *dispatch;
{
    if (timer != (XtIntervalId)NULL && event->type == UnmapNotify) {
	XtRemoveTimeOut(timer);
	timer = (XtIntervalId)NULL;
    } else if (timer == (XtIntervalId)NULL && event->type == MapNotify) {
    	timer = XtAppAddTimeOut(app_con, 10, Update, (XtPointer) app_con);
    }
}


/*
 * Load the contents of all controls.
 */
static void 
Update(client_data, timerid)
	XtPointer           client_data;
	XtIntervalId       *timerid;
{
    int                 i, left, right, data, cntl;

   /* Update record source mask */
    if (ioctl(mixdev, SOUND_MIXER_READ_RECSRC, &recsrc) < 0) {
	fprintf(stderr, "%s: Unable get recording sources\n",
		ProgramName);
	recsrc = recmask;
    }
   /* Update each of the sliders */
    for (i = 0; i < XtNumber(Mixdata); i++) {
	cntl = 1 << Mixdata[i].control;

	if ((mixmask & cntl) == 0)
	    continue;

	if (ioctl(mixdev, MIXER_READ(Mixdata[i].control), &data) < 0) {
	    Widget	w = XtParent(XtParent(Mixdata[i].slide));

	    mixmask &= ~cntl;
	    XtSetSensitive(w, False);
	    fprintf(stderr, "%s: Unable to read setting for %s\n",
		    ProgramName, devnames[i]);
	    continue;
	}
	left = data & 0x7f;
	right = (data >> 8) & 0x7f;
	data = 100 - ((left + right) / 2);
	if (Mixdata[i].control == SOUND_MIXER_VOLUME) {
	    balence = 50 + ((right - left)/2);
	    if (balence > 100)
	        balence = 100;
	    if (balence < 0)
	        balence = 0;
	    data = 100 - ((left > right)?left:right);
	    if (bal_slide != NULL)
		XpwSliderSetPosition(bal_slide, balence);
	}
	if (Mixdata[i].value != data) {
	    Mixdata[i].value = data;
	    if (Mixdata[i].slide != NULL)
		XpwSliderSetPosition(Mixdata[i].slide, data);
	}
	if (Mixdata[i].rec != NULL) {
	    Arg                 args[1];

	    XtSetArg(args[0], XtNstate, ((recsrc & cntl) != 0) ? True : False);
	    XtSetValues(Mixdata[i].rec, args, 1);
	}
    }
    timer = XtAppAddTimeOut(app_con, resources.poll_rate, Update, NULL);
}

/*
 * Handle push button controls.
 */
static void 
button_call(w, client_data, call_data)
	Widget              w;
	XtPointer           client_data;
	XtPointer           call_data;
{
    int                 cntl = (int) client_data;
    int                 state = (int) call_data;

    state = (state) ? 80 : 0;
    if (ioctl(mixdev, MIXER_WRITE(cntl), &state) < 0) {
	XtVaSetValues(w, XtNstate, (state != 0)?False:True);
	XBell(dpy, 0);
	return;
    }
}

/*
 * Handle the balence slider.
 */
static void 
balence_call(w, client_data, call_data)
	Widget              w;
	XtPointer           client_data;
	XtPointer           call_data;
{
    int                 cntl = (int) client_data;
    int                 data = (int) call_data;

    balence = data;
    slide_call(w, client_data, (XtPointer)Mixdata[cntl].value);
}

/*
 * Handle generic slider.
 */
static void 
slide_call(w, client_data, call_data)
	Widget              w;
	XtPointer           client_data;
	XtPointer           call_data;
{
    int                 cntl = (int) client_data;
    int                 data = (int) call_data;

    Mixdata[cntl].value = data;
    data = 0x7f & (100 - data);
    if (cntl == SOUND_MIXER_VOLUME) {
        int             left, right;

        if (balence > 50) {
	    left = data - (balence - 50)*2;
	    if (left < 0)
	        left = 0;
	    right = data;
        } else {
	    right = data - (50 - balence)*2;
	    if (right < 0)
	        right = 0;
	    left = data;
        }
	data = (right << 8) + (left);
    } else
	data += data << 8;

    if (ioctl(mixdev, MIXER_WRITE(Mixdata[cntl].control), &data) < 0) {
	fprintf(stderr, "%s: Unable to write setting for %s\n",
		ProgramName, devnames[cntl]);
	XBell(dpy, 0);
    }
}

/*
 * Set or clear a recording source.
 */
static void 
rec_call(w, client_data, call_data)
	Widget              w;
	XtPointer           client_data;
	XtPointer           call_data;
{
    int                 cntl = (1 << Mixdata[(int) client_data].control);
    int                 state = (int) call_data;

    if (state)
	recsrc |= cntl;
    else
	recsrc &= ~cntl;

    if (ioctl(mixdev, SOUND_MIXER_WRITE_RECSRC, &recsrc) < 0) {
	XpwSelectUnsetCurrent(w);
	XBell(dpy, 0);
    }
}
