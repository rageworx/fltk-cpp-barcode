#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include <FL/Fl.H>
#include <FL/fl_utf8.h>
#include <FL/Fl_SVG_Image.H>
#include <FL/Fl_Image_Surface.H>
#include <FL/fl_draw.H>
#include <FLFTRender.h>
#include <fl_imgtk.h>
#ifdef USE_OMP
#include <omp.h>
#endif /// of USE_OMP

#include "QRcode.h"
#include "mmath.h"
#include "qrcodegen.hpp"

#define SVG_TMPBUFF_LEN        (1024*512)
#define Swap32( _X_ )          (_X_ & 0x000000FF) << 24 | (_X_ & 0x0000FF00) << 8 | \
                               (_X_ & 0x00FF0000) >> 8 | (_X_ & 0xFF000000) >> 24

using namespace std;
using std::uint8_t;
using qrcodegen::QrCode;
using qrcodegen::QrSegment;

#ifdef __APPLE__
size_t strcat_s(char *d, size_t n, char const *s )
{
    return snprintf(d, n, "%s%s", d, s);
}
#endif /// of __APPLE__

QRCode::QRCode( std::string& dt )
 : BarCodeBase( dt ), qrc_inst( nullptr ) , qrc_border( 2 )
{
    static QrCode Qr = QrCode::encodeText("fltk_cpp_qrcode", QrCode::Ecc::MEDIUM);
    
    qrc_inst = &Qr;

#ifdef DEBUG
    printf( "qrc_inst = %p\nQr = %p\n", qrc_inst, &Qr );
    fflush( stdout );
#endif /// of DEBUG
}

QRCode::~QRCode()
{
}

Fl_RGB_Image* QRCode::getImage( unsigned width, unsigned height )
{    
    if ( qrc_inst != nullptr )
    {
        QrCode& qr = *(QrCode*)qrc_inst;

        if ( encode( nullptr ) == nullptr )
            return 0;
        
        size_t border = qrc_border;
        
        if ( border > INT_MAX / 2 || border * 2 > INT_MAX - qr.getSize() )
        {
            border = ( INT_MAX - qr.getSize() )/ 2;
        }
        
        size_t qrsize = qr.getSize();

        unsigned cast_w = width / qrsize - ( border / 2 );
        unsigned cast_h = height / qrsize - ( border / 2 );

        if ( cast_w > cast_h )
        {
            cast_w = cast_h;
        }
        else
        if ( cast_h > cast_w )
        {
            cast_h = cast_w;
        }

        unsigned start_x = ( width - ( cast_w * qrsize ) - ( border / 2 ) ) / 2;
        unsigned start_y = ( height - ( cast_h * qrsize ) - ( border / 2 ) ) / 2;

        Fl_RGB_Image* retimg = fl_imgtk::makeanempty( width, height, 4, colBg );
        
        if ( retimg != nullptr )
        {
            for (size_t y=0; y<qrsize; y++) 
            {
                for (size_t x=0; x<qrsize; x++) 
                {
                    if ( qr.getModule(x, y) ) 
                    {
                        unsigned cast_x = cast_w * x + start_x;
                        unsigned cast_y = cast_h * y + start_y;
                                                
                        fl_imgtk::\
                        draw_fillrect( retimg,
                                       cast_x, cast_y,
                                       cast_w, cast_h,
                                       colFg );
                    }
                }
            }
            
            retimg->uncache();
        }
        
        return retimg;
    }

    return nullptr;
}

Fl_SVG_Image* QRCode::getImage()
{
    char* renderbuff = NULL;
    
    size_t bufflen = getSVG( &renderbuff );

    if ( ( renderbuff != nullptr ) && ( bufflen > 0 ) )
    {
        Fl_SVG_Image* retImg = new Fl_SVG_Image( "fltk_cpp_barcode_svg_image", 
                                                 (const char*)renderbuff );
        
        delete[] renderbuff;
        
        return retImg;
    }
    
    return nullptr;
}

size_t QRCode::getSVG( char** svgbuff )
{
    if ( ( qrc_inst != nullptr ) && ( svgbuff != nullptr ) )
    {
        QrCode& qr = *(QrCode*)qrc_inst;

#ifdef DEBUG
        printf( "Fl_SVG_Image* QRCode::getImage(), qr = %p\n", &qr );
        fflush( stdout );
#endif /// of DEBUG

        if ( encode( nullptr ) == nullptr )
            return 0;
        
        size_t border = qrc_border;
        
        if ( border > INT_MAX / 2 || border * 2 > INT_MAX - qr.getSize() )
        {
            border = ( INT_MAX - qr.getSize() )/ 2;
        }
	    
        char renderbuff[SVG_TMPBUFF_LEN] = {0};
        
        snprintf( renderbuff, SVG_TMPBUFF_LEN, 
                  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" \
                  "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n" \
                  "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" viewBox=\"0 0 " \
                  "%zu %zu\" stroke=\"none\">\n" \
                  "\t<rect width=\"100%%\" height=\"100%%\" fill=\"#FFFFFF\"/>\n" \
                  "\t<path d=\"",
                  qr.getSize() + border * 2,
                  qr.getSize() + border * 2 );

        for (size_t y=0; y<qr.getSize(); y++) 
        {
            for (size_t x=0; x<qr.getSize(); x++) 
            {
                if ( qr.getModule(x, y) ) 
                {
                    if (x != 0 || y != 0)
                        strcat_s( renderbuff, SVG_TMPBUFF_LEN, " " );

                    char tmpbuff[50] = {0};
                    snprintf( tmpbuff, 50,
                              "M %zu, %zuh1v1h-1z",
                              x + border,
                              y + border );
                    
                    strcat_s( renderbuff, SVG_TMPBUFF_LEN, tmpbuff );
                }
            }
        }
        
        strcat_s( renderbuff, SVG_TMPBUFF_LEN, "\" fill=\"#000000\"/>\n</svg>\n" );

#ifdef DEBUG_SVG_V
        printf( "renderbuff = \n%s", renderbuff );
        fflush( stdout );
#endif /// of DEBUG_SVG_V

        size_t renderbufflen = strlen( renderbuff ) + 1;
        
        *svgbuff = new char[ renderbufflen ];

        if ( *svgbuff != nullptr )
        {
            memset( *svgbuff, 0, renderbufflen );
            memcpy( *svgbuff, renderbuff, renderbufflen - 1 );
            
            return renderbufflen;
        }
    }
    
    return 0;    
}

uint8_t* QRCode::encode( size_t* retlen )
{
    if ( ( qrc_inst != nullptr ) && ( data.size() > 0 ) )
    {        
        QrCode& qr = *(QrCode*)qrc_inst;

#ifdef DEBUG
        printf( "QRCode::encode(), qr = %p\n", &qr );
        fflush( stdout );
#endif /// of DEBUG

        // default QRcode error correction level is LOW.
        QrCode::Ecc errCorLvl = QrCode::Ecc::LOW; 
        
        switch( qrc_ecc_lvl )
        {
            case 1:
                errCorLvl = QrCode::Ecc::MEDIUM;
                break;
                
            case 2:
                errCorLvl = QrCode::Ecc::HIGH;
                break;
                
            case 3:
                errCorLvl = QrCode::Ecc::QUARTILE;
                break;
        }
        
        // detect data type ...
        uint8_t dt_type = 0;
        if ( checkNumberics( data ) == true )
        {
            qr = QrCode::encodeText( data.c_str(), errCorLvl );
        }
        else
        {
            // check is safe for ASCII ?
            int utf8testr = fl_utf8test( data.c_str(), data.size() );
            /*
            fl_utf8test() returns -
              0 if there is any illegal UTF-8 sequences, using the same rules as fl_utf8decode(). Note that some UCS values considered illegal by RFC 3629, such as 0xffff, are considered legal by this.
              1 if there are only single-byte characters (ie no bytes have the high bit set). This is legal UTF-8, but also indicates plain ASCII. It also returns 1 if srclen is zero.
              2 if there are only characters less than 0x800.
              3 if there are only characters less than 0x10000.
              4 if there are characters in the 0x10000 to 0x10ffff range.
            */
            if ( utf8testr <= 3 ) /// meaning ASCII to simple UTF-8
            {
                qr = QrCode::encodeText( data.c_str(), errCorLvl );
            }
#ifdef SUPPROR_UTF8_KANJI /// fixme : this is still buggy, need to fixed.
            else
            if ( utf8testr == 4 ) /// 13bit UTF, KANJI ? 
            {
                qr = QrCode::encodeText( data.c_str(), errCorLvl );
                
                int wlen = fl_utf8strlen( data.c_str(), data.size() );
                if ( wlen > 0 )
                {                    
                    vector<uint32_t> ibuff;
                    
                    char* start = (char*)data.c_str();
                    char* end   = (char*)&start[data.size() - 1];
                    size_t q    = 0;
                    
                    // convert UTF8 to int codes ..
                    for( int cnt=0; cnt<wlen; cnt++ )
                    {
                        int l1 = 0;
                        int l2 = 0;
                        uint32_t u1 = fl_utf8decode((const char*)(start + cnt), end, &l1);
                        if (l1 < 1) 
                        {
                            cnt += 1;
                        } else {
                            cnt += l1;
                        }
                                                    
                        ibuff.push_back( u1 );
                    }
                    
                    qrcodegen::BitBuffer bbuff;
                    
                    for (uint32_t x : ibuff )
                        bbuff.appendBits(x, 13);
                    
                    qr = QrCode::encodeSegments(
                        { QrSegment( QrSegment::Mode::KANJI, static_cast<int>(ibuff.size() ), bbuff ) },
                        errCorLvl );

                    ibuff.clear();
                }
            }
#endif /// of SUPPROR_UTF8_KANJI
        }

        if ( retlen != nullptr )
        {
            *retlen = qr.getSize();
        }
                
        return (uint8_t*)data.c_str();
    }
    
    if ( retlen != nullptr )
    {
        *retlen = 0;
    }

    return nullptr;
}
