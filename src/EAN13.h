#ifndef __EAN13_H__
#define __EAN13_H__

#include "BarCodeBase.h"

class EAN13 : public BarCodeBase
{
    public:
        EAN13( std::string& dt );
        ~EAN13();
        
    public:
        Fl_RGB_Image* getImage( unsigned width, unsigned height );        

    private:
        uint8_t*    initBuffer( size_t* retlen = nullptr );
        size_t      appendData( const void* src, size_t srclen, uint8_t* dst, size_t pos, const char* debugdata = nullptr );
        bool        checkNumber( std::string& data );
        bool        isVaildBarcodeData();
        
    protected:
        uint8_t*    encode( size_t* retlen );      
};

#endif /// of __EAN13_H__