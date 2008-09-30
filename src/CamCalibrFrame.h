#ifndef CAMCALIBRFRAME_H
#define CAMCALIBRFRAME_H

#ifndef WX_PRECOMP
	//(*HeadersPCH(CamCalibrFrame)
	#include <wx/scrolwin.h>
	#include <wx/sizer.h>
	#include <wx/stattext.h>
	#include <wx/textctrl.h>
	#include <wx/listbox.h>
	#include <wx/radiobut.h>
	#include <wx/slider.h>
	#include <wx/filedlg.h>
	#include <wx/choice.h>
	#include <wx/statbmp.h>
	#include <wx/button.h>
	#include <wx/frame.h>
	#include <wx/timer.h>
	//*)
#endif
//(*Headers(CamCalibrFrame)
#include <wx/spinctrl.h>
#include <wx/statline.h>
//*)

#include <wx/image.h>

struct ColorHelper;
struct CameraData;

struct ColorArea
{
	int left, top, right, bottom;
	ColorArea() : left(0), top(0), right(0), bottom(0) {}
};

class CamCalibrFrame: public wxFrame
{
	public:

		CamCalibrFrame(const CameraData &camera, wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~CamCalibrFrame();

		boost::function<void (const std::vector<float> &matrix)> on_save_;

		//(*Declarations(CamCalibrFrame)
		wxTextCtrl* txtGamma;
		wxTimer tmrStart;
		wxTimer tmrPaintTimer;
		wxButton* btnSave;
		wxTextCtrl* txtJpegFile;
		wxScrolledWindow* scrwImage;
		wxSpinCtrl* spinY2;
		wxTextCtrl* txtResult;
		wxSpinCtrl* spinY1;
		wxStaticBitmap* sbmpImage;
		wxChoice* chLogLevel;
		wxFileDialog* FileDialog;
		wxTextCtrl* txtBright;
		wxStaticLine* StaticLine2;
		wxSpinCtrl* spinX1;
		wxFlexGridSizer* szMain;
		wxFlexGridSizer* szImg;
		wxChoice* chImageType;
		wxStaticLine* StaticLine3;
		wxStaticLine* StaticLine1;
		wxRadioButton* rbResize;
		wxRadioButton* rbMove;
		wxSlider* sldGamma;
		wxSlider* sldBright;
		wxTextCtrl* txtRawFile;
		wxListBox* lbxAreas;
		wxSpinCtrl* spinX2;
		//*)

	protected:

		//(*Identifiers(CamCalibrFrame)
		static const long ID_STATICTEXT3;
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTON1;
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL2;
		static const long ID_STATICTEXT9;
		static const long ID_SLIDER1;
		static const long ID_TEXTCTRL4;
		static const long ID_STATICTEXT10;
		static const long ID_SLIDER2;
		static const long ID_TEXTCTRL5;
		static const long ID_STATICLINE1;
		static const long ID_STATICTEXT7;
		static const long ID_STATICTEXT5;
		static const long ID_CHOICE1;
		static const long ID_STATICTEXT6;
		static const long ID_LISTBOX1;
		static const long ID_STATICTEXT15;
		static const long ID_RADIOBUTTON2;
		static const long ID_STATICTEXT16;
		static const long ID_RADIOBUTTON1;
		static const long ID_STATICTEXT13;
		static const long ID_SPINCTRL3;
		static const long ID_STATICTEXT12;
		static const long ID_SPINCTRL2;
		static const long ID_STATICTEXT14;
		static const long ID_SPINCTRL4;
		static const long ID_STATICTEXT11;
		static const long ID_SPINCTRL1;
		static const long ID_BUTTON5;
		static const long ID_BUTTON7;
		static const long ID_BUTTON8;
		static const long ID_BUTTON6;
		static const long ID_STATICLINE2;
		static const long ID_STATICTEXT8;
		static const long ID_BUTTON3;
		static const long ID_BUTTON4;
		static const long ID_BUTTON2;
		static const long ID_STATICLINE3;
		static const long ID_STATICTEXT4;
		static const long ID_STATICTEXT17;
		static const long ID_CHOICE2;
		static const long ID_BUTTON9;
		static const long ID_TEXTCTRL3;
		static const long ID_STATICBITMAP_IMAGE;
		static const long ID_SCROLLEDWINDOW1;
		static const long ID_TIMER1;
		static const long ID_TIMER2;
		//*)

	private:
		/*
         * Generated Methods
         */

		//(*Handlers(CamCalibrFrame)
		void OntmrStartTrigger(wxTimerEvent& event);
		void OnImageTypeSelect(wxCommandEvent& event);
		void OntmrPaintTimerTrigger(wxTimerEvent& event);
		void OnProcessClick(wxCommandEvent& event);
		void OnDelAreaClick(wxCommandEvent& event);
		void OnbtnExitClick(wxCommandEvent& event);
		void OnSelectRawClick(wxCommandEvent& event);
		void OnClearAreasClick(wxCommandEvent& event);
		void OnGammaCmdScroll(wxScrollEvent& event);
		void OnClearLogClick(wxCommandEvent& event);
		void OnLogLevelSelect(wxCommandEvent& event);
		void OnAreaSpinCoordChange(wxSpinEvent& event);
		void OnAreasSelect(wxCommandEvent& event);
		void btnSaveClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()

		void OnImageLeftDown(wxMouseEvent& event);
		void OnImageMouseMove(wxMouseEvent& event);
		void OnImageLeftUp(wxMouseEvent& event);
		void OnImagePaint(wxPaintEvent& event);


		/*
         * Custom Members
         */
        // Misc
		bool area_is_selecting_;
		wxImage jpeg_image, raw_image, calculated_image;
		std::vector<ColorArea> areas_;
		ColorArea temp_area;
		const CameraData &camera_;
		std::vector<float> result_matrix_;

        // Detail levels for the process status log
		static const long LOGLVL_BRIEF;
		static const long LOGLVL_STANDARD;
		static const long LOGLVL_VERBOSE;
		long log_level;

        // Records initial values for area spinners
		int old_x1;
		int old_x2;
		int old_y1;
		int old_y2;


		/*
         * Custom Methods
         */
		void load_jpeg_file();
		void load_raw_file(bool update_scroll_bars);
		void show_color_areas();
		void paint_areas();
		void calc_color_for_area(wxImage &image, size_t area_num, ColorHelper &color, bool correct_gamma, double correction, double gammut);
		void log(const wxString &text);
		void update_scroll_window();

		void show_gamma_and_bright();
		double get_curr_gamma();

        // Methods for managing areas
		void area_move();
		void area_resize();
        void area_to_spinners(const ColorArea& current_area);
		bool area_viewable(const ColorArea& current_area);
		void centre_area_view(const ColorArea& current_area);
		void spinners_to_area();
};

#endif
