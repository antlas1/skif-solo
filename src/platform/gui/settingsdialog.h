#pragma once

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

//Configurable settings
struct ConfigSettings
{
    bool use_screenreader;
};

class SettingsDialog: public wxDialog
{
public:
    SettingsDialog(const ConfigSettings& sets, wxWindow *parent, wxWindowID id, const wxString &title, 
        const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE, 
        const wxString &name=wxDialogNameStr);
    ConfigSettings newSettings() const;
private:
    void OnCheckScreen(wxCommandEvent& event);

    ConfigSettings _sets;
    //Подключение таблицы событий
    wxDECLARE_EVENT_TABLE();
};