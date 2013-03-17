#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

class AboutDialog: public wxDialog
{
public:
	AboutDialog(wxWindow* parent,wxWindowID id = -1);

protected:
	void txtAboutTextTextUrl(wxTextUrlEvent& event);
};

#endif
