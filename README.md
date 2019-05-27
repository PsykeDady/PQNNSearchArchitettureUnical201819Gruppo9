# PQNNSearchArchitettureUnical201819Gruppo9

## Architetture e Programmazione dei Sistemi di Elaborazione, Progetto a.a. 2018/19:

**Traccia**: *Sviluppare in linguaggio assembly x86-32+SSE e x86-64+AVX l' algoritmo* ***“Product Quantization for Nearest Neighbor Search”***

**Professore**: *Angiulli Fabrizio*

**Candidati** del *Gruppo 9*:

* Davide Galati
* Gianpaolo Cascardo
* Maria Chiara Nicoletti

**TODO:**

* ~~creazione progetto su github~~
* ~~distanza euclidea -> dist(x,y) : sqrt((Sommatoria(xi,yi))^2)~~
* ~~mindist(int* x, int* dataset)~~
* ~~vq(codebook,Dataset): ad ogni elemento in dataset y applica il quantizzatore~~
* ~~obiettivo() :  Sommatoria(distanza(y,q(y))^2)~~
* ~~nuovi centroidi=nuovicentroidi(mappa)~~ 
* ~~product quantization :~~
  * ~~divide(): divide ogni vettore in m sottovettori di dimensione d*=d/m (m passato come parametro)~~
    ~~divide(v)=(v1,v2,v3....vm)~~
  * ~~pq(v) : applica la quantizzazione su ogni sottovettore e li concatena 
    q(divide(xv)= (q(v1),q(vm),...,q(vm))~~
* ~~k-means(eps) : tecnica da applicare con vq e pq, si prendono random k centroidi, si applica vq e si sostituisce ogni centroide come media della sua cella di voronoi si ferma dopo le due iterazioni successive ad un incremento impercettibile della funzione obiettivo (minore ad una soglia passata come parametro), il tutto deve essere fatto in un numero di tmin iterazioni minimo e massimo tmax~~

* ~~Aggiornamento mappa a indici (anzichè centroidi)~~

* ~~Test PQ e nuovicentroidi~~

* ~~TESTARE K-MEANS~~

* ~~ANN esaustiva~~
  * ~~distanza simmetrica o SDC(x,y): SQRT(Sommatoria(distanza(q(x),q(y))^2)), tenendo in memoria l'insieme di distanze tra ogni centroide~~
  * ~~distanza asimmetrita o ADC(x,y): SQRT(Sommatoria(distanza(x,q(y))^2)),    tenendo in memoria per ogni diversa x le distanze da ogni centroide~~
* ANN non esaustiva
  * ~~calcolo residui : r(y)=y-q(y)~~
  * ~~quantizzare con pq r(y)~~
  * ~~creiamo un vettore 'r(x)': con  'w' vicini di x ( e le loro distanze da x)~~
  * calcolare le distanze tra x e i punti y vicini ai w centroidi di x, salvare K risultati piu' piccoli
  
* sostituire in SDC la struttura distanze con una struttura dove la cella [ i , j , m ] accede alla distanza tra il centroide i, il centroide j e sottocentroide w. 
* unire il codice ai template dati da Angiulli (64-32bit), preparare i dati come matrice ANN 
