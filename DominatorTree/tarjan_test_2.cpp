#include <algorithm>
#include <assert.h>
#include <cstring>
#include <iostream>
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
class SLT {
  vector<int> e, o, label, fa, pa;
  int n, cnt;
  Graph G, rG;
  void dfs(int u) {
    e[u] = ++cnt;
    o[cnt] = u;
    for (int x = G.head[u]; ~x; x = G.edges[x].next) {
      int v = G.edges[x].v;
      if (!e[v]) {
        dfs(v);
        pa[v] = u;
      }
      rG.add_edge(v, u);
    }
  }
  int find(int u, bool m = 0) {
    if (fa[u] == u)
      return m ? -1 : u;
    int v = find(fa[u], m || 1);
    if (v < 0)
      return m ? fa[u] : label[u];
    if (e[label[fa[u]]] < e[label[u]])
      label[u] = label[fa[u]];
    fa[u] = v;
    return m ? v : label[u];
  }
  int link(int v, int w) { return fa[w] = v; }

public:
  SLT(int n)
      : G(n), rG(n), n(n), cnt(0), e(n), o(n + 1), label(n), fa(n), pa(n) {}
  void add_edge(int u, int v) { G.add_edge(u, v); }
  vector<int> solve(int s) {
    dfs(s);
    Adj t(n);
    vector<int> dom(n, -1);
    for (int i = 0; i < n; i++) {
      label[i] = fa[i] = i;
    }
    for (int i = cnt; i >= 2; i--) {
      int w = o[i];
      for (int x = rG.head[w]; ~x; x = rG.edges[x].next) {
        int v = rG.edges[x].v;
        int u = find(v);
        e[w] = min(e[w], e[u]);
      }
      link(pa[w], w);
      t[o[e[w]]].push_back(w);
      for (int v : t[pa[w]]) {
        int u = find(v);
        if (e[u] == e[v])
          dom[v] = pa[w];
        else {
          assert(e[u] < e[v]);
          dom[v] = u;
        }
      }
      t[pa[w]].clear();
    }
    for (int i = 2; i <= cnt; i++) {
      int v = o[i];
      if (dom[v] != o[e[v]])
        dom[v] = dom[dom[v]];
    }
    dom[s] = s;
    return dom;
  }
};
int main() {
  int n, m, s;
  cin.tie(0)->sync_with_stdio(0);
  cin >> n >> m >> s;
  SLT sol(n);
  for (int i = 0; i < m; i++) {
    int u, v;
    cin >> u >> v;
    sol.add_edge(u, v);
  }
  auto dom = sol.solve(s);
  for (int i = 0; i < n; i++) {
    cout << dom[i] << ' ';
  }
  return 0;
}
