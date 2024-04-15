#pragma once

#include <string>
#include "Macros.h"

class Buffer {

private:
    std::string buf;

public:
    Buffer() = default;
    ~Buffer() = default;

    DISALLOW_COPY_AND_MOVE(Buffer);

    void append(const char* str, int size);
    ssize_t size();
    const char* c_str();
    void clear();
    void getline();
    void setBuf(const char* buf);
};