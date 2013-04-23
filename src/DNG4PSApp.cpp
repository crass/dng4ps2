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

  /* Command line operation

  dng4ps-2 [-d <directory] [-p0 | -p1 | -p2] input-filename...

  If any parameter is present, the program will run in command line.
  <input-filename> is the FULL path to one or more raw files to be processed. At least one is mandatory
  -d directory  Base output directory path
  -p0           No preview
  -p1           Medium preview
  -p2           Full preview
  -c            Compressed SNG file
  -u            Uncompressed DNG file

  All other parameters are taken from the users dng4ps2 settings as set in the GUI
  If an option is not specified, then the default is take from the users dng4ps2 settings as set in the GUI

  Note: Where possible using the same parameter spec as Adobe DNG Converter are used
  http://www.adobe.com/content/dam/Adobe/en/products/photoshop/pdfs/dng_commandline.pdf

  TODO: Better error handling and logging
  TODO: Support -dng1.1, -dng1.3, -e, and -l options

  */

#include "pch.h"

#include "DNG4PSApp.h"
#include "ProgramObjects.h"
#include "Options.h"
#include "CameraOpts.h"

#include "gui/DNG4PSMain.h"

#include <iostream>
#include <iterator>
// -----------------
using namespace std;


wxIMPLEMENT_APP(DNG4PSApp);

wxIcon main_icon;
wxString exe_dir, path_sep;

bool DNG4PSApp::OnInit()
{
	ProgramObjects &sys = ::sys();
	srand((unsigned)time(NULL));

#if defined(__WIN32__)
	main_icon.LoadFile(L"main_icon", wxBITMAP_TYPE_ICO_RESOURCE);
	path_sep = L"\\";
#else
	path_sep = L"/";
#endif

	exe_dir = wxFileName(argv[0]).GetPath();
	commandLineMode = (argc > 1);

	wxLog *logger=new wxLogStderr();
	wxLog::SetActiveTarget(logger);

	wxFileName lang_path(exe_dir, wxT("langs"));

	sys.langs_path = lang_path.GetFullPath();
	sys.options->load();
	sys.init_language();

	sys.cameras->load();
	sys.cameras->set_defaults(false);


	//(*AppInitialize
	bool wxsOK = true;
	wxInitAllImageHandlers();
	if ( wxsOK )
	{
        if (commandLineMode) //Are we running the command line version?
        {

            wxCmdLineParser parser(argc, argv);
            wxString output_dir = _T("");
            wxString output_filename = _T("");
            wxArrayString files;

            static const wxCmdLineEntryDesc cmdLineDesc[] =
            {
                { wxCMD_LINE_SWITCH, "h",  "help",         "Display this help message", wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
                { wxCMD_LINE_OPTION, "d",  "outputdir",    "Base output directory" },
                { wxCMD_LINE_SWITCH, "p0", "nopreview",    "No preview" },
                { wxCMD_LINE_SWITCH, "p1", "medium",       "Medium Preview" },
                { wxCMD_LINE_SWITCH, "p2", "full",         "Full preview" },
                { wxCMD_LINE_SWITCH, "u",  "uncompressed", "Uncompressed DNG file" },
                { wxCMD_LINE_SWITCH, "c",  "compressed",   "Compressed DNG file" },
                { wxCMD_LINE_SWITCH, "m",  "meta",         "Add meta data from jpeg" },
                { wxCMD_LINE_SWITCH, "n",  "nooverwrite",  "Don't overwrite DNG files" },
                { wxCMD_LINE_OPTION, "o",  "outputfile",   "Not used. Reserved for future use" },

                { wxCMD_LINE_PARAM,  NULL, NULL, "input files", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE },
                { wxCMD_LINE_NONE }
            };

            parser.DisableLongOptions();
            parser.SetDesc(cmdLineDesc);
            if( parser.Parse() == 0 )
            {
                files.clear();
                // Default to overwriting DNG because we only have a commandline
                // option for "not" overwriting DNG files.
                sys.options->dont_overwrite = false;

                wxString s = _T("DNG4PS-2 command line mode:\nInput files: ");  //message string

                size_t count = parser.GetParamCount();
                if (count > 0) // must be at least one filename
                {
                    for ( size_t param = 0; param < count; param++ )
                    {
                        files.Add(parser.GetParam(param));
                        s << parser.GetParam(param) << ' ';
                    }
                    s << '\n';

                    if ( parser.Found(_T("o"), &output_filename) )
                    {
                        s << _T("Output filname IGNORED:\t") << output_filename << '\n';
                    }

                    if ( parser.Found(_T("d"), &output_dir) )
                    {
                        s << _T("Output dir:\t") << output_dir << '\n';
                    }

                    if ( parser.Found(_T("p0")))
                    {
                        sys.options->preview_type=pt_None;
                    } else if ( parser.Found(_T("p1")))
                    {
                        sys.options->preview_type=pt_Medium;
                    } else if ( parser.Found(_T("p2")))
                    {
                        sys.options->preview_type=pt_Full;
                    }

                    if ( parser.Found(_T("u")))
                    {
                        sys.options->compress_dng=false;
                    } else if ( parser.Found(_T("c")))
                    {
                        sys.options->compress_dng=true;
                    }

                    if ( parser.Found(_T("m")))
                    {
                        sys.options->add_metadata = true;
                    }

                    if ( parser.Found(_T("n")))
                    {
                        sys.options->dont_overwrite = true;
                    }

                    wxLogMessage(s);
                    DNG4PSFrame::convertFiles(files, output_dir);
                }
                else
                {
                    wxsOK = false;
                }
            }
            else
            {
                wxsOK = false;
            }
        }
        else //otherwise use GUI version
        {
            wxLog *logger=new wxLogGui();
            wxLog *oldlogger = wxLog::SetActiveTarget(logger);
            if (oldlogger)
                delete oldlogger;
            
            DNG4PSFrame* Frame = new DNG4PSFrame(0);
            Frame->Show();
            SetTopWindow(Frame);
        }
	}
	//*)

	return wxsOK;
}

int DNG4PSApp::OnRun()
{
    // If run in commandline mode, then we're already done processing the
    // files.  So don't enter into main loop.
    if (commandLineMode)
        return 0;
    
    return wxApp::OnRun();
}
