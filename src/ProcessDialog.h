#ifndef PROCESSDIALOG_H
#define PROCESSDIALOG_H

//(*Headers(ProcessDialog)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/gauge.h>
//*)

enum PD_States
{
    PD_Work,
    PD_Done
};

class ProcessDialog: public wxDialog
{
	public:

		ProcessDialog(wxWindow* parent, wxFrame * frame, wxWindowID id = -1);
		virtual ~ProcessDialog();

		void add_text(const wxString & text);
		void set_state(PD_States state);
		void clear();
		void set_percent(int percent);
		void show_time(int current, int total);

		//(*Identifiers(ProcessDialog)
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_STATICTEXT3;
		static const long ID_GAUGE1;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		//*)

	protected:

		//(*Handlers(ProcessDialog)
		void btnExitClick(wxCommandEvent& event);
		void btnStopClick(wxCommandEvent& event);
		void OnClose(wxCloseEvent& event);
		//*)

		//(*Declarations(ProcessDialog)
		wxButton* btnStop;
		wxFlexGridSizer* sizerMain;
		wxTextCtrl* txtLog;
		wxGauge* gaPercent;
		wxStaticText* stxtPercent;
		wxButton* btnExit;
		wxStaticText* stxtTime;
		//*)

	private:
        wxFrame * parent;
		DECLARE_EVENT_TABLE()
};

#endif
