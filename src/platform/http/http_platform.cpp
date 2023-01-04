#include <httplib.h>
#include <string>
#include <iostream>
#include <map>
#include <fstream>
#include "../platformio.h"
#include "../../logger.h"

using namespace httplib;
using namespace std::literals;

std::string currUserInp;
std::string currResponce;
bool readyResponce = false;

namespace skif {
namespace platform {

class HttpPlatform : public PlatformIo
{
public:
    HttpPlatform(uint16_t port)
    {
        server_.Get("/hi", [](const Request& req, Response& res) {
          res.set_content("Hello World!", "text/plain");
        });

        server_.Get(R"(/input/(.*))", [&](const Request& req, Response& res) {
            //LOG(info) << "inp: "<<req.target;
            auto text = req.matches[1];
            currUserInp = text;
            readyResponce = false;
            while (!readyResponce)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            //LOG(info) << "out: " << currResponce;
            res.set_content(currResponce, "text/plain; charset=utf-8");
            });

        server_.Get("/stop", [&](const Request& req, Response& res) {
            server_.stop();
        });
        //TODO: сделать нормальную многопоточность
        server_thread_ = std::thread([this, port] {
            std::cout << "Started server at http://localhost:" << port<<std::endl;
            server_.listen("localhost", port);
            });
        //http_thread.detach();
    }
    bool sendTextUtf8(const std::string& data) override
    {
        if (wantExit) return true;
        currResponce = data;
        readyResponce = true;
        return true;
    }
    std::optional<std::string> readInputUtf8()
    {
        while (currUserInp.size() == 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            if (wantExit) return {};
        }
        std::string u8s(currUserInp.begin(), currUserInp.end());
        currUserInp.clear();
        return u8s;
    }
    void platformCleanup() override
    {
        LOG(debug) << "Stop HTTP thread"sv;
        if (server_.is_running()) server_.stop();
        server_thread_.join();
    }
    ~HttpPlatform()
    {
        LOG(debug) << "Finish HTTP platform"sv;
    }
private:
    Server server_;
    std::thread server_thread_;
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