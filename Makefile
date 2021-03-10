# File che definisce compilatore, linker, flags e vpath
include makesettings

# dir dove verranno messi i file oggetto e i file .d
BUILD_DIR := ./build
DEPS_DIR := ./deps
BIN_DIR := ./bin

PROJ_LIB_SRC := $(notdir $(wildcard $(PROJ_LIB_PATH)/*.c))
PHASE1_SRC := $(notdir $(wildcard $(PHASE1_PATH)/*.c))
PHASE2_SRC := $(notdir $(wildcard $(PHASE2_PATH)/*.c)) 

PROJECT_SRC := $(PROJ_LIB_SRC) $(PHASE1_SRC) $(PHASE2_SRC)

PROJECT_OBJ := $(patsubst %.c, $(BUILD_DIR)/%.o, $(PROJECT_SRC)) 
UMPS_LIB_OBJ := $(BUILD_DIR)/crtso.o $(BUILD_DIR)/libumps.o

TARGET_TEST := p1test

DEPS = $(patsubst %.c, $(DEPS_DIR)/%.d, $(PROJECT_SRC)) $(DEPS_DIR)/$(TARGET_TEST).d
 

$(shell mkdir -p $(BUILD_DIR))
$(shell mkdir -p $(DEPS_DIR))
$(shell mkdir -p $(BIN_DIR))

.PHONY: all clean help

all : $(BIN_DIR)/kernel.core.umps

include $(DEPS)

$(BIN_DIR)/kernel.core.umps : $(BIN_DIR)/kernel 
	umps3-elf2umps -k $<

$(BIN_DIR)/kernel : $(BUILD_DIR)/$(TARGET_TEST).o $(PROJECT_OBJ) $(UMPS_LIB_OBJ)
	
	$(LD) -o $@ $^ $(LINK_FLAGS)


# Pattern rule per i file oggetto
$(BUILD_DIR)/%.o : %.c 

	$(CC) $(CFLAGS) -c -o $@ $<


# Pattern rule per compilare i file assembler
$(BUILD_DIR)/%.o : %.S

	$(CC) $(CFLAGS) -c -o $@ $<

# Pattern rule per i requirement tracking
$(DEPS_DIR)/%.d: %.c

	@$(CC) -M $(INC) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\$(BUILD_DIR)/\1.o $@ : ,g' < $@.$$$$ > $@;\
	rm -f $@.$$$$

clean:

	-@rm -f $(BUILD_DIR)/*.o
	-@rm -f  $(BIN_DIR)/*

help: 
	-@echo
	-@echo Comandi disponibili:
	-@echo
	-@echo make \(all\): costrusce kernel, kernel.stab.umps e kernel.core.umps
	-@echo make kernel: costrusce kernel ma non i file .umps \(non viene eseguito umps3-elf2umps\)
	-@echo make clean: rimuove i file .o .umps e kernel \(ma non i file .d\)
	-@echo 
	-@echo Note\:
	-@echo
	-@echo I file oggetto \(.o\) sono contenuti nella directory $(BUILD_DIR), i file .d nella cartella $(DEPS_DIR).\
		Entrambe vengono generate automaticamente.
	-@echo
	-@echo Se i file di umps non sono ne in /usr ne in /usr/local, occorre settare a mano la variabile \
		UMPS_PREFIX nel file makesettings 
	-@echo Se il cross-compiler \(mipsel-linux-gnu-gcc\) e il linker non sono in path occorre settare la variabile \
		CROSS_TOOLS_PREF in makesettings.
	-@echo
	-@echo Le dependencies dei file vengono tracciate nei file .d, contenuti in $(DEPS_DIR), che vengono aggiornati \
		ad ogni compilazione \(o creati se non presenti\).
	-@echo

