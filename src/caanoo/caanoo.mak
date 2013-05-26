CPUDEFS += -DHAS_CYCLONE=1 -DHAS_DRZ80=1
OBJDIRS += $(OBJ)/cpu/m68000_cyclone $(OBJ)/cpu/z80_drz80
CPUOBJS += $(OBJ)/cpu/m68000_cyclone/cyclone.o $(OBJ)/cpu/m68000_cyclone/c68000.o
CPUOBJS += $(OBJ)/cpu/z80_drz80/drz80.o $(OBJ)/cpu/z80_drz80/drz80_z80.o

#OBJDIRS += $(OBJ)/cpu/nec_armnec
#CPUDEFS += -DHAS_ARMNEC
#CPUOBJS += $(OBJ)/cpu/nec_armnec/armV30.o $(OBJ)/cpu/nec_armnec/armV33.o $(OBJ)/cpu/nec_armnec/armnecintrf.o

OSOBJS = $(OBJ)/caanoo/te9_tf9_hybrid_driver.o $(OBJ)/caanoo/wiz_lib.o $(OBJ)/caanoo/uppermem.o \
    $(OBJ)/caanoo/warm.o $(OBJ)/caanoo/pollux_set.o $(OBJ)/caanoo/sys_cacheflush.o \
	$(OBJ)/caanoo/memcmp.o $(OBJ)/caanoo/memcpy.o $(OBJ)/caanoo/memset.o \
	$(OBJ)/caanoo/strcmp.o $(OBJ)/caanoo/strlen.o $(OBJ)/caanoo/strncmp.o \
	$(OBJ)/caanoo/wiz.o $(OBJ)/caanoo/video.o $(OBJ)/caanoo/blit.o \
	$(OBJ)/caanoo/sound.o $(OBJ)/caanoo/input.o $(OBJ)/caanoo/fileio.o \
	$(OBJ)/caanoo/config.o $(OBJ)/caanoo/fronthlp.o \
	$(OBJ)/caanoo/usbjoy.o $(OBJ)/caanoo/usbjoy_mame.o

mame.gpe:
	$(LD) $(LDFLAGS) \
		src/caanoo/te9_tf9_hybrid_driver.c src/caanoo/wiz_lib.cpp src/caanoo/uppermem.cpp src/caanoo/pollux_set.cpp src/caanoo/sys_cacheflush.S \
		src/caanoo/wiz_frontend.cpp src/caanoo/usbjoy.cpp src/caanoo/usbjoy_mame.cpp $(LIBS) -o $@
