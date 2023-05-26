#ifndef BWHASH_H
#define BWHASH_H

#include <bwstring.h>

class BWHash {
    public:
        BWHash();
        virtual ~BWHash();

        virtual BWString HashString(const BWString &) = 0;
        virtual BWString HashFile(const BWString &) = 0;
};

#endif // BWHASH_H
