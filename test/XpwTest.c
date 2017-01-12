/* Widget -- test program for the Xpw library of widgets */

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xpw/Arrow.h>
#include <Xpw/Paned.h>
#include <Xpw/RowCol.h>
#include <Xpw/Frame.h>
#include <Xpw/TextLine.h>
#include <Xpw/Dialog.h>
#include <Xpw/ComboBox.h>
#include <Xpw/MenuBar.h>
#include <Xpw/MenuButton.h>
#include <Xpw/MenuArrow.h>
#include <Xpw/BarButton.h>
#include <Xpw/Select.h>
#include <Xpw/PopupMenu.h>
#include <Xpw/PmeEntry.h>
#include <Xpw/PmeSelect.h>
#include <Xpw/PmeLine.h>
#include <Xpw/Slider.h>
#include <Xpw/Percent.h>
#include <Xpw/Label.h>
#include <Xpw/Command.h>
#include <Xpw/Clue.h>
#include <Xpw/List.h>
#include <Xpw/Line.h>
#include <Xpw/Scroller.h>

static String       fallback_resources[] =
{

    NULL,
};

static Widget       toplevel, menubar, menu1, menu2, quit;
static Widget       mbut, drop1, drop2, button1, button2, button3, button4;
static Widget       subdrop, sbutton1, sbutton2, sbutton3, sbutton4;
static Widget	    drop3, button5, button6, textline, combobox, col2;
static Widget       form, select1, select2, select3, select4;
static Widget	    label, command, clue, list, scroll, percent, slider;
static XtAppContext app_context;

void 
menucall(w, client_data, call_data)
	Widget              w;
	XtPointer           client_data;
	XtPointer           call_data;
{
    fprintf(stderr, "Menu %s\n", XtName(w));
}

void 
combocall(w, client_data, call_data)
	Widget              w;
	XtPointer           client_data;
	XtPointer           call_data;
{
    XpwComboBoxReturnStruct	    *rp = (XpwComboBoxReturnStruct *)call_data;
    fprintf(stderr, "ComboBox %s %d %s\n", XtName(w), rp->index, rp->string);
}

void 
listcall(w, client_data, call_data)
	Widget              w;
	XtPointer           client_data;
	XtPointer           call_data;
{
    XpwListReturnStruct	    *rp = (XpwListReturnStruct *)call_data;
    for(; rp->string != NULL; rp++)
	    fprintf(stderr, "List %s %d %s\n", XtName(w), rp->index, rp->string);
}

char *Larray[] = {"First Item", "Second Item", "Third Item", NULL};
char *Larray1[] = {"Item #4", "Item #5", "Item #6", NULL};
char *Larray2[] = {"Item #7", "Item #8", "Item #9", NULL};
char *Larray3[] = {"Item #10", "Item #11", "Item #12", NULL};
char *Larray4[] = {"Item #13", "Item #14", "Item #15", "Item #16",
		   "Item #17", "Item #18", "Item #19", "Item #20",
		   "Item #21", "Item #22", "Item #23", "Item #24",
		   "Item #25", "Item #26", "Item #27", "Item #28",
		   "Item #29", "Item #30", NULL};

char *Carray[] = {"Question", "Information", "Working", "Error", "Notice", NULL};

void 
selectcall(w, client_data, call_data)
	Widget              w;
	XtPointer           client_data;
	XtPointer           call_data;
{
    XpwListAddItems(list, Larray, -1, 0, XpwListOutline, 1);
    XpwListAddItems(list, Larray1, -1, 0, XpwListUnderline, 2);
    XpwListAddItems(list, Larray2, -1, 0, XpwListCrossout, 3);
    XpwListAddItems(list, Larray3, -1, 0, XpwListShape, 4);
    XpwListAddItems(list, Larray4, -1, 0, 0, 0);
    fprintf(stderr, "Select %s %d\n", XtName(w), (int) call_data);
}

void 
scrollcall(w, client_data, call_data)
	Widget              w;
	XtPointer           client_data;
	XtPointer           call_data;
{
    fprintf(stderr, "Scroller %s %d\n", XtName(w), (int) call_data);
}

void
cmdcall(w, client_data, call_data)
	Widget              w;
	XtPointer           client_data;
	XtPointer           call_data;
{
        Widget  dialog;
        Arg	arg[1];
        int     result;
        XpwComboBoxReturnStruct	    rp;
   
	XtSetArg(arg[0], XtNmessage, "This is a long\nmessage test");
	XpwComboBoxGetSelected(combobox, &rp);
	switch(rp.index) {
	defualt:
	case 0:
        	dialog = XpwDialogCreateQuestion(toplevel, "dialog", arg, 1);
		break;
	case 1:
        	dialog = XpwDialogCreateInfo(toplevel, "dialog", arg, 1);
		break;
	case 2:
        	dialog = XpwDialogCreateWork(toplevel, "dialog", arg, 1);
		break;
	case 3:
        	dialog = XpwDialogCreateError(toplevel, "dialog", arg, 1);
		break;
	case 4:
        	dialog = XpwDialogCreateNotice(toplevel, "dialog", arg, 1);
		break;
	}
	XtCreateManagedWidget("TextLine", textLineWidgetClass, dialog, NULL, 0);
	XtCreateManagedWidget("test", commandWidgetClass, dialog, NULL, 0);
	XtManageChild(dialog);
        result = XpwDialogDisplay(dialog);
        XtDestroyWidget(dialog);
	fprintf(stderr, "Dialog returned %d\n", result);
}

void
slidercall(w, client_data, call_data)
	Widget		     w;
	XtPointer	     client_data;
	XtPointer	     call_data;
{
	int		     pos = (int)call_data;

        XpwPercentSetState(percent, (pos != 0));
        XpwPercentSetPosition(percent, pos);
}

int 
main(argc, argv)
	int                 argc;
	String             *argv;
{
    toplevel = XtVaAppInitialize
	(&app_context, "MenuBarT", NULL, 0, &argc, argv,
	 fallback_resources, NULL);

    clue = XtVaCreatePopupShell( "ClueWindow", clueWidgetClass, toplevel, NULL);


    form = XtCreateManagedWidget("form", panedWidgetClass, toplevel, NULL, 0);
    menubar = XtVaCreateManagedWidget
	("menubar", menubarWidgetClass, form, NULL);
    menu1 = XtVaCreateManagedWidget
	("menu1", barbuttonWidgetClass, menubar,
	 XtNmenuName, "menu1drop", NULL);
    menu2 = XtVaCreateManagedWidget
	("menu2", barbuttonWidgetClass, menubar,
	 XtNmenuName, "menu2drop", NULL);
    quit = XtVaCreateManagedWidget
	("quit", commandWidgetClass, menubar, NULL);

    XtAddCallback(quit, XtNcallback, exit, NULL);

   /* Create a menu shell for PullDown button 1 */
    drop1 = XtVaCreatePopupShell
	("menu1drop", popupMenuWidgetClass, menubar, NULL);
    button1 = XtVaCreateManagedWidget
	("button1", pmeEntryObjectClass, drop1, NULL);
    XtAddCallback(button1, XtNcallback, menucall, NULL);
    (void) XtVaCreateManagedWidget
	("line1", pmeLineObjectClass, drop1, NULL);
    button2 = XtVaCreateManagedWidget
	("button2", pmeEntryObjectClass, drop1, NULL);
    XtAddCallback(button2, XtNcallback, menucall, NULL);

   /* Create a menu shell for PullDown button 2 */
    drop2 = XtVaCreatePopupShell
	("menu2drop", popupMenuWidgetClass, menubar, NULL);
    button3 = XtVaCreateManagedWidget
	("button3", pmeEntryObjectClass, drop2,
	 XtNmenuName, "submenu1drop", NULL);
    XtAddCallback(button3, XtNcallback, menucall, NULL);
    (void) XtVaCreateManagedWidget
	("line2", pmeLineObjectClass, drop2, NULL);
    button4 = XtVaCreateManagedWidget
	("button4", pmeEntryObjectClass, drop2, NULL);
    XtAddCallback(button4, XtNcallback, menucall, NULL);

    subdrop = XtVaCreatePopupShell
	("submenu1drop", popupMenuWidgetClass, drop2, NULL);
    sbutton1 = XtVaCreateManagedWidget
	("sbutton1", pmeSelectObjectClass, subdrop,
	  XtNstate, FALSE, XtNrightButton, FALSE, XtNradioData, (XtPointer)&sbutton1,
	 NULL);
    XpwPmeSelectChangeRadioGroup(sbutton1, sbutton1);
    XtAddCallback(sbutton1, XtNcallback, menucall, NULL);
    sbutton2 = XtVaCreateManagedWidget
	("sbutton2", pmeSelectObjectClass, subdrop,
	  XtNstate, FALSE, XtNrightButton, FALSE, XtNradioGroup, sbutton1,  XtNradioData, (XtPointer)&sbutton1,
	 NULL);
    XpwPmeSelectChangeRadioGroup(sbutton2, sbutton1);
    XtAddCallback(sbutton2, XtNcallback, menucall, NULL);
    sbutton3 = XtVaCreateManagedWidget
	("sbutton3", pmeSelectObjectClass, subdrop,
	  XtNstate, FALSE, XtNrightButton, FALSE, XtNradioGroup, sbutton1, XtNradioData, (XtPointer)&sbutton1,
	 NULL);
    XpwPmeSelectChangeRadioGroup(sbutton3, sbutton1);
    XtAddCallback(sbutton3, XtNcallback, menucall, NULL);
    sbutton4 = XtVaCreateManagedWidget
	("sbutton4", pmeSelectObjectClass, subdrop, 
	  XtNstate, FALSE, XtNrightButton, FALSE, XtNradioGroup, sbutton1,  XtNradioData, (XtPointer)&sbutton1,
	 NULL);
    XpwPmeSelectChangeRadioGroup(sbutton4, sbutton1);
    XtAddCallback(sbutton4, XtNcallback, menucall, NULL);
    label = XtCreateManagedWidget("label", labelWidgetClass, form, NULL, 0);
    col2 = XtVaCreateManagedWidget("form", rowColWidgetClass, form,
		    XtNorientation, XtorientHorizontal, NULL);
    command = XtVaCreateManagedWidget("command", commandWidgetClass, col2, 
		XtNclue, "Hi There!", NULL, 0);
    XtAddCallback(command, XtNcallback, cmdcall, NULL);
    textline = XtVaCreateManagedWidget("TextLine", textLineWidgetClass,
			 form, NULL);
    combobox = XtVaCreateManagedWidget("ComboBox", comboBoxWidgetClass,
			form, XtNitemList, Carray, NULL );
    XtAddCallback(combobox, XtNcallback, combocall, NULL);
    menu1 = XtVaCreateManagedWidget("Menuarrow", menuarrowWidgetClass,
			 col2, XtNmenuName, "menu3drop", NULL);
   /* Create a menu shell for MenuButton  */
    drop3 = XtVaCreatePopupShell
	("menu3drop", popupMenuWidgetClass, menu1, NULL);
    button5 = XtVaCreateManagedWidget
	("button5", pmeEntryObjectClass, drop3, NULL);
    XtAddCallback(button5, XtNcallback, menucall, NULL);
    (void) XtVaCreateManagedWidget
	("line1", pmeLineObjectClass, drop3, NULL);
    button6 = XtVaCreateManagedWidget
	("button6", pmeEntryObjectClass, drop3, NULL);
    XtAddCallback(button6, XtNcallback, menucall, NULL);

    list = XtVaCreateManagedWidget("list", listWidgetClass, form, 
		XtNheight, 100, NULL, 0);
    XtAddCallback(list, XtNcallback, menucall, NULL);
    XtAddCallback(list, XtNmultiCallback, listcall, NULL);
    XtVaCreateManagedWidget("line", lineWidgetClass, form, NULL);
    slider = XtVaCreateManagedWidget("slider", sliderWidgetClass, form,
				XtNorientation, XtorientHorizontal, NULL);
    XtAddCallback(slider, XtNcallback, slidercall, NULL);
    percent = XtVaCreateManagedWidget("percent", percentWidgetClass, form,
				XtNorientation, XtorientHorizontal, NULL);
    form = XtCreateManagedWidget("frame", frameWidgetClass, form, NULL, 0);
    form = XtVaCreateManagedWidget("sform", rowColWidgetClass, form,
		XtNborderWidth, 0, NULL);
    select1 = XtVaCreateManagedWidget("select1", selectWidgetClass, form,
       XtNrightButton, False, XtNstate, True, XtNallowNone, False, NULL);
    XtAddCallback(select1, XtNcallback, selectcall, NULL);

    select2 = XtVaCreateManagedWidget("select2", selectWidgetClass, form,
				      XtNradioGroup, select1, XtNrightButton, False, XtNallowNone, False, NULL);
    XtAddCallback(select2, XtNcallback, selectcall, NULL);
    select3 = XtVaCreateManagedWidget("select3", selectWidgetClass, form,
				      XtNradioGroup, select1, XtNrightButton, False, XtNallowNone, False, NULL);
    XtAddCallback(select3, XtNcallback, selectcall, NULL);
    select4 = XtVaCreateManagedWidget("select4", selectWidgetClass, form,
				      XtNradioGroup, select1, XtNrightButton, False, XtNallowNone, False, NULL);
    XtAddCallback(select4, XtNcallback, selectcall, NULL);

    XtRealizeWidget(toplevel);
    XtAppMainLoop(app_context);
}
