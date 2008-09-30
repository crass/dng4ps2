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

//(*InternalHeaders(ProcessDialog)
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(ProcessDialog)
const long ProcessDialog::ID_STATICTEXT1 = wxNewId();
const long ProcessDialog::ID_TEXTCTRL1 = wxNewId();
const long ProcessDialog::ID_STATICTEXT2 = wxNewId();
const long ProcessDialog::ID_STATICTEXT3 = wxNewId();
const long ProcessDialog::ID_GAUGE1 = wxNewId();
const long ProcessDialog::ID_BUTTON1 = wxNewId();
const long ProcessDialog::ID_BUTTON2 = wxNewId();
//*)

extern wxThread* work_thread;

BEGIN_EVENT_TABLE(ProcessDialog,wxDialog)
	//(*EventTable(ProcessDialog)
	//*)
END_EVENT_TABLE()

ProcessDialog::ProcessDialog(wxWindow* parent, wxFrame * frame, wxWindowID id)
{
	//(*Initialize(ProcessDialog)
	wxFlexGridSizer* sizerInfo;
	wxFlexGridSizer* FlexGridSizer2;
	wxStaticText* StaticText1;

	Create(parent, id, _("procFormCaption"), wxDefaultPosition, wxDefaultSize, wxCAPTION, _T("id"));
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	sizerMain = new wxFlexGridSizer(0, 1, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("procLogLabel"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	sizerMain->Add(StaticText1, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	txtLog = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDLG_UNIT(this,wxSize(245,151)), wxTE_MULTILINE|wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	sizerMain->Add(txtLog, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	sizerInfo = new wxFlexGridSizer(0, 2, 0, 0);
	sizerInfo->AddGrowableCol(1);
	stxtPercent = new wxStaticText(this, ID_STATICTEXT2, _("procExecPercent1"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	sizerInfo->Add(stxtPercent, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	stxtTime = new wxStaticText(this, ID_STATICTEXT3, _("Label"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	stxtTime->SetToolTip(_("procPass_Hint"));
	sizerInfo->Add(stxtTime, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	sizerMain->Add(sizerInfo, 1, wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	gaPercent = new wxGauge(this, ID_GAUGE1, 100, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_GAUGE1"));
	sizerMain->Add(gaPercent, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer2 = new wxFlexGridSizer(0, 3, 0, wxDLG_UNIT(this,wxSize(-5,0)).GetWidth());
	btnStop = new wxButton(this, ID_BUTTON1, _("btnCancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	FlexGridSizer2->Add(btnStop, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	btnExit = new wxButton(this, ID_BUTTON2, _("btnExit"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	FlexGridSizer2->Add(btnExit, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	sizerMain->Add(FlexGridSizer2, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(0,0)).GetWidth());
	SetSizer(sizerMain);
	sizerMain->Fit(this);
	sizerMain->SetSizeHints(this);
	Center();

	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ProcessDialog::btnStopClick);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ProcessDialog::btnExitClick);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&ProcessDialog::OnClose);
	//*)

	this->parent = frame;
}

ProcessDialog::~ProcessDialog()
{
	//(*Destroy(ProcessDialog)
	//*)
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
	else event.Skip();
}
