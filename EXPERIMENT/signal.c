#include <stdio.h>
#include <signal/signal_protocol.h>
#include <signal/key_helper.h>
#include <openssl/rand.h>

int myrand(uint8_t *data, size_t len, void *user_data)
{
    if(RAND_bytes(data, len)) {
        return 0;
    }
    else {
        return SG_ERR_UNKNOWN;
    }
}

int main(int argc, char **argv)
{
    signal_crypto_provider provider = {
            .random_func = myrand,
            .hmac_sha256_init_func = HMAC_CTX_new,
            .hmac_sha256_update_func = HMAC_Update,
            .hmac_sha256_final_func = HMAC_Final,
            .hmac_sha256_cleanup_func = HMAC_CTX_free,
            .sha512_digest_init_func = SHA512_Init,
            .sha512_digest_update_func = SHA512_Update,
            .sha512_digest_final_func = SHA512_Final,
            .sha512_digest_cleanup_func = EVP_MD_CTX_free,
            .encrypt_func = EVP_aes_256_cbc,
            .decrypt_func = EVP_aes_256_cbc,
            .user_data = 0
    };

    signal_context *global_context;
    signal_context_create(&global_context, 0);
    signal_context_set_crypto_provider(global_context, &provider);
    //signal_context_set_locking_functions(global_context, lock_function,
    //unlock_function);


    ratchet_identity_key_pair *identity_key_pair;
    uint32_t registration_id;
    signal_protocol_key_helper_pre_key_list_node *pre_keys_head;
    session_signed_pre_key *signed_pre_key;


    signal_protocol_key_helper_generate_identity_key_pair(
        &identity_key_pair,
        global_context
    );

    exit(EXIT_SUCCESS);

}
