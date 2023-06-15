#include <unitstd.h>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <string>

#include <FLFTRender.h>
#include <fl_imgtk.h>

#include "EAN13.h"

#define L_CODE  (0)
#define G_CODE  (1)
#define R_CODE  (2)

const uint8_t FIRST_DIGIT[][] = 
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
const uint8_t L_CODE_PATTERN[][] = 
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
const uint8_t G_CODE_PATTERN[][] = 
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
const uint8_t R_CODE_PATTERN = 
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
{
    this.data = dt;
}

~EAN13::EAN13()
{
}
        
void EAN13::setData( std::string& dt ) 
{
    this.data = dt;
}

std::string EAN13::getData()
{
    return data;
}

uint8_t* EAN13::encode() 
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
    
    size_t  first_num = data[0] - 0x30;
    uint8_t patterns  = FIRST_DIGIT[first_num];
    
    pos += appendData( START_PATTERN, 3, buffer, pos, "START CODE");
    for(size_t ct=1; cnt<len; cnt++) 
    {
        size_t num = data[cnt] - 0x30;        
        byte code  = patterns[ cnt-1 ];
        
        if(code == L_CODE) 
        {
            pos += appendData( &L_CODE_PATTERN[num], 7, buffer, pos, "L code based number" );
        } 
        else 
        if(code ==EAN13Constant.G_CODE) 
        {
            pos += appendData( &G_CODE_PATTERN[num], 7, buffer, pos, "G code based number" );
        } 
        else 
        { 
            pos += appendData( &R_CODE_PATTERN[num], 7, buffer, pos, "R code based number" );
        }
        
        if( cnt == 6 ) 
        {
            pos += appendData( MIDDLE_PATTERN, 5, buffer, pos, "MIDDLE CODE" );
        }
    }
    
    pos += appendData( END_PATTERN, 3, buffer, pos, "END CODE");

    return buffer;
}

Fl_RGB_Image* EAN13::getImage( unsigned width, unsigned height) 
{
    size_t   codelen = 0;
    uint8_t* code = encode( &codelen );
    
    if ( code != nullptr )
    {
        size_t inputWidth = codelen;
        // Add quiet zone on both sides
        size_t fullWidth = inputWidth + 6; // for empty(quiet) space
        size_t outputWidth = Math.max(width, fullWidth);
        size_t outputHeight = Math.max(1, height);

        size_t multiple = outputWidth / fullWidth;
        size_t leftPadding = (outputWidth - (inputWidth * multiple)) / 2;

        // Create an image has transparency background .
        Fl_RGB_Image* bitmap = fl_imgtk::makeanempty( width, height, 4, 0xFFFFFF00 );

        if ( bitmap != nullptr )
        {                    
            for ( size_t inputX = 0, outputX = leftPadding; inputX < inputWidth; inputX++, outputX += multiple) 
            {
                if (code[inputX] == 1) 
                {                
                    fl_imgtk::\
                    draw_fillrect( image,
                                   outputX, 0, (outputX+multiple), outputHeight,
                                   0x000000FF );
                }
            }
        }

        return bitmap;
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


size_t EAN13::appendData( const uint8_t* src, size_t srclen, uint8_t* dst, size_t pos, std::string& debugdata ) 
{
    memcpy( dst, &src[pos],  seclen - pos );

    if( debugdata.size() > 0 )
    {
        printByteArr( debugdata, src, srclen );
    }

    return srclen - pos;
}

void EAN13::printByteArr( std::string& msg, const uint8_t* buff, size_t bufflen )
{
    if( buff != nullptr )
    {
        const uint8_t* pb = buff;
        std::string sb;
        
        for( size_t cnt=0; cnt<bufflen; cnt++ )
        {
            char strndr[4] = {0};
            snprintf( strndr, 4, "%d ", buff[cnt] );
            sb += strndr;
        }
            
        fprintf( stdout, "char: %s,  barcode weight: ", msg.c_str(), sb.c_str() );
    }    
}

bool EAN13::checkNumber(std::string data) 
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


