# File che definisce compilatore, linker, flags e vpath
include makesettings

# dir dove verranno messi i file oggetto e i file .d
BUILD_DIR = ./build

# Lista dei source file di src
PROJ_LIB_SRC := $(notdir $(wildcard proj_lib/*.c))

# Lista dei source file in phase1
PHASE1_SRC := $(notdir $(wildcard phase1/*.c))

# Sostituisce i .c con .o nei file sorgenti e aggiunge il prefisso $(BUILD_DIR)/ 
# per generare automaticamente i nomi dei file oggetto
PROJ_LIB_OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(PROJ_LIB_SRC))
PHASE1_OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(PHASE1_SRC))

# Prima o poi automatizzero' anche questo i swear
UMPS_LIB_OBJS = $(BUILD_DIR)/crtso.o $(BUILD_DIR)/libumps.o

# Quando sara' tutto implementato andra' cambiato col nome del test
TARGET_TEST = p1test

DEPS = $(PROJ_LIB_SRC:.c=.d) $(PHASE1_SRC:.c=.d) $(BUILD_DIR)/$(TARGET_TEST).d

$(shell mkdir -p $(BUILD_DIR))

.PHONY: all clean

all : kernel.core.umps | $(BUILD_DIR)

-include $(patsubst %, $(BUILD_DIR)/%, $(DEPS))

kernel.core.umps : kernel 
	umps3-elf2umps -k $<

kernel : $(BUILD_DIR)/$(TARGET_TEST).o $(PROJ_LIB_OBJS) $(UMPS_LIB_OBJS) $(PHASE1_OBJS)
	
	$(LD) -o $@ $^ $(LINK_FLAGS)


# Pattern rule per i file oggetto
$(BUILD_DIR)/%.o : %.c 

	$(CC) $(CFLAGS) $(OUTPUT_OPTION) -c -o $@ $<


# Pattern rule per compilare i file lassembler
$(BUILD_DIR)/%.o : %.S

	$(CC) $(CFLAGS) -c -o $@ $<

clean:

	-@rm -f $(BUILD_DIR)/*.o
	-@rm -f  kernel
	-@rm -f  kernel.*.umps

help: 
	-@echo make \(all\): costrusce kernel, kernel.stab.umps e kernel.core.umps
	-@echo make kernel: costrusce kernel ma non i file .umps \(non viene eseguito umps3-elf2umps\)
	-@echo make clean: rimuove i file .o .umps e kernel
	-@echo 
	-@echo I file intermedi \(.o\) sono contenuti nella directory $(BUILD_DIR), insieme ai file .d
	-@echo
	-@echo se i file di umps non sono ne in /usr ne in /usr/local, occorre settare a mano la variabile \
		UMPS_PREFIX nel file makesettings 
	-@echo Se il cross-compiler \(mipsel-linux-gnu-gcc\) e il linker non sono in path occorre settare le variabili \
		corrispondenti in makesettings



# Al termine della compilazione i file .o saranno nella cartella build, mentre i file kernel e .umps saranno nella root del progetto
