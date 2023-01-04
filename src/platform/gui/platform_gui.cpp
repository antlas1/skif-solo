#include "mainframe.h"
#include "../platformio.h"
#include "../../logger.h"
#include <regex>
#include <thread>

//Главное приложение
class App : public wxApp
{
private:
	//wxLanguage m_lang;  // language specified by user
	//wxLocale m_locale;  // locale we'll be using
	std::unique_ptr<MainFrame> mainFrame;
public:
	void DisplayOutText(const wxString& text)
	{
		mainFrame->DisplayOutText(text);
	}
	std::optional<wxString> enteredText() const
	{
		return mainFrame->enteredText();
	}
	
    bool OnInit() override
	{
		//явная установка языка
		//m_lang = wxLANGUAGE_RUSSIAN;
		// don't use wxLOCALE_LOAD_DEFAULT flag so that Init() doesn't return
		// false just because it failed to load wxstd catalog
		//if (!m_locale.Init(m_lang, wxLOCALE_DONT_LOAD_DEFAULT))
		//{
		//	wxLogWarning(_("This language is not supported by the system."));

			// continue nevertheless
		//}

		// normally this wouldn't be necessary as the catalog files would be found
		// in the default locations, but when the program is not installed the
		// catalogs are in the build directory where we wouldn't find them by
		// default
		//wxLocale::AddCatalogLookupPathPrefix(".");

		// Initialize the catalogs we'll be using
		//const wxLanguageInfo* pInfo = wxLocale::GetLanguageInfo(m_lang);
		//if (!m_locale.AddCatalog("skif-gui"))
		//{
			//TODO: вернуть
			//wxLogError(_("Couldn't find/load the 'skif-gui' catalog for locale '%s'."),
			//	pInfo ? pInfo->GetLocaleName() : _("unknown"));
		//}

		mainFrame = std::make_unique<MainFrame>(_("SKIF GUI"), wxPoint(50, 50), wxSize(450, 340));
		mainFrame->Show(true);
		SetTopWindow(mainFrame.get());
		//GLogger::get() << "Load translation: " << ok_load;

		return true;
	}
	int OnExit() override
	{
		mainFrame->Destroy();
		mainFrame.release();
		return wxApp::OnExit();
	}
};

DECLARE_APP(App)
IMPLEMENT_APP_NO_MAIN(App)

using namespace std::literals;

namespace skif {
namespace platform {

class GuiProxyPlatform : public PlatformIo
{
public:
	GuiProxyPlatform()
	{
		gui_thread_ = std::thread([this] {
			app_ = new App();
			wxApp::SetInstance(app_);
			int argc = 0; wxChar** argv = NULL;
			wxEntryStart(argc, argv);
			wxTheApp->CallOnInit();
			wxTheApp->OnRun();
			});
	}
	bool sendTextUtf8(const std::string& data) override
	{
		if (wantExit) return true;
		app_->DisplayOutText(wxString::FromUTF8(data));
		return true;
	}
	std::optional<std::string> readInputUtf8() override
	{
		while (app_->enteredText().has_value() == false)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(10));
			if (wantExit) return {};
		}
		auto wStr = app_->enteredText().value();
		std::string userInp = wStr.utf8_string();
		return userInp;
	}
	void platformCleanup() override
	{
		LOG(debug) << "Stop GUI thread"sv;
		wxTheApp->OnExit();
		LOG(debug) << "Wait exit"sv;
		std::this_thread::sleep_for(std::chrono::milliseconds(100)); //даём время GUI-потоку завершить свой цикл сообщений
		LOG(debug) << "Cleanup"sv;
		wxEntryCleanup();
		gui_thread_.join();
	}
	~GuiProxyPlatform()
	{
		LOG(debug) << "Finish GUI platform"sv;
	}
private:
	App* app_;
	std::thread gui_thread_;
};

}//end namespace platform


std::vector<SpecInitArg> platformInputArgs()
{
	return {};
}

std::shared_ptr<PlatformIo> makePlatform(const ConfiguredArgValue&)
{
	return std::make_shared<platform::GuiProxyPlatform>();
}

}//end namespace skif
