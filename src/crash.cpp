#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Return_Button.H>

#include "main.h"

static Fl_Round_Button *restore=(Fl_Round_Button *)0;
static Fl_Round_Button *normal=(Fl_Round_Button *)0;
static Fl_Round_Button *blank=(Fl_Round_Button *)0;

static void cb_Start(Fl_Return_Button *b, void*) {
	b->parent()->hide();
}

crashchoice crashdialog() {
	Fl_Double_Window* w;
	{ Fl_Double_Window* o = new Fl_Double_Window(350, 240, _("Crash recovery"));
		w = o;
		{ new Fl_Box(11, 10, 327, 55, _("Looks like we crashed."));
		} // Fl_Box* o
		{ restore = new Fl_Round_Button(110, 85, 155, 25, _("Restore session"));
			restore->down_box(FL_ROUND_DOWN_BOX);
			restore->value(1);
			restore->type(FL_RADIO_BUTTON);
		} // Fl_Round_Button* restore
		{ normal = new Fl_Round_Button(110, 115, 155, 25, _("Normal start"));
			normal->down_box(FL_ROUND_DOWN_BOX);
			normal->type(FL_RADIO_BUTTON);
		} // Fl_Round_Button* normal
		{ blank = new Fl_Round_Button(110, 145, 155, 25, _("Blank page"));
			blank->down_box(FL_ROUND_DOWN_BOX);
			blank->type(FL_RADIO_BUTTON);
		} // Fl_Round_Button* blank
		{ Fl_Return_Button* o = new Fl_Return_Button(115, 200, 120, 25, _("Start"));
			o->callback((Fl_Callback*)cb_Start);
		} // Fl_Return_Button* o
		o->end();
	} // Fl_Double_Window* o

	w->show();

	while (w->shown())
		Fl::wait();

	if (restore->value())
		return CRASH_RESTORE;
	if (blank->value())
		return CRASH_BLANK;

	return CRASH_NORMAL;
}
