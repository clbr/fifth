//
// Inputplace_Choice -- An input/chooser widget
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

#ifndef _INPUTPLACE_CHOICE_H
#define _INPUTPLACE_CHOICE_H

#include <FL/Fl.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/fl_draw.H>
#include "inputplace.h"

class Inputplace_Choice : public Fl_Group {
protected:
    // Private class to handle slightly 'special' behavior of menu button
    class InputMenuButton : public Fl_Menu_Button {
    public:
	void draw() override {
	    draw_box(FL_UP_BOX, color());
	    fl_color(active_r() ? labelcolor() : fl_inactive(labelcolor()));
	    int xc = x()+w()/2, yc=y()+h()/2;
	    fl_polygon(xc-4,yc-2,xc+4,yc-2,xc,yc+2);
	    if (Fl::focus() == this) draw_focus();
	}
	const Fl_Menu_Item* popup() {
            if (!menu() || !menu()->text) return 0;
	    const Fl_Menu_Item* m;
	    Fl_Widget_Tracker mb(this);
	    m = menu()->pulldown(pullx, pully, pullw, h(), 0, this);
	    picked(m);
	    if (mb.exists()) redraw();
	    return m;
	}
	int pullx, pully, pullw;
	InputMenuButton(int x,int y,int w,int h,const char*l=0) :
	    Fl_Menu_Button(x,y,w,h,l) { box(FL_UP_BOX); }

        int handle(const int e) override {
            if (!menu() || !menu()->text) return 0;
            if (e == FL_PUSH) {
                popup();
                return 1;
            } else if (e == FL_KEYBOARD) {
                if (Fl::event_key() == ' ' &&
                    !(Fl::event_state() & (FL_SHIFT | FL_CTRL | FL_ALT | FL_META))) {
                    popup();
                    return 1;
                }
            }

            return Fl_Menu_Button::handle(e);
        }
    };

    inputplace *_inp;
    InputMenuButton *_menu;

    static void _menu_cb(Fl_Widget*, void *data) { 
        Inputplace_Choice *o=(Inputplace_Choice *)data;
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

        _menu->pullx = X;
        _menu->pully = Y;
        _menu->pullw = W;
    }

    Inputplace_Choice (int x,int y,int w,int h,const char*l=0) : Fl_Group(x,y,w,h,l) {
	Fl_Group::box(FL_DOWN_BOX);
	align(FL_ALIGN_LEFT);				// default like Fl_Input
        _inp = new inputplace(_InpSizeX(), _InpSizeY(),
			    _InpSizeW(), _InpSizeH());
        _inp->box(FL_FLAT_BOX); // cosmetic
	_menu = new InputMenuButton(_MenuSizeX(), _MenuSizeY(),
				    _MenuSizeW(), _MenuSizeH());
	_menu->callback(_menu_cb, (void*)this);
	_menu->box(FL_FLAT_BOX);				// cosmetic
	end();

        _menu->pullx = x;
        _menu->pully = y;
        _menu->pullw = w;
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
    InputMenuButton& inpmenubutton() { return(*_menu); }
    inputplace& input() { return(*_inp); }
};

#endif /*_INPUTPLACE_CHOICE*/
