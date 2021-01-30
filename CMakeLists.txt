
cmake_minimum_required(VERSION 3.18)
project(PandOS VERSION 0.1.0 LANGUAGES C ASM)

# Directories del progetto
SET(SRC phase1)
SET(INC proj_lib)

# Da me umps e' installato in /usr/, potrebbe anche essere in
# /usr/local/, in caso cambiate a mano queste tre variabili,
# e' possibile fare in modo che lo cerchi lui, ma non ho
# idea di come si fa.
SET(UMPS3_DIR_PREFIX /usr)
SET(UMPS3_DATA_DIR ${UMPS3_DIR_PREFIX}/share/umps3)
SET(UMPS3_INCLUDE_DIR ${UMPS3_DIR_PREFIX}/include/umps3)

# Info per il compilatore
SET(CMAKE_C_STANDARD 90)
SET(CFLAGS_LANG -ffreestanding -ansi -Wall)

add_compile_options(${CFLAGS_LANG})

# Dir delle librerie di umps, volendo potremmo anche copiare i file
# e inserirli in proj_lib, ditemi un po' voi
include_directories(${UMPS3_DATA_DIR})
include_directories(${UMPS3_INCLUDE_DIR})

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR mips)

# Setta compiler e linker
set(CMAKE_C_COMPILER mipsel-linux-gnu-gcc)
set(CMAKE_ASM_COMPILER mipsel-linux-gnu-gcc) # Compiler per assembly
set(CMAKE_C_LINKER mipsel-linux-gnu-ld)

set(CMAKE_C_COMPILER_WORKS 1)

# Altre flag per il compilatore
SET(
	CFLAGS_UMPS
	-ffreestanding -ansi -mips1 -mabi=32 -mno-gpopt -G 0
	-mfp32 -Wall -O0
)

add_compile_options(${CFLAGS_UMPS})

# Script e flag da usare nel linking
SET(LINK_SCRIPT ${UMPS3_DATA_DIR}/umpscore.ldscript)
SET(LD_FLAGS "-nostdlib -Wl,-G,0,-nostdlib,-T,${LINK_SCRIPT}")

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LD_FLAGS}")


# Tutti i file (di libreria o scritti da noi) sono compilati come librerie statiche
# che verranno linkate nell'eseguibile (per ora e' main.c ma quando avremo completato)
# tutto sara' testers/p1test.c)

# File di Libreria di UMPs3, se vi capita di includere file di umps nuovi (tipo umps/arch.h),
# aggiungeteli qui, ma almeno in questa fase non credo ci sara' bisogno
SET(UMPS3_LIB ${UMPS3_INCLUDE_DIR}/umps/libumps.h ${UMPS3_DATA_DIR}/crtso.S ${UMPS3_DATA_DIR}/libumps.S)

add_library(umps3_lib ${UMPS3_LIB})


# Libreria contenente i file in proj_lib/, se ne create di nuovi aggiungeteli qui 
SET(PROJ_LIB ${INC}/pandos_const.h ${INC}/pandos_types.h ${INC}/proj_lib.h ${INC}/proj_lib.c)
add_library(project_lib ${PROJ_LIB})


# Per ora non c'e' nessuna libreria statica relativa a phase1/
# qundo ci metteremo roba andra' creata analogamente alle due precedenti
# (usando ${SRC}/ come prefisso)

# Prima creazione del kernel, se avete creato nuove librerie aggiungetele nella seconda riga,
# se volete usare un file main diverso cambiate il nome nella prima
add_executable(kernel main.c)
target_link_libraries(kernel umps3_lib project_lib)

# Si deve far andare umps3-elf2umps per generare i file .umps
add_custom_target(
	kernel.core.umps ALL
	COMMAND umps3-elf2umps -k kernel
	BYPRODUCTS kernel.core.umps kernel.stab.umps
	DEPENDS kernel
)

# Per compilare il tutto create una cartella chiamata build (nome arbitrario) allo stesso livello del CMakeLists,
# cd nella cartella
#
# cmake .. per inizializzarci dentro l'ambiente CMake
#
# cmake --build . per compilare
# 
# Una volta fatto cio dentro alla cartella dovrebbero esserci tre nuovi file:
# kernel, kernel.core.umps, kernel.stab.umps
# a questo punto basta lanciare l'emulatore, fargli creare il file config, accederlo, aprire il terminale0 e fare play.
#
# Ora come ora dovrebbe stampare Hello World.
#
# Mi raccomando non pushate mai la cartella build

