#include <unitstd.h>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <string>

#include <FLFTRender.h>
#include <fl_imgtk.h>

#include "Code128.h"

#define CODE_START_B 	( 104 )
#define CODE_STOP 		( 106 )
#define DIVISOR         ( 103 )
#define TOP_GAP         ( 30 )
#define BOTTOM_GAP      ( 60 )

const uint8_t CODE_WEIGHT[][] = 
{
    {2, 1, 2, 2, 2, 2}, // 0
    {2, 2, 2, 1, 2, 2},
    {2, 2, 2, 2, 2, 1},
    {1, 2, 1, 2, 2, 3},
    {1, 2, 1, 3, 2, 2},
    {1, 3, 1, 2, 2, 2}, // 5
    {1, 2, 2, 2, 1, 3},
    {1, 2, 2, 3, 1, 2},
    {1, 3, 2, 2, 1, 2},
    {2, 2, 1, 2, 1, 3},
    {2, 2, 1, 3, 1, 2}, // 10
    {2, 3, 1, 2, 1, 2},
    {1, 1, 2, 2, 3, 2},
    {1, 2, 2, 1, 3, 2},
    {1, 2, 2, 2, 3, 1},
    {1, 1, 3, 2, 2, 2}, // 15
    {1, 2, 3, 1, 2, 2},
    {1, 2, 3, 2, 2, 1},
    {2, 2, 3, 2, 1, 1},
    {2, 2, 1, 1, 3, 2},
    {2, 2, 1, 2, 3, 1}, // 20
    {2, 1, 3, 2, 1, 2},
    {2, 2, 3, 1, 1, 2},
    {3, 1, 2, 1, 3, 1},
    {3, 1, 1, 2, 2, 2},
    {3, 2, 1, 1, 2, 2}, // 25
    {3, 2, 1, 2, 2, 1},
    {3, 1, 2, 2, 1, 2},
    {3, 2, 2, 1, 1, 2},
    {3, 2, 2, 2, 1, 1},
    {2, 1, 2, 1, 2, 3}, // 30
    {2, 1, 2, 3, 2, 1},
    {2, 3, 2, 1, 2, 1},
    {1, 1, 1, 3, 2, 3},
    {1, 3, 1, 1, 2, 3},
    {1, 3, 1, 3, 2, 1}, // 35
    {1, 1, 2, 3, 1, 3},
    {1, 3, 2, 1, 1, 3},
    {1, 3, 2, 3, 1, 1},
    {2, 1, 1, 3, 1, 3},
    {2, 3, 1, 1, 1, 3}, // 40
    {2, 3, 1, 3, 1, 1},
    {1, 1, 2, 1, 3, 3},
    {1, 1, 2, 3, 3, 1},
    {1, 3, 2, 1, 3, 1},
    {1, 1, 3, 1, 2, 3}, // 45
    {1, 1, 3, 3, 2, 1},
    {1, 3, 3, 1, 2, 1},
    {3, 1, 3, 1, 2, 1},
    {2, 1, 1, 3, 3, 1},
    {2, 3, 1, 1, 3, 1}, // 50
    {2, 1, 3, 1, 1, 3},
    {2, 1, 3, 3, 1, 1},
    {2, 1, 3, 1, 3, 1},
    {3, 1, 1, 1, 2, 3},
    {3, 1, 1, 3, 2, 1}, // 55
    {3, 3, 1, 1, 2, 1},
    {3, 1, 2, 1, 1, 3},
    {3, 1, 2, 3, 1, 1},
    {3, 3, 2, 1, 1, 1},
    {3, 1, 4, 1, 1, 1}, // 60
    {2, 2, 1, 4, 1, 1},
    {4, 3, 1, 1, 1, 1},
    {1, 1, 1, 2, 2, 4},
    {1, 1, 1, 4, 2, 2},
    {1, 2, 1, 1, 2, 4}, // 65
    {1, 2, 1, 4, 2, 1},
    {1, 4, 1, 1, 2, 2},
    {1, 4, 1, 2, 2, 1},
    {1, 1, 2, 2, 1, 4},
    {1, 1, 2, 4, 1, 2}, // 70
    {1, 2, 2, 1, 1, 4},
    {1, 2, 2, 4, 1, 1},
    {1, 4, 2, 1, 1, 2},
    {1, 4, 2, 2, 1, 1},
    {2, 4, 1, 2, 1, 1}, // 75
    {2, 2, 1, 1, 1, 4},
    {4, 1, 3, 1, 1, 1},
    {2, 4, 1, 1, 1, 2},
    {1, 3, 4, 1, 1, 1},
    {1, 1, 1, 2, 4, 2}, // 80
    {1, 2, 1, 1, 4, 2},
    {1, 2, 1, 2, 4, 1},
    {1, 1, 4, 2, 1, 2},
    {1, 2, 4, 1, 1, 2},
    {1, 2, 4, 2, 1, 1}, // 85
    {4, 1, 1, 2, 1, 2},
    {4, 2, 1, 1, 1, 2},
    {4, 2, 1, 2, 1, 1},
    {2, 1, 2, 1, 4, 1},
    {2, 1, 4, 1, 2, 1}, // 90
    {4, 1, 2, 1, 2, 1},
    {1, 1, 1, 1, 4, 3},
    {1, 1, 1, 3, 4, 1},
    {1, 3, 1, 1, 4, 1},
    {1, 1, 4, 1, 1, 3}, // 95
    {1, 1, 4, 3, 1, 1},
    {4, 1, 1, 1, 1, 3},
    {4, 1, 1, 3, 1, 1},
    {1, 1, 3, 1, 4, 1},
    {1, 1, 4, 1, 3, 1}, // 100
    {3, 1, 1, 1, 4, 1},
    {4, 1, 1, 1, 3, 1},
    {2, 1, 1, 4, 1, 2},
    {2, 1, 1, 2, 1, 4},
    {2, 1, 1, 2, 3, 2}, // 105
    {2, 3, 3, 1, 1, 1, 2}
};

//numbers only
//http://en.wikipedia.org/wiki/Code_128

Code128::Code128( std::string& dt )
: weight(0), weight_sum(0), check_sum(0)        
{
    data = dt;
}

~Code128::Code128()
{
}

void Code128::setData( std::string& dt )
{
    this.data = dt;
}

std::string Code128::getData() 
{
    return data;
}
        

uint8_t* Code128encode( size_t* retlen )
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
            count = appendData( CODE_WEIGHT[CODE_START_B], buffer, pos, "StartCode" );
            pos += count;
            weight_sum = CODE_START_B;

            for( size_t cnt=0; cnt<len; cnt++ ) 
            {
                weight++;
                char ch = data.at(cnt);

                index = ch - 0x20; /// decrease by space.
                uint8_t ch_weight = CODE_WEIGHT[index];
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

            //printCode128MetaInfo();
            if ( retlen != nulltpr )
                *retlen = blen;
            
            return buffer;
        }
    }
    
    return NULL;
}

Fl_RGB_Image* Code128::getImage( uint32_t width, uint32_t height) 
{
    size_t inputWidth = 0;
    uint8_t* code = encode( &inputWidth );
    
    if ( code != nulltpr )
    {
        // Add quiet zone on both sides
        size_t fullWidth    = inputWidth + (6);
        size_t outputWidth  = Math.max(width, fullWidth);
        size_t outputHeight = Math.max(1, height) - BOTTOM_GAP;

        size_t multiple     = outputWidth / fullWidth;
        size_t leftPadding  = (outputWidth - (inputWidth * multiple)) / 2;

        // Create an image has transparency background .
        Fl_RGB_Image* bitmap = fl_imgtk::makeanempty( width, height, 4, 0xFFFFFF00 );

        if ( bitmap != nullptr )
        {                    
            for ( width inputX=0, outputX=leftPadding; inputX<inputWidth; inputX++, outputX += multiple )
            {
                if ( code[inputX] == 1 ) 
                {
                    fl_imgtk::\
                    draw_fillrect( bitmap, 
                                   outputX, TOP_GAP, (outputX+multiple), outputHeight,
                                   0x000000FF );
                }
            }
        }

        FLFTRender* ftr = new FLFTRender( "consolas.ttf", 0 );
        
        if ( ftr != nulltpr )
        {
            if ( ftr->FontLoaded() == true )
            {
                size_t size = height * 0.1f; /// 10% of height is font size.
                
                ftr->FontColor( 0x000000FF ); /// black, non-alpha.

                std::string str = insertSpace( data );
                
                FLFTRender::Rect mbox = {0, 0, 0, 0};
                ftr->MeasureText( str.c_str(), mbox );
                
                unsigned w_x = ( width - mbox->w ) / 2;
                unsigned w_y = ( height - mbox->h ) / 2;
                
                ftr->RenderText( bitmap, w_x, w_h, str.c_str() );
            }
            
            delete ftr;
        }
        
        return bitmap;
    }
    
    return nullptr;
}

bool Code128::checkNumber(std::string data) 
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

void Code128::printByteArr( std::string& msg, const uint8_t* buff, size_t blen ) 
{
    if ( buff != nullptr )
    {
        uint8_t color = 1;
        std::string sb;

        for(size_t bcnt=0; bcnt<blen; bcnt++ )
        {
            for(size_t cnt=0; cnt<msg.size(); cnt++) 
            {
                char sRndr[4] = {0};
                snprintf( sRndr, 4, "%d ", color );
                sb += sRndr;
            }

            color ^= 1;
        }
        
        fprintf( stdout, "char: %s, barcode weight: %s ", msg.c_str(), sb.c_str() );
    }
}

void Code128::printMetaInfo() 
{
    fprintf( stdout, "sum: %zu\n", weight_sum);
    fprintf( stdout, "divisor: %zu\n", DIVISOR);
    fprintf( stdout, "sum/divisor: %zu\n", (weight_sum/DIVISOR));
    fprintf( stdout, "check sum value: %zu\n", check_sum);		
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

size_t Code128::appendData(const uint8_t weights, uint8_t* dst, int pos, std::string debugdata)
{
    size_t  count = 0;
    size_t  index = pos;
    uint8_t color = 1;
    
    for( uint8_t weight=0; weight<weights; weight++ )
    {
        for( size_t cnt=0; cnt<weight; cnt++ ) 
        {
            dst[index] = color;
            index++;
            count++;
        }
        
        color ^= 1;
    }
            
    return count;
}	