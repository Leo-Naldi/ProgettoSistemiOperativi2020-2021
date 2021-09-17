# PandOS - ProgettoSistemiOperativi2020-2021

**PandOS** è Evoluzione di Kaya O.S., a sua volta evoluzione di una lunga lista di sistemi operativi proposti a scopo didattico(HOCA, TINA, ICARO, etc). La sua arctettura è basata su sei livelli di astrazione, sul modello del sistema operativo THE proposto da Dijkstra in un suo articolo del 1968.

## Prerequisiti

È necessario avere installato [umps3](https://github.com/virtualsquare/umps3). I makefile suppongono che i file siano installati in /usr o /usr/local, ma può succedere che sono installati in qualche altro posto. In tal caso è necessario settare a mano la variabile UMPS\_PREFIX del makesettings dove è indicato il prefisso dei file umps3. Si presuppone anche che il cross-compiler e il linker siano in path, se cosi' non fosse e' necessario aggiungere il path assoluto nella variabile CROSS\_TOOLS\_PREF, sempre in makesettings.
Il Makefile mette anche a disposizione i target clean, che rimuove i file .o, kernel e .umps, e help, che stampa informazioni sull'utilizzo del makefile.

## Installazione e Esecuzione

Per far girare il progetto si deve solamente aprire la repo dal terminale e fare:

```bash
$	make
```

Questo produrrà nella cartella bin i file seguenti: kernel, kernel.core.umps, kernel.stab.umps, la cartella build dove finiranno tutti file compilati \*.o, 
e la cartella deps dove finranno i file \*.d. A questo punto si deve creare il file p3config.json che contiene le configurazioni. 
Con questo comando verranno anche compilati i test e creati i device per la fase 3.

E' anche possibile compilare con il comando 
```bash
$   make debug
```

Che aggiungera' la macro \_\_PANDOS\_DEBUGGER\_ACTIVE\_\_, che aggiungera' controlli e variabili (i cui valori sono visualizzabili dal debugger di umps3) utili 
per debuggare, inoltre con questa macro molte delle situazioni anomale (p.e. errori io sui flash) che non dovrebbero verificarsi nella fase 3 verranno risolte
con una PANIC() invece che con una SYSCALL(TERMINATE, 0, 0, 0), sempre per facilitare il debugging. Questa modalita' ricompilera' sempre tutto da capo (inclusi
i test della fase 3)

In fine si apre il umps3 col commando:

```bash
$	umps3
```

Si carica il file p3config.json e si preme *Power On*, si apre il terminale 0 se preme *Continue* per vedere l'output.
Occorre settare il campo TLB floor address a 0x80000000.

## Note

Il progetto è stato testato su Debian GNU/Linux 9 ed Ubuntu 20.04.2 LTS. 

Occorre fare una make clean prima di passare dal compilare con "make debug" a compilare con "make" per eliminare veramente la macro.

La syscall 13 (READTERMINAL) legge anche il carattere di newline \n, che ci e' sembrata l'implementazione voluta leggendo il test strCat,
se cosi' non dovesse essere e' sufficiente rimpiazzare il do while con un while.

## Bug Fixes

Risolto il bug della fase 2 (era un errore in outChild ereditato dalla fase 1).
