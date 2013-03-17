/*=============================================================================
  DNG4PS - software for converting Canon PS RAW files to DNG ones
  Copyright (C) 2007-2008 Denis Artyomov

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  =============================================================================
  File:      ProcessDialog.cpp
  Author(s): Denis Artyomov (denis.artyomov@gmail.com)
  ===========================================================================*/

#include "pch.h"
#include "ProcessDialog.h"
#include "lib/wxGUIBuilder.hpp"

#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/string.h>

extern wxThread* work_thread;

ProcessDialog::ProcessDialog(wxWindow* parent, wxFrame * frame, wxWindowID id)
{
	using namespace gb;

	Create(parent, id, _("procFormCaption"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	auto gui = existing_window(this)
	[
		vbox(expand | stretch)
		[
			text("procLogLabel"),
			edit(size(250,150) | expand | stretch, ed_multiline | ed_readonly) >> txtLog,
			hbox(border(0) | expand)
			[
				text("procExecPercent1") >> stxtPercent,
				spring(),
				text("") >> stxtTime
			],
			gauge(100, expand) >> gaPercent
		],
		hbox(border(0) | expand)
		[
			spring(),
			button("Cancel") >> btnStop,
			button("btnExit") >> btnExit
		]
	];

	gui.build_gui();

	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
	Center();

	btnStop->Bind(wxEVT_COMMAND_BUTTON_CLICKED, [this] (wxCommandEvent& event) { btnStopClick(event); });
	btnExit->Bind(wxEVT_COMMAND_BUTTON_CLICKED, [this] (wxCommandEvent& event) { btnExitClick(event); });
	this   ->Bind(wxEVT_CLOSE_WINDOW,           [this] (wxCloseEvent&   event) { OnClose(event);      });

	this->parent = frame;
}


// ProcessDialog::add_text
void ProcessDialog::add_text(const wxString & text)
{
	txtLog->AppendText(text);
	txtLog->Refresh();
}

// ProcessDialog::set_state
void ProcessDialog::set_state(PD_States state)
{
	switch (state)
	{
	case PD_Work:
		btnStop->Enable();
		btnExit->Disable();
		break;

	case PD_Done:
		btnStop->Disable();
		btnExit->Enable();
		break;
	}
}

// ProcessDialog::btnExitClick
void ProcessDialog::btnExitClick(wxCommandEvent& event)
{
	parent->Enable();
	Close();
}

// ProcessDialog::btnStopClick
void ProcessDialog::btnStopClick(wxCommandEvent& event)
{
	btnStop->Disable();
	if (work_thread) work_thread->Delete();
}

// ProcessDialog::clear
void ProcessDialog::clear()
{
	txtLog->Clear();
	stxtPercent->SetLabel(_("procExecPercent"));
	stxtTime->SetLabel(L"");
	gaPercent->SetValue(0);
}

// ProcessDialog::set_percent
void ProcessDialog::set_percent(int percent)
{
	wxString buffer;
	buffer.Printf(L"%s %i%%", _("procExecPercent"), percent);
	stxtPercent->SetLabel(buffer);
	gaPercent->SetValue(percent);
}

// ProcessDialog::show_time
void ProcessDialog::show_time(int current, int total)
{
	wxString buffer;

	buffer << _("procPass1") << L" " << wxTimeSpan::Seconds(current).Format(L"%H:%M:%S") << L" "<< _("procPass2") << L" " << wxTimeSpan::Seconds(total).Format(L"%H:%M:%S");
	stxtTime->SetLabel(buffer);
	sizerMain->RecalcSizes();
}

// ProcessDialog::OnClose
void ProcessDialog::OnClose(wxCloseEvent& event)
{
	if ( !btnExit->IsEnabled() ) event.Veto();
	else 
	{
		parent->Enable();
		event.Skip();
	}
}
