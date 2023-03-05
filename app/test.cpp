#include "src/hash_map.h"
#include "timer.h"
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <map>
#include <iomanip>
#include <unordered_map>
#include <fstream>

struct Query {
    int type;
    int key, value;

    Query(int type, int key, int value = 0) : type(type), key(key), value(value) {};
};

template<class HashTable>
double hash_map_time(const char *file_name) {
    freopen(file_name, "r", stdin);
    std::vector<Query> queries;
    int n = 0;
    std::cin >> n;
    for (int i = 0; i < n; ++i) {
        char type = '\0';
        std::cin >> type;
        if (type == '?') {
            int key = 0;
            std::cin >> key;
            queries.push_back(Query(0, key));
        }
        if (type == '+') {
            int key = 0;
            std::cin >> key;
            int value = 0;
            std::cin >> value;
            queries.push_back(Query(1, key, value));
        }
        if (type == '-') {
            int key = 0;
            std::cin >> key;
            queries.push_back(Query(2, key));
        }
    }
    HashTable table;
    int start = clock();
    for (int i = 0; i < queries.size(); ++i) {
        if (queries[i].type == 0) {
            table.find(queries[i].key);
        }
        if (queries[i].type == 1) {
            table.insert({queries[i].key, queries[i].value});
        }
        if (queries[i].type == 2) {
            table.erase(queries[i].key);
        }
    }
    int time = clock() - start;
    return static_cast<double>(time) / CLOCKS_PER_SEC;
}

void gen_test(const char *file_name, int n) {
    std::ofstream fout(file_name);
    fout << n << '\n';
    for (int i = 0; i < n; ++i) {
        int type = rand() % 3;
        if (type == 0) {
            fout << "? ";
            int key = rand();
            fout << key << "\n";
        }
        if (type == 1) {
            fout << "+ ";
            int key = rand();
            int value = rand();
            fout << key << " " << value << "\n";
        }
        if (type == 2) {
            fout << "- ";
            int key = rand();
            fout << key << "\n";
        }
    }
    fout.close();
}

void print_results(const char *file_name) {
    std::cout << std::fixed << std::setprecision(10) << "hash_map_time " << hash_map_time<HashMap<int, int> >
            (file_name)
              << " seconds\n";
    std::cout << std::fixed << std::setprecision(10) << "unordered_map_time "
              << hash_map_time<std::unordered_map<int, int> >(file_name)
              << " seconds\n";
}

void speed_test() {
    std::cout << "small_test\n";
    gen_test("small_test.txt", 1e5);
    std::cout << "Input generated" << std::endl;
    print_results("small_test.txt");
    std::cout << "\n\n";
    std::cout << "middle_test\n";
    gen_test("middle_test.txt", 1e7);
    std::cout << "Input generated" << std::endl;
    print_results("middle_test.txt");
    std::cout << "\n\n";
    std::cout << "large_test\n";
    gen_test("large_test.txt", 1e8);
    std::cout << "Input generated" << std::endl;
    print_results("large_test.txt");
}

int main() {
    speed_test();
    return 0;
}