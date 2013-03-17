#ifndef SCANPROGRESSWIN_H
#define SCANPROGRESSWIN_H

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/dialog.h>
#include <wx/gauge.h>

class ScanProgressWin: public wxDialog
{
public:
	ScanProgressWin(wxWindow* parent);

	void set_progress(int percent);

private:
	wxGauge* gaugProgress;
};

#endif
