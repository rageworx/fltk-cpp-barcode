#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include <FL/fl_utf8.h>
#include <FL/Fl_SVG_Image.H>
#include <FLFTRender.h>
#include <fl_imgtk.h>

#include "QRCode.h"
#include "mmath.h"
#include "qrcodegen.hpp"

#define  SVG_TMPBUFF_LEN        (1024*512)

using namespace std;
using std::uint8_t;
using qrcodegen::QrCode;
using qrcodegen::QrSegment;

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
    Fl_SVG_Image* tmpimg = getImage();
    if ( tmpimg != nullptr )
    {
        unsigned min_l = __MIN( width, height );
        
        Fl_RGB_Image* retimg = (Fl_RGB_Image*)tmpimg->copy( min_l, min_l );
        
        delete tmpimg;
        
        return retimg;
    }
    return nullptr;
}

Fl_SVG_Image* QRCode::getImage()
{
    if ( qrc_inst != nullptr )
    {
        QrCode& qr = *(QrCode*)qrc_inst;

#ifdef DEBUG
        printf( "Fl_SVG_Image* QRCode::getImage(), qr = %p\n", &qr );
        fflush( stdout );
#endif /// of DEBUG

        if ( encode( nullptr ) == nullptr )
            return nullptr;
        
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
                  "\t<rect width=\"100%\" height=\"100%\" fill=\"#FFFFFF\"/>\n" \
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

        Fl_SVG_Image* retImg = new Fl_SVG_Image( "fltk_cpp_barcode_svg_image", 
                                                 (const char*)renderbuff );
        
        return retImg;
    }
    
    return nullptr;
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