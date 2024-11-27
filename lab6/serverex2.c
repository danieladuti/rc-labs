/*

consideram un fisier cu o configuratie de tipul cheie:comanda.
ex.
1111:/bin/ls:
2222:/bin/pwd:
3333:/bin/ps:

Sa se scrie un server tcp iterativ care functioneaza astfel:
    -atunci cand un client se conecteaza la server si ii trimite una dintre key-urile aflate in fisier,
atunci serverul va executa comanda asociata si va intoarce clientului rezultatul acesteia;
    -pentru orice alta cheie, serverul va trimite raspunsul "unknown key" clientului si va inchide 
conexiunea cu acesta.

*/