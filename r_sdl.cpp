#define vgl_notextern
#include <X11/cursorfont.h>
#include "interfaces.h"
#include "r_private.h"
#include "r_interface.h"


#include "SDL/SDL.h"

static SDL_Surface* sdl_surf_display = 0;
sh_var_t *r_devicewidth, *r_deviceheight;
static sh_var_t *r_fullscreen;

static int              isshift = 0;

static char             shiftmap[256];

keyevent_t              keventlist[128];
unsigned int            keventlistptr;
int     md_x, md_y;

gl_info_t       *r_glinfo = NULL;
/*
  ====================
  R_StartUp

  ====================
*/
void R_StartUp()
{
    TFUNC_ENTER;
    
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        __error( "SDL_Init failed\n" );
    }
    
    r_devicewidth = SHP_GetVar( "r_devicewidth" );
    r_deviceheight = SHP_GetVar( "r_deviceheight" );
    r_fullscreen = SHP_GetVar( "r_fullscreen" );
 
    if(( sdl_surf_display = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_OPENGL )) == NULL) {
        __error( "SDL_SetVidMode failed\n" );;
    }
    
    
    // hard coded gl info. I assume that _every_ card in use on this planet supports this stuff...
    r_glinfo = (gl_info_t *)MM_Malloc( sizeof( gl_info_t ));
    r_glinfo->arb_multitexture = 1;
    r_glinfo->texenv_units = 1;
    r_glinfo->texenv_have_add = 1;
    
    
    glBegin( GL_POLYGON );
    
    glVertex3f( -0.5, -0.5, 0 );
    glVertex3f( 0.5, -0.5, 0 );
    glVertex3f( 0.5, 0.5, 0 );
    glVertex3f( -0.5, 0.5, 0 );
    glEnd();
    
    glFlush();
    SDL_GL_SwapBuffers();
    glClear( GL_COLOR_BUFFER_BIT );

// let e'm say sth
    R_Talk();
    
    if( r_fullscreen->ivalue )
        R_Hint( R_HINT_GRAB_MOUSE );
    
    TFUNC_LEAVE;
}

/*
  ====================
  R_ShutDown

  ====================
*/
void R_ShutDown()
{
        TFUNC_ENTER;

        
        __named_message( "\n" );
        

        TFUNC_LEAVE;
}

/*
  ====================
  R_SwapBuffer

  ====================
*/
void R_SwapBuffer( void )
{
//      __named_message( "\n" );
        glFlush();
        SDL_GL_SwapBuffers();
}




///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void R_Hint( int hint )
{
    switch( hint )
    {
    case R_HINT_GRAB_MOUSE:
        SDL_WM_GrabInput(SDL_GRAB_ON);
        break;
        
    case R_HINT_UNGRAB_MOUSE:
        SDL_WM_GrabInput(SDL_GRAB_OFF);
        break;
        
    default:
        __warning( "unknown render hint: %d\n", hint );
    }
}


void I_SDLDoKey( SDL_Event event );
void I_SDLDoMouseMotion( SDL_Event event );
void I_SDLDoMouseButton( SDL_Event event );

void I_Update()
{
    SDL_Event event;
    keventlistptr = 1;
    md_x = md_y = 0;
    
    while ( SDL_PollEvent(&event) ) {
        switch (event.type) {
            case SDL_MOUSEMOTION:
                I_SDLDoMouseMotion( event );
                break;
            
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN:
                I_SDLDoMouseButton( event );
                break;
                
                
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                I_SDLDoKey( event );
                break;
                
            default:
                printf( "sdl event: %d\n", event.type );
        }
    }
    
    keventlist[0].sym = (gs_ksym)keventlistptr; // first element in list is listsize!
}


void I_SDLStartUp( Window arg_window, Display *arg_display )
{
        

    for( size_t i = 0; i < 256; i++ )
    {
        shiftmap[i] = 0;
    }
    
    shiftmap['1'] = '!';
    shiftmap['2'] = '@';
    shiftmap['3'] = '#';
    shiftmap['4'] = '$';
    shiftmap['5'] = '%';
    shiftmap['6'] = '^';
    shiftmap['7'] = '&';
    shiftmap['8'] = '*';
    shiftmap['9'] = '(';
    shiftmap['0'] = ')';
    shiftmap['-'] = '_';
    shiftmap['='] = '+';
    shiftmap['\\'] = '|';
    shiftmap['['] = '{';
    shiftmap[']'] = '}';
    shiftmap[';'] = ':';
    shiftmap['\''] = '"';
    shiftmap[','] = '<';
    shiftmap['.'] = '>';
    shiftmap['/'] = '?';
    

}

void I_X11ShutDown( Window arg_window, Display *arg_display )
{
    SDL_WM_GrabInput(SDL_GRAB_OFF);
}




void I_X11GrabMouse( Display *arg_display, int flag )
{
    
    
    switch( flag )
    {
    case 1:
        SDL_WM_GrabInput(SDL_GRAB_ON);
        break;
        
    case 0:
        SDL_WM_GrabInput(SDL_GRAB_OFF);
        break;
        
    }

}
                                
void I_SDLDoKey( SDL_Event event )
{
        
        unsigned int    gsksym = 0;

        unsigned int type = event.key.type == SDL_KEYDOWN ? SYMTYPE_PRESS : SYMTYPE_RELEASE;

        // do abstraction from X11 keysym to gsksym

        switch( event.key.keysym.sym )
        {
// ascii control codes 0x0 to 0x1F

        case SDLK_BACKSPACE:
                gsksym = '\b';
                goto abs_end;

        case SDLK_TAB:
                gsksym = '\t';
                goto abs_end;   
        
        case SDLK_RETURN:
                gsksym = '\n';
                goto abs_end;   

// cursor keys
        case SDLK_UP:
        //case SDLK_KP:
                gsksym = GSK_CUP;
                goto abs_end;   

        case SDLK_DOWN:
        //case XK_KP_Down:
                gsksym = GSK_CDOWN;
                goto abs_end;   

        case SDLK_RIGHT:
//         case XK_KP_Right:
                gsksym = GSK_CRIGHT;
                goto abs_end;

        case SDLK_LEFT:
//         case XK_KP_Left:
                gsksym = GSK_CLEFT;
                goto abs_end;


// modifier keys
        case SDLK_RSHIFT:
        case SDLK_LSHIFT:
                gsksym = GSK_SHIFT;
                if( !isshift )
                        isshift = 1;
                else
                        isshift = 0;
                SHV_Printf( "isshift" );
                goto abs_end;

        case SDLK_RCTRL:
        case SDLK_LCTRL:
            gsksym = GSK_CTRL;
                goto abs_end;

        case SDLK_RALT:
        case SDLK_LALT:
//         case XK_Meta_L:
//         case XK_Meta_R:
                gsksym = GSK_ALT;
                goto abs_end;

                
        case SDLK_RSUPER:
        case SDLK_LSUPER:
                gsksym = GSK_FUCK0;
                goto abs_end;

//         case SDLK_RH:
//         case XK_Hyper_R:
//                 gsksym = GSK_FUCK1;
//                 goto abs_end;   
//                 // other cursor conrol keys

        case SDLK_INSERT:
//         case XK_KP_Insert:      
                gsksym = GSK_INSERT;
                goto abs_end;

        case SDLK_HOME:
//         case XK_KP_Home:
                gsksym = GSK_HOME;
                goto abs_end;   
                
        case SDLK_END:
//         case XK_KP_End:
                gsksym = GSK_END;
                goto abs_end;

        case SDLK_PAGEUP:
//         case XK_KP_Page_Up:
                gsksym = GSK_PGUP;
                goto abs_end;

        case SDLK_PAGEDOWN:
//         case XK_KP_Page_Down:
                gsksym = GSK_PGDN;
                goto abs_end;   

        case SDLK_ESCAPE:
                gsksym = GSK_ESCAPE;
                goto abs_end;

// function keys

        case SDLK_F1:
                gsksym = GSK_F1;
                goto abs_end;


        case SDLK_F2:
                gsksym = GSK_F2;
                goto abs_end;

        case SDLK_F3:
                gsksym = GSK_F3;
                goto abs_end;

        case SDLK_F4:
                gsksym = GSK_F4;
                goto abs_end;

        case SDLK_F5:
                gsksym = GSK_F5;
                goto abs_end;

        case SDLK_F6:
                gsksym = GSK_F6;
                goto abs_end;

        case SDLK_F7:
                gsksym = GSK_F7;
                goto abs_end;

        case SDLK_F8:
                gsksym = GSK_F8;
                goto abs_end;

        case SDLK_F9:
                gsksym = GSK_F9;
                goto abs_end;

        case SDLK_F10:
                gsksym = GSK_F10;
                goto abs_end;

        case SDLK_F11:
                gsksym = GSK_F11;
                goto abs_end;

        case SDLK_F12:
                gsksym = GSK_F12;
                goto abs_end;

        default:
        
            if( ( event.key.keysym.sym >= 0x020 ) && ( event.key.keysym.sym <= 0x07f ))
            {
                gsksym = event.key.keysym.sym;   
            }
            goto abs_end;

        }

 
abs_end:
        if( shiftmap[gsksym] && isshift )
                gsksym = shiftmap[gsksym];
        
        keventlist[keventlistptr].sym = (gs_ksym)gsksym;
        keventlist[keventlistptr].type = type;
        keventlistptr++;
//      printf( "gsksym: %d\n", gsksym );
}


void I_SDLDoMouseMotion( SDL_Event event )
{
    
    
    
    md_x += event.motion.xrel;
    md_y += event.motion.yrel;
    
        
}
        



void I_SDLDoMouseButton( SDL_Event event )
{
    unsigned int    gsksym = 0;
    unsigned int    type = event.button.type == SDL_MOUSEBUTTONDOWN ? SYMTYPE_PRESS : SYMTYPE_RELEASE;
    
    
    switch( event.button.button )
    {
        case 1:
            gsksym = GSK_BUTTON1;
            break;
            
        case 2:
            gsksym = GSK_BUTTON2;
            break;
            
        case 3:
            gsksym = GSK_BUTTON3;
            break;
            
        case 4:
            gsksym = GSK_BUTTON4;
            break;
            
            
        case 5:
            gsksym = GSK_BUTTON5;
            break;          
            
        default:
            __warning( "noknown mousebutton pressed\n" );
                break;
    }
    
    keventlist[keventlistptr].sym = (gs_ksym)gsksym;
    keventlist[keventlistptr].type = type;
    keventlistptr++;
}


void I_SetInputMode( int _mode )
{
        __named_message( "\n" );
}


unsigned char I_GetChar()
{
        return  'q';
}


