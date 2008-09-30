#ifndef NEWFOLDERDIALOG_H
#define NEWFOLDERDIALOG_H

//(*Headers(NewFolderDialog)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/dialog.h>
//*)

class NewFolderDialog: public wxDialog
{
	public:

		NewFolderDialog(wxWindow* parent,wxWindowID id=wxID_ANY);
		virtual ~NewFolderDialog();

		//(*Identifiers(NewFolderDialog)
		static const long ID_STATICTEXT3;
		static const long ID_STATICTEXT1;
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL1;
		//*)

		bool execute(const wxString &root);
		wxString get_path();

	protected:

		//(*Handlers(NewFolderDialog)
		void OnInit(wxInitDialogEvent& event);
		//*)

		//(*Declarations(NewFolderDialog)
		wxFlexGridSizer* sizerMain;
		wxFlexGridSizer* FlexGridSizer2;
		wxTextCtrl* txtFolderName;
		wxStaticText* stxtRootFolder;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif
