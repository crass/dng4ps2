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

#include "OptionsDialog.h"
#include "Options.h"
#include "ProgramObjects.h"
#include "CameraOpts.h"
#include "CamCalibrFrame.h"

#include "dng_tag_values.h"

//(*InternalHeaders(OptionsDialog)
#include <wx/settings.h>
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

namespace
{
	static const wxLanguage langs[] =
	{
		wxLANGUAGE_CHINESE,
		wxLANGUAGE_ENGLISH,
		wxLANGUAGE_GERMAN,
		wxLANGUAGE_ITALIAN,
		wxLANGUAGE_FINNISH,
		wxLANGUAGE_NORWEGIAN_NYNORSK,
		wxLANGUAGE_POLISH,
		wxLANGUAGE_RUSSIAN,
		wxLANGUAGE_SPANISH,
		wxLANGUAGE_FRENCH,
		wxLANGUAGE_DEFAULT
	};

	MosaicType mosaic_types[] = {MOSAIC_TYPE1, MOSAIC_TYPE2};

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


//(*IdInit(OptionsDialog)
const long OptionsDialog::ID_STATICTEXT1 = wxNewId();
const long OptionsDialog::ID_CHECKBOX1 = wxNewId();
const long OptionsDialog::ID_CHECKBOX2 = wxNewId();
const long OptionsDialog::ID_STATICTEXT2 = wxNewId();
const long OptionsDialog::ID_CHECKBOX3 = wxNewId();
const long OptionsDialog::ID_CHECKBOX4 = wxNewId();
const long OptionsDialog::ID_STATICTEXT3 = wxNewId();
const long OptionsDialog::ID_CHOICE1 = wxNewId();
const long OptionsDialog::ID_CHECKBOX5 = wxNewId();
const long OptionsDialog::ID_CHOICE2 = wxNewId();
const long OptionsDialog::ID_CHECKBOX6 = wxNewId();
const long OptionsDialog::ID_TEXTCTRL1 = wxNewId();
const long OptionsDialog::ID_STATICTEXT5 = wxNewId();
const long OptionsDialog::ID_STATICTEXT6 = wxNewId();
const long OptionsDialog::ID_CHOICE3 = wxNewId();
const long OptionsDialog::ID_PANEL1 = wxNewId();
const long OptionsDialog::ID_STATICTEXT4 = wxNewId();
const long OptionsDialog::ID_PANEL2 = wxNewId();
const long OptionsDialog::ID_STATICTEXT12 = wxNewId();
const long OptionsDialog::ID_CHOICE9 = wxNewId();
const long OptionsDialog::ID_BUTTON1 = wxNewId();
const long OptionsDialog::ID_PANEL3 = wxNewId();
const long OptionsDialog::ID_BUTTON3 = wxNewId();
const long OptionsDialog::ID_BUTTON2 = wxNewId();
const long OptionsDialog::ID_BUTTON5 = wxNewId();
const long OptionsDialog::ID_STATICTEXT13 = wxNewId();
const long OptionsDialog::ID_TEXTCTRL2 = wxNewId();
const long OptionsDialog::ID_STATICTEXT8 = wxNewId();
const long OptionsDialog::ID_TEXTCTRL5 = wxNewId();
const long OptionsDialog::ID_STATICTEXT30 = wxNewId();
const long OptionsDialog::ID_STATICTEXT14 = wxNewId();
const long OptionsDialog::ID_TEXTCTRL3 = wxNewId();
const long OptionsDialog::ID_STATICTEXT15 = wxNewId();
const long OptionsDialog::ID_TEXTCTRL4 = wxNewId();
const long OptionsDialog::ID_STATICTEXT7 = wxNewId();
const long OptionsDialog::ID_STATICTEXT17 = wxNewId();
const long OptionsDialog::ID_TEXTCTRL6 = wxNewId();
const long OptionsDialog::ID_STATICTEXT18 = wxNewId();
const long OptionsDialog::ID_TEXTCTRL7 = wxNewId();
const long OptionsDialog::ID_STATICTEXT19 = wxNewId();
const long OptionsDialog::ID_TEXTCTRL8 = wxNewId();
const long OptionsDialog::ID_STATICTEXT20 = wxNewId();
const long OptionsDialog::ID_TEXTCTRL9 = wxNewId();
const long OptionsDialog::ID_STATICTEXT21 = wxNewId();
const long OptionsDialog::ID_TEXTCTRL10 = wxNewId();
const long OptionsDialog::ID_STATICTEXT22 = wxNewId();
const long OptionsDialog::ID_TEXTCTRL11 = wxNewId();
const long OptionsDialog::ID_STATICTEXT23 = wxNewId();
const long OptionsDialog::ID_TEXTCTRL12 = wxNewId();
const long OptionsDialog::ID_STATICTEXT24 = wxNewId();
const long OptionsDialog::ID_TEXTCTRL13 = wxNewId();
const long OptionsDialog::ID_STATICTEXT25 = wxNewId();
const long OptionsDialog::ID_CHOICE10 = wxNewId();
const long OptionsDialog::ID_STATICTEXT31 = wxNewId();
const long OptionsDialog::ID_STATICTEXT26 = wxNewId();
const long OptionsDialog::ID_CHOICE11 = wxNewId();
const long OptionsDialog::ID_STATICTEXT27 = wxNewId();
const long OptionsDialog::ID_TEXTCTRL15 = wxNewId();
const long OptionsDialog::ID_BUTTON4 = wxNewId();
const long OptionsDialog::ID_STATICTEXT28 = wxNewId();
const long OptionsDialog::ID_TEXTCTRL16 = wxNewId();
const long OptionsDialog::ID_STATICTEXT32 = wxNewId();
const long OptionsDialog::ID_STATICTEXT29 = wxNewId();
const long OptionsDialog::ID_CHOICE12 = wxNewId();
const long OptionsDialog::ID_STATICTEXT33 = wxNewId();
const long OptionsDialog::ID_TEXTCTRL18 = wxNewId();
const long OptionsDialog::ID_STATICTEXT34 = wxNewId();
const long OptionsDialog::ID_TEXTCTRL19 = wxNewId();
const long OptionsDialog::ID_SCROLLEDWINDOW1 = wxNewId();
const long OptionsDialog::ID_PNL_CAM_OPTS = wxNewId();
const long OptionsDialog::ID_NOTEBOOK1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(OptionsDialog,wxDialog)
	//(*EventTable(OptionsDialog)
	//*)
END_EVENT_TABLE()

OptionsDialog::OptionsDialog(wxWindow* parent,wxWindowID id) : cam_opts_(new CameraOpts)
{
	//(*Initialize(OptionsDialog)
	wxStaticText* StaticText22;
	wxButton* btnGetLastest;
	wxStaticText* StaticText20;
	wxFlexGridSizer* FlexGridSizer4;
	wxFlexGridSizer* szOptions;
	wxFlexGridSizer* FlexGridSizer16;
	wxScrolledWindow* scrlwCameraOpt;
	wxStaticText* StaticText29;
	wxStaticText* StaticText33;
	wxStaticText* StaticText13;
	wxStaticText* StaticText2;
	wxStaticText* StaticText14;
	wxStaticText* StaticText30;
	wxFlexGridSizer* FlexGridSizer10;
	wxFlexGridSizer* FlexGridSizer3;
	wxStaticText* StaticText26;
	wxStaticText* StaticText6;
	wxButton* btnResetToDefaults;
	wxFlexGridSizer* FlexGridSizer5;
	wxStaticText* StaticText32;
	wxStaticText* StaticText19;
	wxButton* btnDelete;
	wxStaticText* StaticText18;
	wxFlexGridSizer* FlexGridSizer9;
	wxStaticText* StaticText31;
	wxPanel* Panel1;
	wxFlexGridSizer* FlexGridSizer2;
	wxStaticText* StaticText1;
	wxStaticText* StaticText27;
	wxStaticText* StaticText3;
	wxPanel* Panel3;
	wxStaticText* StaticText21;
	wxStaticText* StaticText23;
	wxStaticText* StaticText24;
	wxStaticText* StaticText34;
	wxStaticText* StaticText5;
	wxFlexGridSizer* FlexGridSizer15;
	wxFlexGridSizer* FlexGridSizer8;
	wxFlexGridSizer* FlexGridSizer14;
	wxButton* btnCalibr;
	wxStaticText* StaticText28;
	wxStaticText* StaticText15;
	wxFlexGridSizer* FlexGridSizer13;
	wxStaticText* StaticText12;
	wxFlexGridSizer* FlexGridSizer12;
	wxFlexGridSizer* FlexGridSizer6;
	wxNotebook* nbMain;
	wxFlexGridSizer* FlexGridSizer1;
	wxStaticText* StaticText25;
	wxFlexGridSizer* FlexGridSizer11;
	wxStaticText* StaticText17;
	wxStaticText* StaticText4;
	wxButton* btnCopy;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;

	Create(parent, wxID_ANY, _("optsDialogCaption"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	szOptions = new wxFlexGridSizer(0, 1, 0, 0);
	szOptions->AddGrowableCol(0);
	nbMain = new wxNotebook(this, ID_NOTEBOOK1, wxDefaultPosition, wxDefaultSize, 0, _T("ID_NOTEBOOK1"));
	Panel1 = new wxPanel(nbMain, ID_PANEL1, wxPoint(20,48), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, _("optsMainLabel"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	wxFont StaticText1Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	if ( !StaticText1Font.Ok() ) StaticText1Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	StaticText1Font.SetPointSize((int)(StaticText1Font.GetPointSize() * 1.000000));
	StaticText1Font.SetWeight(wxBOLD);
	StaticText1->SetFont(StaticText1Font);
	FlexGridSizer1->Add(StaticText1, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel1,wxSize(5,0)).GetWidth());
	FlexGridSizer4 = new wxFlexGridSizer(0, 1, 0, 0);
	chbxIncludeSubfilders = new wxCheckBox(Panel1, ID_CHECKBOX1, _("optsIncludeSubfolders"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	chbxIncludeSubfilders->SetValue(false);
	chbxIncludeSubfilders->SetToolTip(_("optsIncludeSubfolders_Hint"));
	FlexGridSizer4->Add(chbxIncludeSubfilders, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel1,wxSize(5,0)).GetWidth());
	chbxDontOverwrite = new wxCheckBox(Panel1, ID_CHECKBOX2, _("optsDontOverwrite"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
	chbxDontOverwrite->SetValue(false);
	chbxDontOverwrite->SetToolTip(_("optsDontOverwrite_Hint"));
	FlexGridSizer4->Add(chbxDontOverwrite, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel1,wxSize(5,0)).GetWidth());
	FlexGridSizer1->Add(FlexGridSizer4, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel1,wxSize(5,0)).GetWidth());
	StaticText2 = new wxStaticText(Panel1, ID_STATICTEXT2, _("optsDngFormatLabel"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	wxFont StaticText2Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	if ( !StaticText2Font.Ok() ) StaticText2Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	StaticText2Font.SetPointSize((int)(StaticText2Font.GetPointSize() * 1.000000));
	StaticText2Font.SetWeight(wxBOLD);
	StaticText2->SetFont(StaticText2Font);
	FlexGridSizer1->Add(StaticText2, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel1,wxSize(5,0)).GetWidth());
	FlexGridSizer3 = new wxFlexGridSizer(0, 1, 0, 0);
	chbxCompressDng = new wxCheckBox(Panel1, ID_CHECKBOX3, _("optsCompressDng"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
	chbxCompressDng->SetValue(false);
	chbxCompressDng->SetToolTip(_("optsCompressDng_Hint"));
	FlexGridSizer3->Add(chbxCompressDng, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel1,wxSize(5,0)).GetWidth());
	chbxAddMetadata = new wxCheckBox(Panel1, ID_CHECKBOX4, _("optsAddMetadata"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX4"));
	chbxAddMetadata->SetValue(false);
	chbxAddMetadata->SetToolTip(_("optsAddMetadata_Hint"));
	FlexGridSizer3->Add(chbxAddMetadata, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel1,wxSize(5,0)).GetWidth());
	FlexGridSizer1->Add(FlexGridSizer3, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel1,wxSize(5,0)).GetWidth());
	FlexGridSizer5 = new wxFlexGridSizer(0, 2, 0, 0);
	StaticText3 = new wxStaticText(Panel1, ID_STATICTEXT3, _("optsPreviewLabel"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer5->Add(StaticText3, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel1,wxSize(5,0)).GetWidth());
	chPreview = new wxChoice(Panel1, ID_CHOICE1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
	chPreview->Append(_("optsPreviewNone"));
	chPreview->Append(_("optsPreviewMedium"));
	chPreview->Append(_("optsPreviewBig"));
	chPreview->SetToolTip(_("optsPreview_Hint"));
	FlexGridSizer5->Add(chPreview, 1, wxBOTTOM|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel1,wxSize(5,0)).GetWidth());
	chbxUseDateForPath = new wxCheckBox(Panel1, ID_CHECKBOX5, _("optsRegulating"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX5"));
	chbxUseDateForPath->SetValue(false);
	chbxUseDateForPath->SetToolTip(_("optsRegulating_Hint"));
	FlexGridSizer5->Add(chbxUseDateForPath, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel1,wxSize(5,0)).GetWidth());
	chDateType = new wxChoice(Panel1, ID_CHOICE2, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE2"));
	chDateType->Append(_("optsDateFormat1"));
	chDateType->Append(_("optsDateFormat2"));
	chDateType->Append(_("optsDateFormat3"));
	chDateType->Append(_("optsDateFormat4"));
	chDateType->SetToolTip(_("optsDateFormat_Hint"));
	FlexGridSizer5->Add(chDateType, 1, wxBOTTOM|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel1,wxSize(5,0)).GetWidth());
	chkbArtist = new wxCheckBox(Panel1, ID_CHECKBOX6, _("optsArtistChk"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX6"));
	chkbArtist->SetValue(false);
	chkbArtist->SetToolTip(_("optsArtistChk_Hint"));
	FlexGridSizer5->Add(chkbArtist, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel1,wxSize(5,0)).GetWidth());
	txtArtist = new wxTextCtrl(Panel1, ID_TEXTCTRL1, _("Text"), wxDefaultPosition, wxDLG_UNIT(Panel1,wxSize(143,12)), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	txtArtist->SetToolTip(_("optsArtist_Hint"));
	FlexGridSizer5->Add(txtArtist, 1, wxBOTTOM|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel1,wxSize(5,0)).GetWidth());
	FlexGridSizer1->Add(FlexGridSizer5, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel1,wxSize(5,0)).GetWidth());
	StaticText5 = new wxStaticText(Panel1, ID_STATICTEXT5, _("optsMiscLabel"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	wxFont StaticText5Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	if ( !StaticText5Font.Ok() ) StaticText5Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	StaticText5Font.SetPointSize((int)(StaticText5Font.GetPointSize() * 1.000000));
	StaticText5Font.SetWeight(wxBOLD);
	StaticText5->SetFont(StaticText5Font);
	FlexGridSizer1->Add(StaticText5, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel1,wxSize(5,0)).GetWidth());
	FlexGridSizer6 = new wxFlexGridSizer(0, 2, -2, wxDLG_UNIT(Panel1,wxSize(-5,0)).GetWidth());
	FlexGridSizer6->AddGrowableCol(0);
	StaticText6 = new wxStaticText(Panel1, ID_STATICTEXT6, _("optsLanguageLabel"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	FlexGridSizer6->Add(StaticText6, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel1,wxSize(5,0)).GetWidth());
	chLanguage = new wxChoice(Panel1, ID_CHOICE3, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE3"));
	chLanguage->Append(_("Chinese"));
	chLanguage->Append(_("English"));
	chLanguage->Append(_("German"));
	chLanguage->Append(_("Italian"));
	chLanguage->Append(_("Finnish"));
	chLanguage->Append(_("Norwegian"));
	chLanguage->Append(_("Polish"));
	chLanguage->Append(_("Russian"));
	chLanguage->Append(_("Spanish"));
	chLanguage->Append(_("French"));
	chLanguage->SetToolTip(_("optsLanguage_Hint"));
	FlexGridSizer6->Add(chLanguage, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel1,wxSize(5,0)).GetWidth());
	FlexGridSizer1->Add(FlexGridSizer6, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel1,wxSize(5,0)).GetWidth());
	Panel1->SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(Panel1);
	FlexGridSizer1->SetSizeHints(Panel1);
	pnlGroups = new wxPanel(nbMain, ID_PANEL2, wxPoint(88,5), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL2"));
	szGroupsMain = new wxFlexGridSizer(0, 1, 0, 0);
	szGroups = new wxFlexGridSizer(0, 2, -2, wxDLG_UNIT(pnlGroups,wxSize(-5,0)).GetWidth());
	szGroupsMain->Add(szGroups, 0, wxALIGN_LEFT|wxALIGN_TOP, 0);
	szGroupsMain->Add(-1,-1,1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(pnlGroups,wxSize(5,0)).GetWidth());
	lblGroupNotice = new wxStaticText(pnlGroups, ID_STATICTEXT4, _("lblGroupNotice"), wxDefaultPosition, wxDLG_UNIT(pnlGroups,wxSize(150,-1)), 0, _T("ID_STATICTEXT4"));
	szGroupsMain->Add(lblGroupNotice, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(pnlGroups,wxSize(5,0)).GetWidth());
	pnlGroups->SetSizer(szGroupsMain);
	szGroupsMain->Fit(pnlGroups);
	szGroupsMain->SetSizeHints(pnlGroups);
	Panel3 = new wxPanel(nbMain, ID_PNL_CAM_OPTS, wxPoint(177,10), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PNL_CAM_OPTS"));
	FlexGridSizer8 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer8->AddGrowableCol(0);
	FlexGridSizer9 = new wxFlexGridSizer(0, 1, wxDLG_UNIT(Panel3,wxSize(-7,0)).GetWidth(), 0);
	FlexGridSizer9->AddGrowableCol(0);
	StaticText12 = new wxStaticText(Panel3, ID_STATICTEXT12, _("optsCamera"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT12"));
	wxFont StaticText12Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	if ( !StaticText12Font.Ok() ) StaticText12Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	StaticText12Font.SetPointSize((int)(StaticText12Font.GetPointSize() * 1.000000));
	StaticText12Font.SetWeight(wxBOLD);
	StaticText12->SetFont(StaticText12Font);
	FlexGridSizer9->Add(StaticText12, 1, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel3,wxSize(5,0)).GetWidth());
	chCameraSelector = new wxChoice(Panel3, ID_CHOICE9, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE9"));
	FlexGridSizer9->Add(chCameraSelector, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel3,wxSize(5,0)).GetWidth());
	FlexGridSizer10 = new wxFlexGridSizer(0, 4, 0, wxDLG_UNIT(Panel3,wxSize(0,0)).GetWidth());
	FlexGridSizer10->AddGrowableCol(1);
	btnGetLastest = new wxButton(Panel3, ID_BUTTON1, _("btnGetLastest"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	FlexGridSizer10->Add(btnGetLastest, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel3,wxSize(5,0)).GetWidth());
	FlexGridSizer2 = new wxFlexGridSizer(0, 3, 0, 0);
	SecretPanel = new wxPanel(Panel3, ID_PANEL3, wxDefaultPosition, wxSize(40,20), wxTAB_TRAVERSAL, _T("ID_PANEL3"));
	FlexGridSizer2->Add(SecretPanel, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel3,wxSize(5,0)).GetWidth());
	btnCopy = new wxButton(Panel3, ID_BUTTON3, _("btnCopy"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON3"));
	FlexGridSizer2->Add(btnCopy, 1, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_RIGHT|wxALIGN_TOP, wxDLG_UNIT(Panel3,wxSize(5,0)).GetWidth());
	btnDelete = new wxButton(Panel3, ID_BUTTON2, _("btnDelete"), wxDefaultPosition, wxSize(-1,-1), wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON2"));
	FlexGridSizer2->Add(btnDelete, 1, wxALL|wxALIGN_RIGHT|wxALIGN_TOP, wxDLG_UNIT(Panel3,wxSize(5,0)).GetWidth());
	FlexGridSizer10->Add(FlexGridSizer2, 1, wxALIGN_RIGHT|wxALIGN_TOP, wxDLG_UNIT(Panel3,wxSize(5,0)).GetWidth());
	FlexGridSizer9->Add(FlexGridSizer10, 1, wxBOTTOM|wxEXPAND|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel3,wxSize(5,0)).GetWidth());
	FlexGridSizer8->Add(FlexGridSizer9, 1, wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel3,wxSize(5,0)).GetWidth());
	scrlwCameraOpt = new wxScrolledWindow(Panel3, ID_SCROLLEDWINDOW1, wxDefaultPosition, wxDefaultSize, wxVSCROLL, _T("ID_SCROLLEDWINDOW1"));
	scrlwCameraOpt->SetMaxSize(wxDLG_UNIT(Panel3,wxSize(-1,190)));
	scrlwCameraOpt->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	FlexGridSizer11 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer11->AddGrowableCol(1);
	wxSize __SpacerSize_1 = wxDLG_UNIT(scrlwCameraOpt,wxSize(5,5));
	FlexGridSizer11->Add(__SpacerSize_1.GetWidth(),__SpacerSize_1.GetHeight(),1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	btnResetToDefaults = new wxButton(scrlwCameraOpt, ID_BUTTON5, _("btnResetToDefaults"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON5"));
	btnResetToDefaults->SetToolTip(_("btnResetToDefaultsHint"));
	FlexGridSizer11->Add(btnResetToDefaults, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	StaticText13 = new wxStaticText(scrlwCameraOpt, ID_STATICTEXT13, _("lblCameraName"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT13"));
	FlexGridSizer11->Add(StaticText13, 1, wxTOP|wxLEFT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	txtCamName = new wxTextCtrl(scrlwCameraOpt, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	FlexGridSizer11->Add(txtCamName, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	StaticText4 = new wxStaticText(scrlwCameraOpt, ID_STATICTEXT8, _("lblShortName"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
	FlexGridSizer11->Add(StaticText4, 1, wxTOP|wxLEFT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	txtShortName = new wxTextCtrl(scrlwCameraOpt, ID_TEXTCTRL5, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL5"));
	FlexGridSizer11->Add(txtShortName, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	StaticText30 = new wxStaticText(scrlwCameraOpt, ID_STATICTEXT30, _("lblGeometry"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT30"));
	wxFont StaticText30Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	if ( !StaticText30Font.Ok() ) StaticText30Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	StaticText30Font.SetPointSize((int)(StaticText30Font.GetPointSize() * 1.000000));
	StaticText30Font.SetWeight(wxBOLD);
	StaticText30->SetFont(StaticText30Font);
	FlexGridSizer11->Add(StaticText30, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	FlexGridSizer11->Add(-1,-1,1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	StaticText14 = new wxStaticText(scrlwCameraOpt, ID_STATICTEXT14, _("lblRawSize"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT14"));
	FlexGridSizer11->Add(StaticText14, 1, wxTOP|wxLEFT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	FlexGridSizer12 = new wxFlexGridSizer(0, 4, 0, wxDLG_UNIT(scrlwCameraOpt,wxSize(-2,0)).GetWidth());
	txtSensWidth = new wxTextCtrl(scrlwCameraOpt, ID_TEXTCTRL3, wxEmptyString, wxDefaultPosition, wxDLG_UNIT(scrlwCameraOpt,wxSize(30,-1)), 0, wxDefaultValidator, _T("ID_TEXTCTRL3"));
	FlexGridSizer12->Add(txtSensWidth, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	StaticText15 = new wxStaticText(scrlwCameraOpt, ID_STATICTEXT15, _("x"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT15"));
	FlexGridSizer12->Add(StaticText15, 1, wxTOP|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	txtSensHeight = new wxTextCtrl(scrlwCameraOpt, ID_TEXTCTRL4, wxEmptyString, wxDefaultPosition, wxDLG_UNIT(scrlwCameraOpt,wxSize(30,-1)), 0, wxDefaultValidator, _T("ID_TEXTCTRL4"));
	FlexGridSizer12->Add(txtSensHeight, 1, wxTOP|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	lblFileSize = new wxStaticText(scrlwCameraOpt, ID_STATICTEXT7, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
	FlexGridSizer12->Add(lblFileSize, 1, wxTOP|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	FlexGridSizer11->Add(FlexGridSizer12, 1, wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	StaticText17 = new wxStaticText(scrlwCameraOpt, ID_STATICTEXT17, _("lblJpegSize"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT17"));
	FlexGridSizer11->Add(StaticText17, 1, wxTOP|wxLEFT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	FlexGridSizer13 = new wxFlexGridSizer(0, 3, 0, wxDLG_UNIT(scrlwCameraOpt,wxSize(-2,0)).GetWidth());
	txtJpegWidth = new wxTextCtrl(scrlwCameraOpt, ID_TEXTCTRL6, wxEmptyString, wxDefaultPosition, wxDLG_UNIT(scrlwCameraOpt,wxSize(30,-1)), 0, wxDefaultValidator, _T("ID_TEXTCTRL6"));
	FlexGridSizer13->Add(txtJpegWidth, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	StaticText18 = new wxStaticText(scrlwCameraOpt, ID_STATICTEXT18, _("x"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT18"));
	FlexGridSizer13->Add(StaticText18, 1, wxTOP|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	txtJpegHeight = new wxTextCtrl(scrlwCameraOpt, ID_TEXTCTRL7, wxEmptyString, wxDefaultPosition, wxDLG_UNIT(scrlwCameraOpt,wxSize(30,-1)), 0, wxDefaultValidator, _T("ID_TEXTCTRL7"));
	FlexGridSizer13->Add(txtJpegHeight, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	FlexGridSizer11->Add(FlexGridSizer13, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	StaticText19 = new wxStaticText(scrlwCameraOpt, ID_STATICTEXT19, _("lblActiveX1Y1"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT19"));
	FlexGridSizer11->Add(StaticText19, 1, wxTOP|wxLEFT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	FlexGridSizer14 = new wxFlexGridSizer(0, 3, 0, wxDLG_UNIT(scrlwCameraOpt,wxSize(-2,0)).GetWidth());
	txtActiveX1 = new wxTextCtrl(scrlwCameraOpt, ID_TEXTCTRL8, wxEmptyString, wxDefaultPosition, wxDLG_UNIT(scrlwCameraOpt,wxSize(30,-1)), 0, wxDefaultValidator, _T("ID_TEXTCTRL8"));
	FlexGridSizer14->Add(txtActiveX1, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	StaticText20 = new wxStaticText(scrlwCameraOpt, ID_STATICTEXT20, _(","), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT20"));
	FlexGridSizer14->Add(StaticText20, 1, wxTOP|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	txtActiveY1 = new wxTextCtrl(scrlwCameraOpt, ID_TEXTCTRL9, wxEmptyString, wxDefaultPosition, wxDLG_UNIT(scrlwCameraOpt,wxSize(30,-1)), 0, wxDefaultValidator, _T("ID_TEXTCTRL9"));
	FlexGridSizer14->Add(txtActiveY1, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	FlexGridSizer11->Add(FlexGridSizer14, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	StaticText21 = new wxStaticText(scrlwCameraOpt, ID_STATICTEXT21, _("lblActiveX2Y2"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT21"));
	FlexGridSizer11->Add(StaticText21, 1, wxTOP|wxLEFT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	FlexGridSizer15 = new wxFlexGridSizer(0, 3, 0, wxDLG_UNIT(scrlwCameraOpt,wxSize(-2,0)).GetWidth());
	txtActiveX2 = new wxTextCtrl(scrlwCameraOpt, ID_TEXTCTRL10, wxEmptyString, wxDefaultPosition, wxDLG_UNIT(scrlwCameraOpt,wxSize(30,-1)), 0, wxDefaultValidator, _T("ID_TEXTCTRL10"));
	FlexGridSizer15->Add(txtActiveX2, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	StaticText22 = new wxStaticText(scrlwCameraOpt, ID_STATICTEXT22, _(","), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT22"));
	FlexGridSizer15->Add(StaticText22, 1, wxTOP|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	txtActiveY2 = new wxTextCtrl(scrlwCameraOpt, ID_TEXTCTRL11, wxEmptyString, wxDefaultPosition, wxDLG_UNIT(scrlwCameraOpt,wxSize(30,-1)), 0, wxDefaultValidator, _T("ID_TEXTCTRL11"));
	FlexGridSizer15->Add(txtActiveY2, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	FlexGridSizer11->Add(FlexGridSizer15, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	StaticText23 = new wxStaticText(scrlwCameraOpt, ID_STATICTEXT23, _("lblBlackWhileLevels"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT23"));
	FlexGridSizer11->Add(StaticText23, 1, wxTOP|wxLEFT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	FlexGridSizer16 = new wxFlexGridSizer(0, 3, 0, wxDLG_UNIT(scrlwCameraOpt,wxSize(-2,0)).GetWidth());
	txtBlackLevel = new wxTextCtrl(scrlwCameraOpt, ID_TEXTCTRL12, wxEmptyString, wxDefaultPosition, wxDLG_UNIT(scrlwCameraOpt,wxSize(30,-1)), 0, wxDefaultValidator, _T("ID_TEXTCTRL12"));
	FlexGridSizer16->Add(txtBlackLevel, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	StaticText24 = new wxStaticText(scrlwCameraOpt, ID_STATICTEXT24, _("-"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT24"));
	FlexGridSizer16->Add(StaticText24, 1, wxTOP|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	txtWhiteLevel = new wxTextCtrl(scrlwCameraOpt, ID_TEXTCTRL13, wxEmptyString, wxDefaultPosition, wxDLG_UNIT(scrlwCameraOpt,wxSize(30,-1)), 0, wxDefaultValidator, _T("ID_TEXTCTRL13"));
	FlexGridSizer16->Add(txtWhiteLevel, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	FlexGridSizer11->Add(FlexGridSizer16, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	StaticText25 = new wxStaticText(scrlwCameraOpt, ID_STATICTEXT25, _("lblMosaicType"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT25"));
	FlexGridSizer11->Add(StaticText25, 1, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	chMosaicType = new wxChoice(scrlwCameraOpt, ID_CHOICE10, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE10"));
	chMosaicType->Append(_("Red,Green Green,Blue"));
	chMosaicType->Append(_("Green,Blue Red,Green"));
	FlexGridSizer11->Add(chMosaicType, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	StaticText31 = new wxStaticText(scrlwCameraOpt, ID_STATICTEXT31, _("lblColorMatrix1"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT31"));
	wxFont StaticText31Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	if ( !StaticText31Font.Ok() ) StaticText31Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	StaticText31Font.SetPointSize((int)(StaticText31Font.GetPointSize() * 1.000000));
	StaticText31Font.SetWeight(wxBOLD);
	StaticText31->SetFont(StaticText31Font);
	FlexGridSizer11->Add(StaticText31, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	FlexGridSizer11->Add(-1,-1,1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	StaticText26 = new wxStaticText(scrlwCameraOpt, ID_STATICTEXT26, _("lblMatrIll"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT26"));
	FlexGridSizer11->Add(StaticText26, 1, wxTOP|wxLEFT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	chIll1 = new wxChoice(scrlwCameraOpt, ID_CHOICE11, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE11"));
	FlexGridSizer11->Add(chIll1, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	StaticText27 = new wxStaticText(scrlwCameraOpt, ID_STATICTEXT27, _("lblMatr"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT27"));
	FlexGridSizer11->Add(StaticText27, 1, wxTOP|wxLEFT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	txtMatr1 = new wxTextCtrl(scrlwCameraOpt, ID_TEXTCTRL15, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL15"));
	FlexGridSizer11->Add(txtMatr1, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	wxSize __SpacerSize_2 = wxDLG_UNIT(scrlwCameraOpt,wxSize(5,5));
	FlexGridSizer11->Add(__SpacerSize_2.GetWidth(),__SpacerSize_2.GetHeight(),1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	btnCalibr = new wxButton(scrlwCameraOpt, ID_BUTTON4, _("btnCalibr"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON4"));
	FlexGridSizer11->Add(btnCalibr, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	StaticText28 = new wxStaticText(scrlwCameraOpt, ID_STATICTEXT28, _("lblMatrixMult"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT28"));
	FlexGridSizer11->Add(StaticText28, 1, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	txtMatrix1Mult = new wxTextCtrl(scrlwCameraOpt, ID_TEXTCTRL16, wxEmptyString, wxDefaultPosition, wxDLG_UNIT(scrlwCameraOpt,wxSize(64,-1)), 0, wxDefaultValidator, _T("ID_TEXTCTRL16"));
	FlexGridSizer11->Add(txtMatrix1Mult, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	StaticText32 = new wxStaticText(scrlwCameraOpt, ID_STATICTEXT32, _("lblColorMatrix2"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT32"));
	wxFont StaticText32Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	if ( !StaticText32Font.Ok() ) StaticText32Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	StaticText32Font.SetPointSize((int)(StaticText32Font.GetPointSize() * 1.000000));
	StaticText32Font.SetWeight(wxBOLD);
	StaticText32->SetFont(StaticText32Font);
	FlexGridSizer11->Add(StaticText32, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	FlexGridSizer11->Add(-1,-1,1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	StaticText29 = new wxStaticText(scrlwCameraOpt, ID_STATICTEXT29, _("lblMatrIll"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT29"));
	FlexGridSizer11->Add(StaticText29, 1, wxTOP|wxLEFT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	chIll2 = new wxChoice(scrlwCameraOpt, ID_CHOICE12, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE12"));
	FlexGridSizer11->Add(chIll2, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	StaticText33 = new wxStaticText(scrlwCameraOpt, ID_STATICTEXT33, _("lblMatr"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT33"));
	FlexGridSizer11->Add(StaticText33, 1, wxTOP|wxLEFT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	txtMatr2 = new wxTextCtrl(scrlwCameraOpt, ID_TEXTCTRL18, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL18"));
	FlexGridSizer11->Add(txtMatr2, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	StaticText34 = new wxStaticText(scrlwCameraOpt, ID_STATICTEXT34, _("lblMatrixMult"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT34"));
	FlexGridSizer11->Add(StaticText34, 1, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	txtMatrix2Mult = new wxTextCtrl(scrlwCameraOpt, ID_TEXTCTRL19, wxEmptyString, wxDefaultPosition, wxDLG_UNIT(scrlwCameraOpt,wxSize(64,-1)), 0, wxDefaultValidator, _T("ID_TEXTCTRL19"));
	FlexGridSizer11->Add(txtMatrix2Mult, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrlwCameraOpt,wxSize(5,0)).GetWidth());
	scrlwCameraOpt->SetSizer(FlexGridSizer11);
	FlexGridSizer11->Fit(scrlwCameraOpt);
	FlexGridSizer11->SetSizeHints(scrlwCameraOpt);
	FlexGridSizer8->Add(scrlwCameraOpt, 1, wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(Panel3,wxSize(5,0)).GetWidth());
	Panel3->SetSizer(FlexGridSizer8);
	FlexGridSizer8->Fit(Panel3);
	FlexGridSizer8->SetSizeHints(Panel3);
	nbMain->AddPage(Panel1, _("pgMain"), false);
	nbMain->AddPage(pnlGroups, _("optsCameraTypesLabel"), false);
	nbMain->AddPage(Panel3, _("pgCamOpts"), false);
	szOptions->Add(nbMain, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, _("btnOk")));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, _("btnCancel")));
	StdDialogButtonSizer1->Realize();
	szOptions->Add(StdDialogButtonSizer1, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(7,0)).GetWidth());
	SetSizer(szOptions);
	szOptions->Fit(this);
	szOptions->SetSizeHints(this);
	Center();

	Connect(ID_CHECKBOX5,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&OptionsDialog::chbxUseDateForPathClick);
	Connect(ID_CHECKBOX6,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&OptionsDialog::chkbArtistClick);
	Connect(ID_CHOICE9,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&OptionsDialog::chCameraSelectorSelect);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OptionsDialog::OnGetLastestClick);
	SecretPanel->Connect(wxEVT_MIDDLE_DOWN,(wxObjectEventFunction)&OptionsDialog::PnlSecretLeftDClick,0,this);
	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OptionsDialog::btnCopyClick);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OptionsDialog::btnDeleteClick);
	Connect(ID_BUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OptionsDialog::btnResetToDefaultsClick);
	Connect(ID_TEXTCTRL3,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&OptionsDialog::txtSensWidthText);
	Connect(ID_TEXTCTRL4,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&OptionsDialog::txtSensWidthText);
	Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OptionsDialog::btnCalibrClick);
	Connect(ID_NOTEBOOK1,wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,(wxObjectEventFunction)&OptionsDialog::nbMainPageChanged);
	Connect(ID_NOTEBOOK1,wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING,(wxObjectEventFunction)&OptionsDialog::nbMainPageChanging);
	//*)


	scrlwCameraOpt->SetScrollRate(16, 16);

	fill_illum_strings(chIll1);
	fill_illum_strings(chIll2);

	FlexGridSizer11->Layout();
}

OptionsDialog::~OptionsDialog()
{
	//(*Destroy(OptionsDialog)
	//*)
}

// OptionsDialog::btnOkClick
void OptionsDialog::btnOkClick(wxCommandEvent& event)
{
    this->EndModal(1);
}

// OptionsDialog::btnCancelClick
void OptionsDialog::btnCancelClick(wxCommandEvent& event)
{
    this->EndModal(0);
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

    for (int i = 0; langs[i] != wxLANGUAGE_DEFAULT; i++)
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

// OptionsDialog::chbxUseDateForPathClick
void OptionsDialog::chbxUseDateForPathClick(wxCommandEvent& event)
{
    correct_interface();
}

void OptionsDialog::chkbArtistClick(wxCommandEvent& event)
{
    correct_interface();
}

///////////////////////////////////////////////
//                                           //
//        C A M E R A   O P T I O N S        //
//                                           //
///////////////////////////////////////////////

void OptionsDialog::PnlSecretLeftDClick(wxMouseEvent& event)
{
	CameraOpts def_opts;
	wxFileConfig fc;

	def_opts.set_defaults(true);
	def_opts.save(fc, false);

	wxFileName file_name;

	file_name.SetPath(wxStandardPaths::Get().GetUserDataDir());
	wxFileName::Mkdir(file_name.GetFullPath(), 0777, wxPATH_MKDIR_FULL);

	file_name.SetName(L"cameras.txt");

	wxFFileOutputStream stream(file_name.GetFullPath());
	fc.Save(stream);
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

	item.mosaic = (chMosaicType->GetSelection() == wxNOT_FOUND) ? MOSAIC_TYPE1 : mosaic_types[chMosaicType->GetSelection()];

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
	int width = get_int(txtSensWidth);
	int height = get_int(txtSensHeight);
	int size = -1;
	bool ok = true;
	if ((width != -1) && (height != -1))
	{
		int size_in_bits = (width*height*10);
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
	frame->on_save_ = boost::bind(&OptionsDialog::matrix_calculated, this, _1);
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
}


///////////////////////////////////////////////
//                                           //
//        C A M E R A S   G R O U P S        //
//                                           //
///////////////////////////////////////////////


void OptionsDialog::show_groups()
{
	szGroups->Clear(true);
	group_controls_.clear();

	typedef std::map<unsigned int, std::pair<unsigned int, unsigned int> > FileSizes;
	FileSizes file_sizes;
	const size_t count = cam_opts_->get_size();
	for (size_t i = 0; i < count; i++)
	{
		const CameraData &item = cam_opts_->at(i);
		unsigned int file_size = CameraOpts::get_file_size(item);
		file_sizes[file_size] = std::make_pair(item.width, item.height);
	}

	foreach(FileSizes::value_type &size_item, file_sizes)
	{
		wxString label = wxString::Format(L"%i x %i:", size_item.second.first, size_item.second.second);
		wxStaticText *text = new wxStaticText(pnlGroups, wxNewId(), label, wxDefaultPosition, wxDefaultSize);
		szGroups->Add(text, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(pnlGroups,wxSize(5,0)).GetWidth());

		wxChoice *choice = new wxChoice(pnlGroups, wxNewId(), wxDefaultPosition, wxDefaultSize);
		szGroups->Add(choice, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(pnlGroups,wxSize(5,0)).GetWidth());

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
}

void OptionsDialog::read_groups()
{
	if (group_controls_.empty()) return;

	cam_opts_->clear_groups();

	foreach(GroupControls::value_type &item, group_controls_)
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
	if (event.GetSelection() == 1)
	{
		read_camera_opts();
		show_groups();
	}
}

void OptionsDialog::nbMainPageChanging(wxNotebookEvent& event)
{
	if (event.GetSelection() == 1) read_groups();
}

void OptionsDialog::OnGetLastestClick(wxCommandEvent& event)
{
	wxHTTP get;
	get.SetHeader(_T("Content-type"), _T("text/html; charset=utf-8"));
	get.SetTimeout(30);
	 
	while (!get.Connect(_T("dng4ps2.googlecode.com"))) wxSleep(5);
	 
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
