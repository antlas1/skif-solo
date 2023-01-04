#include <iostream>
#include <exception>
#include <regex>
#include <SFML/Network.hpp>
#include "../platformio.h"

namespace skif {
namespace platform {

class SfmlTcpPlatform : public PlatformIo
{
public:
    SfmlTcpPlatform(std::string ip, uint16_t port):
       ip_(ip),
       port_(port)
    {
    }
    
    void platformInit() override
    {
        // Listen to the given port for incoming connections
        if (listener_.listen(port_, ip_) != sf::Socket::Done)
            throw std::runtime_error("Cannot start listening");
        std::cout << "Server is listening to port " << port_ << ", waiting for connections... " << std::endl;

        // Wait for a connection
        if (listener_.accept(socket_) != sf::Socket::Done)
            throw std::runtime_error("Cannot connect to socket");
        std::cout << "Client connected: " << socket_.getRemoteAddress() << std::endl;
    }
    
    bool sendTextUtf8(const std::string& data) override
    {
        if (socket_.send(data.c_str(), data.size()) == sf::Socket::Done)
        {
            socket_.send("\r\n>", 3);
            return true;
        }
        return false;
    }
    std::optional<std::string> readInputUtf8() override
    {
        using namespace std::string_literals;
        char in[256]{ 0 };
        std::size_t received;
        if (socket_.receive(in, sizeof(in), received) != sf::Socket::Done)
            return {};
        std::string inp(in);
        inp = std::regex_replace(inp, std::regex("\\r\\n|\\r|\\n"s), ""s);
        return std::string(inp.begin(),inp.end());
    }
    bool isSynchroIo() const
    {
        return true;
    }
private:
    const std::string ip_;
    const uint16_t port_;
    sf::TcpListener listener_;         // Create a server socket to accept new connections
    sf::TcpSocket socket_;
}; 

}//end platform

//std::shared_ptr<PlatformIo> makeTelnetPlatform(std::string ip, uint16_t port)
//{
//    return std::make_shared<platform::SfmlTcpPlatform>(ip, port);
//}
std::vector<skif::SpecInitArg> platformInputArgs()
{
    return std::vector<skif::SpecInitArg>{
        skif::SpecInitArg{ .argName = "ip", .argComment = "ip for set telnet", .defaultValue = "127.0.0.1" },
        skif::SpecInitArg{ .argName = "port", .argComment = "port for set telnet", .defaultValue = "9000" }
    };
}

std::shared_ptr<skif::PlatformIo> makePlatform(const skif::ConfiguredArgValue& configuredOptions)
{
    return std::make_shared<platform::SfmlTcpPlatform>(configuredOptions.at("ip"), std::stoi(configuredOptions.at("port")));
}

}//end skif