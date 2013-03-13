#ifndef DNG4PSMAIN_H
#define DNG4PSMAIN_H

#include <wx/listctrl.h>
#include <wx/timer.h>

class FileList;
class ScanProgressWin;

class DNG4PSFrame: public wxFrame
{
public:

	DNG4PSFrame(wxWindow* parent,wxWindowID id = -1);
	void convertFiles(wxArrayString files, wxString outputDir); //for command line version

private:
	void btnAboutClick(wxCommandEvent& event);
	void btnOptionsClick(wxCommandEvent& event);
	void btnSelectPathToRawClick(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
	void btnRescanClick(wxCommandEvent& event);
	void lstFileListItemRClick(wxListEvent& event);
	void btnSelectOutputDirClick(wxCommandEvent& event);
	void btnStartClick(wxCommandEvent& event);
	void startTimerTrigger(wxTimerEvent& event);

	wxTimer startTimer;
	wxListCtrl* lstFileList;
	wxTextCtrl* txtPathToRaw;
	wxTextCtrl* txtOutputDir;

	std::auto_ptr<FileList> file_list;
	void fill_files_list();
	void read_some_options();
	void show_progress(ScanProgressWin * win, int percent, const wxDateTime &start_time);
};

#endif // DNG4PSMAIN_H
