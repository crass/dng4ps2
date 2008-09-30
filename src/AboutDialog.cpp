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
  File:      AboutDialog.cpp
  Author(s): Denis Artyomov (denis.artyomov@gmail.com)
  ===========================================================================*/

#include "pch.h"
#include "AboutDialog.h"

#include "./version.h"

//(*InternalHeaders(AboutDialog)
#include <wx/settings.h>
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/string.h>
//*)

//(*IdInit(AboutDialog)
const long AboutDialog::ID_STATICBITMAP1 = wxNewId();
const long AboutDialog::ID_TEXTCTRL1 = wxNewId();
const long AboutDialog::ID_STATICTEXT1 = wxNewId();
const long AboutDialog::ID_TEXTCTRL2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(AboutDialog,wxDialog)
	//(*EventTable(AboutDialog)
	//*)
END_EVENT_TABLE()

extern wxIcon main_icon;

AboutDialog::AboutDialog(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(AboutDialog)
	wxFlexGridSizer* FlexGridSizer3;
	wxStaticBitmap* bmpIcon;
	wxStaticText* StaticText1;
	wxFlexGridSizer* FlexGridSizer1;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;
	
	Create(parent, id, _("aboutFormCaption"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	FlexGridSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
	bmpIcon = new wxStaticBitmap(this, ID_STATICBITMAP1, wxNullBitmap, wxDefaultPosition, wxSize(32,32), 0, _T("ID_STATICBITMAP1"));
	FlexGridSizer1->Add(bmpIcon, 1, wxALL|wxALIGN_LEFT|wxALIGN_TOP, wxDLG_UNIT(this,wxSize(15,0)).GetWidth());
	FlexGridSizer2 = new wxFlexGridSizer(0, 1, 0, 0);
	txtAboutText = new wxTextCtrl(this, ID_TEXTCTRL1, _("aboutMain"), wxDefaultPosition, wxDLG_UNIT(this,wxSize(197,114)), wxTE_NO_VSCROLL|wxTE_AUTO_SCROLL|wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH2|wxTE_AUTO_URL|wxNO_BORDER|wxFULL_REPAINT_ON_RESIZE, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	wxFont txtAboutTextFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	if ( !txtAboutTextFont.Ok() ) txtAboutTextFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	txtAboutTextFont.SetPointSize((int)(txtAboutTextFont.GetPointSize() * 1.200000));
	txtAboutText->SetFont(txtAboutTextFont);
	FlexGridSizer2->Add(txtAboutText, 1, wxTOP|wxBOTTOM|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer1->Add(FlexGridSizer2, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer1->Add(-1,-1,1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer3 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer3->AddGrowableCol(0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("lblYouLikeProgram"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	StaticText1->SetForegroundColour(wxColour(0,0,255));
	wxFont StaticText1Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	if ( !StaticText1Font.Ok() ) StaticText1Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	StaticText1Font.SetPointSize((int)(StaticText1Font.GetPointSize() * 1.000000));
	StaticText1Font.SetWeight(wxBOLD);
	StaticText1->SetFont(StaticText1Font);
	FlexGridSizer3->Add(StaticText1, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	TextCtrl1 = new wxTextCtrl(this, ID_TEXTCTRL2, _("* E-gold: 5492482\n* Yandex.Money: 4100179854863\n* WebMoney: R157974029861"), wxDefaultPosition, wxDLG_UNIT(this,wxSize(201,31)), wxTE_NO_VSCROLL|wxTE_MULTILINE|wxTE_READONLY|wxNO_BORDER|wxFULL_REPAINT_ON_RESIZE, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	TextCtrl1->SetForegroundColour(wxColour(0,0,255));
	TextCtrl1->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	wxFont TextCtrl1Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	if ( !TextCtrl1Font.Ok() ) TextCtrl1Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	TextCtrl1Font.SetPointSize((int)(TextCtrl1Font.GetPointSize() * 1.000000));
	TextCtrl1Font.SetWeight(wxBOLD);
	TextCtrl1->SetFont(TextCtrl1Font);
	FlexGridSizer3->Add(TextCtrl1, 1, wxTOP|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer1->Add(FlexGridSizer3, 1, wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer1->Add(-1,-1,1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, _("btnExit")));
	StdDialogButtonSizer1->Realize();
	FlexGridSizer1->Add(StdDialogButtonSizer1, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);
	Center();
	
	Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_URL,(wxObjectEventFunction)&AboutDialog::txtAboutTextTextUrl);
	//*)

	bmpIcon->SetIcon(main_icon);
	wxString text = txtAboutText->GetValue();

	wxString vers;

	vers.Printf
	(
		L"%d.%d.%d %s build %d ",
		AutoVersion::MAJOR,
		AutoVersion::MINOR,
		AutoVersion::BUILD,
		wxString(AutoVersion::STATUS, wxConvLocal).c_str(),
		AutoVersion::BUILDS_COUNT
	);

	text.Replace(L"__VERSION__", vers, true);
	txtAboutText->Clear();
	txtAboutText->WriteText(text);

	txtAboutText->WriteText(wxString::Format(L"\n\n(c) 2007-%s Denis Artyomov (denis.artyomov@gmail.com)", wxString(AutoVersion::YEAR, wxConvLocal).c_str()));
}

AboutDialog::~AboutDialog()
{
	//(*Destroy(AboutDialog)
	//*)
}


void AboutDialog::txtAboutTextTextUrl(wxTextUrlEvent& event)
{
	if ( !event.GetMouseEvent().LeftDown() ) return;
	wxString url = txtAboutText->GetRange(event.GetURLStart(), event.GetURLEnd());
#if defined(__WIN32__)
	ShellExecuteW(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOWDEFAULT);
#endif
}
