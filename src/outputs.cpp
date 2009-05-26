/***************************************************************************
 *   Copyright (C) 2008-2009 by Andrzej Rybczak                            *
 *   electricityispower@gmail.com                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.              *
 ***************************************************************************/

#include "outputs.h"

#ifdef ENABLE_OUTPUTS

#include "display.h"
#include "global.h"

using namespace Global;

Outputs *myOutputs = new Outputs;

void Outputs::Init()
{
	w = new Menu<MPD::Output>(0, MainStartY, COLS, MainHeight, "", Config.main_color, brNone);
	w->SetTimeout(ncmpcpp_window_timeout);
	w->CyclicScrolling(Config.use_cyclic_scrolling);
	w->HighlightColor(Config.main_highlight_color);
	w->SetItemDisplayer(Display::Pairs);
	
	FetchList();
}

void Outputs::SwitchTo()
{
	if (myScreen == this)
		return;
	
	if (hasToBeResized)
		Resize();
	
	myScreen = this;
	w->Window::Clear();
	
	RedrawHeader = 1;
}

void Outputs::Resize()
{
	w->Resize(COLS, MainHeight);
	hasToBeResized = 0;
}

std::string Outputs::Title()
{
	return "Outputs";
}

void Outputs::EnterPressed()
{
	if (w->Current().second)
	{
		if (Mpd->DisableOutput(w->Choice()))
			ShowMessage("Output \"%s\" disabled", w->Current().first.c_str());
		w->Current().second = 0;
		w->BoldOption(w->Choice(), 0);
	}
	else
	{
		if (Mpd->EnableOutput(w->Choice()))
			ShowMessage("Output \"%s\" enabled", w->Current().first.c_str());
		w->Current().second = 1;
		w->BoldOption(w->Choice(), 1);
	}
	
}

void Outputs::MouseButtonPressed(MEVENT me)
{
	if (w->Empty() || !w->hasCoords(me.x, me.y) || size_t(me.y) >= w->Size())
		return;
	if (me.bstate & BUTTON1_PRESSED || me.bstate & BUTTON3_PRESSED)
	{
		w->Goto(me.y);
		if (me.bstate & BUTTON3_PRESSED)
			EnterPressed();
	}
	else
		Screen< Menu<MPD::Output> >::MouseButtonPressed(me);
}

void Outputs::FetchList()
{
	MPD::OutputList ol;
	Mpd->GetOutputs(ol);
	w->Clear();
	for (MPD::OutputList::const_iterator it = ol.begin(); it != ol.end(); it++)
		w->AddOption(*it, it->second);
}

#endif // ENABLE_OUTPUTS
