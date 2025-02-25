#include <algorithm>
#include <assert.h>
#include <iostream>
#include <vector>
using namespace std;
using Adj = vector<vector<int>>;
constexpr int N = 100001;
int pa[N], o[N], dom[N], e[N];
int tot = 0;
void dfs(int u, int p, const Adj &G) {
  e[u] = ++tot;
  o[tot] = u;
  pa[u] = p;
  for (int v : G[u]) {
    if (!e[v])
      dfs(v, u, G);
  }
}
int fa[N], label[N];
void link(int v, int w) { fa[w] = v; }
void compress(int v) {
  if (fa[fa[v]] != 0) {
    compress(fa[v]);
    if (e[label[fa[v]]] < e[label[v]]) {
      label[v] = label[fa[v]];
    }
    fa[v] = fa[fa[v]];
  }
}
int find(int v) {
  if (fa[v] == 0)
    return v;
  else {
    compress(v);
    return label[v];
  }
}
void tarjan(const Adj &G, const Adj &rG) {
  int n = G.size();
  assert(n <= N);
  Adj t(n);
  --n;
  dfs(1, 0, G);
  for (int i = 0; i <= n; i++)
    label[i] = i;
  for (int i = n; i >= 2; i--) {
    int w = o[i];
    for (int v : rG[w]) {
      if (!e[v])
        continue;
      int u = find(v);
      if (e[u] < e[w])
        e[w] = e[u];
    }
    link(pa[w], w);
    t[o[e[w]]].push_back(w);
    for (int v : t[pa[w]]) {
      int u = find(v);
      if (e[u] < e[v])
        dom[v] = u;
      else
        dom[v] = pa[w];
    }
    t[pa[w]].clear();
  }
  for (int i = 2; i <= n; i++) {
    int w = o[i];
    if (dom[w] != o[e[w]]) {
      dom[w] = dom[dom[w]];
    }
  }
  dom[1] = 0;
}
int main() {
  int n, m;
  cin.tie(0)->sync_with_stdio(0);
  cin >> n >> m;
  Adj G(n + 1), rG(n + 1);
  for (int i = 0; i < m; i++) {
    int u, v;
    cin >> u >> v;
    G[u].push_back(v);
    rG[v].push_back(u);
  }
  tarjan(G, rG);
  int t = n;
  vector<int> res;
  do {
    res.push_back(t);
    t = dom[t];
  } while (t);
  sort(res.begin(), res.end());
  cout << res.size() << '\n';
  for (int i = 0; i < res.size(); ++i) {
    cout << res[i] << ' ';
  }
  return 0;
}
