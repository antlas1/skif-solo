#include "settingsdialog.h"

//Идентификаторы для кнопок
enum { ID_CbScreenreader = 1 };

// clang-format off
wxBEGIN_EVENT_TABLE(SettingsDialog, wxDialog)
EVT_CHECKBOX(ID_CbScreenreader, SettingsDialog::OnCheckScreen)
wxEND_EVENT_TABLE()

SettingsDialog::SettingsDialog(const ConfigSettings& sets, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name):
	wxDialog(parent,id,title,pos,size,style,name),
	_sets(sets)
{
	wxPanel* panel = new wxPanel(this, -1);
	
	{
		wxCheckBox* cb = new wxCheckBox(panel, ID_CbScreenreader,
			_("Use screenreader"), wxPoint(15, 30), wxDefaultSize, wxRB_GROUP);
		cb->SetValue(sets.use_screenreader);
	}

	wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

	vbox->Add(panel, 1);
	vbox->Add(CreateButtonSizer(wxOK | wxCANCEL), 1);

	SetSizer(vbox);
}

ConfigSettings SettingsDialog::newSettings() const
{
	return _sets;
}

void SettingsDialog::OnCheckScreen(wxCommandEvent& event)
{
	_sets.use_screenreader = event.IsChecked();
}
