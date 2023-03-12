//
// Created by Surface on 2023/3/8.
//

#ifndef CORE_CORE_H
#define CORE_CORE_H

//extern "C" {
//__declspec(dllimport) int gen_chains_all(char *words[], int len, char *result[]) noexcept(false);
//__declspec(dllimport) int
//gen_chain_word(char *words[], int len, char *result[], char head, char tail, bool allow_circ) noexcept(false);
//__declspec(dllimport) int gen_chain_word_unique(char *words[], int len, char *result[]) noexcept(false);
//__declspec(dllimport) int
//gen_chain_char(char *words[], int len, char *result[], char head, char tail, bool enable_loop) noexcept(false);
//}
int gen_chains_all(char *words[], int len, char *result[], void *out_malloc(size_t));
int gen_chain_word(char *words[], int len, char *result[], char head, char tail, char jail, bool enable_loop, void *out_malloc(size_t));
int gen_chain_char(char *words[], int len, char *result[], char head, char tail, char jail, bool enable_loop, void *out_malloc(size_t));

#endif //CORE_CORE_H
