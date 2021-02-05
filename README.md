# ProgettoSistemiOperativi2020-2021

# N.B. questo readme al momento serve solo per aiutare in caso di problemi nell'inizializzazione
# del emulatore (che verranno presto automatizzati dal make)

Una volta Scaricato ed installato UMPS3 creare una cartella per contenere il progetto

############################### Parte di git #########################################

Creare una cartella dove verrà clonato il progetto e apriteci un terminale

Prendere l'url da git nella repo 

fare il comando git clone <url>

--------------------------------------------------------------------------------------

############################## Crere le cartelle importanti ##########################

come prima cosa notare che se si porvasse a fare *make* nella cartella appena clonata 
la compilazione fallirebbe ed è per questo che bisogna prendere qualche accorgimento

1) aggiungere una cartella nominata "build" //dove finiranno tutti file compilati .o e .d

2) creare una file .json dove verranno registrate le configurazioni 

3) in fine se comoilando il file main.c da problemi probabilmente abbiamo un main vecchio

	3.1) quindi bisong includere "proj_lib/proj_lib.h" 
	
	3.2) rimuovere sayHello() ed in caso si voglia avere un output 
	     usare "term_puts("Hello\n");"
-------------------------------------------------------------------------------------
