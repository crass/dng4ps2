#ifndef FILESPATHDIALOG_H
#define FILESPATHDIALOG_H

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/dirctrl.h>
#include <wx/button.h>
#include <wx/dialog.h>

class FilesPathDialog: public wxDialog
{
public:

	FilesPathDialog(const wxString& path, const wxString& text, wxWindow* parent,wxWindowID id = -1);

	bool execute();
	wxString get_path();

private:
	void btnNewFolderClick(wxCommandEvent& event);

	wxGenericDirCtrl* dirPath;
};

#endif
