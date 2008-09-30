#ifndef DNG4PS2_EXCEPTION_HPP_FILE_INCLUDED_
#define DNG4PS2_EXCEPTION_HPP_FILE_INCLUDED_

class Exception
{
public:
	Exception(const wxString & text) : text_(text) {}

	wxString what() const
	{
		return text_;
	}

private:
	wxString text_;
};

#endif
