# PandOS - ProgettoSistemiOperativi2020-2021

**PandOS** è Evoluzione di Kaya O.S., a sua volta evoluzione di una lunga lista di sistemi operativi proposti a scopo didattico(HOCA, TINA, ICARO, etc). La sua arctettura è basata su sei livelli di astrazione, sul modello del sistema operativo THE proposto da Dijkstra in un suo articolo del 1968.

## Prerequisiti

È necessario avere installato [umps3](https://github.com/virtualsquare/umps3). I makefile suppongono che i file siano installati in /usr o /usr/local, ma può succedere che sono installati in qualche altro posto. In tal caso è necessario settare a mano la variabile UMPS\_PREFIX del makesettings dove è indicato il prefisso dei file umps3. Si presuppone anche che il cross-compiler e il linker siano in path, se cosi' non fosse e' necessario aggiungere il path assoluto nella variabile CROSS\_TOOLS\_PREF, sempre in makesettings.
Il Makefile mette anche a disposizione i target clean, che rimuove i file .o, kernel e .umps, e help, che stampa informazioni sull'utilizzo del makefile.

## Installazione

Per far girare il progetto si deve solamente aprire la repo dal terminale e fare:

```bash
$	make
```

Questo produrrà nella cartella i file seguenti: kernel, kernel.core.umps, kernel.stab.umps e la cartella build dove finiranno tutti file compilati *.o e *.d. A questo punto si deve creare un file .json che contiene le configurazioni(questo si fa facilmente da umps3). In fine si apre il umps3 col commando:

```bash
$	umps3
```

Si carica il file .json e si preme *Power On*, si apre il terminale 0 se preme *Continue* per vedere l'output.

## Note

Il progetto è stato testato su Debian GNU/Linux 9 ed Ubuntu 20.04.2 LTS.