/*
 * Three D frame common functions.
 *
 * This is not realy a class, but more if a library. The reason this is not
 * a class, is that functions need to highlight parts of thenselfs rather 
 * than just the border. This library defines helper functions for them.
 * Also the library impliments caching for common highlight types.
 * 
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
 * $Id: threeDdraw.h,v 1.1 1997/10/26 04:22:04 rich Beta $
 *
 * $Log: threeDdraw.h,v $
 * Revision 1.1  1997/10/26 04:22:04  rich
 * Initial revision
 *
 *
 */
#ifndef _threeDdraw_H_
#define _threeDdraw_H_

#include <X11/Xmu/Converters.h>

/* ThreeD Highlight Resources:
 * 
 * Name                Class              RepType         Default Value
 * ----                -----              -------         -------------
 * raised              Raised             Boolean         True
 * frameType           FrameType          FrameType       XfBeveled
 * topShadowPixel      TopShadowPixel     Pixel           XtDefaultForeground 
 * botShadowPixel      BotShadowPixel     Pixel           XtDefaultForeground 
 * topShadowPixmap     TopShadowPixmap    Pixmap          Null 
 * botShadowPixmap     BotShadowPixmap    Pixmap          Null 
 * topShadowContrast   TopShadowContrast  Int             20 
 * botShadowContrast   BotShadowContrast  Int             40 
 * shadowWidth         ShadowWidth        Dimension       2
 * beNiceToColorMap    BeNiceToColorMap   Boolean         True
 * 
 */

typedef enum {
    XfBeveled, XfGrooved, XfEdge, XfOutline, XfNone
} FrameType;

#define XtNraised	"raised"
#define XtCraised	"raised"

#ifndef XtNframeType
#define XtNframeType "frameType"
#endif
#ifndef XtCFrameType
#define XtCFrameType "FrameType"
#endif
#ifndef XtRFrameType
#define XtRFrameType "FrameType"
#endif

typedef enum {
    XaNone, XaSquare, XaDiamond, XaUp, XaDown, XaLeft, XaRight
} ShapeType;

#ifndef XtNshapeType
#define XtNshapeType "shapeType"
#endif
#ifndef XtCShapeType
#define XtCShapeType "ShapeType"
#endif
#ifndef XtRShapeType
#define XtRShapeType "ShapeType"
#endif

#define XtNshadowWidth "shadowWidth"
#define XtCShadowWidth "ShadowWidth"
#define XtNtopShadowPixel "topShadowPixel"
#define XtCTopShadowPixel "TopShadowPixel"
#define XtNbottomShadowPixel "bottomShadowPixel"
#define XtCBottomShadowPixel "BottomShadowPixel"
#define XtNtopShadowContrast "topShadowContrast"
#define XtCTopShadowContrast "TopShadowContrast"
#define XtNbottomShadowContrast "bottomShadowContrast"
#define XtCBottomShadowContrast "BottomShadowContrast"
#define XtNbeNiceToColormap "beNiceToColormap"
#define XtCBeNiceToColormap "BeNiceToColormap"
#define XtNbeNiceToColourmap "beNiceToColormap"
#define XtCBeNiceToColourmap "BeNiceToColormap"

Boolean             cvtStringToFrameType(
#if NeedFunctionPrototypes
Display *, XrmValuePtr, Cardinal *, XrmValuePtr, XrmValuePtr, XtPointer *
#endif
);

Boolean             cvtFrameTypeToString(
#if NeedFunctionPrototypes
Display *, XrmValuePtr, Cardinal *, XrmValuePtr, XrmValuePtr, XtPointer *
#endif
);

Boolean             cvtStringToShapeType(
#if NeedFunctionPrototypes
Display *, XrmValuePtr, Cardinal *, XrmValuePtr, XrmValuePtr, XtPointer *
#endif
);

Boolean             cvtShapeTypeToString(
#if NeedFunctionPrototypes
Display *, XrmValuePtr, Cardinal *, XrmValuePtr, XrmValuePtr, XtPointer *
#endif
);

#endif /*_threeDdraw_H_*/

