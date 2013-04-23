/*=============================================================================
  DNG4PS - software for converting Canon PS RAW files to DNG ones
  Copyright (C) 2007-2013 Denis Artyomov, Glenn Washburn

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
  File:      Languages.cpp
  Author(s): Glenn Washburn (development@efficientek.com)
  ===========================================================================*/

#include "pch.h"

#include <initializer_list>

#include "Languages.h"


const Languages langs = {
    "de",
    "en",
    "es",
    "fi",
    "fr",
    "it",
    "nn",
    "pl",
    "ru",
    "zh",
    /* GENERATE LANGTABL */
};


const SupportedLanguage Languages::find_by_language_id (wxLanguage lang_id) const
{
    for (auto i : *this)
    {
        if (i.langinfo->Language == lang_id)
            return i;
    }
    return *(this->end()-1);
}

const SupportedLanguage Languages::find_by_language_iso_name (wxString lang_iso_name) const
{
    for (auto i : *this)
    {
        if (i.iso_name == lang_iso_name)
            return i;
    }
    return *(this->end()-1);
}
