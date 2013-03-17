#ifndef SCANPROGRESSWIN_H
#define SCANPROGRESSWIN_H

class ScanProgressWin: public wxDialog
{
public:
	ScanProgressWin(wxWindow* parent);

	void set_progress(int percent);

private:
	wxGauge* gaugProgress;
};

#endif
