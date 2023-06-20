#ifndef __CODE128_H__
#define __CODE128_H__

#include "BarCodeBase.h"

class Code128 : public BarCodeBase
{
    public:
        Code128( std::string& dt );
        ~Code128();
        
    public:
        void   Spaced( bool f ) { spaced = f; }
        bool   Spaced()         { return spaced; }
        
    public:
        Fl_RGB_Image* getImage( unsigned width, unsigned height );

    protected:
        void        printMetaInfo();
        size_t      appendData(const void* weights, uint8_t* dst, int pos, std::string debugdata = nullptr);

    protected:
        void        init();
        uint8_t*    initBuffer(size_t dataLen, size_t* retlen = nullptr);
        std::string insertSpace(std::string& data);
        uint8_t*    encode( size_t* retlen );

    private:
        bool        spaced;
        size_t      weight;
        size_t      weight_sum;
        size_t      check_sum;
};

#endif /// of  __CODE128_H__