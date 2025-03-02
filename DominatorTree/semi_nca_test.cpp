#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <iterator>
#include <vector>
using namespace std;
constexpr int N = 200001;
using Adj = vector<vector<int>>;
struct Edge {
  int v, next;
};
struct Graph {
  Edge edges[N];
  int head[N];
  int size = 0;
  Graph(int n) { memset(head, -1, n * sizeof(int)); }
  void add_edge(int u, int v) {
    int e = size++;
    edges[e] = {v, head[u]};
    head[u] = e;
  }
};
struct SemiNCA {
  // int dom[N], fa[N], pa[N], e[N], o[N], label[N];
  vector<int> dom, fa, pa, e, o, label;
  int cnt = 0;
  Graph G, rG;
  int n;
  SemiNCA(int n)
      : G(n), rG(n), dom(n, -1), pa(n, -1), fa(n, -1), e(n, 0), o(n + 1, -1),
        label(n, 0) {
    this->n = n;
  }
  void link(int v, int w) { pa[w] = v; }
  // void compress(int v) {
  //   if (fa[fa[v]] != fa[v]) {
  //     compress(fa[v]);
  //     if (e[label[fa[v]]] < e[label[v]]) {
  //       label[v] = label[fa[v]];
  //     }
  //     fa[v] = fa[fa[v]];
  //   }
  // }
  // int find(int v) {
  //   if (v == fa[v])
  //     return v;
  //   compress(v);
  //
  //   return label[v];
  // }
  int find(int u, bool m = false) {
    if (fa[u] == u)
      return m ? -1 : u;
    int v = find(fa[u], m || 1);
    if (v < 0)
      return u;
    if (e[label[fa[u]]] < e[label[u]])
      label[u] = label[fa[u]];
    fa[u] = v;
    return (m ? v : label[u]);
  }
  void dfs(int u, int p) {
    e[u] = ++cnt;
    fa[u] = label[u] = u;
    o[cnt] = u;
    pa[u] = p;
    for (int x = G.head[u]; ~x; x = G.edges[x].next) {
      int v = G.edges[x].v;
      if (!e[v]) {
        dfs(v, u);
      }
      rG.add_edge(v, u);
    }
  }
  void add_edge(int u, int v) { G.add_edge(u, v); }
  void solve(int s) {
    dfs(s, -1);
    assert(o[1] == s && e[s] == 1);
    for (int i = cnt; i >= 2; i--) {
      int w = o[i];
      for (int x = rG.head[w]; ~x; x = rG.edges[x].next) {
        int v = rG.edges[x].v;
        e[w] = min(e[w], e[find(v)]);
      }
      link(pa[w], w);
      dom[w] = pa[w];
    }
    for (int i = 2; i <= cnt; i++) {
      int v = o[i];
      while (e[dom[v]] > e[v]) {
        dom[v] = dom[dom[v]];
      }
    }
    dom[s] = s;
  }
};
int main() {
  cin.tie(0)->sync_with_stdio(0);
  int n, m, s;
  cin >> n >> m >> s;
  SemiNCA sol(n);
  while (m--) {
    int u, v;
    cin >> u >> v;
    sol.add_edge(u, v);
  }
  sol.solve(s);
  for (int i = 0; i < n; i++)
    cout << sol.dom[i] << ' ';
}
