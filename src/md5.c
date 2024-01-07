#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>
#include <openssl/evp.h>
#include "md5.h"

char* genHash(const char *string) {
	size_t length = strlen(string);
	EVP_MD_CTX *mdctx;
	const EVP_MD *md;
	unsigned char digest[MD5_DIGEST_LENGTH];
	char *hash = (char*) malloc(2 * MD5_DIGEST_LENGTH + 1);

	md = EVP_md5();
	mdctx = EVP_MD_CTX_new();
	EVP_DigestInit_ex(mdctx, md, NULL);

	EVP_DigestUpdate(mdctx, string, length);

	EVP_DigestFinal_ex(mdctx, digest, NULL);

	EVP_MD_CTX_free(mdctx);


	return hash;
}
