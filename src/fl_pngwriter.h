#ifndef __FL_PNGWRITER_H__
#define __FL_PNGWRITER_H__

#include <FL/Fl.H>
#include <FL/Fl_Image.H>

typedef void (*pngwrite_progress_cb)(void* inst, unsigned current, unsigned maxsz);

void fl_pngwriter_setcallback_inst( void* p );
void fl_pngwriter_setcallback( pngwrite_progress_cb cb );
void fl_pngwriter_unsetcallback();

bool fl_image_write_to_pngfile( Fl_Image* refimg, const char* refpath, int q );

#endif /// of __FL_PNGWRITER_H__
