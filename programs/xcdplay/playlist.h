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
 * $Id: $
 *
 * $Log: $
 *
 */

class               PlayList
{
private:
    int			randplay;
    int                 ntrcks;
    int                 curtrk;
    int                *playlist;
    char               *ignorlist;
    int                *tlenght;
    Str                 playorder;
    int			playsize;

public:
                PlayList();
                ~PlayList();

    void	simplelist(int);
    void	restart(int);
    void        setinfo(int, Str);
    int         timeplayed(int);
    void        totaltime(int &, int &);

   /* General methods */
    void        setrandom(int mode, int track) {
	if (mode != randplay) {
	    randplay = mode; 
	    restart(track);
        }
    }

   /* Set a track to ignore */
    void	setignore(int num) {
	if (num >= 0 && num < ntrcks) 
	    ignorlist[num] |= 2;
    }

   /* Check if track is being ignored */
    int		getignore(int num) {
	return (num >= 0 && num < ntrcks) ? ignorlist[num] : 0;
    }

   /* Set length for a given track */
    void         settracklen(int num, int len) {
	if (num >= 0 && num < ntrcks)
	    tlenght[num] = len;
    }
   /* Sets the current track */
    void	setcurtrk(int num) {
	curtrk = -1;
	for (int i = 0; i < playsize; i++)
	    if (playlist[i] == num) {
		curtrk = i;
		break;
	    }
    }

   /* Gets the next track in playlist or 0 if list over */
    int		getnexttrk(void) {
	return ((curtrk+1) > playsize) ? 0 : playlist[++curtrk];
    }
   /* Returns next track without updating curtrack */
    int		peeknexttrk(void) {
	return ((1+curtrk) > playsize) ? 0 : playlist[1+curtrk];
    }
   /* Gets the prev track in playlist or 0 if list over */
    int		getprevtrk(void) {
	return (curtrk <= 0) ? 0 : playlist[--curtrk];
    }
};
