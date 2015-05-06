/*******************************************************************************
#	 	luvcview: Sdl video Usb Video Class grabber          .         #
#This package work with the Logitech UVC based webcams with the mjpeg feature. #
#All the decoding is in user space with the embedded jpeg decoder              #
#.                                                                             #
# 		Copyright (C) 2005 2006 Laurent Pinchart &&  Michel Xhaard     #
#                                                                              #
# This program is free software; you can redistribute it and/or modify         #
# it under the terms of the GNU General Public License as published by         #
# the Free Software Foundation; either version 2 of the License, or            #
# (at your option) any later version.                                          #
#                                                                              #
# This program is distributed in the hope that it will be useful,              #
# but WITHOUT ANY WARRANTY; without even the implied warranty of               #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                #
# GNU General Public License for more details.                                 #
#                                                                              #
# You should have received a copy of the GNU General Public License            #
# along with this program; if not, write to the Free Software                  #
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    #
#                                                                              #
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <linux/videodev2.h>
#include "../../mjpg_streamer.h"
#include "avilib.h"

#include "uvcvideo.h"
#include "dynctrl-logitech.h"


#define NB_BUFFER 4

#define IOCTL_RETRY 4

/* ioctl with a number of retries in the case of I/O failure
* args:
* fd - device descriptor
* IOCTL_X - ioctl reference
* arg - pointer to ioctl data
* returns - ioctl result
*/
int xioctl(int fd, int IOCTL_X, void *arg);

#define DHT_SIZE 432



struct vdIn {
    int fd;
    char *videodevice;
    char *status;
    char *pictName;
    struct v4l2_capability cap;
    struct v4l2_format fmt;
    struct v4l2_buffer buf;
    struct v4l2_requestbuffers rb;
    void *mem[NB_BUFFER];
    unsigned char *tmpbuffer;
    unsigned char *framebuffer;
    int isstreaming;
    int grabmethod;
    int width;
    int height;
    int fps;
    int formatIn;
    int formatOut;
    int framesizeIn;
    int signalquit;
    int toggleAvi;
    int getPict;
    int rawFrameCapture;
    /* raw frame capture */
    unsigned int fileCounter;
    /* raw frame stream capture */
    unsigned int rfsFramesWritten;
    unsigned int rfsBytesWritten;
    /* raw stream capture */
    FILE *captureFile;
    unsigned int framesWritten;
    unsigned int bytesWritten;
    avi_t *avifile;
    char *avifilename;
    int framecount;
    int recordstart;
    int recordtime;
};

/* context of each camera thread */
typedef struct {
    int id;
    globals *pglobal;
    pthread_t threadID;
    pthread_mutex_t controls_mutex;
    struct vdIn *videoIn;
} context;

context cams[MAX_INPUT_PLUGINS];

/*
int
init_videoIn(struct vdIn *vd, char *device, int width, int height, int fps,
	     int format, int grabmethod, char *avifilename);
*///Ignat
int
init_videoIn(struct vdIn *vd, char *device, int width, int height, int fps,
             int format, int grabmethod, globals *pglobal, int id);
int enum_controls(int vd);
int save_controls(int vd);
int load_controls(int vd);

int memcpy_picture(unsigned char *out, unsigned char *buf, int size);
int uvcGrab(struct vdIn *vd);
int close_v4l2(struct vdIn *vd);

int v4l2GetControl(struct vdIn *vd, int control);
//Ignat
int v4l2SetControl(struct vdIn *vd, int control, int value, int plugin_number, globals *pglobal);
int v4l2UpControl(struct vdIn *vd, int control);
int v4l2DownControl(struct vdIn *vd, int control);
int v4l2ToggleControl(struct vdIn *vd, int control);
int v4l2ResetControl(struct vdIn *vd, int control);
int v4l2ResetPanTilt(struct vdIn *vd);
int v4L2UpDownPan(struct vdIn *vd, short inc);
int v4L2UpDownTilt(struct vdIn *vd,short inc);
int v4L2UpDownPanTilt(struct vdIn *vd, short inc_p, short inc_t);
int v4l2SetLightFrequencyFilter(struct vdIn *vd,int flt);
int enum_frame_intervals(int dev, __u32 pixfmt, __u32 width, __u32 height);
int enum_frame_sizes(int dev, __u32 pixfmt);
int enum_frame_formats(int dev, unsigned int *supported_formats, unsigned int max_formats);

