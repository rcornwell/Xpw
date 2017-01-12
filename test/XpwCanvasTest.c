#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xpw/Frame.h>
#include <Xpw/Canvas.h>
#include <Xpw/ViewPort.h>

void InitGraphics(/*GraphicsData *gdp*/);
void ResizeCallback(/*Widget w, XtPointer client_data, XtPointer call_data*/);
void ExposeCallback(/*Widget w, XtPointer client_data, XtPointer call_data*/);
void RedrawAll(/*GraphicsData *gdp*/);

typedef struct {
	Dimension	width, height;
	GC	drawing_gc;
	Widget	canvas;
	XSegment *display_list;
	int	nlines;
} GraphicsData;

main(argc, argv)
int argc;
char **argv;
{
	Widget toplevel, manager, canvas;
	XtAppContext	context;
	GraphicsData	gdata;

	toplevel = XtAppInitialize( &context, "Canvas", NULL, 0, &argc,
		argv, NULL, NULL, 0);

	manager = XtCreateManagedWidget( "manager", viewportWidgetClass, 
			toplevel, NULL, 0);
	gdata.canvas = XtCreateManagedWidget( "canvas", canvasWidgetClass, 
			manager, NULL, 0);

	XtAddCallback(gdata.canvas, XtNresizeCallback, ResizeCallback,
		(XtPointer) & gdata);
	XtAddCallback(gdata.canvas, XtNexposeCallback, ExposeCallback,
		(XtPointer) & gdata);

	XtRealizeWidget(toplevel);
	InitGraphics(&gdata);
	XtAppMainLoop(context);
}

void InitGraphics(gdp)
GraphicsData	*gdp;
{
	Pixel	color;
	XGCValues	gcvalues;
	static XSegment	stardata[] = {
		{17, 85, 87, 31 },
		{87,31,13,31},
		{13, 31, 83, 85 },
		{83, 85, 50, 5 },
		{50, 5, 17, 85 } };
	gdp->display_list = stardata;
	gdp->nlines = XtNumber(stardata);

	XtVaGetValues( gdp->canvas,
		XtNwidth, &(gdp->width), XtNheight, &(gdp->height),
		XtNforeground, &color, NULL);
	gcvalues.foreground = color;
	gdp->drawing_gc = XCreateGC(XtDisplay(gdp->canvas), 
		XtWindow(gdp->canvas), GCForeground, &gcvalues);
}

void ResizeCallback(w, client_data, call_data)
Widget	w;
XtPointer	client_data;
XtPointer	call_data;
{
	GraphicsData *gdp = (GraphicsData *)client_data;
	XtVaGetValues( w, XtNwidth, &(gdp->width), XtNheight, &(gdp->height), NULL);
	if (XtIsRealized(gdp->canvas)) RedrawAll(gdp);
}

void ExposeCallback(w, client_data, call_data)
Widget w;
XtPointer	client_data;
XtPointer	call_data;
{

	GraphicsData *gdp = (GraphicsData *)client_data;
	
	 RedrawAll(gdp);
}

void RedrawAll (gdp)
	GraphicsData *gdp;
{
	int i;
	Widget	canvas = gdp->canvas;

	XClearWindow(XtDisplay(canvas), XtWindow(canvas));
	for (i =0; i < gdp->nlines; i++) {
		XDrawLine( XtDisplay(canvas), XtWindow(canvas), gdp->drawing_gc,
			(gdp->width * gdp->display_list[i].x1 ) / 100,
			(gdp->height * gdp->display_list[i].y1 ) / 100,
			(gdp->width * gdp->display_list[i].x2 ) / 100,
			(gdp->height * gdp->display_list[i].y2 ) / 100);
	}
}


		
