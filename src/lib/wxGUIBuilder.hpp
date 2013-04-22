#pragma once

#include <vector>
#include <functional>
#include <stdint.h>
#include <wx/string.h>

// fwd.
class wxObject;
class wxSizer;
class wxWindow;
class wxString;

namespace gb {

class UIElem; typedef std::vector<UIElem> UIElems;
class Layout;

class BuildOptions;

typedef std::function <wxObject* (const BuildOptions &build_obptions, wxObject *parent, wxSizer *sizer, const UIElems &items)> CreateWidgetFun;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class UIElemOptions
{
public:
	UIElemOptions(uint32_t flags = 0, int border = -1, int width = -1, int height = -1) : 
		flags_(flags), 
		border_(border),
		width_(width),
		height_(height)
	{}

	bool has_flag(const UIElemOptions &flag) const { return (flags_ &  flag.flags_) != 0; }
	int get_border(int default_value) const { return (border_  == -1) ? default_value : border_; }
	int get_width() const { return width_; }
	int get_height() const { return height_; }

	uint32_t get_style() const;

	UIElemOptions operator | (const UIElemOptions &other) const;

private:
	uint32_t flags_;
	int border_;
	int width_;
	int height_;
};

const UIElemOptions bord_top       = UIElemOptions(0x00000001);
const UIElemOptions bord_bottom    = UIElemOptions(0x00000002);
const UIElemOptions bord_left      = UIElemOptions(0x00000004);
const UIElemOptions bord_right     = UIElemOptions(0x00000008);
const UIElemOptions bord_all       = UIElemOptions(0x00000010);
const UIElemOptions bord_all_exc_bottom = UIElemOptions(0x00000001|0x00000004|0x00000008);
const UIElemOptions bord_all_exc_top = UIElemOptions(0x00000002|0x00000004|0x00000008);

const UIElemOptions align_left     = UIElemOptions(0x00000020);
const UIElemOptions align_right    = UIElemOptions(0x00000040);
const UIElemOptions align_hcenter  = UIElemOptions(0x00000080);
const UIElemOptions align_top      = UIElemOptions(0x00000100);
const UIElemOptions align_bottom   = UIElemOptions(0x00000200);
const UIElemOptions align_vcenter  = UIElemOptions(0x00000400);

const UIElemOptions font_bold      = UIElemOptions(0x00000800);
const UIElemOptions font_italic    = UIElemOptions(0x00001000);
const UIElemOptions font_underline = UIElemOptions(0x00002000);

const UIElemOptions expand         = UIElemOptions(0x00004000);
const UIElemOptions stretch        = UIElemOptions(0x00008000);
const UIElemOptions noborder       = UIElemOptions(0x00010000);

const UIElemOptions vscroll        = UIElemOptions(0x00020000);
const UIElemOptions hscroll        = UIElemOptions(0x00040000);

UIElemOptions border(int border);
UIElemOptions width(int value);
UIElemOptions height(int value);
UIElemOptions size(int width, int height);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class BuildOptions
{
public:
	explicit BuildOptions(int default_border = 2) : default_border_(default_border) {}

	int get_default_border() const { return default_border_; }

private:
	int default_border_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class UIElem
{
public:
	UIElem(const CreateWidgetFun &create_fun, const UIElemOptions &options, const wxString &name = wxEmptyString) : 
		create_fun_(create_fun),
		options_(options),
		name_(name)
	{}

	template <typename T> UIElem& operator >> (T & var);
	void set_item(const UIElem &item);
	void set_items(const UIElems &items);
	wxObject* build(const BuildOptions &build_options = BuildOptions(), wxObject *parent = nullptr, wxSizer *sizer = nullptr) const;
	wxString get_name() const { return name_; }

	const UIElemOptions& get_options() const { return options_; }

	UIElem& operator [] (const UIElem &elem);
	UIElem& operator [] (const UIElems &elems);

private:
	UIElems sub_items_;
	CreateWidgetFun create_fun_;
	UIElemOptions options_;
	wxString name_;
};


template <typename T>
UIElem& UIElem::operator >> (T & var) 
{ 
	auto old_create_object_fun = create_fun_;
	auto *var_ptr = &var;
	create_fun_ = [=] (const BuildOptions &build_options, wxObject *parent, wxSizer *sizer, const UIElems &items) 
	{
		auto obj = old_create_object_fun(build_options, parent, sizer, items);
		*var_ptr = dynamic_cast<T>(obj);
		assert(*var_ptr);
		return obj;
	};
	return *this; 
}

UIElems operator , (UIElem elem1, UIElem elem2);
UIElems operator , (UIElems elems, UIElem elem);
UIElems operator , (UIElems elems1, UIElems elems2);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Window
{
public:
	Window(const CreateWidgetFun &create_fun, const UIElem &layout, const UIElemOptions &options, const wxString &name = wxEmptyString) : 
		create_fun_(create_fun), 
		layout_(layout),
		options_(options),
		name_(name)
	{}

	UIElem operator [] (const UIElem &elem);
	UIElem operator [] (const UIElems &elems);

private:
	UIElem layout_;
	const CreateWidgetFun create_fun_;
	UIElemOptions options_;
	wxString name_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
class ExtraFlagsOptions
{
public:
	ExtraFlagsOptions(const uint32_t &flags = 0) : flags_(flags) {}
	explicit ExtraFlagsOptions(const UIElemOptions &elem_options) : elem_options_(elem_options), flags_(0) {}
	explicit ExtraFlagsOptions(const UIElemOptions &elem_options, uint32_t flags) : elem_options_(elem_options), flags_(flags) {}

	uint32_t get_flags() const { return flags_; }
	bool has_flag(const ExtraFlagsOptions<T> &flag) const { return (flags_ & flag.flags_) != 0; }
	const UIElemOptions& get_elem_options() const { return elem_options_; }

protected:
	uint32_t flags_;
	UIElemOptions elem_options_;
};

template <typename T>
ExtraFlagsOptions<T> operator | (const ExtraFlagsOptions<T> &opt1, const ExtraFlagsOptions<T> &opt2)
{
	return ExtraFlagsOptions<T>(opt1.get_elem_options() | opt2.get_elem_options(), opt1.get_flags() | opt2.get_flags());
}

template <typename T>
ExtraFlagsOptions<T> operator | (const UIElemOptions &opt1, const ExtraFlagsOptions<T> &opt2)
{
	return ExtraFlagsOptions<T>(opt1 | opt2.get_elem_options(), opt2.get_flags());
}

template <typename T>
ExtraFlagsOptions<T> operator | (const ExtraFlagsOptions<T> &opt1, const UIElemOptions &opt2)
{
	return ExtraFlagsOptions<T>(opt1.get_elem_options() | opt2, opt1.get_flags());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GridOptions
{
public:
	void add_col(int col) { growable_cols_.push_back(col); }

	const std::vector<int>& get_growable_cols() const { return growable_cols_; }
	const std::vector<int>& get_growable_rows() const { return growable_rows_; }

private:
	std::vector<int> growable_cols_;
	std::vector<int> growable_rows_;
};

GridOptions growable_cols(int col1);
GridOptions growable_cols(int col1, int col2);
GridOptions growable_cols(int col1, int col2, int col3);
GridOptions growable_cols(int col1, int col2, int col3, int col4);
GridOptions growable_cols(int col1, int col2, int col3, int col4, int col5);

UIElem grid(int cols, int rows = 0, const UIElemOptions &options = expand, const GridOptions &grid_options = GridOptions(), int hgap = 0, int vgap = 0);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UIElem item(const wxString &text);

UIElem hbox(const UIElemOptions &options = expand);
UIElem vbox(const UIElemOptions &options = expand);
UIElem dlg_buttons(const UIElemOptions &options = align_right);
UIElem dlg_buttons_ok_cancel(const UIElemOptions &options = align_right);

Window existing_window(wxWindow *window, const UIElemOptions &options = UIElemOptions(), const UIElem &layout = vbox());

UIElem hline(const UIElemOptions &options = expand);
UIElem spacer(int size = 0);
UIElem spring();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UIElem text(const wxString &text, const UIElemOptions &options = UIElemOptions());

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct EditOptionsWrapper{}; typedef ExtraFlagsOptions<EditOptionsWrapper> EditOptions;

const auto ed_multiline = EditOptions(0x00000001);
const auto ed_readonly  = EditOptions(0x00000002);
const auto ed_rich      = EditOptions(0x00000004);
const auto ed_autourl   = EditOptions(0x00000008);
const auto ed_password  = EditOptions(0x00000010);
const auto ed_left      = EditOptions(0x00000020);
const auto ed_centre    = EditOptions(0x00000040);
const auto ed_right     = EditOptions(0x00000080);

UIElem edit(const EditOptions &edit_options = EditOptions());
UIElem edit(const UIElemOptions &options);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ListOptionsWrapper{}; typedef ExtraFlagsOptions<ListOptionsWrapper> ListOptions;

const auto l_list            = ListOptions(0x00000001);
const auto l_report          = ListOptions(0x00000002);
const auto l_virtual         = ListOptions(0x00000004);
const auto l_icon            = ListOptions(0x00000008);
const auto l_small_icon      = ListOptions(0x00000010);
const auto l_align_top       = ListOptions(0x00000020);
const auto l_align_left      = ListOptions(0x00000040);
const auto l_autoarrange     = ListOptions(0x00000080);
const auto l_edit_labels     = ListOptions(0x00000100);
const auto l_no_header       = ListOptions(0x00000200);
const auto l_single_sel      = ListOptions(0x00000400);
const auto l_sort_ascending  = ListOptions(0x00000800);
const auto l_sort_descending = ListOptions(0x00001000);
const auto l_hrules          = ListOptions(0x00002000);
const auto l_vrules          = ListOptions(0x00004000);

UIElem list(const ListOptions& list_options = ListOptions());
UIElem list(const UIElemOptions &options);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ButtonOptionsWrapper{}; typedef ExtraFlagsOptions<ButtonOptionsWrapper> ButtonOptions;

auto const bu_left     = ButtonOptions(0x00000001);
auto const bu_right    = ButtonOptions(0x00000002);
auto const bu_top      = ButtonOptions(0x00000004);
auto const bu_bottom   = ButtonOptions(0x00000008);
auto const bu_exactfit = ButtonOptions(0x00000010);
auto const bu_notext   = ButtonOptions(0x00000020);

UIElem button(const wxString &text, int id, bool is_default = false, const ButtonOptions &options = ButtonOptions());
UIElem button(const wxString &text, const ButtonOptions &options = ButtonOptions());
UIElem button(const wxString &text, const UIElemOptions &options);

UIElem button_ok(const wxString &text = wxEmptyString, const UIElemOptions &options = UIElemOptions());
UIElem button_cancel(const wxString &text = wxEmptyString, const UIElemOptions &options = UIElemOptions());

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UIElem check_box(const wxString &text, const UIElemOptions &options = UIElemOptions());

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UIElem choice(const UIElemOptions &options = UIElemOptions());

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UIElem dir_ctrl(const UIElemOptions &options = UIElemOptions(), bool dirs_only = true);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UIElem image(const UIElemOptions &options = UIElemOptions());

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Window scroll_box(const UIElemOptions &options = UIElemOptions(), const UIElem &layout = vbox());

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UIElem notebook(const UIElemOptions &options = UIElemOptions(), const UIElem &layout = vbox());

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Window page(const wxString &name, const UIElemOptions &options = UIElemOptions(), const UIElem &layout = vbox());

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UIElem gauge(int range, const UIElemOptions &options = UIElemOptions()); // TODO: gauge options

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UIElem slider(int min, int max, const UIElemOptions &options = UIElemOptions()); // TODO: slider options

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ListboxOptionsWrapper{}; typedef ExtraFlagsOptions<ListboxOptionsWrapper> ListboxOptions;

const auto lb_single    = ListboxOptions(0x00000001);
const auto lb_multiple  = ListboxOptions(0x00000002);
const auto lb_extended  = ListboxOptions(0x00000004);
const auto lb_hscroll   = ListboxOptions(0x00000008);
const auto lb_always_sb = ListboxOptions(0x00000010);
const auto lb_needed_sb = ListboxOptions(0x00000020);
const auto lb_no_sb     = ListboxOptions(0x00000040);
const auto lb_sort      = ListboxOptions(0x00000080);

UIElem listbox(const ListboxOptions &options = ListboxOptions());
UIElem listbox(const UIElemOptions &options);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UIElem radio(const wxString &text, bool group_start = false, const UIElemOptions &options = UIElemOptions());

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct SpinOptionsWrapper{}; typedef ExtraFlagsOptions<SpinOptionsWrapper> SpinOptions;

const auto sp_arrow_keys    = SpinOptions(0x0001);
const auto sp_wrap          = SpinOptions(0x0002);
const auto sp_process_enter	= SpinOptions(0x0004);
const auto sp_align_left	= SpinOptions(0x0008);
const auto sp_align_centre	= SpinOptions(0x0010);
const auto sp_align_right	= SpinOptions(0x0020);

UIElem spin(int min, int max, const SpinOptions &options = SpinOptions());
UIElem spin(int min, int max, const UIElemOptions &options);

} // namespace gb
