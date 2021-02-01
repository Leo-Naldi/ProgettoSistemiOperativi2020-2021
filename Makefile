# File che definisce compilatore, linker, flags e vpath
include makesettings

# dir dove verranno messi i file oggetto
BUILD_DIR = build

# Lista dei source file di src
PROJ_LIB_SRC := $(notdir $(wildcard proj_lib/*.c))

# Lista dei source file in phase1
PHASE1_SRC := $(notdir $(wildcard phase1/*.c))

# Queste righe stampano i contenuti di PROJ_LIB_SRC e PHSE1_SRC se decommentate
# $(info proj lib src is $(PROJ_LIB_SRC))
# $(info phase1 src is $(PHASE1_SRC))

# Sostituisce i .c con .o nei file sorgenti e aggiunge il prefisso $(BUILD_DIR)/ 
# per generare automaticamente i nomi dei file oggetto
PROJ_LIB_OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(PROJ_LIB_SRC))
PHASE1_OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(PHASE1_SRC))

# $(info lib obj $(PROJ_LIB_OBJS))
# $(info phase 1 obj $(PHASE1_OBJS))

# Prima o poi automatizzero' anche questo i swear
UMPS_LIB_OBJS = $(BUILD_DIR)/crtso.o $(BUILD_DIR)/libumps.o


.PHONY: all clean

all : kernel.core.umps

kernel.core.umps : kernel

	umps3-elf2umps -k $<

kernel : $(BUILD_DIR)/main.o $(PROJ_LIB_OBJS) $(UMPS_LIB_OBJS) $(PHASE1_OBJS)

	$(LD) -o $@ $^ $(LINK_FLAGS)


# Pattern rule per i file oggetto
$(BUILD_DIR)/%.o : %.c 

	$(CC) $(CFLAGS) -c -o $@ $<


# Pattern rule per compilare i file lassembler
$(BUILD_DIR)/%.o : %.S

	$(CC) $(CFLAGS) -c -o $@ $<


clean:

	rm $(BUILD_DIR)/*.o
	rm kernel
	rm kernel.*.umps


# Al termine della compilazione i file .o saranno nella cartella build, mentre i file kernel e .umps saranno nella root del progetto
