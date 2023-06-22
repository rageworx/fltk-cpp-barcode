#ifndef __QRCODE_H__
#define __QRCODE_H__

#include "BarCodeBase.h"

class QRCode : public BarCodeBase
{
    public:
        QRCode( std::string& dt );
        ~QRCode();
        
    public:
        Fl_RGB_Image* getImage( unsigned width, unsigned height );
        Fl_SVG_Image* getImage();
        size_t        getSVG( char** svgbuff );
        
    public:
        void        border( size_t bsz ) { qrc_border = bsz; if ( qrc_border < 2 ) qrc_border = 2; }
        size_t      border() { return qrc_border; }
        void        ECClevel( uint8_t lvl ) { qrc_ecc_lvl = lvl; if ( lvl > 3 ) lvl = 3; }
        uint8_t     ECClevel() { return qrc_ecc_lvl; }
        
    protected:
        uint8_t*    encode( size_t* retlen );
        
    private:
        void*       qrc_inst;
        uint8_t     qrc_ecc_lvl;
        size_t      qrc_border;
};

#endif /// of __QRCODE_H__