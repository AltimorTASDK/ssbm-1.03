#pragma once

#include <cstddef>

extern "C" {

size_t File_GetLength(const char *path);
void File_Read(const char *path, void *buffer, size_t *size);

} // extern "C"