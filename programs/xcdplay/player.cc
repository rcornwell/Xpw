/*
 * XCdplay: Plays a cd.
 *
 * Code to handle main interface panel.
 *
 * $Log: $
 *
 */

#ifndef lint
static char        *rcsid = "$Id: $";
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
 * Read in the table of contents off the cdrom.
 */
int
Player::readtoc()
{
    struct cdrom_tochdr	hdr;
    struct cdrom_tocentry	entry;
    int i, pos;
    
    if (device < 0)
        return 1;
    numtracks = -1;
    length = -1;
    if (ioctl(device, CDROMREADTOCHDR, &hdr) < 0) {
        app_error("Unable to read TOC");
        return 1;
    }
    length = 0;
    numtracks = hdr.cdth_trk1;

    delete [] tracks;
    tracks = new rawtrackinfo[numtracks+1];

    for (i=0; i <= numtracks; i++) {
        entry.cdte_track = (i == numtracks)?CDROM_LEADOUT:(i+1);
        entry.cdte_format = CDROM_MSF;
        if (ioctl(device, CDROMREADTOCENTRY, &entry) < 0) {
            app_error("Unable to read trackdata");
            numtracks = -1;
            length = -1;
            return 1;
        }
        tracks[i].data = entry.cdte_ctrl & CDROM_DATA_TRACK ? 1 : 0;
        tracks[i].length = entry.cdte_addr.msf.minute * 60 +
              entry.cdte_addr.msf.second;
        tracks[i].start = tracks[i].length * 75 +
               entry.cdte_addr.msf.frame;
        tracks[i].track = i + 1;
    }

   /* Set positions correctly */
    pos = tracks[0].length;
    for(i = 0; i < numtracks; i++) {
        tracks[i].length = tracks[i+1].length - pos;
        pos = tracks[i+1].length;
    }

    length = tracks[numtracks].length;

   /* Compute disc id. Make sure same as all other CD programs */
    pos = 0;
    id = 0;
    for (i = 0; i < numtracks; i++) {
        char	buf[12], *p;
        int	sum = 0;

        sprintf(buf, "%lu", tracks[i].start / 75);
        for(p = buf; *p != '\0'; sum += *p++ - '0');
        pos += sum;
        id += tracks[i].length;
    }
    id = ((pos % 0xff) << 24) | (id << 8) | numtracks;
    return 0;
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
    if (device < 0) {
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
        curmode = Empty;
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

   /* If no table of contents. Get one */
    if (curmode == Empty || curmode == Eject) {	/* Cd ejected */
        if (readtoc()) {
            curmode = Mount;
            close(device);
            device = -1;
            return 0;
        }
        curmode = New;
        curpos = 0;
        curtrack = 1;
        frame = 0;
        return 1;
    }

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
        if (curtrack < 1 || frame < tracks[curtrack-1].start ||
            frame >= (curtrack >= numtracks ? (length + 1) * 75:
            tracks[curtrack].start)) {
            for(curtrack = 0; curtrack <= numtracks &&
                        frame >= tracks[curtrack].start; curtrack++);
        }
        if (curtrack >= 1 && sub.cdsc_trk > tracks[curtrack-1].track)
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
    length = 1;
    curpos = 0;
    curmode = Eject;
    return;
}

/* Play a group of tracks */
int
Player::play_cd(int track, int start, int duration)
{
    int spos, epos;
    struct cdrom_msf	msf;

    track--;

    if (track < 0 || track >= numtracks || device < 0)
        return -1;
    if (tracks[track].data)
        return -2;

    spos = tracks[track].start + start * 75;
    if (duration == 0)
        epos = tracks[track+1].start - 1;
    else
        epos = tracks[track].start + duration * 75 - 1;
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
        if (curtrack > 0 && curtrack <= numtracks)  {
            track = curtrack;
            trktime = (frame - tracks[track - 1].start)/75;
        } else {
            track = 1;
            trktime = 0;
        }
    }
    return curmode;
}

