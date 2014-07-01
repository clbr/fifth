//
// Fl_Input_Choice -- An input/chooser widget
//            ______________  ____
//           |              || __ |
//           | input area   || \/ |
//           |______________||____|
//
// Copyright 2004 by Greg Ercolano.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//

#ifndef _FL_INPUT_CHOICE_H
#define _FL_INPUT_CHOICE_H

#include <FL/Fl.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/fl_draw.H>

class Fl_Input_Choice : public Fl_Group {
protected:
    // Private class to handle slightly 'special' behavior of menu button
    class InputMenuButton : public Fl_Menu_Button {
	void draw() {
	    draw_box(FL_UP_BOX, color());
	    fl_color(active_r() ? labelcolor() : fl_inactive(labelcolor()));
	    int xc = x()+w()/2, yc=y()+h()/2;
	    fl_polygon(xc-4,yc-2,xc+4,yc-2,xc,yc+2);
	    if (Fl::focus() == this) draw_focus();
	}
    public:
	InputMenuButton(int x,int y,int w,int h,const char*l=0) : 
	    Fl_Menu_Button(x,y,w,h,l) { box(FL_UP_BOX); }
    };

    Fl_Input *_inp;
    InputMenuButton *_menu;

    static void _menu_cb(Fl_Widget*, void *data) { 
        Fl_Input_Choice *o=(Fl_Input_Choice *)data;
	o->_menu_cb2(); 
    }
    void _menu_cb2() {
        _inp->value(_menu->text());
    }

    // Custom resize behavior -- input stretches, menu button doesn't
    inline int _InpSizeX() { return(x() + Fl::box_dx(box())); }
    inline int _InpSizeY() { return(y() + Fl::box_dy(box())); }
    inline int _InpSizeW() { return(w() - Fl::box_dw(box()) - buttonw); }
    inline int _InpSizeH() { return(h() - Fl::box_dh(box())); }

    inline int _MenuSizeX() { return(x() + w() - buttonw - Fl::box_dx(box())); }
    inline int _MenuSizeY() { return(y() + Fl::box_dy(box())); }
    inline int _MenuSizeW() { return(buttonw); }
    inline int _MenuSizeH() { return(h() - Fl::box_dh(box())); }

    static const int buttonw = 17;

public:
    void resize(int X, int Y, int W, int H) {
	Fl_Group::resize(X,Y,W,H);
	_inp->resize(_InpSizeX(), _InpSizeY(), _InpSizeW(), _InpSizeH());
	_menu->resize(_MenuSizeX(), _MenuSizeY(), _MenuSizeW(), _MenuSizeH());
    }

    Fl_Input_Choice (int x,int y,int w,int h,const char*l=0) : Fl_Group(x,y,w,h,l) {
	Fl_Group::box(FL_DOWN_BOX);
	align(FL_ALIGN_LEFT);				// default like Fl_Input
        _inp = new Fl_Input(_InpSizeX(), _InpSizeY(),
			    _InpSizeW(), _InpSizeH());
        _inp->box(FL_FLAT_BOX); // cosmetic
	_menu = new InputMenuButton(_MenuSizeX(), _MenuSizeY(),
				    _MenuSizeW(), _MenuSizeH());
	_menu->callback(_menu_cb, (void*)this);
	_menu->box(FL_FLAT_BOX);				// cosmetic
	end();
    }
    void menuvalue(int val) {
        _menu->value(val);
	_inp->value(_menu->text(val));
    }
    void inputvalue(const char *val) {
	_inp->value(val);
    }
    const char* inputvalue() const {
	return(_inp->value());
    }
    const char* value() const {
	return(inputvalue());
    }
    void value(int val) {
	menuvalue(val);
    }
    Fl_Menu_Button& menubutton() { return(*_menu); }
    Fl_Input& input() { return(*_inp); }
};

#endif /*_FL_INPUT_CHOICE*/
