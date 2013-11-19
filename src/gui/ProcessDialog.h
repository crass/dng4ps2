#ifndef PROCESSDIALOG_H
#define PROCESSDIALOG_H

enum PD_States
{
    PD_Work,
    PD_Done
};

class ProcessDialog: public wxDialog
{
public:

	ProcessDialog(wxWindow* parent, wxFrame * frame, wxWindowID id = -1);

	void add_text(const wxString & text);
	void set_state(PD_States state);
	void clear();
	void set_percent(int percent);
	void show_time(int current, int total);

protected:
	void btnExitClick(wxCommandEvent& event);
	virtual void btnStopClick(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);

private:
	wxButton* btnStop;
	wxTextCtrl* txtLog;
	wxGauge* gaPercent;
	wxStaticText* stxtPercent;
	wxButton* btnExit;
	wxStaticText* stxtTime;
    wxFrame * parent;
};

#endif
