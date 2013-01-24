#ifndef CAMERAOPTS_H_INCLUDED
#define CAMERAOPTS_H_INCLUDED

class wxConfigBase;

enum MosaicType
{
	MOSAIC_RGGB = 1,
	MOSAIC_GBRG = 3
};

struct CameraColorMatrix
{
	int illum;
	std::vector<float> matrix;
	std::vector<float> mult;
};

enum BitsPerUnit
{
	ppc_10 = 10,
	ppc_12 = 12
};

struct CameraData
{
	wxString id;
	wxString model_name;
	wxString short_name;

	unsigned int width;
	unsigned int height;

	unsigned int active_origin_x;
	unsigned int active_origin_y;
	unsigned int active_origin_right;
	unsigned int active_origin_bottom;

	unsigned int cropped_width;
	unsigned int cropped_height;

	unsigned int black_level;
	unsigned int white_level;
	MosaicType mosaic;

	CameraColorMatrix matrix1;
	CameraColorMatrix matrix2;

	BitsPerUnit bits_per_unit;
};

class CameraOpts
{
public:
	CameraOpts();
	void set_defaults(bool clear_all);

	size_t get_size() const;
	CameraData& at(size_t index);
	const CameraData& at(size_t index) const;

	const CameraData* find_by_id(const wxString &id) const;
	const CameraData* find_by_name(const wxString &name) const;
	const CameraData* find_by_file_size(unsigned int size) const;
	void reset_to_defaults(const wxString &id);
	void reset_to_defaults(const wxString &id, CameraData &item);

	void set_item(const wxString &id, const CameraData &item);

	void sort_by_name();

	void save();
	void save(wxConfigBase &config, bool save_groups);

	void load();
	void load(wxConfigBase &config);

	static wxString vct_to_str(const std::vector<float> &vct, const wchar_t *format);
	static void str_to_vect(std::vector<float> &vct, const wxString &txt, size_t count);
	static unsigned int get_file_size(const CameraData &camera);
	void enum_file_sizes(std::vector<size_t> &items);

	void add(const CameraData &item, wxString *new_id = NULL);
	void erase(const wxString& id);

	void clear_groups();
	void add_group(unsigned int size, const wxString &id);
	wxString get_group(unsigned int size) const;

private:
	typedef std::vector<CameraData> Items;
	typedef std::map<unsigned int, wxString> Groups;

	Items items_;
	Groups file_sizes_;

	static void save_matrix(wxConfigBase &config, const wxString &path, const CameraColorMatrix &matrix);
	static void load_matrix(wxConfigBase &config, const wxString &path, CameraColorMatrix &matrix);

};

#endif // CAMERAOPTS_H_INCLUDED
