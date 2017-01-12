/*
 * XCdplay: Plays a cd.
 *
 * Handles playlist control.
 *
 * $Log: playlist.cc,v $
 * Revision 1.2  1998/01/26 01:02:02  rich
 * Added shuffle support.
 * Added List rolling.
 *
 * Revision 1.1  1997/12/16 05:48:46  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: playlist.cc,v 1.2 1998/01/26 01:02:02 rich Exp rich $";

#endif

/* System stuff */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

/* Include all the widget stuff we need */
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>

/* To get Maxdrives correct */
#ifdef linux
#include <linux/cdrom.h>
#include <linux/ucdrom.h>
#else
#include <sys/cdio.h>
#endif

/* Local includes */
#include "xcdplay.h"
#include "string.h"
#include "playlist.h"

/* Make a new playlist */
PlayList::PlayList()
{
    time_t              tim;

    time(&tim);
    srand(tim);
    playlist = new int [2];
    playsize = -2;
    for(int i = 0; i<MAXDRIVES; i++) {
	ignorlist[i] = NULL;
    	tlenght[i] = NULL;
    	ntrcks[i] = 0;
    }
    randplay = 0;
}

/* Free a playlist */
PlayList::~PlayList()
{
    for(int i = 0; i<MAXDRIVES; delete [] ignorlist[i++]);
    for(int i = 0; i<MAXDRIVES; delete [] tlenght[i++]);
    delete [] playlist;
}

/* Change number of tracks */
void
PlayList::setinfo(int drive, int num, Str nord)
{
    int		nsize = 0;
    int		ig, n, i;
    char	*po;

    
    if (num != ntrcks[drive]) {
        delete   []         tlenght[drive];
        delete   []         ignorlist[drive];
	tlenght[drive] = new int[num];
	ignorlist[drive] = new char[num];
	ntrcks[drive] = num;
    }
    for (n = 0; n < num; n++) {
	tlenght[drive][n] = 0;
	ignorlist[drive][n] = 0;
    }
    curtrk = -1;

   /* Remove old list */
    playorder[drive] = nord;

   /* Count number of entries and update ignore list */
    for (i = 0; i < MAXDRIVES; i++) {
	po = playorder[i].cstring();
        for (ig = 0, n = 0; *po != '\0' && *po != '\n'; po++) {
            if (*po == '-')
	        ig = 1;
            else if (*po >= '0' && *po <= '9')
	        n = (n * 10) + (*po - '0');
            else if (*po == ',') {
  	        if (n >= 1 && n <=ntrcks[i]) 
		        ignorlist[i][n-1] = ig;
	        if (!ig)
		        nsize++;
	        ig = n = 0;
            }
        }

       /* Fill in last entry */
        if (n >= 1 && n <=ntrcks[i]) 
            ignorlist[i][n-1] |= ig;
        if (!ig)
	    nsize++;
    }

    for(i = 0, n = 0; i<MAXDRIVES; n += ntrcks[i++]);

    if (nsize < n)
	nsize = n;

   /* Allocate array */
    delete []	playlist;
    playlist = new int [nsize + 1];
    playsize = nsize;

   /* Initialize array */
    for (n = 0, i = 0, ig = 0; n < nsize; n++) {
	if (i > ntrcks[ig]) {
	   ig++;
	   i = 0;
	}
        playlist[n] = ((i+1) * MAXDRIVES) + ig;
	i++;
    }
}


/* Compute time played on cd not including current track */
int
PlayList::timeplayed()
{
    int                 len = 0;

   /* Compute played so far in list */
    for (int i = 0; i != curtrk && i < playsize; i++)
	if (playlist[i] != 0) {
	    int d = playlist[i] % MAXDRIVES;
	    len += tlenght[d][(playlist[i]/MAXDRIVES) - 1];
	}
    return len;
}

/* Compute total lenght of playlist */
void
PlayList::totaltime(int drive, int &sec, int &trks)
{

    sec = trks = 0;
   /* Compute played so far in list */
    for (int i = 0; i < playsize; i++) {
	if (playlist[i] != 0) {
	    int d = playlist[i] % MAXDRIVES;
	    if (d == drive) {
	        sec += tlenght[d][(playlist[i]/MAXDRIVES) - 1];
	        trks++;
	    }
	}
    }
}

void
PlayList::rolllist(int num)
{
    int	tlist[playsize];
    int i, j;

    for(i = num, j = 0; playlist[i] != 0 && i < playsize; 
	tlist[j++] = playlist[i++]);
    for(i = 0; i < num; tlist[j++] = playlist[i++]);
    for(; j < playsize; tlist[j++] = 0);
    for(i = 0; i < playsize; i++)
	playlist[i] = tlist[i];
}

void
PlayList::randomize(int drive, int num, int *md, int size)
{
    int base[MAXDRIVES];

    for(int i = 0, b = 0; i< MAXDRIVES; i++) {
	base[i] = b;
	b += md[i];
    }

   /* Now randomize the list */
    for (int i = 0; i < size; i++) {
	int x, d, k;

	d = playlist[i]%MAXDRIVES;
	x = (randplay == 2)?md[d]:size;
	k = int ((float (x) * float (rand())) /float (RAND_MAX));
	if (randplay == 2)
	   k += base[d];
	if (i != k && playlist[k] != 0) {
	    int                 z;

	    z = playlist[i];
	    playlist[i] = playlist[k];
	    playlist[k] = z;
        }
    }
}


/* Make simple playlist */
void
PlayList::simplelist(int drive, int num)
{
    int                 i, j, k, n;
    int			md[MAXDRIVES];

   /* Reinitialize list */
    for (k = 0, j = 0; k < MAXDRIVES; k++) {
        for (i = 0, n = 0; i < ntrcks[k]; i++) {
            if (!ignorlist[k][i]) {
	        playlist[j++] = ((i + 1) * MAXDRIVES) + k;
	        n++;
	    }
	}
	md[k] = n;
    }

    if (randplay) 
	randomize(drive, num, md, j);
    setcurtrk(drive, num);
   /* Back up one so we start at correct track */
    if (curtrk >= 0)
	curtrk--;
}

/* Set playmode */
void
PlayList::restart(int drive, int num)
{
    int                 i, j, d, t;
    int			md[MAXDRIVES];

   /* Reinitialize list */
    for(d = 0, j = 0; d < MAXDRIVES; d++) {
	int                 n, ig;
        char		    *po;

	po = playorder[d].cstring();
	t = 0;

	if (*po == '\0') {
	    for (i = 0; i < ntrcks[d]; i++) {
	        if (!ignorlist[d][i]) {
		    playlist[j++] = (i + 1)*MAXDRIVES + d;
		    t++;
		}
	    }
	} else {

	    for (n = 0, ig = 0; *po != '\0' && *po != '\n'; po++) {
	        if (*po == '-')
		    ig = 1;
	        else if (*po >= '0' && *po <= '9')
		    n = (n * 10) + (*po - '0');
	        else if (*po == ',') {
		    if (n >= 1 && n <=ntrcks[d]) {
		        if (!ig) {
		            playlist[j++] = n*MAXDRIVES + d;
			    t++;
			}
		    }
		    ig = n = 0;
	        }
	    }
	    /* Handle last entry */
	    if (n >= 1 && n <=ntrcks[d]) {
	        if (!ig) {
	            playlist[j++] = n*MAXDRIVES + d;
		    t++;
		}
	    }
	}
	md[d] = t;
    }
    for(i = j; i < playsize; playlist[i++] = 0);

    if (randplay) {
	randomize(drive, num, md, j);
    }
    setcurtrk(drive, num);
}
