#include <assert.h>

#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/dirctrl.h>

#include "wxGUIBuilder.hpp"

const UIElemOptions bord_top       = UIElemOptions(0x00000001);
const UIElemOptions bord_bottom    = UIElemOptions(0x00000002);
const UIElemOptions bord_left      = UIElemOptions(0x00000004);
const UIElemOptions bord_right     = UIElemOptions(0x00000008);
const UIElemOptions bord_all       = UIElemOptions(0x00000010);

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

UIElemOptions UIElemOptions::operator & (const UIElemOptions &other) const
{
	uint32_t flags = flags_ | other.flags_;
	int border = std::max(border_, other.border_);
	int width = std::max(width_, other.width_);
	int height = std::max(height_, other.height_);
	return UIElemOptions(flags, border, width, height);
}

UIElemOptions border(int border)
{
	return UIElemOptions(0, border);
}

UIElemOptions width(int value)
{
	return UIElemOptions(0, 0, value);
}

UIElemOptions height(int value)
{
	return UIElemOptions(0, 0, -1, value);
}

UIElemOptions size(int width, int height)
{
	return UIElemOptions(0, 0, width, height);
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

wxObject* UIElem::build_gui(wxObject *parent, wxSizer *sizer) const
{
	if (create_fun_ == nullptr) return nullptr;
	return create_fun_(parent, sizer, sub_items_);
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


UIElem Layout::operator [] (const UIElem &elem)
{
	UIElem result(create_fun_, options_);
	result.set_item(elem);
	return result;
}

UIElem Layout::operator [] (const UIElems &elems) 
{ 
	UIElem result(create_fun_, options_);
	result.set_items(elems);
	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UIElem Window::operator [] (const UIElem &elem)
{
	UIElem result(create_fun_, options_);
	result.set_item(layout_[elem]);
	return result;
}

UIElem Window::operator [] (const UIElems &elems)
{
	UIElem result(create_fun_, options_);
	result.set_item(layout_[elems]);
	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	if (!options.has_flag(align_top) && 
		!options.has_flag(align_bottom) && 
		!options.has_flag(align_vcenter)) result |= wxALIGN_CENTER_VERTICAL;

	return result;
}

int get_sizer_item_proportion(const UIElemOptions &options)
{
	return options.has_flag(stretch) ? 1 : 0;
}

void process_sizer_items(wxObject *parent, wxSizer *sizer_obj, const UIElems &items, int spacer_size)
{
	for (auto &item : items)
	{
		wxObject *sub_item_obj = item.build_gui(parent, sizer_obj);
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
			if ((spacer_size != 0) && sizer_obj->GetItemCount()) sizer_obj->AddSpacer(spacer_size);
			uint32_t flags = get_sizer_item_flags(item.get_options());
			int border = parent_window->ConvertDialogToPixels(wxSize(item.get_options().get_border(), 0)).x;
			int propoption = get_sizer_item_proportion(item.get_options());
			if (sub_item_window) sizer_obj->Add(sub_item_window, propoption, flags, border);
			if (sub_item_sizer) sizer_obj->Add(sub_item_sizer, propoption, flags, border);
		}
	}
}

Layout grid(int cols, int rows, const UIElemOptions &options, int hgap, int vgap)
{
	auto create_fun = [=] (wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* 
	{
		wxWindow *parent_window = dynamic_cast<wxWindow*>(parent);
		wxPoint gap_pt = parent_window->ConvertDialogToPixels(wxPoint(hgap, vgap));
		wxFlexGridSizer *sizer_obj = new wxFlexGridSizer(rows, cols, gap_pt.x, gap_pt.y);
		sizer_obj->SetFlexibleDirection(wxBOTH);
		sizer_obj->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
		process_sizer_items(parent, sizer_obj, items, 0);
		if (sizer == nullptr) parent_window->SetSizer(sizer_obj);
		return sizer_obj;
	};
	return Layout(create_fun, options);
}

Layout box(const UIElemOptions &options, int gap, wxOrientation orient)
{
	auto create_fun = [=] (wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* 
	{
		wxWindow *parent_window = dynamic_cast<wxWindow*>(parent);
		wxBoxSizer *sizer_obj = new wxBoxSizer(orient);
		wxPoint gap_pt = parent_window->ConvertDialogToPixels(wxPoint(gap, gap));
		process_sizer_items(parent, sizer_obj, items, orient == wxHORIZONTAL ? gap_pt.x : gap_pt.y);
		if (sizer == nullptr) parent_window->SetSizer(sizer_obj);
		return sizer_obj;
	};
	return Layout(create_fun, options);
}

Layout hbox(const UIElemOptions &options, int gap)
{
	return box(options, gap, wxHORIZONTAL);
}

Layout vbox(const UIElemOptions &options, int gap)
{
	return box(options, gap, wxVERTICAL);
}

Layout dlg_buttons(const UIElemOptions &options)
{
	auto create_fun = [=] (wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* 
	{
		wxWindow *parent_window = dynamic_cast<wxWindow*>(parent);
		wxStdDialogButtonSizer *sizer_obj = new wxStdDialogButtonSizer();
		for (auto &item : items)
		{
			wxButton *btn = dynamic_cast<wxButton*>(item.build_gui(parent, sizer_obj));
			assert(btn);
			sizer_obj->AddButton(btn);
		}
		sizer_obj->Realize();
		return sizer_obj;
	};
	return Layout(create_fun, options);
}

UIElem dlg_buttons_ok_cancel(const UIElemOptions &options)
{
	return dlg_buttons(options) [
		button_ok(), 
		button_cancel()
	];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Window frame(wxFrame *frame, const Layout &layout)
{
	auto create_fun = [=] (wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* { 
		for (auto &item : items) item.build_gui(frame, sizer);
		return frame; 
	};
	return Window(create_fun, layout, UIElemOptions());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Window existing_window(wxWindow *window, const Layout &layout)
{
	auto create_fun = [=] (wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* {
		if (items.empty()) return window;
		assert(items.size() == 1);
		items[0].build_gui(window, sizer);
		return window;
	};
	return Window(create_fun, layout, UIElemOptions());
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

uint32_t get_widget_flags(const UIElemOptions &options)
{
	uint32_t result = 0;
	return result;
}

wxSize get_widget_size(wxObject *parent, const UIElemOptions &options)
{
	wxWindow *parent_window = dynamic_cast<wxWindow*>(parent);
	int width = options.get_width();
	int height = options.get_height();
	wxSize size = parent_window->ConvertDialogToPixels(wxSize(width, height));
	return wxSize(width != -1 ? size.x : -1, height != -1 ? size.y : -1);
}

UIElem hline(const UIElemOptions &options)
{
	return UIElem([=] (wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* {
		return new wxStaticLine(
			dynamic_cast<wxWindow*>(parent), 
			wxID_ANY, 
			wxDefaultPosition, 
			get_widget_size(parent, options),
			get_widget_flags(options) | wxLI_HORIZONTAL
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
	return UIElem([=] (wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* {
		wxStaticText *widget = new wxStaticText(
			dynamic_cast<wxWindow*>(parent), 
			wxID_ANY, 
			_(text),
			wxDefaultPosition, 
			get_widget_size(parent, options),
			get_widget_flags(options)
		);
		set_widgets_props(widget, options);
		return widget;
	}, options);
}

UIElem edit(const UIElemOptions &options)
{
	return UIElem([=] (wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* {
		wxTextCtrl *widget = new wxTextCtrl(
			dynamic_cast<wxWindow*>(parent), 
			wxID_ANY, 
			wxEmptyString, 
			wxDefaultPosition, 
			get_widget_size(parent, options),
			get_widget_flags(options)
		);
		set_widgets_props(widget, options);
		return widget;
	}, options);
}

UIElem button(const wxString &text, int id, bool is_default, const UIElemOptions &options)
{
	return UIElem([=] (wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* {
		wxButton* widget = new wxButton(
			dynamic_cast<wxWindow*>(parent), 
			id, 
			_(text), 
			wxDefaultPosition, 
			get_widget_size(parent, options),
			get_widget_flags(options)
		);
		if (is_default) widget->SetDefault();
		set_widgets_props(widget, options);
		return widget;
	}, options);
}

UIElem button_ok(const wxString &text, bool is_default, const UIElemOptions &options)
{
	return button(text, wxID_OK, is_default, options);
}

UIElem button_cancel(const wxString &text, const UIElemOptions &options)
{
	return button(text, wxID_CANCEL, false, options);
}

UIElem dir_ctrl(const UIElemOptions &options, bool dirs_only)
{
	return UIElem([=] (wxObject *parent, wxSizer *sizer, const UIElems &items) -> wxObject* {
		wxGenericDirCtrl* widget = new wxGenericDirCtrl(
			dynamic_cast<wxWindow*>(parent), 
			wxID_ANY,
			wxEmptyString,
			wxDefaultPosition, 
			get_widget_size(parent, options),
			get_widget_flags(options) | (dirs_only ? wxDIRCTRL_DIR_ONLY : 0)
		);
		set_widgets_props(widget, options);
		return widget;
	}, options);
}