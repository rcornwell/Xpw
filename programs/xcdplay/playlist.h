/*
 * PlayList Class.
 *
 * Handles playlist generation.
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
 * $Id: playlist.h,v 1.1 1997/12/16 05:48:46 rich Exp rich $
 *
 * $Log: playlist.h,v $
 * Revision 1.1  1997/12/16 05:48:46  rich
 * Initial revision
 *
 *
 */

class               PlayList
{
private:
    int			randplay;
    int                 ntrcks[MAXDRIVES];
    int                 curtrk;
    int                *playlist;
    char               *ignorlist[MAXDRIVES];
    int                *tlenght[MAXDRIVES];
    Str                 playorder[MAXDRIVES];
    int			playsize;

public:
                PlayList();
                ~PlayList();

    void	simplelist(int, int);
    void	restart(int, int);
    void	randomize(int, int, int *, int);
    void        setinfo(int, int, Str);
    int         timeplayed();
    void        totaltime(int, int &, int &);
    void	rolllist(int);

   /* General methods */
    void        setrandom(int mode, int drive, int track) {
	if (mode != randplay) {
	    randplay = mode; 
	    restart(drive, track);
        }
    }

   /* Set a track to ignore */
    void	setignore(int drive, int num) {
	if (num >= 0 && num < ntrcks[drive]) 
	    ignorlist[drive][num] |= 2;
    }

   /* Check if track is being ignored */
    int		getignore(int drive, int num) {
	return (num >= 0 && num < ntrcks[drive]) ? ignorlist[drive][num] : 0;
    }

   /* Set length for a given track */
    void         settracklen(int drive, int num, int len) {
	if (num >= 0 && num < ntrcks[drive])
	    tlenght[drive][num] = len;
    }

   /* Sets the current track */
    void	setcurtrk(int drive, int num) {
	num = (num*MAXDRIVES)+drive;
	curtrk = -1;
	for (int i = 0; i < playsize; i++)
	    if (playlist[i] == num) {
		curtrk = i;
		break;
	    }
	if (curtrk >= 0)
	     rolllist(curtrk);
	curtrk = -1;
    }

   /* Sets to the first track on drive. */
    void	setdrive(int drive) {
	curtrk = -1;
	for (int i = 0; i < playsize; i++)
	    if ((playlist[i]%MAXDRIVES) == drive) {
		curtrk = i;
		break;
	    }
	if (curtrk >= 0)
	    rolllist(curtrk);
	curtrk = -1;
    }

   /* Gets the next track in playlist or 0 if list over */
    int		getnexttrk(int &drive) {
	int next = ((curtrk+1) >= playsize) ? 0 : playlist[++curtrk];
	drive = next % MAXDRIVES;
	return next / MAXDRIVES;
    }
   /* Returns next track without updating curtrack */
    int		peeknexttrk(int &drive) {
	int next = ((1+curtrk) >= playsize) ? 0 : playlist[1+curtrk];
	drive = next % MAXDRIVES;
	return next / MAXDRIVES;
    }
   /* Gets the prev track in playlist or 0 if list over */
    int		getprevtrk(int &drive) {
	int next = (curtrk <= 0) ? 0 : playlist[--curtrk];
	drive = next % MAXDRIVES;
	return next / MAXDRIVES;
    }
};
