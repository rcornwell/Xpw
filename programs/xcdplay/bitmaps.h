/*
 * XCdplay: Plays a cd.
 *
 * "$Id: bitmaps.h,v 1.1 1997/12/16 05:48:46 rich Exp rich $"
 *
 * $Log: bitmaps.h,v $
 * Revision 1.1  1997/12/16 05:48:46  rich
 * Initial revision
 *
 *
 */


enum Bitmapnames { icon, toolEject, fastback, fastforw, pausebits, playbut, skipback, skipforw,
		stopbut, toolDisc, toolEditPlay, toolEditTrack, toolElapsed,
	        toolIntro, toolNormal, toolPlay, toolRandom, toolRemaining,
		toolTotal, toolStop, toolExit, toolRepeat, toolShuffle,
		top, bot, up, down };

extern Pixmap	Bitmaps[];
extern Pixmap	Bitmask[];

extern void mkbitmaps(Display *dpy);
