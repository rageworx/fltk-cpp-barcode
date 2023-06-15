#ifndef __EAN13_H__
#define __EAN13_H__

class EAN13 
{
    public:
        EAN13( std::string& dt );
        ~EAN13();
        
    public:
        void setData( std::string& dt );
        std::string getData();
        Fl_RGB_Image* getImage( unsigned width, unsigned height );        

    private:
        uint8_t* initBuffer( size_t* retlen = nullptr );
        size_t appendData( const uint8_t* src, size_t srclen, uint8_t* dst, size_t pos, std::string& debugdata );
        void printByteArr( std::string& msg, const uint8_t* buff, size_t bufflen );
        bool checkNumber( std::string& data );
        bool isVaildBarcodeData();
        uint8_t* encode();

    private:
        std::string data;
        size_t      bufferlen;
        
};

#endif /// of __EAN13_H__