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
#include <FL/Fl_SVG_Image.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/fl_show_colormap.H>
#include <FL/fl_ask.H>
#include <fl_imgtk.h>

#include "resource.h"
#include "Code128.h"
#include "EAN13.h"
#include "QRcode.h"
#include "fl_pngwriter.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

#define DEF_APP_CLSNAME         "fltkbcgen"

////////////////////////////////////////////////////////////////////////////////

const char* convLoc                 = nullptr;
const char* convLng                 = nullptr;

char* argv_me_path                  = nullptr;
char* argv_me_bin                   = nullptr;

Fl_Double_Window*   window          = nullptr;
Fl_Group*           grpDiv0         = nullptr;
Fl_Group*           grpDiv1         = nullptr;
Fl_Group*           grpDiv2         = nullptr;
Fl_Group*           grpDiv3         = nullptr;
Fl_Group*           grpDiv10        = nullptr;
Fl_Input*           inpCode         = nullptr;
Fl_Button*          btnGenerate     = nullptr;
Fl_Choice*          chsType         = nullptr;
Fl_Box*             boxRender       = nullptr;
Fl_RGB_Image*       imgBarCode      = nullptr;
char*               svgQRCode       = nullptr;
Fl_Menu_Button*     popMenu         = nullptr;

uint32_t            codeCol         = 0x99AACCFF;
uint32_t            codeBackCol     = 0x50505000;

static string ttfFontFaceFile       = "DejaVuSansMono.ttf";

////////////////////////////////////////////////////////////////////////////////

// #define DEBUG_TRANSPARENCY_DRAW_BACK

////////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
    #include <iostream>
    #include <list>

    using namespace std;

      typedef struct {
	      uint64_t address;
	      size_t   size;
	      char     file[64];
	      size_t   line;
      } ALLOC_INFO;

      typedef list<ALLOC_INFO*> AllocList;

      AllocList *allocList;

      void AddTrack(uint64_t addr,  size_t asize,  const char *fname, size_t lnum)
      {
	      ALLOC_INFO *info;

	      if(!allocList) 
          {
		      allocList = new(AllocList);
	      }

	      info = new(ALLOC_INFO);
	      info->address = addr;
	      strncpy(info->file, fname, 63);
	      info->line = lnum;
	      info->size = asize;
	      allocList->insert(allocList->begin(), info);
      };

      void RemoveTrack(uint64_t addr)
      {
	      AllocList::iterator i;

	      if(!allocList)
		      return;
          
	      for(i = allocList->begin(); i != allocList->end(); i++)
	      {
		      if((*i)->address == addr)
		      {
			      allocList->remove((*i));
			      break;
		      }
	      }
      };
      
      inline void * __cdecl operator new(size_t size)
      {
	      void *ptr = (void *)malloc(size);
        
          if ( ptr != NULL )
            AddTrack((size_t)ptr, size, "MEM", 0);
        
	      return(ptr);          
      };

      inline void __cdecl operator delete(void *p)
      {
	      RemoveTrack((size_t)p);
	      free(p);
      };
      
      void DumpUnfreed()
      {
	      AllocList::iterator i;
	      size_t totalSize = 0;
	      char buf[1024] = {0};

	      if(!allocList)
		      return;

	      for(i = allocList->begin(); i != allocList->end(); i++) 
          {
		      sprintf(buf, "%-50s:\t\tLINE %zu,\t\tADDRESS %X\t%zu unfreed\n",
			      (*i)->file, (*i)->line, (*i)->address, (*i)->size );
		      OutputDebugStringA(buf);
		      totalSize += (*i)->size;
	      }
	      sprintf(buf, "-----------------------------------------------------------\n");
	      OutputDebugStringA(buf);
	      sprintf(buf, "Total Unfreed: %zu bytes\n", totalSize);
	      OutputDebugStringA(buf);
      };
       
#endif /// of DEBUG

void usr_scale(Fl_RGB_Image* s, int x, int y, int w, int h, Fl_RGB_Image** o)
{
    printf( "(debug)usr_scale( %p, %d, %d, %d, %d, .. );\n",
            s, x, y, w, h );

    if ( s != NULL )
    {
        printf( "(debug)s->w() = %d, s->h() = %d\n", s->w(), s->h() );
        Fl_RGB_Image* r = fl_imgtk::rescale( s, w, h, fl_imgtk::BICUBIC );
        printf( "(debug)o->w() = %d, o->h() = %d\n", r->w(), r->h() );
        *o = r;
    }
    
    fflush( stdout );
}

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

void disablePopMenuSVG()
{
    if ( popMenu != nullptr )
    {
        const Fl_Menu_Item* item = popMenu->menu();
        if ( item != nullptr )
        {
            item++;
            Fl_Menu_Item* forcecasted = (Fl_Menu_Item*)item;
            forcecasted->deactivate();
        }
    }
}

void enablePopMenuSVG()
{
    if ( popMenu != nullptr )
    {
        const Fl_Menu_Item* item = popMenu->menu();
        if ( item != nullptr )
        {
            item++;
            Fl_Menu_Item* forcecasted = (Fl_Menu_Item*)item;
            forcecasted->activate();
        }
    }
}

bool Save2SVG()
{
    if ( svgQRCode == nullptr )
        return false;
    
    Fl_Native_File_Chooser nFC;

    static string presetfn;

    nFC.title( "Select PNG file to save." );
    nFC.type( Fl_Native_File_Chooser::BROWSE_SAVE_FILE );
    nFC.options( Fl_Native_File_Chooser::USE_FILTER_EXT
                 | Fl_Native_File_Chooser::SAVEAS_CONFIRM );
    nFC.filter( "PNG Image\t*.png" );

    int retVal = nFC.show();

    if ( retVal == 0 )
    {
        string imgFNameUTF8 = nFC.filename();
        
        // auto appends "svg" ext.

#ifdef _WIN32
        size_t seppos = imgFNameUTF8.find_last_of( "\\" );
        if ( seppos == string::npos )
            seppos = imgFNameUTF8.find_last_of( "/" );
#else
        size_t seppos = imgFNameUTF8.find_last_of( "/" );
#endif
        if ( seppos == string::npos )
            seppos = 0;
        
        size_t extdotpos = imgFNameUTF8.find_last_of( seppos, '.' );
        if ( extdotpos == string::npos )
        {
            imgFNameUTF8 += ".svg";
        }
                
        printf( "txtbuff = %p\n", svgQRCode );
        fflush( stdout );
        
        size_t txtbufflen = strlen( svgQRCode );
        
        printf( "txtbufflen = %zu\n", txtbufflen );
        fflush( stdout );
        
        if ( txtbufflen > 0 )
        {
            FILE* fp = fopen( imgFNameUTF8.c_str(), "wb" );
            if ( fp != nullptr )
            {
                fwrite( svgQRCode, txtbufflen, 1, fp );
                fclose( fp );
                
                return true;
            }
        }
    }

    return false;    
}

bool Save2PNG()
{
    if ( imgBarCode == nullptr )
        return false;
    
    Fl_Native_File_Chooser nFC;

    static string presetfn;

    nFC.title( "Select PNG file to save." );
    nFC.type( Fl_Native_File_Chooser::BROWSE_SAVE_FILE );
    nFC.options( Fl_Native_File_Chooser::USE_FILTER_EXT
                 | Fl_Native_File_Chooser::SAVEAS_CONFIRM );
    nFC.filter( "PNG Image\t*.png" );

    int retVal = nFC.show();

    if ( retVal == 0 )
    {
        string imgFNameUTF8 = nFC.filename();
        
        // auto appends "png" ext.

#ifdef _WIN32
        size_t seppos = imgFNameUTF8.find_last_of( "\\" );
        if ( seppos == string::npos )
            seppos = imgFNameUTF8.find_last_of( "/" );
#else
        size_t seppos = imgFNameUTF8.find_last_of( "/" );
#endif
        if ( seppos == string::npos )
            seppos = 0;
        
        size_t extdotpos = imgFNameUTF8.find_last_of( seppos, '.' );
        if ( extdotpos == string::npos )
        {
            imgFNameUTF8 += ".png";
        }
        
        return fl_image_write_to_pngfile( imgBarCode, imgFNameUTF8.c_str(), 7 );
    }

    return false;
}

void fl_wcb( Fl_Widget* w )
{
    if ( w == window )
    {
        window->deactivate();
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
            
            if ( svgQRCode != nullptr )
            {
                delete[] svgQRCode;
                svgQRCode = nullptr;
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
                        c128->ForegroundColor( codeCol );
                        c128->FontFace( ttfFontFaceFile );
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
                        ean13->ForegroundColor( codeCol );
                        ean13->FontFace( ttfFontFaceFile );
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
                
                case 2: /// == QR
                {
                    QRCode* qrc = new QRCode( strCode );
                    if ( qrc != nullptr )
                    {
                        // testing random color --
                        //qrc->ForegroundColor( rand() );
                        qrc->ForegroundColor( codeCol );
                        qrc->getSVG( &svgQRCode );
                        
                        imgBarCode = qrc->getImage( img_w, img_h );
                        
                        delete qrc;
                    }
                }
                break;
            }
        }
        
        if ( imgBarCode != nullptr )
        {
            if ( boxRender->label() != nullptr )
            {
                boxRender->label( nullptr );
            }
            
            boxRender->image( imgBarCode );
            boxRender->redraw();
        }
        
        btnGenerate->activate();
        inpCode->activate();
        return;
    }
    
    if ( w == popMenu )
    {
        switch( popMenu->value() )
        {
            case 0:
                Save2PNG();
                break;
                
            case 1:
                Save2SVG();
                break;
                
            case 2: // fore-color...
                {
                    // care for FLTK color ---
                    uint32_t fltkcol = codeCol & 0xFFFFFF00;
                    uint32_t colsel = fl_show_colormap( fltkcol );

                    if ( colsel != fltkcol )
                    {
                        if ( colsel < 256 )
                        {
                            colsel = Fl::get_color( colsel );
                            
                            if (colsel == 0 )
                                codeCol = 0x010101FF;
                            else
                                codeCol = colsel | 0x000000FF;
                        }

                        if ( boxRender->image() != nullptr )
                        {
                            btnGenerate->do_callback();
                        }
                    }
                }
                break;
                
            case 3: // back-color ...
                {
                    // care for FLTK color ---
                    uint32_t fltkcol = codeBackCol & 0xFFFFFF00;
                    uint32_t colsel = fl_show_colormap( fltkcol );
                        
                    if ( colsel != fltkcol )
                    {                        
                        if ( colsel < 256 )
                        {
                            colsel = Fl::get_color( colsel );

                            if (colsel == 0 )
                                codeBackCol = 0x010101FF;
                            else
                                codeBackCol = colsel | 0x000000FF;                            
                        }

                        if ( boxRender->image() != nullptr )
                        {
                            boxRender->color( codeBackCol );
                            boxRender->redraw();
                        }                        
                    }                    
                }
                break;

            default:
                break;
        }
        
        return;
    }
    
    if ( w == chsType )
    {
        if ( window->visible() == 0 )
            return;
        
        switch( chsType->value() )
        {
            case 0: // Code128
            case 1: // EAN13
                disablePopMenuSVG();
                break;
            
            case 2: // QR
                enablePopMenuSVG();
                break;
        }
        
        return;
    }
}

void createWindow()
{
    window = new Fl_Double_Window( 600, 300, "Barcode/QR Generator Testing" );
    if ( window != nullptr )
    {
        window->color( 0x30303000 );
        window->labelcolor( 0xE0E0E000 );

        grpDiv0 = new Fl_Group( 0, 0, 600, 35 );
        if ( grpDiv0 != nullptr )
        {
            grpDiv0->begin();
            
            grpDiv1 = new Fl_Group( 50, 0, 105, 30 );
            if ( grpDiv1 != nullptr )
            {
                grpDiv1->begin();
            }
            
            chsType = new Fl_Choice( 50, 5, 100, 25, "Type : " );
            if ( chsType != nullptr )
            {
                chsType->box( FL_THIN_UP_BOX );
                chsType->when( FL_WHEN_CHANGED );
                chsType->add( "CODE128" );
                chsType->add( "EAN13" );
                chsType->add( "QR" );
                chsType->value( 0 );
                chsType->color( window->color() );
                chsType->labelcolor( window->labelcolor() );
                chsType->textcolor( window->labelcolor() );
                chsType->callback( fl_wcb );
            }

            if ( grpDiv1 != nullptr )
            {
                grpDiv1->end();
            }

            grpDiv2 = new Fl_Group( 155, 0, 350, 30 );
            if ( grpDiv2 != nullptr )
            {
                grpDiv2->begin();
            }
            
            inpCode = new Fl_Input( 155, 5, 345, 25 );
            if ( inpCode != nullptr )
            {
                inpCode->textfont( FL_COURIER );
                inpCode->when( FL_WHEN_ENTER_KEY );
                inpCode->color( fl_darker( window->color() ) );
                inpCode->labelcolor( window->labelcolor() );
                inpCode->textcolor( window->labelcolor() );
                inpCode->cursor_color( fl_darker( window->labelcolor() ) );
                inpCode->callback( fl_wcb );
            }

            if ( grpDiv2 != nullptr )
            {
                grpDiv2->end();
            }

            grpDiv3 = new Fl_Group( 505, 0, 95, 30 );
            if ( grpDiv3 != nullptr )
            {
                grpDiv3->begin();
            }
            
            btnGenerate = new Fl_Button( 505, 5, 90, 25, "Generate" );
            if ( btnGenerate != nullptr )
            {
                btnGenerate->color( fl_lighter( window->color() ) );
                btnGenerate->labelcolor( window->labelcolor() );
                btnGenerate->callback( fl_wcb );
            }

            if ( grpDiv2 != nullptr )
            {
                grpDiv2->end();
            }

            grpDiv0->end();
            
            if ( grpDiv2 != nullptr )
                grpDiv0->resizable( grpDiv2 );
        }
        
        grpDiv10 = new Fl_Group( 0,35, 600, 265 );
        if ( grpDiv10 != nullptr )
        {
            grpDiv10->begin();
            
            boxRender = new Fl_Box( 0, 35, 600, 265 );
            if ( boxRender != nullptr )
            {
                boxRender->box( FL_THIN_DOWN_BOX );
                boxRender->align( FL_ALIGN_CLIP );
                boxRender->color( codeBackCol );
                boxRender->labelsize( window->labelsize() * 2 );
                boxRender->labelcolor( fl_darker( window->labelcolor() ) );
                boxRender->label( "Right click to more menu" );
            }
            
            grpDiv10->end();
        }

#ifdef __APPLE__            
    #define FL_C_   FL_COMMAND
#else
    #define FL_C_   FL_CTRL
#endif /// of __APPLE__

        popMenu = new Fl_Menu_Button( 0, 0, window->w(), window->h() );
        if ( popMenu != nullptr )
        {
            popMenu->color( fl_darker( window->color() ) );
            popMenu->labelcolor( window->labelcolor() );
            popMenu->textcolor( window->labelcolor() );
            popMenu->type( Fl_Menu_Button::POPUP3 );
            popMenu->add( "Save to PNG ...\t",      FL_C_ + 's', 0, 0, 0 );
            popMenu->add( "Save to SVG ...\t",      FL_C_ + 'v', 0, 0, 0 );
            popMenu->add( "Change fore-color\t",    FL_C_ + 'c', 0, 0, 0 );
            popMenu->add( "Change back-color\t",    FL_C_ + 'b', 0, 0, 0 );
            popMenu->mode( 1, FL_MENU_DIVIDER );
            popMenu->callback( fl_wcb );
            
            // disable SVG for now.
            disablePopMenuSVG();
        }

        if ( grpDiv10 != nullptr )
            window->resizable( grpDiv10 );

        window->end();
        window->callback( fl_wcb );
        window->size_range( window->w(), window->h() );
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

#ifdef FLTK_EXT_VERSION
    Fl::scheme( "flat" );
    
    Fl_Image:: scaling_algorithm( FL_RGB_SCALING_USER, usr_scale );
#else
    Fl::scheme( "gtk+" );
    // Default RGB image scaling for high-DPI, but not works on Windows.
    Fl_Image::scaling_algorithm( FL_RGB_SCALING_BILINEAR );
#endif /// of FLTK_EXT_VERSION

}

int main (int argc, char ** argv)
{
    int reti = 0;

    parserArgvZero( argv[0] );
    presetFLTKenv();
    createWindow();
    
    // check where TTF is ...
    if ( access( ttfFontFaceFile.c_str(), 0 ) != 0 )
    {
        ttfFontFaceFile.insert( 0, "./ttf/" );
    }

    reti = Fl::run();
    
    if ( imgBarCode != nullptr )
    {
        delete imgBarCode;
    }
    
    if ( svgQRCode != nullptr )
    {
        delete svgQRCode;
    }

    if ( argv_me_path != NULL )
    {
        free( argv_me_path );
    }

    if ( argv_me_bin != NULL )
    {
        free( argv_me_bin );
    }
    
#ifdef DEBUG
    DumpUnfreed();
#endif    
    
    return reti;
}
