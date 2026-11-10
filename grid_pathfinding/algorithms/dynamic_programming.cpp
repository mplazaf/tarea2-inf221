#include "path_result.h"
#include<bits/stdc++.h>
using namespace std;

typedef long long ll;

PathResult solveDP(int n, int m, int v, int p,const std::vector<std::vector<int>>& types,const std::vector<std::vector<long long>>& values) {

    const long long NEG = LLONG_MIN/4; 
    auto idx = [&](int i, int j, int k) {
        return ((long long)i * m + j) * (v + 1) + k;
    };
    ll total = (long long)n * m * (v + 1);
    vector<ll> best(total, NEG);
    vector<char> from(total, 0);
    best[idx(0, 0, v)] = values[0][0];

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (i == 0 && j == 0) continue;
            int t = types[i][j];
            if (t == 0) continue;
            for (int k = 1; k <= v; k++) {
                int prevK = (t == -1) ? k + 1 : k;
                if (prevK > v) continue;

                ll fromLeft = (j > 0) ? best[idx(i, j - 1, prevK)] : NEG;
                ll fromUp   = (i > 0) ? best[idx(i - 1, j, prevK)] : NEG;
                if (fromLeft == NEG && fromUp == NEG) continue;
                ll gain = (t == -1) ? -p : values[i][j];
                if (fromLeft >= fromUp){
                    best[idx(i, j, k)] = fromLeft + gain;
                    from[idx(i, j, k)] = 'R';
                } else{
                    best[idx(i, j, k)] = fromUp + gain;
                    from[idx(i, j, k)] = 'D';
                }
            }
        }
    }

    PathResult res{false, 0, "", 0};
    int bestK = -1;
    for (int k = 1; k <= v; k++) {
        long long s = best[idx(n - 1, m - 1, k)];
        if (s != NEG && (bestK == -1 || s > res.score)) {
            res.score = s;
            bestK = k;
        }
    }
    if (bestK == -1){
      return res;
    }
    res.found = true;
    res.remaining_lives = bestK;
    int i = n - 1;
    int j = m - 1;
    int k = bestK;
    while (i != 0 || j != 0) {
        char move = from[idx(i, j, k)];
        res.path.push_back(move);
        if (types[i][j] == -1) {
          k++;
        }
        if (move == 'R') {
          j--;
        }
        else{
          i--;
        }
    }
    reverse(res.path.begin(), res.path.end());
    return res;
}
