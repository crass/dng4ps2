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
  File:      ProgramObjects.cpp
  Author(s): Denis Artyomov (denis.artyomov@gmail.com)
  ===========================================================================*/

#include "pch.h"

#include "Options.h"
#include "CameraOpts.h"
#include "ProgramObjects.h"

ProgramObjects& sys()
{
    static ProgramObjects obj;
    return obj;
}

ProgramObjects::ProgramObjects() :
    options(new Options   ),
    cameras(new CameraOpts)
{
}

// ProgramObjects::init_language
void ProgramObjects::init_language(const wxString & dir)
{
	//wxLanguage lang = wxLANGUAGE_ENGLISH;

	locale_.Init(options->lang, options->lang, options->lang);
    wxLocale::AddCatalogLookupPathPrefix(dir);
    locale_.AddCatalog(L"dng4ps-2");
}
