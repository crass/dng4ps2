#ifndef FILELIST_H_INCLUDED
#define FILELIST_H_INCLUDED

class wxFileName;

struct FileListItem
{
	wxString name, path_and_name, jpeg, jpeg_file_name;
	unsigned int size;
	bool have_jpeg;
};

class FileList
{
private:
	typedef std::vector<FileListItem> Items;
	Items items;

public:
	void scan(const wxString & dir, bool recursive, const std::function<void(int)> & on_progress);
	size_t size() const;
	void clear();
	void addOneFile(wxString);

	const FileListItem& operator[](int index) const;
};

#endif // FILELIST_H_INCLUDED
