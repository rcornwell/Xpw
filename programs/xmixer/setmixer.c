/*
 * setmixer: Control sound card volume settings.
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
#include <strings.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>

static char        *ProgramName;
static char        *mixerdevice = "/dev/mixer";
static char        *devnames[] = SOUND_DEVICE_NAMES;

static struct _mixarray {
    int                 control;
    char              **name;
    int                 value;
    int                 show;
} Mixdata[] = {
    { SOUND_MIXER_VOLUME, &devnames[SOUND_MIXER_VOLUME], -1, 0 },
    { SOUND_MIXER_BASS, &devnames[SOUND_MIXER_BASS], -1, 0 },
    { SOUND_MIXER_TREBLE, &devnames[SOUND_MIXER_TREBLE], -1, 0 },
    { SOUND_MIXER_SYNTH, &devnames[SOUND_MIXER_SYNTH], -1, 0 },
    { SOUND_MIXER_PCM, &devnames[SOUND_MIXER_PCM], -1, 0 },
    { SOUND_MIXER_SPEAKER, &devnames[SOUND_MIXER_SPEAKER], -1, 0 },
    { SOUND_MIXER_LINE, &devnames[SOUND_MIXER_LINE], -1, 0 },
    { SOUND_MIXER_MIC, &devnames[SOUND_MIXER_MIC], -1, 0 },
    { SOUND_MIXER_CD, &devnames[SOUND_MIXER_CD], -1, 0 },
    { SOUND_MIXER_IMIX, &devnames[SOUND_MIXER_IMIX], -1, 0 },
    { SOUND_MIXER_ALTPCM, &devnames[SOUND_MIXER_ALTPCM], -1, 0 },
    { SOUND_MIXER_RECLEV, &devnames[SOUND_MIXER_RECLEV], -1, 0 },
    { SOUND_MIXER_IGAIN, &devnames[SOUND_MIXER_IGAIN], -1, 0 },
    { SOUND_MIXER_OGAIN, &devnames[SOUND_MIXER_OGAIN], -1, 0 },
    { SOUND_MIXER_LINE1, &devnames[SOUND_MIXER_LINE1], -1, 0 },
    { SOUND_MIXER_LINE2, &devnames[SOUND_MIXER_LINE2], -1, 0 },
    { SOUND_MIXER_LINE3, &devnames[SOUND_MIXER_LINE3], -1, 0 },
};

static void
usage()
{
    int i;

    fprintf(stderr, "usage:  %s [-options ...]\n\n", ProgramName);
    fprintf(stderr, "where options include:\n");
    fprintf(stderr, "    -show <channel>       Shows settings for device\n");
    fprintf(stderr, "    -rec <channel>        Set or clear (+/-) record flag\n");
    fprintf(stderr, "    -<channel> value      Set device to value\n");
    fprintf(stderr, "    -device name          Name of mixer device\n");
    fprintf(stderr, " <channel> is ");
    for(i = 0; i<((sizeof(devnames)/sizeof(char *))-1); i++) {
	if (i != 0 && (i%10) == 0)
		fprintf(stderr, "\n             ");
	fprintf(stderr, "%s,", devnames[i]);
    }
    fprintf(stderr, "%s or all\n", devnames[i]);
    exit(0);
}

void
main(argc, argv)
	int                 argc;
	char              **argv;
{
    int                 mixdev, mixmask, recmask, recsrc;
    int                 recclear = 0;
    int                 recset = 0;
    int                 i;
    char               *p, *p1;
    int                 all = 0;
    int                 value, left;
    int                 NumDevs = (sizeof(Mixdata) / sizeof(struct _mixarray));

    ProgramName = argv[0];

   /* Scan args */
    for (;--argc > 0;) {
	p = *++argv;
	if (argc == 0 || strcmp(p, "-help") == 0)
	    usage();
	p1 = *++argv;
        argc--;
	if (strcmp(p, "-rec") == 0) {
	    char                state = *p1++;

	    all = 0;
	    if (p1 == NULL || strcmp(p1, "all") == 0)
		all = 1;
	    for (i = 0; i < (NumDevs); i++) {
		if (all || strcmp(p1, *Mixdata[i].name) == 0) {
		    if (state == '+')
			recclear |= (1 << Mixdata[i].control);
		    else
			recset |= (1 << Mixdata[i].control);
		}
		continue;
	    }
	    continue;
	}
	if (*p1 == '-') {
	    p1 = NULL;
	    argv--;
	    argc++;
	}
	if (p1 != NULL && (strcmp(p, "-device") == 0 || strcmp(p, "-dev") == 0)) {
	    mixerdevice = p1;
	    continue;
	}
	if (strcmp(p, "-show") == 0) {
	    all = 0;
	    if (p1 == NULL || strcmp(p1, "all") == 0)
		all = 1;
	    for (i = 0; i < NumDevs; i++) {
		if (all || strcmp(p1, *Mixdata[i].name) == 0)
		    Mixdata[i].show = 1;
	    }
	    continue;
	} 
	for (left = -1, value = 0; *p1 != '\0'; p1++) {
	    if (*p1 == ',') {
		if (left < 0)
		    left = value;
		else
		    usage();
		value = 0;
	    } else if (*p1 >= '0' && *p1 <= '9')
		value = value * 10 + *p1 - '0';
	    else
		usage();
	}

	if (value > 100)
	    value = 100;
	if (left > 100)
	    left = 100;
	if (left == -1)
	    value = (value << 8) + value;
	else
	    value = (left << 8) + value;

	for (i = 0; i < NumDevs; i++) {
	    if (strcmp(&p[1], *Mixdata[i].name) == 0) {
		Mixdata[i].value = value;
		break;
	    }
	}
	if (i == NumDevs)
	    usage();
    }
   /* Open mixer */
    if ((mixdev = open(mixerdevice, O_RDWR)) < 0) {
	fprintf(stderr, "%s: Unable to open mixer device %s\n",
		ProgramName, mixerdevice);
	exit(0);
    }
   /* Get device masks */
    if (ioctl(mixdev, SOUND_MIXER_READ_DEVMASK, &mixmask) < 0) {
	fprintf(stderr, "%s: Unable get list of devices\n", ProgramName);
	mixmask = 0xffffff;
    }
    if (ioctl(mixdev, SOUND_MIXER_READ_RECMASK, &recmask) < 0) {
	fprintf(stderr, "%s: Unable get list of record devices\n", ProgramName);
	recmask = 0;
    }
    if (ioctl(mixdev, SOUND_MIXER_READ_RECSRC, &recsrc) < 0) {
	fprintf(stderr, "%s: Unable get recording sources\n", ProgramName);
	recsrc = recmask;
    }
   /* Update record sources. */
    if (recclear != 0 || recset != 0) {
	value = recset | ((~recclear) & recsrc);
	if (value != recsrc && ioctl(mixdev, SOUND_MIXER_WRITE_RECSRC, &value) < 0)
	    fprintf(stderr, "%s: Unable set recording sources\n", ProgramName);
	recsrc = value;
    }
   /* Now set and print the values */
    for (i = 0; i < NumDevs; i++) {
	int                 cntl = 1 << Mixdata[i].control;

	if (Mixdata[i].value > 0) {
	    if ((mixmask & cntl) == 0) {
		fprintf(stderr, "%s: Can't set unsupported device %s\n",
			ProgramName, devnames[i]);
		continue;
	    }

	    if (ioctl(mixdev, MIXER_WRITE(Mixdata[i].control), &Mixdata[i].value) < 0) {
		fprintf(stderr, "%s: Unable to write setting for %s\n",
			    ProgramName, devnames[i]);
	    }
	}
	if ((Mixdata[i].show != 0) && (mixmask & cntl) != 0) {
	    if (ioctl(mixdev, MIXER_READ(Mixdata[i].control), &value) < 0) {
	        fprintf(stderr, "%s: Unable to read setting for %s\n",
		        ProgramName, devnames[i]);
	    } else {
	        left = 0xff & (value >> 8);
	        value &= 0xff;
	        if (left != value)
		    printf("%s = %d,%d", devnames[i], left, value);
	        else
		    printf("%s = %d", devnames[i], value);
	        if ((cntl & recsrc & recmask) != 0)
		    printf(" +rec");
	        printf("\n");
	    }
	}
    }
    close(mixdev);
}
