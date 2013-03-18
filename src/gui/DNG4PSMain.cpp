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

#include "DNG4PSMain.h"
#include "FileList.h"
#include "ProcessDialog.h"
#include "FilesPathDialog.h"
#include "AboutDialog.h"
#include "OptionsDialog.h"
#include "Options.h"
#include "Utils.h"
#include "ProgramObjects.h"
#include "ScanProgressWin.h"
#include "Exception.hpp"
#include "lib/wxGUIBuilder.hpp"

extern wxIcon main_icon;
ProcessDialog * process_dialog = NULL;

DNG4PSFrame::DNG4PSFrame(wxWindow* parent,wxWindowID id) : file_list(new FileList)
{
	using namespace gb;

	wxButton* btnSelectOutputDir = nullptr;
	wxButton* btnAbout = nullptr;
	wxButton* btnStart = nullptr;
	wxButton* btnOptions = nullptr;
	wxButton* btnSelectPathToRaw = nullptr;
	wxButton* btnRescan = nullptr;
	
	Create(parent, id, _("MainFormCaption"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL, _T("id"));
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	auto gui = existing_window(this, size(400, 400)) 
	[ 
		vbox(stretch | expand)
		[
			text("PathToRAWFiles", bord_all_exc_bottom),
			hbox(expand | border(0))
			[
				edit(stretch) >> txtPathToRaw,
				button("...", width(15)) >> btnSelectPathToRaw
			],
			text("RawFilesList", bord_all_exc_bottom),
			list(stretch | expand | l_report) >> lstFileList,
			button("ButtonRescan", align_right) >> btnRescan,
			text("OutputDir", bord_all_exc_bottom),
			hbox(expand | border(0))
			[
				edit(stretch) >> txtOutputDir,
				button("...", width(15)) >> btnSelectOutputDir
			],
			text("ActionsLabel", bord_all_exc_bottom),
			hbox(expand | border(0))
			[
				button("ButtonGo", wxID_ANY, true) >> btnStart,
				button("ButtonOptions") >> btnOptions,
				button("ButtonAbout") >> btnAbout
			]
		]
	];

	gui.build();

	Layout();
	GetSizer()->SetSizeHints(this);
	Center();

	btnSelectPathToRaw->Bind(wxEVT_COMMAND_BUTTON_CLICKED, [this] (wxCommandEvent& event) { btnSelectPathToRawClick(event); });
	btnRescan         ->Bind(wxEVT_COMMAND_BUTTON_CLICKED, [this] (wxCommandEvent& event) { btnRescanClick(event);          });
	btnSelectOutputDir->Bind(wxEVT_COMMAND_BUTTON_CLICKED, [this] (wxCommandEvent& event) { btnSelectOutputDirClick(event); });
	btnStart          ->Bind(wxEVT_COMMAND_BUTTON_CLICKED, [this] (wxCommandEvent& event) { btnStartClick(event);           });
	btnOptions        ->Bind(wxEVT_COMMAND_BUTTON_CLICKED, [this] (wxCommandEvent& event) { btnOptionsClick(event);         });
	btnAbout          ->Bind(wxEVT_COMMAND_BUTTON_CLICKED, [this] (wxCommandEvent& event) { btnAboutClick(event);           });
	this              ->Bind(wxEVT_CLOSE_WINDOW,           [this] (wxCloseEvent&   event) { OnClose(event);                 });

	startTimer.SetOwner(this);
	startTimer.Start(30, true);
	Bind(wxEVT_TIMER, [this] (wxTimerEvent& event) { startTimerTrigger(event); }, startTimer.GetId());

	this->SetIcon(main_icon);
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
