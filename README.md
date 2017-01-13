 In developing a email system called XMS I found a few large holes in the Athena widget set. 
 This forced me to write some of my own widgets. I have decided to complete this into a full
 library (since only a few functions are missing. The library will have build in support for
 3D drawing with GC caching and XPM reader/converter and support for fancy backrounds.

Design goals:  

   * Provide a wide range of widgets in a small space.
   * Allow for easy conversion from Athena Widgets to Xpw. Note: Xpw will not be a drop in replacement for Xaw.
   * Built in support for 3D look and feel.
   * Built in XPM support.
   * Built in clue support. 

# Why a new widget set?

Becuase all the free sets I could find don't seem to have the functionality that is required to write high
quality X windows applications. The ones that do look promising either do not use the Toolkit (this may be
bad or good depending on your point of view), too restrictive, or cost money.

Why not drop in replacement for Xaw? First off there already are many of these. Second the limitations I
found in Xaw are still in these. For example in Xms (which is being developed in parrelel with Xpw, I am
finding that the application is shrinking as new widgets are completed. The Athena widgets require that 
you either call their convience routines or redo their translation tables. Two examples, with Athena, to
do cascaded menus I had to overide translations and install my own event handles, with Xpw I just install
a popup menu as a child of the menu with to be cascaded, the popmenu widget does all the right actions, 
including delayed popup of the cascade if the mouse stays on the entry. Second example is with the Toggle
widget, (or Select under Xpw), to support only a single selection in a group it took about 100 lines of code
to impliment this. Under Xpw, it took zero lines, and you only get called by the new selection not the old
and the new then the old agian as under Athena.

# Licensing

Xpw will be release free for noncommercial use. Please contact me if you want to use Xpw in your commerical
or shareware application, we can work out either a royaly or flat fee license depending on projected market
volume. My hope is that in releasing the source for free it will allow for people to use the widget set and 
thereby make it more widely available. I feel this is one of the major hinderances to Motif and Xforms today.

# Project status

All widgets marked as completed have been checked into RCS. Xcdplay released, also released a new binary of
xmixer linked with current libraries. Currently working on finishing off SimpleText and man pages. Next app 
to be release will be xnotepad, a simple text editor and clipboard (test bed for SimpleText).

  *  SimpleText: needs to be finished off.
  *  AnsiText, Board, FileSelect, Led, and Text: need to be written.
  *  XPM reader need to be written (widgets support color pixmaps properly now).
  *  Man pages for all widgets. 
