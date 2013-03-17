#ifndef FILESPATHDIALOG_H
#define FILESPATHDIALOG_H

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
