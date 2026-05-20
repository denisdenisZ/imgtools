#ifndef IO_H
#define IO_H

#include <stdint.h>

typedef enum {
    IO_SUCCESS,
    IO_ERR_NO_PERMISSION,
    IO_ERR_NOT_FOUND,
    IO_ERR_READ,
    IO_ERR_OOM,
    IO_ERR_WRITE,
} e_io_status;

e_io_status read_file(const char *filename, uint8_t **out, uint32_t *out_size);
e_io_status write_file(const char *filename, uint8_t *data, uint32_t size);

#endif
