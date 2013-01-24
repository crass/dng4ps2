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

#ifndef WX_PRECOMP
//(*InternalHeadersPCH(CamCalibrFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)
#endif
//(*InternalHeaders(CamCalibrFrame)
#include <wx/settings.h>
#include <wx/font.h>
//*)

#include <wx/image.h>
#include <wx/bitmap.h>

#include "CamCalibrFrame.h"
#include "Exception.hpp"
#include "CameraOpts.h"
#include "Utils.h"

//(*IdInit(CamCalibrFrame)
const long CamCalibrFrame::ID_STATICTEXT3 = wxNewId();
const long CamCalibrFrame::ID_STATICTEXT1 = wxNewId();
const long CamCalibrFrame::ID_TEXTCTRL1 = wxNewId();
const long CamCalibrFrame::ID_BUTTON1 = wxNewId();
const long CamCalibrFrame::ID_STATICTEXT2 = wxNewId();
const long CamCalibrFrame::ID_TEXTCTRL2 = wxNewId();
const long CamCalibrFrame::ID_STATICTEXT9 = wxNewId();
const long CamCalibrFrame::ID_SLIDER1 = wxNewId();
const long CamCalibrFrame::ID_TEXTCTRL4 = wxNewId();
const long CamCalibrFrame::ID_STATICTEXT10 = wxNewId();
const long CamCalibrFrame::ID_SLIDER2 = wxNewId();
const long CamCalibrFrame::ID_TEXTCTRL5 = wxNewId();
const long CamCalibrFrame::ID_STATICLINE1 = wxNewId();
const long CamCalibrFrame::ID_STATICTEXT7 = wxNewId();
const long CamCalibrFrame::ID_STATICTEXT5 = wxNewId();
const long CamCalibrFrame::ID_CHOICE1 = wxNewId();
const long CamCalibrFrame::ID_STATICTEXT6 = wxNewId();
const long CamCalibrFrame::ID_LISTBOX1 = wxNewId();
const long CamCalibrFrame::ID_STATICTEXT15 = wxNewId();
const long CamCalibrFrame::ID_RADIOBUTTON2 = wxNewId();
const long CamCalibrFrame::ID_STATICTEXT16 = wxNewId();
const long CamCalibrFrame::ID_RADIOBUTTON1 = wxNewId();
const long CamCalibrFrame::ID_STATICTEXT13 = wxNewId();
const long CamCalibrFrame::ID_SPINCTRL3 = wxNewId();
const long CamCalibrFrame::ID_STATICTEXT12 = wxNewId();
const long CamCalibrFrame::ID_SPINCTRL2 = wxNewId();
const long CamCalibrFrame::ID_STATICTEXT14 = wxNewId();
const long CamCalibrFrame::ID_SPINCTRL4 = wxNewId();
const long CamCalibrFrame::ID_STATICTEXT11 = wxNewId();
const long CamCalibrFrame::ID_SPINCTRL1 = wxNewId();
const long CamCalibrFrame::ID_BUTTON5 = wxNewId();
const long CamCalibrFrame::ID_BUTTON7 = wxNewId();
const long CamCalibrFrame::ID_BUTTON8 = wxNewId();
const long CamCalibrFrame::ID_BUTTON6 = wxNewId();
const long CamCalibrFrame::ID_STATICLINE2 = wxNewId();
const long CamCalibrFrame::ID_STATICTEXT8 = wxNewId();
const long CamCalibrFrame::ID_BUTTON3 = wxNewId();
const long CamCalibrFrame::ID_BUTTON4 = wxNewId();
const long CamCalibrFrame::ID_BUTTON2 = wxNewId();
const long CamCalibrFrame::ID_STATICLINE3 = wxNewId();
const long CamCalibrFrame::ID_STATICTEXT4 = wxNewId();
const long CamCalibrFrame::ID_STATICTEXT17 = wxNewId();
const long CamCalibrFrame::ID_CHOICE2 = wxNewId();
const long CamCalibrFrame::ID_BUTTON9 = wxNewId();
const long CamCalibrFrame::ID_TEXTCTRL3 = wxNewId();
const long CamCalibrFrame::ID_STATICBITMAP_IMAGE = wxNewId();
const long CamCalibrFrame::ID_SCROLLEDWINDOW1 = wxNewId();
const long CamCalibrFrame::ID_TIMER1 = wxNewId();
const long CamCalibrFrame::ID_TIMER2 = wxNewId();
//*)

const long CamCalibrFrame::LOGLVL_BRIEF = 0;
const long CamCalibrFrame::LOGLVL_STANDARD = 1;
const long CamCalibrFrame::LOGLVL_VERBOSE = 2;

BEGIN_EVENT_TABLE(CamCalibrFrame,wxFrame)
	//(*EventTable(CamCalibrFrame)
	//*)
END_EVENT_TABLE()

CamCalibrFrame::CamCalibrFrame(const CameraData &camera, wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size) : camera_(camera)
{
	//(*Initialize(CamCalibrFrame)
	wxStaticText* StaticText10;
	wxStaticText* StaticText9;
	wxFlexGridSizer* FlexGridSizer4;
	wxButton* btnProcess;
	wxStaticText* StaticText13;
	wxStaticText* StaticText2;
	wxStaticText* StaticText14;
	wxFlexGridSizer* FlexGridSizer10;
	wxFlexGridSizer* FlexGridSizer3;
	wxStaticText* StaticText6;
	wxFlexGridSizer* FlexGridSizer5;
	wxStaticText* StaticText8;
	wxStaticText* StaticText11;
	wxFlexGridSizer* FlexGridSizer9;
	wxFlexGridSizer* FlexGridSizer2;
	wxStaticText* StaticText1;
	wxButton* btnSelectRaw;
	wxStaticText* StaticText3;
	wxFlexGridSizer* FlexGridSizer7;
	wxButton* btnDelArea;
	wxButton* btnExit;
	wxStaticText* StaticText5;
	wxStaticText* StaticText7;
	wxFlexGridSizer* FlexGridSizer8;
	wxButton* btnLoad;
	wxButton* btnClearLog;
	wxFlexGridSizer* FlexGridSizer14;
	wxStaticText* StaticText15;
	wxFlexGridSizer* FlexGridSizer13;
	wxStaticText* StaticText12;
	wxFlexGridSizer* FlexGridSizer12;
	wxButton* btnClearAreas;
	wxStaticText* StaticText17;
	wxStaticText* StaticText4;
	wxButton* btnSaveAreas;
	wxStaticText* StaticText16;
	
	Create(parent, wxID_ANY, _("Camera calibration"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
	SetClientSize(wxSize(-1,-1));
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	szMain = new wxFlexGridSizer(1, 2, 0, 0);
	szMain->AddGrowableCol(1);
	szMain->AddGrowableRow(0);
	FlexGridSizer2 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer2->AddGrowableRow(12);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Images source"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	wxFont StaticText3Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	if ( !StaticText3Font.Ok() ) StaticText3Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	StaticText3Font.SetPointSize((int)(StaticText3Font.GetPointSize() * 1.000000));
	StaticText3Font.SetWeight(wxBOLD);
	StaticText3->SetFont(StaticText3Font);
	FlexGridSizer2->Add(StaticText3, 1, wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer4 = new wxFlexGridSizer(0, 2, wxDLG_UNIT(this,wxSize(-3,0)).GetWidth(), wxDLG_UNIT(this,wxSize(0,0)).GetWidth());
	FlexGridSizer4->AddGrowableCol(0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("RAW file:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer4->Add(StaticText1, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	wxSize __SpacerSize_1 = wxDLG_UNIT(this,wxSize(5,5));
	FlexGridSizer4->Add(__SpacerSize_1.GetWidth(),__SpacerSize_1.GetHeight(),1, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	txtRawFile = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDLG_UNIT(this,wxSize(140,12)), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	FlexGridSizer4->Add(txtRawFile, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	btnSelectRaw = new wxButton(this, ID_BUTTON1, _("..."), wxDefaultPosition, wxDLG_UNIT(this,wxSize(15,12)), 0, wxDefaultValidator, _T("ID_BUTTON1"));
	FlexGridSizer4->Add(btnSelectRaw, 1, wxBOTTOM|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("JPEG file:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer4->Add(StaticText2, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	wxSize __SpacerSize_2 = wxDLG_UNIT(this,wxSize(5,5));
	FlexGridSizer4->Add(__SpacerSize_2.GetWidth(),__SpacerSize_2.GetHeight(),1, wxBOTTOM|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	txtJpegFile = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxDLG_UNIT(this,wxSize(140,12)), wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	txtJpegFile->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	FlexGridSizer4->Add(txtJpegFile, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer2->Add(FlexGridSizer4, 1, wxLEFT|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer9 = new wxFlexGridSizer(0, 3, wxDLG_UNIT(this,wxSize(-5,0)).GetWidth(), 0);
	StaticText9 = new wxStaticText(this, ID_STATICTEXT9, _("Gamma:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT9"));
	FlexGridSizer9->Add(StaticText9, 1, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	sldGamma = new wxSlider(this, ID_SLIDER1, 190, 101, 300, wxPoint(0,0), wxDLG_UNIT(this,wxSize(104,12)), wxSL_HORIZONTAL|wxSL_BOTH, wxDefaultValidator, _T("ID_SLIDER1"));
	FlexGridSizer9->Add(sldGamma, 1, wxTOP|wxBOTTOM|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	txtGamma = new wxTextCtrl(this, ID_TEXTCTRL4, _("Text"), wxDefaultPosition, wxDLG_UNIT(this,wxSize(30,12)), 0, wxDefaultValidator, _T("ID_TEXTCTRL4"));
	FlexGridSizer9->Add(txtGamma, 1, wxTOP|wxBOTTOM|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	StaticText10 = new wxStaticText(this, ID_STATICTEXT10, _("Im. bright.:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT10"));
	FlexGridSizer9->Add(StaticText10, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	sldBright = new wxSlider(this, ID_SLIDER2, 100, 50, 300, wxDefaultPosition, wxDLG_UNIT(this,wxSize(104,12)), wxSL_HORIZONTAL|wxSL_BOTH, wxDefaultValidator, _T("ID_SLIDER2"));
	FlexGridSizer9->Add(sldBright, 1, wxTOP|wxBOTTOM|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	txtBright = new wxTextCtrl(this, ID_TEXTCTRL5, _("Text"), wxDefaultPosition, wxDLG_UNIT(this,wxSize(30,12)), 0, wxDefaultValidator, _T("ID_TEXTCTRL5"));
	FlexGridSizer9->Add(txtBright, 1, wxTOP|wxBOTTOM|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer2->Add(FlexGridSizer9, 1, wxBOTTOM|wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	StaticLine1 = new wxStaticLine(this, ID_STATICLINE1, wxDefaultPosition, wxSize(10,1), wxLI_HORIZONTAL, _T("ID_STATICLINE1"));
	FlexGridSizer2->Add(StaticLine1, 1, wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	StaticText7 = new wxStaticText(this, ID_STATICTEXT7, _("Areas selection"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
	wxFont StaticText7Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	if ( !StaticText7Font.Ok() ) StaticText7Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	StaticText7Font.SetPointSize((int)(StaticText7Font.GetPointSize() * 1.000000));
	StaticText7Font.SetWeight(wxBOLD);
	StaticText7->SetFont(StaticText7Font);
	FlexGridSizer2->Add(StaticText7, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer7 = new wxFlexGridSizer(0, 1, wxDLG_UNIT(this,wxSize(0,0)).GetWidth(), 0);
	FlexGridSizer7->AddGrowableCol(0);
	FlexGridSizer13 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer13->AddGrowableCol(0);
	FlexGridSizer14 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer14->AddGrowableCol(0);
	FlexGridSizer14->AddGrowableRow(2);
	FlexGridSizer5 = new wxFlexGridSizer(0, 3, wxDLG_UNIT(this,wxSize(-3,0)).GetWidth(), 0);
	FlexGridSizer5->AddGrowableCol(1);
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("Show:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	FlexGridSizer5->Add(StaticText5, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	chImageType = new wxChoice(this, ID_CHOICE1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
	chImageType->Append(_("Jpeg file"));
	chImageType->Append(_("RAW file"));
	chImageType->Append(_("Calculated image"));
	FlexGridSizer5->Add(chImageType, 1, wxTOP|wxBOTTOM|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer14->Add(FlexGridSizer5, 1, wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	StaticText6 = new wxStaticText(this, ID_STATICTEXT6, _("Areas:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	FlexGridSizer14->Add(StaticText6, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	lbxAreas = new wxListBox(this, ID_LISTBOX1, wxDefaultPosition, wxSize(176,72), 0, 0, 0, wxDefaultValidator, _T("ID_LISTBOX1"));
	FlexGridSizer14->Add(lbxAreas, 1, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer13->Add(FlexGridSizer14, 1, wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
	FlexGridSizer12 = new wxFlexGridSizer(4, 2, 0, 0);
	StaticText15 = new wxStaticText(this, ID_STATICTEXT15, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT15"));
	FlexGridSizer12->Add(StaticText15, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	rbMove = new wxRadioButton(this, ID_RADIOBUTTON2, _("Move"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON2"));
	rbMove->SetValue(true);
	rbMove->SetToolTip(_("rbMove_hint"));
	FlexGridSizer12->Add(rbMove, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	StaticText16 = new wxStaticText(this, ID_STATICTEXT16, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT16"));
	FlexGridSizer12->Add(StaticText16, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	rbResize = new wxRadioButton(this, ID_RADIOBUTTON1, _("Resize"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON1"));
	rbResize->SetToolTip(_("rbResize_hint"));
	FlexGridSizer12->Add(rbResize, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	StaticText13 = new wxStaticText(this, ID_STATICTEXT13, _("X1"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT13"));
	FlexGridSizer12->Add(StaticText13, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	spinX1 = new wxSpinCtrl(this, ID_SPINCTRL3, _T("0"), wxDefaultPosition, wxSize(64,22), 0, 0, 9999, 0, _T("ID_SPINCTRL3"));
	spinX1->SetValue(_T("0"));
	FlexGridSizer12->Add(spinX1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(1,0)).GetWidth());
	StaticText12 = new wxStaticText(this, ID_STATICTEXT12, _("X2"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT12"));
	FlexGridSizer12->Add(StaticText12, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	spinX2 = new wxSpinCtrl(this, ID_SPINCTRL2, _T("0"), wxDefaultPosition, wxSize(64,22), 0, 0, 9999, 0, _T("ID_SPINCTRL2"));
	spinX2->SetValue(_T("0"));
	FlexGridSizer12->Add(spinX2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(1,0)).GetWidth());
	StaticText14 = new wxStaticText(this, ID_STATICTEXT14, _("Y1"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT14"));
	FlexGridSizer12->Add(StaticText14, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	spinY1 = new wxSpinCtrl(this, ID_SPINCTRL4, _T("0"), wxDefaultPosition, wxSize(64,22), 0, 0, 9999, 0, _T("ID_SPINCTRL4"));
	spinY1->SetValue(_T("0"));
	FlexGridSizer12->Add(spinY1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(1,0)).GetWidth());
	StaticText11 = new wxStaticText(this, ID_STATICTEXT11, _("Y2"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT11"));
	FlexGridSizer12->Add(StaticText11, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	spinY2 = new wxSpinCtrl(this, ID_SPINCTRL1, _T("0"), wxDefaultPosition, wxSize(64,22), 0, 0, 9999, 0, _T("ID_SPINCTRL1"));
	spinY2->SetValue(_T("0"));
	FlexGridSizer12->Add(spinY2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(1,0)).GetWidth());
	FlexGridSizer13->Add(FlexGridSizer12, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer7->Add(FlexGridSizer13, 1, wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer3 = new wxFlexGridSizer(0, 4, 0, 0);
	FlexGridSizer3->AddGrowableCol(3);
	btnSaveAreas = new wxButton(this, ID_BUTTON5, _("Save..."), wxDefaultPosition, wxDLG_UNIT(this,wxSize(35,14)), 0, wxDefaultValidator, _T("ID_BUTTON5"));
	btnSaveAreas->Disable();
	FlexGridSizer3->Add(btnSaveAreas, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	btnLoad = new wxButton(this, ID_BUTTON7, _("Load..."), wxDefaultPosition, wxDLG_UNIT(this,wxSize(35,14)), 0, wxDefaultValidator, _T("ID_BUTTON7"));
	btnLoad->Disable();
	FlexGridSizer3->Add(btnLoad, 1, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	btnClearAreas = new wxButton(this, ID_BUTTON8, _("Clear"), wxDefaultPosition, wxDLG_UNIT(this,wxSize(27,14)), 0, wxDefaultValidator, _T("ID_BUTTON8"));
	btnClearAreas->SetHelpText(_("btnClearAreas_hint"));
	FlexGridSizer3->Add(btnClearAreas, 1, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	btnDelArea = new wxButton(this, ID_BUTTON6, _("Delete"), wxDefaultPosition, wxDLG_UNIT(this,wxSize(34,14)), 0, wxDefaultValidator, _T("ID_BUTTON6"));
	btnDelArea->SetHelpText(_("btnDelArea_hint"));
	FlexGridSizer3->Add(btnDelArea, 1, wxTOP|wxBOTTOM|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer7->Add(FlexGridSizer3, 1, wxEXPAND|wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer2->Add(FlexGridSizer7, 1, wxLEFT|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	StaticLine2 = new wxStaticLine(this, ID_STATICLINE2, wxDefaultPosition, wxSize(10,1), wxLI_HORIZONTAL, _T("ID_STATICLINE2"));
	FlexGridSizer2->Add(StaticLine2, 1, wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	StaticText8 = new wxStaticText(this, ID_STATICTEXT8, _("Control"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
	wxFont StaticText8Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	if ( !StaticText8Font.Ok() ) StaticText8Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	StaticText8Font.SetPointSize((int)(StaticText8Font.GetPointSize() * 1.000000));
	StaticText8Font.SetWeight(wxBOLD);
	StaticText8->SetFont(StaticText8Font);
	FlexGridSizer2->Add(StaticText8, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer8 = new wxFlexGridSizer(0, 5, 0, 0);
	btnProcess = new wxButton(this, ID_BUTTON3, _("Process"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
	FlexGridSizer8->Add(btnProcess, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	btnExit = new wxButton(this, ID_BUTTON4, _("Exit"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
	FlexGridSizer8->Add(btnExit, 1, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	btnSave = new wxButton(this, ID_BUTTON2, _("Save"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	btnSave->Disable();
	btnSave->SetForegroundColour(wxColour(0,128,0));
	wxFont btnSaveFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	if ( !btnSaveFont.Ok() ) btnSaveFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	btnSaveFont.SetPointSize((int)(btnSaveFont.GetPointSize() * 1.000000));
	btnSaveFont.SetWeight(wxBOLD);
	btnSave->SetFont(btnSaveFont);
	btnSave->SetToolTip(_(" Save color matrix "));
	FlexGridSizer8->Add(btnSave, 1, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer2->Add(FlexGridSizer8, 1, wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	StaticLine3 = new wxStaticLine(this, ID_STATICLINE3, wxDefaultPosition, wxSize(10,1), wxLI_HORIZONTAL, _T("ID_STATICLINE3"));
	FlexGridSizer2->Add(StaticLine3, 1, wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Progress and results"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	wxFont StaticText4Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	if ( !StaticText4Font.Ok() ) StaticText4Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	StaticText4Font.SetPointSize((int)(StaticText4Font.GetPointSize() * 1.000000));
	StaticText4Font.SetWeight(wxBOLD);
	StaticText4->SetFont(StaticText4Font);
	FlexGridSizer2->Add(StaticText4, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer10 = new wxFlexGridSizer(0, 3, 0, 0);
	StaticText17 = new wxStaticText(this, ID_STATICTEXT17, _("Log level:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT17"));
	FlexGridSizer10->Add(StaticText17, 1, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	chLogLevel = new wxChoice(this, ID_CHOICE2, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE2"));
	chLogLevel->Append(_("Minimal"));
	chLogLevel->SetSelection( chLogLevel->Append(_("Normal")) );
	chLogLevel->Append(_("Verbose"));
	chLogLevel->SetHelpText(_("chLogLevel_hint"));
	FlexGridSizer10->Add(chLogLevel, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	btnClearLog = new wxButton(this, ID_BUTTON9, _("Clear log"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON9"));
	btnClearLog->SetHelpText(_("btnClearLog_hint"));
	FlexGridSizer10->Add(btnClearLog, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	FlexGridSizer2->Add(FlexGridSizer10, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	txtResult = new wxTextCtrl(this, ID_TEXTCTRL3, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL3"));
	FlexGridSizer2->Add(txtResult, 1, wxBOTTOM|wxLEFT|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(10,0)).GetWidth());
	szMain->Add(FlexGridSizer2, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	scrwImage = new wxScrolledWindow(this, ID_SCROLLEDWINDOW1, wxPoint(-1,-1), wxSize(700,700), wxHSCROLL|wxVSCROLL, _T("ID_SCROLLEDWINDOW1"));
	scrwImage->SetMinSize(wxSize(100,100));
	scrwImage->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	szImg = new wxFlexGridSizer(0, 3, 0, 0);
	sbmpImage = new wxStaticBitmap(scrwImage, ID_STATICBITMAP_IMAGE, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICBITMAP_IMAGE"));
	szImg->Add(sbmpImage, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(scrwImage,wxSize(5,0)).GetWidth());
	scrwImage->SetSizer(szImg);
	szImg->SetSizeHints(scrwImage);
	szMain->Add(scrwImage, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	SetSizer(szMain);
	tmrStart.SetOwner(this, ID_TIMER1);
	tmrStart.Start(40, false);
	tmrPaintTimer.SetOwner(this, ID_TIMER2);
	tmrPaintTimer.Start(30, true);
	FileDialog = new wxFileDialog(this, _("Select file"), wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_DEFAULT_STYLE|wxFD_OPEN, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
	szMain->SetSizeHints(this);
	
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CamCalibrFrame::OnSelectRawClick);
	Connect(ID_SLIDER1,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&CamCalibrFrame::OnGammaCmdScroll);
	Connect(ID_SLIDER2,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&CamCalibrFrame::OnGammaCmdScroll);
	Connect(ID_CHOICE1,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&CamCalibrFrame::OnImageTypeSelect);
	Connect(ID_LISTBOX1,wxEVT_COMMAND_LISTBOX_SELECTED,(wxObjectEventFunction)&CamCalibrFrame::OnAreasSelect);
	Connect(ID_SPINCTRL3,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&CamCalibrFrame::OnAreaSpinCoordChange);
	Connect(ID_SPINCTRL2,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&CamCalibrFrame::OnAreaSpinCoordChange);
	Connect(ID_SPINCTRL4,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&CamCalibrFrame::OnAreaSpinCoordChange);
	Connect(ID_SPINCTRL1,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&CamCalibrFrame::OnAreaSpinCoordChange);
	Connect(ID_BUTTON8,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CamCalibrFrame::OnClearAreasClick);
	Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CamCalibrFrame::OnDelAreaClick);
	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CamCalibrFrame::OnProcessClick);
	Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CamCalibrFrame::OnbtnExitClick);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CamCalibrFrame::btnSaveClick);
	Connect(ID_CHOICE2,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&CamCalibrFrame::OnLogLevelSelect);
	Connect(ID_BUTTON9,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CamCalibrFrame::OnClearLogClick);
	Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&CamCalibrFrame::OntmrStartTrigger);
	Connect(ID_TIMER2,wxEVT_TIMER,(wxObjectEventFunction)&CamCalibrFrame::OntmrPaintTimerTrigger);
	//*)

	area_is_selecting_ = false;
	sbmpImage->Connect(ID_STATICBITMAP_IMAGE,wxEVT_LEFT_DOWN,(wxObjectEventFunction)&CamCalibrFrame::OnImageLeftDown,0,this);
	sbmpImage->Connect(ID_STATICBITMAP_IMAGE,wxEVT_MOTION,(wxObjectEventFunction)&CamCalibrFrame::OnImageMouseMove,0,this);
	sbmpImage->Connect(ID_STATICBITMAP_IMAGE,wxEVT_LEFT_UP,(wxObjectEventFunction)&CamCalibrFrame::OnImageLeftUp,0,this);
	sbmpImage->Connect(ID_STATICBITMAP_IMAGE,wxEVT_PAINT,(wxObjectEventFunction)&CamCalibrFrame::OnImagePaint,0,this);


	scrwImage->SetScrollRate(16, 16);
	show_gamma_and_bright();
    log_level = chLogLevel->GetSelection();
}

CamCalibrFrame::~CamCalibrFrame()
{
	//(*Destroy(CamCalibrFrame)
	//*)
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
	szImg->Layout();

	scrwImage->SetSizer(szImg);
	szImg->SetSizeHints(scrwImage);

	szMain->Layout();
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
    log_level = chLogLevel->GetSelection();
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
