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

#include <wx/settings.h>
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/string.h>

#include "lib/wxGUIBuilder.hpp"

extern wxIcon main_icon;

void fill_about_text_into(wxTextCtrl* txtAboutText)
{
	wxString text = _("aboutMain");
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

AboutDialog::AboutDialog(wxWindow* parent,wxWindowID id)
{
	using namespace gb;
	wxStaticBitmap* bmpIcon = nullptr;
	wxTextCtrl* txtAboutText = nullptr;

	Create(parent, id, _("aboutFormCaption"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));

	auto gui = existing_window(this) 
	[ 
		vbox(border(5) | bord_all) 
		[
			hbox()
			[
				image(align_top | border(7) | bord_all) >> bmpIcon,
				edit(size(150, 120) | noborder, ed_multiline | ed_autourl | ed_rich | ed_readonly) >> txtAboutText
			],
			button_ok("", align_right)
		]
	];

	gui.build();

	bmpIcon->SetIcon(main_icon);

	fill_about_text_into(txtAboutText);

	txtAboutText->Bind(
		wxEVT_COMMAND_TEXT_URL, 
		[this] (wxTextUrlEvent& event) { txtAboutTextTextUrl(event); }
	);

	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
	Center();
}


void AboutDialog::txtAboutTextTextUrl(wxTextUrlEvent& event)
{
	if ( !event.GetMouseEvent().LeftDown() ) return;
	wxTextCtrl* txtAboutText = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
	assert(txtAboutText);
	wxString url = txtAboutText->GetRange(event.GetURLStart(), event.GetURLEnd());
#if defined(__WIN32__)
	ShellExecuteW(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOWDEFAULT);
#endif
}
