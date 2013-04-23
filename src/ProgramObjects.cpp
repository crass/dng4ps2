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
    cameras(new CameraOpts),
    locale_(nullptr)
{
}

// ProgramObjects::init_language
void ProgramObjects::init_language()
{
    if (locale_)
        delete locale_;
    locale_ = new wxLocale();
    
    if ( !locale_->Init(options->lang, wxLOCALE_DONT_LOAD_DEFAULT) )
    {
        wxLogWarning(_("This language is not supported by the system."));
        // continue nevertheless
    }

    wxLocale::AddCatalogLookupPathPrefix(langs_path);
    
    // Initialize the catalogs we'll be using
    const wxLanguageInfo* pInfo = wxLocale::GetLanguageInfo(options->lang);
    if (!locale_->AddCatalog(wxT("dng4ps-2")))
    {
        wxLogError(_("Couldn't find/load the 'internat' catalog for locale '%s'."),
                   pInfo ? pInfo->GetLocaleName() : _("unknown"));
    }
    
    // Now try to add wxstd.mo so that loading "NOTEXIST.ING" file will produce
    // a localized error message:
    locale_->AddCatalog("wxstd");
        // NOTE: it's not an error if we couldn't find it!

    // this catalog is installed in standard location on Linux systems and
    // shows that you may make use of the standard message catalogs as well
    //
    // if it's not installed on your system, it is just silently ignored
#if UNIX_ENV
    {
        wxLogNull noLog;
        locale_->AddCatalog("fileutils");
    }
#endif
}
