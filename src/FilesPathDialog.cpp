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

//(*InternalHeaders(FilesPathDialog)
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(FilesPathDialog)
const long FilesPathDialog::ID_STATICTEXT1 = wxNewId();
const long FilesPathDialog::ID_GENERICDIRCTRL1 = wxNewId();
const long FilesPathDialog::ID_BUTTON1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(FilesPathDialog,wxDialog)
	//(*EventTable(FilesPathDialog)
	//*)
END_EVENT_TABLE()

extern wxString path_sep;

FilesPathDialog::FilesPathDialog(const wxString& path, const wxString& text, wxWindow* parent,wxWindowID id)
{
	//(*Initialize(FilesPathDialog)
	wxButton* btnNewFolder;
	wxFlexGridSizer* FlexGridSizer1;
	wxStaticText* lblMain;

	Create(parent, id, _("SelectDir"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	lblMain = new wxStaticText(this, ID_STATICTEXT1, _("Path"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(lblMain, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	dirPath = new wxGenericDirCtrl(this, ID_GENERICDIRCTRL1, wxEmptyString, wxDefaultPosition, wxDLG_UNIT(this,wxSize(204,174)), wxDIRCTRL_DIR_ONLY|wxNO_BORDER, wxEmptyString, 0, _T("ID_GENERICDIRCTRL1"));
	FlexGridSizer1->Add(dirPath, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer2 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer2->AddGrowableCol(1);
	btnNewFolder = new wxButton(this, ID_BUTTON1, _("btnNewFolder"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	btnNewFolder->SetToolTip(_("btnNewFolderHint"));
	FlexGridSizer2->Add(btnNewFolder, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, _("btnOk")));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, _("btnCancel")));
	StdDialogButtonSizer1->Realize();
	FlexGridSizer2->Add(StdDialogButtonSizer1, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer1->Add(FlexGridSizer2, 1, wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);
	Center();

	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&FilesPathDialog::btnNewFolderClick);
	//*)

	dirPath->SetPath(path);
	dirPath->SetFocus();
	dirPath->SetFocusFromKbd();

	lblMain->SetLabel(text);
}

FilesPathDialog::~FilesPathDialog()
{
	//(*Destroy(FilesPathDialog)
	//*)
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
