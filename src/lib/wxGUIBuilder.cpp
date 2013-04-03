#include <assert.h>

#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/dirctrl.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/gauge.h>
#include <wx/slider.h>
#include <wx/listbox.h>
#include <wx/radiobut.h>
#include <wx/spinctrl.h>

#include "wxGUIBuilder.hpp"

namespace gb {

class Item : public wxObject
{
public:
	Item(const wxString &text, const wxBitmap &image) : text_(text), image_(image) {}

	wxString get_text() const { return text_; }
	wxBitmap get_image() const { return image_; }

private:
	wxString text_;
	wxBitmap image_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UIElemOptions UIElemOptions::operator | (const UIElemOptions &other) const
{
	uint32_t flags = flags_ | other.flags_;
	int border = std::max(border_, other.border_);
	int width = std::max(width_, other.width_);
	int height = std::max(height_, other.height_);
	return UIElemOptions(flags, border, width, height);
}

uint32_t UIElemOptions::get_style() const
{
	uint32_t result = 0;
	if (has_flag(noborder)) result |= wxNO_BORDER;
	return result;
}

UIElemOptions border(int border)
{
	return UIElemOptions(0, border);
}

UIElemOptions width(int value)
{
	return UIElemOptions(0, -1, value);
}

UIElemOptions height(int value)
{
	return UIElemOptions(0, -1, -1, value);
}

UIElemOptions size(int width, int height)
{
	return UIElemOptions(0, -1, width, height);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UIElem::set_item(const UIElem &item)
{
	assert(sub_items_.empty());
	sub_items_.push_back(item);
}

void UIElem::set_items(const UIElems &items)
{
	assert(sub_items_.empty());
	sub_items_ = items;
}

UIElem& UIElem::operator [] (const UIElem &elem)
{
	set_item(elem);
	return *this;
}

UIElem& UIElem::operator [] (const UIElems &elems)
{
	set_items(elems);
	return *this;
}

wxObject* UIElem::build(const BuildOptions &build_options, wxObject *parent, wxSizer *sizer) const
{
	if (create_fun_ == nullptr) return nullptr;
	return create_fun_(build_options, parent, sizer, sub_items_);
}

UIElems operator , (UIElem &elem1, UIElem &elem2)
{
	UIElems res;
	res.push_back(elem1);
	res.push_back(elem2);
	return res;
}

UIElems& operator , (UIElems &elems, UIElem &elem)
{
	elems.push_back(elem);
	return elems;
}

UIElems& operator , (UIElems &elems1, UIElems &elems2)
{
	elems1.insert(elems1.end(), elems2.begin(), elems2.end());
	return elems1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UIElem Window::operator [] (const UIElem &elem)
{
	UIElem result(create_fun_, options_, name_);
	result.set_item(layout_[elem]);
	return result;
}

UIElem Window::operator [] (const UIElems &elems)
{
	UIElem result(create_fun_, options_, name_);
	result.set_item(layout_[elems]);
	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UIElem item(const wxString &text)
{
	return UIElem([=] (const BuildOptions &build_options, wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* {
		return new Item(text, wxNullBitmap);
	}, UIElemOptions());
}

uint32_t get_sizer_item_flags(const UIElemOptions &options)
{
	uint32_t result = 0;
	if (options.has_flag(align_right))   result |= wxALIGN_RIGHT;
	if (options.has_flag(align_left))    result |= wxALIGN_LEFT;
	if (options.has_flag(align_top))     result |= wxALIGN_TOP;
	if (options.has_flag(align_bottom))  result |= wxALIGN_BOTTOM;
	if (options.has_flag(align_hcenter)) result |= wxALIGN_CENTER_HORIZONTAL;
	if (options.has_flag(align_vcenter)) result |= wxALIGN_CENTER_VERTICAL;
	if (options.has_flag(bord_left))     result |= wxLEFT;
	if (options.has_flag(bord_right))    result |= wxRIGHT;
	if (options.has_flag(bord_top))      result |= wxTOP;
	if (options.has_flag(bord_bottom))   result |= wxBOTTOM;
	if (options.has_flag(bord_all))      result |= wxALL;
	if (options.has_flag(expand))        result |= wxEXPAND;
	if (options.has_flag(vscroll))       result |= wxVSCROLL;
	if (options.has_flag(hscroll))       result |= wxHSCROLL;

	if (!options.has_flag(bord_left) && 
		!options.has_flag(bord_right) && 
		!options.has_flag(bord_top) && 
		!options.has_flag(bord_bottom) && 
		!options.has_flag(bord_all)) result |= wxALL;

	if (!options.has_flag(align_top) && 
		!options.has_flag(align_bottom) && 
		!options.has_flag(align_vcenter)) result |= wxALIGN_CENTER_VERTICAL;

	return result;
}

int get_sizer_item_proportion(const UIElemOptions &options)
{
	return options.has_flag(stretch) ? 1 : 0;
}

void process_sizer_items(const BuildOptions &build_options, wxObject *parent, wxSizer *sizer_obj, const UIElems &items)
{
	int default_border = build_options.get_default_border();
	for (auto &item : items)
	{
		wxObject *sub_item_obj = item.build(build_options, parent, sizer_obj);
		wxWindow *parent_window = dynamic_cast<wxWindow*>(parent);
		if (sub_item_obj == nullptr)
		{
			if (item.get_options().has_flag(stretch))
				sizer_obj->AddStretchSpacer();
			else
			{
				int spacer_size = parent_window->ConvertDialogToPixels(wxSize(item.get_options().get_width(), 0)).x;
				sizer_obj->AddSpacer(spacer_size);
			}
		}
		else
		{
			wxWindow *sub_item_window = dynamic_cast<wxWindow*>(sub_item_obj);
			wxSizer *sub_item_sizer = dynamic_cast<wxSizer*>(sub_item_obj);
			assert(sub_item_window || sub_item_sizer);
			uint32_t flags = get_sizer_item_flags(item.get_options());
			int border = parent_window->ConvertDialogToPixels(wxSize(item.get_options().get_border(default_border), 0)).x;
			int propoption = get_sizer_item_proportion(item.get_options());
			if (sub_item_window) sizer_obj->Add(sub_item_window, propoption, flags, border);
			if (sub_item_sizer) sizer_obj->Add(sub_item_sizer, propoption, flags, border);
		}
	}
}

GridOptions growable_cols(int col1)
{
	GridOptions options;
	options.add_col(col1);
	return options;
}

GridOptions growable_cols(int col1, int col2)
{
	GridOptions options;
	options.add_col(col1);
	options.add_col(col2);
	return options;
}

GridOptions growable_cols(int col1, int col2, int col3)
{
	GridOptions options;
	options.add_col(col1);
	options.add_col(col2);
	options.add_col(col3);
	return options;
}

GridOptions growable_cols(int col1, int col2, int col3, int col4)
{
	GridOptions options;
	options.add_col(col1);
	options.add_col(col2);
	options.add_col(col3);
	options.add_col(col4);
	return options;
}

GridOptions growable_cols(int col1, int col2, int col3, int col4, int col5)
{
	GridOptions options;
	options.add_col(col1);
	options.add_col(col2);
	options.add_col(col3);
	options.add_col(col4);
	options.add_col(col5);
	return options;
}

UIElem grid(int cols, int rows, const UIElemOptions &options, const GridOptions &grid_options, int hgap, int vgap)
{
	auto create_fun = [=] (const BuildOptions &build_options, wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* 
	{
		wxWindow *parent_window = dynamic_cast<wxWindow*>(parent);
		wxPoint gap_pt = parent_window->ConvertDialogToPixels(wxPoint(hgap, vgap));
		wxFlexGridSizer *sizer_obj = new wxFlexGridSizer(rows, cols, gap_pt.x, gap_pt.y);
		for (int col : grid_options.get_growable_cols()) sizer_obj->AddGrowableCol(col);
		for (int row : grid_options.get_growable_rows()) sizer_obj->AddGrowableRow(row);
		process_sizer_items(build_options, parent, sizer_obj, items);
		if (sizer == nullptr) parent_window->SetSizer(sizer_obj);
		return sizer_obj;
	};
	return UIElem(create_fun, options);
}

UIElem box(const UIElemOptions &options, wxOrientation orient)
{
	auto create_fun = [=] (const BuildOptions &build_options, wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* 
	{
		wxWindow *parent_window = dynamic_cast<wxWindow*>(parent);
		wxBoxSizer *sizer_obj = new wxBoxSizer(orient);
		process_sizer_items(build_options, parent, sizer_obj, items);
		if (sizer == nullptr) parent_window->SetSizer(sizer_obj);
		return sizer_obj;
	};
	return UIElem(create_fun, options);
}

UIElem hbox(const UIElemOptions &options)
{
	return box(options, wxHORIZONTAL);
}

UIElem vbox(const UIElemOptions &options)
{
	return box(options, wxVERTICAL);
}

UIElem dlg_buttons(const UIElemOptions &options)
{
	auto create_fun = [=] (const BuildOptions &build_options, wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* 
	{
		wxWindow *parent_window = dynamic_cast<wxWindow*>(parent);
		wxStdDialogButtonSizer *sizer_obj = new wxStdDialogButtonSizer();
		for (auto &item : items)
		{
			wxButton *btn = dynamic_cast<wxButton*>(item.build(build_options, parent, sizer_obj));
			assert(btn);
			sizer_obj->AddButton(btn);
		}
		sizer_obj->Realize();
		return sizer_obj;
	};
	return UIElem(create_fun, options);
}

UIElem dlg_buttons_ok_cancel(const UIElemOptions &options)
{
	return dlg_buttons(options) [
		button_ok(), 
		button_cancel()
	];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

wxSize cvt_dialog_size_into_pixels_size(wxWindow *window, int width, int height)
{
	wxSize size = window->ConvertDialogToPixels(wxSize(width, height));
	return wxSize(width != -1 ? size.x : -1, height != -1 ? size.y : -1);
}

wxSize get_widget_size(wxObject *parent, const UIElemOptions &options)
{
	wxWindow *parent_window = dynamic_cast<wxWindow*>(parent);
	return cvt_dialog_size_into_pixels_size(parent_window, options.get_width(), options.get_height());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void set_widgets_props(wxWindow *widget, const UIElemOptions &options)
{
	bool bold_font = options.has_flag(font_bold);
	bool italic_font = options.has_flag(font_italic);
	bool underline_font = options.has_flag(font_underline);
	if (bold_font || italic_font || underline_font)
	{
		wxFont font = widget->GetFont();
		if (bold_font) font.MakeBold();
		if (italic_font) font.MakeItalic();
		if (underline_font) font.MakeUnderlined();
		widget->SetFont(font);
	}
}

void build_window_child_items(const BuildOptions &build_options, wxWindow *window, wxSizer *sizer, const UIElems &items, bool do_layout)
{
	if (items.empty()) return;
	assert(items.size() == 1);
	wxSizer* internal_sizer = dynamic_cast<wxSizer*>(items[0].build(build_options, window, sizer));
	assert(internal_sizer);

	if (do_layout) window->SetSizerAndFit(internal_sizer);
	else window->SetSizer(internal_sizer);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Window existing_window(wxWindow *window, const UIElemOptions &options, const UIElem &layout)
{
	auto create_fun = [=] (const BuildOptions &build_options, wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* {
		set_widgets_props(window, options);
		if (items.empty()) return window;
		assert(items.size() == 1);
		items[0].build(build_options, window, sizer);
		window->SetSize(cvt_dialog_size_into_pixels_size(window, options.get_width(), options.get_height()));
		return window;
	};
	return Window(create_fun, layout, options);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


UIElem hline(const UIElemOptions &options)
{
	return UIElem([=] (const BuildOptions &build_options, wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* {
		return new wxStaticLine(
			dynamic_cast<wxWindow*>(parent), 
			wxID_ANY, 
			wxDefaultPosition, 
			get_widget_size(parent, options),
			options.get_style() | wxLI_HORIZONTAL
		);
	}, options);
}

UIElem spacer(int size)
{
	return UIElem(nullptr, width(size));
}

UIElem spring()
{
	return UIElem(nullptr, stretch);
}

UIElem text(const wxString &text, const UIElemOptions &options)
{
	return UIElem([=] (const BuildOptions &build_options, wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* {
		wxStaticText *widget = new wxStaticText(
			dynamic_cast<wxWindow*>(parent), 
			wxID_ANY, 
			_(text),
			wxDefaultPosition, 
			get_widget_size(parent, options),
			options.get_style()
		);
		set_widgets_props(widget, options);
		return widget;
	}, options);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UIElem edit(const EditOptions &edit_options)
{
	return UIElem([=] (const BuildOptions &build_options, wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* {
		uint32_t style = edit_options.get_elem_options().get_style();

		if (edit_options.has_flag(ed_multiline)) style |= wxTE_MULTILINE;
		if (edit_options.has_flag(ed_readonly))  style |= wxTE_READONLY;
		if (edit_options.has_flag(ed_rich))      style |= wxTE_RICH2;
		if (edit_options.has_flag(ed_autourl))   style |= wxTE_AUTO_URL;
		if (edit_options.has_flag(ed_password))  style |= wxTE_PASSWORD;
		if (edit_options.has_flag(ed_left))      style |= wxTE_LEFT;
		if (edit_options.has_flag(ed_centre))    style |= wxTE_CENTRE;
		if (edit_options.has_flag(ed_right))     style |= wxTE_RIGHT;

		wxTextCtrl *widget = new wxTextCtrl(
			dynamic_cast<wxWindow*>(parent), 
			wxID_ANY, 
			wxEmptyString, 
			wxDefaultPosition, 
			get_widget_size(parent, edit_options.get_elem_options()),
			style
		);
		set_widgets_props(widget, edit_options.get_elem_options());
		return widget;
	}, edit_options.get_elem_options());
}

UIElem edit(const UIElemOptions &options = UIElemOptions())
{
	return edit(EditOptions(options));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UIElem list(const ListOptions& list_options)
{
	return UIElem([=] (const BuildOptions &build_options, wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* {
		uint32_t style = list_options.get_elem_options().get_style();

		if (list_options.has_flag(l_list))            style |= wxLC_LIST;
		if (list_options.has_flag(l_report))          style |= wxLC_REPORT;
		if (list_options.has_flag(l_virtual))         style |= wxLC_VIRTUAL;
		if (list_options.has_flag(l_icon))            style |= wxLC_ICON;
		if (list_options.has_flag(l_small_icon))      style |= wxLC_SMALL_ICON;
		if (list_options.has_flag(l_align_top))       style |= wxLC_ALIGN_TOP;
		if (list_options.has_flag(l_align_left))      style |= wxLC_ALIGN_LEFT;
		if (list_options.has_flag(l_autoarrange))     style |= wxLC_AUTOARRANGE;
		if (list_options.has_flag(l_edit_labels))     style |= wxLC_EDIT_LABELS;
		if (list_options.has_flag(l_no_header))       style |= wxLC_NO_HEADER;
		if (list_options.has_flag(l_single_sel))      style |= wxLC_SINGLE_SEL;
		if (list_options.has_flag(l_sort_ascending))  style |= wxLC_SORT_ASCENDING;
		if (list_options.has_flag(l_sort_descending)) style |= wxLC_SORT_DESCENDING;
		if (list_options.has_flag(l_hrules))          style |= wxLC_HRULES;
		if (list_options.has_flag(l_vrules))          style |= wxLC_VRULES;

		if (!list_options.has_flag(l_list)    && 
			!list_options.has_flag(l_report)  && 
			!list_options.has_flag(l_virtual) && 
			!list_options.has_flag(l_icon)) style |= wxLC_ICON;

		wxListCtrl *widget = new wxListCtrl (
			dynamic_cast<wxWindow*>(parent), 
			wxID_ANY, 
			wxDefaultPosition, 
			get_widget_size(parent, list_options.get_elem_options()),
			style
		);
		set_widgets_props(widget, list_options.get_elem_options());
		return widget;
	}, list_options.get_elem_options());
}

UIElem list(const UIElemOptions &options)
{
	return list(ListOptions(options));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UIElem button(const wxString &text, int id, bool is_default, const ButtonOptions &options)
{
	return UIElem([=] (const BuildOptions &build_options, wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* {
		uint32_t style = options.get_elem_options().get_style();

		if (options.has_flag(bu_left))     style |= wxBU_LEFT;
		if (options.has_flag(bu_right))    style |= wxBU_RIGHT;
		if (options.has_flag(bu_top))      style |= wxBU_TOP;
		if (options.has_flag(bu_bottom))   style |= wxBU_BOTTOM;
		if (options.has_flag(bu_exactfit)) style |= wxBU_EXACTFIT;
		if (options.has_flag(bu_notext))   style |= wxBU_NOTEXT;

		wxButton* widget = new wxButton(
			dynamic_cast<wxWindow*>(parent), 
			id, 
			_(text), 
			wxDefaultPosition, 
			get_widget_size(parent, options.get_elem_options()),
			style
		);
		if (is_default) widget->SetDefault();
		set_widgets_props(widget, options.get_elem_options());
		return widget;
	}, options.get_elem_options());
}

UIElem button(const wxString &text, const UIElemOptions &options)
{
	return button(text, wxID_ANY, false, ButtonOptions(options));
}

UIElem button_ok(const wxString &text, const UIElemOptions &options)
{
	return button(text, wxID_OK, true, ButtonOptions(options));
}

UIElem button(const wxString &text, const ButtonOptions &options)
{
	return button(text, wxID_ANY, false, options);
}

UIElem button_cancel(const wxString &text, const UIElemOptions &options)
{
	return button(text, wxID_CANCEL, false, ButtonOptions(options));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UIElem check_box(const wxString &text, const UIElemOptions &options)
{
	return UIElem([=] (const BuildOptions &build_options, wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* {
		wxCheckBox* widget = new wxCheckBox(
			dynamic_cast<wxWindow*>(parent), 
			wxID_ANY,
			_(text), 
			wxDefaultPosition, 
			get_widget_size(parent, options),
			options.get_style()
		);
		set_widgets_props(widget, options);
		return widget;
	}, options);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UIElem choice(const UIElemOptions &options)
{
	return UIElem([=] (const BuildOptions &build_options, wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* {
		wxChoice* widget = new wxChoice(
			dynamic_cast<wxWindow*>(parent), 
			wxID_ANY,
			wxDefaultPosition, 
			get_widget_size(parent, options),
			options.get_style()
		);
		set_widgets_props(widget, options);
		for (auto &item : items)
		{
			const Item *obj = dynamic_cast<const Item*>(item.build(build_options));
			assert(obj);
			widget->Append(_(obj->get_text()));
			delete obj;
		}
		return widget;
	}, options);
}


UIElem dir_ctrl(const UIElemOptions &options, bool dirs_only)
{
	return UIElem([=] (const BuildOptions &build_options, wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* {
		wxGenericDirCtrl* widget = new wxGenericDirCtrl(
			dynamic_cast<wxWindow*>(parent), 
			wxID_ANY,
			wxEmptyString,
			wxDefaultPosition, 
			get_widget_size(parent, options),
			options.get_style() | (dirs_only ? wxDIRCTRL_DIR_ONLY : 0)
		);
		set_widgets_props(widget, options);
		return widget;
	}, options);
}

UIElem image(const UIElemOptions &options)
{
	return UIElem([=] (const BuildOptions &build_options, wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* {
		wxStaticBitmap* widget = new wxStaticBitmap(
			dynamic_cast<wxWindow*>(parent), 
			wxID_ANY,
			wxNullBitmap,
			wxDefaultPosition, 
			get_widget_size(parent, options),
			options.get_style()
		);
		set_widgets_props(widget, options);
		return widget;
	}, options);
}

Window scroll_box(const UIElemOptions &options, const UIElem &layout)
{
	auto create_fun = [=] (const BuildOptions &build_options, wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* {
		wxScrolledWindow* widget = new wxScrolledWindow(
			dynamic_cast<wxWindow*>(parent), 
			wxID_ANY,
			wxDefaultPosition, 
			get_widget_size(parent, options),
			options.get_style()
		);
		set_widgets_props(widget, options);
		widget->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_DEFAULT);
		build_window_child_items(build_options, widget, sizer, items, false);
		return widget;
	};
	return Window(create_fun, layout, options);
}

UIElem notebook(const UIElemOptions &options, const UIElem &layout)
{
	return UIElem([=] (const BuildOptions &build_options, wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* {
		wxNotebook* widget = new wxNotebook(
			dynamic_cast<wxWindow*>(parent), 
			wxID_ANY,
			wxDefaultPosition, 
			get_widget_size(parent, options),
			options.get_style()
		);
		set_widgets_props(widget, options);
		for (auto &page : items)
		{
			wxWindow *page_ctrl = dynamic_cast<wxWindow*>(page.build(build_options, widget));
			widget->AddPage(page_ctrl, _(page.get_name()));
		}
		return widget;
	}, options);
}

Window page(const wxString &name, const UIElemOptions &options, const UIElem &layout)
{
	auto create_fun = [=] (const BuildOptions &build_options, wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* {
		wxPanel* widget = new wxPanel(
			dynamic_cast<wxWindow*>(parent), 
			wxID_ANY,
			wxDefaultPosition, 
			get_widget_size(parent, options),
			options.get_style()
		);
		set_widgets_props(widget, options);
		build_window_child_items(build_options, widget, sizer, items, true);
		return widget;
	};
	return Window(create_fun, layout, options, name);
}

UIElem gauge(int range, const UIElemOptions &options)
{
	return UIElem([=] (const BuildOptions &build_options, wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* {
		wxGauge* widget = new wxGauge(
			dynamic_cast<wxWindow*>(parent), 
			wxID_ANY,
			range,
			wxDefaultPosition, 
			get_widget_size(parent, options),
			options.get_style()
		);
		set_widgets_props(widget, options);
		return widget;
	}, options);
}

UIElem slider(int min, int max, const UIElemOptions &options)
{
	return UIElem([=] (const BuildOptions &build_options, wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* {
		wxSlider* widget = new wxSlider(
			dynamic_cast<wxWindow*>(parent), 
			wxID_ANY,
			0,
			min,
			max,
			wxDefaultPosition, 
			get_widget_size(parent, options),
			options.get_style()
		);
		set_widgets_props(widget, options);
		return widget;
	}, options);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UIElem listbox(const ListboxOptions &options)
{
	return UIElem([=] (const BuildOptions &build_options, wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* {
		uint32_t style = options.get_elem_options().get_style();

		if (options.has_flag(lb_single))    style |= wxLB_SINGLE;
		if (options.has_flag(lb_multiple))  style |= wxLB_MULTIPLE;
		if (options.has_flag(lb_extended))  style |= wxLB_EXTENDED;
		if (options.has_flag(lb_hscroll))   style |= wxLB_HSCROLL;
		if (options.has_flag(lb_always_sb)) style |= wxLB_ALWAYS_SB;
		if (options.has_flag(lb_needed_sb)) style |= wxLB_NEEDED_SB;
		if (options.has_flag(lb_no_sb))     style |= wxLB_NO_SB;
		if (options.has_flag(lb_sort))      style |= wxLB_SORT;

		wxListBox *widget = new wxListBox(
			dynamic_cast<wxWindow*>(parent), 
			wxID_ANY, 
			wxDefaultPosition, 
			get_widget_size(parent, options.get_elem_options()),
			style
		);
		set_widgets_props(widget, options.get_elem_options());
		return widget;
	}, options.get_elem_options());
}

UIElem listbox(const UIElemOptions &options)
{
	return listbox(ListboxOptions(options));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UIElem radio(const wxString &text, bool group_start, const UIElemOptions &options)
{
	return UIElem([=] (const BuildOptions &build_options, wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* {
		uint32_t style = options.get_style();

		if (group_start) style |= wxRB_GROUP;

		wxRadioButton *widget = new wxRadioButton(
			dynamic_cast<wxWindow*>(parent), 
			wxID_ANY, 
			text,
			wxDefaultPosition, 
			get_widget_size(parent, options),
			style
		);
		set_widgets_props(widget, options);
		return widget;
	}, options);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UIElem spin(int min, int max, const SpinOptions &options)
{
	return UIElem([=] (const BuildOptions &build_options, wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* {
		uint32_t style = options.get_elem_options().get_style();

		if (options.has_flag(sp_arrow_keys))    style |= wxSP_ARROW_KEYS;
		if (options.has_flag(sp_wrap))          style |= wxSP_WRAP;
		if (options.has_flag(sp_process_enter)) style |= wxTE_PROCESS_ENTER;
		if (options.has_flag(sp_align_left))    style |= wxALIGN_LEFT;
		if (options.has_flag(sp_align_centre))  style |= wxALIGN_CENTRE;
		if (options.has_flag(sp_align_right))   style |= wxALIGN_RIGHT;

		wxSpinCtrl *widget = new wxSpinCtrl(
			dynamic_cast<wxWindow*>(parent), 
			wxID_ANY, 
			wxEmptyString,
			wxDefaultPosition, 
			get_widget_size(parent, options.get_elem_options()),
			style,
			min,
			max,
			min
		);
		set_widgets_props(widget, options.get_elem_options());
		return widget;
	}, options.get_elem_options());
}

UIElem spin(int min, int max, const UIElemOptions &options)
{
	return spin(min, max, SpinOptions(options));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


} // namespace gb
