#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <string>

#include <FLFTRender.h>
#include <fl_imgtk.h>

#include "Code128.h"
#include "mmath.h"

#define CODE_START_B 	( 104 )
#define CODE_STOP 		( 106 )
#define DIVISOR         ( 103 )
#define TOP_GAP         ( 30 )
#define BOTTOM_GAP      ( 60 )

typedef uint8_t cwItem[8];

const cwItem CODE_WEIGHT[] = 
{
    {2, 1, 2, 2, 2, 2, 0}, // 0
    {2, 2, 2, 1, 2, 2, 0},
    {2, 2, 2, 2, 2, 1, 0},
    {1, 2, 1, 2, 2, 3, 0},
    {1, 2, 1, 3, 2, 2, 0},
    {1, 3, 1, 2, 2, 2, 0}, // 5
    {1, 2, 2, 2, 1, 3, 0},
    {1, 2, 2, 3, 1, 2, 0},
    {1, 3, 2, 2, 1, 2, 0},
    {2, 2, 1, 2, 1, 3, 0},
    {2, 2, 1, 3, 1, 2, 0}, // 10
    {2, 3, 1, 2, 1, 2, 0},
    {1, 1, 2, 2, 3, 2, 0},
    {1, 2, 2, 1, 3, 2, 0},
    {1, 2, 2, 2, 3, 1, 0},
    {1, 1, 3, 2, 2, 2, 0}, // 15
    {1, 2, 3, 1, 2, 2, 0},
    {1, 2, 3, 2, 2, 1, 0},
    {2, 2, 3, 2, 1, 1, 0},
    {2, 2, 1, 1, 3, 2, 0},
    {2, 2, 1, 2, 3, 1, 0}, // 20
    {2, 1, 3, 2, 1, 2, 0},
    {2, 2, 3, 1, 1, 2, 0},
    {3, 1, 2, 1, 3, 1, 0},
    {3, 1, 1, 2, 2, 2, 0},
    {3, 2, 1, 1, 2, 2, 0}, // 25
    {3, 2, 1, 2, 2, 1, 0},
    {3, 1, 2, 2, 1, 2, 0},
    {3, 2, 2, 1, 1, 2, 0},
    {3, 2, 2, 2, 1, 1, 0},
    {2, 1, 2, 1, 2, 3, 0}, // 30
    {2, 1, 2, 3, 2, 1, 0},
    {2, 3, 2, 1, 2, 1, 0},
    {1, 1, 1, 3, 2, 3, 0},
    {1, 3, 1, 1, 2, 3, 0},
    {1, 3, 1, 3, 2, 1, 0}, // 35
    {1, 1, 2, 3, 1, 3, 0},
    {1, 3, 2, 1, 1, 3, 0},
    {1, 3, 2, 3, 1, 1, 0},
    {2, 1, 1, 3, 1, 3, 0},
    {2, 3, 1, 1, 1, 3, 0}, // 40
    {2, 3, 1, 3, 1, 1, 0},
    {1, 1, 2, 1, 3, 3, 0},
    {1, 1, 2, 3, 3, 1, 0},
    {1, 3, 2, 1, 3, 1, 0},
    {1, 1, 3, 1, 2, 3, 0}, // 45
    {1, 1, 3, 3, 2, 1, 0},
    {1, 3, 3, 1, 2, 1, 0},
    {3, 1, 3, 1, 2, 1, 0},
    {2, 1, 1, 3, 3, 1, 0},
    {2, 3, 1, 1, 3, 1, 0}, // 50
    {2, 1, 3, 1, 1, 3, 0},
    {2, 1, 3, 3, 1, 1, 0},
    {2, 1, 3, 1, 3, 1, 0},
    {3, 1, 1, 1, 2, 3, 0},
    {3, 1, 1, 3, 2, 1, 0}, // 55
    {3, 3, 1, 1, 2, 1, 0},
    {3, 1, 2, 1, 1, 3, 0},
    {3, 1, 2, 3, 1, 1, 0},
    {3, 3, 2, 1, 1, 1, 0},
    {3, 1, 4, 1, 1, 1, 0}, // 60
    {2, 2, 1, 4, 1, 1, 0},
    {4, 3, 1, 1, 1, 1, 0},
    {1, 1, 1, 2, 2, 4, 0},
    {1, 1, 1, 4, 2, 2, 0},
    {1, 2, 1, 1, 2, 4, 0}, // 65
    {1, 2, 1, 4, 2, 1, 0},
    {1, 4, 1, 1, 2, 2, 0},
    {1, 4, 1, 2, 2, 1, 0},
    {1, 1, 2, 2, 1, 4, 0},
    {1, 1, 2, 4, 1, 2, 0}, // 70
    {1, 2, 2, 1, 1, 4, 0},
    {1, 2, 2, 4, 1, 1, 0},
    {1, 4, 2, 1, 1, 2, 0},
    {1, 4, 2, 2, 1, 1, 0},
    {2, 4, 1, 2, 1, 1, 0}, // 75
    {2, 2, 1, 1, 1, 4, 0},
    {4, 1, 3, 1, 1, 1, 0},
    {2, 4, 1, 1, 1, 2, 0},
    {1, 3, 4, 1, 1, 1, 0},
    {1, 1, 1, 2, 4, 2, 0}, // 80
    {1, 2, 1, 1, 4, 2, 0},
    {1, 2, 1, 2, 4, 1, 0},
    {1, 1, 4, 2, 1, 2, 0},
    {1, 2, 4, 1, 1, 2, 0},
    {1, 2, 4, 2, 1, 1, 0}, // 85
    {4, 1, 1, 2, 1, 2, 0},
    {4, 2, 1, 1, 1, 2, 0},
    {4, 2, 1, 2, 1, 1, 0},
    {2, 1, 2, 1, 4, 1, 0},
    {2, 1, 4, 1, 2, 1, 0}, // 90
    {4, 1, 2, 1, 2, 1, 0},
    {1, 1, 1, 1, 4, 3, 0},
    {1, 1, 1, 3, 4, 1, 0},
    {1, 3, 1, 1, 4, 1, 0},
    {1, 1, 4, 1, 1, 3, 0}, // 95
    {1, 1, 4, 3, 1, 1, 0},
    {4, 1, 1, 1, 1, 3, 0},
    {4, 1, 1, 3, 1, 1, 0},
    {1, 1, 3, 1, 4, 1, 0},
    {1, 1, 4, 1, 3, 1, 0}, // 100
    {3, 1, 1, 1, 4, 1, 0},
    {4, 1, 1, 1, 3, 1, 0},
    {2, 1, 1, 4, 1, 2, 0},
    {2, 1, 1, 2, 1, 4, 0},
    {2, 1, 1, 2, 3, 2, 0}, // 105
    {2, 3, 3, 1, 1, 1, 2, 0}
};

//numbers only
//http://en.wikipedia.org/wiki/Code_128

Code128::Code128( std::string& dt )
: BarCodeBase(dt), weight(0), weight_sum(0), check_sum(0)        
{
}

Code128::~Code128()
{
}

uint8_t* Code128::encode( size_t* retlen )
{
    if( data.size() > 0 )
    {
        size_t len = data.size();
        size_t index = 0;
        size_t count = 0;
        size_t pos = 0;
        size_t blen = 0;
        
        init();
        
        uint8_t* buffer = initBuffer(len, &blen);
        
        if ( buffer != nullptr )
        {
            count = appendData( &CODE_WEIGHT[CODE_START_B], buffer, pos, "StartCode" );
            pos += count;
            weight_sum = CODE_START_B;

            for( size_t cnt=0; cnt<len; cnt++ ) 
            {
                weight++;
                char ch = data.at(cnt);

                index = ch - 0x20; /// decrease by space.
                uint8_t* ch_weight = (uint8_t*)&CODE_WEIGHT[index];
                char tmpch[2] = { ch, 0x00 };
                count = appendData( ch_weight, buffer, pos, tmpch );
                pos += count;
                
                int weightByValue = weight * index; 
                weight_sum += weightByValue;
            }

            check_sum = weight_sum % DIVISOR;

            count = appendData( CODE_WEIGHT[check_sum], buffer, pos, "CheckSum" );
            pos += count;
            count = appendData( CODE_WEIGHT[CODE_STOP], buffer, pos, "CODE_STOP" );
            pos += count;

#ifdef DEBUG_META_INFO
            printCode128MetaInfo();
#endif /// of DEBUG_META_INFO

            if ( retlen != nullptr )
                *retlen = blen;
            
            return buffer;
        }
    }
    
    return nullptr;
}

Fl_RGB_Image* Code128::getImage( uint32_t width, uint32_t height) 
{
    size_t inputWidth = 0;
    uint8_t* code = encode( &inputWidth );
    
    if ( code != nullptr )
    {
        // Add quiet zone on both sides
        size_t fullWidth    = inputWidth + (6);
        size_t outputWidth  = __MAX(width, fullWidth);
        size_t outputHeight = __MAX(1, height) - BOTTOM_GAP;

        size_t multiple     = outputWidth / fullWidth;
        size_t leftPadding  = (outputWidth - (inputWidth * multiple)) / 2;

        Fl_RGB_Image* bitmap = fl_imgtk::makeanempty( width, height, 4, colBg );

        if ( bitmap != nullptr )
        {   
            size_t cfsize = 0;
            
            FLFTRender* ftr = new FLFTRender( ttfname.c_str(), 0 );
            
            if ( ftr == nullptr )
            {
                fprintf( stderr, "Font load failure = %s\n", ttfname.c_str() );
            }
            else
            {
                cfsize = height * 0.1f; /// 10% of height is font size.
            }
            
            for ( size_t inputX=0, outputX=leftPadding; \
                  inputX<inputWidth; \
                  inputX++, outputX += multiple )
            {
                if ( code[inputX] == 1 ) 
                {
                    fl_imgtk::\
                    draw_fillrect( bitmap, 
                                   outputX, TOP_GAP, 
                                   multiple, outputHeight - cfsize + 5,
                                   colFg );
                }
            }
            
            if ( ftr != nullptr )
            {
                if ( ftr->FontLoaded() == true )
                {                    
                    ftr->FontSize( cfsize );

                    std::string str;
                    
                    if ( spaced == true )
                        str = insertSpace( data );
                    else
                        str = data;
                    
                    FLFTRender::Rect mbox = {0, 0, 0, 0};
                    ftr->MeasureText( str.c_str(), mbox );
                    
                    unsigned w_x = ( width - mbox.w ) / 2;
                    unsigned w_y = height - mbox.h - 5;
                    
#ifdef DRAW_FONT_BACK_RECTANGLE
                    uint32_t maskedCol = colBg & 0x000000FF;
                    fl_imgtk::\
                    draw_fillrect( bitmap,
                                   w_x, w_y,
                                   mbox.w, mbox.h,
                                   maskedCol );
#endif /// of DRAW_FONT_BACK_RECTANGLE

                    // need draw it to remove transparency,
                    ftr->FontColor( colMaskFg ); /// white for remove transparency
                    ftr->RenderText( bitmap, w_x, w_y, str.c_str() );
                    ftr->FontColor( colFg ); /// black, non-alpha.
                    ftr->RenderText( bitmap, w_x, w_y, str.c_str() );

#ifdef DEBUG
                    fprintf( stdout, "Font rendered [%s] at %u, %u\n",
                             str.c_str(), w_x, w_y );
                    fflush( stdout );
#endif /// of DEBUG
                }
                
                delete ftr;
            }
            else
            {
                
            }            
        }
        
        return bitmap;
    }
    
    return nullptr;
}

void Code128::printMetaInfo() 
{
    fprintf( stdout, "sum: %zu\n", weight_sum);
    fprintf( stdout, "divisor: %zu\n", DIVISOR);
    fprintf( stdout, "sum/divisor: %zu\n", (weight_sum/DIVISOR));
    fprintf( stdout, "check sum value: %zu\n", check_sum);
    fflush( stdout );
}	

void Code128::init() 
{
    weight = 0;
    weight_sum = 0;
    check_sum = 0;    
}

uint8_t* Code128::initBuffer(size_t dataLen, size_t* retlen)
{
    if ( dataLen > 0 )
    {
        //1. add start code 11byte
        //2. add encoded data 11 byte * dataLen
        //3. add check sum
        //4. add end code 12byte
        size_t sum = 11 + (dataLen * 11) + 11 + 13;
                        
        // sum = 11 + 11 + 12 + (11*dataLen);
        if ( retlen != nullptr )
            *retlen = sum;

        return new uint8_t[sum];
    }
    
    return nullptr;
}


std::string Code128::insertSpace(std::string& data) 
{
    std::string sb;
    
    size_t queue = 1;
    size_t len = data.length();
    
    for( size_t cnt=0; cnt<len; cnt++, queue++) 
    {
        sb += data.at(cnt);
        
        if( queue % 4 == 0 ) 
        {
            sb += " ";
        }
    }
    
    return sb;
}

size_t Code128::appendData(const void* weights, uint8_t* dst, int pos, std::string debugdata)
{
    size_t   count = 0;
    size_t   index = pos;
    uint8_t  color = 1;
    uint8_t* pW = (uint8_t*)weights;
    
    while( *pW != 0 )
    {
        uint8_t weight = *pW;
        for( size_t cnt=0; cnt<weight; cnt++ ) 
        {
            dst[index] = color;
            index++;
            count++;
        }
        
        color ^= 1;
        pW++;
    }
            
    return count;
}	
