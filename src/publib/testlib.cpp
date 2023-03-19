#include <iostream>
#include <cstring>
#include "testlib.h"

typedef std::set<XXH64_hash_t> word_set_t;

int check_validation(const word_set_t &dictionary, word_set_t &appeared, const char *word_list, int expect_len) {
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
    int acc_len = 0;
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
            acc_len++;
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
    if(acc_len == 1 || (expect_len >= 0 && acc_len != expect_len)) {
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
