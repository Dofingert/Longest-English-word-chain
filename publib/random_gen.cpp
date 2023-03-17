//
// Created by 10901 on 2023/3/12.
//
#include <cstdlib>
#include <algorithm>
#include <cstring>

static unsigned int seed;

int status[1 << 20][20];

int dp(char *words[], int len, char head, char tail, bool weighted) {
    int words_len[20];
    for (int i = 0; i < len; i++) {
        words_len[i] = (int) strlen(words[i]);
    }
    for (int i = 0; i < (1 << len); i++) {
        for (int j = 0; j < len; j++) {
            status[i][j] = (int) -1e9;
        }
    }
    for (int i = 0; i < len; i++) {
        if (!head || words[i][0] == head) {
            status[1ll << i][i] = weighted ? words_len[i] : 1;
        }
    }
    for (int i = 0; i < (1 << len); i++) {
        for (int j = 0; j < len; j++) {
            if (i & (1 << j)) {
                for (int k = 0; k < len; k++) {
                    if (!(i & (1 << k))) {
                        if (words[j][words_len[j] - 1] == words[k][0]) {
                            status[i | (1ll << k)][k] = std::max(status[i | (1ll << k)][k],
                                                                 status[i][j] +
                                                                 (weighted ? words_len[k] : 1));
                        }
                    }
                }
            }
        }
    }
    int ans = 0;
    for (int state = 0; state < (1 << len); state++) {
        for (int j = 0; j < len; j++) {
            if (state & (1 << j)) {
                if (state == (1 << j)) continue;
                if (!tail || words[j][words_len[j] - 1] == tail) ans = std::max(ans, status[state][j]);
            }
        }
    }
    return ans;
}


unsigned int rnd() {
    seed ^= seed << 13;
    seed ^= seed >> 7;
    seed ^= seed << 17;
    return seed;
}

char **generator(int n, bool DAG, int word_cnt, unsigned int Seed, bool is_complete) {
    seed = Seed ^ n ^ word_cnt;
    char **words = (char **) malloc(word_cnt * sizeof(char *));
    words[0] = (char *) malloc(word_cnt * 34);
    char *alloca_space = words[0];
    if (!is_complete) {
        for (int i = 0; i < word_cnt; i++) {
            int len = (int) ((rnd() % 30) + 3);
//        words[i] = (char *) malloc((len + 1ll) * sizeof(char));
            words[i] = alloca_space;
            alloca_space += len + 1;
            if (words[i] != nullptr) {
                words[i][0] = (char) ('a' + rnd() % n);
                words[i][1] = (char) (i % n + 'a');
                for (int j = 2; j < len; j++) words[i][j] = (char) (rnd() % n + 'a');
                if (DAG && words[i][0] >= words[i][len - 1]) {
                    if (words[i][0] == words[i][len - 1]) {
                        if (words[i][0] == n - 1 + 'a') words[i][0]--;
                        else words[i][len - 1]++;
                    } else std::swap(words[i][0], words[i][len - 1]);
                }
                words[i][len] = 0;
            }
        }
    } else {
        int tot = 0;
        while (true) {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    int len = (int) (rnd() % 30) + 3;
//        words[i] = (char *) malloc((len + 1ll) * sizeof(char));
                    words[tot] = alloca_space;
                    alloca_space += len + 1;
                    if (words[tot] != nullptr) {
                        // TODO 可能生成重复的单词
                        words[tot][0] = (char) (i + 'a');
                        words[tot][len - 1] = (char) (j + 'a');
                        for (int k = 1; k < len; k++) words[tot][k] = (char) (rnd() % n + 'a');
                        words[tot][len] = 0;
                    }
                    if (--word_cnt == 0) {
                        return words;
                    }
                    ++tot;
                }
            }
        }

    }

    return words;
}