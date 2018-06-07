#include <stdio.h>
#include "lpxstd.h"
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>

uint64_t tv2mks(struct timeval tv) {
    return tv.tv_sec * 1000000ULL + tv.tv_usec;
}

uint64_t s2mks(uint32_t seconds) {
    return seconds * 1000000ULL;
}

void printArray(char *prefix, const unsigned char *arr, int len) {
    printf("%s", prefix);
    for (int i = 0; i < len; i++) {
        printf("0x%02X ", arr[i]);
    }
    printf("\n");
}

void *xmalloc(size_t n) {
    void *p = malloc(n);
    if (p == NULL) {
        fprintf(stderr, "Could not allocate %ld bytes", n);
        abort();
    }
    return p;
}

void *xcalloc(size_t n, size_t size) {
    void *p = calloc(n, size);
    if (p == NULL) {
        fprintf(stderr, "Could not allocate %ld bytes", n);
        abort();
    }
    return p;
}

char *append_path(char *base, char *child) {
    size_t size = sizeof(char) * (strlen(base) + 1 + strlen(child) + 1);
    char *path = xcalloc(size, sizeof(char));
    snprintf(path, size, "%s/%s", base, child);
    return path;
}

int8_t file_size(FILE *file, off_t *size) {
    int fd = fileno(file);
    struct stat buf;
    if (fstat(fd, &buf) == -1) {
        return LPX_IO;
    }
    *size = buf.st_size;

    return LPX_SUCCESS;
}