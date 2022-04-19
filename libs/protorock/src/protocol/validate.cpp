#include "protocol/validate.h"


#include <regex>
#include <stdexcept>

#include <fmt/core.h>
#include <asio.hpp>
#include <jsoncpp/json/json.h>

#include "common/base64.h"
#include "common/common.h"
using namespace ProtoRock;

auto checkUsernameReg = std::regex("[A-Za-z0-9 ]");

bool ProtoRock::Validate::Username(const std::string &v) {
    return std::regex_match(v, checkUsernameReg);
}

auto checkVersionReg = std::regex("[0-9.]");

bool ProtoRock::Validate::Version(const std::string &v) {
    return std::regex_match(v, checkVersionReg);
}

bool ProtoRock::Validate::UUID(const std::string &v) {
    try {
        Common::UUID::Parse(v);
    } catch(Common::Exception) {
        return false;
    }
    return true;
}

bool ProtoRock::Validate::StringToInt(const std::string &v) {
    try {
        stol(v);
        return true;
    } catch (std::invalid_argument) {
        return false;
    } catch (std::out_of_range) {
        return false;
    }
}

bool ProtoRock::Validate::Address(const std::string &v) {
    asio::io_service io_service;
    asio::ip::udp::resolver resolver(io_service);
    asio::ip::udp::resolver::query q(v);
    auto iter = resolver.resolve(q);
    return iter.size() > 0;
}

Error ProtoRock::Validate::Base64DecodeLength(const std::string &v, int len) {
    try {
        auto k = Base64::Decode(v);
        if (k.size() == len) {
            return Error(fmt::format("expected decoded length to be {} got {}", len, k.size()));
        }
    } catch (std::invalid_argument &e) {
        return Error(std::string(e.what()));
    }

    return Error();
}

bool ProtoRock::Validate::Base64(const std::string &v) {
    try {
        auto k = Base64::Decode(v);
        return true;
    } catch (std::invalid_argument &e) {
        return false;
    }
}

bool ProtoRock::Validate::JSON(const std::string &v) {
    Json::Reader reader;
    Json::Value root;
    return reader.parse(v, root);
}