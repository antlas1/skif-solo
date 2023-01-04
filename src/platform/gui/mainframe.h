#pragma once

#include "settingsdialog.h"
#include <wx/wxprec.h>
#include <optional>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

//Главное окно
class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title, 
        const wxPoint& pos, 
        const wxSize& size);
    void DisplayOutText(const wxString& text);
    std::optional<wxString> enteredText() const;
private:
    void ClearOutText();
    void DisplayStatus(std::string text);
    //Handlers - обработчики событий от окна
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OpenSettings(wxCommandEvent& event);

    ConfigSettings getOrCreateConfig() const;
    bool haveConfigFile() const;
    bool readFromConfigFile(ConfigSettings& sets) const;
    ConfigSettings defaultSettings() const;
    void applySettings(const ConfigSettings&);
    void saveSettingsToFile(const ConfigSettings&);
    void OnTextEnter(wxCommandEvent& event);

    const wxString _conf_file_name;
    bool _enable_tolk;
    std::optional<wxString> entered_;
    wxTextCtrl* _multiout;
    wxTextCtrl* _linein;
    //std::shared_ptr<Talker> talker_;

    //Подключение таблицы событий
    wxDECLARE_EVENT_TABLE();
};