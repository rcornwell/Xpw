/*
 * XCdplay: Plays a cd.
 *
 * Handles playlist control.
 *
 * $Log: $
 *
 */

#ifndef lint
static char        *rcsid = "$Id: $";

#endif

/* System stuff */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

/* Include all the widget stuff we need */

/* Local includes */
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
    ignorlist = NULL;
    tlenght = NULL;
    ntrcks = 0;
    randplay = 0;
}

/* Free a playlist */
PlayList::~PlayList()
{
    delete [] playlist;
    delete [] tlenght;
    delete [] ignorlist;
}

/* Change number of tracks */
void
PlayList::setinfo(int num, Str nord)
{
    int		nsize = 0;
    int		ig, n;
    char	*po = nord.cstring();
    
    if (num != ntrcks) {
        delete   []         tlenght;
        delete   []         ignorlist;
	tlenght = new int[num];
	ignorlist = new char[num];
	ntrcks = num;
    }
    for (n = 0; n < num; n++) {
	tlenght[n] = 0;
	ignorlist[n] = 0;
    }
    curtrk = -1;

   /* Remove old list */
    playorder = nord;
    delete []	playlist;

   /* Count number of entries and update ignore list */
    for (ig = 0, n = 0; *po != '\0' && *po != '\n'; po++) {
        if (*po == '-')
	    ig = 1;
        else if (*po >= '0' && *po <= '9')
	    n = (n * 10) + (*po - '0');
        else if (*po == ',') {
  	    if (n >= 1 && n <=ntrcks) 
		    ignorlist[n-1] = ig;
	    if (!ig)
		    nsize++;
	    ig = n = 0;
        }
    }

   /* Fill in last entry */
    if (n >= 1 && n <=ntrcks) 
        ignorlist[n-1] |= ig;
    if (!ig)
	nsize++;

   /* Allocate array */
    if (nsize < ntrcks)
	nsize = ntrcks;
    playlist = new int [nsize];
    playsize = nsize;

   /* Initialize array */
    for (n = 0; n < nsize; n++) 
	playlist[n] = (n > ntrcks)? 0: (n+1);
}


/* Compute time played on cd not including current track */
int
PlayList::timeplayed(int num)
{
    int                 len = 0;

   /* Compute played so far in list */
    for (int i = 0; playlist[i] != num && i < playsize; i++)
	if (playlist[i] != 0)
	    len += tlenght[playlist[i] - 1];
    return len;
}

/* Compute total lenght of playlist */
void
PlayList::totaltime(int &sec, int &trks)
{

    sec = trks = 0;
   /* Compute played so far in list */
    for (int i = 0; i < playsize; i++) {
	if (playlist[i] != 0) {
	    sec += tlenght[playlist[i] - 1];
	    trks++;
	}
    }
}

/* Make simple playlist */

void
PlayList::simplelist(int num)
{
    int                 i, j;

   /* Reinitialize list */
    for (i = 0, j = 0; i < ntrcks; i++)
        if (!ignorlist[i])
	    playlist[j++] = i + 1;

    if (randplay) {
       /* Now randomize the list */
	for (i = 0; i < j; i++) {
	    int                 k;

	    k = int ((float (j) * float (rand())) /float (RAND_MAX));
	    if (i != k && playlist[k] != 0) {
		int                 x;

		x = playlist[i];
		playlist[i] = playlist[k];
		playlist[k] = x;
	    }
	}

   /* If a track is playing... make it the first in list */
	if (num >= 0) {
            for (i = 0; i < j; i++) {
	        if (playlist[i] == num) {
	            int                 x;
    
	            x = playlist[i];
	            playlist[i] = playlist[0];
	            playlist[0] = x;
	            break;
	        }
            }
        }
    }
    setcurtrk(num);
   /* Back up one so we start at correct track */
    if (curtrk >= 0)
	curtrk--;
}

/* Set playmode */
void
PlayList::restart(int num)
{
    int                 i, j;
    char		*po = playorder.cstring();

   /* Reinitialize list */
    if (po != NULL && *po != '\0') {
	int                 n, ig;

	for (n = 0, ig = 0, j = 0; *po != '\0' && *po != '\n'; po++) {
	    if (*po == '-')
		ig = 1;
	    else if (*po >= '0' && *po <= '9')
		n = (n * 10) + (*po - '0');
	    else if (*po == ',') {
		if (n >= 1 && n <=ntrcks) {
		    if (!ig)
		        playlist[j++] = n;
		}
		ig = n = 0;
	    }
	}
	/* Handle last entry */
	if (n >= 1 && n <=ntrcks) {
	    if (!ig)
	        playlist[j++] = n;
	}
	for(n = j; n < ntrcks; playlist[n++] = 0);
    } else {
	for (i = 0, j = 0; i < ntrcks; i++)
	    if (!ignorlist[i])
		playlist[j++] = i + 1;
    }

    if (randplay) {
       /* Now randomize the list */
	for (i = 0; i < j; i++) {
	    int                 k;

	    k = int ((float (j) * float (rand())) /float (RAND_MAX));
	    if (i != k && playlist[k] != 0) {
		int                 x;

		x = playlist[i];
		playlist[i] = playlist[k];
		playlist[k] = x;
	    }
	}

   /* If a track is playing... make it the first in list */
	if (num >= 0) {
            for (i = 0; i < j; i++) {
	        if (playlist[i] == num) {
	            int                 x;
    
	            x = playlist[i];
	            playlist[i] = playlist[0];
	            playlist[0] = x;
	            break;
	        }
            }
        }
    }
    setcurtrk(num);
}
