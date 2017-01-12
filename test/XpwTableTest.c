/* Widget -- test program for the Xpw tabler widget */

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xpw/Tabler.h>
#include <Xpw/Label.h>
#include <Xpw/Command.h>
#include <Xpw/Clue.h>
#include <Xpw/Line.h>

static String       fallback_resources[] =
{

    NULL,
};

static Widget       toplevel, clue, tabler, label, quit;
static XtAppContext app_context;

void
cmdcall(w, client_data, call_data)
	Widget              w;
	XtPointer           client_data;
	XtPointer           call_data;
{
   	String		str;

	fprintf(stderr, "Button(%s)\n", XtName(w));
	XtVaGetValues(w, XtNlabel, &str, NULL);
	XtVaSetValues(label, XtNlabel, str, NULL);
}


int 
main(argc, argv)
	int                 argc;
	String             *argv;
{
	char	buf[20];
	int	i;
	Widget w;
    toplevel = XtVaAppInitialize
	(&app_context, "MenuBarT", NULL, 0, &argc, argv,
	 fallback_resources, NULL);

    clue = XtVaCreatePopupShell( "ClueWindow", clueWidgetClass, toplevel, NULL);


    tabler = XtVaCreateManagedWidget("tabler", tablerWidgetClass, toplevel, 
		XtNcols, 3, NULL);
    label = XtVaCreateManagedWidget("label", labelWidgetClass, tabler, 
		XtNcolSpan, 2, NULL);
    quit = XtVaCreateManagedWidget("quit", commandWidgetClass, tabler, NULL);

    XtAddCallback(quit, XtNcallback, exit, NULL);

    for (i = 0; i <= 10; i++) {
	sprintf(buf, "button%d", i);
	w = XtVaCreateManagedWidget(buf, commandWidgetClass, tabler, NULL);
	XtAddCallback(w, XtNcallback, cmdcall, NULL);
    }

    XtRealizeWidget(toplevel);
    XtAppMainLoop(app_context);
}
