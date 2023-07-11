#ifndef __BARCODE_H__
#define __BARCODE_H__

class BarCodeBase
{
    public:
        BarCodeBase( std::string& dt );
        ~BarCodeBase();
        
    public:
        void            Data( std::string& dt );
        std::string     Data();
        void            FontFace( std::string& ffname );
        std::string     FontFace();
        void            ForegroundColor( uint32_t c );
        uint32_t        ForegroundColor();
        void            BackgroundColor( uint32_t c );
        uint32_t        BackgroundColor();
        
    public:
        virtual \
        Fl_RGB_Image*   getImage( unsigned width, unsigned height ) = 0;

    protected:
        void            printByteArr( const char* , const uint8_t* , size_t );
        
    protected:
        bool            checkNumberics(std::string& data);
        
    protected:
        virtual \
        uint8_t*        encode( size_t* retlen ) = 0;

    protected:
        std::string     ttfname;
        std::string     data;
        size_t          bufferlen;
        uint32_t        colBg;
        uint32_t        colFg;
        uint32_t        colMaskFg;
};

#endif /// of __BARCODE_H__