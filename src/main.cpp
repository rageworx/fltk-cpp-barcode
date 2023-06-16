#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <string>

#include <FL/platform.H>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Choice.H>
#include <FL/fl_ask.H>

#include "resource.h"
#include "Code128.h"
#include "EAN13.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

#define DEF_APP_CLSNAME         "fltkbcgen"

////////////////////////////////////////////////////////////////////////////////

const char* convLoc                 = nullptr;
const char* convLng                 = nullptr;

char* argv_me_path                  = nullptr;
char* argv_me_bin                   = nullptr;

Fl_Double_Window*   window          = nullptr;
Fl_Input*           inpCode         = nullptr;
Fl_Button*          btnGenerate     = nullptr;
Fl_Choice*          chsType         = nullptr;
Fl_Box*             boxRender       = nullptr;
Fl_RGB_Image*       imgBarCode      = nullptr;

////////////////////////////////////////////////////////////////////////////////

void applyIcon()
{
#ifdef _WIN32
    extern HINSTANCE fl_display;

    HICON \
    hIconWindowLarge = (HICON)LoadImage( fl_display,
                                         MAKEINTRESOURCE( IDC_ICON_A ),
                                         IMAGE_ICON,
                                         256,
                                         256,
                                         LR_SHARED );
    HICON \
    hIconWindowSmall = (HICON)LoadImage( fl_display,
                                         MAKEINTRESOURCE( IDC_ICON_A ),
                                         IMAGE_ICON,
                                         16,
                                         16,
                                         LR_SHARED );

    if ( window != NULL )
    {
        SendMessage( fl_xid( window ),
                     WM_SETICON,
                     ICON_BIG,
                     (LPARAM)hIconWindowLarge );

        SendMessage( fl_xid( window ),
                     WM_SETICON,
                     ICON_SMALL,
                     (LPARAM)hIconWindowSmall );
    }
#endif
}

void fl_wcb( Fl_Widget* w )
{
    if ( w == window )
    {
        window->deactivate();
        if ( imgBarCode != nullptr )
        {
            boxRender->deimage();
            delete imgBarCode;
        }
        window->hide();
        return;
    }
    
    if ( ( w == btnGenerate ) || ( w == inpCode ) )
    {
        btnGenerate->deactivate();
        inpCode->deactivate();
        
        const char* pRef = inpCode->value();
        
        if ( pRef != nullptr )
        {
            string strCode = pRef;
            
            if ( imgBarCode != nullptr )
            {
                boxRender->deimage();
                delete imgBarCode;
                imgBarCode = nullptr;
            }
            
            unsigned img_w = boxRender->w() - 10;
            unsigned img_h = boxRender->h() - 10;
            
            switch( chsType->value() )
            {
                case 0: /// CODE128
                {
                    Code128* c128 = new Code128( strCode );
                    if ( c128 != nullptr )
                    {
                        imgBarCode = c128->getImage( img_w, img_h );
                        delete c128;
                    }
                }
                break;
                
                case 1: /// EAN13
                {
                    if ( strCode.size() != 13 )
                    {
                        fl_message_title( "ERROR" );
                        fl_message( "EAN13 code requires 13 digits." );
                        btnGenerate->activate();
                        inpCode->activate();                        
                        return;
                    }
                    
                    const char* pC = strCode.c_str();
                    bool testDigit = true;
                    
                    for( size_t cnt=0; cnt<strCode.size(); cnt++ )
                    {
                        if ( ( pC[cnt] < '0' ) || ( pC[cnt] >'9' ) )
                        {
                            testDigit = false;
                            break;
                        }
                    }
                    
                    if ( testDigit == false )
                    {
                        fl_message_title( "ERROR" );
                        fl_message( "EAN13 code onlt accepts digit." );
                        btnGenerate->activate();
                        inpCode->activate();                        
                        return;
                    }
                    
                    EAN13* ean13 = new EAN13( strCode );
                    if ( ean13 != nullptr )
                    {
                        imgBarCode = ean13->getImage( img_w, img_h );
                        delete ean13;
                        
                        if ( imgBarCode == nullptr )
                        {
                            fprintf( stdout, 
                                     "Failed to generate EAN13 barcode with %s\n",
                                     strCode.c_str() );
                            fflush( stdout );
                        }
                    }
                }
                break;
            }
        }
        
        if ( imgBarCode != nullptr )
        {
            boxRender->image( imgBarCode );
            boxRender->redraw();
        }
        
        btnGenerate->activate();
        inpCode->activate();
        return;
    }
}

void createWindow()
{
    window = new Fl_Double_Window( 600, 300, "Barcode Generator Test" );
    if ( window != nullptr )
    {
        chsType = new Fl_Choice( 50, 5, 100, 25, "Type : " );
        if ( chsType != nullptr )
        {
            chsType->add( "CODE128" );
            chsType->add( "EAN13" );
            chsType->value( 0 );
            chsType->callback( fl_wcb );
        }
        
        inpCode = new Fl_Input( 155, 5, 345, 25 );
        if ( inpCode != nullptr )
        {
            inpCode->textfont( FL_COURIER );
            inpCode->when( FL_WHEN_ENTER_KEY );
            inpCode->callback( fl_wcb );
        }
        
        btnGenerate = new Fl_Button( 505, 5, 90, 25, "Generate" );
        if ( btnGenerate != nullptr )
        {
            btnGenerate->callback( fl_wcb );
        }
        
        boxRender = new Fl_Box( 5, 40, 590, 255 );
        if ( boxRender != nullptr )
        {
            boxRender->box( FL_FLAT_BOX );
            boxRender->color( FL_WHITE );
        }

        window->end();
        window->callback( fl_wcb );
        window->show();
    }
    
    applyIcon();
}

void parserArgvZero( const char* argv0 )
{
    string argvextractor = argv0;

#ifdef _WIN32
    char splitter[] = "\\";
#else
    char splitter[] = "/";
#endif

    if ( argvextractor.size() > 0 )
    {
        string::size_type lastSplitPos = argvextractor.rfind( splitter );

        string extracted_path = argvextractor.substr(0, lastSplitPos + 1 );
        string extracted_name = argvextractor.substr( lastSplitPos + 1 );

        argv_me_path = strdup( extracted_path.c_str() );
        argv_me_bin  = strdup( extracted_name.c_str() );
    }
}

void presetFLTKenv()
{
    Fl::set_font( FL_FREE_FONT, convLng );
    Fl_Double_Window::default_xclass( DEF_APP_CLSNAME );

    Fl::scheme( "gtk+" );
}

int main (int argc, char ** argv)
{
    int reti = 0;

    parserArgvZero( argv[0] );
    presetFLTKenv();
    createWindow();

    reti = Fl::run();

    if ( argv_me_path != NULL )
    {
        free( argv_me_path );
    }

    if ( argv_me_bin != NULL )
    {
        free( argv_me_bin );
    }
    
    return reti;
}