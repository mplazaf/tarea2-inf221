#include <bits/stdc++.h>
#include "path_result.h"
using namespace std;

typedef long long ll;

struct datosBF{
    int n, m, p;
    const vector<vector<int>>& types;
    const vector<vector<ll>>& values;
    string path; 
    PathResult mejor;  
};

void exploreBF(datosBF& datos, int i, int j, int lives, ll score){
    int tipo = datos.types[i][j];
    if (tipo == 0){
      return;
    }  
    if (tipo == -1){ 
        lives--;
        score -= datos.p;
        if (lives == 0){
          return;
        }
    } 
    else{                          
        score += datos.values[i][j];
    }
    if (i == datos.n - 1 && j == datos.m - 1){
        if (!datos.mejor.found || score > datos.mejor.score){
            datos.mejor.found = true;
            datos.mejor.score = score;
            datos.mejor.path = datos.path;
            datos.mejor.remaining_lives = lives;
        }
        return;
    }
    if (j+1 < datos.m){
        datos.path.push_back('R');
        exploreBF(datos, i, j + 1, lives, score);
        datos.path.pop_back();
    }
    if (i+1 < datos.n){
        datos.path.push_back('D');
        exploreBF(datos, i+1, j, lives, score);
        datos.path.pop_back();
    }
}

PathResult solveBruteForce(int n, int m, int v, int p,
                           const std::vector<std::vector<int>>& types,
                           const std::vector<std::vector<long long>>& values) {

    datosBF datos{n, m, p, types, values, "", PathResult{false, 0, "", 0}};
    datos.path.reserve(n + m - 2);
    exploreBF(datos, 0, 0, v, 0);
    return datos.mejor;  
}
