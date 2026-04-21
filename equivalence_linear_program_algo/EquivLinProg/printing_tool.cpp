#include <iostream>
#include "printing_tool.h"



void print_triplet_list(std::vector<Eigen::Triplet<double>> tripletList, std::string name) {
    std::cout << name << " = ";

    for (int i = 0; i < tripletList.size(); i++) {
        std::cout << tripletList[i].row() << ", " << tripletList[i].col() << ", " << tripletList[i].value() << "|";
    }
    std::cout << std::endl;

}

void print_vector_of_pair(std::vector<std::vector<std::pair<int,int>>>& incom_couples) {
    for (int i = 0; i < incom_couples.size(); ++i) {
        print_pair(incom_couples[i], "incom_couples["+std::to_string(i));
    }
}

void print_matrix(std::vector<std::vector<int>> matrix,  std::string  name) {

    std::cout << name << " = [ ";

    for (int i = 0; i < matrix.size(); ++i)
    {
        std::cout <<  " [ ";
        for (int j = 0; j < matrix[i].size(); ++j)
        {
            std::cout << matrix[i][j] ;

            if (j <  matrix[i].size()-1) std::cout << ", ";
        }

        std::cout <<  " ] ";

        if (i <  matrix.size()-1) std::cout << ", ";
    }
    std::cout <<  " ] " << std::endl;
}

void print_vector(std::vector<int> vector,  std::string  vector_name) {
    std::cout << vector_name << " = [ ";
    for (unsigned i = 0; i < vector.size(); ++i) {
        std::cout << vector[i] ;
        if (i < vector.size()-1) std::cout << ", ";
    }
    std::cout << "] " << std::endl;
}

void print_vector(std::vector<double> vector,  std::string  vector_name) {
    std::cout << vector_name << " = [ ";
    for (unsigned i = 0; i < vector.size(); ++i) {
        std::cout << vector[i] ;
        if (i < vector.size()-1) std::cout << ", ";
    }
    std::cout << "] " << std::endl;
}

void print_tuple(std::vector<std::tuple<int, int, int>> tree, std::string  tuple_name) {
    std::cout << tuple_name <<   " = [ ";
    for (unsigned i=0; i<tree.size(); ++i) {
        std::cout << "(" << std::get<0>(tree[i]) << ", " << std::get<1>(tree[i]) << ", " << std::get<2>(tree[i]) << ") ";
        if (i < tree.size()-1) std::cout << ", ";
    }
    std::cout <<  " ] " << std::endl;
}

void print_tuple_4(std::vector<std::tuple<int, int, int, int>> tree, std::string  tuple_name) {
    std::cout << tuple_name <<   " = [ ";
    for (unsigned i=0; i<tree.size(); ++i) {
        std::cout << "(" << std::get<0>(tree[i]) << ", " << std::get<1>(tree[i]) << ", " << std::get<2>(tree[i]) << ", " << std::get<3>(tree[i]) << ") ";
        if (i < tree.size()-1) std::cout << ", ";
    }
    std::cout <<  " ] " << std::endl;
}

void print_pair(std::vector<std::pair<int, int>> tree, std::string  tuple_name) {
    std::cout << tuple_name <<   " = [ ";
    for (unsigned i=0; i<tree.size(); ++i) {
        std::cout << "(" << std::get<0>(tree[i]) << ", " << std::get<1>(tree[i])  << ") ";
        if (i < tree.size()-1) std::cout << ", ";
    }
    std::cout <<  " ] " << std::endl;
}

void print_pair_without_equal(std::vector<std::pair<int, int>> tree, std::string  tuple_name) {
    std::cout << tuple_name <<   " [ ";
    for (unsigned i=0; i<tree.size(); ++i) {
        std::cout << "(" << std::get<0>(tree[i]) << ", " << std::get<1>(tree[i])  << ") ";
        if (i < tree.size()-1) std::cout << ", ";
    }
    std::cout <<  " ] " ;
}

void print_dict(std::unordered_map<std::pair<int,int>, std::vector<std::pair<int,int>>, PairHash> learned_wrong_scaling, std::string  dict_name) {

    std::cout << dict_name <<   " = { ";
    int i(0);
    for (auto& pair : learned_wrong_scaling) {
        std::cout << "(" << pair.first.first << ", " << pair.first.second  << "): ";
        print_pair_without_equal(pair.second, "");
        if (i < learned_wrong_scaling.size()-1) std::cout << ", ";
        i ++;
    }
    std::cout <<  " } " << std::endl;
}

void print_pair(std::vector<std::pair<int, double>> tree, std::string  tuple_name) {
    std::cout << tuple_name <<   " = [ ";
    for (unsigned i=0; i<tree.size(); ++i) {
        std::cout << "(" << std::get<0>(tree[i]) << ", " << std::get<1>(tree[i])  << ") ";
        if (i < tree.size()-1) std::cout << ", ";
    }
    std::cout <<  " ] " << std::endl;
}

void print_vector_of_vectors_of_tuples(std::vector<std::vector<std::tuple<int, int, int>>> allChildren, std::string  name) {
    std::cout << name <<   " = [ ";
    for (unsigned i=0; i<allChildren.size(); ++i) {
        std::cout << " [ ";
        for (unsigned j=0; j <allChildren[i].size(); ++j) {

            std::cout << "(" << std::get<0>(allChildren[i][j]) << ", " << std::get<1>(allChildren[i][j]) << ", " << std::get<2>(allChildren[i][j])  << ") ";

            if (j < allChildren[i].size()-1) std::cout << ", ";
        }
        std::cout <<  " ] ";
        if (i < allChildren.size()-1) std::cout << ", ";
    }
    std::cout <<  " ] " << std::endl;
}

// Created by Admin on 2025-10-08.
