#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <string>

#include <FLFTRender.h>
#include <fl_imgtk.h>

#include "EAN13.h"
#include "mmath.h"

#define L_CODE  (0)
#define G_CODE  (1)
#define R_CODE  (2)

typedef uint8_t digitItem[12];
typedef uint8_t patternItem[7];

const digitItem FIRST_DIGIT[] = 
{
    { L_CODE,L_CODE,L_CODE,L_CODE,L_CODE,L_CODE,R_CODE,R_CODE,R_CODE,R_CODE,R_CODE,R_CODE },
    { L_CODE,L_CODE,G_CODE,L_CODE,G_CODE,G_CODE,R_CODE,R_CODE,R_CODE,R_CODE,R_CODE,R_CODE },
    { L_CODE,L_CODE,G_CODE,G_CODE,L_CODE,G_CODE,R_CODE,R_CODE,R_CODE,R_CODE,R_CODE,R_CODE },
    { L_CODE,L_CODE,G_CODE,G_CODE,G_CODE,L_CODE,R_CODE,R_CODE,R_CODE,R_CODE,R_CODE,R_CODE },
    { L_CODE,G_CODE,L_CODE,L_CODE,G_CODE,G_CODE,R_CODE,R_CODE,R_CODE,R_CODE,R_CODE,R_CODE },
    { L_CODE,G_CODE,G_CODE,L_CODE,L_CODE,G_CODE,R_CODE,R_CODE,R_CODE,R_CODE,R_CODE,R_CODE },
    { L_CODE,G_CODE,G_CODE,G_CODE,L_CODE,L_CODE,R_CODE,R_CODE,R_CODE,R_CODE,R_CODE,R_CODE },
    { L_CODE,G_CODE,L_CODE,G_CODE,L_CODE,G_CODE,R_CODE,R_CODE,R_CODE,R_CODE,R_CODE,R_CODE },
    { L_CODE,G_CODE,L_CODE,G_CODE,G_CODE,L_CODE,R_CODE,R_CODE,R_CODE,R_CODE,R_CODE,R_CODE },
    { L_CODE,G_CODE,G_CODE,L_CODE,G_CODE,L_CODE,R_CODE,R_CODE,R_CODE,R_CODE,R_CODE,R_CODE },
};

const uint8_t START_PATTERN[] = { 1, 0, 1 };
const uint8_t MIDDLE_PATTERN[] = { 0, 1, 0, 1, 0 };
const uint8_t END_PATTERN[] =  { 1, 0, 1 };

// l-code
const patternItem L_CODE_PATTERN[] = 
{
    { 0,0,0,1,1,0,1 },
    { 0,0,1,1,0,0,1 },
    { 0,0,1,0,0,1,1 },
    { 0,1,1,1,1,0,1 },
    { 0,1,0,0,0,1,1 },
    { 0,1,1,0,0,0,1 },
    { 0,1,0,1,1,1,1 },
    { 0,1,1,1,0,1,1 },
    { 0,1,1,0,1,1,1 },
    { 0,0,0,1,0,1,1 },
};

// g-code
const patternItem G_CODE_PATTERN[] = 
{
    { 0,1,0,0,1,1,1 },
    { 0,1,1,0,0,1,1 },
    { 0,0,1,1,0,1,1 },
    { 0,1,0,0,0,0,1 },
    { 0,0,1,1,1,0,1 },
    { 0,1,1,1,0,0,1 },
    { 0,0,0,0,1,0,1 },
    { 0,0,1,0,0,0,1 },
    { 0,0,0,1,0,0,1 },
    { 0,0,1,0,1,1,1 },
};

// r-code
const patternItem R_CODE_PATTERN[] = 
{
    { 1,1,1,0,0,1,0 },
    { 1,1,0,0,1,1,0 },
    { 1,1,0,1,1,0,0 },
    { 1,0,0,0,0,1,0 },
    { 1,0,1,1,1,0,0 },
    { 1,0,0,1,1,1,0 },
    { 1,0,1,0,0,0,0 },
    { 1,0,0,0,1,0,0 },
    { 1,0,0,1,0,0,0 },
    { 1,1,1,0,1,0,0 },
};


EAN13::EAN13( std::string& dt )
 : BarCodeBase( dt )
{
}

EAN13::~EAN13()
{
}

uint8_t* EAN13::encode( size_t* retlen )
{
    if( isVaildBarcodeData() == false ) 
    {
        fprintf( stderr, "invalid data length!" );
        return nullptr;
    }
    
    size_t len = data.size();
    size_t pos = 0;

    uint8_t* buffer = initBuffer( &bufferlen );
    
    if ( buffer == nullptr )
        return nullptr;
    
    size_t   first_num = data[0] - 0x30;
    uint8_t* patterns  = (uint8_t*)&FIRST_DIGIT[first_num];
    
    pos += appendData( START_PATTERN, 3, buffer, pos, "START CODE" );
    for(size_t cnt=1; cnt<len; cnt++) 
    {
        size_t  num  = data[cnt] - 0x30; /// 0x30 == '0'.
        uint8_t code = patterns[ cnt-1 ];
        
        switch( code )
        {
            case L_CODE:
                pos += appendData( &L_CODE_PATTERN[num], 7, buffer, pos, "L code based number" );
                break;
                
            case G_CODE:
                pos += appendData( &G_CODE_PATTERN[num], 7, buffer, pos, "G code based number" );
                break;
                
            default:
                pos += appendData( &R_CODE_PATTERN[num], 7, buffer, pos, "R code based number" );
        }
        
        if( cnt == 6 ) 
        {
            pos += appendData( MIDDLE_PATTERN, 5, buffer, pos, "MIDDLE CODE" );
        }
    }
    
    pos += appendData( END_PATTERN, 3, buffer, pos, "END CODE");
    
    if ( retlen != nullptr )
    {
        *retlen = pos + 3;
    }

    return buffer;
}

Fl_RGB_Image* EAN13::getImage( unsigned width, unsigned height) 
{
    size_t   codelen = 0;
    uint8_t* code = encode( &codelen );
    
    if ( code != nullptr )
    {        
        // Add quiet zone on both sides
        size_t fullWidth = codelen + 6; // for empty(quiet) space
        size_t outputWidth = __MAX(width, fullWidth);
        size_t outputHeight = __MAX(1, height);
        size_t multiple = outputWidth / fullWidth;
        size_t leftPadding = (outputWidth - (codelen * multiple)) / 2;
        size_t fntHeight = leftPadding;

        // Create an image has transparency background .
        Fl_RGB_Image* image = fl_imgtk::makeanempty( width, height, 4, colBg );

        if ( image != nullptr )
        {
            FLFTRender* ftr = new FLFTRender( ttfname.c_str(), 0 );            
            
            if ( ftr != nullptr )
            {
                if ( ftr->FontLoaded() == true )
                {                    
                    ftr->FontSize( fntHeight );
                }
                else
                {
                    delete ftr;
                    ftr = nullptr;
                }
            }
#ifdef DEBUG
            else
            {
                fprintf( stderr, "Font load failure = %s\n", ttfname.c_str() );
            }                
#endif /// of DEBUG

            size_t midIdxMin = ( codelen / 2 ) - 3;
            size_t midIdxMax = ( codelen / 2 );

            for ( size_t inputX=0, outputX=leftPadding; \
                  inputX<codelen; \
                  inputX++, outputX+=multiple ) 
            {
                if (code[inputX] == 1) 
                {
                    size_t drawHeight = outputHeight;
                    
                    // check start code, middle, endcode index.
                    if ( ( inputX > 2 ) && ( inputX < ( codelen - 6 ) ) )
                    {
                        if ( ( inputX < midIdxMin ) || ( inputX > midIdxMax ) )
                        {
                            drawHeight = outputHeight - fntHeight;                            
                        }
                    }

                    fl_imgtk::\
                    draw_fillrect( image,
                                   outputX, 0, multiple, drawHeight,
                                   colFg );
#ifdef DEBUG_DRAW_LINE
                    fprintf( stdout,
                             "draw_fillrect( %p, %u, %u, %u, %u, .. );\n",
                             image, outputX, 0, multiple, drawHeight );
                    fflush( stdout );
#endif /// of DEBUG_DRAW_LINE
                }
            }
            
            if ( ftr != nullptr )
            {
                unsigned f_w = fntHeight * 0.75f;
                unsigned p_x = leftPadding;
                unsigned p_y = height - fntHeight;
                
                for( size_t cnt=0; cnt<data.size(); cnt++ )
                {
                    if ( cnt == 0 )
                    {
                        p_x = leftPadding - ( f_w * 1.1f );
                    }
                    else
                    if ( cnt > 0 )
                    {
                        if ( cnt < ( data.size() / 2 + 1 ) )
                        {
                            p_x = leftPadding + ( f_w * cnt - 1 ) - multiple;
                        }
                        else
                        {
                            p_x = leftPadding + ( multiple * 6 ) + ( f_w * cnt - 1 );
                        }
                    }
                        
                    FLFTRender::Rect mbox = {0, 0, 0, 0};
                    const char* pref = data.c_str();
                    char tmpbuff[2] = {0,0};
                    tmpbuff[0] = pref[cnt];

                    ftr->MeasureText( tmpbuff, mbox );
                    
                    if ( p_y + mbox.h > height )
                    {
                        mbox.h = height - p_y;
                    }

#ifdef DRAW_FONT_BACK_RECTANGLE
                    uint32_t maskedCol = colBg & 0x000000FF;
                    fl_imgtk::\
                    draw_fillrect( image,
                                   p_x, p_y , mbox.w, mbox.h,
                                   maskedCol );
#endif /// of DRAW_FONT_BACK_RECTANGLE
                    
                    // draw font in corrected position.
                    ftr->FontColor( colMaskFg ); /// draw white font to remove transparency.
                    ftr->RenderText( image, p_x, p_y - ( fntHeight * 0.1f ), tmpbuff );
                    ftr->FontColor( colFg );
                    ftr->RenderText( image, p_x, p_y - ( fntHeight * 0.1f ), tmpbuff );
                }
                
                delete ftr;
            }
        }

        return image;
    }
    
    return nullptr;
}

bool EAN13::isVaildBarcodeData() 
{
    if( data.size() == 0 )
    {
        return false;
    }
    
    if( data.size() != 13 ) 
    {
        return false;
    }
    
    if( checkNumber(data) == false ) 
    {
        return false;
    }
    
    return true;
}

uint8_t* EAN13::initBuffer( size_t* retlen ) 
{
    //1. add start code 4byte
    //2. add middle code 5byte
    //3. add end code 4 byte
    //4. add encoded data 7byte * 12
    size_t sum = 3 + 5 + 3 + (7*12);

    if ( retlen != nullptr )
        *retlen = sum;
    
    return new uint8_t[sum];
}


size_t EAN13::appendData( const void* src, size_t srclen, uint8_t* dst, size_t pos, const char* debugdata ) 
{
    if ( ( src != nullptr ) && ( dst != nullptr ) )
    {
        uint8_t* ps = (uint8_t*)src;
        
        if ( srclen < 12 )
        {
            memcpy( &dst[pos], ps, srclen );
            
            if( debugdata != nullptr )
            {
                printByteArr( debugdata, ps, srclen );
            }

            return srclen;
        }
    }
    
    return 0;
}


bool EAN13::checkNumber(std::string& data) 
{
    const char* pref = data.c_str();

    for(size_t cnt=0; cnt<data.length(); cnt++ ) 
    {
        if ( pref[cnt] < '0' || pref[cnt] > '9' )
        {
            return false;
        }
    }

    return true;
}


