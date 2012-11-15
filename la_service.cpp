/* 
 * 3dyne Legacy Engine GPL Source Code
 * 
 * Copyright (C) 1996-2012 Matthias C. Berger & Simon Berger.
 * 
 * This file is part of the 3dyne Legacy Engine GPL Source Code ("3dyne Legacy
 * Engine Source Code").
 *   
 * 3dyne Legacy Engine Source Code is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 * 
 * 3dyne Legacy Engine Source Code is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * 3dyne Legacy Engine Source Code.  If not, see
 * <http://www.gnu.org/licenses/>.
 * 
 * In addition, the 3dyne Legacy Engine Source Code is also subject to certain
 * additional terms. You should have received a copy of these additional terms
 * immediately following the terms and conditions of the GNU General Public
 * License which accompanied the 3dyne Legacy Engine Source Code.
 * 
 * Contributors:
 *     Matthias C. Berger (mcb77@gmx.de) - initial API and implementation
 *     Simon Berger (simberger@gmail.com) - initial API and implementation
 */



//layouter.c
#include "interfaces.h"

#define	LA_CACHESIZE	( 32 )
#define LA_VARVIEWNUM	( 25 )


#if 0
typedef struct {
	char		name[64];
	ca_tga_t	*tga;
	int		r_tga;
} la_centry_t;




static la_centry_t	la_cache[256];
static int		la_cachelast;

static ca_tga_t		*tga_font16 = NULL;

#endif




typedef struct {
	sh_var_t	*var;
	void	(*callback)( int flag, char *string );
} la_varview_t;




static la_varview_t	la_varviews[LA_VARVIEWNUM];


static res_gltex_cache_t	*r_font16 = NULL;


void LA_FontTexel16( int asc, fp_t *tx, fp_t *ty, fp_t *twidth, fp_t *theight )
{
	int	x, y;
	
	TFUNC_ENTER;
	x = asc % 16;
	y = asc / 16;

	*tx = ((fp_t)(x)) / 16.0;
	*ty = ((fp_t)(y)) / 16.0;
	
	*ty = 1.0-(*ty) - 1.0/16.0;

	*twidth = 1.0 / 16.0;
	*theight = 1.0 / 16.0;
	TFUNC_LEAVE;
}

void LA_RegisterFont( const char *name )
{
	TFUNC_ENTER;
	if( !r_font16 )
	{
		g_resource_t	*r;
		
		r = G_ResourceSearch( g_rs, name );
		__chkptr( r );
		r_font16 = (res_gltex_cache_t *) r->res_cache;
	}
	TFUNC_LEAVE;
}	

void LA_StartUp()
{
	TFUNC_ENTER;
	
	G_ResourceFromClass( g_rs, "res/hud.hobj" );
	r_font16 = NULL;
	LA_RegisterFont( "misc.tex.font16" );

	memset( la_varviews, 0, sizeof( la_varview_t ) * LA_VARVIEWNUM );

	LA_InitVarviews();

	TFUNC_LEAVE;
}

void LA_ShutDown()
{
	TFUNC_ENTER;

	TFUNC_LEAVE;
}      




		
	


void LA_InitGLResources()
{

	TFUNC_ENTER;
	LA_RegisterFont( "misc.tex.font16" );
	GC_LoadGraphCursor();
	TFUNC_LEAVE;
}

void LA_FreeGLResources()
{
	TFUNC_ENTER;
	r_font16 = NULL;
	TFUNC_LEAVE;
}
		


void LA_BeginDraw()
{
	TFUNC_ENTER;
	R_BeginDraw2d();
	R_Draw2dSetWindow( -2.0, -2.0, 2.0, 2.0 );
	R_Draw2dSetDrawMode( DRAW2D_DRAWMODE_ARGB );
	R_Draw2dSetColor3f( 1.0, 1.0, 1.0 );
	R_Draw2dSetGrid( 640, 480 );
	TFUNC_LEAVE;
}

void LA_EndDraw()
{
	TFUNC_ENTER;
	R_EndDraw2d();
	TFUNC_LEAVE;
}

void LA_DrawString( const char *string, int x, int y )
{
	fp_t	tx, ty, twidth, theight;

	TFUNC_ENTER;
	R_Draw2dSetDrawMode( DRAW2D_DRAWMODE_ARGB );
	R_Draw2dSetColor3f( 1.0, 1.0, 1.0 );
	R_Draw2dSetTexture( r_font16->texobj );
	
	for( ;; )
	{
		if( !*string )
			break;
		
		LA_FontTexel16( *string, &tx, &ty, &twidth, &theight );

		R_Draw2dTexRect( x, y, 16, 16, tx, ty, twidth, theight );
		x += 16;
		string++;
	}
	TFUNC_LEAVE;
}

void LA_DrawString80( const char *string, int x, int y )
{
	fp_t	tx, ty, twidth, theight;

	TFUNC_ENTER;
	R_Draw2dSetDrawMode( DRAW2D_DRAWMODE_ARGB );
	R_Draw2dSetColor3f( 1.0, 1.0, 1.0 );
	R_Draw2dSetTexture( r_font16->texobj );
	
	for( ;; )
	{
		if( !*string )
			break;
		
		LA_FontTexel16( *string, &tx, &ty, &twidth, &theight );

		R_Draw2dTexRect( x, y, 8, 16, tx, ty, twidth, theight );
		x += 8;
		string++;
	}
	TFUNC_LEAVE;
}

void LA_DrawStringCRT( const char *string, int x, int y )
{
	fp_t	tx, ty, twidth, theight;
	int	rx, ry;

	TFUNC_ENTER;
	R_Draw2dSetDrawMode( DRAW2D_DRAWMODE_ADD_RGB );
	R_Draw2dSetColor3f( 0.1, 0.6, 0.1 );
	R_Draw2dSetTexture( r_font16->texobj );
#if 0
	if( gc_renderframe%64 > 40 )
	{
		ry = y + (gc_renderframe&2 );
		rx = x;
	}
	else
	{	
		rx = x + (gc_renderframe&2 );
		ry = y;
	}
#endif

	ry = y; // + (gc_renderframe&2);
	rx = x;

	for( ;; )
	{
		if( !*string )
			break;

		LA_FontTexel16( toupper( *string ) + 128, &tx, &ty, &twidth, &theight );
		if( ( *string >= 97 ) && ( *string <= 122 ) )
			R_Draw2dTexRect( rx, ry, 16, 16 - 2, tx, ty, twidth, theight );
		else
			R_Draw2dTexRect( rx, ry, 16, 16, tx, ty, twidth, theight );

		rx += 16;
		string++;
	}
	TFUNC_LEAVE;
}

void LA_DrawString80CRT( const char *string, int x, int y )
{
	fp_t	tx, ty, twidth, theight;
	int	rx, ry;

	TFUNC_ENTER;
	R_Draw2dSetDrawMode( DRAW2D_DRAWMODE_ADD_RGB );
	R_Draw2dSetColor3f( 0.1, 0.6, 0.1 );
	R_Draw2dSetTexture( r_font16->texobj );
#if 0
	if( gc_renderframe%64 > 40 )
	{
		ry = y + (gc_renderframe&2 );
		rx = x;
	}
	else
	{	
		rx = x + (gc_renderframe&2 );
		ry = y;
	}
#endif

	ry = y; // + (gc_renderframe&2);
	rx = x;

	for( ;; )
	{
		if( !*string )
			break;

		LA_FontTexel16( toupper( *string ) + 128, &tx, &ty, &twidth, &theight );

		if( ( *string >= 97 ) && ( *string <= 122 ) )
			R_Draw2dTexRect( rx, ry, 8, 16 - 2, tx, ty, twidth, theight );
		else
			R_Draw2dTexRect( rx, ry, 8, 16, tx, ty, twidth, theight );
		rx += 8;
		string++;
	}
	TFUNC_LEAVE;
}


void LA_DrawChar( char c, int x, int y )
{
	fp_t	tx, ty, twidth, theight;
	TFUNC_ENTER;

	R_Draw2dSetDrawMode( DRAW2D_DRAWMODE_ARGB );
	R_Draw2dSetColor3f( 1.0, 1.0, 1.0 );
	LA_FontTexel16( c, &tx, &ty, &twidth, &theight );
	R_Draw2dSetTexture( r_font16->texobj );
	R_Draw2dTexRect( x, y, 16, 16, tx, ty, twidth, theight );
	TFUNC_LEAVE;
}

void LA_DrawCharCRT( char c, int x, int y )
{
	fp_t	tx, ty, twidth, theight;
	int	ry;
	
	TFUNC_ENTER;
	R_Draw2dSetDrawMode( DRAW2D_DRAWMODE_ADD_RGB );

	LA_FontTexel16( c + 128, &tx, &ty, &twidth, &theight );
	R_Draw2dSetTexture( r_font16->texobj );


	if( (gc_renderframe&16) > 12 )
		ry = y;// + (gc_renderframe&2 );
	else
		ry = y;
	
	R_Draw2dSetColor3f( 0.1, 0.6, 0.1 );
	R_Draw2dTexRect( x, ry, 16, 16, tx, ty, twidth, theight );
	TFUNC_LEAVE;
}

#if 0

int LA_TgaIsCached( char *filename )
{
	int		i;
	ca_tga_t	*tga;
	

   	TFUNC_ENTER;

	for( i = 0; i < la_cachelast; i++ )
	{
		if( !strcmp( filename, la_cache[i].name ))
		{
			TFUNC_LEAVE;		
			return i;
		}
	}

	if( la_cachelast >= LA_CACHESIZE )
		__error( "while loading %s: LA_CACHESIZE too small\n", filename );

	tga = ( ca_tga_t * )CA_Load( filename, CA_LoadTga );

	la_cache[la_cachelast].r_tga = R_Draw2dRegisterTga( tga, 0 );
	la_cache[la_cachelast].tga = tga;
	strncpy( la_cache[la_cachelast].name, filename, 64 );

	TFUNC_LEAVE;
	return la_cachelast++;
}

#endif

void LA_DrawGltexByGltex( res_gltex_cache_t *gltex, int x, int y, int width, int height, int mode )
{
	TFUNC_ENTER;
	R_Draw2dSetTexture( gltex->texobj );
	R_Draw2dSetDrawMode( mode );
	
	if( !width )
		width = gltex->width;
	
	if( !height )
		height = gltex->height;

	R_Draw2dTexRect( x, y, width, height, 0.0, 0.0, 1.0, 1.0 );
	TFUNC_LEAVE;
}

void LA_DrawGltexByName( const char *resname, int x, int y, int width, int height, int mode )
{
	g_resource_t	*r;
	res_gltex_cache_t	*gltex;
	
	TFUNC_ENTER;
	
	r = G_ResourceSearch( g_rs, resname );

	gltex = (res_gltex_cache_t *) r->res_cache;

	R_Draw2dSetTexture( gltex->texobj );
	R_Draw2dSetDrawMode( mode );
	if( !width )
		width = gltex->width;

	if( !height )
		height = gltex->height;

	R_Draw2dTexRect( x, y, width, height, 0.0, 0.0, 1.0, 1.0 );
	TFUNC_LEAVE;
}


hudc_text_t* LA_HUD_CTextInit( int cols, int rows, void (*drawFunc)( const char *, int, int) )
{

	//__chkptr( context );
	hudc_text_t* context;
	TFUNC_ENTER;

	context = ( hudc_text_t* ) MM_Malloc( sizeof( hudc_text_t ) );
	__chkptr( context );
	context->cols = cols;
	context->rows = rows;
	context->firstline = 0;
	context->lastline = 0;
	context->colpointer = -1;
	if( !drawFunc )
		context->drawString = LA_DrawString;
	else
		context->drawString = drawFunc;

	TFUNC_LEAVE;
	return context;
}

void LA_HUDPutChar( hudc_text_t* context, char character )
{

	if( character == 10 )
	{
		context->colpointer = -1;
	}
	if( context->colpointer == -1 )
	{
		if( context->lastline < context->rows )
			context->lastline++;
		else
			context->firstline++;
		if( context->firstline >= context->rows )
			context->firstline = 0;
		context->colpointer = 0;
		if( character == 10 )
		{
			context->textbuf[(context->firstline+context->lastline-1)%context->rows][0] = 0;

			return;
		}
	}
	if( context->colpointer >= context->cols )
	{

		return;
	}
	
	context->textbuf[(context->firstline+context->lastline-1)%context->rows][context->colpointer] = character;
	context->textbuf[(context->firstline+context->lastline-1)%context->rows][context->colpointer+1] = 0;
	context->colpointer++;
}

void LA_HUDPutLine( hudc_text_t* context, char* textline )
{
	TFUNC_ENTER;
	__chkptr( textline );
	strncpy( context->textbuf[(context->firstline+context->lastline)%context->rows], textline, 80 );
	if( context->lastline < context->rows )
		context->lastline++;
	else
		context->firstline++;
	if( context->firstline >= context->rows )
		context->firstline = 0;
	context->colpointer = -1;
	TFUNC_LEAVE;
}

void LA_HUDPutString( hudc_text_t* context, char* string )
{
	size_t i;
	size_t	len;
	
	len = strlen( string );

	for( i = 0; i < len; i++ )
		LA_HUDPutChar( context, string[i] );
}


void LA_HUD_CTextFlush(  hudc_text_t* context, int xoffs, int yoffs )
{
	int	y, i = 0;

	TFUNC_ENTER;
	y = context->firstline;
	__chkptr( context->drawString );

	for( i = 0; i < context->lastline; i++ )
	{	
//		LA_DrawString( context->textbuf[y], xoffs, 480 - ( i * 16 + yoffs ));
		context->drawString( context->textbuf[y], xoffs, (-1+context->lastline - i) * 16 + yoffs );
			       
		y++;
		if( y >= context->lastline )
			y = 0;
		
	}
	TFUNC_LEAVE;
}


void LA_InsertVarview( const char *varname, void (* callback )( int flag, char *string ))
{
	int	i;
	sh_var_t	*var;

	for( i = 0; i < LA_VARVIEWNUM; i++ )
	{
		if( !la_varviews[i].var )
			break;
	}

	if( i >= LA_VARVIEWNUM )
	{
		__warning( "no free varwiew\n" );
		return;
	}

	var = SHP_GetVar( varname );

	la_varviews[i].var = var;
	la_varviews[i].callback = callback;
}


void LA_ShowVarviews()
{
	int	i;
	int	i2 = 0;
	
	for( i = 0; i < LA_VARVIEWNUM; i++ )
	{
		char	string[256];

		if( !la_varviews[i].var )
			continue;
		
		if( !la_varviews[i].callback )
			continue;
		
		if( !la_varviews[i].var->ivalue )
			continue;


		
		la_varviews[i].callback( la_varviews[i].var->ivalue, string );


		LA_DrawString( string, 0, 416 - i2 * 16 );
		i2++;
	}
}
	       
