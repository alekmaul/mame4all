CPUDEFS += -DHAS_CYCLONE=1 -DHAS_DRZ80=1
OBJDIRS += $(OBJ)/cpu/m68000_cyclone $(OBJ)/cpu/z80_drz80
CPUOBJS += $(OBJ)/cpu/m68000_cyclone/cyclone.o $(OBJ)/cpu/m68000_cyclone/c68000.o
CPUOBJS += $(OBJ)/cpu/z80_drz80/drz80.o $(OBJ)/cpu/z80_drz80/drz80_z80.o

#OBJDIRS += $(OBJ)/cpu/nec_armnec
#CPUDEFS += -DHAS_ARMNEC
#CPUOBJS += $(OBJ)/cpu/nec_armnec/armV30.o $(OBJ)/cpu/nec_armnec/armV33.o $(OBJ)/cpu/nec_armnec/armnecintrf.o

OSOBJS = $(OBJ)/pandora/minimal.o \
	$(OBJ)/pandora/pandora.o $(OBJ)/pandora/video.o $(OBJ)/pandora/blit.o \
	$(OBJ)/pandora/sound.o $(OBJ)/pandora/input.o $(OBJ)/pandora/fileio.o \
	$(OBJ)/pandora/config.o $(OBJ)/pandora/fronthlp.o \
	$(OBJ)/pandora/memcmp.o $(OBJ)/pandora/memcpy.o $(OBJ)/pandora/memset.o \
	$(OBJ)/pandora/strcmp.o $(OBJ)/pandora/strlen.o $(OBJ)/pandora/strncmp.o \
	$(OBJ)/pandora/usbjoy_mame.o $(OBJ)/pandora/usbjoy.o

FEOBJS = $(OBJ)/pandora/pnd_frontend.o $(OBJ)/pandora/minimal.o \
	$(OBJ)/pandora/usbjoy.o $(OBJ)/pandora/usbjoy_mame.o
