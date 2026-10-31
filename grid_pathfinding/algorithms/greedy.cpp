#include <bits/stdc++.h>
#include "path_result.h"
using namespace std;

typedef long long ll;

PathResult solveGreedy(int n, int m, int v, int p,
                       const std::vector<std::vector<int>>& types,
                       const std::vector<std::vector<long long>>& values) {

    PathResult res{false, 0, "", v};
    int i = 0, j = 0;
    int lives = v;
    long long score = values[0][0]; 

    while (i != n - 1 || j != m - 1){
        bool puedeDerecha = false;
        ll puntosDerecha = 0;
        if (j + 1 < m){
            int tipo = types[i][j+1];
            if (tipo == 1){
                puedeDerecha = true;
                puntosDerecha = values[i][j+1];
            } else if (tipo == -1 && lives>1){
                puedeDerecha = true;
                puntosDerecha = -p;
            }
        }
        bool puedeAbajo = false;
        long long puntosAbajo = 0;
        if (i + 1 < n) {
            int tipo = types[i+1][j];
            if (tipo == 1){
                puedeAbajo = true;
                puntosAbajo = values[i+1][j];
            } else if (tipo == -1 && lives>1){
                puedeAbajo = true;
                puntosAbajo = -p;
            }
        }
        if (!puedeDerecha && !puedeAbajo){
            res.found = false; 
            return res; 
        }
        bool irDerecha = puedeDerecha && (!puedeAbajo || puntosDerecha >= puntosAbajo);

        if (irDerecha){
            j++;
            res.path.push_back('R');
        } else {
            i++;
            res.path.push_back('D');
        }

        if (types[i][j] == -1){
            lives--;
            score -= p;
        } else {
            score += values[i][j];
        }
    }
    res.found = true;
    res.score = score;
    res.remaining_lives = lives;
    return res;
}
