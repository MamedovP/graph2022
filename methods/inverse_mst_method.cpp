/**
 * @file methods/inverse_mst_method.cpp
 * @author Polad Mamedov
 *
 * Файл содержит функцию, которая вызывает алгоритм обратной задачи минимального остова.
 * Функция принимает и возвращает данные в JSON формате.
 */

#include <string>
#include <nlohmann/json.hpp>
#include "inverse_mst.hpp"

namespace graph {

template<typename T>
static int InverseMstMethodHelper(const nlohmann::json& input,
                                  nlohmann::json* output,
                                  std::string type);

int InverseMstMethod(const nlohmann::json& input, nlohmann::json* output) {
    /* 
    С классом nlohmann::json можно работать как со словарём.
    Метод at() в отличие оператора [] не меняет объект, поэтому
    этот метод можно использовать с константными объектами. 
    */
    std::string type = input.at("type");
    std::string typeWeight = input.at("typeWeights");
    

    if (type == "weighted_graph") {
        if (typeWeight == "int") {
            return InverseMstMethodHelper<int>(
                input, output, type);
        } else if (typeWeight == "float") {
            return InverseMstMethodHelper<float>(
                input, output, type);
        } else if (typeWeight == "double") {
            return InverseMstMethodHelper<double>(
                input, output, type);
        } else if (typeWeight == "long double") {
            return InverseMstMethodHelper<long double>(
                input, output, type);
        }
    }
    return -1;
}
/**
 * @brief Метод алгоритма обратной задачи минимального остова.
 * 
 * @tparam T Тип данных весов рёбер графа.
 * @param input Входные данные в формате JSON.
 * @param output Выходные данные в формате JSON.
 * @param type Строковое представление типа.
 * @return Функция возвращает 0 в случае успеха и отрицательное число,
 * если входные данные заданы некорректно.
 * 
 * Функция запускает алгоритм Inverse-MST, используя входные данные
 * в JSON формате. Результат также выдаётся в JSON формате. Функция
 * используется для сокращения кода, необходимого для поддержки различных типов данных. 
 */
template<typename T>
static int InverseMstMethodHelper(const nlohmann::json& input,
                                  nlohmann::json* output, 
                                  std::string type) {
    size_t size = input.at("size");
    (*output)["id"] = input.at("id");
    (*output)["size"] = size;
    (*output)["type"] = type;
    size_t numEdges = input.at("numEdges");
    if (numEdges == 0) {
        return -1;
    }
    WeightedGraph<T> wGraph;
    for (size_t i = 0; i < size; i++) {
        /* Для словарей используется индекс в виде строки,
        а для массивов просто целое число типа size_t. */
        size_t v = input.at("vertices").at(i);
        if (v != 0) {
            wGraph.AddVertex(v);
        }
    }
    // Добавляем ребра
    for (size_t j = 0; j < numEdges; j++) {
        wGraph.AddEdge(input.at("edges").at(j).at(0),
                      input.at("edges").at(j).at(1),
                      input.at("edges").at(j).at(2));
    }
    /* Здесь вызывается сам алгоритм. */
    InverseMst(wGraph);    
    return 0;
}

} // namespace graph
