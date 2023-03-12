//
// Created by 10901 on 2023/3/12.
//
#include "gtest/gtest.h"
#include <iostream>
#include <set>

#define XXH_INLINE_ALL

#include "xxhash.h"

extern "C" {
__declspec(dllimport) int gen_chains_all(char *words[], int len, char *result[]) noexcept(false);
__declspec(dllimport) int
gen_chain_word(char *words[], int len, char *result[], char head, char tail, bool allow_circ) noexcept(false);
__declspec(dllimport) int gen_chain_word_unique(char *words[], int len, char *result[]) noexcept(false);
__declspec(dllimport) int
gen_chain_char(char *words[], int len, char *result[], char head, char tail, bool enable_loop) noexcept(false);
}
typedef std::set<XXH64_hash_t> word_set_t;

int check_validation(const word_set_t &dictionary, word_set_t &appeared, const char *word_list) {
    // 检查word_list是否合法
    // 由一个字母开头，所有字母均为小写
    // 不存在连续两位的空白符
    // 空白符仅可为 ' '
    // 对于每一个合法单词 检查其是否在词典dictionary中
    // 每一个合法的单词也仅可以出现一次
    word_set_t local_appeared;
    enum {
        IN_WORD,
        WAIT_WORD
    } fsm_state = WAIT_WORD;
    int last_pos = 0;
    char end_char = '\0';
    int pos = 0;
    for (; word_list[pos]; pos++) {
        if (fsm_state == IN_WORD && !isalpha(word_list[pos])) {
            XXH64_hash_t word_hash = XXH64(word_list + last_pos, pos - last_pos, 0x0);
            if (dictionary.find(word_hash) == dictionary.end()) {
                // 单词不在词典dictionary中
                return 1;
            }
            if (local_appeared.find(word_hash) != local_appeared.end()) {
                // 单词多次出现
                return 1;
            }
            if (word_list[pos] != ' ') {
                // 出现非空格的空白符
                return 1;
            }
            local_appeared.insert(word_hash);
            end_char = word_list[pos - 1];
            fsm_state = WAIT_WORD;
        } else if (fsm_state == IN_WORD && isupper(word_list[pos])) {
            // 出现大写字符
            return 1;
        } else if (fsm_state == WAIT_WORD && !isalpha(word_list[pos])) {
            // 出现连续的空白符 或者以空白符开头
            return 1;
        } else if (fsm_state == WAIT_WORD) {
            if (end_char && end_char != word_list[pos]) {
                // 接龙失败
                return 1;
            }
            if (isupper(word_list[pos])) {
                // 出现大写字符
                return 1;
            }
            fsm_state = IN_WORD;
            last_pos = pos;
        }
    }
    if (fsm_state == IN_WORD) {
        XXH64_hash_t word_hash = XXH64(word_list + last_pos, pos - last_pos, 0x0);
        if (dictionary.find(word_hash) == dictionary.end()) {
            // 单词不在词典dictionary中
            return 1;
        }
        if (local_appeared.find(word_hash) != local_appeared.end()) {
            // 单词多次出现
            return 1;
        }
    }
    // 计算合法word_list的hash 检查是否出现过在appeared中 (检查重复单词列)
    XXH64_hash_t chain_hash = XXH64(word_list, pos, 0);
    if (appeared.find(chain_hash) != appeared.end()) {
        return 1;
    }
    appeared.insert(chain_hash);
    return 0;
}

word_set_t build_dictionary(int cnt, char **words) {
    word_set_t dictionary;
    for (int i = 0; i < cnt; i++) {
        dictionary.insert(XXH64(words[i], strlen(words[i]), 0));
    }
    return dictionary;
}

void std_check_gen_chains_all(std::string words_str[], int word_cnt, int cnt) {
    const char *words[word_cnt];
    char **result = new char *[65536];
    result[0] = new char[128 * 1024 * 1024];
    for (int i = 0; i < word_cnt; i++) {
        words[i] = words_str[i].c_str();
    }
    int ret = gen_chains_all(const_cast<char **>(words), word_cnt, result);
    EXPECT_EQ(ret, cnt);
    word_set_t appeared;
    word_set_t dictionary = build_dictionary(word_cnt, const_cast<char **>(words));
    int err_cnt = 0;
    for (int i = 0; i < ret; i++) {
        err_cnt += check_validation(dictionary, appeared, result[i]);
    }
    EXPECT_EQ(err_cnt, 0);
    delete[]result[0];
    delete[]result;
}

void std_check_gen_max(std::string words_str[], int word_cnt, int (*fut)(char *[], int, char *[], char, char, bool),
                       char head, char tail, bool ring, int max) {
    const char *words[word_cnt];
    char **result = new char *[65536];
    result[0] = new char[128 * 1024 * 1024];
    for (int i = 0; i < word_cnt; i++) {
        words[i] = words_str[i].c_str();
    }
    int ret = fut(const_cast<char **>(words), word_cnt, result, head, tail, ring);
    EXPECT_EQ(ret, max);
    word_set_t appeared;
    word_set_t dictionary = build_dictionary(word_cnt, const_cast<char **>(words));
    std::stringstream str_builder;
    for (int i = 0; i < ret; i++) {
        str_builder << result[i];
        if (i != ret - 1) {
            str_builder << ' ';
        }
    }
    int err_cnt = check_validation(dictionary, appeared, str_builder.str().c_str());
    EXPECT_EQ(err_cnt, 0);
    delete[]result[0];
    delete[]result;
}

TEST(gen_chains_all, no_loop_0) {
#include"test_point/no_loop_0.txt"
#include "gen_chains_default.txt"
}

TEST(gen_chains_all, no_loop_1) {
#include"test_point/no_loop_1.txt"
#include "gen_chains_default.txt"
}

TEST(gen_chains_all, loop_0) {
#include"test_point/loop_0.txt"
#include "gen_chains_default.txt"
}

TEST(gen_chain_word, no_loop_0) {
#include"test_point/no_loop_0.txt"
#include "gen_chain_word_default.txt"
}

TEST(gen_chain_word, no_loop_1) {
#include"test_point/no_loop_1.txt"
#include "gen_chain_word_default.txt"
}

TEST(gen_chain_word, loop_0) {
#include"test_point/loop_0.txt"
#include "gen_chain_word_default.txt"
}

TEST(gen_chain_char, no_loop_0) {
#include"test_point/no_loop_0.txt"
#include "gen_chain_char_default.txt"
}

TEST(gen_chain_char, no_loop_1) {
#include"test_point/no_loop_1.txt"
#include "gen_chain_char_default.txt"
}

TEST(gen_chain_char, loop_0) {
#include"test_point/loop_0.txt"
#include "gen_chain_char_default.txt"
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}