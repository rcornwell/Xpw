/*
 * Player class.
 *
 * Raw CDrom device.
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

struct rawtrackinfo
{
     int	track;		/* Track number */
     int	length;		/* Lenght of track in seconds */
     int	start;		/* Start frame */
     int	data;		/* Data track */
};

const max_volume = 256;
const min_volume = 128;


enum Cdmode { Empty, New, Valid, Play, Pause, Stop, Eject, Mount };
class Player
{
   private:
     int	device;		/* Open device */
     int	mixdevice;	/* Mixer device */
     Str	name;		/* Name opened on */
     int	id;		/* Idnumber of disc */
     int	numtracks;	/* Number of tracks */
     int	length;		/* Total run time in seconds */
     int	volume;		/* Current volume setting */
     Cdmode	curmode;	/* Current mode */
     int	curtrack;	/* Current track */
     int	curpos;		/* Current position */
     int	frame;		/* Current frame */
     rawtrackinfo *tracks;	/* Track list */

     int	checkstatus();	/* Check cd status */
     int	readtoc();
     int	scalevolume(int value) {
		return ((100 * 100 - (100 - value) * (100 - value)) *
		(max_volume - min_volume) / (100 * 100) + min_volume);
	}
   public:
     Player() { device = -1;
		 curmode = Empty;
		tracks = new rawtrackinfo[1];
		numtracks = 0;
		mixdevice = -1;
	       };
     ~Player() { if (device >= 0) close(device);
		  delete [] tracks; }
     void setdevice(char *devname) { name = devname; }
     void pause();
     void resume();
     void stop();
     void start();
     void eject();
     void setvolume(int value);
     int play_cd(int track, int start, int duration);
     int getdiscid() { return id; }
     int gettracks() { return numtracks; }
     int getlength() { return length; }
     Cdmode getposition(int &track, int &trktime);
     int getvolume();
     int gettracklength(int track) {
	 return (track < 0 || track >= numtracks) ? -1 : tracks[track].length;
	 }
     int getdata(int track) {
         return (track < 0 || track >= numtracks) ? -1 : tracks[track].data;
	 }
     int getframe(int track) {
	 return (track < 0 || track >= numtracks) ? 0 : tracks[track].start;
	 }
};

