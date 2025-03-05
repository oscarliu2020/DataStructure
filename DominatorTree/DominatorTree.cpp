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
constexpr int N = 1e8;
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
///

struct Edge {
  int v, next;
};
struct Graph {
  vector<Edge> edges;
  vector<int> head;
  int size = 0;
  Graph(int n) : head(n, -1) {}
  void add_edge(int u, int v) {
    int e = size++;
    if (size >= edges.size())
      edges.push_back((Edge){-1, -1});
    edges[e] = {v, head[u]};
    head[u] = e;
  }
};
struct Frame {
  int u;
  int e;
  int prev;
};
class Stack {
  array<Frame, N> *inner;
  int sz = 0;

public:
  Stack() { inner = new array<Frame, N>; }
  ~Stack() { delete inner; }
  void push(const Frame f) {
    assert(sz < inner[0].size());
    inner[0][sz++] = f;
  }
  void pop() {
    assert(sz > 0);
    --sz;
  }
  Frame &top() {
    assert(sz > 0);
    return inner[0][sz - 1];
  }
  size_t size() { return sz; }
};
class SLT {
  vector<int> e, o, label, fa, pa;
  int n, cnt;
  Graph G, rG;
  // void dfs(int u) {
  //   e[u] = ++cnt;
  //   o[cnt] = u;
  //   for (int x = G.head[u]; ~x; x = G.edges[x].next) {
  //     int v = G.edges[x].v;
  //     if (!e[v]) {
  //       dfs(v);
  //       pa[v] = u;
  //     }
  //     rG.add_edge(v, u);
  //   }
  // }
  void dfsi(int r) {
    e[r] = ++cnt;
    o[cnt] = r;
    Stack st;
    st.push({r, G.head[r], -1});
    while (st.size()) {
      auto &[u, nxt, prev] = st.top();
      if (~prev) {
        rG.add_edge(prev, u);
        prev = -1;
      }
      if (nxt != -1) {
        int v = G.edges[nxt].v;
        nxt = G.edges[nxt].next;
        if (!e[v]) {
          prev = v;
          Frame t{v, G.head[v], -1};
          st.push(t);
          e[v] = ++cnt;
          o[cnt] = v;
          pa[v] = u;
        } else {
          rG.add_edge(v, u);
        }
      } else {
        st.pop();
      }
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
    dfsi(s);
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
class BFSCooper {
  int cnt;
  vector<int> po, pi, idom;
  Graph G, rG;
  vector<bool> vis;
  int n;
  int intersect(int u, int v) {
    while (u != v) {
      if (pi[u] < pi[v])
        u = idom[u];
      else
        v = idom[v];
    }
    return u;
  }

public:
  BFSCooper(int n) : n(n), rG(n), G(n), vis(n, 0), po(n), pi(n), idom(n, -1) {
    fill(idom.begin(), idom.begin() + n, -1);
  }
  void add_edge(int u, int v) {
    G.add_edge(u, v);
    rG.add_edge(v, u);
  }
  vector<int> solve(int s) {
    queue<int> q;
    q.push(s);
    vis[s] = 1;
    while (q.size()) {
      int t = q.front();
      q.pop();
      pi[t] = n - cnt - 1;
      po[cnt++] = t;
      for (int x = G.head[t]; ~x; x = G.edges[x].next) {
        int v = G.edges[x].v;
        if (vis[v])
          continue;
        vis[v] = 1;
        q.push(v);
      }
    }
    idom[s] = s;
    bool changed = true;
    while (changed) {
      changed = false;
      for (int i = 1; i < cnt; i++) {
        int temp = -1;
        int u = po[i];
        for (int x = rG.head[u]; ~x; x = rG.edges[x].next) {
          int v = rG.edges[x].v;
          if (idom[v] == -1)
            continue;
          if (temp == -1)
            temp = v;
          else
            temp = intersect(temp, v);
        }
        if (idom[u] != temp)
          idom[u] = temp, changed = true;
      }
    }
    return idom;
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
    int prev;
  };

  class Stack {
    array<Frame, N> *inner;
    int sz = 0;

  public:
    Stack() { inner = new array<Frame, N>; }
    ~Stack() { delete inner; }
    void push(const Frame f) {
      assert(sz < inner[0].size());
      inner[0][sz++] = f;
    }
    void pop() {
      assert(sz > 0);
      --sz;
    }
    Frame &top() {
      assert(sz > 0);
      return inner[0][sz - 1];
    }
    size_t size() { return sz; }
  };
  ;
  void dfsi(int r) {
    Stack st;
    st.push({r, G.head[r], -1});
    e[r] = ++cnt;
    o[cnt] = r;
    while (st.size()) {
      auto &[u, nxt, prev] = st.top();
      if (prev != -1) {
        assert(e[prev]);
        rG.add_edge(e[prev], e[u]);
        pa[e[prev]] = e[u];
        prev = -1;
      }
      if (~nxt) {
        int v = G.edges[nxt].v;
        nxt = G.edges[nxt].next;
        if (!e[v]) {
          prev = v;
          st.push({v, G.head[v], -1});
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
constexpr int NN = 1e5;
void test_tarjan() {
  auto g = rand_comp(NN);
  BFSCooper s1(NN);
  SLT s2(NN);
  SemiNCA s3(NN);
  for (int i = 0; i < g.size(); i++) {
    for (int v : g[i]) {
      s1.add_edge(i, v);
      s2.add_edge(i, v);
      s3.add_edge(i, v);
    }
  }
  cout << "Benchmarking...\n" << endl;
  {
    clock_t start = clock();
    s1.solve(0);
    cout << "BFS Iterative: " << (clock() - start) / (double)CLOCKS_PER_SEC
         << endl;
    start = clock();
    s2.solve(0);
    cout << "SLT Iterative: " << (clock() - start) / (double)CLOCKS_PER_SEC
         << endl;
    start = clock();
    s3.solve(0);
    cout << "Semi-NCA Iterative: " << (clock() - start) / (double)CLOCKS_PER_SEC
         << endl;
  }
}

int main() {
  // for (int i = 0; i < 1; i++)
  //   test();
  for (int i = 0; i < 1; i++) {
    test_tarjan();
  }
  return 0;
}
