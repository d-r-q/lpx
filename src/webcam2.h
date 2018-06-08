#ifndef LPX_WEBCAM2_H
#define LPX_WEBCAM2_H

#include <stdint.h>
#include "stream_storage.h"

#define WC_IO     1
#define WC_CAP    2
#define WC_STRG   3
#define WC_THREAD 4

typedef struct Webcam Webcam;

typedef void (*error_callback)(Webcam *webcam, int errcode);

int8_t webcam_init(Storage *storage, char *device, Webcam **webcam, error_callback callback);

int8_t webcam_start_stream(Webcam *webcam, char *train_id);

int8_t webcam_stop_stream(Webcam *webcam);

void webcam_close(Webcam *webcam);

#endif //LPX_WEBCAM2_H
