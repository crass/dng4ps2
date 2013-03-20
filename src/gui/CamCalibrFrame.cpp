/*=============================================================================
  DNG4PS - software for converting Canon PS RAW files to DNG ones
  Copyright (C) 2008 Denis Artyomov

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  =============================================================================
  File:      CamCalibrFrame.cpp
  Author(s): Denis Artyomov (denis.artyomov@gmail.com)
             Matt Dawson (coder@dawsons.id.au)
  ===========================================================================*/

#include "pch.h"

#include "CamCalibrFrame.h"
#include "Exception.hpp"
#include "CameraOpts.h"
#include "Utils.h"
#include "lib/wxGUIBuilder.hpp"

CamCalibrFrame::CamCalibrFrame(const CameraData &camera, wxWindow* parent) : camera_(camera)
{
	wxButton* btnProcess = nullptr;
	wxButton* btnSelectRaw = nullptr;
	wxButton* btnDelArea = nullptr;
	wxButton* btnExit = nullptr;
	wxButton* btnLoad = nullptr;
	wxButton* btnClearLog = nullptr;
	wxButton* btnClearAreas = nullptr;
	wxButton* btnSaveAreas = nullptr;

	// Build GUI
	Create(parent, wxID_ANY, _("Camera calibration"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	using namespace gb;

	auto image_type_choice = choice() 
	[ 
		item("Jpeg file"), 
		item("RAW file"), 
		item("Calculated image") 
	];

	auto log_level_choice = choice() 
	[
		item("Minimal"), 
		item("Normal"), 
		item("Verbose")
	];

	auto gui = existing_window(this)
	[
		hbox(expand | stretch)
		[
			vbox(expand)
			[
				text("Images source", font_bold),
				vbox(expand | border(4) | bord_all_exc_top)
				[
					grid(2, 0, expand | border(0), growable_cols(0))
					[
						text("RAW file:", bord_all_exc_bottom),
						spacer(),
						edit(expand) >> txtRawFile,
						button("...", size(14, 12)) >> btnSelectRaw,
						text("JPEG file:", bord_all_exc_bottom),
						spacer(),
						edit(expand | ed_readonly) >> txtJpegFile
					],
					grid(3, 0, expand | border(0), growable_cols(1))
					[
						text("Gamma:"),
						slider(50, 300, width(100)) >> sldGamma,
						edit(width(30) | ed_readonly) >> txtGamma,
						text("Im. bright.:"),
						slider(50, 300, width(100)) >> sldBright,
						edit(width(30) | ed_readonly) >> txtBright
					]
				],
				hline(),

				text("Areas selection", font_bold),
				vbox(expand | border(4) | bord_all_exc_top)
				[
					hbox(border(0) | expand)
					[
						vbox(border(0) | expand | stretch)
						[
							hbox(border(0) | expand)
							[
								text("Show:"),
								image_type_choice >> chImageType
							],
							text("Areas:", bord_all_exc_bottom),
							listbox(expand | stretch) >> lbxAreas
						],

						grid(2, 0, border(0))
						[
							spacer(),    radio("Move", true) >> rbMove,
							spacer(),    radio("Resize") >> rbResize,
							text("X1:"), spin(0, 10000, width(36)) >> spinX1,
							text("X2:"), spin(0, 10000, width(36)) >> spinX2,
							text("Y1:"), spin(0, 10000, width(36)) >> spinY1,
							text("Y2:"), spin(0, 10000, width(36)) >> spinY2
						]
					],
					hbox(border(0))
					[
						button("Save...", bu_exactfit) >> btnSaveAreas,
						button("Load...", bu_exactfit) >> btnLoad,
						button("Clear", bu_exactfit) >> btnClearAreas,
						button("Delete", bu_exactfit) >> btnDelArea
					]
				],
				hline(),
				text("Control", font_bold),
				hbox(expand | border(4) | bord_all_exc_top)
				[
					button("Process") >> btnProcess,
					button("Exit") >> btnExit,
					button("Save") >> btnSave
				],
				hline(),

				text("Progress and results", font_bold),
				vbox(expand | stretch | border(4) | bord_all_exc_top)
				[
					hbox(border(0))
					[
						text("Log level:"),
						log_level_choice >> chLogLevel,
						button("Clear log") >> btnClearLog
					],
					edit(ed_multiline | ed_readonly | stretch | expand) >> txtResult
				]
			],
			scroll_box(expand | stretch)
			[
				image() >> sbmpImage
			] >> scrwImage
		]
	];

	gui.build(1);

	btnSaveAreas->Disable();
	btnLoad->Disable();

	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);

	tmrStart.SetOwner(this);
	tmrStart.Start(40, false);
	tmrPaintTimer.SetOwner(this);
	tmrPaintTimer.Start(30, true);
	FileDialog = new wxFileDialog(this, _("Select file"), wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_DEFAULT_STYLE|wxFD_OPEN, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));

	sldGamma->SetValue(100);
	sldBright->SetValue(100);
	chLogLevel->SetSelection(int(LOGLVL_STANDARD));

	area_is_selecting_ = false;
	
	// Connect events
	btnSelectRaw ->Bind(wxEVT_COMMAND_BUTTON_CLICKED,   [this] (wxCommandEvent &event) { OnSelectRawClick(event);       });
	sldGamma     ->Bind(wxEVT_SCROLL_CHANGED,           [this] (wxScrollEvent  &event) { OnGammaCmdScroll(event);       });
	sldBright    ->Bind(wxEVT_SCROLL_CHANGED,           [this] (wxScrollEvent  &event) { OnGammaCmdScroll(event);       });
	chImageType  ->Bind(wxEVT_COMMAND_CHOICE_SELECTED,  [this] (wxCommandEvent &event) { OnImageTypeSelect(event);      });
	lbxAreas     ->Bind(wxEVT_COMMAND_LISTBOX_SELECTED, [this] (wxCommandEvent &event) { OnAreasSelect(event);          });
	spinX1       ->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, [this] (wxSpinEvent    &event) { OnAreaSpinCoordChange(event);  });
	spinX2       ->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, [this] (wxSpinEvent    &event) { OnAreaSpinCoordChange(event);  });
	spinY1       ->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, [this] (wxSpinEvent    &event) { OnAreaSpinCoordChange(event);  });
	spinY2       ->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, [this] (wxSpinEvent    &event) { OnAreaSpinCoordChange(event);  });
	btnClearAreas->Bind(wxEVT_COMMAND_BUTTON_CLICKED,   [this] (wxCommandEvent &event) { OnClearAreasClick(event);      });
	btnDelArea   ->Bind(wxEVT_COMMAND_BUTTON_CLICKED,   [this] (wxCommandEvent &event) { OnDelAreaClick(event);         });
	btnProcess   ->Bind(wxEVT_COMMAND_BUTTON_CLICKED,   [this] (wxCommandEvent &event) { OnProcessClick(event);         });
	btnExit      ->Bind(wxEVT_COMMAND_BUTTON_CLICKED,   [this] (wxCommandEvent &event) { OnbtnExitClick(event);         });
	btnSave      ->Bind(wxEVT_COMMAND_BUTTON_CLICKED,   [this] (wxCommandEvent &event) { btnSaveClick(event);           });
	chLogLevel   ->Bind(wxEVT_COMMAND_CHOICE_SELECTED,  [this] (wxCommandEvent &event) { OnLogLevelSelect(event);       });
	btnClearLog  ->Bind(wxEVT_COMMAND_BUTTON_CLICKED,   [this] (wxCommandEvent &event) { OnClearLogClick(event);        });
	sbmpImage    ->Bind(wxEVT_LEFT_DOWN,                [this] (wxMouseEvent   &event) { OnImageLeftDown(event);        });
	sbmpImage    ->Bind(wxEVT_MOTION,                   [this] (wxMouseEvent   &event) { OnImageMouseMove(event);       });
	sbmpImage    ->Bind(wxEVT_LEFT_UP,                  [this] (wxMouseEvent   &event) { OnImageLeftUp(event);          });
	sbmpImage    ->Bind(wxEVT_PAINT,                    [this] (wxPaintEvent   &event) { OnImagePaint(event);           });

	Connect(tmrStart.GetId(),      wxEVT_TIMER,(wxObjectEventFunction)&CamCalibrFrame::OntmrStartTrigger);
	Connect(tmrPaintTimer.GetId(), wxEVT_TIMER,(wxObjectEventFunction)&CamCalibrFrame::OntmrPaintTimerTrigger);

	scrwImage->SetScrollRate(16, 16);
	scrwImage->ShowScrollbars(wxSHOW_SB_ALWAYS, wxSHOW_SB_ALWAYS);
	show_gamma_and_bright();
    log_level = LogLevel(chLogLevel->GetSelection());
}

/* CamCalibrFrame::OntmrStartTrigger */
void CamCalibrFrame::OntmrStartTrigger(wxTimerEvent& event)
{
	tmrStart.Stop();
	chImageType->SetSelection(0);

	//load_jpeg_file();
	//load_raw_file(true);
	show_color_areas();
}

/* CamCalibrFrame::load_jpeg_file */
void CamCalibrFrame::load_jpeg_file()
{
	wxString jpeg_file = txtJpegFile->GetValue();
	if (jpeg_file.IsEmpty()) return;
	jpeg_image.LoadFile(jpeg_file);
	if (chImageType->GetSelection() == 0) sbmpImage->SetBitmap(jpeg_image);
}

/*  CamCalibrFrame::load_raw_file */
void CamCalibrFrame::load_raw_file(bool update_scroll_bars)
{
	wxString raw_file = txtRawFile->GetValue();
	if (raw_file.IsEmpty()) return;

	unsigned long file_size = wxFileName::GetSize(raw_file).GetLo();
	if (file_size != CameraOpts::get_file_size(camera_))
		throw Exception(wxString::Format(_("wrong_camera"), camera_.model_name.c_str()));

	double bright = 1;
	txtBright->GetValue().ToDouble(&bright);
	Utils::create_raw_image(&camera_, raw_file, NULL, raw_image, true, bright, get_curr_gamma());

	if (chImageType->GetSelection() == 1) sbmpImage->SetBitmap(raw_image);
}

void CamCalibrFrame::update_scroll_window()
{
	scrwImage->SetScrollPos(wxHORIZONTAL, 0, true);
	scrwImage->SetScrollPos(wxVERTICAL, 0, true);

	sbmpImage->FitInside();
	scrwImage->GetSizer()->Layout();

	scrwImage->GetSizer()->SetSizeHints(scrwImage);

	GetSizer()->Layout();
}

void CamCalibrFrame::OnImageTypeSelect(wxCommandEvent& event)
{
	switch (chImageType->GetSelection())
	{
	case 0:
		sbmpImage->SetBitmap(jpeg_image);
		break;

	case 1:
		sbmpImage->SetBitmap(raw_image);
		break;

	case 2:
		sbmpImage->SetBitmap(calculated_image);
		break;
	}

	//scrwImage->AdjustScrollbars();
}

void CamCalibrFrame::show_color_areas()
{
	int last_selection = lbxAreas->GetSelection();
	lbxAreas->Clear();

	wxString buffer;

	for (size_t i = 0; i < areas_.size(); i++)
	{
		const ColorArea &area = areas_[i];

		buffer.Printf(L"%i: [%i,%i,%i,%i]", i+1, area.left, area.top, area.right, area.bottom);
		lbxAreas->Append(buffer);
	}
	if (last_selection < lbxAreas->GetCount()) lbxAreas->SetSelection(last_selection);
}

void CamCalibrFrame::OnImageLeftDown(wxMouseEvent& event)
{
	area_is_selecting_ = false;
	temp_area.right = temp_area.left = event.GetX();
	temp_area.bottom = temp_area.top = event.GetY();
	area_is_selecting_ = true;
}

void CamCalibrFrame::OnImageMouseMove(wxMouseEvent& event)
{
}

void CamCalibrFrame::OnImageLeftUp(wxMouseEvent& event)
{
	if (area_is_selecting_)
	{
		area_is_selecting_ = false;

		temp_area.right = event.GetX();
		temp_area.bottom = event.GetY();

		if ((temp_area.left == temp_area.right) || (temp_area.top == temp_area.bottom)) return;

		if (temp_area.left > temp_area.right) std::swap(temp_area.left, temp_area.right);
		if (temp_area.top > temp_area.bottom) std::swap(temp_area.top, temp_area.bottom);
		areas_.push_back(temp_area);

		show_color_areas();
		sbmpImage->RefreshRect(wxRect(temp_area.left, temp_area.top, temp_area.right-temp_area.left, temp_area.bottom-temp_area.top));

		lbxAreas->SetSelection(areas_.size()-1);

		area_to_spinners(temp_area);
	}
}

void CamCalibrFrame::OnImagePaint(wxPaintEvent& event)
{
	event.Skip();
	tmrPaintTimer.Start();
}

void CamCalibrFrame::OntmrPaintTimerTrigger(wxTimerEvent& event)
{
	tmrPaintTimer.Stop();
	paint_areas();
}

void CamCalibrFrame::paint_areas()
{
	wxWindowDC dc(sbmpImage);
	wxString buffer;

	dc.SetPen(*wxWHITE_PEN);
	dc.SetBackgroundMode(wxSOLID);
	dc.SetTextBackground(*wxWHITE);
	dc.SetTextForeground(*wxBLACK);
	dc.SetBrush(*wxTRANSPARENT_BRUSH);

	for (size_t i = 0; i < areas_.size(); i++)
	{
		const ColorArea &area = areas_[i];
		dc.DrawRectangle(area.left, area.top, area.right-area.left, area.bottom-area.top);
		buffer.Printf(L" %i ", i+1);
		dc.DrawText(buffer, area.left+1, area.top+1);
	}
}

void CamCalibrFrame::log(const wxString &text)
{
	txtResult->AppendText(text+L"\n");
}

void CamCalibrFrame::calc_color_for_area(wxImage &image, size_t area_num, ColorHelper &color, bool correct_gamma, double correction, double gammut)
{
	//double gamma = gammut;

	const ColorArea &area = areas_[area_num];
	int x1 = area.left;
	int y1 = area.top;
	int x2 = area.right;
	int y2 = area.bottom;
	if (x1 > x2) std::swap(x1, x2);
	if (y1 > y2) std::swap(y1, y2);

	unsigned long long red_total = 0;
	unsigned long long green_total = 0;
	unsigned long long blue_total = 0;
	unsigned long long counter = 0;

	for (int x = x1; x < x2; x++)
	{
		for (int y = y1; y < y2; y++)
		{
			red_total += image.GetRed(x, y);
			green_total += image.GetGreen(x, y);
			blue_total += image.GetBlue(x, y);
			counter++;
		}
	}

	color.r = (double)red_total/(double)(counter*255);
	color.g = (double)green_total/(double)(counter*255);
	color.b = (double)blue_total/(double)(counter*255);

	if (correct_gamma)
	{
		if (color.r > 0.04045) color.r = pow(( ( color.r + 0.055 ) / 1.055 ), gammut);
		else color.r = color.r / 12.92;

		if (color.g > 0.04045) color.g = pow(( ( color.g + 0.055 ) / 1.055 ), gammut);
		else color.g = color.g / 12.92;

		if (color.b > 0.04045) color.b = pow(( ( color.b + 0.055 ) / 1.055 ), gammut);
		else color.b = color.b / 12.92;
	}

	color.r *= correction;
	color.g *= correction;
	color.b *= correction;

}

/* CamCalibrFrame::OnProcessClick */
void CamCalibrFrame::OnProcessClick(wxCommandEvent& event)
{
	wxString buffer;
	const double imagine_gammut = 1.9;
	double m[3][3];
	double inv_m[3][3];
	double max_error, aver_error;
	static const double max_val = 0.90;
	static const double acceptable_error = 0.15;
	const int iter_count = 10000;

	btnSave->Enable(false);

	std::vector<Area> areas;
	Area area;

	double bright = 1;
	txtBright->GetValue().ToDouble(&bright);

	wxString raw_file = txtRawFile->GetValue();

	Utils::create_raw_image(&camera_, raw_file, /*inv_matrix*/NULL, raw_image, true, 1, imagine_gammut);

	double gammut_start = 1.4;
	double gammut_stop = 2.6;
	double gammut_step = 0.01;
	double best_gammut;
	log(L"TESTING GAMMUT. WAIT..."); // All logging levels
	for (int stage = 1; stage <= 4; stage++)
	{
		buffer.Printf(L"Stage %i", stage);
		log(buffer); // All logging levels

		best_gammut = 0;
		double min_error = 1000;
		for (double gammut = gammut_start; gammut <= gammut_stop; gammut += gammut_step)
		{
			areas.clear();

			for (size_t i = 0; i < areas_.size(); i++)
			{
				calc_color_for_area(jpeg_image, i, area.jpeg, true, 1/bright, gammut);
				calc_color_for_area(raw_image, i, area.camera, true, 1, imagine_gammut);
				if ((area.jpeg.r > max_val) || (area.jpeg.g > max_val) || (area.jpeg.b > max_val) ||
				        (area.camera.r > max_val) || (area.camera.g > max_val) || (area.camera.b > max_val))
					continue;

				areas.push_back(area);
			}

			if (log_level >= LOGLVL_VERBOSE)
			{
				Utils::calc_camera_profile(m, areas, iter_count, std::bind(&CamCalibrFrame::log, this, _1), &max_error, &aver_error);
			}
			else
			{
				Utils::calc_camera_profile(m, areas, iter_count, nullptr, &max_error, &aver_error);
			}

			if (max_error < min_error)
			{
				min_error = max_error;
				best_gammut = gammut;
			}

			if (log_level >= LOGLVL_STANDARD)
			{
				buffer.Printf(L"G=%.5f, range(E)=%.1f%%->%.1f%%, aver(E)=%.1f%%", gammut, 100.0*min_error, 100.0*max_error, 100.0*aver_error);
				log(buffer);
			}
		}

		if (min_error > acceptable_error) // All logging levels
		{
			log(L"\nPOOR RESULTS. CAN'T CONTINUE!!!");
			buffer.Printf(L"\nREQUIRES MIN <= %.1f%%.\nBEST RESULTS FROM G=%.5f, min(E)=%.1f%%", 100.0*acceptable_error, best_gammut, 100.0*min_error);
			log(buffer);
			return;
		}

		gammut_start = best_gammut-2*gammut_step;
		gammut_stop = best_gammut+2*gammut_step;
		gammut_step /= 10.0;
	}

	buffer.Printf(L"Best gammut=%.5f", best_gammut);
	log(buffer);

	areas.clear();

	log(L"\nCALCULATION");
	for (size_t i = 0; i < areas_.size(); i++)
	{
		buffer.Printf(L"Area %i", i+1);
		log(buffer);
		calc_color_for_area(jpeg_image, i, area.jpeg, true, 1/bright, best_gammut);
		calc_color_for_area(raw_image, i, area.camera, true, 1, imagine_gammut);

		buffer.Printf(L"jpeg color r=%.2f, g=%.2f, b=%.2f", area.jpeg.r, area.jpeg.g, area.jpeg.b);
		log(buffer);
		buffer.Printf(L"camera color r=%.2f, g=%.2f, b=%.2f", area.camera.r, area.camera.g, area.camera.b);
		log(buffer);

		if ((area.jpeg.r > max_val) || (area.jpeg.g > max_val) || (area.jpeg.b > max_val) ||
		        (area.camera.r > max_val) || (area.camera.g > max_val) || (area.camera.b > max_val))
		{
			log(L"[bad!]");
			continue;
		}

		areas.push_back(area);
	}

	log(L"\nRESULTS");

	Utils::calc_camera_profile(m, areas, iter_count, std::bind(&CamCalibrFrame::log, this, _1), &max_error, &aver_error);

	Utils::inverse_matrix(m, inv_m);

	log(L"Camera -> XYZ matrix:");
	for (int i = 0; i < 3; i++)
	{
		buffer.Printf(L"%.6f %.6f %.6f", m[0][i], m[1][i], m[2][i]);
		log(buffer);
	}

	log(L"XYZ -> Camera matrix:");
	for (int i = 0; i < 3; i++)
	{
		buffer.Printf(L"%.6f %.6f %.6f", inv_m[0][i], inv_m[1][i], inv_m[2][i]);
		log(buffer);
	}

	result_matrix_.clear();
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++) result_matrix_.push_back(inv_m[j][i]);


	wxImage result_image;
	Utils::create_raw_image(&camera_, raw_file, m, result_image, true, 1, best_gammut);
	//Utils::process_file(raw_file, wxString(), &result_image, inv_m, NULL);


	calculated_image = result_image;

	sbmpImage->SetBitmap(calculated_image);
	chImageType->SetSelection(2);

	btnSave->Enable(true);
}

/* CamCalibrFrame::OnDelAreaClick */
void CamCalibrFrame::OnDelAreaClick(wxCommandEvent& event)
{
	int index = lbxAreas->GetSelection();
	if (index == -1) return;

	const ColorArea &area = areas_.at(index);
	wxRect rect = wxRect(area.left, area.top, area.right-area.left, area.bottom-area.top);

	areas_.erase(areas_.begin()+index);
	show_color_areas();
	lbxAreas->SetSelection(-1);

	sbmpImage->RefreshRect(rect);
	for (size_t i = 0; i > areas_.size(); i++)
	{
		const ColorArea &area = areas_.at(i);
		sbmpImage->RefreshRect(wxRect(area.left, area.top, area.right-area.left, area.bottom-area.top));
	}
}

/* CamCalibrFrame::OnbtnExitClick */
void CamCalibrFrame::OnbtnExitClick(wxCommandEvent& event)
{
	Close();
}

/* CamCalibrFrame::OnSelectRawClick */
void CamCalibrFrame::OnSelectRawClick(wxCommandEvent& event)
{
	try
	{
		FileDialog->SetPath(txtRawFile->GetValue());
		int res = FileDialog->ShowModal();
		if (res != wxID_OK) return;
		wxString path = FileDialog->GetPath();
		txtRawFile->SetValue(path);

		load_raw_file(true);

		wxFileName jpeg_name;
		bool jpeg_exists = Utils::get_jpeg_name(path, jpeg_name);
		if (jpeg_exists)
		{
			txtJpegFile->SetValue(jpeg_name.GetLongPath());
			load_jpeg_file();
		}
		else
		{
			throw Exception(_("cant_find_jpeg_file"));
		}

		update_scroll_window();
	}
	catch (const Exception &e)
	{
		wxString err_text = wxString(_("dlgOpenRawError"))+L"\n\n"+e.what();
		wxMessageDialog *dialog = new wxMessageDialog(
			this,
			err_text.c_str(),
			_("dlgOpenRawError"),
			wxICON_ERROR
		);
		dialog->ShowModal();
	}
}

/* CamCalibrFrame::OnClearAreasClick */
void CamCalibrFrame::OnClearAreasClick(wxCommandEvent& event)
{
	areas_.clear();
	show_color_areas();
	sbmpImage->Refresh();
}

/* CamCalibrFrame::show_gamma_and_bright */
void CamCalibrFrame::show_gamma_and_bright()
{
	wxString buffer;
	double gamma = sldGamma->GetValue()/100.0;
	buffer.Printf(L"%.2f", gamma);
	txtGamma->SetValue(buffer);
	double bright = sldBright->GetValue()/100.0;
	buffer.Printf(L"%.2f", bright);
	txtBright->SetValue(buffer);
}

/* CamCalibrFrame::OnGammaCmdScroll */
void CamCalibrFrame::OnGammaCmdScroll(wxScrollEvent& event)
{
	show_gamma_and_bright();
	load_raw_file(false);
}

/*  CamCalibrFrame::get_curr_gamma */
double CamCalibrFrame::get_curr_gamma()
{
	double result = 1;
	txtGamma->GetValue().ToDouble(&result);
	return result;
}

void CamCalibrFrame::OnClearLogClick(wxCommandEvent& event)
{
	txtResult->Clear();
}

void CamCalibrFrame::OnLogLevelSelect(wxCommandEvent& event)
{
    log_level = LogLevel(chLogLevel->GetSelection());
}

void CamCalibrFrame::OnAreaSpinCoordChange(wxSpinEvent& event)
{
	if (rbResize->GetValue())
        area_resize();
    else
        area_move();
}

void CamCalibrFrame::OnAreasSelect(wxCommandEvent& event)
{
	int index = lbxAreas->GetSelection();
	if (index == -1)
	{
        spinX1->SetValue(_T("0"));
        spinX2->SetValue(_T("0"));
        spinY1->SetValue(_T("0"));
        spinY2->SetValue(_T("0"));
	}
	else
	{
        const ColorArea &area = areas_.at(index);

        area_to_spinners(area);

        if (!area_viewable(area))
        {
            centre_area_view(area);
        }
	}
}


/*
 * Methods for managing areas
 *
 * MSD: 24/5/2008
 */

// Moves currently selected area in response to the changes in the spinners
void CamCalibrFrame::area_move()
{
	int index = lbxAreas->GetSelection();
	int delta_value;

	if (index == -1) return;

    // Determines the changed spinner, and adjusts the matching spinner
    // by the amount the first spinner was changed.
    if (old_x1 != spinX1->GetValue())
    {
		delta_value = spinX1->GetValue() - old_x1;
        spinX2->SetValue(spinX2->GetValue() + delta_value);
    }
    else if (old_x2 != spinX2->GetValue())
    {
		delta_value = spinX2->GetValue() - old_x2;
        spinX1->SetValue(spinX1->GetValue() + delta_value);
    }
    else if (old_y1 != spinY1->GetValue())
    {
		delta_value = spinY1->GetValue() - old_y1;
        spinY2->SetValue(spinY2->GetValue() + delta_value);
    }
    else if (old_y2 != spinY2->GetValue())
    {
		delta_value = spinY2->GetValue() - old_y2;
        spinY1->SetValue(spinY1->GetValue() + delta_value);
    }

	ColorArea &area = areas_.at(index);
	wxRect rect = wxRect(area.left, area.top, area.right-area.left, area.bottom-area.top);

    spinners_to_area();
	show_color_areas();

	sbmpImage->RefreshRect(rect);
	for (size_t i = 0; i > areas_.size(); i++)
	{
		const ColorArea &area = areas_.at(i);
		sbmpImage->RefreshRect(wxRect(area.left, area.top, area.right-area.left, area.bottom-area.top));
	}
}

// Resizes currently selected area in response to the changes in the spinners
void CamCalibrFrame::area_resize()
{
	int index = lbxAreas->GetSelection();
	if (index == -1) return;

	ColorArea &area = areas_.at(index);
	wxRect rect = wxRect(area.left, area.top, area.right-area.left, area.bottom-area.top);

    spinners_to_area();
	show_color_areas();

	sbmpImage->RefreshRect(rect);
	for (size_t i = 0; i > areas_.size(); i++)
	{
		const ColorArea &area = areas_.at(i);
		sbmpImage->RefreshRect(wxRect(area.left, area.top, area.right-area.left, area.bottom-area.top));
	}
}

// Uses the area to set the spinners and related variables
void CamCalibrFrame::area_to_spinners(const ColorArea& current_area)
{
    spinX1->SetValue(current_area.left);
    spinX2->SetValue(current_area.right);
    spinY1->SetValue(current_area.top);
    spinY2->SetValue(current_area.bottom);

    old_x1 = current_area.left;
    old_x2 = current_area.right;
    old_y1 = current_area.top;
    old_y2 = current_area.bottom;
}

// Is the whole area currently visible?
bool CamCalibrFrame::area_viewable(const ColorArea& current_area)
{
    int start_x, start_y;
    int width, height;
    int unit_x, unit_y;
    bool result = false;

    scrwImage->GetClientSize(&width, &height);
    scrwImage->GetViewStart(&start_x, &start_y);
	scrwImage->GetScrollPixelsPerUnit(&unit_x, &unit_y);

    // Is each side of the rectanble visible?
    if (current_area.left >= (start_x * unit_x)
        && current_area.right <= ((start_x * unit_x) + width)
        && current_area.top >= (start_y * unit_y)
        && current_area.bottom <= ((start_y * unit_y) + height))
        result = true;

	return result;
}

// Scrolls the current view to centre on the area
void CamCalibrFrame::centre_area_view(const ColorArea& current_area)
{
    int width, height;
    int start_x, start_y;
    int unit_x, unit_y;
    int scroll_x, scroll_y;

    scrwImage->GetClientSize(&width, &height);

	// Set the view start coordinates (in pixels) to centre the area in the current window
    start_x = current_area.left + ((current_area.right - current_area.left) / 2) - (width / 2);
    start_y = current_area.top + ((current_area.bottom - current_area.top) / 2) - (height / 2);

	scrwImage->GetScrollPixelsPerUnit(&unit_x, &unit_y);

	// Convert pixels to scroll units
    if (start_x <= 0)
	{
        scroll_x = 0;
	}
	else
	{
        scroll_x = start_x / unit_x;
	}

    if (start_y <= 0)
	{
        scroll_y = 0;
	}
	else
	{
        scroll_y = start_y / unit_y;
	}

    scrwImage->Scroll(scroll_x, scroll_y);
}

// Resets the currently selected area with the current spinner values
void CamCalibrFrame::spinners_to_area()
{
	int index = lbxAreas->GetSelection();
	ColorArea &area = areas_.at(index);

    area.left = spinX1->GetValue();
    area.right = spinX2->GetValue();
    area.top = spinY1->GetValue();
    area.bottom = spinY2->GetValue();

    old_x1 = area.left;
    old_x2 = area.right;
    old_y1 = area.top;
    old_y2 = area.bottom;
}

// CamCalibrFrame::btnSaveClick
void CamCalibrFrame::btnSaveClick(wxCommandEvent& event)
{
	if (on_save_) on_save_(result_matrix_);
	Close();
}
