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
  File:      Options.cpp
  Author(s): Denis Artyomov (denis.artyomov@gmail.com)
             Matt Dawson (coder@dawsons.id.au)
  ===========================================================================*/

#include "pch.h"
#include <wx/config.h>
#include "Options.h"

namespace
{
    const wchar_t * app_name_opt_str         = L"dng4ps2";
    const wchar_t * path_opt_str             = L"LastPath";
    const wchar_t * recursive_search_opt_str = L"RecursiveSearch";
    const wchar_t * dont_overwrite_opt_str   = L"DontOverWrite";
    const wchar_t * add_metadata_opt_str     = L"AddMetadata";
    const wchar_t * compress_dng_opt_str     = L"CompressDNG";
    const wchar_t * output_path_opt_str      = L"OutputPath";
    const wchar_t * preview_type_opt_str     = L"PreviewMode";
    const wchar_t * use_date_opt_str         = L"UseDateForPath";
    const wchar_t * date_type_opt_str        = L"DateType";
    const wchar_t * artist_opt_str           = L"Artist";
    const wchar_t * use_artist_opt_str       = L"UseArtist";
    const wchar_t * lang_opt_str             = L"Language";
    const wchar_t * last_camera_id_opt_str   = L"LastCameraId";
};

Options::Options()
{
    recursive_search = false;
    dont_overwrite = true;
    add_metadata = true;
    compress_dng = true;
    preview_type = pt_Full;
    use_date_for_path = true;
    date_type = dt_YYYY_MM_DD;
    use_artist = false;
    lang = wxLANGUAGE_ENGLISH;

}

// Options::load
void Options::load()
{
    wxConfig config(app_name_opt_str);

    config.Read(recursive_search_opt_str, &recursive_search,  recursive_search );
    config.Read(dont_overwrite_opt_str,   &dont_overwrite,    dont_overwrite);
    config.Read(add_metadata_opt_str,     &add_metadata,      add_metadata);
    config.Read(compress_dng_opt_str,     &compress_dng,      compress_dng);
    config.Read(use_date_opt_str,         &use_date_for_path, use_date_for_path);
    config.Read(use_artist_opt_str,       &use_artist,        use_artist);

    path_to_files  = config.Read(path_opt_str,             wxEmptyString   );
    output_path    = config.Read(output_path_opt_str,      wxEmptyString   );
    artist         = config.Read(artist_opt_str,           wxEmptyString   );

    preview_type   = (PreviewType) config.Read(preview_type_opt_str, pt_Full);

    date_type      = (DateTypes)config.Read(date_type_opt_str, date_type);
    lang           = (wxLanguage)config.Read(lang_opt_str, lang);

	last_camera_id = config.Read(last_camera_id_opt_str,   wxEmptyString   );

}

// Options::save
void Options::save() const
{
    wxConfig config(app_name_opt_str);

    config.Write(path_opt_str,             path_to_files    );
    config.Write(output_path_opt_str,      output_path      );
    config.Write(recursive_search_opt_str, recursive_search );
    config.Write(dont_overwrite_opt_str,   dont_overwrite   );
    config.Write(add_metadata_opt_str,     add_metadata     );
    config.Write(compress_dng_opt_str,     compress_dng     );
    config.Write(preview_type_opt_str,     preview_type     );
    config.Write(use_date_opt_str,         use_date_for_path);
    config.Write(date_type_opt_str,        date_type        );
    config.Write(artist_opt_str,           artist           );
    config.Write(use_artist_opt_str,       use_artist       );
    config.Write(lang_opt_str,             lang             );
	config.Write(last_camera_id_opt_str,   last_camera_id);
}
