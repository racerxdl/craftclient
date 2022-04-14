#pragma once

#include <jwt-cpp/jwt.h>

struct AuthResult {
    std::shared_ptr<EVP_PKEY> publicKey;
    bool XBOXLiveAuthenticated;
};

struct IdentityData {

};

struct ClientData {

};