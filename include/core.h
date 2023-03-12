//
// Created by Surface on 2023/3/8.
//

#ifndef CORE_CORE_H
#define CORE_CORE_H

#ifdef EXPORT_FPIC
#define EXPOSED_FUNCTION extern "C" __declspec(dllexport)
#else
#define EXPOSED_FUNCTION extern "C" __declspec(dllimport)
#endif

EXPOSED_FUNCTION int gen_chains_all(char *words[], int len, char *result[], void *out_malloc(size_t));

EXPOSED_FUNCTION int gen_chain_word(char *words[], int len, char *result[], char head, char tail, char jail, bool enable_loop,
                   void *out_malloc(size_t));

EXPOSED_FUNCTION int gen_chain_char(char *words[], int len, char *result[], char head, char tail, char jail, bool enable_loop,
                   void *out_malloc(size_t));

#endif //CORE_CORE_H
