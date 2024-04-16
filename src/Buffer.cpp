#include "include/Buffer.h"

#include <iostream>

void Buffer::append(const char* str, int size) { buf.append(str, size); }

ssize_t Buffer::size() { return buf.size(); }

void Buffer::clear() { buf.clear(); }

const char* Buffer::c_str() { return buf.c_str(); }

void Buffer::getline() {
    buf.clear();
    std::getline(std::cin, buf);
}

void Buffer::setBuf(const char* _buf) {
    buf.clear();
    buf.append(_buf);
}