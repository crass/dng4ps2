#pragma once

#include <vector>
#include <functional>
#include <stdint.h>

// fwd.
class wxObject;
class wxSizer;
class wxFrame;
class wxDialog;
class wxString;
class UIElem; typedef std::vector<UIElem> UIElems;
class Layout;

typedef std::function <wxObject* (wxObject *parent, wxSizer *sizer, const UIElems &items)> CreateWidgetFun;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class UIElemOptions
{
public:
	UIElemOptions(uint32_t flags = 0, int border = 0, int width = -1, int height = -1) : 
		flags_(flags), 
		border_(border),
		width_(width),
		height_(height)
	{}

	bool has_flag(const UIElemOptions &flag) const { return (flags_ &  flag.flags_) != 0; }
	int get_border() const { return border_; }

	int get_width() const { return width_; }
	int get_height() const { return height_; }

	UIElemOptions operator & (const UIElemOptions &other) const;

private:
	uint32_t flags_;
	int border_;
	int width_;
	int height_;
};

extern const UIElemOptions bord_top;
extern const UIElemOptions bord_bottom;
extern const UIElemOptions bord_left;
extern const UIElemOptions bord_right;
extern const UIElemOptions bord_all;

extern const UIElemOptions align_left;
extern const UIElemOptions align_right;
extern const UIElemOptions align_hcenter;
extern const UIElemOptions align_top;
extern const UIElemOptions align_bottom;
extern const UIElemOptions align_vcenter;

extern const UIElemOptions font_bold;
extern const UIElemOptions font_italic;
extern const UIElemOptions font_underline;

extern const UIElemOptions expand;
extern const UIElemOptions stretch;

UIElemOptions border(int border);
UIElemOptions width(int value);
UIElemOptions height(int value);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class UIElem
{
public:
	UIElem(const CreateWidgetFun &create_fun, const UIElemOptions &options) : 
		create_fun_(create_fun),
		options_(options)
	{}

	template <typename T> UIElem& operator >> (T & var);
	void set_item(const UIElem &item);
	void set_items(const UIElems &items);

	wxObject* build_gui(wxObject *parent = nullptr, wxSizer *sizer = nullptr) const;

	const UIElemOptions& get_options() const { return options_; }

private:
	UIElems sub_items_;
	CreateWidgetFun create_fun_;
	UIElemOptions options_;
};


template <typename T>
UIElem& UIElem::operator >> (T & var) 
{ 
	auto old_create_object_fun = create_fun_;
	auto *var_ptr = &var;
	create_fun_ = [=] (wxObject *parent, wxSizer *sizer, const UIElems &items) 
	{
		auto obj = old_create_object_fun(parent, sizer, items);
		*var_ptr = dynamic_cast<T>(obj);
		assert(*var_ptr);
		return obj;
	};
	return *this; 
}

UIElems operator , (UIElem &elem1, UIElem &elem2);
UIElems& operator , (UIElems &elems, UIElem &elem);
UIElems& operator , (UIElems &elems1, UIElems &elems2);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Layout
{
public:
	Layout(const CreateWidgetFun &create_fun, const UIElemOptions &options) :
		create_fun_(create_fun),
		options_(options)
	{}

	UIElem operator [] (const UIElem &elem);
	UIElem operator [] (const UIElems &elems);

private:
	const CreateWidgetFun create_fun_;
	UIElemOptions options_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Window
{
public:
	Window(const CreateWidgetFun &create_fun, const Layout &layout, const UIElemOptions &options) : 
		create_fun_(create_fun), 
		layout_(layout),
		options_(options)
	{}

	UIElem operator [] (const UIElem &elem);
	UIElem operator [] (const UIElems &elems);

private:
	Layout layout_;
	const CreateWidgetFun create_fun_;
	UIElemOptions options_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Layout grid(int cols, int rows = 0, const UIElemOptions &options = UIElemOptions(), int hgap = 5, int vgap = 5);
Layout hbox(const UIElemOptions &options = UIElemOptions(), int gap = 5);
Layout vbox(const UIElemOptions &options = UIElemOptions(), int gap = 5);
Layout dlg_buttons(const UIElemOptions &options = align_right);

Window frame(wxFrame *frame, const Layout &layout = vbox());
Window dialog(wxDialog *dialog, const Layout &layout = vbox());

UIElem hline(const UIElemOptions &options = expand);
UIElem text(const wxString &text, const UIElemOptions &options = UIElemOptions());
UIElem edit(const UIElemOptions &options = UIElemOptions());
UIElem button(const wxString &text, int id = wxID_ANY, bool is_default = false, const UIElemOptions &options = UIElemOptions());
UIElem button_ok(const wxString &text, bool is_default = true, const UIElemOptions &options = UIElemOptions());
UIElem button_cancel(const wxString &text, const UIElemOptions &options = UIElemOptions());
