#ifndef FILESPATHDIALOG_H
#define FILESPATHDIALOG_H

//(*Headers(FilesPathDialog)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/dirctrl.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

class FilesPathDialog: public wxDialog
{
public:

	FilesPathDialog(const wxString& path, const wxString& text, wxWindow* parent,wxWindowID id = -1);
	virtual ~FilesPathDialog();

	//(*Identifiers(FilesPathDialog)
	static const long ID_STATICTEXT1;
	static const long ID_GENERICDIRCTRL1;
	static const long ID_BUTTON1;
	//*)

	bool execute();
	wxString get_path();

protected:

	//(*Handlers(FilesPathDialog)
	void btnNewFolderClick(wxCommandEvent& event);
	//*)

	//(*Declarations(FilesPathDialog)
	wxFlexGridSizer* FlexGridSizer2;
	wxGenericDirCtrl* dirPath;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;
	//*)

private:

	DECLARE_EVENT_TABLE()
};

#endif
