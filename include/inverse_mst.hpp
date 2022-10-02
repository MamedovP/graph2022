/**
 * @file inverse_mst.hpp
 * @author Polad Mamedov
 *
 * Реализация алгоритма обратной задачи минимального остова.
 */
#ifndef INCLUDE_INVERSE_MST_HPP_
#define INCLUDE_INVERSE_MST_HPP_

#include <iostream>
#include <algorithm>
#include <cmath>
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
 * @param ribs Вектор, в котором хранится результат работы функции.
 */
template <typename T>

void InverseMst(const WeightedGraph<T>& wGraph,
                std::vector<std::pair<std::pair<size_t, size_t>, T>>* ribs) {
  const size_t INF = 1000 * 1000 * 1000;
  size_t n = wGraph.NumVertices();

  /*
  for (auto v = ribs->begin(); v < ribs->end(); ++v) {
    std::cout << (*v).first.first << (*v).first.second << (*v).second << std::endl;
  }*/
  size_t m = ribs->size();
  size_t nn = m + 2, s = nn - 2, t = nn - 1;
  std::vector<std::vector<T>> f(nn, std::vector<T>(nn));
  std::vector<std::vector<T>> u(nn, std::vector<T>(nn));
  std::vector<std::vector<T>> c(nn, std::vector<T>(nn));

  for (size_t i = (n - 1); i < m; ++i) {
    std::vector<size_t> q(n);
    size_t h = 0, t = 0;
    std::pair<std::pair<size_t, size_t>, T>& cur = (*ribs)[i];
    q[t++] = cur.first.first;
    std::vector<int64_t> rib_id(n, -1);
    rib_id[cur.first.first] = -2;
    while (h < t) {
      size_t v = q[h++];
      // std::cout << v << "   v" << std::endl;
      for (auto j : wGraph.Edges(v))
        if (std::abs(std::distance(ribs->begin(),
            std::find(ribs->begin(), ribs->end(),
            (v < j) ? std::make_pair(std::make_pair(v, j),
                                     wGraph.EdgeWeight(v, j))
                    : std::make_pair(std::make_pair(j, v),
                                     wGraph.EdgeWeight(j, v))))) >= n - 1) {
          // break;
        } else if (rib_id[j] == -1) {
          rib_id[j] = std::abs(std::distance(ribs->begin(),
                      std::find(ribs->begin(), ribs->end(),
                      (v < j) ? std::make_pair(std::make_pair(v, j),
                                               wGraph.EdgeWeight(v, j))
                              : std::make_pair(std::make_pair(j, v),
                                               wGraph.EdgeWeight(j, v)))));
          // std::cout << j << " j" << std::endl;
          // std::cerr << rib_id[j] << " ribid" << std::endl;
          q[t++] = j;
        }
    }
    /*for (size_t c = 0; c < n; c++) {
      std::cout << rib_id[c] << "        " << c << std::endl;
    }*/
    for (size_t v = cur.first.second, pv; v != cur.first.first; v = pv) {
      // std::cout << v << " [v]" << std::endl;
      size_t r = static_cast<size_t>(rib_id[v]);
      // std::cout << r << " r" << std::endl;
      pv = (v != (*ribs)[r].first.first)
               ? (*ribs)[r].first.first
               : (*ribs)[r].first.second;
      u[r][i] = n;
      c[r][i] = (*ribs)[i].second - (*ribs)[r].second;
      c[i][r] = -c[r][i];
    }
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
        pi[i] = std::min(pi[i], (*ribs)[j].second - (*ribs)[i].second);
    pi[s] = std::min(pi[s], pi[i]);
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
    if (f[s][i]) (*ribs)[i].second += pi[i];
  for (size_t i = n - 1; i < m; ++i)
    if (f[i][t]) (*ribs)[i].second += pi[i];
/*
  for (auto v = ribs->begin(); v < ribs->end(); ++v) {
    std::cout << (*v).first.first << "&"
              << (*v).first.second << "&"
              << (*v).second << std::endl;
  }*/
}

}  // namespace graph

#endif  // INCLUDE_INVERSE_MST_HPP_
