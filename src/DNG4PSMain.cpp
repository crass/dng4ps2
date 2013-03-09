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
  File:      DNG4PSMain.cpp
  Author(s): Denis Artyomov (denis.artyomov@gmail.com)
             Matt Dawson (coder@dawsons.id.au)
  ===========================================================================*/

#include "pch.h"
#include <wx/msgdlg.h>
#include <wx/app.h>
#include <wx/thread.h>

//(*InternalHeaders(DNG4PSFrame)
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include "DNG4PSMain.h"
#include "FileList.h"
#include "ProcessDialog.h"
#include "gui/FilesPathDialog.h"
#include "AboutDialog.h"
#include "OptionsDialog.h"
#include "Options.h"
#include "Utils.h"
#include "ProgramObjects.h"
#include "ScanProgressWin.h"
#include "Exception.hpp"

//(*IdInit(DNG4PSFrame)
const long DNG4PSFrame::ID_STATICTEXT1 = wxNewId();
const long DNG4PSFrame::ID_TEXTCTRL1 = wxNewId();
const long DNG4PSFrame::ID_BUTTON1 = wxNewId();
const long DNG4PSFrame::ID_STATICTEXT3 = wxNewId();
const long DNG4PSFrame::ID_LISTCTRL1 = wxNewId();
const long DNG4PSFrame::ID_BUTTON6 = wxNewId();
const long DNG4PSFrame::ID_STATICTEXT2 = wxNewId();
const long DNG4PSFrame::ID_TEXTCTRL2 = wxNewId();
const long DNG4PSFrame::ID_BUTTON4 = wxNewId();
const long DNG4PSFrame::ID_STATICTEXT4 = wxNewId();
const long DNG4PSFrame::ID_BUTTON2 = wxNewId();
const long DNG4PSFrame::ID_BUTTON5 = wxNewId();
const long DNG4PSFrame::ID_BUTTON3 = wxNewId();
const long DNG4PSFrame::ID_TIMER1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DNG4PSFrame,wxFrame)
	//(*EventTable(DNG4PSFrame)
	//*)
END_EVENT_TABLE()

extern wxIcon main_icon;
ProcessDialog * process_dialog = NULL;

DNG4PSFrame::DNG4PSFrame(wxWindow* parent,wxWindowID id) : file_list(new FileList)
{
	//(*Initialize(DNG4PSFrame)
	wxFlexGridSizer* FlexGridSizer4;
	wxButton* btnSelectOutputDir;
	wxButton* btnAbout;
	wxStaticText* StaticText2;
	wxButton* btnStart;
	wxFlexGridSizer* FlexGridSizer3;
	wxFlexGridSizer* FlexGridSizer5;
	wxButton* btnOptions;
	wxStaticText* StaticText1;
	wxStaticText* StaticText3;
	wxFlexGridSizer* szMain;
	wxButton* btnSelectPathToRaw;
	wxFlexGridSizer* szSource;
	wxFlexGridSizer* FlexGridSizer1;
	wxStaticText* StaticText4;
	
	Create(parent, id, _("MainFormCaption"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL, _T("id"));
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	szMain = new wxFlexGridSizer(0, 1, 0, 0);
	szMain->AddGrowableCol(0);
	szMain->AddGrowableRow(1);
	szSource = new wxFlexGridSizer(0, 2, wxDLG_UNIT(this,wxSize(-5,0)).GetWidth(), 0);
	szSource->AddGrowableCol(0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("PathToRAWFiles"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	szSource->Add(StaticText1, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	wxSize __SpacerSize_1 = wxDLG_UNIT(this,wxSize(5,5));
	szSource->Add(__SpacerSize_1.GetWidth(),__SpacerSize_1.GetHeight(),1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(0,0)).GetWidth());
	txtPathToRaw = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDLG_UNIT(this,wxSize(180,10)), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	szSource->Add(txtPathToRaw, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	btnSelectPathToRaw = new wxButton(this, ID_BUTTON1, _("..."), wxDefaultPosition, wxDLG_UNIT(this,wxSize(16,13)), 0, wxDefaultValidator, _T("ID_BUTTON1"));
	btnSelectPathToRaw->SetToolTip(_("PathToRAW_Hint"));
	szSource->Add(btnSelectPathToRaw, 1, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	szMain->Add(szSource, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer3 = new wxFlexGridSizer(0, 1, wxDLG_UNIT(this,wxSize(-5,0)).GetWidth(), 0);
	FlexGridSizer3->AddGrowableCol(0);
	FlexGridSizer3->AddGrowableRow(1);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("RawFilesList"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer3->Add(StaticText3, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	lstFileList = new wxListCtrl(this, ID_LISTCTRL1, wxDefaultPosition, wxDLG_UNIT(this,wxSize(256,120)), wxLC_REPORT|wxLC_HRULES|wxLC_VRULES|wxLC_NO_SORT_HEADER, wxDefaultValidator, _T("ID_LISTCTRL1"));
	lstFileList->SetToolTip(_("RawFilesList_Hint"));
	FlexGridSizer3->Add(lstFileList, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	btnRescan = new wxButton(this, ID_BUTTON6, _("ButtonRescan"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON6"));
	btnRescan->SetToolTip(_("ButtonRescanHint"));
	FlexGridSizer3->Add(btnRescan, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	szMain->Add(FlexGridSizer3, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer1 = new wxFlexGridSizer(0, 2, wxDLG_UNIT(this,wxSize(-5,0)).GetWidth(), 0);
	FlexGridSizer1->AddGrowableCol(0);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("OutputDir"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer1->Add(StaticText2, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer1->Add(-1,-1,1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	txtOutputDir = new wxTextCtrl(this, ID_TEXTCTRL2, _("Text"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	FlexGridSizer1->Add(txtOutputDir, 1, wxTOP|wxBOTTOM|wxLEFT|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	btnSelectOutputDir = new wxButton(this, ID_BUTTON4, _("..."), wxDefaultPosition, wxDLG_UNIT(this,wxSize(16,13)), 0, wxDefaultValidator, _T("ID_BUTTON4"));
	btnSelectOutputDir->SetToolTip(_("OutputDirButton_Hint"));
	FlexGridSizer1->Add(btnSelectOutputDir, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	szMain->Add(FlexGridSizer1, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer4 = new wxFlexGridSizer(0, 1, wxDLG_UNIT(this,wxSize(-5,0)).GetWidth(), 0);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("ActionsLabel"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	FlexGridSizer4->Add(StaticText4, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer5 = new wxFlexGridSizer(0, 4, 0, 0);
	btnStart = new wxButton(this, ID_BUTTON2, _("ButtonGo"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	btnStart->SetDefault();
	btnStart->SetToolTip(_("ButtonGo_Hint"));
	FlexGridSizer5->Add(btnStart, 1, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	btnOptions = new wxButton(this, ID_BUTTON5, _("ButtonOptions"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON5"));
	FlexGridSizer5->Add(btnOptions, 1, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	btnAbout = new wxButton(this, ID_BUTTON3, _("ButtonAbout"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
	FlexGridSizer5->Add(btnAbout, 1, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer4->Add(FlexGridSizer5, 1, wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	szMain->Add(FlexGridSizer4, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	SetSizer(szMain);
	startTimer.SetOwner(this, ID_TIMER1);
	startTimer.Start(30, true);
	startTimer.SetOwner(this, ID_TIMER1);
	startTimer.Start(30, true);
	szMain->Fit(this);
	szMain->SetSizeHints(this);
	Center();
	
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DNG4PSFrame::btnSelectPathToRawClick);
	Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DNG4PSFrame::btnRescanClick);
	Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DNG4PSFrame::btnSelectOutputDirClick);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DNG4PSFrame::btnStartClick);
	Connect(ID_BUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DNG4PSFrame::btnOptionsClick);
	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DNG4PSFrame::btnAboutClick);
	Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&DNG4PSFrame::startTimerTrigger);
	Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&DNG4PSFrame::startTimerTrigger);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&DNG4PSFrame::OnClose);
	//*)

	this->SetIcon(main_icon);
}

DNG4PSFrame::~DNG4PSFrame()
{
	//(*Destroy(DNG4PSFrame)
	//*)
}

// DNG4PSFrame::btnAboutClick
void DNG4PSFrame::btnAboutClick(wxCommandEvent& event)
{
	AboutDialog * dialog = new AboutDialog(this);
	dialog->ShowModal();
	dialog->Destroy();
}

// DNG4PSFrame::btnOptionsClick
void DNG4PSFrame::btnOptionsClick(wxCommandEvent& event)
{
	OptionsDialog * dialog = new OptionsDialog(this);

	bool last_recursive = sys().options->recursive_search;
	bool ok = dialog->execute();
	if (ok)
	{
		if (last_recursive != sys().options->recursive_search) fill_files_list();
	}
	dialog->Destroy();
}

// DNG4PSFrame::btnSelectPathToRawClick
void DNG4PSFrame::btnSelectPathToRawClick(wxCommandEvent& event)
{
	FilesPathDialog * dialog = new FilesPathDialog(txtPathToRaw->GetValue(), _("PathToRAWFiles"), this);
	bool ok = dialog->execute();
	if (ok)
	{
		txtPathToRaw->SetValue(dialog->get_path());
		fill_files_list();
	}
	dialog->Destroy();
}

// DNG4PSFrame::OnClose
void DNG4PSFrame::OnClose(wxCloseEvent& event)
{
	read_some_options();
	sys().options->save();
	event.Skip();
}

// DNG4PSFrame::read_some_options
void DNG4PSFrame::read_some_options()
{
	sys().options->path_to_files = txtPathToRaw->GetValue();
	sys().options->output_path = txtOutputDir->GetValue();
}

// DNG4PSFrame::show_progress
void DNG4PSFrame::show_progress(ScanProgressWin * win, int percent, const wxDateTime &start_time)
{
	wxTimeSpan diff = wxDateTime::Now().Subtract(start_time);
	if ((diff.GetMilliseconds() > 100) && !win->IsShown()) win->Show();
	win->set_progress(percent);
	win->Update();
}

// DNG4PSFrame::fill_files_list
void DNG4PSFrame::fill_files_list()
{
	wxListItem itemCol;
	wxString str;
	static const wchar_t * col_strings[] = {_("FileNumber"), _("FileName"), _("FileSize"), L"JPEG", NULL};
	ScanProgressWin *progress_win = new ScanProgressWin(this);

	wxString path = txtPathToRaw->GetValue().Trim();

	lstFileList->ClearAll();
	lstFileList->DeleteAllColumns();

	if (path.IsEmpty()) return;

	file_list->scan(
		path, 
		sys().options->recursive_search, 
		std::bind(&DNG4PSFrame::show_progress, this, progress_win, _1, wxDateTime::Now())
	);

	lstFileList->Hide();

	for (int i = 0; col_strings[i]; i++)
	{
		itemCol.SetText(col_strings[i]);
		lstFileList->InsertColumn(i, itemCol);
	}

	int count = file_list->size();
	for (int i = 0; i < count; i++)
	{
		const FileListItem& item = (*file_list)[i];
		str.Printf(L"%i", i+1);
		int index = lstFileList->InsertItem(i, str, 0);
		lstFileList->SetItem(index, 1, item.name);
		str.Printf(L"%.1f kB", item.size/1024.0);
		lstFileList->SetItem(index, 2, str);
		lstFileList->SetItem(index, 3, item.jpeg);
	}

	for (int i = 0; i < lstFileList->GetColumnCount(); i++)
		lstFileList->SetColumnWidth( i, wxLIST_AUTOSIZE );

	lstFileList->Show();

	progress_win->Destroy();
}

// DNG4PSFrame::btnRescanClick
void DNG4PSFrame::btnRescanClick(wxCommandEvent& event)
{
	fill_files_list();
}

// DNG4PSFrame::btnSelectOutputDirClick
void DNG4PSFrame::btnSelectOutputDirClick(wxCommandEvent& event)
{
	FilesPathDialog * dialog = new FilesPathDialog(txtOutputDir->GetValue(), _("OutputDir"), this);
	bool ok = dialog->execute();
	if (ok) txtOutputDir->SetValue(dialog->get_path());
	dialog->Destroy();
}

// class WorkThread //
class WorkThread : public wxThread
{
private:
	FileList file_list;
	DNG4PSFrame * frame;
	virtual void* Entry();
	virtual void OnExit();

public:
	WorkThread(const FileList & fl, DNG4PSFrame * fr) : file_list(fl), frame(fr)
	{ }
};

void show_log_text(const wxString & text)
{
	wxMutexGuiEnter();
	process_dialog->add_text(text);
	wxMutexGuiLeave();
}

wxThread* work_thread = NULL;

// WorkThread::Entry
void* WorkThread::Entry()
{
	size_t count = file_list.size();
	wxDateTime start_time = wxDateTime::Now();

	for (size_t i = 0; i < count; i++)
	{
		if (TestDestroy()) break;
		const FileListItem & item = file_list[i];

		wxString err_text = L"";
		try
		{
			Utils::process_file(item.path_and_name, item.jpeg_file_name, NULL, NULL, show_log_text);
		}
		catch (const std::exception & e)
		{
			err_text = wxString(e.what(), wxConvLibc);
		}
		catch (const Exception &e)
		{
			err_text = e.what();
		}
		catch (...)
		{
			err_text = _("errorUnknown");
		}

		wxDateTime now = wxDateTime::Now();
		wxTimeSpan diff = now.Subtract(start_time);
		int seconds = diff.GetSeconds().GetLo();
		int total_seconds = seconds*count/(i+1);

		wxMutexGuiEnter();
		process_dialog->set_percent(100*(i+1)/count);
		if (i >= 2) process_dialog->show_time(seconds, total_seconds);
		if (err_text.Length() != 0) process_dialog->add_text(wxString() << L"\n" << _("Error") << L": " << err_text << L"\n");
		wxMutexGuiLeave();
	}

	wxMutexGuiEnter();
	process_dialog->add_text(L"\n==================\n");
	process_dialog->add_text(TestDestroy() ? _("Stopped") : _("Finished"));
	process_dialog->add_text(L"\n");
	process_dialog->set_state(PD_Done);
	wxMutexGuiLeave();

	return NULL;
}

// WorkThread::OnExit
void WorkThread::OnExit()
{
	wxMutexGuiEnter();
	work_thread = NULL;
	wxMutexGuiLeave();
}

// DNG4PSFrame::btnStartClick
void DNG4PSFrame::btnStartClick(wxCommandEvent& event)
{
	if (work_thread != NULL) return;
	if (process_dialog == NULL) process_dialog = new ProcessDialog(this, this);

	read_some_options();
	fill_files_list();

	process_dialog->set_state(PD_Work);
	process_dialog->clear();
	process_dialog->Show();
	Disable();
	work_thread = new WorkThread(*file_list, this);
	work_thread->Create();
	work_thread->Run();
}

// DNG4PSFrame::startTimerTrigger
void DNG4PSFrame::startTimerTrigger(wxTimerEvent& event)
{
  	txtOutputDir->SetValue(sys().options->output_path);
	txtPathToRaw->SetValue(sys().options->path_to_files);
    fill_files_list();
}

// DNG4PSFrame: convertFiles
// for command line, does not use threads
void DNG4PSFrame::convertFiles(wxArrayString files, wxString outputDir)
{
    file_list->clear();
    if( !outputDir.IsEmpty() )
    {
        txtOutputDir->SetValue(outputDir);
        sys().options->output_path = txtOutputDir->GetValue();
    }

    for ( size_t num = 0; num < files.GetCount(); num++ )
    {
        wxString filename = files[num];
        file_list->addOneFile(filename);
    }

	size_t count = file_list->size();
	wxDateTime start_time = wxDateTime::Now();

	for (size_t num = 0; num < count; num++)
	{
		const FileListItem& item= (*file_list)[num];

		wxString err_text = L"";

		try
		{
			Utils::process_file(item.path_and_name, item.jpeg_file_name, nullptr, nullptr, nullptr);
		}
		catch (const std::exception & e)
		{
			err_text = wxString(e.what(), wxConvLibc);
		}
		catch (const Exception &e)
		{
			err_text = e.what();
		}
		catch (...)
		{
			err_text = _("errorUnknown");
		}
    }
}
