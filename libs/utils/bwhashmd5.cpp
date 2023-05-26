#include "bwhashmd5.h"

BWHashMd5::BWHashMd5() {
}

BWHashMd5::~BWHashMd5() {
}

BWString BWHashMd5::HashString(const BWString &str) {
    unsigned char hash[MD5_DIGEST_LENGTH + 1];
    unsigned char *data = reinterpret_cast<unsigned char *>
                          (const_cast<char *>(str.c_str()));

    MD5(data, str.size(), hash);

    return getStrHash(hash);
}

BWString BWHashMd5::HashFile(const BWString &path) {
    MD5_CTX ctx;
    char data[1025];
    unsigned char hash[MD5_DIGEST_LENGTH + 1];
    fstream stream(path.c_str(), fstream::in | fstream::binary);

    MD5_Init(&ctx);

    do {
        stream.read(data, 1024);
        MD5_Update(&ctx, reinterpret_cast<unsigned char *>(data), stream.gcount());    
    } while (!stream.eof());

    MD5_Final(hash, &ctx);

    return getStrHash(hash);
}

BWString BWHashMd5::getStrHash(unsigned char *hash) {
    BWString ret = "";
    char tmp[3];

    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
	sprintf(tmp, "%02x", hash[i]);
	ret += tmp;
    }

    return ret;
}

