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
  File:      DNG4PSApp.cpp
  Author(s): Denis Artyomov (denis.artyomov@gmail.com)
  ===========================================================================*/

#include "pch.h"
#include <wx/filename.h>
#include <wx/log.h>

#include "DNG4PSApp.h"
#include "ProgramObjects.h"
#include "Options.h"
#include "CameraOpts.h"

//(*AppHeaders
#include "DNG4PSMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(DNG4PSApp);

wxIcon main_icon;
wxString exe_dir, path_sep;

bool DNG4PSApp::OnInit()
{
	 srand((unsigned)time(NULL));

#if defined(__WIN32__)
	main_icon.LoadFile(L"main_icon", wxBITMAP_TYPE_ICO_RESOURCE);
	path_sep = L"\\";
#else
	path_sep = L"/";
#endif

	exe_dir = wxFileName(argv[0]).GetPath();

	wxFileName lang_path(exe_dir, L"langs");

	sys().options->load();
	sys().init_language(lang_path.GetFullPath());

	sys().cameras->load();
	sys().cameras->set_defaults(false);

	//(*AppInitialize
	bool wxsOK = true;
	wxInitAllImageHandlers();
	if ( wxsOK )
	{
	DNG4PSFrame* Frame = new DNG4PSFrame(0);
	Frame->Show();
	SetTopWindow(Frame);
	}
	//*)

	return wxsOK;
}
