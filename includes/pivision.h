#ifndef __PIVISION_H
#define __PIVISION_H

#ifdef __cplusplus
extern "C" {

#endif

void * sendvideo(void *address);

void * detect(void * dummy);

void disable_camera_thread();

#ifdef __cplusplus
}

#endif

#endif
