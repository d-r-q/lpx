#ifndef LPX_LPXSTD_H
#define LPX_LPXSTD_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define ALEN(arr) ((sizeof (arr)) / sizeof ((arr)[0]))

// Коды статусов вызова функций
#define LPX_SUCCESS 0
#define LPX_IO      1

#define MAX_INT_LEN 20

uint64_t tv2mks(struct timeval tv);

uint64_t tv2ms(struct timeval tv);

void print_array(char *prefix, const unsigned char *arr, int size);

void* xmalloc(size_t n);

void* xcalloc(size_t n, size_t size);

/*
 * Добавление child к пути base через "/"
 */
char* append_path(char *base, char* child);

int8_t file_size(FILE *file, off_t *size);

int8_t fd_size(int fd, off_t *size);

/*
 * Перевод числа в строку
 */
char *itoa(uint64_t i);

int8_t list_directory(char *dir, char ***children, size_t *children_size);

void free_array(void **arr, size_t size);

bool starts_with(const char *str, const char *prefix);

int uint64_t_cmp(const void *a, const void *b);

#endif //LPX_LPXSTD_H
