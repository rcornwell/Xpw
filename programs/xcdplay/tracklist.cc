/*
 * XCdplay: Plays a cd.
 *
 * Handles tracklist and xmcd database.
 *
 * $Log: tracklist.cc,v $
 * Revision 1.2  1998/01/26 01:02:02  rich
 * Merged Playlist edit into Tracklist Edit.
 * Changed to support new API for List Widget.
 *
 * Revision 1.1  1997/12/16 05:48:46  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: tracklist.cc,v 1.2 1998/01/26 01:02:02 rich Exp rich $";
#endif

/* System stuff */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define __USE_GNU
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

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
#include "bitmaps.h"
#include "tracklist.h"

/* Make a new track list */
TrackList::TrackList() : ntracks(0) {
    tracks = NULL;
    dialog = NULL;
    ignore = NULL;
    changed = 0;
    updated = 0;
}

/*
 * Set up to read info for a CD.
 */
TrackList::TrackList(int id, int disklen, int ntrks, Widget top, int *trkinfo)
	:  discid(id), disclen (disklen), ntracks(ntrks), toplevel(top) {
   dialog = NULL;
   ignore = NULL;
   changed = 0;
   tracks = new Track [ntracks];
  /* Copy over offset stuff. */
   for(int i = 0; i< ntracks; i++) 
      tracks[i].setoffset(trkinfo[i]);
   char *p = findfile(resources.cddb_path);
   if (p != NULL) {
       filename = Str(p);
       readfile();
   } else {
       char buffer[10];
       sprintf(buffer, "%08x", discid);
       ids = buffer;
       filename = resources.cddb_path;
       filename = filename + "/" + ids;
   }
   updated = 0;
}

/* Free a playlist */
TrackList::~TrackList() {
    if (dialog != NULL) 
        dialogdone(dialog, this, NULL);

   /* Last chance to ask to save any changes */
    editsave(toplevel);
    delete [] tracks;
    delete [] ignore;
}



/*
 * Find a file in a directory. Check any subdirectories too.
 */
char *
TrackList::findfile(char *path) {
    char	file[10];
    char	*result;
    int	len;
    DIR	*dir;
    struct	dirent *dd;
    struct	stat	sbuf;

    sprintf(file, "%08x", discid);

    result = new char [strlen(path) + strlen(file) + 2];
    strcpy(result, path);
    strcat(result, "/");
    len = strlen(result);

    if ((dir = opendir(path)) == NULL) {
    	delete [] result;
    	return NULL;
    }
    
    while ((dd = readdir(dir))) {
        if (*dd->d_name == '.')
    	    continue;
        strcpy(&result[len], dd->d_name);
        if (strcasecmp(file, dd->d_name) == 0) {
    	    closedir(dir);
    	    return result;
        }
        if (stat(result, &sbuf) < 0)
    	    continue;
        if (S_ISDIR(sbuf.st_mode)) {
    	    char *t;
    	    if ((t = findfile(result)) != NULL) {
    	        delete [] result;
    	        closedir(dir);
    	        return t;
    	    }
        }
     }
     closedir(dir);
     delete [] result;
     return NULL;
}

/*
 * Read a DB file in.
 */
void
TrackList::readfile() {
    FILE	*fd;
    char	buffer[1000];
    char	*p, *p1;
    int	i;

    if ((fd = fopen(filename.cstring(), "r")) == NULL) {
    	char	*buffer = new char [strlen(filename.cstring()) + 50];
    	sprintf(buffer, "Unable to open %s for reading", 
    		filename.cstring());
    	app_error(buffer);
    	return;
    }
    while(fgets(buffer, sizeof(buffer), fd) != NULL) {
    	if ((p = strchr(buffer, '\n')) != NULL)
    		*p = '\0';
    	if (buffer[0] == '#') {
    		/* Skip leading blanks */
    	    for(p = &buffer[1]; *p == ' ' || *p == '\t'; p++);
    	    for(p1 = p ; *p1 != ' ' && *p1 != '\t'; p1++);
    	    for( ; *p1 == ' ' || *p1 == '\t'; p1++);
    	    if (strncasecmp(buffer, "Revision:", 9) == 0) {
    		revision = atoi(p1);
    		continue;
    	    }
        }
       /* Scan for equals */
        for(p = &buffer[0]; *p != '='; p++);
    	p++;
    	if (strncasecmp(buffer, "DISCID", 6) == 0) {
    	    ids.addline(p);
    	} else if (strncasecmp(buffer, "DTITLE", 6) == 0) {
    	    title.addline(p);
    	} else if (strncasecmp(buffer, "EXTD", 4) == 0) {
    	    extd.addline(p);
    	} else if (strncasecmp(buffer, "PLAYORDER", 9) == 0) {
    	    playorder.addline(p);
    	} else if (strncasecmp(buffer, "TTITLE", 6) == 0) {
    	    for(p1 = &buffer[6], i = 0; *p1 != '='; p1++) 
    	        i = (i*10) + (*p1 - '0');
    	    if (i < ntracks)
    	        tracks[i].trkappd(p);
    	} else if (strncasecmp(buffer, "EXTT", 4) == 0) {
    	    for(p1 = &buffer[4], i = 0; *p1 != '='; p1++) 
    	        i = (i*10) + (*p1 - '0');
    	    if (i < ntracks)
    	        tracks[i].extappd(p);
    	}
    }
    fclose(fd);
}

/*
 * Write string to file.
 */
void
TrackList::writestring(FILE *fd, char *title, Str str) {
    char	buffer[1000];
    char	*p = str.cstring();
    int	len = strlen(title);

    if (*p == '\0') {
    	fputs(title, fd);
    	fputc('\n', fd);
    	return;
    }
    for(; *p != '\0'; ) {
    	char *p1;

     	strcpy(buffer, title);
    	if ((p1 = strchr(p, '\n')) != NULL) {
    	     p1++;
    	     strncpy(&buffer[len], p, p1-p);
    	     buffer[len + (p1 - p)] = '\0';
    	     p = p1;
    	} else {
    	     strcpy(&buffer[len], p);
    	     strcat(buffer, "\n");
    	     p += strlen(p);
    	}
        fputs(buffer, fd);
    }
}

/*
 * Write a DB file out.
 */
void
TrackList::writefile() {
    FILE	*fd;
    int	i;

    if ((fd = fopen(filename.cstring(), "w")) == NULL) {
    	char	*buffer = new char [strlen(filename.cstring()) + 50];
    	sprintf(buffer, "Unable to open %s for writing", 
    		filename.cstring());
    	app_error(buffer);
    	return;
    }
    /* Write the header */
    fputs("# xmcd\n#\n# Track frame offsets:\n", fd);
    /* Now write out the track offsets */
    for(i = 0; i < ntracks; i++) 
    	fprintf(fd, "#\t%d\n", tracks[i].getoffset());
    fprintf(fd, "#\n# Disc length: %d seconds\n#\n", disclen);
    revision++;
    fprintf(fd, "# Revision: %d\n", revision);
    fputs("# Submitted via: xcdplay 1.0\n#\n", fd);
    writestring(fd, "DISCID=", ids);
    writestring(fd, "DTITLE=", title);
    for(i = 0; i < ntracks; i++) {
       char	tname[10];

       sprintf(tname, "TTITLE%d=", i);
       writestring(fd, tname, tracks[i].gettrk());
    }
    writestring(fd, "EXTD=", extd);
    for(i = 0; i < ntracks; i++) {
       char	ename[10];

       sprintf(ename, "EXTT%d=", i);
       writestring(fd, ename, tracks[i].getext());
    }
    writestring(fd, "PLAYORDER=", playorder);
    changed = 0;
    fclose(fd);
}


/*
 * Make a edit window for this track list.
 */
void
TrackList::editTrack() {
    Widget      w1, mgr, trkmgr, playmgr;
    char        **newlist, *po = playorder.cstring();
    int		*play; 
    int         i, ig, n;
    Arg		arg[2];

    if (dialog != NULL)
        return;

   /* Figure out size of play list */
    ignore = new char [ntracks+1];
    for(i = 0; i < ntracks; ignore[i++] = 0);
   /* Count number of entries and update ignore list */
    for (ig = 0, n = 0; *po != '\0' && *po != '\n'; po++) {
        if (*po == '-')
            ig = 1;
        else if (*po >= '0' && *po <= '9')
            n = (n * 10) + (*po - '0');
        else if (*po == ',') {
            if (n >= 1 && n <=ntracks) 
               ignore[n-1] = ig;
            ig = n = 0;
        }
    }

   /* Fill in last entry */
    if (n >= 1 && n <=ntracks) 
        ignore[n-1] |= ig;

    dialog = XpwDialogCreateDialog(toplevel, "EditTrack", NULL, 0);
    XtUnmanageChild(XpwDialogGetChild(dialog, XpwDialog_Help_Button));
    XtAddCallback(XpwDialogGetChild(dialog, XpwDialog_Cancel_Button),
        XtNcallback, &TrackList::dialogdone, (XtPointer)this);
    XtAddCallback(XpwDialogGetChild(dialog, XpwDialog_Ok_Button),
        XtNcallback, &TrackList::editok, (XtPointer)this);
    w1 = XtCreateManagedWidget("apply", commandWidgetClass, dialog, NULL, 0);
    XtAddCallback(w1, XtNcallback, &TrackList::editapply, (XtPointer)this);
    w1 = XtCreateManagedWidget("showplay", commandWidgetClass, dialog, NULL, 0);
    XtAddCallback(w1, XtNcallback, &TrackList::showplay, (XtPointer)this);
    w1 = XtCreateWidget("hideplay", commandWidgetClass, dialog, NULL, 0);
    XtAddCallback(w1, XtNcallback, &TrackList::hideplay, (XtPointer)this);
    XtSetArg(arg[0], XtNorientation, XtorientHorizontal);
    mgr = XtCreateWidget("manager", rowColWidgetClass, dialog, arg, 1);
    trkmgr = XtCreateManagedWidget("tmanager", rowColWidgetClass, mgr, NULL, 0);
    XtSetArg(arg[0], XtNtraversalOn, TRUE);
    XtSetArg(arg[1], XtNfocusGroup, dialog);
    w1 = XtCreateManagedWidget("title", textLineWidgetClass, trkmgr, arg, 2);
    XpwTextLineSetString(w1, title.cstring());
    XtCreateManagedWidget("list", labelWidgetClass, trkmgr, NULL, 0);
    w1 = XtCreateManagedWidget("tracklist", listWidgetClass, trkmgr, NULL, 0);
    XtAddCallback(w1, XtNcallback, &TrackList::trackselect, (XtPointer)this);
    XtAddCallback(w1, XtNmultiCallback, &TrackList::trackselect, (XtPointer)this);

    XpwListNew(w1);
    newlist = new char * [ntracks+1];
    for(i = 0; i < ntracks; i++) {
        Str s = tracks[i].gettrk();
        char *p = s.cstring();

        newlist[i] = new char [ strlen(p) + 8];
        sprintf(newlist[i], "%2d - %s", i+1, p);
        XpwListAddItem(w1, newlist[i], -1, 0,
    	     ignore[i]?XpwListCrossout:XpwListNone, 0);
    }
    newlist[i] = NULL;
    XtCreateManagedWidget("tracklabel", labelWidgetClass, trkmgr, NULL, 0);
    w1 = XtCreateManagedWidget("tracktitle", textLineWidgetClass, trkmgr, arg, 2);
    XpwTextLineSetString(w1, "");
    XtAddCallback(w1, XtNcallback, &TrackList::trackchange, (XtPointer)this);

   /* Create play list */
    playmgr = XtCreateWidget("pmanager", tablerWidgetClass, mgr, NULL, 0);
    w1 = XtCreateManagedWidget("clear", commandWidgetClass, playmgr, NULL, 0);
    XtAddCallback(w1, XtNcallback, &TrackList::playclear, (XtPointer)this);
    w1 = XtCreateManagedWidget("addall", commandWidgetClass, playmgr, NULL, 0);
    XtAddCallback(w1, XtNcallback, &TrackList::playaddall, (XtPointer)this);
    w1 = XtCreateManagedWidget("add", commandWidgetClass, playmgr, NULL, 0);
    XtAddCallback(w1, XtNcallback, &TrackList::playadd, (XtPointer)this);
    XtSetSensitive(w1, False);
    w1 = XtCreateManagedWidget("remove", commandWidgetClass, playmgr, NULL, 0);
    XtAddCallback(w1, XtNcallback, &TrackList::playremove, (XtPointer)this);
    XtSetSensitive(w1, False);
    w1 = XtCreateManagedWidget("delete", commandWidgetClass, playmgr, NULL, 0);
    XtAddCallback(w1, XtNcallback, &TrackList::playdelete, (XtPointer)this);
    XtSetSensitive(w1, False);
    w1 = XtCreateManagedWidget("undelete", commandWidgetClass, playmgr, NULL, 0);
    XtAddCallback(w1, XtNcallback, &TrackList::playundelete, (XtPointer)this);
    XtSetSensitive(w1, False);
    w1 = XtCreateManagedWidget("playlist", listWidgetClass, playmgr, NULL, 0);
    XtAddCallback(w1, XtNcallback, &TrackList::playplay, (XtPointer)this);
    XtAddCallback(w1, XtNmultiCallback, &TrackList::playplay, (XtPointer)this);
    XpwListNew(w1);
    po = playorder.cstring();
    for (n = 0, ig = 0; *po != '\0' && *po != '\n'; po++) {
        if (*po == '-')
            ig = 1;
        else if (*po >= '0' && *po <= '9')
            n = (n * 10) + (*po - '0');
        else if (*po == ',') {
            if (n >= 1 && n <=ntracks) {
                if (!ig)
                   XpwListAddItem(w1, newlist[n-1], -1, 0, 0, 0);
            }
            ig = n = 0;
        }
    }
    /* Handle last entry */
    if (n >= 1 && n <=ntracks) {
        if (!ig)
            XpwListAddItem(w1, newlist[n-1], -1, 0, 0, 0);
    }
    XtSetArg(arg[0], XtNbitmap, Bitmaps[top]);
    XtSetArg(arg[1], XtNbitmapMask, Bitmask[top]);
    w1 = XtCreateManagedWidget("top", commandWidgetClass, playmgr, arg, 2);
    XtAddCallback(w1, XtNcallback, &TrackList::playmove, (XtPointer)this);
    XtSetSensitive(w1, False);
    XtSetArg(arg[0], XtNbitmap, Bitmaps[up]);
    XtSetArg(arg[1], XtNbitmapMask, Bitmask[up]);
    w1 = XtCreateManagedWidget("up", commandWidgetClass, playmgr, arg, 2);
    XtAddCallback(w1, XtNcallback, &TrackList::playmove, (XtPointer)this);
    XtSetSensitive(w1, False);
    XtSetArg(arg[0], XtNbitmap, Bitmaps[down]);
    XtSetArg(arg[1], XtNbitmapMask, Bitmask[down]);
    w1 = XtCreateManagedWidget("down", commandWidgetClass, playmgr, arg, 2);
    XtAddCallback(w1, XtNcallback, &TrackList::playmove, (XtPointer)this);
    XtSetSensitive(w1, False);
    XtSetArg(arg[0], XtNbitmap, Bitmaps[bot]);
    XtSetArg(arg[1], XtNbitmapMask, Bitmask[bot]);
    w1 = XtCreateManagedWidget("bot", commandWidgetClass, playmgr, arg, 2);
    XtAddCallback(w1, XtNcallback, &TrackList::playmove, (XtPointer)this);
    XtSetSensitive(w1, False);

    tnum = 0;

    for(i = 0; i < ntracks; i++) 
        delete [] newlist[i];

    XtManageChild(mgr);
    XtManageChild(dialog);
}

void
TrackList::showplay(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget	wid;

    wid = XtNameToWidget(XtParent(w), "manager.pmanager");
    XtManageChild(wid);
    wid = XtNameToWidget(XtParent(w), "hideplay");
    XtChangeManagedSet(&w, 1, NULL, NULL, &wid, 1);
}

void
TrackList::hideplay(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget	wid;

    wid = XtNameToWidget(XtParent(w), "manager.pmanager");
    XtUnmanageChild(wid);
    wid = XtNameToWidget(XtParent(w), "showplay");
    XtChangeManagedSet(&w, 1, NULL, NULL, &wid, 1);
}

void
TrackList::trackchange(Widget w, XtPointer client_data, XtPointer call_data)
{
    TrackList    * tl = (TrackList *)client_data;
    Widget	wid = XtNameToWidget(XtParent(w), "tracklist");
    XpwTextLineReturnStruct	*rs = (XpwTextLineReturnStruct *)call_data;
    XpwListReturnStruct		lrs, *psel;
    int 		i;
    char		*p = new char [ strlen(rs->string) + 8];

    if (tl->tnum >= 0 && tl->tnum < tl->ntracks) {
        i = tl->tnum++;

        sprintf(p, "%2d - ", i+1);
        strcat(p, rs->string);
        XpwListClearItem(wid, i, FALSE);
        XpwListReplaceItem(wid, p, i, 0, 
    	     tl->ignore[i]?XpwListCrossout:XpwListNone, 0);
    }

  /* Update playlist names */
    wid = XtNameToWidget(XtParent(XtParent(w)), "pmanager.playlist");

   /* Turn off display while we work */
    XpwListDisableDisplay(wid);
    
    i = 0; 
    while (1) {
        XpwListReturnStruct		lrs;

        (void)XpwListGetItem(wid, i, &lrs);
        if (lrs.string == NULL)
           break;

        int n = 0;
        if (lrs.string[0] != ' ')
            n = (lrs.string[0] - '0') * 10;
        n += (lrs.string[1] - '0');
	if (n == tl->tnum) 
            XpwListReplaceItem(wid, p, i, 0, 0, 0);
        i++;
    } 
 
    XpwListEnableDisplay(wid);

    if (tl->tnum < 0 || tl->tnum >= tl->ntracks) 
        tl->tnum = 0;
    wid = XtNameToWidget(XtParent(w), "tracklist");
    XpwListSetItem(wid, tl->tnum, TRUE);
    XpwListGetSelected(wid, &lrs);
    XpwTextLineSetString(w, (lrs.string==NULL)? "" : &(lrs.string)[5]);
}
    
void
TrackList::trackselect(Widget w, XtPointer client_data, XtPointer call_data)
{
    TrackList    * tl = (TrackList *)client_data;
    Widget	wid = XtNameToWidget(XtParent(w), "tracktitle");
    XpwListReturnStruct	*rs = (XpwListReturnStruct *)call_data;

    tl->tnum = rs->index;
    if (tl->tnum < 0 || tl->tnum >= tl->ntracks) 
        tl->tnum = 0;
    XpwTextLineSetString(wid, (rs->string == NULL) ? "" : &(rs->string)[5]);

    playtrack(w, client_data, NULL);
}

void
TrackList::editapply(Widget w, XtPointer client_data, XtPointer call_data)
{
    TrackList    * tl = (TrackList *)client_data;
    XpwListReturnStruct		lrs;
    Arg		arg[1];
    Widget	wid = XtNameToWidget(XtParent(w), "manager.tmanager.title");
    Widget	tlw = XtNameToWidget(XtParent(w), "manager.tmanager.tracklist");
    Widget	plw = XtNameToWidget(XtParent(w), "manager.pmanager.playlist");
    Str		tlist, nlist;
    char	buffer[10];
    int		i;

    char *p = XpwTextLineGetString(wid);
    if (tl->title != Str(p)) {
        tl->title = p;
        tl->changed = 1;
    }
    XtFree(p);

    for (i = 0; i < tl->ntracks; i++) {
        (void)XpwListGetItem(tlw, i, &lrs);
        if (tl->tracks[i].gettrk() != Str(&lrs.string[5])) {
    	    tl->tracks[i].settrk(Str(&lrs.string[5]));
    	    tl->changed = 1;
        }
        if (tl->ignore[i]) {
            sprintf(buffer, ",-%d", i+1);
            tlist = tlist + buffer;
        }
    } 

    i = 0; 
    while (1) {
        (void)XpwListGetItem(plw, i, &lrs);
        if (lrs.string == NULL)
           break;

        int n = 0;
        if (lrs.string[0] != ' ')
            n = (lrs.string[0] - '0') * 10;
        n += (lrs.string[1] - '0');
        sprintf(buffer, ",%d", n);
        tlist = tlist + buffer;
        i++;
    } 

    if ((tlist.cstring())[0] == ',') 
	nlist = &((tlist.cstring())[1]);
    else
    	nlist = tlist;

    if (nlist != tl->playorder) {
        tl->playorder = nlist;
        tl->changed = 1;
    }

    if (tl->changed)
    	tl->updated = 1;
} 

void
TrackList::editsave(Widget parent)
{
    Widget	w;

    if (changed) {
        w = XpwDialogCreateQuestion(parent, "SaveQuest", NULL, 0);
        XtUnmanageChild(XpwDialogGetChild(w, XpwDialog_Help_Button));
        XtAddCallback(XpwDialogGetChild(w, XpwDialog_Ok_Button),
            XtNcallback, &TrackList::saveok, (XtPointer)this);
        XpwDialogDisplay(w);
        XtDestroyWidget(w);
    }
}

void
TrackList::editok(Widget w, XtPointer client_data, XtPointer call_data)
{
    TrackList    * tl = (TrackList *)client_data;

   /* Collect any changes */
    editapply(w, client_data, call_data);

   /* Build question dialog box */
    tl->editsave(tl->dialog);

    XtUnmanageChild(tl->dialog);
    XtDestroyWidget(tl->dialog);
    tl->dialog = NULL;
    delete [] tl->ignore;
    tl->ignore = NULL;
} 

void
TrackList::saveok(Widget w, XtPointer client_data, XtPointer call_data)
{
    TrackList    * tl = (TrackList *)client_data;
    tl->writefile();
}

void
TrackList::dialogdone(Widget w, XtPointer client_data, XtPointer call_data)
{
    TrackList    * tl = (TrackList *)client_data;
    if (tl->dialog != NULL) {
    	XtDestroyWidget(tl->dialog);
        tl->dialog = NULL;
    }
    delete [] tl->ignore;
    tl->ignore = NULL;
}


void
TrackList::playclear(Widget w, XtPointer client_data, XtPointer call_data)
{
    TrackList    * tl = (TrackList *)client_data;
    Widget	plw = XtNameToWidget(XtParent(w), "playlist");

    XpwListNew(plw);
    playplay(plw, client_data, NULL);
}

void
TrackList::playaddall(Widget w, XtPointer client_data, XtPointer call_data)
{
    TrackList    * tl = (TrackList *)client_data;
    Widget	plw = XtNameToWidget(XtParent(w), "playlist");
    Widget	tlw = XtNameToWidget(XtParent(XtParent(w)), "tmanager.tracklist");

    for (int i = 0; i < tl->ntracks; i++) {
        XpwListReturnStruct		lrs;

        if (tl->ignore[i] != 0)
            continue;
        (void)XpwListGetItem(tlw, i, &lrs);
        XpwListAddItem(plw, lrs.string, -1, 0, 0, 0);
    } 

   /* Reset pointer */
    XpwListClearAll(tlw);
    XpwListClearAll(plw);
    playplay(plw, client_data, NULL);
    playtrack(tlw, client_data, NULL);
}

void
TrackList::playadd(Widget w, XtPointer client_data, XtPointer call_data)
{
    TrackList    * tl = (TrackList *)client_data;
    Widget	plw = XtNameToWidget(XtParent(w), "playlist");
    Widget	tlw = XtNameToWidget(XtParent(XtParent(w)), "tmanager.tracklist");
    XpwListReturnStruct		*rs;
    int		count = XpwListGetAllSelected(tlw, &rs);

    for(int i = 0; i < count; i++) 
    if (tl->ignore[i] == 0)
       	XpwListAddItem(plw, rs[i].string, -1, 0, 0, 0);

    XpwListClearAll(tlw);
    XtFree((char *)rs);
    playtrack(tlw, client_data, NULL);
}

void
TrackList::playremove(Widget w, XtPointer client_data, XtPointer call_data)
{
    TrackList    * tl = (TrackList *)client_data;
    Widget	plw = XtNameToWidget(XtParent(w), "playlist");
    XpwListReturnStruct		*rs;
    int		count = XpwListGetAllSelected(plw, &rs);

    for(int i = count-1; i >= 0 ; i--) 
        XpwListDeleteItems(plw, rs[i].index, rs[i].index+1);

    XtFree((char *)rs);
    XpwListSetItem(plw, 0, TRUE);
    XpwListClearAll(plw);
    playplay(plw, client_data, NULL);
}

void
TrackList::playdelete(Widget w, XtPointer client_data, XtPointer call_data)
{
    TrackList    * tl = (TrackList *)client_data;
    Widget	tlw = XtNameToWidget(XtParent(XtParent(w)), "tmanager.tracklist");
    XpwListReturnStruct		*rs;
    int		count = XpwListGetAllSelected(tlw, &rs);

   /* Turn off display while we work */
    XpwListDisableDisplay(tlw);
    
    for (int i = 0; i < count; i++) {
        XpwListReplaceItem(tlw, rs[i].string, rs[i].index, 0, XpwListCrossout, 0);
        tl->ignore[rs[i].index] = 1;
    } 
    XtFree((char *)rs);

   /* Reset pointer */
    XpwListClearAll(tlw);
    XpwListEnableDisplay(tlw);
    playtrack(tlw, client_data, NULL);
}
  
void
TrackList::playundelete(Widget w, XtPointer client_data, XtPointer call_data)
{
    TrackList    * tl = (TrackList *)client_data;
    Widget	tlw = XtNameToWidget(XtParent(XtParent(w)), "tmanager.tracklist");
    XpwListReturnStruct		*rs;
    int		count = XpwListGetAllSelected(tlw, &rs);

   /* Turn off display while we work */
    XpwListDisableDisplay(tlw);
    
    for (int i = 0; i < count; i++) {
        XpwListReplaceItem(tlw, rs[i].string, rs[i].index, 0, XpwListNone, 0);
        tl->ignore[rs[i].index] = 0;
    } 
    XtFree((char *)rs);

   /* Reset pointer */
    XpwListClearAll(tlw);
    XpwListEnableDisplay(tlw);
    playtrack(tlw, client_data, NULL);
}

void
TrackList::playmove(Widget w, XtPointer client_data, XtPointer call_data)
{
    TrackList    * tl = (TrackList *)client_data;
    Widget	plw = XtNameToWidget(XtParent(w), "playlist");
    XpwListReturnStruct		*rs;
    char	*p;
    int		newloc = 0;

   /* Grab element */
    XpwListGetAllSelected(plw, &rs);
   /* Save string */
    p = (char *)XtNewString(rs[0].string);
   /* Remove it */
    XpwListDeleteItems(plw, rs[0].index, rs[0].index+1);
    if (strcmp(XtName(w), "top") == 0)
        newloc = 0;
    else if (strcmp(XtName(w), "bot") == 0)
        newloc = -1;
    else if (strcmp(XtName(w), "up") == 0 && rs[0].index != 0)
        newloc = rs[0].index - 1;
    else if (strcmp(XtName(w), "down") == 0)
        newloc = rs[0].index + 1;
    XtFree((char *)rs);
    XpwListAddItem(plw, p, newloc, 0, 0, 0);
    XpwListSetItem(plw, newloc, TRUE);
    playplay(plw, client_data, NULL);
}

void
TrackList::playplay(Widget w, XtPointer client_data, XtPointer call_data)
{
    TrackList    * tl = (TrackList *)client_data;
    Widget	wid = XtParent(w);
    int		count = XpwListGetAllSelected(w, NULL);

    XtSetSensitive(XtNameToWidget(wid, "remove"), count != 0);
    XtSetSensitive(XtNameToWidget(wid, "top"), count == 1);
    XtSetSensitive(XtNameToWidget(wid, "up"), count == 1);
    XtSetSensitive(XtNameToWidget(wid, "down"), count == 1);
    XtSetSensitive(XtNameToWidget(wid, "bot"), count == 1);
}

void
TrackList::playtrack(Widget w, XtPointer client_data, XtPointer call_data)
{
    TrackList    * tl = (TrackList *)client_data;
    Widget	wid = XtNameToWidget(XtParent(XtParent(w)), "pmanager");
    XpwListReturnStruct		*rs;
    int		count = XpwListGetAllSelected(w, &rs);
    int		del = 0, undel = 0;

    for (int i = 0; i < count; i++) {
        if (tl->ignore[rs[i].index] != 0)
    	    undel++;
        else
    	    del++;
    }
    XtFree((char *)rs);

    XtSetSensitive(XtNameToWidget(wid, "add"), (count != 0 && undel == 0));
    XtSetSensitive(XtNameToWidget(wid, "delete"), (count != 0 && del != 0));
    XtSetSensitive(XtNameToWidget(wid, "undelete"), undel != 0);
}

