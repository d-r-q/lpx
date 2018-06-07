#ifndef LPX_STREAM_H
#define LPX_STREAM_H

#include <stdint.h>

/**
 * Структура записи в индексе потока
 */
typedef struct FrameMeta {
    int64_t start_time;
    int64_t end_time;
} FrameMeta;

#endif //LPX_STREAM_H
