#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <wx/dialog.h>

class AboutDialog: public wxDialog
{
public:
	AboutDialog(wxWindow* parent,wxWindowID id = -1);

protected:
	void txtAboutTextTextUrl(wxTextUrlEvent& event);
};

#endif
