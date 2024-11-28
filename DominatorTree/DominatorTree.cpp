#include <assert.h>
#include <iostream>
#include <random>
#include <set>
#include <vector>
using namespace std;
struct e {
  int u, v;
  e() = default;
  e(int a, int b) : u(a), v(b) {}
  bool operator<(const e &e) const { return (u == e.u ? v < e.v : u < e.u); }
};
using vvi = vector<vector<int>>;
constexpr int N = 1000;
random_device rd;
mt19937_64 gen(rd());
bitset<N> nodes;
vector<e> get_random_edges() {
  uniform_int_distribution<> dist(10, 50);
  int len = dist(gen);
  set<e> s;
  s.emplace(0, 1);
  uniform_int_distribution<> d(1, N - 1);
  for (int i = 1; i < len; i++) {
    int a = d(gen), b = d(gen);
    if (a == b)
      b = (b + 1) % (N - 1) + 1;
    assert(0 <= b && b < N);
    s.emplace(a, b);
  }

  vector<e> edges(s.begin(), s.end());
  return edges;
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
void connected(int u, const vvi &G) {
  nodes[u] = 1;
  for (int v : G[u]) {
    if (nodes[v])
      continue;
    connected(v, G);
  }
}
vector<bitset<N>> bruteforce(const vector<vector<int>> &G) {
  vector<bitset<N>> dom(N);
  for (int i = 1; i < N; i++) {
    vis.reset();
    if (!nodes[i])
      continue;
    dfs(0, i, G);
    for (int j = 0; j < N; j++) {
      if (!nodes[j])
        continue;
      dom[j].set(0);
      if (!vis[j] && nodes[j])
        dom[j].set(i);
    }
  }
  return dom;
}

// quadratic time
vector<bitset<N>> flow(const vector<vector<int>> &rG) {
  vector<bitset<N>> dom(N);
  bool changed = true;
  dom[0][0] = 1;
  for (int i = 1; i < N; i++) {
    if (!nodes[i])
      continue;
    dom[i].set();
  }
  while (changed) {
    changed = false;
    for (int i = 1; i < N; i++) {
      if (!nodes[i])
        continue;
      bitset<N> tmp;
      tmp.set();
      for (int p : rG[i]) {
        if (nodes[p])
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
// Tarjan
/// Reference: [https://usaco.guide/adv/critical?lang=cpp]
///
/// u strictly dominates v if u dominate v and u!=v
/// let the immediate dominator of u, or idom(u), be the unique node v such that
/// it strictly dominates node u and every other dominator of node u strictly
/// dominates node v Let e(u) be the entry time in node u doing a Euler tour
/// semi-dominator of u ,sdom(u), be a v such that there's a path from v to u
/// and e(i)>=e(u) for every intermidate node i along the path from u to v

int main() {
  vector<vector<int>> G(N);
  vector<vector<int>> rG(N);
  vector<e> edges = get_random_edges();
  nodes.reset();
  cout << rG.size() << endl;
  for (auto edge : edges) {
    G[edge.u].push_back(edge.v);
    rG[edge.v].push_back(edge.u);
  }
  connected(0, G);
  cout << nodes << endl;
  // for (auto e : edges) {
  //   if (!(nodes[e.u] && nodes[e.v]))
  //     continue;
  //   cout << e.u << ' ' << e.v << endl;
  // }
  auto ans = bruteforce(G);
  auto res = flow(rG);
  for (int i = 0; i < N; i++) {
    if (!nodes[i])
      continue;
    if (ans[i] != res[i]) {
      cout << i << " WA\n";
      cout << ans[i] << endl;
      cout << res[i] << endl;
      break;
    }
  }
  return 0;
}
