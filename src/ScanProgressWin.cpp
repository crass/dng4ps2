#include "pch.h"
#include "ScanProgressWin.h"

//(*InternalHeaders(ScanProgressWin)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(ScanProgressWin)
const long ScanProgressWin::ID_STATICTEXT1 = wxNewId();
const long ScanProgressWin::ID_GAUGE1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(ScanProgressWin,wxDialog)
	//(*EventTable(ScanProgressWin)
	//*)
END_EVENT_TABLE()

ScanProgressWin::ScanProgressWin(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(ScanProgressWin)
	wxStaticText* StaticText1;
	
	Create(parent, id, _("Wait"), wxDefaultPosition, wxDefaultSize, wxCAPTION, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("ScanInProgress"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	gaugProgress = new wxGauge(this, ID_GAUGE1, 100, wxDefaultPosition, wxDLG_UNIT(this,wxSize(185,15)), 0, wxDefaultValidator, _T("ID_GAUGE1"));
	FlexGridSizer1->Add(gaugProgress, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);
	Center();
	//*)
}

ScanProgressWin::~ScanProgressWin()
{
	//(*Destroy(ScanProgressWin)
	//*)
}

void ScanProgressWin::set_progress(int percent)
{
	gaugProgress->SetValue(percent);
}

