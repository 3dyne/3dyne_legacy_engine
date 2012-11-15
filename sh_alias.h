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



#ifndef __SHALIAS_H_INCLUDED
#define __SHALIAS_H_INCLUDED

void ALIAS_AddSource();
void ALIAS_SetVar();
void ALIAS_ShowVar();
void ALIAS_SetEnv( const char* var, const char* value );
void ALIAS_RunScript();
void ALIAS_SHToggle();
void ALIAS_Echo();
void ALIAS_R_Grab();
void ALIAS_MenuPage();
void ALIAS_PlaySound();
void ALIAS_Toggle();
void ALIAS_StopSound();
void ALIAS_MemInfo();

void ALIAS_C64Run();
void ALIAS_C64List();
void ALIAS_Dlevel_Start();
void ALIAS_LdbSet();
void ALIAS_Who();

void ALIAS_AddPlayer();
void ALIAS_AddDummy();
// gc_state async updates
void ALIAS_StartLocalGame();
void ALIAS_StartRemoteGame();
void ALIAS_StartSingle();
void ALIAS_StartDemo();
void ALIAS_DropGame();
void ALIAS_Quit();

void IALIAS_Toggle();
void IALIAS_Forward();
void IALIAS_Back();
void IALIAS_Left();
void IALIAS_Right();
void IALIAS_StrafeLeft();
void IALIAS_StrafeRight();
void IALIAS_Fire0();
void IALIAS_Use();
void IALIAS_Jump();
void IALIAS_Strafe();
void IALIAS_LookUp();
void IALIAS_LookDown();

#endif
