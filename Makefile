# File che definisce compilatore, linker, flags e vpath
include makesettings

# dir dove verranno messi i file oggetto
BUILD_DIR = build


# Lista dei file oggetto da creare partendo dai .c di phase1 e proj_lib, per aggiungerne di nuovi basta aggiungere:
# $(BUILD_DIR)/nome_del_file.o (ogni file .c dovrebbe avere un corrispettivo .o in una delle liste)

PROJ_LIB_OBJS := $(BUILD_DIR)/proj_lib.o
PHASE1_OBJS := $(BUILD_DIR)/ProcQ.o

UMPS_LIB_OBJS = $(BUILD_DIR)/crtso.o $(BUILD_DIR)/libumps.o

# Esiste un modo per prendere tutti i nomi dei file .c, attaccarci davanti $(BUILD_DIR)/ e 
# rimpiazzare il .c con un .o automaticamente? Assolutamente si (ma ora non ne ho voglia so //TODO)

.PHONY: all clean

all : kernel.core.umps

kernel.core.umps : kernel

	umps3-elf2umps -k $<

kernel : build/main.o $(PROJ_LIB_OBJS) $(UMPS_LIB_OBJS) $(PHASE1_OBJS)

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
