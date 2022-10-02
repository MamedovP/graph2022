/* @file tests/inverse_mst_test.cpp
 * @author Polad Mamedov
 *
 * Реализация набора тестов для алгоритма обратной задачи минимального остова.
*/

#include <httplib.h>
#include <nlohmann/json.hpp>
#include "test_core.hpp"
#include "test.hpp"
#include <inverse_mst.hpp>

static void SimpleTest(httplib::Client* cli);
static void RandomTest(httplib::Client* cli);

void TestInverseMst(httplib::Client* cli) {
  TestSuite suite("TestInverseMst");

  RUN_TEST_REMOTE(suite, cli, SimpleTest);
  RUN_TEST_REMOTE(suite, cli, RandomTest);
}

/**
 * @brief Простейший статический тест.
 *
 * @param cli Указатель на HTTP клиент.
 */

static void SimpleTest(httplib::Client* cli) {
  {
    /*
     Библиотека nlohmann json позволяет преобразовать
     строку в объект nlohmann::json не только при помощи
     функции nlohmann::json::parse(), но и при помощи
     специального литерала _json. Если его поставить после строки
     в кавычках, то она конвертируется в json объект.
     R"(
     )" Так записывается строка, содержащая символы перевода строки
     в C++. Всё, что между скобками это символы строки. Перводы строк
     можно ставить просто как перевод строки в текстовом редактора
     (а не через \n).
    */  
    nlohmann::json input = R"(
  {
    "id": 1,
    "type": "weighted_graph",
    "typeWeights": "int",
    "size": 4,
    "vertices": [0, 1, 2, 3],
    "numEdges": 4,
    "edges": [[0,1,-1],[1,2,1],[2,3,3],[0,3,1]]
  }
)"_json;
      /* Делаем POST запрос по адресу нашего метода на сервере.
    Метод dump() используется для преобразования JSON обратно в строку.
    (Можно было сразу строку передать). При передаче JSON данных
    необходимо поставить тип MIME "application/json".
    */
    httplib::Result res = cli->Post("/InverseMst", input.dump(),
        "application/json");
      /* Используем метод parse() для преобразования строки ответа сервера
    (res->body) в объект JSON. */ 
    nlohmann::json output = nlohmann::json::parse(res->body);
    REQUIRE_EQUAL(4, output["size"]);
    REQUIRE_EQUAL(1, output["id"]);
    REQUIRE_EQUAL("weighted_graph", output["type"]);
    int sum = 0;
    for (size_t i = 0; i < static_cast<size_t>(output["size"]) - 1; i++)
      sum += static_cast<int>(output["data"][i][2]);
    REQUIRE_EQUAL(sum, static_cast<int>(1));
    REQUIRE_EQUAL(output["data"].size(), static_cast<size_t>(4));
      /*Второй тест*/
    nlohmann::json input1 = R"(
  {
    "id": 2,
    "type": "weighted_graph",
    "typeWeights": "int",
    "size": 4,
    "vertices": [0, 1, 2, 3],
    "numEdges": 5,
    "edges": [[0,1,-1],[1,2,1],[2,3,3],[0,3,1],[1,3,2]]
  }
)"_json;
      /* Делаем POST запрос по адресу нашего метода на сервере.
    Метод dump() используется для преобразования JSON обратно в строку.
    (Можно было сразу строку передать). При передаче JSON данных
    необходимо поставить тип MIME "application/json".
    */
    httplib::Result res1 = cli->Post("/InverseMst", input1.dump(),
        "application/json");
      /* Используем метод parse() для преобразования строки ответа сервера
    (res->body) в объект JSON. */ 
    nlohmann::json output1 = nlohmann::json::parse(res1->body);
    REQUIRE_EQUAL(4, output1["size"]);
    REQUIRE_EQUAL(2, output1["id"]);
    REQUIRE_EQUAL("weighted_graph", output1["type"]);
    int sum1 = 0;
    for (size_t i = 0; i < static_cast<size_t>(output1["size"]) - 1; i++)
      sum1 += static_cast<int>(output1["data"][i][2]);
    REQUIRE_EQUAL(sum1, static_cast<int>(1));
    REQUIRE_EQUAL(output1["data"].size(), static_cast<size_t>(5));
       /*Третий тест*/
    nlohmann::json input2 = R"(
  {
    "id": 3,
    "type": "weighted_graph",
    "typeWeights": "int",
    "size": 4,
    "vertices": [0, 1, 2, 3],
    "numEdges": 3,
    "edges": [[0,1,-1],[1,2,1],[2,3,3]]
  }
)"_json;
      /* Делаем POST запрос по адресу нашего метода на сервере.
    Метод dump() используется для преобразования JSON обратно в строку.
    (Можно было сразу строку передать). При передаче JSON данных
    необходимо поставить тип MIME "application/json".
    */
    httplib::Result res2 = cli->Post("/InverseMst", input2.dump(),
        "application/json");
      /* Используем метод parse() для преобразования строки ответа сервера
    (res->body) в объект JSON. */ 
    nlohmann::json output2 = nlohmann::json::parse(res2->body);
    REQUIRE_EQUAL(4, output2["size"]);
    REQUIRE_EQUAL(3, output2["id"]);
    REQUIRE_EQUAL("weighted_graph", output2["type"]);
    int sum2 = 0;
    for (size_t i = 0; i < static_cast<size_t>(output2["size"]) - 1; i++)
      sum2 += static_cast<int>(output2["data"][i][2]);
    REQUIRE_EQUAL(sum2, static_cast<int>(3));
    REQUIRE_EQUAL(output2["data"].size(), static_cast<size_t>(3));
}
}

static void RandomTest(httplib::Client* cli) {
  // Число попыток.
  const int numTries = 10;
  // Используется для инициализации генератора случайных чисел.
  std::random_device rd;
  // Генератор случайных чисел.
  std::mt19937 gen(rd());
  // Распределение для количества элементов массива.
  std::uniform_int_distribution<size_t> arraySize(10, 50);
  // Распределение для элементов массива.
  std::uniform_int_distribution<int> elem(-10'000, 10'000);
  std::uniform_int_distribution<size_t> yesno(0, 1);
  for (int it = 0; it < numTries; it++) {
    nlohmann::json input;
    input["id"] = it;
    input["type"] = "weighted_graph";
    input["typeWeights"] = "int";

    // Получаем случайный размер массива, используя функцию распределения.
    size_t n = arraySize(gen);
    input["size"] = n;
    std::vector<size_t> matrix(n * n);
    std::vector<int> weight(n * n);
    size_t numEdges = 0;

    std::vector<size_t> vFree;
    for (size_t i = 0; i < n; i++) vFree.push_back(i);
    size_t a = 0;
    size_t b = 0;
    vFree.erase(std::find(vFree.begin(), vFree.end(), a));
    while (vFree.size() != 0) {
      std::uniform_int_distribution<size_t> v(0, (vFree.size() - 1));
      b = vFree[v(gen)];
      vFree.erase(std::find(vFree.begin(), vFree.end(), b));
      if (a < b) {
        matrix[a * n + b] = 1;
        weight[a * n + b] = elem(gen);
        input["edges"][numEdges][0] = a;
        input["edges"][numEdges][1] = b;
        input["edges"][numEdges][2] = weight[a * n + b];
        numEdges++;
        } else {
          matrix[b * n + a] = 1;
          weight[b * n + a] = elem(gen);
          input["edges"][numEdges][0] = b;
          input["edges"][numEdges][1] = a;
          input["edges"][numEdges][2] = weight[b * n + a];
          numEdges++;
          }
      a = b;
    }
    for (size_t i = 0; i < n; i++)
      for (size_t j = i + 1; j < n; j++) {
        if ((matrix[i * n + j] != 1) && (yesno(gen))) {
          matrix[i * n + j] = 1;
          weight[i * n + j] = elem(gen);
          input["edges"][numEdges][0] = i;
          input["edges"][numEdges][1] = j;
          input["edges"][numEdges][2] = weight[i * n + j];
          numEdges++;
        }
      }
    input["numEdges"] = numEdges;
    for (size_t i = 0; i < n; i++) {
      input["vertices"][i] = i;
    }
    // Отправляем данные на сервер POST запросом.
    httplib::Result res = cli->Post("/InverseMst", input.dump(),
        "application/json");
    // Используем метод parse() для преобразования строки ответа сервера
    // (res->body) в объект JSON.
    nlohmann::json output = nlohmann::json::parse(res->body);

    // Алгоритм Краскала
    std::vector<std::pair<int, std::pair<size_t, size_t>>> g;
    // вес - вершина 1 - вершина 2

    // заполнение вектора для сортировки
    for (size_t i = 0; i < n; i++)
      for (size_t j = i + 1; j < n; j++)
        if (matrix[i * n + j] == 1)
          g.push_back({weight[i * n + j], {i, j}});
    sort(g.begin(), g.end());
    // std::cout << g.size() << std::endl;
    int cost = 0;
    std::vector<std::pair<int, std::pair<size_t, size_t>>> result;
    std::vector<size_t> treeId(n, n);
    for (size_t i = 0; i < n; i++)
      for (size_t j = i + 1; j < n; j++)
        treeId[i] = i;
    for (size_t i = 0; i < g.size(); i++) {
      size_t a = g[i].second.first,  b = g[i].second.second;
      int l = g[i].first;
      if (treeId[a] != treeId[b]) {
        cost += l;
        result.push_back(std::make_pair(weight[a * n + b],
                         std::make_pair(a, b)));
        size_t oldId = treeId[b],  newId = treeId[a];
        for (size_t j = 0; j < n; j++)
          if (treeId[j] == oldId)
            treeId[j] = newId;
      }
    }
    int sumWeight = 0;
    int sumInverseMst = 0;
    for (size_t i = 0; i < result.size(); i++) {
      sumWeight += result[i].first;
    }
    for (size_t i = 0; i < (n - 1); i++) {
      sumInverseMst += static_cast<size_t>(output["data"][i][2]);
      /*std::cout << output["data"][i][2] << "     " << output["data"][i][0]
                << "  " << output["data"][i][1] << std::endl;*/
    }
    // std::cout << cost << "cost" << std::endl;
    REQUIRE_EQUAL(n, output["size"]);
    REQUIRE_EQUAL(it, output["id"]);
    REQUIRE_EQUAL("weighted_graph", output["type"]);
    REQUIRE_EQUAL(sumWeight, cost);
  }
}
