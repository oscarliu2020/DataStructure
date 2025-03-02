#include <algorithm>
#include <assert.h>
#include <cstring>
#include <iostream>
#include <vector>
using namespace std;
using Adj = vector<vector<int>>;
constexpr int N = 200001;
int e[N], o[N];
int cnt;
int label[N], fa[N], pa[N];
void dfs(int u, int p, const Adj &G, Adj &rG) {
  e[u] = ++cnt;
  o[cnt] = u;
  label[u] = u;
  pa[u] = p;
  fa[u] = u;
  for (int v : G[u]) {
    if (!e[v])
      dfs(v, u, G, rG);
    rG[v].push_back(u);
  }
}
void compress(int v) {
  if (fa[fa[v]] != fa[v]) {
    compress(fa[v]);
    if (e[label[fa[v]]] < e[label[v]]) {
      label[v] = label[fa[v]];
    }
    fa[v] = fa[fa[v]];
  }
}
int find(int v) {
  if (v == fa[v])
    return v;
  compress(v);

  return label[v];
}
int dom[N];
inline void link(int w, int v) { fa[v] = w; }
void tarjan(int s, const Adj &G) {
  Adj rG(G.size());
  dfs(s, -1, G, rG);
  assert(e[s] == 1);
  assert(o[1] == s);
  Adj t(G.size());
  memset(dom, -1, G.size() * sizeof(int));
  for (int i = cnt; i >= 2; i--) {
    int w = o[i];
    assert(pa[w] != -1);
    for (int v : rG[w]) {
      int u = find(v);
      assert(e[u] && e[v]);
      e[w] = min(e[u], e[w]);
    }
    link(pa[w], w);
    t[o[e[w]]].push_back(w);
    for (int v : t[pa[w]]) {
      int u = find(v);
      assert(e[v] && e[u]);
      if (e[v] > e[u])
        dom[v] = u;
      else
        dom[v] = pa[w];
    }
    t[pa[w]].clear();
  }
  for (int i = 2; i <= cnt; i++) {
    int w = o[i];
    if (dom[w] != o[e[w]])
      dom[w] = dom[dom[w]];
  }
  dom[s] = s;
}
int main() {
  int n, m, s;
  cin.tie(0)->sync_with_stdio(0);
  cin >> n >> m >> s;
  Adj G(n), rG(n);
  for (int i = 0; i < m; i++) {
    int u, v;
    cin >> u >> v;
    G[u].push_back(v);
  }
  tarjan(s, G);
  for (int i = 0; i < n; i++) {
    cout << dom[i] << ' ';
  }
  return 0;
}
