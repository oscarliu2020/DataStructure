#include <algorithm>
#include <assert.h>
#include <ctime>
#include <iostream>
#include <random>
#include <set>
#include <unordered_set>
#include <vector>
using namespace std;
random_device rd;
mt19937 gen(rd());
using Adj = vector<vector<int>>;
using vvi = vector<vector<int>>;
constexpr int N = 20;
Adj rand_comp(int n) {
  vvi M(n, vector<int>(n, 0));
  unordered_set<int> S;
  //  for (int i = n - 1; i >= 0; i--) {
  //    if (!S.empty()) {
  //      for (int k = 0; k < 3; k++) {
  //        uniform_int_distribution<> dist(0, S.size() - 1), rev(0, 1);
  //        int v = dist(gen);
  //        M[i][v] = 1;
  //        int r = rev(gen);
  //        if (r)
  //          M[v][i] = 1;
  //      }
  //    }
  //    S.insert(i);
  //  }
  S.insert(0);
  for (int i = 1; i < n; i++) {
    for (int k = 0; k < 3; k++) {
      uniform_int_distribution<> dist(0, S.size() - 1), rev(0, 1);
      int v = dist(gen);
      M[v][i] = 1;
      int r = rev(gen);
      if (r)
        M[i][v] = 1;
    }
    S.insert(i);
  }
  Adj g(n);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (M[i][j])
        g[i].push_back(j);
    }
  }
  return g;
}

Adj rand_graph(int n, bitset<N> &exist) {
  Adj g(n);

  vector<pair<int, int>> pairs;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      pairs.emplace_back(i, j);
    }
  }
  shuffle(pairs.begin(), pairs.end(), gen);
  uniform_int_distribution<> dist(3, n);
  int len = dist(gen);
  for (int i = 0; i < len; i++) {
    auto [u, v] = pairs[i];
    g[u].push_back(v);
    exist[u] = true;
    exist[v] = true;
  }
  return g;
}

bitset<N> vis;
void dfs(int u, int del, const vvi &G) {
  assert(u >= 0 && u <= N - 1);
  vis[u] = 1;
  for (int v : G[u]) {
    if (vis[v] || v == del)
      continue;
    dfs(v, del, G);
  }
}

vector<bitset<N>> bruteforce(const vector<vector<int>> &G) {
  vector<bitset<N>> dom(N);
  for (int i = 1; i < N; i++) {
    vis.reset();
    dfs(0, i, G);
    for (int j = 0; j < N; j++) {
      dom[j].set(0);
      if (!vis[j])
        dom[j].set(i);
    }
  }
  return dom;
}

// quadratic time
vector<bitset<N>> flow1(const vector<vector<int>> &rG) {
  vector<bitset<N>> dom(N);
  bool changed = true;
  dom[0][0] = 1;
  for (int i = 1; i < N; i++) {
    dom[i].set();
  }
  while (changed) {
    changed = false;
    for (int i = 1; i < N; i++) {
      bitset<N> tmp;
      tmp.set();
      for (int p : rG[i]) {
        tmp &= dom[p];
      }
      tmp[i] = 1;
      if (tmp != dom[i]) {
        dom[i] = tmp;
        changed = true;
      }
    }
  }
  return dom;
}
void postorder(int u, const Adj &G, vector<int> &po) {
  vis[u] = 1;
  for (int v : G[u]) {
    if (!vis[v])
      postorder(v, G, po);
  }
  po.push_back(u);
}
vector<bitset<N>> flow2(const Adj &rG, const vector<int> &po) {
  bool changed = true;
  vector<bitset<N>> dom(N);
  dom[0][0] = 1;
  for (int i = 1; i < N; i++)
    dom[i].set();
  while (changed) {
    changed = false;
    for (int i = 1; i < po.size(); i++) {
      int u = po[i];
      bitset<N> tmp;
      tmp.set();
      for (int p : rG[u]) {
        tmp &= dom[p];
      }
      tmp[u] = 1;
      if (dom[u] != tmp) {
        dom[u] = tmp;
        changed = true;
      }
    }
  }
  return dom;
}
int intersect(int a, int b, const vector<int> &idom, const array<int, N> &pi) {
  while (a != b) {
    if (pi[a] < pi[b])
      a = idom[a];
    else
      b = idom[b];
  }
  assert(a != -1);
  return a;
}
// optimized
vector<bitset<N>> flow3(const Adj &rG, const vector<int> &po,
                        const array<int, N> &pi) {
  vector<int> idom(N, -1);
  bool changed = true;
  idom[0] = 0;
  while (changed) {
    changed = false;
    assert(po[0] == 0);
    for (int i = 1; i < po.size(); i++) {
      int u = po[i];
      int j = 0;
      for (j = 0; j < rG[u].size(); j++)
        if (idom[rG[u][j]] != -1)
          break;
      int new_dom = idom[rG[u][j]];
      for (; j < rG[u].size(); j++) {
        int v = rG[u][j];
        if (idom[v] == -1)
          continue;
        new_dom = intersect(new_dom, v, idom, pi);
      }
      if (idom[u] != new_dom) {
        changed = true;
        idom[u] = new_dom;
      }
    }
  }
  vector<bitset<N>> dom(N);
  for (int i = 0; i < N; i++) {
    int tmp = i;
    do {
      dom[i].set(tmp);
      tmp = idom[tmp];
      // assert(tmp != -1);
    } while (tmp > 0);
    dom[i][0] = 1;
  }
  return dom;
}
// Tarjan
/// Reference: [https://usaco.guide/adv/critical?lang=cpp]
///
/// u strictly dominates v if u dominate v and u!=v
/// let the immediate dominator of u, or idom(u), be the unique node v such that
/// it strictly dominates node u and every other dominator of node u strictly
/// dominates node v Let e(u) be the entry time in node u doing a Euler tour
/// semi-dominator of u ,sdom(u), be a v such that there's a path from v to u
/// and e(i)>=e(u) for every intermidate node i along the path from u to v
void test() {
  Adj G = rand_comp(N);
  auto ans = bruteforce(G);
  Adj Gr(N);
  for (int u = 0; u < N; u++) {
    for (int v : G[u])
      Gr[v].push_back(u);
  }
  for (int i = 0; i < N; i++) {
    for (int v : G[i]) {
      cout << i << "->" << v << endl;
    }
  }
  for (int i = 0; i < N; i++)
    cout << ans[i].to_string() << ' ';
  cout << endl;
  {
    clock_t start = clock();
    auto f1 = flow1(Gr);
    cout << "flow1() Time: " << (clock() - start) / (double)CLOCKS_PER_SEC
         << endl;
    assert(f1 == ans);
  }
  {

    clock_t start = clock();
    vis.reset();
    vector<int> po;
    postorder(0, G, po);
    reverse(po.begin(), po.end());
    auto f2 = flow2(Gr, po);
    cout << "flow2() Time: " << (clock() - start) / (double)CLOCKS_PER_SEC
         << endl;
    if (ans != f2) {
      cout << "ans\n";
      for (int i = 0; i < N; i++) {
        cout << ans[i].to_string() << ' ';
      }
      cout << endl << "f2\n";
      for (int i = 0; i < N; i++) {
        cout << f2[i].to_string() << ' ';
      }
      cout << endl;
    }
  }
  {
    clock_t start = clock();
    vis.reset();
    vector<int> po;
    postorder(0, G, po);
    array<int, N> pi{0};
    for (int i = 0; i < po.size(); i++) {
      pi[po[i]] = i;
    }
    reverse(po.begin(), po.end());
    auto f3 = flow3(Gr, po, pi);
    cout << "flow3() Time: " << (clock() - start) / (double)CLOCKS_PER_SEC
         << endl;
    if (f3 != ans) {
      for (int i = 0; i < N; i++) {
        for (int v : G[i]) {
          cout << i << "->" << v << endl;
        }
      }
      cout << "ans\n";
      for (int i = 0; i < N; i++)
        cout << ans[i].to_string() << ' ';
      cout << endl << "f3\n";
      for (int i = 0; i < N; i++)
        cout << f3[i].to_string() << ' ';
      cout << endl;
    }
  }
}
int main() {
  for (int i = 0; i < 1; i++)
    test();
  return 0;
}
