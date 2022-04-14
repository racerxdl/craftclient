#include "login.h"

const std::string mojangPublicKey = "-----BEGIN PUBLIC KEY-----\nMHYwEAYHKoZIzj0CAQYFK4EEACIDYgAE8ELkixyLcwlZryUQcu1TvPOmI2B7vX83ndnWRUaXm74wFfa5f/lwQNTfrLVHa2PmenpGI6JhIMUJaWZrjmMj90NoKNFSNBuKdm8rYiXsfaz3K36x/1U26HpG0ZxK/V1V\n-----END PUBLIC KEY-----";
std::shared_ptr<EVP_PKEY> mojangKey;

void init() {
    mojangKey = jwt::helper::load_public_key_from_string(mojangPublicKey);
}