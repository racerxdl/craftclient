#include "oauth2/live.h"

#include <cpr/cpr.h>
#include <fmt/format.h>

using namespace ProtoRock::OAuth2;

const std::string scope = "service::user.auth.xboxlive.com::MBI_SSL";

DeviceAuthConnect ProtoRock::OAuth2::StartDeviceAuth() {
    auto r = cpr::Post(cpr::Url{"https://login.live.com/oauth20_connect.srf"},
                       cpr::Payload{
                           {"client_id", minecraftClientId},
                           {"scope", scope},
                           {"response_type", "device_code"},
                       });

    if (r.error) {
        throw Exception(fmt::format("POST https://login.live.com/oauth20_connect.srf: {}", r.error.message));
    }

    if (r.status_code != 200) {
        throw Exception(fmt::format("POST https://login.live.com/oauth20_connect.srf: {}", r.status_line));
    }

    return DeviceAuthConnect(r.text);
}

bool ProtoRock::OAuth2::PollDeviceAuth(const std::string &deviceCode, Token &tkn) {
    auto r = cpr::Post(cpr::Url{"https://login.live.com/oauth20_token.srf"},
                       cpr::Payload{
                           {"client_id", minecraftClientId},
                           {"grant_type", "urn:ietf:params:oauth:grant-type:device_code"},
                           {"device_code", deviceCode},
                       });
    if (r.error) {
        throw Exception(fmt::format("POST https://login.live.com/oauth20_token.srf: {}", r.error.message));
    }

    auto d = DeviceAuthPoll(r.text);
    if (d.Error == "authorization_pending") {
        return false;
    } else if (d.Error.empty()) {
        tkn = d;
        return true;
    }
    throw Exception(fmt::format("Device Auth error: {}", d.Error));
}

void ProtoRock::OAuth2::RefreshToken(Token &tkn) {
    auto r = cpr::Post(cpr::Url{"https://login.live.com/oauth20_token.srf"},
                       cpr::Payload{
                           {"client_id", minecraftClientId},
                           {"scope", scope},
                           {"grant_type", "refresh_token"},
                           {"refresh_token", tkn.RefreshToken},
                       });
    if (r.error) {
        throw Exception(fmt::format("POST https://login.live.com/oauth20_token.srf: {}", r.error.message));
    }
    if (r.status_code != 200) {
        throw Exception(fmt::format("POST https://login.live.com/oauth20_token.srf: {}", r.status_line));
    }
    auto d = DeviceAuthPoll(r.text);
    if (!d.Error.empty()) {
        throw Exception(fmt::format("Error renewing token: {}", d.Error));
    }

    tkn = d;
}