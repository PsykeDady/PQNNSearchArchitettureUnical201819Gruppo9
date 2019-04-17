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
* obiettivo() :  Sommatoria(distanza(y,q(y))^2)
* k-means : tecnica da applicare con vq, si prendono random k centroidi, si applica vq e si sostituisce ogni centroide come media della sua cella di voronoi **(? media calcolata come ?)** si ferma dopo le due iterazioni successive ad un incremento impercettibile della funzione obiettivo (minore ad una soglia passata come parametro)
* product quantization : 
  * divide(): divide ogni vettore in m sottovettori di dimensione d*=d/m (m passato come parametro)
    divide(v)=(v1,v2,v3....vm)
  * pq(v) : applica la quantizzazione su ogni sottovettore e li concatena 
    q(divide(xv)= (q(v1),q(vm),...,q(vm))
* ANN esaustiva
  * distanza simmetrica o SDC(x,y): SQRT(Sommatoria(distanza(q(x),q(y))^2)), tenendo in memoria l'insieme di distanze tra ogni centroide
  * distanza asimmetrita o ADC(x,y): SQRT(Sommatoria(distanza(x,q(y))^2)),    tenendo in memoria per ogni diversa x le distanze da ogni centroide
* ANN non esaustiva
  * calcolo residui : r(y)=y-q(y)
  * approx(y)=q(y)-pq(r(y))
  * dist_approx(x,y)=dist(approx(x)-q(y),approx(y)-q(y)) **(? approx o x e y puri?)**
    * dist_approx deve funzionare sia con SDC che con ADC
  * algoritmo:
    * scegliere un set di campioni Ry **(? come viene scelto)** di dimensione nr (passato come input)
    * calcolare attraverso questo un qp unico per tutti **(?unico in che senso?)**
    * per ogni centroide ci determinare approx_dist(x,y) tale che q(y)=ci e collezionare le K distanze più piccoli
    * restituire quindi i K punti

