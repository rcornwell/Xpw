/*
 * XCdplay: Plays a cd.
 *
 * $Id: xcdplay.h,v 1.2 1998/01/26 01:02:02 rich Exp rich $
 *
 * $Log: xcdplay.h,v $
 * Revision 1.2  1998/01/26 01:02:02  rich
 * Support for hiding clues.
 *
 * Revision 1.1  1997/12/16 05:48:46  rich
 * Initial revision
 *
 *
 */

#ifndef CDROM_SELECT_DISC
#define MAXDRIVES       1
#else
#define MAXDRIVES       4
#endif

typedef struct _XCdplayResources {
    String		rcpath;
    String              cd_device;
#ifdef linux
    String              mix_device;
#endif
    String		cddb_path;
    Boolean             show_toolbar;
    Boolean		show_status;
    Boolean		show_volume;
    Boolean		show_title;
    Boolean		show_track;
    Boolean		show_infobar;
    Boolean		autoplay;
    Boolean		show_clues;
    int                 poll_rate;
    int			stop_rate;
    int			empty_rate;
    int			skip_time;
    int			intro_time;
} XCdplayResources;

extern XCdplayResources resources;

typedef struct {
     String     name;
     String     type;
     Cardinal   size;
     XtPointer  offset;
     XtPointer  def_addr;
} DefRes;

extern DefRes *default_resources;
extern void app_quit(Widget w, XtPointer client_data, XtPointer call_data);
extern void app_error(char *msg);
extern void app_fatal(char *msg);
extern void checkdb();
extern char *ExpandPath(char *path);
extern void ReadRCFile();
extern void WriteRCFile();

extern char               *ProgramName;

