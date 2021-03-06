#include <stdio.h>
#include "../include/lpxstd.h"
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <dirent.h>
#include <list.h>

/*
 * Переводит значение struct timeval в микросекунды.
 */
uint64_t tv2mks(struct timeval tv) {
    return tv.tv_sec * 1000000ULL + tv.tv_usec;
}

/*
 * Переводит значение struct timeval в миллисекунды.
 */
uint64_t tv2ms(struct timeval tv) {
    return (tv.tv_sec * 1000ULL) + (tv.tv_usec / 1000ULL);
}

void print_array(char *prefix, const unsigned char *arr, int size) {
    printf("%s", prefix);
    for (int i = 0; i < size; i++) {
        printf("0x%02X ", arr[i]);
    }
    printf("\n");
}

void *xmalloc(size_t n) {
    void *p = malloc(n);
    if (p == NULL) {
        // На линуксе вроде как никогда не случается - когда кончается память, линукс запускает OOM-killer,
        // который убивает случайные процессы, чтобы освободить память, так что скорее должен прилететь SIGKILL
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

int8_t fd_size(int fd, off_t *size) {
    struct stat buf;
    if (fstat(fd, &buf) == -1) {
        return LPX_IO;
    }
    *size = buf.st_size;

    return LPX_SUCCESS;
}

char *itoa(uint64_t i) {
    char *res = xmalloc(MAX_INT_LEN);
    snprintf(res, MAX_INT_LEN, "%" PRId64, i);
    return res;
}


int8_t list_directory(char *dir, char ***children, size_t *children_size) {
    DIR *dp;
    struct dirent *dir_entry;

    dp = opendir(dir);
    List *chldrn = lst_create();
    if (dp != NULL) {
        while ((dir_entry = readdir(dp)) != NULL) {
            if (strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0) {
                continue;
            }
            char *name = strndup(dir_entry->d_name, 256);
            lst_append(chldrn, name);
        }
        closedir(dp);
        size_t entries = lst_size(chldrn);
        char **res = xcalloc(entries, sizeof(char*));
        lst_to_array(chldrn, (const void **) res);
        *children = res;
        *children_size = entries;
        lst_free(chldrn);
    } else {
        perror("Couldn't open the directory");
        return LPX_IO;
    }

    return LPX_SUCCESS;
}

void free_array(void **arr, size_t size) {
    for (int i = 0; i < size; i++) {
        free(arr[i]);
    }
    free(arr);
}

bool starts_with(const char* str, const char *prefix) {
    size_t prefix_size = strlen(prefix);
    return strncmp(str, prefix, prefix_size) == 0;
}

int uint64_t_cmp(const void *a, const void *b) {
    const uint64_t *ua = (const uint64_t*) a;
    const uint64_t *ub = (const uint64_t*) b;
    if (*ua < *ub) {
        return -1;
    } else if (*ua > *ub) {
        return 1;
    } else {
        return 0;
    }
}