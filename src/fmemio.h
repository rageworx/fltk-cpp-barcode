#ifndef __FMEMIO_H__
#define __FMEMIO_H__

#ifndef __linux__
FILE* fmemopen(void *buf, size_t size, const char *mode);
#endif /// of __linux__

#endif /// of __FMEMIO_H__