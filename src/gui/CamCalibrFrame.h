#ifndef CAMCALIBRFRAME_H
#define CAMCALIBRFRAME_H

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
	CamCalibrFrame(const CameraData &camera, wxWindow* parent);

	std::function<void (const std::vector<float> &matrix)> on_save_; // TODO: move into private section

private:
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

	void OnImageLeftDown(wxMouseEvent& event);
	void OnImageMouseMove(wxMouseEvent& event);
	void OnImageLeftUp(wxMouseEvent& event);
	void OnImagePaint(wxPaintEvent& event);

	bool area_is_selecting_;
	wxImage jpeg_image, raw_image, calculated_image;
	std::vector<ColorArea> areas_;
	ColorArea temp_area;
	const CameraData &camera_;
	std::vector<float> result_matrix_;

	// Detail levels for the process status log
	enum LogLevel
	{
		LOGLVL_BRIEF = 0,
		LOGLVL_STANDARD = 1,
		LOGLVL_VERBOSE = 2
	};

	LogLevel log_level;

	// Records initial values for area spinners
	int old_x1;
	int old_x2;
	int old_y1;
	int old_y2;

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
