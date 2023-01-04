#include <SFML/Network.hpp>
#include <string>
#include <iostream>
#include <map>
#include <fstream>
#include "../platformio.h"

#include "httpserver.h"

using namespace std;
using namespace sf;

void doStuff(UrlChar* chars)
{
    //auto& var = *chars->var;
    //if (var.find("$body") == var.end())
    //{
    //    /*chars->code = 301;
    //    chars->redirectUrl = "/home";*/
    //    return;
    //}
    //// Changes the body variable of the page (in this case the body of the fake mail)
    //chars->var->at("$body") = "Well, I guess not.";
}

std::string currUserInp;
bool allow_redirect = false;

string urlDecode(string& SRC) {
    string ret;
    char ch;
    int i, ii;
    for (i = 0; i < SRC.length(); i++) {
        if (SRC[i] == '%') {
            sscanf(SRC.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            ret += ch;
            i = i + 2;
        }
        else {
            ret += SRC[i];
        }
    }
    return (ret);
}

void processInput(UrlChar* chars)
{
    auto& var = *chars->var;

    if (var.find("$userinp") != var.end())
    {
        //записываем
        currUserInp = urlDecode(var.at("$userinp"));
    }
    else
    {
        //error! no user input
    }

    while (!allow_redirect)
    {
        sf::sleep(sf::milliseconds(10));
    }

    chars->code = 301;
    chars->redirectUrl = "/";
}

const std::string index_start_page = "<!DOCTYPE HTML/>\n"
"<html>\n"
"<head>\n"
"    <title>Home</title>\n"
"    <meta charset=\"utf-8\" />\n"
"</head>\n"
"    <body>\n"
"        <h1>Welcome skif!</h1>\n"
"        <form method=\"post\" action=\"/input\">\n"
"            <label for=\"text\">Text: </label>\n"
"            <input type=\"text\" name=\"userinp\" required />\n"
"            <input type=\"submit\" value=\"Send\" />\n"
"        </form>\n"
"        <b>REPLACE TEXT</b>\n"
"    </body>\n"
"</html>";
namespace skif {
namespace platform {

class HttpPlatform : public PlatformIo
{
public:
    HttpPlatform(uint16_t port):server_(port)
    {
        server_.logs = true;
        // Prepare the server responses
        server_.req("/", 200, doStuff, index_start_page)
            .req("/input", 301, "/", processInput);
        server_launched_ = false;
    }
    bool sendTextUtf8(const std::string& data) override
    {
        if (!server_launched_)
        {
            server_.launch(1);
            server_launched_ = true;
        }
        std::string curr_info(data.begin(), data.end());
        std::string new_index(index_start_page);
        size_t start_pos = new_index.find("REPLACE TEXT");
        if (start_pos == std::string::npos)
            return false;
        new_index.replace(start_pos, "REPLACE TEXT"s.length(), curr_info);

        server_.replacePageContent("/", new_index);
        currUserInp.clear();
        allow_redirect = true;
        //std::string output(data.begin(),data.end());
        //std::cout << output<<"\n>";
        //std::cout.flush();
        return true;
    }
    std::optional<std::string> readInputUtf8()
    {
        while (currUserInp.size() == 0)
        {
            sf::sleep(sf::milliseconds(10));
        }
        allow_redirect = false;
        std::string u8s(currUserInp.begin(), currUserInp.end());
        return u8s;
    }
private:
    HttpServer server_;
    bool server_launched_;
    std::string index_page_;
};

}//end platform

std::vector<skif::SpecInitArg> platformInputArgs()
{
    return std::vector<skif::SpecInitArg>{
        skif::SpecInitArg{ .argName = "port", .argComment = "port for http server", .defaultValue = "8080" }
    };
}

std::shared_ptr<skif::PlatformIo> makePlatform(const skif::ConfiguredArgValue& configuredOptions)
{
    return std::make_shared<platform::HttpPlatform>(std::stoi(configuredOptions.at("port")));
}

}//end skif
/*
int main()
{
	// Create the server and set it on port 8080
	HttpServer server(8080);
	server.logs = false;
	// Prepare the server responses
	server.req("/home", 200, "text/html", "webSrc/index.html")
		.req("/index.css", 200, "text/css", "webSrc/index.css")
		.req("/index.js", 200, "application/javascript", "webSrc/index.js")
		.req("/sendmail", 200, "text/html", "webSrc/sendmail.html", doStuff) // Uses doStuff(UrlChar*) as a callback
		.req("/img", 200, "image/png", "webSrc/img.png")
		.req("/ogg", 200, "audio/ogg", "webSrc/mettaton.ogg")
		.req("/loaderio-4db54377b4e344be0bc09288db5301eb.txt", 200, "text/plain", "webSrc/loaderio-4db54377b4e344be0bc09288db5301eb.txt")
		.req("/", 301, "/home").launch(1);
	return 0;
}
*/