#pragma once

#include "common/common.h"
#include "common/crypto.h"
#include "protocol/clientData.h"
#include "protocol/identityData.h"
#include "protocol/authResult.h"

namespace ProtoRock {
namespace Login {

const std::string MojangPublicKey = "-----BEGIN PUBLIC KEY-----\nMHYwEAYHKoZIzj0CAQYFK4EEACIDYgAE8ELkixyLcwlZryUQcu1TvPOmI2B7vX83ndnWRUaXm74wFfa5f/lwQNTfrLVHa2PmenpGI6JhIMUJaWZrjmMj90NoKNFSNBuKdm8rYiXsfaz3K36x/1U26HpG0ZxK/V1V\n-----END PUBLIC KEY-----";

struct ParsedLogin {
    Protocol::IdentityData iData;
    Protocol::ClientData cData;
    Protocol::AuthResult result;
};

Common::ByteBuffer Encode(const std::string &chainData, const Protocol::ClientData &clientData, const Crypto::KeyPair &keyPair);
Common::ByteBuffer EncodeOffline(const Protocol::IdentityData &identityData, const Protocol::ClientData &clientData, const Crypto::KeyPair &keyPair);
ParsedLogin Parse(const Common::ByteBuffer &request);
}
}  // namespace ProtoRock