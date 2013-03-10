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
  File:      FilesPathDialog.cpp
  Author(s): Denis Artyomov (denis.artyomov@gmail.com)
  ===========================================================================*/

#include "pch.h"
#include "FilesPathDialog.h"
#include "gui/NewFolderDialog.h"
#include <wx/filename.h>

#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/string.h>

#include "lib/wxGUIBuilder.hpp"


extern wxString path_sep;

FilesPathDialog::FilesPathDialog(const wxString& path, const wxString& descr_text, wxWindow* parent,wxWindowID id)
{
	Create(parent, id, _("SelectDir"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));

	wxButton* btnNewFolder = nullptr;
	wxStaticText* lblMain = nullptr;
	dirPath = nullptr;

	auto dialog_gui = dialog(this)
	[ 
		vbox (bord_all & border(5)) 
		[
			text ("Path") >> lblMain,
			dir_ctrl (size(200, 200) & expand & stretch) >> dirPath,
			hbox () 
			[
				button ("btnNewFolder") >> btnNewFolder,
				spring (),
				dlg_buttons_ok_cancel ()
			]
		]
	];

	dialog_gui.build_gui();

	dirPath->SetPath(path);
	dirPath->SetFocus();
	dirPath->SetFocusFromKbd();
	lblMain->SetLabel(descr_text);
	Center();

	btnNewFolder->Bind(wxEVT_COMMAND_BUTTON_CLICKED, [this] (wxCommandEvent& event) { btnNewFolderClick(event); }  );
}


// FilesPathDialog::execute
bool FilesPathDialog::execute()
{
	ShowModal();
	if (GetReturnCode() != wxID_OK) return false;
	return true;
}

// FilesPathDialog::get_path
wxString FilesPathDialog::get_path()
{
	return dirPath->GetPath();
}

void FilesPathDialog::btnNewFolderClick(wxCommandEvent& event)
{
	NewFolderDialog * dlg = new NewFolderDialog(this);
	bool ok = dlg->execute(dirPath->GetPath());
	if (ok)
	{
		wxLogNull logNo;

		wxString new_dir = dirPath->GetPath()+path_sep+dlg->get_path();
		bool ok = wxFileName::Mkdir(new_dir);
		if (!ok) wxMessageBox(_("cant_create_dir"), _("error"));
		else
		{
			dirPath->Hide();
			dirPath->ReCreateTree();
			dirPath->SetPath(new_dir);
			dirPath->Show();
		}
	}
	dlg->Destroy();
}
