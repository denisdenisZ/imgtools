#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"

e_io_status read_file(const char *filename, uint8_t **out, uint32_t *out_size) {
    *out = NULL;
    *out_size = 0;
    e_io_status status = IO_SUCCESS;

    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "fopen failed for '%s': %s\n", filename, strerror(errno));
        return IO_ERR_NOT_FOUND;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    if (size < 0) {
        fprintf(stderr, "ftell failed for '%s': %s\n", filename, strerror(errno));
        status = IO_ERR_READ;
        goto cleanup;
    }

    *out = malloc(size);
    if (!*out) {
        fprintf(stderr, "malloc failed for '%s': %s\n", filename, strerror(errno));
        status = IO_ERR_OOM;
        goto cleanup;
    }

    if (fread(*out, 1, size, f) != (size_t)size) {
        fprintf(stderr, "fread failed for '%s': %s\n", filename, strerror(errno));
        free(*out);
        *out = NULL;
        status = IO_ERR_READ;
        goto cleanup;
    }

    *out_size = (uint32_t)size;

cleanup:
    fclose(f);
    return status;
}

e_io_status write_file(const char *filename, uint8_t *data, uint32_t size) {
    e_io_status status = IO_SUCCESS;

    FILE *f = fopen(filename, "wb");
    if (!f) {
        fprintf(stderr, "fopen failed for '%s': %s\n", filename, strerror(errno));
        return IO_ERR_NOT_FOUND;
    }

    if (fwrite(data, 1, size, f) != size) {
        fprintf(stderr, "fwrite failed for '%s': %s\n", filename, strerror(errno));
        status = IO_ERR_WRITE;
        goto cleanup;
    }

cleanup:
    fclose(f);
    return status;
}
