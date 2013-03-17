#include "pch.h"

#include "ScanProgressWin.h"
#include "lib/wxGUIBuilder.hpp"


ScanProgressWin::ScanProgressWin(wxWindow* parent)
{
	using namespace gb;

	Create(parent, wxID_ANY, _("Wait"), wxDefaultPosition, wxDefaultSize, wxCAPTION, _T("id"));

	auto gui = existing_window(this)
	[
		vbox()
		[
			text("ScanInProgress"),
			gauge(100, size(185,15)) >> gaugProgress
		]
	];

	gui.build();

	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
	Center();
}

void ScanProgressWin::set_progress(int percent)
{
	gaugProgress->SetValue(percent);
}

