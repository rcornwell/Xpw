/*
 * XCdplay: Plays a cd.
 *
 * Code to handle main interface panel.
 *
 * $Log: player.cc,v $
 * Revision 1.1  1997/12/16 05:48:46  rich
 * Initial revision
 *
 *
 */

#ifndef lint
static char        *rcsid = "$Id: player.cc,v 1.1 1997/12/16 05:48:46 rich Exp rich $";
#endif
    
/* System stuff */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <errno.h>
#ifdef linux
#include <mntent.h>
#include <sys/soundcard.h>
#include <linux/cdrom.h>
#include <linux/ucdrom.h>
#else
#include <sys/mnttab.h>
#include <sys/cdio.h>
#include <ustat.h>
#endif
    
/* X stuff */
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
    
/* Include app stuff */
#include "xcdplay.h"
#include "string.h"
#include "player.h"

/*
 * Set the drive.
 */
int
Player::setdevice(char *devname) {
    if (name != devname) {
	if (device > 0)
	    close(device);
	device = -1;
	name = devname;
        if (!opendrive())
           return MAXDRIVES;
    }
#if MAXDRIVES > 1
    changing = 1;
    for (int i = 0; i < MAXDRIVES; i++ ) {
       if (ioctl(device, CDROM_SELECT_DISC, i)== 0) {
            readtoc(&info[i]);
	    continue;
       }
      /* Linux is broken here, could be either empty Slot or No Slot */
       if (errno == ENOENT)
	    continue;
       changing = 0;
      /* Check if maybe not a changer */
       if (errno == EINVAL) 
	    return (i > 0) ? (i+1) : 1;
       {
	    char buffer[50];

	    sprintf(buffer, "Unable to access slot %d", i + 1);
	    app_error(buffer);
       }
       return i;
   }
   changing = 0;
#else
   readtoc(&info[0]);
#endif
   return MAXDRIVES;
}


/*
 * Read in the table of contents off the cdrom.
 */
int
Player::readtoc(driveinfo *ip)
{
    struct cdrom_tochdr	hdr;
    struct cdrom_tocentry	entry;
    struct rawtrackinfo *rp;
    int i, pos;
    
    if (device < 0)
        return 1;
    ip->numtracks = -1;
    ip->length = -1;
    if (ioctl(device, CDROMREADTOCHDR, &hdr) < 0) {
        app_error("Unable to read TOC");
        return 1;
    }
    ip->length = 0;
    ip->numtracks = hdr.cdth_trk1;

    delete [] ip->tracks;
    rp = ip->tracks = new rawtrackinfo[ip->numtracks+1];

    for (i=0; i <= ip->numtracks; i++, rp++) {
        entry.cdte_track = (i == ip->numtracks)?CDROM_LEADOUT:(i+1);
        entry.cdte_format = CDROM_MSF;
        if (ioctl(device, CDROMREADTOCENTRY, &entry) < 0) {
            app_error("Unable to read trackdata");
            ip->numtracks = -1;
            ip->length = -1;
            return 1;
        }
        rp->data = entry.cdte_ctrl & CDROM_DATA_TRACK ? 1 : 0;
        rp->length = entry.cdte_addr.msf.minute * 60 +
              entry.cdte_addr.msf.second;
        rp->start = rp->length * 75 + entry.cdte_addr.msf.frame;
        rp->track = i + 1;
    }

   /* Set positions correctly */
    pos = ip->tracks[0].length;
    rp = ip->tracks;
    for(i = 0; i < ip->numtracks; i++, rp++) {
        rp->length = rp[1].length - pos;
        pos = rp[1].length;
    }

    ip->length = ip->tracks[ip->numtracks].length;

   /* Compute disc id. Make sure same as all other CD programs */
    pos = 0;
    ip->id = 0;
    rp = ip->tracks;
    for (i = 0; i < ip->numtracks; i++, rp++) {
        char	buf[12], *p;
        int	sum = 0;

        sprintf(buf, "%lu", rp->start / 75);
        for(p = buf; *p != '\0'; sum += *p++ - '0');
        pos += sum;
        ip->id += rp->length;
    }
    ip->id = ((pos % 0xff) << 24) | (ip->id << 8) | ip->numtracks;
    return 0;
}

/*
 * Open the drive.
 */
int
Player::opendrive()
{
    char 	*p;

    curmode = Eject;

   /* If name is a list try each one */
    if ((p = strchr(name.cstring(), ':')) != NULL) {
        char   *p1 = new char [strlen(name.cstring())+1];
        strcpy(p1, name.cstring());
        while(*p1 != '\0') {
            if((p = strchr(p1, ':')) == NULL)
                *p++ ='\0';
            else
                p = &p1[strlen(p1)];
            if ((device = open(p1, O_RDONLY)) >= 0) {
                curmode = Empty;
                break;
            }
            p1 = p;
        }
        return 0;
    } else {
        if ((device = open(name.cstring(), O_RDONLY)) < 0) {
            switch (errno) {
#ifndef sun
            case ENOENT:
#endif
            case EACCES:
            case ENXIO:
                app_fatal("Unable to open CDROM");
#ifdef sun
            case ENOENT:
#endif
                break;
            default:
                curmode = Mount;
            }
            device = -1;
            return 0;
        }
    }
   return 1;
}

/*
 * Update what the CDrom is doing.
 */
int
Player::checkstatus()
{
    struct cdrom_subchnl sub;

    curpos = -1;
    frame = -1;

   /* If the device is not opened. Try to open it */
    if (device < 0 && !opendrive()) 
	return 0;

    if (changing)
	return 1;
#ifdef linux
    if (ioctl(device, CDROM_MEDIA_CHANGED, 0) > 0) {
	fprintf(stderr, "Media Changed\n");
	curmode = Empty;
    }
#endif

   /* If no table of contents. Get one */
    if (curmode == Empty || curmode == Eject) {	/* Cd ejected */
    	for (int i = 0; i < MAXDRIVES; i++ ) 
	    if (ioctl(device, CDROM_SELECT_DISC, i)== 0) 
	        readtoc(&info[i]);
	if (info[curdrive].numtracks == -1) {
            curmode = Mount;
            close(device);
            device = -1;
            return 0;
        }
        ioctl(device, CDROM_SELECT_DISC, curdrive);
        curmode = New;
        curpos = 0;
        curtrack = 1;
        frame = 0;
        return 1;
    }

   /* Try to read device status */
    sub.cdsc_format = CDROM_MSF;
    if (ioctl(device, CDROMSUBCHNL, &sub) < 0) {
        if (errno == EIO) {
            curmode = Eject;
        } else {
            curmode = Empty;
            close(device);
            device = -1;
        }
        curtrack = -1;
        return 0;
    }

    struct driveinfo *ip = &info[curdrive];
   /* Now update frame and track information */	
    switch (sub.cdsc_audiostatus) {
    case CDROM_AUDIO_PAUSED:
        if (curmode == Play || curmode == Pause) 
            curmode = Pause;
        else 
            curmode = Stop;
    case CDROM_AUDIO_PLAY:
        if (sub.cdsc_audiostatus == CDROM_AUDIO_PLAY)
            curmode = Play;
        curpos = sub.cdsc_absaddr.msf.minute * 60 +
                  sub.cdsc_absaddr.msf.second;
        frame = curpos * 75 + sub.cdsc_absaddr.msf.frame;
        if (curtrack < 1 || frame < ip->tracks[curtrack-1].start ||
            frame >= (curtrack >= ip->numtracks ? (ip->length + 1) * 75:
            ip->tracks[curtrack].start)) {
            for(curtrack = 0; curtrack <= ip->numtracks &&
                        frame >= ip->tracks[curtrack].start; curtrack++);
        }
        if (curtrack >= 1 && sub.cdsc_trk > ip->tracks[curtrack-1].track)
            curtrack++;
        break;
    case CDROM_AUDIO_COMPLETED:
        curmode = Valid;
        curpos = 0;
        frame = 0;
        break;
    case CDROM_AUDIO_NO_STATUS:
        curmode = Stop;
        curpos = 0;
        frame = 0;
        break;
    }
    return 1;
}

/* Change to a new device */
int
Player::setdrive(int num)
{
#if MAXDRIVES > 1
    if (device < 0)
	return 0;
    changing = 1;
    if (ioctl(device, CDROM_SELECT_DISC, num) == 0) {
        readtoc(&info[num]);
        curdrive = num;
	changing = 0;
	return 1;
    } else {
	if (errno != ENOENT && errno != EINVAL) {
	    char buffer[50];

	    sprintf(buffer, "Unable to change to slot %d", num + 1);
	    app_error(buffer);
	}
    }
    changing = 0;
    return 0;
#else
    return 1;
#endif
}

/* Pause cdrom */
void
Player::pause()
{
    if (device >= 0 && curmode == Play) {
        curmode = Pause;
        ioctl(device, CDROMPAUSE, NULL);
    }
}

/* Unpause cdrom */
void 
Player::resume()
{
    if (device >= 0 && curmode == Pause) {
        curmode = Play;
        ioctl(device, CDROMRESUME, NULL);
    }
}

/* Stop cdrom drive */
void
Player::stop()
{
    if (device >= 0 && (curmode == Play || curmode == Pause)) {
        curmode = Stop;
        curpos = 0;
        ioctl(device, CDROMSTOP, NULL);
    }
}

/* Eject cdrom */
void 
Player::eject()
{
    struct	stat	statbuf;
    struct ustat	ustatbuf;

    if (curmode == Eject)
        return;

    if (fstat(device, &statbuf) != 0) {
        app_error("Unable to stat CDROM");
        return;
    }
    if (ustat(statbuf.st_rdev, &ustatbuf) == 0) 
        return;

    if (ioctl(device, CDROMEJECT, NULL)) {
        app_error("Unable to eject CDROM");
        return;
    }
#ifndef linux
    close(device);
    device = -1;
#endif
    curtrack = -1;
    info[curdrive].length = 1;
    curpos = 0;
    curmode = Eject;
    return;
}

/* Play a group of tracks */
int
Player::play_cd(int track, int start, int duration)
{
    int spos, epos;
    struct driveinfo *ip = &info[curdrive];
    struct cdrom_msf	msf;

    track--;

    if (track < 0 || track >= ip->numtracks || device < 0)
        return -1;
    if (ip->tracks[track].data)
        return -2;

    spos = ip->tracks[track].start + start * 75;
    if (duration == 0)
        epos = ip->tracks[track+1].start - 1;
    else
        epos = ip->tracks[track].start + duration * 75 - 1;
    msf.cdmsf_min0 = spos / (60*75);
    msf.cdmsf_sec0 = (spos / 75) % 60;
    msf.cdmsf_frame0 = spos % 75; 
    msf.cdmsf_min1 = epos / (60*75);
    msf.cdmsf_sec1 = (epos / 75) % 60;
    msf.cdmsf_frame1 = epos % 75; 

    if (ioctl(device, CDROMSTART, NULL)) {
        app_error("Unable to start CDROM");
        return -1;
    }

    if (ioctl(device, CDROMPLAYMSF, &msf)) {
        app_error("Unable to play CDROM");
        if (errno == EINVAL)
            return -2;
        return -1;
    }
    return 0;
}

/* Read the cdrom volume seting */
int
Player::getvolume()
{
    int	vol;
#ifdef linux

    if (mixdevice < 0) {
        if ((mixdevice = open(resources.mix_device, O_RDWR)) < 0) {
            app_fatal("Unable to open Mixer");
            return -1;
        }
    }
    if (ioctl(mixdevice, SOUND_MIXER_READ_CD,  &vol) < 0)
        app_error("Unable to read volume");
    vol = (((vol>>8) & 0x7f ) + (vol & 0x7f))/ 2;
#else
#ifdef CDROMVOLREAD
    struct cdrom_volctrl v;
    int cdvol, incr, scaled;

    if (device >= 0 && ioctl(device, CDROMVOLREAD, &v) < 0) 
        app_error("Unable to read volume");

    cdvol = (v.channel0 + v.channel1)/2;
    for (vol = 100/2, incr = 100/4 + 1; incr ; incr /= 2) {
        scaled = scalevolume(vol);
        if (cdvol == scaled)
            break;
        if (cdvol < scaled)
            vol -= incr;
        else
            vol += incr;
    }

    if (vol < 0)
        vol = 0;
    else if (vol > 100)
        vol = 100;
#endif
#endif
    return vol;
}

/* Set the cdrom volume */
void
Player::setvolume(int value)
{
#ifdef linux
    if (mixdevice < 0) {
        if ((mixdevice = open(resources.mix_device, O_RDWR)) < 0) {
            app_fatal("Unable to open mixer");
        return;
        }
    }
    value = (value << 8) | value;
    if (ioctl(mixdevice, SOUND_MIXER_WRITE_CD,  &value) < 0)
        app_error("Unable to set volume");
#else
    struct cdrom_volctrl v;

    int vol = scalevolume(value);
    v.channel0 = (vol < min_volume) ? min_volume :
    ((vol > max_volume) ? max_volume : vol);
    v.channel1 = (vol < min_volume) ? min_volume : 
    ((vol > max_volume) ? max_volume : vol);
    if (device >= 0 && ioctl(device, CDROMVOLCTRL, &v) < 0) 
        app_error("Unable to set volume");
#endif
}

/* Return current position, 0 if no runing, 1 if data is valid */
Cdmode
Player::getposition(int &track, int &trktime)
{
    if(checkstatus()) {
        if (curtrack > 0 && curtrack <= info[curdrive].numtracks)  {
            track = curtrack;
            trktime = (frame - info[curdrive].tracks[track - 1].start)/75;
        } else {
            track = 1;
            trktime = 0;
        }
    }
    return curmode;
}

