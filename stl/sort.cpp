//
// Created by zhuanxu on 2018/6/6.
//

#include <vector>
#include <iostream>
#include <algorithm>
#include <memory>
#include <random>
#include <chrono>
#include <map>
#include <unordered_map>

using Clock = std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::microseconds;

using namespace std;


class node {
public:
    double v_;

    node(double v) : v_(v) {}
};

typedef std::shared_ptr<node> nodeWrapper;

struct StgNodeWrapper {
    nodeWrapper index_node;
    std::string name;
    uint64_t id;
    double score;
    int filter_code;

    StgNodeWrapper() : score(0), filter_code(0) {
    }

    StgNodeWrapper(nodeWrapper index, const std::string t, double score)
            : index_node(std::move(index)), name(t), id(0), score(score), filter_code(0) {
    }
};

double get_random() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<double> dis(1.0, 100.0);
    return dis(gen);
}

int get_int() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dis(0, 52);
    return dis(gen);
}


int main() {
    std::array<std::string, 52> alapha;
    for (char c = 'a'; c <= 'z'; c++) {
        alapha[c - 'a'] = c;
    }
    for (char c = 'A'; c <= 'Z'; c++) {
        alapha[c - 'A' + 26] = c;
    }

    std::vector<StgNodeWrapper> result;
    std::vector<StgNodeWrapper *> ptr_result;
    int result_num = 4000;
    for (int i = 0; i < result_num; i++) {
        result.emplace_back(StgNodeWrapper(make_shared<node>(1.0), alapha[get_int()], get_random()));
        ptr_result.emplace_back(&result[result.size() - 1]);
    }
    const auto t1 = Clock::now();
    std::sort(result.begin(), result.end(), [](const StgNodeWrapper &lhs, const StgNodeWrapper &rhs) {
        return lhs.score > rhs.score;
    });
    const auto t2 = Clock::now();
    const Clock::rep dt = duration_cast<microseconds>(t2 - t1).count();
    printf("std::sort(value) %lu us\n", dt);

    {
        const auto t1 = Clock::now();
        std::sort(ptr_result.begin(), ptr_result.end(), [](const StgNodeWrapper *lhs, const StgNodeWrapper *rhs) {
            return lhs->score > rhs->score;
        });
        const auto t2 = Clock::now();
        const Clock::rep dt = duration_cast<microseconds>(t2 - t1).count();
        printf("std::sort(ptr) %lu us\n", dt);
    }

    {
        const auto t1 = Clock::now();
        std::map<std::string, std::vector<StgNodeWrapper *>> term_cont_list;
        for (int i = 0; i < result_num; ++i) {
            if (term_cont_list.find(result[i].name) == term_cont_list.end()) {
                term_cont_list.emplace(result[i].name, std::vector<StgNodeWrapper *>());
            }
            term_cont_list[result[i].name].emplace_back(&result[i]);
        }
        const auto t2 = Clock::now();
        const Clock::rep dt = duration_cast<microseconds>(t2 - t1).count();
        printf("map(ptr) %lu us\n", dt);
    }
    {
        const auto t1 = Clock::now();
        std::unordered_map<std::string, std::vector<StgNodeWrapper *>> term_cont_list;
        for (int i = 0; i < result_num; ++i) {
            if (term_cont_list.find(result[i].name) == term_cont_list.end()) {
                term_cont_list.emplace(result[i].name, std::vector<StgNodeWrapper *>());
            }
            term_cont_list[result[i].name].emplace_back(&result[i]);
        }
        const auto t2 = Clock::now();
        const Clock::rep dt = duration_cast<microseconds>(t2 - t1).count();
        printf("unordered_map(ptr) %lu us\n", dt);
    }
    std::unordered_map<std::string, std::vector<StgNodeWrapper *>> term_cont_list;
    for (int i = 0; i < result_num; ++i) {
        if (term_cont_list.find(result[i].name) == term_cont_list.end()) {
            term_cont_list.emplace(result[i].name, std::vector<StgNodeWrapper *>());
        }
        term_cont_list[result[i].name].emplace_back(&result[i]);
    }


//        const auto t1 = Clock::now();
    int max_term_size = std::numeric_limits<int>::min();
    int min_term_size = std::numeric_limits<int>::max();
    std::vector<std::string> valid_term; // 包含有数据的term
    valid_term.resize(52);
    for (int i = 0; i < 52; i++) {
        const std::string &name = alapha[i];
        if (term_cont_list.find(name) != term_cont_list.end()) {
            max_term_size = std::max(max_term_size, static_cast<int>(term_cont_list[name].size()));
            min_term_size = std::min(min_term_size, static_cast<int>(term_cont_list[name].size()));
            valid_term.emplace_back(name);
        }
    }
//        const auto t2 = Clock::now();
//        const Clock::rep dt = duration_cast<microseconds>(t2 - t1).count();
//        printf("find(ptr) %lu us\n", dt);
    {
        const auto t1 = Clock::now();
        int z_recall_num = 0;
        std::vector<StgNodeWrapper> ret_result;
        result_num = 2000;
        ret_result.reserve(static_cast<unsigned long>(result_num));
//        cout << ret_result.size() << endl;
        for (int i = 0; i < max_term_size; i++) {
            if (z_recall_num >= result_num) {
                break;
            }
            for (int j = 0; j < valid_term.size(); j++) {
                if (z_recall_num >= result_num) {
                    break;
                }
                const std::string &term =  valid_term[j];
                if (term_cont_list[term].size() > i) {
                    ret_result.emplace_back(std::move(*(term_cont_list[term][i])));
                    z_recall_num++;
                }
            }
        }
//        cout << ret_result.size() << endl;
        result.swap(ret_result);
        const auto t2 = Clock::now();
        const Clock::rep dt = duration_cast<microseconds>(t2 - t1).count();
//        cout << result.size() << endl;
        printf("z_recall(ptr) %lu us\n", dt);
    }
    {
        const auto t1 = Clock::now();
        int z_recall_num = 0;
        std::vector<StgNodeWrapper> ret_result;
        result_num = 2000;
        ret_result.reserve(static_cast<unsigned long>(result_num));
        std::vector<bool> valid_bit(valid_term.size(),true);
        for (int i = 0; i < max_term_size; i++) {
            if (z_recall_num >= result_num) {
                break;
            }
            for (int j = 0; j < valid_term.size(); j++) {
                if (z_recall_num >= result_num) {
                    break;
                }
                const std::string &term =  valid_term[j];
                if (valid_bit[j] && term_cont_list[term].size() > i) {
                    ret_result.emplace_back(std::move(*(term_cont_list[term][i])));
                    z_recall_num++;
                }else {
                    valid_bit[j] = false;
                }
            }
        }
        result.swap(ret_result);
        const auto t2 = Clock::now();
        const Clock::rep dt = duration_cast<microseconds>(t2 - t1).count();
        printf("z_recall_continue(ptr) %lu us\n", dt);
    }
}

