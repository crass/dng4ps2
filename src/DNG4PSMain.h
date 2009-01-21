#ifndef DNG4PSMAIN_H
#define DNG4PSMAIN_H

//(*Headers(DNG4PSFrame)
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/timer.h>
//*)

class FileList;
class ScanProgressWin;

class DNG4PSFrame: public wxFrame
{
public:

	DNG4PSFrame(wxWindow* parent,wxWindowID id = -1);
	virtual ~DNG4PSFrame();
	void convertFiles(wxArrayString files, wxString outputDir); //for command line version

private:

	//(*Handlers(DNG4PSFrame)
	void btnAboutClick(wxCommandEvent& event);
	void btnOptionsClick(wxCommandEvent& event);
	void btnSelectPathToRawClick(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
	void btnRescanClick(wxCommandEvent& event);
	void lstFileListItemRClick(wxListEvent& event);
	void btnSelectOutputDirClick(wxCommandEvent& event);
	void btnStartClick(wxCommandEvent& event);
	void startTimerTrigger(wxTimerEvent& event);
	//*)

	//(*Identifiers(DNG4PSFrame)
	static const long ID_STATICTEXT1;
	static const long ID_TEXTCTRL1;
	static const long ID_BUTTON1;
	static const long ID_STATICTEXT3;
	static const long ID_LISTCTRL1;
	static const long ID_BUTTON6;
	static const long ID_STATICTEXT2;
	static const long ID_TEXTCTRL2;
	static const long ID_BUTTON4;
	static const long ID_STATICTEXT4;
	static const long ID_BUTTON2;
	static const long ID_BUTTON5;
	static const long ID_BUTTON3;
	static const long ID_TIMER1;
	//*)

	//(*Declarations(DNG4PSFrame)
	wxTimer startTimer;
	wxListCtrl* lstFileList;
	wxTextCtrl* txtPathToRaw;
	wxButton* btnRescan;
	wxTextCtrl* txtOutputDir;
	//*)

	std::auto_ptr<FileList> file_list;
	void fill_files_list();
	void read_some_options();
	void show_progress(ScanProgressWin * win, int percent, const wxDateTime &start_time);

	DECLARE_EVENT_TABLE()
};

#endif // DNG4PSMAIN_H
