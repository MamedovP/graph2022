/**
 * @file inverse_mst.hpp
 * @author Polad Mamedov
 *
 * Реализация алгоритма обратной задачи минимального остова.
 */
#ifndef INCLUDE_INVERSE_MST_HPP_
#define INCLUDE_INVERSE_MST_HPP_

#include <iostream>
#include <vector>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include "iterators.hpp"
#include "weighted_graph.hpp"

namespace graph {
/**
 * @brief Функция для решения обратной задачи минимального остова.
 *
 * @tparam T Тип данных весов рёбер графа.
 * @param wGraph Взвешенный граф.
 */
template <typename T>

void InverseMst(const WeightedGraph<T>& wGraph) {
  const size_t INF = 1000 * 1000 * 1000;
  size_t n = wGraph.NumVertices();
  size_t m = wGraph.weights.size();

  size_t nn = m + 2, s = nn - 2, t = nn - 1;
  std::vector<std::vector<T>> f(nn, std::vector<T>(nn));
  std::vector<std::vector<T>> u(nn, std::vector<T>(nn));
  std::vector<std::vector<T>> c(nn, std::vector<T>(nn));
  
  for (auto it = wGraph.weights.begin() + (n - 1); it != wGraph.weights.end();
       ++it) {
    size_t i = n - 1;
    std::vector<size_t> q(n);
    size_t h = 0, t = 0;
    auto& cur = *it;
    q[t++] = cur.first.first;
    std::vector<size_t> rib_id(n, -1);
    rib_id[cur.first.first] = -2;
    while (h < t) {
      size_t v = q[h++];
      for (auto j = (wGraph.edges[v]).begin(); j != (wGraph.edges[v]).end();
           ++j)
        if ((wGraph.weights.find(wGraph.MakeEdgeId(v, *j)) -
             wGraph.weights.begin()) >= n - 1)
          break;
        else if (rib_id[(*j)] == -1) {
          rib_id[(*j)] = wGraph.weights.find(wGraph.MakeEdgeId(v, *j)) -
                         wGraph.weights.begin();
          q[t++] = (*j);
        }
    }
    for (size_t v = cur.first.second, pv; v != cur.first.first; v = pv) {
      size_t r = rib_id[v];
      pv = v != (*(wGraph.weights.begin() + r)).first.first
               ? (*(wGraph.weights.begin() + r)).first.first
               : (*(wGraph.weights.begin() + r)).first.second;
      u[r][i] = n;
      c[r][i] = (*(wGraph.weights.begin() + i)).second -
                (*(wGraph.weights.begin() + r)).second;
      c[i][r] = -c[r][i];
    }
    ++i;
  }
  u[s][t] = n + 1;
  for (size_t i = 0; i < n - 1; ++i) u[s][i] = 1;
  for (size_t i = n - 1; i < m; ++i) u[i][t] = 1;

  std::vector<T> pi(nn);
  pi[s] = INF;
  for (size_t i = 0; i < n - 1; ++i) {
    pi[i] = INF;
    for (size_t j = n - 1; j < m; ++j)
      if (u[i][j])
        pi[i] = min(pi[i], (*(wGraph.weights.begin() + j)).second -
                               (*(wGraph.weights.begin() + i)).second);
    pi[s] = min(pi[s], pi[i]);
  }

  for (;;) {
    std::vector<size_t> id(nn);
    std::deque<size_t> q;
    q.push_back(s);
    std::vector<T> d(nn, INF);
    d[s] = 0;
    std::vector<size_t> p(nn, -1);
    while (!q.empty()) {
      size_t v = q.front();
      q.pop_front();
      id[v] = 2;
      for (size_t i = 0; i < nn; ++i)
        if (f[v][i] < u[v][i]) {
          T new_d = d[v] + c[v][i] - pi[v] + pi[i];
          if (new_d < d[i]) {
            d[i] = new_d;
            if (id[i] == 0)
              q.push_back(i);
            else if (id[i] == 2)
              q.push_front(i);
            id[i] = 1;
            p[i] = v;
          }
        }
    }
    for (size_t i = 0; i < nn; ++i) pi[i] -= d[i];
    for (size_t v = t; v != s; v = p[v]) {
      size_t pv = p[v];
      ++f[pv][v], --f[v][pv];
    }
    if (p[t] == s) break;
  }

  for (size_t i = 0; i < m; ++i) pi[i] -= pi[s];
  for (size_t i = 0; i < n - 1; ++i)
    if (f[s][i]) (*(wGraph.weights.begin() + i)).second += pi[i];
  for (size_t i = n - 1; i < m; ++i)
    if (f[i][t]) (*(wGraph.weights.begin() + i)).second += pi[i];
}

}  // namespace graph

#endif  // INCLUDE_INVERSE_MST_HPP_
