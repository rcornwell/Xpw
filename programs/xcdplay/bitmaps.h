/*
 * XCdplay: Plays a cd.
 *
 * "$Id: $"
 *
 * $Log: $
 *
 */


enum Bitmapnames { icon, toolEject, fastback, fastforw, pausebits, playbut, skipback, skipforw,
		stopbut, toolDisc, toolEditPlay, toolEditTrack, toolElapsed,
	        toolIntro, toolNormal, toolPlay, toolRandom, toolRemaining,
		toolTotal, toolStop, toolExit, toolRepeat, top, bot, up, down };

extern Pixmap	Bitmaps[];
extern Pixmap	Bitmask[];

extern void mkbitmaps(Display *dpy);
