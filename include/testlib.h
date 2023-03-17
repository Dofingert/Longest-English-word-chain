#ifndef LONGEST_ENGLISH_WORD_CHAIN_TESTLIB_H
#define LONGEST_ENGLISH_WORD_CHAIN_TESTLIB_H

#include <set>

#define XXH_INLINE_ALL

#include <xxhash.h>

typedef std::set<XXH64_hash_t> word_set_t;

int dp(char *words[], int len, char head, char tail, bool weighted);

char **generator(int n, bool DAG, int word_cnt, unsigned int Seed, bool is_complete);

int check_validation(const word_set_t &dictionary, word_set_t &appeared, const char *word_list, int word_cnt);

word_set_t build_dictionary(int cnt, char **words);

#endif //LONGEST_ENGLISH_WORD_CHAIN_TESTLIB_H
