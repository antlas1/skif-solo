#include <iostream>
#include <string>
#include "../platformio.h"

#ifdef WIN32
#include <locale.h>
#include <vector>
#include <Windows.h>
std::string utf8_to_cp1251(std::string const& utf8)
{
    if (!utf8.empty())
    {
        int wchlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), NULL, 0);
        if (wchlen > 0 && wchlen != 0xFFFD)
        {
            std::vector<wchar_t> wbuf(wchlen);
            int result_u = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), &wbuf[0], wchlen);
            if (!result_u) {
                throw std::runtime_error("Skif utf8_to_cp1251 cannot convert MultiByteToWideChar!");
            }
            std::vector<char> buf(wchlen);
            int result_c = WideCharToMultiByte(1251, 0, &wbuf[0], wchlen, &buf[0], wchlen, 0, 0);
            if (!result_c) {
                throw std::runtime_error("Skif utf8_to_cp1251 cannot convert WideCharToMultiByte!");
            }

            return std::string(&buf[0], wchlen);
        }
    }
    return std::string();
}

std::string cp1251_to_utf8(const std::string& cp1251) {
    std::string res;
    int result_u, result_c;
    enum { CP1251 = 1251};
    result_u = MultiByteToWideChar(CP1251, 0, cp1251.c_str(), -1, 0, 0);
    if (!result_u) {
        throw std::runtime_error("Skif cp1251_to_utf8 cannot convert MultiByteToWideChar!");
    }
    wchar_t* ures = new wchar_t[result_u];
    if (!MultiByteToWideChar(CP1251, 0, cp1251.c_str(), -1, ures, result_u)) {
        delete[] ures;
        throw std::runtime_error("Skif cp1251_to_utf8 cannot convert MultiByteToWideChar 2!");
    }
    result_c = WideCharToMultiByte(CP_UTF8, 0, ures, -1, 0, 0, 0, 0);
    if (!result_c) {
        delete[] ures;
        throw std::runtime_error("Skif cp1251_to_utf8 cannot convert WideCharToMultiByte!");
    }
    char* cres = new char[result_c];
    if (!WideCharToMultiByte(CP_UTF8, 0, ures, -1, cres, result_c, 0, 0)) {
        delete[] cres;
        throw std::runtime_error("Skif cp1251_to_utf8 cannot convert WideCharToMultiByte 2!");
    }
    delete[] ures;
    res.append(cres);
    delete[] cres;
    return res;
}

#endif

namespace skif {
namespace platform {

class ConsolePlatform : public PlatformIo
{
public:
    ConsolePlatform() 
    {
#ifdef WIN32
        setlocale(LC_ALL, "RUS");
        system("chcp 1251");
#endif
    }
    bool sendTextUtf8(const std::string& data) override
    {
        std::string output = data;
#ifdef WIN32
        output = utf8_to_cp1251(output);
#endif
        std::cout << output<<"\n>";
        std::cout.flush();
        return true;
    }
    std::optional<std::string> readInputUtf8() override
    {
        std::string input;
        std::getline(std::cin, input);
#ifdef WIN32
        input = cp1251_to_utf8(input);
#endif
        return input;
    }
};

}//end platform

std::vector<SpecInitArg> platformInputArgs()
{
    return {};
}

std::shared_ptr<PlatformIo> makePlatform(const ConfiguredArgValue&)
{
    return std::make_shared<platform::ConsolePlatform>();
}

}//end skif