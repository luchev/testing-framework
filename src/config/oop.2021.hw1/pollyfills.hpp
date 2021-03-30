#pragma once
#include <cstring>

int strcpy_s(char *dest, unsigned int dest_size, const char *src) {
    strcpy(dest, src);
    return 0;
}
