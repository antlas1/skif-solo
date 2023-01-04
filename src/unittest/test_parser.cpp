#include <catch2/catch_test_macros.hpp>
#include <functional>
#include "../engine/programoptions.h"
#include "../logger.h"

using namespace skif::engine;

TEST_CASE( "Parsing empty ok" ) {
    std::vector<skif::SpecInitArg> spec{};
    auto config = parse(0, nullptr, spec);
    CHECK(config.has_value());
    CHECK(config.value().size() == 0);
}

TEST_CASE("Parsing invalid option prefix") {
    std::vector<skif::SpecInitArg> spec{};
    const char* n_argv[] = { "programName", "option-no--" };
    auto config = parse(2, const_cast<char**>(n_argv), spec);
    CHECK(config.has_value() == false);
}

TEST_CASE("Parsing notspec args ok") {
    std::vector<skif::SpecInitArg> spec{};
    const char* n_argv[] = { "programName", "--notused1", "2" };
    auto config = parse(3, const_cast<char**>(n_argv), spec);
    CHECK(config.has_value());
    CHECK(config.value().size() == 0);
}

TEST_CASE("Parsing invalid count arg") {
    std::vector<skif::SpecInitArg> spec{};
    const char* n_argv[] = { "programName", "--notused1", "2", "--ups"};
    auto config = parse(4, const_cast<char**>(n_argv), spec);
    CHECK(config.has_value()==false);
}