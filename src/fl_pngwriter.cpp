#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <string>

#if defined(__APPLE__)
    #include <sys/uio.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <sys/mman.h>
#elif defined(__linux__)
    //#include <sys/io.h>
#else
    #include <io.h>
#endif /// of __APPLE__

#include <vector>
#include <png.h>
#include "fl_pngwriter.h"
#include "fmemio.h"

using namespace std;

static void* pngwrite_pcb_inst = NULL;
static pngwrite_progress_cb pngwrite_p_cb = NULL;

void fl_pngwriter_setcallback_inst( void* p )
{
    pngwrite_pcb_inst = p;
}

void fl_pngwriter_setcallback( pngwrite_progress_cb cb )
{
    pngwrite_p_cb = cb;
}

void fl_pngwriter_unsetcallback()
{
    pngwrite_pcb_inst = NULL;
    pngwrite_p_cb = NULL;
}

bool fl_image_write_to_pngfile( Fl_Image* refimg, const char* refpath, int q  )
{
    if ( ( refimg != NULL ) && ( refpath != NULL ) )
    {
        if ( refimg->d() < 3 )
            return false;

#if defined(_WIN32) && defined(SUPPORT_WCHAR)
        // Solving asian charactor problems ...
        //
        size_t   decodelen = strlen( refpath );
        wchar_t* decodedfn = new wchar_t[ decodelen + 1 ];

        fl_utf8towc( refpath, decodelen,
                     decodedfn, decodelen + 1 );

        if ( _waccess( decodedfn, F_OK ) == 0 )
        {
            if ( _wunlink( decodedfn ) != 0 )
                return false; /// failed to remove file !
        }

        FILE* fp = _wfopen( decodedfn, L"wb" );

        delete[] decodedfn;
#else
        if ( access( refpath, F_OK ) == 0 )
        {
            if ( unlink( refpath ) != 0 )
                return false; /// failed to remove file !
        }

        FILE* fp = fopen( refpath, "wb" );
#endif /// of _WIN32
        if ( fp != NULL )
        {
            png_structp png_ptr     = NULL;
            png_infop   info_ptr    = NULL;
            png_bytep   row         = NULL;

            png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING,
                                               NULL,
                                               NULL,
                                               NULL );
            if ( png_ptr != NULL )
            {
                info_ptr = png_create_info_struct( png_ptr );
                if ( info_ptr != NULL )
                {
                    if ( setjmp( png_jmpbuf( (png_ptr) ) ) == 0 )
                    {
                        int png_c_type = PNG_COLOR_TYPE_RGB;

                        if ( refimg->d() == 4 )
                        {
                            png_c_type = PNG_COLOR_TYPE_RGBA;
                        }

                        png_init_io( png_ptr, fp );
                        // Let compress image with libz
                        //  changing compress levle 8 to 7 for speed.
                        png_set_compression_level( png_ptr, q );
                        png_set_IHDR( png_ptr,
                                      info_ptr,
                                      refimg->w(),
                                      refimg->h(),
                                      8,
                                      png_c_type,
                                      PNG_INTERLACE_NONE,
                                      PNG_COMPRESSION_TYPE_BASE,
                                      PNG_FILTER_TYPE_BASE);

                        png_write_info( png_ptr, info_ptr );

                        const uchar* buff = (const uchar*)refimg->data()[0];

                        unsigned rowsz = refimg->w() * sizeof( png_byte ) * refimg->d();

                        row = (png_bytep)malloc( rowsz );
                        if ( row != NULL )
                        {
                            int bque = 0;
                            int minur = refimg->h() / 50;

                            for( int y=0; y<refimg->h(); y++ )
                            {
                                memcpy( row, &buff[bque], rowsz );
                                bque += rowsz;

                                png_write_row( png_ptr, row );

                                // Display progress..
                                if ( pngwrite_p_cb != NULL )
                                {
                                    pngwrite_p_cb( pngwrite_pcb_inst, y , refimg->h() );
                                }
                            }

                            png_write_end( png_ptr, NULL );

                            fclose( fp );
                            free(row);
                        }

                        png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
                        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);

                        if ( pngwrite_p_cb != NULL )
                        {
                            // Send finalize (maximum size) callback proc.
                            pngwrite_p_cb( pngwrite_pcb_inst, refimg->h(), refimg->h() );
                        }

                        return true;
                    }
                    png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
                }
                png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
            }
        }
    }

   return false;
}

