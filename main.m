X = dsload('prova.ds');
Q = dsload('prova.qs');

dst = alldists(Q,X);

[id,dst2] = sort(dst,2);

