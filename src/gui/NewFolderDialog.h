#ifndef NEWFOLDERDIALOG_H
#define NEWFOLDERDIALOG_H

class NewFolderDialog: public wxDialog
{
public:
	NewFolderDialog(wxWindow* parent,wxWindowID id=wxID_ANY);

	bool execute(const wxString &root);
	wxString get_path();

protected:
	wxTextCtrl* txtFolderName;
	wxStaticText* stxtRootFolder;
};

#endif
