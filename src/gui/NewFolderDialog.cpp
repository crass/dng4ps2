#include "pch.h"
#include "NewFolderDialog.h"

#include "lib/wxGUIBuilder.hpp"


NewFolderDialog::NewFolderDialog(wxWindow* parent, wxWindowID id)
{
	using namespace gb;

	Create(parent, id, _("newfolderCaption")); 

	auto gui = existing_window(this) 
	[ 
		vbox()
		[
			grid(2, 0, border(0)) 
			[
				text ("newfolderRootCap", align_right),
				text (wxEmptyString) >> stxtRootFolder,
				text ("newfolderName", align_right),
				edit (width(150) | expand) >> txtFolderName
			],
			hline(),
			dlg_buttons_ok_cancel()
		]
	];

	gui.build();

	Layout();
	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
}

bool NewFolderDialog::execute(const wxString &root)
{
    stxtRootFolder->SetLabel(root);

	GetSizer()->Fit(this);
	Center();

    int result = this->ShowModal();
    return (result == wxID_OK);
}

wxString NewFolderDialog::get_path()
{
    return txtFolderName->GetValue();
}
