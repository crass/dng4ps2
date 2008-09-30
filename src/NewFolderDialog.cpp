#include "pch.h"
#include "NewFolderDialog.h"

//(*InternalHeaders(NewFolderDialog)
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/string.h>
//*)

//(*IdInit(NewFolderDialog)
const long NewFolderDialog::ID_STATICTEXT3 = wxNewId();
const long NewFolderDialog::ID_STATICTEXT1 = wxNewId();
const long NewFolderDialog::ID_STATICTEXT2 = wxNewId();
const long NewFolderDialog::ID_TEXTCTRL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(NewFolderDialog,wxDialog)
	//(*EventTable(NewFolderDialog)
	//*)
END_EVENT_TABLE()

NewFolderDialog::NewFolderDialog(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(NewFolderDialog)
	wxStaticText* StaticText2;
	wxStaticText* stxtRootFolderCap;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;
	
	Create(parent, id, _("newfolderCaption"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	sizerMain = new wxFlexGridSizer(0, 1, 0, 0);
	sizerMain->AddGrowableCol(0);
	sizerMain->AddGrowableRow(1);
	FlexGridSizer2 = new wxFlexGridSizer(0, 2, 0, wxDLG_UNIT(this,wxSize(-5,0)).GetWidth());
	FlexGridSizer2->AddGrowableCol(1);
	FlexGridSizer2->AddGrowableRow(0);
	stxtRootFolderCap = new wxStaticText(this, ID_STATICTEXT3, _("newfolderRootCap"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer2->Add(stxtRootFolderCap, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	stxtRootFolder = new wxStaticText(this, ID_STATICTEXT1, _("Label"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	stxtRootFolder->SetMaxSize(wxDLG_UNIT(this,wxSize(150,-1)));
	FlexGridSizer2->Add(stxtRootFolder, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("newfolderName"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer2->Add(StaticText2, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	txtFolderName = new wxTextCtrl(this, ID_TEXTCTRL1, _("newfolderDefaultName"), wxDefaultPosition, wxDLG_UNIT(this,wxSize(102,12)), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	FlexGridSizer2->Add(txtFolderName, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	sizerMain->Add(FlexGridSizer2, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, _("btnOk")));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, _("btnCancel")));
	StdDialogButtonSizer1->Realize();
	sizerMain->Add(StdDialogButtonSizer1, 1, wxALL|wxALIGN_RIGHT|wxALIGN_BOTTOM, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	SetSizer(sizerMain);
	sizerMain->Fit(this);
	sizerMain->SetSizeHints(this);
	Center();
	
	Connect(wxID_ANY,wxEVT_INIT_DIALOG,(wxObjectEventFunction)&NewFolderDialog::OnInit);
	//*)
}

NewFolderDialog::~NewFolderDialog()
{
	//(*Destroy(NewFolderDialog)
	//*)
}

bool NewFolderDialog::execute(const wxString &root)
{
    stxtRootFolder->SetLabel(root);
    this->SetSize( sizerMain->Fit(this) );

    int result = this->ShowModal();
    return (result == wxID_OK);
}

wxString NewFolderDialog::get_path()
{
    return txtFolderName->GetValue();
}

void NewFolderDialog::OnInit(wxInitDialogEvent& event)
{
	this->SetSize( sizerMain->Fit(this) );
	event.Skip();
}
