#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

//(*Headers(OptionsDialog)
#include <wx/scrolwin.h>
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

class CameraOpts;
struct CameraData;

class OptionsDialog: public wxDialog
{
	public:

		OptionsDialog(wxWindow* parent,wxWindowID id = -1);
		virtual ~OptionsDialog();

		//(*Identifiers(OptionsDialog)
		static const long ID_STATICTEXT1;
		static const long ID_CHECKBOX1;
		static const long ID_CHECKBOX2;
		static const long ID_STATICTEXT2;
		static const long ID_CHECKBOX3;
		static const long ID_CHECKBOX4;
		static const long ID_STATICTEXT3;
		static const long ID_CHOICE1;
		static const long ID_CHECKBOX5;
		static const long ID_CHOICE2;
		static const long ID_CHECKBOX6;
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT5;
		static const long ID_STATICTEXT6;
		static const long ID_CHOICE3;
		static const long ID_PANEL1;
		static const long ID_STATICTEXT4;
		static const long ID_PANEL2;
		static const long ID_STATICTEXT12;
		static const long ID_CHOICE9;
		static const long ID_PANEL3;
		static const long ID_BUTTON6;
		static const long ID_BUTTON3;
		static const long ID_BUTTON2;
		static const long ID_BUTTON5;
		static const long ID_STATICTEXT13;
		static const long ID_TEXTCTRL2;
		static const long ID_STATICTEXT8;
		static const long ID_TEXTCTRL5;
		static const long ID_STATICTEXT30;
		static const long ID_STATICTEXT14;
		static const long ID_TEXTCTRL3;
		static const long ID_STATICTEXT15;
		static const long ID_TEXTCTRL4;
		static const long ID_STATICTEXT7;
		static const long ID_STATICTEXT17;
		static const long ID_TEXTCTRL6;
		static const long ID_STATICTEXT18;
		static const long ID_TEXTCTRL7;
		static const long ID_STATICTEXT19;
		static const long ID_TEXTCTRL8;
		static const long ID_STATICTEXT20;
		static const long ID_TEXTCTRL9;
		static const long ID_STATICTEXT21;
		static const long ID_TEXTCTRL10;
		static const long ID_STATICTEXT22;
		static const long ID_TEXTCTRL11;
		static const long ID_STATICTEXT23;
		static const long ID_TEXTCTRL12;
		static const long ID_STATICTEXT24;
		static const long ID_TEXTCTRL13;
		static const long ID_STATICTEXT25;
		static const long ID_CHOICE10;
		static const long ID_STATICTEXT31;
		static const long ID_STATICTEXT26;
		static const long ID_CHOICE11;
		static const long ID_STATICTEXT27;
		static const long ID_TEXTCTRL15;
		static const long ID_BUTTON4;
		static const long ID_STATICTEXT28;
		static const long ID_TEXTCTRL16;
		static const long ID_STATICTEXT32;
		static const long ID_STATICTEXT29;
		static const long ID_CHOICE12;
		static const long ID_STATICTEXT33;
		static const long ID_TEXTCTRL18;
		static const long ID_STATICTEXT34;
		static const long ID_TEXTCTRL19;
		static const long ID_SCROLLEDWINDOW1;
		static const long ID_PNL_CAM_OPTS;
		static const long ID_NOTEBOOK1;
		//*)

		bool execute();

	protected:

		//(*Handlers(OptionsDialog)
		void btnOkClick(wxCommandEvent& event);
		void btnCancelClick(wxCommandEvent& event);
		void chbxUseDateForPathClick(wxCommandEvent& event);
		void chkbArtistClick(wxCommandEvent& event);
		void chCameraSelectorSelect(wxCommandEvent& event);
		void txtSensWidthText(wxCommandEvent& event);
		void nbMainPageChanged(wxNotebookEvent& event);
		void nbMainPageChanging(wxNotebookEvent& event);
		void btnCopyClick(wxCommandEvent& event);
		void btnDeleteClick(wxCommandEvent& event);
		void btnCalibrClick(wxCommandEvent& event);
		void btnResetToDefaultsClick(wxCommandEvent& event);
		void PnlSecretLeftDClick(wxMouseEvent& event);
		//*)

		//(*Declarations(OptionsDialog)
		wxTextCtrl* txtCamName;
		wxCheckBox* chbxAddMetadata;
		wxFlexGridSizer* szGroupsMain;
		wxChoice* chLanguage;
		wxTextCtrl* txtJpegHeight;
		wxCheckBox* chbxDontOverwrite;
		wxTextCtrl* txtMatrix2Mult;
		wxTextCtrl* txtMatr2;
		wxStaticText* lblGroupNotice;
		wxTextCtrl* txtBlackLevel;
		wxTextCtrl* txtActiveY1;
		wxChoice* chIll2;
		wxCheckBox* chbxUseDateForPath;
		wxCheckBox* chkbArtist;
		wxPanel* pnlGroups;
		wxChoice* chIll1;
		wxTextCtrl* txtActiveX2;
		wxTextCtrl* txtShortName;
		wxChoice* chPreview;
		wxTextCtrl* txtWhiteLevel;
		wxTextCtrl* txtSensHeight;
		wxTextCtrl* txtMatrix1Mult;
		wxTextCtrl* txtActiveX1;
		wxChoice* chCameraSelector;
		wxCheckBox* chbxCompressDng;
		wxTextCtrl* txtSensWidth;
		wxTextCtrl* txtArtist;
		wxCheckBox* chbxIncludeSubfilders;
		wxFlexGridSizer* szGroups;
		wxStaticText* lblFileSize;
		wxChoice* chMosaicType;
		wxChoice* chDateType;
		wxTextCtrl* txtActiveY2;
		wxTextCtrl* txtJpegWidth;
		wxTextCtrl* txtMatr1;
		//*)

	private:

		DECLARE_EVENT_TABLE()

		std::auto_ptr<CameraOpts> cam_opts_;
		typedef std::map<unsigned int, wxChoice*> GroupControls;
		GroupControls group_controls_;
		wxString lastCameraId;
		const CameraData* get_selected_camera_id();

		void correct_interface();
		void show_cameras_list(const wxString &camera_to_select);
		void show_camera_opts();
		void show_groups();
		void read_groups();

		void read_camera_opts();

		static void show_illum_type(wxChoice *coice, int value);
		static int get_illum_type(wxChoice *coice);
		static int get_int(wxTextCtrl *txt);

		static void fill_illum_strings(wxChoice *coice);

		void matrix_calculated(const std::vector<float> &matrix);
};

#endif
