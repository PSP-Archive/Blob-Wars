TARGET = blob
PSPSDK=$(shell psp-config --pspsdk-path)
PSPBIN = $(PSPSDK)/../bin
SDL_CONFIG = $(PSPBIN)/sdl-config

PSP_EBOOT_ICON = ICON0.PNG
PSP_EBOOT_PIC1 = PIC1.PNG

PSP_FW_VERSION=371
BUILD_PRX = 1


OBJS += aquaBoss.o bosses.o bullets.o cutscene.o
OBJS += CAudio.o CBoss.o CCollision.o CCutscene.o CData.o CEffect.o CEngine.o CEntity.o CFileData.o CGame.o
OBJS += CGameData.o CGameObject.o CGraphics.o CHub.o CKeyboard.o CJoystick.o CLineDef.o CList.o
OBJS += CMap.o CMath.o CObjective.o CPak.o CParticle.o CPersistant.o CPersistData.o
OBJS += CRadarBlip.o CSpawnPoint.o CSprite.o CSwitch.o CTeleporter.o CTrain.o CTrap.o CWeapon.o CWidget.o
OBJS += droidBoss.o effects.o enemies.o entities.o explosions.o finalBattle.o galdov.o game.o graphics.o
OBJS += hub.o info.o init.o intro.o items.o lineDefs.o loadSave.o map.o mapData.o mias.o
OBJS += mission.o objectives.o obstacles.o options.o particles.o player.o resources.o spawnPoints.o
OBJS += switches.o tankBoss.o teleporters.o title.o trains.o traps.o triggers.o weapons.o widgets.o main.o


INCDIR =
CFLAGS = -G0 -O2  -Wall -g $(shell $(SDL_CONFIG) --cflags) -DVER=1.04 -DRELEASE=1


CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti  -fsingle-precision-constant -mno-check-zero-division -ffast-math
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS = -lstdc++ -lSDL_mixer  -lvorbisidec -lSDL_image  -lpng -lz -ljpeg  -lSDL_ttf -lfreetype $(shell $(SDL_CONFIG) --libs) -lsdlmain    -lpsprtc  -lpspvfpu -lpsppower


EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = blobs

include $(PSPSDK)/lib/build.mak
