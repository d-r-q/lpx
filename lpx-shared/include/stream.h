#ifndef LPX_STREAM_H
#define LPX_STREAM_H

/**
 * Ошибка генерации потока архива стрима
 */
#define STRM_IO -2

/**
 * Структура записи в индексе потока
 */
typedef struct FrameMeta {
    int64_t start_time; // ситемное (астрономическое) время запроса фрейма в микросекундах
    int64_t end_time; // систмное (астрономическое) время получения фрейма в микросекундах
} FrameMeta;

/**
 * Поток байт фреймов видео-потока.
 * BNF формата потока:
 * поток ::= <кол-во фреймов>(<eof> | <фрейм>)
 * eof ::= <0 байт>
 * кол-во фреймов ::= 32-битное беззнаковое число (little endian)
 * фрейм :: = <имя файла><размер файла><n байт файла>
 * имя файла ::= строка в кодировке ascii с завершающим нулём
 * размер файла ::= 64-битное беззнаковое число (little endian)
 * n байт файла ::= массив байт длинной n
 */
typedef struct VideoStreamBytesStream VideoStreamBytesStream;

/**
 * Поиск индекса фрейма ближайшего к заданному временному смещению относительно начала стрима
 * index - индекс фреймов
 * index - длина индекса фреймов
 * time_offset - время в микросекундах относительно момента начала стриминга (времени запроса первого фрейма)
 */
ssize_t stream_find_frame(FrameMeta **index, size_t index_size, uint64_t time_offset);

/**
 * Поиск индекса фрема ближайшего к заданному астрономическому времени
 * index - индекс фреймов
 * index - длина индекса фреймов
 * time_offset - астрономическое время в микросекундах
 */
ssize_t stream_find_frame_abs(FrameMeta **index, size_t index_size, uint64_t time);

/**
 * Инициализирует структура архива потока, содержащего заданные файлы. В случае ошибки возвращает NULL.
 */
VideoStreamBytesStream *stream_open(char **files, size_t files_size);

/**
 * Записывает до `max` байт архива в буффер. Возвращает количество реально записанных байт, EOF в случае
 * когда стрим был целиком прочитан и STRM_IO в случае ошибок генерации архива стрима
 */
ssize_t stream_read(VideoStreamBytesStream *stream, uint8_t *buf, size_t max);

/**
 * Закрывет архив и освобождает все ресурсы
 */
void stream_close(VideoStreamBytesStream *stream);

#endif //LPX_STREAM_H
