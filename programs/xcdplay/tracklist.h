/*
 * XCdplay: Plays a cd.
 *
 * Handles tracklist and xmcd database.
 *
 */

/*
 * $Id: tracklist.h,v 1.1 1997/12/16 05:48:46 rich Exp rich $
 *
 * $Log: tracklist.h,v $
 * Revision 1.1  1997/12/16 05:48:46  rich
 * Initial revision
 *
 *
 */

class Track {
private:
	Str		name;
	Str		extd;
	int		offset;

public:
	Track() { offset = 0; }
	~Track() { }
	void setoffset(int off) {  offset = off;}
	int getoffset() { return offset; }
	void setext(Str ext) { extd = ext; }
	Str getext() { return extd; }
	void settrk(Str trk) { name = trk; }
	Str gettrk() { return name; }
	void trkappd(char *trk) { name.addline(trk); } 
	void extappd(char *ext) { extd.addline(ext); }
};
			
	
class TrackList {
private:
	const int	discid;		/* Disk Id. */
	const int	ntracks;	/* Number of tracks */
	const int	disclen;	/* Lenght of disk in seconds */
	const Widget	toplevel;	/* Top level window */
	Str		ids;		/* Disc ids */
	Str		filename;	/* File name we loaded */
	int		revision;	/* Revision of file */
	Str		title;		/* Disk title */
	Str		extd;		/* Disk extended data */
	Track		*tracks;	/* Track data */
	Str		playorder;	/* Play order */
	int		tnum;		/* Editing track number */
	int		changed;	/* Needs to be saved */
	int		updated;	/* Data has changed */
	Widget		dialog;		/* Popup child */
	char   		*ignore;	/* Ignore list */

       /* Callbacks for windows */
        static void trackchange(Widget, XtPointer, XtPointer);
        static void trackselect(Widget, XtPointer, XtPointer);
        static void editapply(Widget, XtPointer, XtPointer);
        static void editok(Widget, XtPointer, XtPointer);
        static void saveok(Widget, XtPointer, XtPointer);
        static void dialogdone(Widget, XtPointer, XtPointer);
        static void showplay(Widget, XtPointer, XtPointer);
        static void hideplay(Widget, XtPointer, XtPointer);
        static void playclear(Widget, XtPointer, XtPointer);
        static void playaddall(Widget, XtPointer, XtPointer);
        static void playadd(Widget, XtPointer, XtPointer);
        static void playremove(Widget, XtPointer, XtPointer);
        static void playdelete(Widget, XtPointer, XtPointer);
        static void playundelete(Widget, XtPointer, XtPointer);
        static void playtrack(Widget, XtPointer, XtPointer);
        static void playmove(Widget, XtPointer, XtPointer);
        static void playplay(Widget, XtPointer, XtPointer);

       /* General private functions */
        void editsave(Widget parent);
	char *findfile(char *);
	void writestring(FILE *, char *, Str);
public:
	TrackList();
	TrackList(int id, int disklen, int ntrks, Widget top, int *trkinfo);
	~TrackList();

	void readfile();
	void writefile();
        void editTrack();
	/* Get track name for a given track.  */
	Str getname(int num) { return tracks[num].gettrk(); }
	char *gettitle() { return title.cstring(); }
        int  chkupded() { int i = updated; updated = 0; return i; }
	Str  getplayorder() { return playorder; }
	void setplayorder(Str n) { playorder = n; changed = 1; }
};
