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



// lib_hobj.h

#ifndef __lib_hobj
#define __lib_hobj

#include <stdio.h>                                                              
#include <stdlib.h>                                                             
#include <math.h>                                                               
                                                                                
#include "u_defs.h"
                                                                      
#include "lib_math.h"
#include "lib_hobjdefs.h"
#include "lib_hmanagerdefs.h"

hobj_t* NewClass( const char *type, const char *name );
void FreeClass( hobj_t *obj );                                                                              
hpair_t* NewHPair( void );                                                                                  
hpair_t* NewHPair2( const char *type, const char *key, const char *value );
hpair_t* CopyHPair( hpair_t *pair );
//hpair_t* NewHPairBstring( char *key, char *bytes );                                                       
void FreeHPair( hpair_t *pair );                                                                            
                                                                                                            
void DumpClass( hobj_t *self );                                                                             
void DumpHPair( hpair_t *self );                                                                            
void DeepDumpClass( hobj_t *self );                                                                         
    
void DeepDestroyClass( hobj_t *obj );
void DestroyClass( hobj_t *obj );
                                                                                                        
void SetClassExtra( hobj_t *self, void *ptr );                                                              
void * GetClassExtra( hobj_t *self );                                                                       
                                                                                                            
void InsertHPair( hobj_t *self, hpair_t *pair );                                                            
bool_t RemoveHPair( hobj_t *self, hpair_t *pair );                                                          

void InsertClass( hobj_t *self, hobj_t *obj );                                                              
bool_t RemoveClass( hobj_t *self, hobj_t *obj );                                                            
bool_t RemoveClass2( hobj_t *obj ); // now objects know their parents                                       
                                                                                                            
void RemoveAndDestroyAllHPairsOfKey( hobj_t *self, char *key );                                             
                                                                                                            
hpair_t* FindHPair( hobj_t *self, const char *key );                                                              
hobj_t* FindClass( hobj_t *self, const char *name );                                                              
hobj_t* FindClassType( hobj_t *self, const char *type );                                                          
                                                                                                            
void InitClassSearchIterator( hobj_search_iterator_t *iter, hobj_t *obj, const char *type );                      
hobj_t* SearchGetNextClass( hobj_search_iterator_t *iter );                                                 
                                                                                                            
void InitHPairSearchIterator( hpair_search_iterator_t *iter, hobj_t *obj, const char *key );                      
hpair_t* SearchGetNextHPair( hpair_search_iterator_t *iter );                                               

void ClassTraceToRoot( hobj_t *obj );
/*
  ==================================================
  hpair cast stuff

  ==================================================
*/
void HPairCastToVec3d( vec3d_t v, hpair_t *pair );
void HPairCastToVec2d( vec2d_t v, hpair_t *pair );
void HPairCastToFloat( fp_t *f, hpair_t *pair );
void HPairCastToInt( int *i, hpair_t *pair );
void HPairCastToString( char *t, hpair_t *pair );

// type safe casts
void HPairCastToVec3d_safe( vec3d_t v, hpair_t *pair );
void HPairCastToVec2d_safe( vec2d_t v, hpair_t *pair );
void HPairCastToFloat_safe( fp_t *f, hpair_t *pair );
void HPairCastToInt_safe( int *i, hpair_t *pair );
void HPairCastToString_safe( char *t, hpair_t *pair );
void HPairCastToClsref_safe( char *t, hpair_t *pair );

void HPairCastToBstring_safe( void *ptr, int *max_buf_size, hpair_t *pair );
int  HPairGetBstringLength( hpair_t *pair );

void BstringCastToHPair( void *ptr, int size, hpair_t *pair );

void HPairCastFromVec3d( vec3d_t v, hpair_t *pair );

// easy find and cast
hpair_t * EasyFindVec3d( vec3d_t v, hobj_t *obj, const char *key );
hpair_t * EasyFindVec2d( vec2d_t v, hobj_t *obj, const char *key );
hpair_t * EasyFindFloat( fp_t *f, hobj_t *obj, const char *key );
hpair_t * EasyFindInt( int *i, hobj_t *obj, const char *key );
hpair_t * EasyFindString( char *t, hobj_t *obj, const char *key );

unique_t StringToUnique( char *text );
hpair_t * EasyFindClsrefAsUnique( unique_t *uni, hobj_t *obj, const char *key );

hobj_t * EasyLookupClsref( hobj_t *obj, const char *key, hmanager_t *hm );

// easy new and insert
void EasyNewVec3d( hobj_t *obj, const char *key, vec3d_t v );
void EasyNewVec2d( hobj_t *obj, const char *key, vec2d_t v );
void EasyNewFloat( hobj_t *obj, const char *key, fp_t f );
void EasyNewInt( hobj_t *obj, const char *key, int i );
void EasyNewString( hobj_t *obj, const char *key, const char *text );

void EasyNewClsref( hobj_t *obj, const char *key, hobj_t *referenced_obj );

hobj_t * EasyNewClassFromID( const char *type, unique_t id );

// easy compare
bool_t EasyCompareValue( hobj_t *obj, const char *key, const char *cmp_value );

#endif
