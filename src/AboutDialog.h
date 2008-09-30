#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

//(*Headers(AboutDialog)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/statbmp.h>
#include <wx/dialog.h>
//*)

class AboutDialog: public wxDialog
{
	public:

		AboutDialog(wxWindow* parent,wxWindowID id = -1);
		virtual ~AboutDialog();

		//(*Identifiers(AboutDialog)
		static const long ID_STATICBITMAP1;
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL2;
		//*)

	protected:

		//(*Handlers(AboutDialog)
		void txtAboutTextTextUrl(wxTextUrlEvent& event);
		//*)

		//(*Declarations(AboutDialog)
		wxTextCtrl* txtAboutText;
		wxFlexGridSizer* FlexGridSizer2;
		wxTextCtrl* TextCtrl1;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif
