/*=============================================================================
  DNG4PS - software for converting Canon PS RAW files to DNG ones
  Copyright (C) 2007-2008 Denis Artyomov

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
  File:      OptionsDialog.cpp
  Author(s): Denis Artyomov (denis.artyomov@gmail.com)
             Matt Dawson (coder@dawsons.id.au)
  ===========================================================================*/

#include "pch.h"

#include <wx/msgdlg.h>
#include <wx/fileconf.h>
#include <wx/wfstream.h>
#include <wx/stdpaths.h>
#include <wx/wfstream.h>
#include <wx/protocol/http.h>
#include <wx/settings.h>
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/notebook.h>

#include "OptionsDialog.h"
#include "Options.h"
#include "ProgramObjects.h"
#include "CameraOpts.h"
#include "CamCalibrFrame.h"

#include "dng_tag_values.h"


#include "lib/wxGUIBuilder.hpp"

namespace
{
	static const char* langs[] = { "zh", "en", "de", "it", "fi", "no", "pl", "ru", "es", "fr", nullptr };

	MosaicType mosaic_types[] = {MOSAIC_RGGB, MOSAIC_GBRG};

	struct IllCorr {int value; const wchar_t * text;};

	static const IllCorr ill_corr[] =
	{
		{ -1,                     L""                       },
		{ lsDaylight,             L"Daylight"               },
		{ lsFluorescent,          L"Fluorescent"            },
		{ lsTungsten,             L"Tungsten"               },
		{ lsFlash,                L"Flash"                  },
		{ lsFineWeather,          L"Fine Weather"           },
		{ lsCloudyWeather,        L"Cloudy Weather"         },
		{ lsShade,                L"Shade"                  },
		{ lsDaylightFluorescent,  L"Daylight Fluorescent"   },
		{ lsDayWhiteFluorescent,  L"Day White Fluorescent"  },
		{ lsCoolWhiteFluorescent, L"Cool White Fluorescent" },
		{ lsWhiteFluorescent,     L"White Fluorescent"      },
		{ lsStandardLightA,       L"Standard Light A"       },
		{ lsStandardLightB,       L"Standard Light B"       },
		{ lsStandardLightC,       L"Standard Light C"       },
		{ lsD55,                  L"D55"                    },
		{ lsD65,                  L"D65"                    },
		{ lsD75,                  L"D75"                    },
		{ lsD50,                  L"D50"                    },
		{ lsISOStudioTungsten,    L"ISO Studio Tungsten"    },
		{ 0, NULL }
	};
}

OptionsDialog::OptionsDialog(wxWindow* parent,wxWindowID id) : cam_opts_(new CameraOpts)
{
	wxButton* btnGetLastest = nullptr;
	wxScrolledWindow* scrlwCameraOpt = nullptr;
	wxButton* btnResetToDefaults = nullptr;
	wxButton* btnDelete = nullptr;
	wxButton* btnCalibr = nullptr;
	wxButton* btnCopy = nullptr;
	
	Create(parent, wxID_ANY, _("optsDialogCaption"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));

	using namespace gb;

	auto name = [] (const char* name_text) { return text(name_text, align_right); };

	auto chDateType_GUI = choice() [
		item("optsDateFormat1"),
		item("optsDateFormat2"),
		item("optsDateFormat3"),
		item("optsDateFormat4")
	];

	auto chLanguage_GUI = choice() [
		item("Chinese"),
		item("English"),
		item("German"),
		item("Italian"),
		item("Finnish"),
		item("Norwegian"),
		item("Polish"),
		item("Russian"),
		item("Spanish"),
		item("French")
	];

	auto chPreview_GUI = choice() [
		item("optsPreviewNone"), 
		item("optsPreviewMedium"), 
		item("optsPreviewBig")
	];

	auto main_page = page("pgMain")
	[
		vbox()
		[
			text("optsMainLabel", font_bold),
			vbox(border(8) | bord_all_exc_top | expand)
			[
				check_box("optsIncludeSubfolders") >> chbxIncludeSubfilders,
				check_box("optsDontOverwrite") >> chbxDontOverwrite,
				grid(2, 0, border(0) | expand, growable_cols(1))
				[
					check_box("optsRegulating") >> chbxUseDateForPath,
					chDateType_GUI >> chDateType,
					text("optsLanguageLabel"),
					chLanguage_GUI >> chLanguage
				]
			],
			text("optsDngFormatLabel", font_bold),
			vbox(border(8) | bord_all_exc_top | expand)
			[
				check_box("optsCompressDng") >> chbxCompressDng,
				check_box("optsAddMetadata") >> chbxAddMetadata,
				spacer(5),
				grid(2, 0, border(0) | expand, growable_cols(1))
				[
					text("optsPreviewLabel"),
					chPreview_GUI >> chPreview,
					check_box("optsArtistChk") >> chkbArtist,
					edit(expand) >> txtArtist
				]
			]
		]
	];

	auto cameras_types_page = page("optsCameraTypesLabel") 
	[
		text("lblGroupNotice", width(150)) >> lblGroupNotice,
		grid(2, 0, expand, growable_cols(1)) >> szGroups
	] >> pnlGroups;

	auto camera_option_page = page("pgCamOpts")
	[
		vbox(expand | stretch)
		[
			text("optsCamera", font_bold | bord_all_exc_bottom), // cameras work
			choice(expand) >> chCameraSelector,
			hbox(border(0) | expand)
			[
				button("btnGetLastest") >> btnGetLastest,
				spring(),
				button("btnCopy") >> btnCopy,
				button("btnDelete") >> btnDelete
			],
			hline(),
			scroll_box(expand | stretch | height(190)) // selected camera options
			[
				grid(2, 0, expand | stretch | border(0), growable_cols(1))
				[
					name("lblCameraName"),
					edit(expand) >> txtCamName,
					name("lblShortName"), 
					edit(expand) >> txtShortName,
					text("lblGeometry", font_bold), // geometry
					spacer(),
					name("lblRawSize"),
					hbox(border(0)) 
					[
						edit(width(30)) >> txtSensWidth,
						text("x"),
						edit(width(30)) >> txtSensHeight,
						text("000000000000000") >> lblFileSize
					],
					name("lblJpegSize"),
					hbox(border(0))
					[
						edit(width(30)) >> txtJpegWidth,
						text("x"),
						edit(width(30)) >> txtJpegHeight
					],
					name("lblActiveX1Y1"),
					hbox(border(0))
					[
						edit(width(30)) >> txtActiveX1,
						text(","),
						edit(width(30)) >> txtActiveY1
					],
					name("lblActiveX2Y2"),
					hbox(border(0))
					[
						edit(width(30)) >> txtActiveX2,
						text(","),
						edit(width(30)) >> txtActiveY2
					],
					name("lblBlackWhileLevels"),
					hbox(border(0))
					[
						edit(width(30)) >> txtBlackLevel,
						text("-"),
						edit(width(30)) >> txtWhiteLevel
					],
					name("lblMosaicType"),
					choice()
					[
						item("Red,Green Green,Blue"),
						item("Green,Blue Red,Green")
					] >> chMosaicType,
					name("lblBitsPerUnit"),
					choice() 
					[
						item("10 bit"),
						item("12 bit")
					] >> chBitsPerUnit,
					text("lblColorMatrix1", font_bold), // Color matrix 1
					spacer(),
					name("lblMatrIll"),
					choice() >> chIll1,
					name("lblMatr"),
					edit(expand) >> txtMatr1,
					spacer(),
					button("btnCalibr", bord_all_exc_top) >> btnCalibr,
					name("lblMatrixMult"),
					edit() >> txtMatrix1Mult,
					text("lblColorMatrix2", font_bold),
					spacer(),
					name("lblMatrIll"),
					choice() >> chIll2,
					name("lblMatr"),
					edit(expand) >> txtMatr2,
					name("lblMatrixMult"),
					edit(expand) >> txtMatrix2Mult,
					spacer(),
					button("btnResetToDefaults") >> btnResetToDefaults
				]
			] >> scrlwCameraOpt
		]
	];

	auto all_gui = existing_window(this) 
	[
		notebook(stretch | expand)
		[
			main_page,
			cameras_types_page,
			camera_option_page
		] >> nbMain,
		dlg_buttons_ok_cancel()
	];

	all_gui.build();

	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
	Center();

	szGroupsMain = pnlGroups->GetSizer();

	// Connecting events
	chbxUseDateForPath->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED,       [this] (wxCommandEvent&)        { correct_interface();            });
	chkbArtist        ->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED,       [this] (wxCommandEvent&)        { correct_interface();            });
	chCameraSelector  ->Bind(wxEVT_COMMAND_CHOICE_SELECTED,        [this] (wxCommandEvent& event)  { chCameraSelectorSelect(event);  });
	btnGetLastest     ->Bind(wxEVT_COMMAND_BUTTON_CLICKED,         [this] (wxCommandEvent& event)  { OnGetLastestClick(event);       });
	btnCopy           ->Bind(wxEVT_COMMAND_BUTTON_CLICKED,         [this] (wxCommandEvent& event)  { btnCopyClick(event);            });
	btnDelete         ->Bind(wxEVT_COMMAND_BUTTON_CLICKED,         [this] (wxCommandEvent& event)  { btnDeleteClick(event);          });
	btnResetToDefaults->Bind(wxEVT_COMMAND_BUTTON_CLICKED,         [this] (wxCommandEvent& event)  { btnResetToDefaultsClick(event); });
	txtSensWidth      ->Bind(wxEVT_COMMAND_TEXT_UPDATED,           [this] (wxCommandEvent& event)  { txtSensWidthText(event);        });
	txtSensHeight     ->Bind(wxEVT_COMMAND_TEXT_UPDATED,           [this] (wxCommandEvent& event)  { txtSensWidthText(event);        });
	btnCalibr         ->Bind(wxEVT_COMMAND_BUTTON_CLICKED,         [this] (wxCommandEvent& event)  { btnCalibrClick(event);          });
	nbMain            ->Bind(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,  [this] (wxNotebookEvent& event) { nbMainPageChanged(event);       });
	nbMain            ->Bind(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, [this] (wxNotebookEvent& event) { nbMainPageChanging(event);      });

	scrlwCameraOpt->SetScrollRate(16, 16);

	fill_illum_strings(chIll1);
	fill_illum_strings(chIll2);
}

// OptionsDialog::execute
bool OptionsDialog::execute()
{
    ProgramObjects &sys = ::sys();

    *cam_opts_ = *sys.cameras;

    chbxIncludeSubfilders->SetValue(sys.options->recursive_search);
    chbxDontOverwrite->SetValue(sys.options->dont_overwrite);
    chbxAddMetadata->SetValue(sys.options->add_metadata);
    chbxCompressDng->SetValue(sys.options->compress_dng);
    chPreview->SetSelection(sys.options->preview_type);

    chbxUseDateForPath->SetValue(sys.options->use_date_for_path);
    chDateType->SetSelection(sys.options->date_type);
    chkbArtist->SetValue(sys.options->use_artist);
    txtArtist->SetValue(sys.options->artist);

    for (int i = 0; langs[i]; i++)
		if (sys.options->lang == langs[i]) chLanguage->SetSelection(i);

	show_cameras_list(sys.options->last_camera_id);

    correct_interface();

    ShowModal();

    if (GetReturnCode() != wxID_OK) return false;

    sys.options->recursive_search   = chbxIncludeSubfilders->GetValue();
    sys.options->dont_overwrite     = chbxDontOverwrite->GetValue();
    sys.options->add_metadata       = chbxAddMetadata->GetValue();
    sys.options->compress_dng       = chbxCompressDng->GetValue();
    sys.options->preview_type       = (PreviewType)chPreview->GetSelection();
    sys.options->use_date_for_path  = chbxUseDateForPath->GetValue();
    sys.options->date_type          = (DateTypes)chDateType->GetSelection();
    sys.options->use_artist         = chkbArtist->GetValue();
    sys.options->artist             = txtArtist->GetValue();

    int lang = chLanguage->GetSelection();
    if (lang == -1) lang = 0;
    sys.options->lang = langs[lang];

    read_camera_opts();
    read_groups();

	const CameraData* cur_camera = get_selected_camera_id();
    if (cur_camera != NULL) sys.options->last_camera_id = cur_camera->id;

    *sys.cameras = *cam_opts_;

    sys.options->save();
    sys.cameras->save();

    return true;
}

// OptionsDialog::correct_interface
void OptionsDialog::correct_interface()
{
    chDateType->Enable(chbxUseDateForPath->GetValue());
    txtArtist->Enable(chkbArtist->GetValue());
}


///////////////////////////////////////////////
//                                           //
//        C A M E R A   O P T I O N S        //
//                                           //
///////////////////////////////////////////////

namespace
{
	const BitsPerUnit bits_per_unit[] = {ppc_10, ppc_12};
}

void OptionsDialog::show_cameras_list(const wxString &camera_to_select)
{
	int selection = wxNOT_FOUND;
	chCameraSelector->Clear();
	for (size_t i = 0; i < cam_opts_->get_size(); i++)
	{
		const CameraData &item = cam_opts_->at(i);
		chCameraSelector->Append(item.short_name, new wxStringClientData(item.id));
		if (item.id == camera_to_select) selection = i;
	}
	chCameraSelector->SetSelection(selection);
	show_camera_opts();
}

void OptionsDialog::chCameraSelectorSelect(wxCommandEvent& event)
{
	read_camera_opts();
	show_camera_opts();
}

void OptionsDialog::show_illum_type(wxChoice *coice, int value)
{
	for (int i = 0; ill_corr[i].text; i++) if (ill_corr[i].value == value) coice->SetSelection(i);
}

void OptionsDialog::fill_illum_strings(wxChoice *coice)
{
	coice->Clear();
	for (int i = 0; ill_corr[i].text; i++) coice->Append(ill_corr[i].text);
}

const CameraData* OptionsDialog::get_selected_camera_id()
{
	const wxStringClientData *data =
		(chCameraSelector->GetCount() == 0)
		? NULL
		: dynamic_cast<wxStringClientData*>(chCameraSelector->GetClientObject(chCameraSelector->GetSelection()));

	return (data != NULL) ? cam_opts_->find_by_id(data->GetData()) : NULL;
}

void OptionsDialog::show_camera_opts()
{
	const CameraData *item = get_selected_camera_id();

	if (item != NULL) lastCameraId = item->id;

	wxString empty;
	txtCamName->SetValue(item != NULL ? item->model_name : empty);
	txtShortName->SetValue(item != NULL ? item->short_name : empty);
	txtSensWidth->SetValue(item != NULL ? wxString::Format(L"%d", (int)item->width) : empty);
	txtSensHeight->SetValue(item != NULL ? wxString::Format(L"%d", (int)item->height) : empty);
	txtJpegWidth->SetValue(item != NULL ? wxString::Format(L"%d", (int)item->cropped_width) : empty);
	txtJpegHeight->SetValue(item != NULL ? wxString::Format(L"%d", (int)item->cropped_height) : empty);
	txtActiveX1->SetValue(item != NULL ? wxString::Format(L"%d", (int)item->active_origin_x) : empty);
	txtActiveY1->SetValue(item != NULL ? wxString::Format(L"%d", (int)item->active_origin_y) : empty);
	txtActiveX2->SetValue(item != NULL ? wxString::Format(L"%d", (int)(item->width-item->active_origin_right)) : empty);
	txtActiveY2->SetValue(item != NULL ? wxString::Format(L"%d", (int)(item->height-item->active_origin_bottom)) : empty);
	txtBlackLevel->SetValue(item != NULL ? wxString::Format(L"%d", (int)item->black_level) : empty);
	txtWhiteLevel->SetValue(item != NULL ? wxString::Format(L"%d", (int)item->white_level) : empty);

	int mosaic = wxNOT_FOUND;
	if (item != NULL)
	{
		static const size_t mosaic_types_len = sizeof(mosaic_types)/sizeof(MosaicType);
		const MosaicType *finded = std::find(mosaic_types, mosaic_types+mosaic_types_len, item->mosaic);
		mosaic = (finded == mosaic_types+mosaic_types_len) ? wxNOT_FOUND : (finded-mosaic_types);
	}
	chMosaicType->SetSelection(mosaic);

	if (item != NULL)
	{
		size_t bits_per_unit_size = sizeof(bits_per_unit)/sizeof(bits_per_unit[0]);
		const BitsPerUnit *bpu = std::find(bits_per_unit, bits_per_unit+bits_per_unit_size, item->bits_per_unit);
		if (bpu == bits_per_unit+bits_per_unit_size) chBitsPerUnit->SetSelection(0);
		else chBitsPerUnit->SetSelection(bpu-bits_per_unit);
	}
	else chBitsPerUnit->SetSelection(wxNOT_FOUND);

	show_illum_type(chIll1, item != NULL ? item->matrix1.illum : -1);
	txtMatr1->SetValue(item != NULL ? CameraOpts::vct_to_str(item->matrix1.matrix, L"%0.6f ") : empty);
	txtMatrix1Mult->SetValue(item != NULL ? CameraOpts::vct_to_str(item->matrix1.mult, L"%0.1f ") : empty);

	show_illum_type(chIll2, item != NULL ? item->matrix2.illum : -1);
	txtMatr2->SetValue(item != NULL ? CameraOpts::vct_to_str(item->matrix2.matrix, L"%0.6f ") : empty);
	txtMatrix2Mult->SetValue(item != NULL ? CameraOpts::vct_to_str(item->matrix2.mult, L"%0.1f ") : empty);
}

int OptionsDialog::get_int(wxTextCtrl *txt)
{
	long value = -1;
	txt->GetValue().ToLong(&value);
	return (int)value;
}

int OptionsDialog::get_illum_type(wxChoice *coice)
{
	int result = coice->GetSelection();
	if (result == wxNOT_FOUND) return -1;
	return ill_corr[result].value;
}

void OptionsDialog::read_camera_opts()
{
	if (lastCameraId.IsEmpty()) return;

	CameraData item;

	cam_opts_->reset_to_defaults(lastCameraId, item);

	item.model_name = txtCamName->GetValue();
	item.short_name = txtShortName->GetValue();

	item.width = get_int(txtSensWidth);
	item.height = get_int(txtSensHeight);

	item.cropped_width = get_int(txtJpegWidth);
	item.cropped_height = get_int(txtJpegHeight);
	item.active_origin_x = get_int(txtActiveX1);
	item.active_origin_y = get_int(txtActiveY1);

	item.active_origin_right = item.width - get_int(txtActiveX2);
	item.active_origin_bottom = item.height - get_int(txtActiveY2);

	item.black_level = get_int(txtBlackLevel);
	item.white_level = get_int(txtWhiteLevel);

	item.mosaic = (chMosaicType->GetSelection() == wxNOT_FOUND) ? MOSAIC_RGGB : mosaic_types[chMosaicType->GetSelection()];

	item.bits_per_unit = (chBitsPerUnit->GetSelection() == wxNOT_FOUND) ? ppc_10 : bits_per_unit[chBitsPerUnit->GetSelection()];

	item.matrix1.illum = get_illum_type(chIll1);
	CameraOpts::str_to_vect(item.matrix1.matrix, txtMatr1->GetValue(), 9);
	CameraOpts::str_to_vect(item.matrix1.mult, txtMatrix1Mult->GetValue(), 3);

	item.matrix2.illum = get_illum_type(chIll2);
	CameraOpts::str_to_vect(item.matrix2.matrix, txtMatr2->GetValue(), 9);
	CameraOpts::str_to_vect(item.matrix2.mult, txtMatrix2Mult->GetValue(), 3);

	cam_opts_->set_item(lastCameraId, item);
}

void OptionsDialog::txtSensWidthText(wxCommandEvent& event)
{
	const CameraData *camera = cam_opts_->find_by_id(lastCameraId);
	if (camera == NULL) return;
	int width = get_int(txtSensWidth);
	int height = get_int(txtSensHeight);
	int size = -1;
	bool ok = true;
	if ((width != -1) && (height != -1))
	{
		int size_in_bits = (width*height*(int)camera->bits_per_unit);
		size = size_in_bits/8;
		ok = ((size_in_bits%8) == 0);
	}

	lblFileSize->SetLabel(size > 0 ? wxString::Format(L"(%i)", size) : wxString());
	lblFileSize->SetForegroundColour(ok ? wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT) : *wxRED);
}

void OptionsDialog::btnCopyClick(wxCommandEvent& event)
{
	const CameraData *item = get_selected_camera_id();
	if (item == NULL) return;

	read_camera_opts();

	wxMessageDialog *dialog = new wxMessageDialog(
		this,
		wxString::Format(_("dlgCopyCameraText"), item->model_name.c_str()),
		_("dlgCopyCameraCap"),
		wxYES_NO|wxNO_DEFAULT
	);

	int result = dialog->ShowModal();
	if (result != wxID_YES) return;

	wxString new_id;
	CameraData new_item = *item;
	new_item.model_name = wxString::Format(_("copyOfCameraProf"), item->model_name.c_str());
	new_item.short_name = wxString::Format(_("copyOfCameraProf"), item->short_name.c_str());
	new_item.id = L"";
	cam_opts_->add(new_item, &new_id);
	show_cameras_list(new_id);
}

void OptionsDialog::btnDeleteClick(wxCommandEvent& event)
{
	const CameraData *item = get_selected_camera_id();
	if (item == NULL) return;

	wxMessageDialog *dialog = new wxMessageDialog(
		this,
		wxString::Format(_("dlgDeleteCameraText"), item->model_name.c_str()),
		_("dlgDeleteCameraCap"),
		wxYES_NO|wxNO_DEFAULT
	);

	int result = dialog->ShowModal();
	if (result != wxID_YES) return;

	cam_opts_->erase(item->id);
	item = NULL;

	show_cameras_list(wxString());
}

void OptionsDialog::btnCalibrClick(wxCommandEvent& event)
{
	const CameraData *item = get_selected_camera_id();
	if (item == NULL) return;

	CamCalibrFrame *frame = new CamCalibrFrame(*item, this);
	frame->on_save_ = std::bind(&OptionsDialog::matrix_calculated, this, _1);
	frame->Show();
}

void OptionsDialog::matrix_calculated(const std::vector<float> &matrix)
{
	txtMatr1->SetValue(CameraOpts::vct_to_str(matrix, L"%0.6f "));
}

void OptionsDialog::btnResetToDefaultsClick(wxCommandEvent& event)
{
	const CameraData *item = get_selected_camera_id();
	if (item == NULL) return;

	wxMessageDialog *dialog = new wxMessageDialog(
		this,
		wxString::Format(_("dlgDefaultCameraText"), item->model_name.c_str()),
		_("dlgDefaultCameraCap"),
		wxYES_NO|wxNO_DEFAULT
	);

	int result = dialog->ShowModal();
	if (result != wxID_YES) return;

	cam_opts_->reset_to_defaults(item->id);
	show_camera_opts();

	dialog->Destroy();
}


///////////////////////////////////////////////
//                                           //
//        C A M E R A S   G R O U P S        //
//                                           //
///////////////////////////////////////////////

// TODO: Show item only if there are several cameras for same sensor size
void OptionsDialog::show_groups()
{
	Freeze();

	szGroups->Clear(true);
	group_controls_.clear();

	struct Tmp
	{
		unsigned width;
		unsigned height;
		unsigned count;

		Tmp() : width(0), height(0), count(0) {}
	};

	typedef std::map<unsigned int, Tmp> FileSizes;
	FileSizes file_sizes;
	const size_t count = cam_opts_->get_size();
	for (size_t i = 0; i < count; i++)
	{
		const CameraData &item = cam_opts_->at(i);
		unsigned int file_size = CameraOpts::get_file_size(item);
		auto existing = file_sizes.find(file_size);
		if (existing == file_sizes.end())
		{
			file_sizes[file_size].width = item.width;
			file_sizes[file_size].height = item.height;
			file_sizes[file_size].count = 1;
		}
		else existing->second.count++;
	}

	for (auto &size_item : file_sizes)
	{
		if (size_item.second.count == 1) continue;

		wxString label = wxString::Format(L"%i x %i:", size_item.second.width, size_item.second.height);
		wxStaticText *text = new wxStaticText(pnlGroups, wxNewId(), label, wxDefaultPosition, wxDefaultSize);
		szGroups->Add(text, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(pnlGroups,wxSize(2,0)).GetWidth());

		wxChoice *choice = new wxChoice(pnlGroups, wxNewId(), wxDefaultPosition, wxDefaultSize);
		szGroups->Add(choice, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(pnlGroups,wxSize(2,0)).GetWidth());

		wxString cur_croup_id = cam_opts_->get_group(size_item.first);

		int selection = wxNOT_FOUND;
		for (size_t i = 0; i < count; i++)
		{
			const CameraData &item = cam_opts_->at(i);
			unsigned int file_size = CameraOpts::get_file_size(item);
			if (file_size == size_item.first)
			{
				if (cur_croup_id == item.id) selection = choice->GetCount();
				choice->Append(item.short_name, new wxStringClientData(item.id));
			}
		}

		choice->SetSelection(selection);

		group_controls_[size_item.first] = choice;
	}

	lblGroupNotice->Wrap(lblGroupNotice->GetSize().GetWidth());
	szGroupsMain->Layout();

	Thaw();
}

void OptionsDialog::read_groups()
{
	if (group_controls_.empty()) return;

	cam_opts_->clear_groups();

	for (auto &item : group_controls_)
	{
		wxChoice *choice = item.second;
		if (choice->GetCount() == 0) continue;
		int selection = choice->GetSelection();
		if (selection == wxNOT_FOUND) continue;
		wxStringClientData *cli_data = dynamic_cast<wxStringClientData*>(choice->GetClientObject(selection));
		cam_opts_->add_group(item.first, cli_data->GetData());
	}
}

void OptionsDialog::nbMainPageChanged(wxNotebookEvent& event)
{
	if (nbMain->GetPage(event.GetSelection()) == pnlGroups)
	{
		read_camera_opts();
		show_groups();
	}
}

void OptionsDialog::nbMainPageChanging(wxNotebookEvent& event)
{
	if (nbMain->GetPage(event.GetSelection()) == pnlGroups) read_groups();
}

void OptionsDialog::OnGetLastestClick(wxCommandEvent& event)
{
	wxHTTP get;
	get.SetHeader(_T("Content-type"), _T("text/html; charset=utf-8"));
	get.SetTimeout(30);
	 
	get.Connect(_T("dng4ps2.googlecode.com"));
	 
	wxApp::IsMainLoopRunning();
	 
	wxInputStream *httpStream = get.GetInputStream(_T("/svn/trunk/src/cameras.txt"));
	 
	if (get.GetError() == wxPROTO_NOERR)
	{
		wxFileName file_name;
		file_name.SetPath(wxStandardPaths::Get().GetUserDataDir());
		wxFileName::Mkdir(file_name.GetFullPath(), 0777, wxPATH_MKDIR_FULL);
		file_name.SetName(L"last_cameras.txt");

		wxFileOutputStream out_stream(file_name.GetFullPath());
		httpStream->Read(out_stream);
		out_stream.Close();

		{	wxFFileInputStream in_stream(file_name.GetFullPath());
			wxFileConfig fc(in_stream);
			cam_opts_->load(fc); }

		wxMessageBox(_T("Success!"));

		show_cameras_list(sys().options->last_camera_id);
		correct_interface();
	}
	else
	{
		wxMessageBox(_T("Unable to connect!"));
	}
	 
	wxDELETE(httpStream);
	get.Close();
}
