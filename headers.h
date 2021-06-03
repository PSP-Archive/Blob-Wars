/*
Copyright (C) 2004 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <zlib.h>

//#ifndef SDL_GetGamma
//extern DECLSPEC int SDLCALL SDL_GetGamma(float *red, float *green, float *blue);
//#endif
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include "defs.h"

#include "CMath.h"

#include "CGameObject.h"
#include "CList.h"

#include "CSprite.h"
#include "CData.h"
#include "CParticle.h"
#include "CWeapon.h"
#include "CEntity.h"
#include "CBoss.h"
#include "CTrain.h"
#include "CSpawnPoint.h"
#include "CSwitch.h"
#include "CEffect.h"
#include "CObjective.h"
#include "CTeleporter.h"
#include "CLineDef.h"
#include "CTrap.h"

#include "CPersistData.h"
#include "CPersistant.h"

#include "CMap.h"

#include "CCollision.h"

#include "CWidget.h"

#include "CFileData.h"
#include "CPak.h"

#include "CJoystick.h"
#include "CKeyboard.h"

#include "CEngine.h"
#include "CGraphics.h"
#include "CAudio.h"

#include "CGame.h"
#include "CGameData.h"

#include "CHub.h"
#include "CRadarBlip.h"

#include "CCutscene.h"
