#include "mainframe.h"
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/wfstream.h>
#include <wx/fileconf.h>
#include <wx/timer.h>
#include "../platformio.h"
#include "../../logger.h"

//Идентификаторы для команд
enum { ID_ShowLog = 1, 
    ID_OpenGame, 
    ID_OpenSettings, 
    ID_OpenManager, 
    ID_StartDialog, 
    ID_MainframeTimer,
    WX_PARSER_ID
};

// clang-format off
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(ID_OpenSettings, MainFrame::OpenSettings)
EVT_MENU(wxID_EXIT, MainFrame::OnExit)
EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
EVT_CLOSE(MainFrame::OnClose)
EVT_TEXT_ENTER(WX_PARSER_ID, MainFrame::OnTextEnter)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title,
    const wxPoint& pos,
    const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size),
    _conf_file_name("config.ini"),
    _enable_tolk(true)
{
    wxMenu* menuFile = new wxMenu();
    //menuFile->Append(ID_OpenGame, _("&Open\tCtrl-O"));
    //menuFile->Append(ID_ShowLog, _("&Log\tCtrl-L"));
    menuFile->Append(ID_OpenManager, _("&Manager\tCtrl-M"));
    menuFile->Append(ID_OpenSettings, _("&Settings\tCtrl-S"));
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT, _("&Quit"));

    wxMenu* menuHelp = new wxMenu();
    menuHelp->Append(wxID_ABOUT, _("&About"));

    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _("&File"));
    menuBar->Append(menuHelp, _("&Help"));

    SetMenuBar(menuBar);
    CreateStatusBar();

    //_panel = new CentralPanel(entered_, this, 10, 10, 600, 600);
    //_panel->GetSizer()->Fit(this);

    _multiout = new wxTextCtrl(this, wxID_ANY, _("Help"),
        wxPoint(10, 120), wxSize(500, 300), wxTE_MULTILINE | wxTE_READONLY);
    _multiout->SetInsertionPoint(0);
    _linein = new wxTextCtrl(this, WX_PARSER_ID, _(""),
        wxDefaultPosition, wxDefaultSize,
        wxTE_PROCESS_ENTER);
    _linein->SetInsertionPoint(0);
    // lay out the controls
    wxBoxSizer* column1 = new wxBoxSizer(wxVERTICAL);
    column1->Add(_multiout, 1, wxALL | wxEXPAND, 10);
    column1->Add(_linein, 0, wxALL | wxEXPAND, 10);

    SetSizer(column1);
    _linein->SetFocus();

    //применяем конфиг
    applySettings(getOrCreateConfig());

    //timer_ = new wxTimer(this, ID_MainframeTimer);

    //talker_ = makeTalker();
    //talker_->speak("");
}

void MainFrame::OpenSettings(wxCommandEvent& event)
{
    ConfigSettings sets(getOrCreateConfig());
    SettingsDialog* dlg = new SettingsDialog(sets,this,0,_("Settings"));
    if (dlg->ShowModal() == wxID_OK)
    {
        ConfigSettings newSets(dlg->newSettings());
        applySettings(newSets);
        saveSettingsToFile(newSets);
    }
    //else: dialog was cancelled or some another button pressed
    dlg->Destroy();
}

void MainFrame::OnTextEnter(wxCommandEvent& event)
{
    //wxLogMessage(wxT("Event string: %s"), event.GetString());
    //wxString encoded_string(event.GetString(), wxCSConv(wxT("cp-1251")));
    //std::string inp(event.GetString().c_str());
    //eneteredText_ = event.GetString().mb_str(wxConvUTF8);
    entered_ = event.GetString();
    auto pinp = static_cast<wxTextCtrl*>(event.GetEventObject());
    if (pinp)
    {
        pinp->Clear();
    }
}

bool MainFrame::haveConfigFile() const
{
    return (wxFileConfig(_("SKIF_APP"), _("VENDOR"), _conf_file_name, wxEmptyString, wxCONFIG_USE_LOCAL_FILE).GetNumberOfEntries() > 0);
}

bool MainFrame::readFromConfigFile(ConfigSettings& sets) const
{
    wxFileConfig config(_("SKIF_APP"),_("VENDOR"), _conf_file_name, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
    ConfigSettings new_sets;
    //TODO: сделать элегантнее
    if (!config.Read("use_screenreader", &new_sets.use_screenreader))
    {
        wxLogError(_("Couldn't read use_screenreader value from config."));
        return false;
    }
    
    sets = new_sets;
    return true;
}

ConfigSettings MainFrame::getOrCreateConfig() const
{
    ConfigSettings sets(defaultSettings());
    //if file exist, parse
    if (haveConfigFile())
    {
        readFromConfigFile(sets);
    }
    //else create and save default settings
    return sets;
}

ConfigSettings MainFrame::defaultSettings() const
{
    ConfigSettings sets;
    sets.use_screenreader = true;
    //add default settings
    return sets;
}

void MainFrame::applySettings(const ConfigSettings& sets)
{
    _enable_tolk = sets.use_screenreader;
    if (!_enable_tolk)
    {
        //if (talker_) talker_->stopSpeak();
    }
}

void MainFrame::saveSettingsToFile(const ConfigSettings& sets)
{
    wxFileConfig config(_("SKIF_APP"), _("VENDOR"), _conf_file_name,wxEmptyString,wxCONFIG_USE_LOCAL_FILE);
    config.EnableAutoSave();
    config.Write("use_screenreader", sets.use_screenreader);
    config.Flush();
}

void MainFrame::ClearOutText()
{
    _multiout->Clear();
   // if (_tolker->IsSpeaking()) _tolker->Silence();
}

void MainFrame::DisplayOutText(const wxString &text)
{
    _multiout->Clear();
    _multiout->AppendText(text);
    //clear entered
    entered_ = {};
    /*
    if (_enable_tolk && talker_ && text.size() > 0)
    {
        talker_->speak(text);
        //LogInfo("Ok tolk? "+std::to_string(ok) + " :" + decoded_string + "\n");
    }
    */
}

std::optional<wxString> MainFrame::enteredText() const
{
    return entered_;
}

void MainFrame::DisplayStatus(std::string text)
{
    SetStatusText(text.c_str());
}

void MainFrame::OnExit(wxCommandEvent& event)
{ 
    LOG(debug) << "GUI start exit";
    wxTheApp->ExitMainLoop(); //останавливаем цикл сообщений, чтобы не было проблем с остановкой потоков
    skif::wantExit = true;
	//Close(true); 
}

void MainFrame::OnClose(wxCloseEvent& event)
{
    LOG(debug) << "GUI start close";
    wxTheApp->ExitMainLoop(); //останавливаем цикл сообщений, чтобы не было проблем с остановкой потоков
    skif::wantExit = true;
    //Destroy();
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    //wxString currReader(_tolker->DetectScreenReader());
    wxString currReader;
    //if (talker_) currReader = talker_->currentScreenReader();
    wxMessageBox(_("Universal text games interpreter. \n Current screen reader: ")+ currReader, _("About"),
        wxOK | wxICON_INFORMATION);
}