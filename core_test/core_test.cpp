#include "gtest/gtest.h"
#include <iostream>
#include <core.h>
#include <testlib.h>

void std_check_gen_chains_all(std::string words_str[], int word_cnt, int cnt) {
    const char *words[word_cnt];
    char **result = (char **) malloc(sizeof(char *) * 65536);
    for (int i = 0; i < word_cnt; i++) {
        words[i] = words_str[i].c_str();
    }
    int ret = gen_chains_all(const_cast<char **>(words), word_cnt, result, malloc);
    EXPECT_EQ(ret, cnt);
    word_set_t appeared;
    word_set_t dictionary = build_dictionary(word_cnt, const_cast<char **>(words));
    int err_cnt = 0;
    for (int i = 0; i < ret; i++) {
        err_cnt += check_validation(dictionary, appeared, result[i], -1);
    }
    EXPECT_EQ(err_cnt, 0);
    if (ret != 0) {
        free(result[0]);
    }
    free(result);
}

void
std_check_gen_max(std::string words_str[], int word_cnt,
                  int (*fut)(char *[], int, char *[], char, char, char, bool, void *(*)(size_t)),
                  char head, char tail, char jail, bool ring, int max) {
    const char *words[word_cnt];
    char **result = (char **) malloc(sizeof(char *) * 65536);
    result[0] = new char[128 * 1024 * 1024];
    for (int i = 0; i < word_cnt; i++) {
        words[i] = words_str[i].c_str();
    }
    int ret = fut(const_cast<char **>(words), word_cnt, result, head, tail, jail, ring, malloc);
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
    int err_cnt = check_validation(dictionary, appeared, str_builder.str().c_str(), -1);
    EXPECT_EQ(err_cnt, 0);
    if (ret != 0) {
        free(result[0]);
    }
    free(result);
}

TEST(gen_chains_all, no_loop_0) {
#include"test_point/no_loop_0.txt"
#include "gen_chains_default.txt"
}

TEST(gen_chain_word, no_loop_0) {
#include"test_point/no_loop_0.txt"
#include "gen_chain_word_default.txt"
}

TEST(gen_chain_char, no_loop_0) {
#include"test_point/no_loop_0.txt"
#include "gen_chain_char_default.txt"
}

TEST(gen_chains_all, no_loop_1) {
#include"test_point/no_loop_1.txt"
#include "gen_chains_default.txt"
}

TEST(gen_chain_word, no_loop_1) {
#include"test_point/no_loop_1.txt"
#include "gen_chain_word_default.txt"
}

TEST(gen_chain_char, no_loop_1) {
#include"test_point/no_loop_1.txt"
#include "gen_chain_char_default.txt"
}

TEST(gen_chains_all, no_loop_2) {
#include"test_point/no_loop_2.txt"
#include "gen_chains_default.txt"
}

TEST(gen_chain_word, no_loop_2) {
#include"test_point/no_loop_2.txt"
#include "gen_chain_word_default.txt"
}

TEST(gen_chain_char, no_loop_2) {
#include"test_point/no_loop_2.txt"
#include "gen_chain_char_default.txt"
}

TEST(gen_chains_all, no_loop_3) {
#include"test_point/no_loop_3.txt"
#include "gen_chains_default.txt"
}

TEST(gen_chain_word, no_loop_3) {
#include"test_point/no_loop_3.txt"
#include "gen_chain_word_default.txt"
}

TEST(gen_chain_char, no_loop_3) {
#include"test_point/no_loop_3.txt"
#include "gen_chain_char_default.txt"
}

TEST(gen_chains_all, no_loop_4) {
#include"test_point/no_loop_4.txt"
#include "gen_chains_default.txt"
}

TEST(gen_chain_word, no_loop_4) {
#include"test_point/no_loop_4.txt"
#include "gen_chain_word_default.txt"
}

TEST(gen_chain_char, no_loop_4) {
#include"test_point/no_loop_4.txt"
#include "gen_chain_char_default.txt"
}

TEST(gen_chains_all, loop_0) {
#include"test_point/loop_0.txt"
#include "gen_chains_default.txt"
}

TEST(gen_chain_word, loop_0) {
#include"test_point/loop_0.txt"
#include "gen_chain_word_default.txt"
}

TEST(gen_chain_char, loop_0) {
#include"test_point/loop_0.txt"
#include "gen_chain_char_default.txt"
}


TEST(gen_chains_all, loop_1) {
#include"test_point/loop_1.txt"
#include "gen_chains_default.txt"
}

TEST(gen_chain_word, loop_1) {
#include"test_point/loop_1.txt"
#include "gen_chain_word_default.txt"
}

TEST(gen_chain_char, loop_1) {
#include"test_point/loop_1.txt"
#include "gen_chain_char_default.txt"
}

TEST(gen_chains_all, loop_2) {
#include"test_point/loop_2.txt"
#include "gen_chains_default.txt"
}

TEST(gen_chain_word, loop_2) {
#include"test_point/loop_2.txt"
#include "gen_chain_word_default.txt"
}

TEST(gen_chain_char, loop_2) {
#include"test_point/loop_2.txt"
#include "gen_chain_char_default.txt"
}

TEST(gen_chains_all, loop_3) {
#include"test_point/loop_3.txt"
#include "gen_chains_default.txt"
}

TEST(gen_chain_word, loop_3) {
#include"test_point/loop_3.txt"
#include "gen_chain_word_default.txt"
}

TEST(gen_chain_char, loop_3) {
#include"test_point/loop_3.txt"
#include "gen_chain_char_default.txt"
}

TEST(gen_chains_all, excp_loop) {
#include"test_point/excp_loop.txt"
#include "gen_chains_default.txt"
}

TEST(gen_chain_word, excp_loop) {
#include"test_point/excp_loop.txt"
#include "gen_chain_word_default.txt"
}

TEST(gen_chain_char, excp_loop) {
#include"test_point/excp_loop.txt"
#include "gen_chain_char_default.txt"
}

TEST(gen_chains_all, head) {
#include"test_point/head_sel.txt"
#include "gen_chains_default.txt"
}

TEST(gen_chain_word, head) {
#include"test_point/head_sel.txt"
#include "gen_chain_word_default.txt"
}

TEST(gen_chain_char, head) {
#include"test_point/head_sel.txt"
#include "gen_chain_char_default.txt"
}

TEST(gen_chains_all, tail) {
#include"test_point/tail_sel.txt"
#include "gen_chains_default.txt"
}

TEST(gen_chain_word, tail) {
#include"test_point/tail_sel.txt"
#include "gen_chain_word_default.txt"
}

TEST(gen_chain_char, tail) {
#include"test_point/tail_sel.txt"
#include "gen_chain_char_default.txt"
}

TEST(gen_chain_word, random_no_loop) {
    for (int i = 1; i < 20; i += 3) {
        for (int n = 2; n <= 26; n += 2) {
            char **input = generator(n, true, i, n * i);
            int std_ans = dp(input, i, 0, 0, false);
            std::string input_str[i];
            for (int id = 0; id < i; id++) {
                input_str[id] = std::string(input[id]);
            }
            std_check_gen_max(input_str, i, gen_chain_word, 0, 0, 0, 0, std_ans);
        }
    }
}

TEST(gen_chain_word, random_loop) {
    for (int i = 1; i < 20; i += 3) {
        for (int n = 2; n <= 26; n += 2) {
            char **input = generator(n, false, i, n * i);
            int std_ans = dp(input, i, 0, 0, false);
            std::string input_str[i];
            for (int id = 0; id < i; id++) {
                input_str[id] = std::string(input[id]);
            }
            std_check_gen_max(input_str, i, gen_chain_word, 0, 0, 0, true, std_ans);
        }
    }
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}