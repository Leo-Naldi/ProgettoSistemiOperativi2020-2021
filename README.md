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

Questo produrrà nella cartella bin i file seguenti: kernel, kernel.core.umps, kernel.stab.umps e la cartella build dove finiranno tutti file compilati \*.o, 
e la cartella deps dove finranno i file \*.d. A questo punto si deve creare un file .json che contiene le configurazioni. 
In fine si apre il umps3 col commando:

```bash
$	umps3
```

Si carica il file .json e si preme *Power On*, si apre il terminale 0 se preme *Continue* per vedere l'output.
Umps3 di default cerca i file .core e .stab nella stessa cartella del .json, quindi se questo non si trova in bin occorre settare nell'emulatore i percorsi corretti ai due file. Inoltre occorre
settare il campo TLB floor address a 0x80000000.

## Note

Il progetto è stato testato su Debian GNU/Linux 9 ed Ubuntu 20.04.2 LTS.

## Issues

Al momento l'esecuzione di p2test entra nel caso default di p5gen, che stampera' il messaggio "other program trap", e carichera' direttamente l'exception state.
Il program counter di questo exception state sara' l'indirizzo dell'istruzione che ha causato l'eccezione, e da qui si va in loop. Il test di per se viene completato
(il processo in loop verra' messo in attesa prima o poi), ma una volta terminati i gli altri processi il test continua a stampare "other program trap" all'infinito.
Un'hack per "evitare" questo loop e' aggiungere una chiamata SYSCALL(TERMINATETHREAD, 0, 0, 0) nella sezione default di p5gen.
Le possibili ragioni di questo comportamento possono essere: o rimane un dangling pointer a p5 che causa in qualche modo un'eccezione non prevista,
oppure un'eccezione non viene letta correttamente, oppure in qualche modo p5 non viene terminato.

## Bug Fixes
Risolto il loop di other program trap (c'era un bug in outChild). Ora tutte le funzioni di asl.h che sbloccano un processo settano il psemadd a NULL.
