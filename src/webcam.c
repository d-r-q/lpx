#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <errno.h>
#include <linux/videodev2.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include <time.h>
#include <poll.h>
#include <stdbool.h>
#include <assert.h>
#include "lpxstd.h"

static char *outDir;

static int outFd;

static int outIdxFd;

static int webcamFd;

static struct pollfd pfd = {0};

static struct v4l2_buffer bufferinfo = {0};

static uint8_t *buffer;

static bool streaming = false;

static struct timeval frameRequestTime;

static struct timeval frameReadyTime;

static uint32_t frameOffset;

int8_t webcam_init(char *_outDir) {
    outDir = _outDir;
    if ((webcamFd = open("/dev/video0", O_RDWR)) < 0) {
        perror("open");
        return -1;
    }
    pfd.fd = webcamFd;
    pfd.events = POLLIN;

    struct v4l2_capability cap = {};
    if (ioctl(webcamFd, VIDIOC_QUERYCAP, &cap) < 0) {
        perror("VIDIOC_QUERYCAP");
        return -1;
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf(stderr, "The device does not handle signle-planar video capture.\n");
        return -1;
    }

    struct v4l2_format format;
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    format.fmt.pix.width = 640;
    format.fmt.pix.height = 480;
    format.fmt.pix.field = V4L2_FIELD_NONE;
    if (ioctl(webcamFd, VIDIOC_S_FMT, &format) < 0) {
        perror("VIDIOC_S_FMT");
        return -1;
    }

    struct v4l2_requestbuffers bufrequest = {0};
    bufrequest.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufrequest.memory = V4L2_MEMORY_MMAP;
    bufrequest.count = 1;
    if (ioctl(webcamFd, VIDIOC_REQBUFS, &bufrequest) < 0) {
        perror("VIDIOC_REQBUFS");
        return -1;
    }

    bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufferinfo.memory = V4L2_MEMORY_MMAP;
    bufferinfo.index = 0;
    if (ioctl(webcamFd, VIDIOC_QUERYBUF, &bufferinfo) < 0) {
        perror("VIDIOC_QUERYBUF");
        return -1;
    }


    buffer = mmap(NULL, bufferinfo.length, PROT_READ | PROT_WRITE, MAP_SHARED, webcamFd, bufferinfo.m.offset);
    if (buffer == MAP_FAILED) {
        perror("mmap");
        return -1;
    }
}

struct pollfd webcam_fd() {
    return pfd;
}

int8_t webcam_start_stream(int64_t trainId) {
    // todo: обработать ситуацию когда стрим уже идёт
    printf("starting stream\n");
    char buf[256];
    sprintf(buf, "%s/%ld.mjpeg", outDir, trainId);
    if ((outFd = open(buf, O_WRONLY | O_CREAT, 0660)) < 0) {
        perror("open stream file");
        return -1;
    }
    sprintf(buf, "%s/%ld.idx", outDir, trainId);
    if ((outIdxFd = open(buf, O_WRONLY | O_CREAT, 0660)) < 0) {
        perror("open index file");
        return -1;
    }
    frameOffset = 0;

    if (-1 == ioctl(webcamFd, VIDIOC_STREAMON, &bufferinfo.type)) {
        perror("Start Capture");
        return -1;
    }

    if (ioctl(webcamFd, VIDIOC_QBUF, &bufferinfo) < 0) {
        perror("VIDIOC_QBUF");
        return -1;
    }
    gettimeofday(&frameRequestTime, NULL);
    streaming = true;
}


int8_t webcam_handle_frame(int64_t trainId, bool last) {
    gettimeofday(&frameReadyTime, NULL);
    printf("handling frame\n");

    assert(streaming && "Handle frame while not streaming");

    if (-1 == ioctl(webcamFd, VIDIOC_DQBUF, &bufferinfo)) {
        perror("Retrieving Frame");
        return -1;
    }
    dprintf(outIdxFd, "%d,%d,%ld,%ld\n", frameOffset, bufferinfo.length, toMicroSeconds(frameRequestTime), toMicroSeconds(frameReadyTime));
    write(outFd, buffer, bufferinfo.length);
    frameOffset += bufferinfo.length;

    if (last) {
        printf("stopping stream\n");
        if (ioctl(webcamFd, VIDIOC_STREAMOFF, &bufferinfo.type) < 0) {
            perror("VIDIOC_STREAMOFF");
            // TODO: can try to stream on later?
        }
        close(outFd);
        outFd = 0;
        close(outIdxFd);
        outIdxFd = 0;
        streaming = false;
    } else {
        if (ioctl(webcamFd, VIDIOC_QBUF, &bufferinfo) < 0) {
            perror("VIDIOC_QBUF");
            return -1;
        } else {
            gettimeofday(&frameRequestTime, NULL);
        }
    }
}

bool webcam_streaming() {
    return streaming;
}

void webcam_close() {
    if (streaming) {
        if (ioctl(webcamFd, VIDIOC_STREAMOFF, &bufferinfo.type) < 0) {
            perror("VIDIOC_STREAMOFF");
        }
    }
    if (outFd) {
        close(outFd);
    }
    if (webcamFd) {
        close(webcamFd);
    }
}
