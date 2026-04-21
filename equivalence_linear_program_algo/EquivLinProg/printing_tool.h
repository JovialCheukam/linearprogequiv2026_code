//
// Created by Admin on 2025-10-08.
//

#ifndef EQUIVLINPROG_PRINTING_TOOL_H
#define EQUIVLINPROG_PRINTING_TOOL_H
#include <vector>
#include <Eigen/Sparse>
#include "pairhash.h"

void print_triplet_list(std::vector<Eigen::Triplet<double>> tripletList, std::string name);
void print_vector_of_pair(std::vector<std::vector<std::pair<int,int>>>& incom_couples);
void print_matrix(std::vector<std::vector<int>> matrix,  std::string  name);
void print_vector(std::vector<int> vector,  std::string  vector_name);
void print_vector(std::vector<double> vector,  std::string  vector_name);
void print_tuple(std::vector<std::tuple<int, int, int>> tree, std::string  tuple_name);
void print_pair(std::vector<std::pair<int, int>> tree, std::string  tuple_name);
void print_vector_of_vectors_of_tuples(std::vector<std::vector<std::tuple<int, int, int>>> allChildren, std::string  name);
void print_pair(std::vector<std::pair<int, double>> tree, std::string  tuple_name);
void print_tuple_4(std::vector<std::tuple<int, int, int, int>> tree, std::string  tuple_name);
void print_dict(std::unordered_map<std::pair<int,int>, std::vector<std::pair<int,int>>, PairHash> learned_wrong_scaling, std::string  dict_name);

#endif //EQUIVLINPROG_PRINTING_TOOL_H