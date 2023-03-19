//
// Created by Surface on 2023/3/8.
//
#include <bits/stdc++.h>
#include "core.h"
#include "phmap.h"
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cassert>
#include <map>

using namespace std;
typedef __int128 state_t;

const int MAX_ANS_CNT = 20000;
const int INF = 0x3f3f3f3f;
const int MAX_N = 26;

template<>
struct std::hash<pair<state_t, int>> {
    size_t operator()(const pair<state_t, int> &k) const {
        return hash<state_t>()(k.first) ^ hash<int>()(k.second);
    }
};

struct ComputeUnit {

    vector<string> word_list;

    vector<string> ans;
    vector<int> ans_vec;

    vector<int> ver[MAX_N + 5];
    vector<int> scc[MAX_N + 5];
    int my_stack[MAX_N + 5]{}, top = 0;
    bool ins[MAX_N + 5]{}, visit[MAX_N + 5]{};
    int dfn[MAX_N + 5]{}, low[MAX_N + 5]{}, cnt = 0;
    int scc_col[MAX_N + 5]{}, scc_tot = 0;


    vector<int> dfs_stack;

    void word_preprocessing(char *words[], int len, char jail) {
        std::set<std::string> dictionary;
        for (int i = 0; i < len; i++) {
            string word = words[i];
            transform(word.begin(), word.end(), word.begin(), ::tolower);
            if (word[0] == jail) {
                continue;
            }
            if (dictionary.find(word) == dictionary.end()) {
                word_list.push_back(word);
                dictionary.insert(word);
            }
        }
        sort(word_list.begin(), word_list.end());
    }


    void dfs_SCC(int x) {
        my_stack[++top] = x, visit[x] = ins[x] = true;
        dfn[x] = low[x] = ++cnt;
        for (int j: ver[x]) {
            int to = word_list[j].back() - 'a';
            if (visit[to]) {
                if (ins[to]) {
                    low[x] = min(low[x], low[to]);
                }
            } else {
                dfs_SCC(to);
                low[x] = min(low[x], low[to]);
            }
        }
        if (dfn[x] == low[x]) {
            ++scc_tot;
            int y;
            do {
                y = my_stack[top--];
                ins[y] = false;
                scc_col[y] = scc_tot;
                scc[scc_tot].push_back(y);
            } while (y != x);
        }
    }

    void get_SCC() {
        for (int i = 0; i < MAX_N; i++) {
            visit[i] = false;
        }
        cnt = scc_tot = 0;
        for (int i = 0; i < word_list.size(); i++) {
            ver[word_list[i][0] - 'a'].push_back(i);
        }
        for (int i = 0; i < MAX_N; i++) {
            if (!visit[i]) {
                dfs_SCC(i);
            }
        }
    }

    void check_loop() {
        // 检查是否有一个点有至少两个自环
        for (int i = 0; i < 26; i++) {
            int last = -1;
            for (int j: ver[i]) {
                if (word_list[j].back() - 'a' == i) {
                    if (last == -1) {
                        last = j;
                    } else {
                        throw logic_error(
                                "Word ring detected, at least two self ring on one node!");
                    }
                }
            }
        }
        if (scc_tot < 26) {
            throw logic_error("Word ring detected, at least one scc has more than two nodes!");
        }
    }

    void dfs_all_word_chain(int i, bool loop) {
        if (dfs_stack.size() > 1) {
            if (ans.size() < MAX_ANS_CNT) {
                stringstream S;
                for (int j: dfs_stack) {
                    S << word_list[j] << ' ';
                }
                ans.push_back(S.str());
            } else {
                throw logic_error("Too many word chains!");
            }
        }
        for (int j: ver[i]) {
            int to = word_list[j].back() - 'a';
            if (i == to && loop) {
                continue; // 每个单词至多用一次
            }
            dfs_stack.push_back(j);
            dfs_all_word_chain(to, i == to);
            dfs_stack.pop_back();
        }
    }

    void write_ans_to_mem(char *result[], void *out_malloc(size_t)) {
        // 计算并分配 ans 的 size
        int ans_size = 0;
        for (auto &i: ans) {
            ans_size += (int) i.length() + 1;
        }
        char *p = (char *) out_malloc((size_t) ans_size + 1);
        // 写入答案
        for (int i = 0; i < ans.size(); i++) {
            result[i] = p;
            for (char j: ans[i]) {
                if (p != nullptr) {
                    *p = j;
                }
                p++;
            }
            if (p != nullptr) {
                *p = 0;
            }
            p++;
        }
    }

    int get_all_chains(char *result[], void *out_malloc(size_t)) {
        for (int i = 0; i < MAX_N; i++) {
            dfs_all_word_chain(i, false);
        }
        int len = (int) ans.size();
        write_ans_to_mem(result, out_malloc);
        return len;
    }


    int get_longest_chain_on_DAG(char *result[], char head, char tail, bool weighted, void *out_malloc(size_t)) {
        // status 记录 dp 值，topology 记录拓扑序，from 记录 status 是从之后哪个单词转移来的，self_from 记录 status 是否是从自环转移来的
        int topology[MAX_N + 5] = {}, f[MAX_N + 5] = {}, from[MAX_N + 5] = {}, self_from[MAX_N + 5] = {};
        for (int i = 0; i < MAX_N; i++) {
            topology[scc_col[i] - 1] = i; // topological 序
        }
        for (int i = 0; i < MAX_N; i++) {
            int x = topology[i];
            f[x] = (!tail || tail - 'a' == x) ? 0 : -INF;
            from[x] = self_from[x] = -1;
            for (int j: ver[x]) {
                int to = word_list[j].back() - 'a';
                if (to == x) {
                    continue;
                }
                int val = f[to] + (weighted ? (int) word_list[j].length() : 1);
                if (val > f[x]) {
                    f[x] = val;
                    from[x] = j;
                }
            }
            for (auto j: ver[x]) {
                int to = word_list[j].back() - 'a';
                if (to == x) {
                    f[x] += weighted ? (int) word_list[j].length() : 1;
                    self_from[x] = j;
                    break;
                }
            }
        }
        // 枚举每一条边作为起始边的情况
        int pos = -1, max_sum = -INF;
        for (int i = 0; i < word_list.size(); i++) {
            if (!head || head == word_list[i][0]) {
                int u = word_list[i][0] - 'a', v = word_list[i].back() - 'a';
                if (u == v) {
                    // 对于自环，需要判断 status[u] 是否只有自环，因为单词链长度大于1，单独一个自环无法构成单词链
                    if (f[u] <= 0 || f[u] == (weighted ? (int) word_list[i].length() : 1)) {
                        continue;
                    }
                    if (max_sum < f[u]) {
                        max_sum = f[u];
                        pos = i;
                    }
                } else {
                    if (f[v] <= 0) {
                        continue;
                    }
                    int tmp_sum = f[v] + (weighted ? (int) word_list[i].length() : 1);
                    if (max_sum < tmp_sum) {
                        max_sum = tmp_sum;
                        pos = i;
                    }
                }
            }
        }
        if (pos != -1) {
            if (word_list[pos][0] == word_list[pos].back()) {
                pos = word_list[pos][0] - 'a';
            } else {
                ans.push_back(word_list[pos]);
                pos = word_list[pos].back() - 'a';
            }
            while (true) {
                if (self_from[pos] != -1) {
                    ans.push_back(word_list[self_from[pos]]);
                }
                if (from[pos] == -1) {
                    break;
                }
                ans.push_back(word_list[from[pos]]);
                pos = word_list[from[pos]].back() - 'a';
            }
        }
        int len = (int) ans.size();
        write_ans_to_mem(result, out_malloc);
        return len;
    }

    mutex mp_lock;
//    unordered_map
    phmap::flat_hash_map
            <pair<state_t, int>, pair<int, int> > mp; // tuple 中的两个 ll 组成 int128 表示 State， int 表示 i； pair<int, int> 表示 ans, from

    vector<pair<int, int> > edge_w[26][26]; // edge_w[i][j] 中存起点为 i，终点为 j 的边的 pair<边权，边id>
    vector<int> Ver[26]; // Ver[i] === ver[i]
    int global_tail = 0;
    struct thread_context_t {
        state_t global_state{0}; // global_state 维护当前 dfs 到的 State，模拟 int128
        int w_used[26][26]{0}; // 维护 edge_w[i][j] 中当前已经在 State 中的边数
    };

    static void modify_global_state(state_t &global_state, int i) {
        global_state |= (((state_t) 1) << i);
    }

    int dfs_state(thread_context_t *thread_context, int i) {
        // 记忆化搜索
        {
            auto const mem_stat = mp.find(make_pair(thread_context->global_state, i));
            if (mem_stat != mp.end()) {
                return mem_stat->second.first;
            }
        }
        int ret = (global_tail == -1 || i == global_tail) ? 0 : -INF;
        int id = -1;
        state_t global_state_bk = thread_context->global_state;
        if (thread_context->w_used[i][i] < edge_w[i][i].size()) {
            // 先选自环
            modify_global_state(thread_context->global_state, edge_w[i][i][thread_context->w_used[i][i]].second);
            thread_context->w_used[i][i]++;
            int dfs_result = dfs_state(thread_context, i);
//            auto const mem_stat = mp.find(make_pair(thread_context->global_state, i));
            ret = dfs_result +
                  edge_w[i][i][thread_context->w_used[i][i] - 1ll].first;
            id = i;
            // 还原现场
            thread_context->global_state = global_state_bk;
            thread_context->w_used[i][i]--;
        } else {
            // 此时i点的自环已经全部选完，开始走i点的出边继续扩展状态
            for (int j: Ver[i]) {
                if (thread_context->w_used[i][j] < edge_w[i][j].size()) {
                    if (scc_col[i] == scc_col[j]) {
                        // 若在同一个强连通分量中
                        modify_global_state(thread_context->global_state,
                                            edge_w[i][j][thread_context->w_used[i][j]].second);
                    } else {
                        // 若进入一个新的强连通分量，则清零State
                        thread_context->global_state = 0;
                    }
                    thread_context->w_used[i][j]++;
                    int dfs_result = dfs_state(thread_context, j); // 由于 dfs_max结束后一定会把val还原成val_bk，因此dfs_max结束后val还是0
                    int sum =
                            dfs_result +
                            edge_w[i][j][thread_context->w_used[i][j] - 1ll].first; // 为什么减去1ll？
                    if (ret < sum) {
                        ret = sum;
                        id = j;
                    }
                    // 还原现场
                    thread_context->global_state = global_state_bk;
                    thread_context->w_used[i][j]--;
                }
            }
        }
//        unique_lock<mutex> local_mp_lock(mp_lock);
        mp[make_pair(thread_context->global_state, i)] = {ret, id};
//        local_mp_lock.unlock();
        return ret;
    }

    int get_longest_chain(char *result[], char head, char tail, bool weighted, void *out_malloc(size_t)) {
        for (int i = 0; i < word_list.size(); i++) {
            edge_w[word_list[i][0] - 'a'][word_list[i].back() - 'a'].emplace_back(
                    weighted ? (int) word_list[i].length() : 1, i);
            Ver[word_list[i][0] - 'a'].push_back(word_list[i].back() - 'a');
        }
        for (auto &i: edge_w) {
            for (auto &j: i) {
                sort(j.begin(), j.end(), greater<>());
            }
        }
        for (auto &i: Ver) {
            sort(i.begin(), i.end());
            i.erase(unique(i.begin(), i.end()), i.end());
        }
        // 记忆化搜索
        global_tail = tail ? tail - 'a' : -1;
//#pragma omp parallel for
        for (int i = 0; i < 26; i++) {
            if (!head || head - 'a' == i) {
                auto thread_context = new thread_context_t;
                // 由于记忆化搜索，因此遍历的起始顺序不再重要
                dfs_state(thread_context, i);
                delete thread_context;
            }
        }
        auto local_context = new thread_context_t;
        // 枚举起始边，枚举答案
        int x = -1, sum = -INF;
        for (int i = 0; i < word_list.size(); i++) {
            if (!head || head == word_list[i][0]) {
                int u = word_list[i][0] - 'a', v = word_list[i].back() - 'a';
                local_context->global_state = 0;
                if (scc_col[u] == scc_col[v]) {
                    modify_global_state(local_context->global_state, i);
                }
                if (mp[make_pair(local_context->global_state, v)].first <= 0) {
                    continue;
                }
                int Sum = mp[make_pair(local_context->global_state, v)].first +
                          (weighted ? (int) word_list[i].length() : 1);
                if (sum < Sum) {
                    sum = Sum;
                    x = i;
                }
            }
        }
        // 回溯答案
        local_context->global_state = 0;
        if (x != -1) {
            ans.push_back(word_list[x]);
            if (scc_col[word_list[x][0] - 'a'] == scc_col[word_list[x].back() - 'a']) {
                modify_global_state(local_context->global_state, x);
                local_context->w_used[word_list[x][0] - 'a'][word_list[x].back() - 'a']++;
            }
            x = word_list[x].back() - 'a';
            while (true) {
                int j = mp[make_pair(local_context->global_state, x)].second;
                if (j == -1) {
                    break;
                }
                ans.push_back(word_list[edge_w[x][j][local_context->w_used[x][j]].second]);
                if (scc_col[x] == scc_col[j]) {
                    modify_global_state(local_context->global_state, edge_w[x][j][local_context->w_used[x][j]].second);
                } else {
                    local_context->global_state = 0;
                }
                local_context->w_used[x][j]++;
                x = j;
            }
        }
        delete local_context;
        int len = (int) ans.size();
        write_ans_to_mem(result, out_malloc);
        return len;
    }

};

int gen_chains_all(char *words[], int len, char *result[], void *out_malloc(size_t)) {
    auto *context = new ComputeUnit;
    try {
        context->word_preprocessing(words, len, 0);
        context->get_SCC();
        context->check_loop();
        int ret = context->get_all_chains(result, out_malloc);
        delete context;
        return ret;
    } catch (exception &e) {
        delete context;
        throw logic_error(e.what());
    }
}

int gen_chain_word(char *words[], int len, char *result[], char head, char tail, char jail, bool enable_loop,
                   void *out_malloc(size_t)) {
    auto *context = new ComputeUnit;
    try {
        int ret;
        if (!enable_loop) {
            context->word_preprocessing(words, len, 0);
            context->get_SCC();
            context->check_loop();
            if (jail) {
                delete context;
                context = new ComputeUnit;
                context->word_preprocessing(words, len, jail);
                context->get_SCC();
                ret = context->get_longest_chain_on_DAG(result, head, tail, false, out_malloc);
            } else {
                ret = context->get_longest_chain_on_DAG(result, head, tail, false, out_malloc);
            }
        } else {
            if (len > 128) {
                throw logic_error("Too many words, support up to 128 words!");
            }
            context->word_preprocessing(words, len, jail);
            context->get_SCC();
            ret = context->get_longest_chain(result, head, tail, false, out_malloc);
        }
        delete context;
        return ret;
    } catch (exception &e) {
        delete context;
        throw logic_error(e.what());
    }
}

int gen_chain_char(char *words[], int len, char *result[], char head, char tail, char jail, bool enable_loop,
                   void *out_malloc(size_t)) {
    auto *context = new ComputeUnit;
    try {
        int ret;
        if (!enable_loop) {
            context->word_preprocessing(words, len, 0);
            context->get_SCC();
            context->check_loop();
            if (jail) {
                delete context;
                context = new ComputeUnit;
                context->word_preprocessing(words, len, jail);
                context->get_SCC();
                ret = context->get_longest_chain_on_DAG(result, head, tail, true, out_malloc);
            } else {
                ret = context->get_longest_chain_on_DAG(result, head, tail, true, out_malloc);
            }
        } else {
            if (len > 128) {
                throw logic_error("Too many words, support up to 128 words!");
            }
            context->word_preprocessing(words, len, jail);
            context->get_SCC();
            ret = context->get_longest_chain(result, head, tail, true, out_malloc);
        }
        delete context;
        return ret;
    } catch (exception &e) {
        delete context;
        throw logic_error(e.what());
    }
}