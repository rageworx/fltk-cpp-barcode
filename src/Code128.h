#ifndef __CODE128_H__
#define __CODE128_H__

class Code128 
{
    public:
        Code128( std::string& dt );
        ~Code128();

    public:
        void setData( std::string& dt );
        std::string getData();
        Fl_RGB_Image* getImage( uint32_t width, uint32_t height);
        void printByteArr( std::string& msg, const uint8_t* buff, size_t blen );
        void printMetaInfo();

    private:
        void init();
        uint8_t* initBuffer(size_t dataLen, size_t* retlen);
        std::string insertSpace(std::string& data);
        size_t appendData(const void* weights, uint8_t* dst, int pos, std::string debugdata);
        bool checkNumber(std::string data);
        uint8_t* encode( size_t* retlen = nullptr );

    private:
        std::string data;
        size_t      weight;
        size_t      weight_sum;
        size_t      check_sum;
};

#endif /// of  __CODE128_H__