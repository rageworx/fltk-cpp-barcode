#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <FL/platform.H>
#include <FL/Fl.H>
#include <FL/Fl_RGB_Image.H>

#include "BarCodeBase.h"

BarCodeBase::BarCodeBase( std::string& dt )
 : colBg( 0xFFFFFF00 ), colMaskFg( 0x7F7F7FFF ), colFg( 0x000000FF )
{ 
    data = dt; 
    recalcColor();
}

BarCodeBase::~BarCodeBase() 
{
    if ( data.size() > 0 )
    {
        data.clear();
    }
}
        
void BarCodeBase::Data( std::string& dt ) 
{ 
    data = dt; 
}

std::string BarCodeBase::Data() 
{ 
    return data; 
}

void BarCodeBase::FontFace( std::string& ffname )
{
    // check file existed,
    if ( access( ffname.c_str(), 0 ) == 0 )
    {
        ttfname = ffname;
    }
}

std::string BarCodeBase::FontFace()
{
    return ttfname;
}

void BarCodeBase::ForegroundColor( uint32_t c )
{
    colFg = c;
    recalcColor();
}

uint32_t BarCodeBase::ForegroundColor()
{
    return colFg;
}

void BarCodeBase::BackgroundColor( uint32_t c )
{
    colBg = c;
    recalcColor();
}

uint32_t BarCodeBase::BackgroundColor()
{
    return colBg;
}

void BarCodeBase::printByteArr( const char* msg, const uint8_t* buff, size_t bufflen )
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

#ifdef DEBUG
        if ( msg != nullptr )
        {
            fprintf( stdout, "char: %s, barcode weight: %s\n", msg, sb.c_str() );
        }
        else
        {
            fprintf( stdout, "barcode weight: %s\n", sb.c_str() );
        }

        fflush( stdout );
#endif /// of DEBUG
    }    
}

bool BarCodeBase::checkNumberics(std::string& data)
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

void BarCodeBase::recalcColor()
{
    uint32_t colBA = ( colBg & 0x00000001 );

    if ( colBA == 0 )
        colBA = 1;
    
    colMaskFg = colBA;
}