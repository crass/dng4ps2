#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

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

class CameraOpts;
struct CameraData;

class OptionsDialog: public wxDialog
{
public:

	OptionsDialog(wxWindow* parent,wxWindowID id = -1);

	bool execute();

protected:
	void btnOkClick(wxCommandEvent& event);
	void btnCancelClick(wxCommandEvent& event);
	void chCameraSelectorSelect(wxCommandEvent& event);
	void txtSensWidthText(wxCommandEvent& event);
	void nbMainPageChanged(wxNotebookEvent& event);
	void nbMainPageChanging(wxNotebookEvent& event);
	void btnCopyClick(wxCommandEvent& event);
	void btnDeleteClick(wxCommandEvent& event);
	void btnCalibrClick(wxCommandEvent& event);
	void btnResetToDefaultsClick(wxCommandEvent& event);
	void OnGetLastestClick(wxCommandEvent& event);

	wxTextCtrl* txtCamName;
	wxCheckBox* chbxAddMetadata;
	wxSizer* szGroupsMain;
	wxChoice* chBitsPerUnit;
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

private:
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
