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
  File:      FileList.cpp
  Author(s): Denis Artyomov (denis.artyomov@gmail.com)
  ===========================================================================*/

#include "pch.h"

#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/file.h>

#include "ProgramObjects.h"
#include "CameraOpts.h"
#include "FileList.h"
#include "Utils.h"

// FileList::scan
void FileList::scan(const wxString & dir, bool recursive, const boost::function<void(int)> & on_progress)
{
	wxArrayString list;
	FileListItem item;
	std::vector<size_t> sizes;

	sys().cameras->enum_file_sizes(sizes);

	items.clear();

	if (dir.IsEmpty()) return;

	wxDir::GetAllFiles(dir, &list, wxEmptyString, recursive ? wxDIR_DIRS|wxDIR_FILES : wxDIR_FILES);

	int count = list.Count();
	for (int i = 0; i < count; i++)
	{
		wxString file_name = list[i];
		wxFileName name(file_name);

		if (on_progress && (i % 16 == 0)) on_progress(100*i/count);

		size_t file_size = name.GetSize().ToULong();

		if ( !std::binary_search(sizes.begin(), sizes.end(), file_size) ) continue;

		wxString ext = name.GetExt().Lower();
		if (ext == L"jpg")
		{
			wxFile file(file_name);
			if (file.Error()) continue;
			char buffer[3] = {0, 0, 0};
			static const char jpg_hdr[] = {0xFF, 0xD8, 0xFF};
			file.Read(buffer, 3);
			if (memcmp(buffer, jpg_hdr, 3) == 0) continue;
		}

		item.path_and_name = name.GetFullPath();
		item.name = name.GetFullName();
		item.size = file_size;

		wxFileName jpeg_name;

		item.have_jpeg = Utils::get_jpeg_name(name, jpeg_name);
		item.jpeg = item.have_jpeg ? jpeg_name.GetFullName() : L"-";
		item.jpeg_file_name = item.have_jpeg ? jpeg_name.GetFullPath() : wxString();

		items.push_back(item);
	}
}

// FileList::size
size_t FileList::size() const
{
	return items.size();
}

// FileList::operator[]
const FileListItem& FileList::operator[](int index) const
{
	return items.at(index);
}
