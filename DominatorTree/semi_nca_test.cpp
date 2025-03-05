#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <iterator>
#include <stack>
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
  vector<int> dom, fa, pa, e, o, label, sdom;
  int cnt = 0;
  Graph G, rG;
  int n;
  SemiNCA(int n)
      : G(n), rG(n + 1), dom(n + 1, -1), pa(n + 1, -1), fa(n + 1, -1), e(n, 0),
        o(n + 1, -1), label(n + 1, -1), sdom(n + 1, -1) {
    this->n = n;
  }
  void link(int v, int w) { fa[w] = v; }
  int find(int u, bool m = false) {
    if (fa[u] == u)
      return m ? -1 : u;
    int v = find(fa[u], m || 1);
    if (v < 0)
      return (m ? fa[u] : label[u]);
    if (label[fa[u]] < label[u])
      label[u] = label[fa[u]];
    fa[u] = v;
    return (m ? v : label[u]);
  }
  void dfs(int u) {
    e[u] = ++cnt;
    sdom[cnt] = fa[cnt] = label[cnt] = cnt;
    o[cnt] = u;
    for (int x = G.head[u]; ~x; x = G.edges[x].next) {
      int v = G.edges[x].v;
      if (!e[v]) {
        dfs(v);
        assert(e[v]);
        pa[e[v]] = e[u];
      }
      assert(e[v]);
      rG.add_edge(e[v], e[u]);
    }
  }
  struct Frame {
    int u, nxt;
    bool post;
  };
  void dfsi(int r) {
    stack<Frame> st;
    st.push({r, G.head[r], 0});
    e[r] = ++cnt;
    o[cnt] = r;
    while (st.size()) {
      auto &[u, nxt, post] = st.top();
      if (post) {
        assert(e[nxt]);
        rG.add_edge(e[nxt], e[u]);
        pa[e[nxt]] = e[u];
        st.pop();
        continue;
      }
      if (~nxt) {
        int v = G.edges[nxt].v;
        nxt = G.edges[nxt].next;
        if (!e[v]) {
          st.push({u, v, 1});
          st.push({v, G.head[v], 0});
          e[v] = ++cnt;
          o[cnt] = v;
        } else {
          rG.add_edge(e[v], e[u]);
        }
      } else {
        st.pop();
      }
    }
    for (int i = 0; i <= cnt; i++)
      sdom[i] = fa[i] = label[i] = i;
  }
  void add_edge(int u, int v) { G.add_edge(u, v); }
  vector<int> solve(int s) {
    dfsi(s);
    assert(o[1] == s && e[s] == 1);
    for (int w = cnt; w >= 2; w--) {
      for (int x = rG.head[w]; ~x; x = rG.edges[x].next) {
        int v = rG.edges[x].v;
        int u = find(v);
        sdom[w] = min(sdom[w], sdom[u]);
      }
      link(pa[w], w);
      label[w] = sdom[w];
      dom[w] = pa[w];
    }
    for (int v = 2; v <= cnt; v++) {
      while (dom[v] > sdom[v]) {
        dom[v] = dom[dom[v]];
      }
    }
    dom[1] = 1;
    vector<int> out_doms(n, -1);
    for (int i = 1; i <= cnt; i++) {
      out_doms[o[i]] = o[dom[i]];
    }
    return out_doms;
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
  auto doms = sol.solve(s);
  for (int i = 0; i < n; i++)
    cout << doms[i] << ' ';
}
