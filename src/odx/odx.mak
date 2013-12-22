CPUDEFS += -DHAS_FAME=1
#CPUDEFS +=-DUSE_FAME_C_CORE=1 -DFAME_IRQ_CLOCKING -DFAME_DIRECT_MAPPING -DFAME_GLOBAL_CONTEXT -DFAME_INLINE_LOOP -DFAME_FETCHBITS=12 -DFAME_DATABITS=12 -DFAME_GOTOS 
OBJDIRS += $(OBJ)/cpu/fame
CPUOBJS += $(OBJ)/cpu/fame/f68000.o $(OBJ)/cpu/fame/famec.o

OSOBJS = $(OBJ)/odx/fastmem.o $(OBJ)/odx/minimal.o \
	$(OBJ)/odx/odx.o $(OBJ)/odx/video.o $(OBJ)/odx/blit.o \
	$(OBJ)/odx/sound.o $(OBJ)/odx/input.o $(OBJ)/odx/fileio.o \
	$(OBJ)/odx/config.o $(OBJ)/odx/fronthlp.o

FEOBJS = $(OBJ)/odx/fastmem.o $(OBJ)/odx/minimal.o $(OBJ)/odx/odx_frontend.o

