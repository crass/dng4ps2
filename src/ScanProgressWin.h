#ifndef SCANPROGRESSWIN_H
#define SCANPROGRESSWIN_H

//(*Headers(ScanProgressWin)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/dialog.h>
#include <wx/gauge.h>
//*)

class ScanProgressWin: public wxDialog
{
	public:

		ScanProgressWin(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~ScanProgressWin();

		//(*Identifiers(ScanProgressWin)
		static const long ID_STATICTEXT1;
		static const long ID_GAUGE1;
		//*)

		void set_progress(int percent);

	protected:

		//(*Handlers(ScanProgressWin)
		//*)

		//(*Declarations(ScanProgressWin)
		wxGauge* gaugProgress;
		wxFlexGridSizer* FlexGridSizer1;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif
