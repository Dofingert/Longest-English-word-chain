#include <iostream>
#include <cstring>
#include <vector>

#include "cli_main.h"
#include <set>

#define XXH_INLINE_ALL

#include <xxhash.h>
typedef std::set<XXH64_hash_t> word_set_t;

// 分割输入字符串的函数
// 传入两个参数 char* input_string 和 std::vector<char*> result
// 前者存放输入的字符串, 后者存放输出的单词数组
// 返回值为单词的数量
size_t split_word(char *input_string, std::vector<char *> &result) {
    word_set_t diction;
    int begin_pos = 0;
    enum {
        WAIT_WORD,
        IN_WORD
    } state = WAIT_WORD;
    int pos = 0;
    for (; input_string[pos]; pos++) {
        if (state == WAIT_WORD && isalpha(input_string[pos])) {
            state = IN_WORD;
            begin_pos = pos;
            input_string[pos] |= ('A' ^ 'a'); // 转小写
        } else if (state == IN_WORD && !isalpha(input_string[pos])) {
            state = WAIT_WORD;
            input_string[pos] = '\0';
            if (diction.find(XXH64(input_string + begin_pos, pos - begin_pos, 0)) == diction.end()) {
                diction.insert(XXH64(input_string + begin_pos, pos - begin_pos, 0));
                result.push_back(input_string + begin_pos);
            }
        }
    }
    if (state == IN_WORD) {
        if (diction.find(XXH64(input_string + begin_pos, pos - begin_pos, 0)) == diction.end()) {
            result.push_back(input_string + begin_pos);
        }
    }
    return result.size();
}

int main(int argc, char *argv[]) {
    try {
        enum {
            CLI_UNDEFINED,
            CLI_COUNT_ALL,
            CLI_MAX_WORD,
            CLI_MAX_CHAR
        } cli_type = CLI_UNDEFINED;
        char head = 0, tail = 0, ban = 0;
        bool ring = false;
        int ret = 0;
        long file_size;
        FILE *file = nullptr;
        char *input_buffer = nullptr;
        size_t word_num = 0;
        std::vector<char *> word_list;
        char **c_word_list;
        for (int i = 1; i < argc; i += 1) {
            if (strcmp(argv[i], "-n") == 0) {
                if (cli_type == CLI_UNDEFINED) {
                    cli_type = CLI_COUNT_ALL;
                } else {
                    throw cli_exception_s(cli_exception_s::err_repeated_args);
                }
            } else if (strcmp(argv[i], "-w") == 0) {
                if (cli_type == CLI_UNDEFINED) {
                    cli_type = CLI_MAX_WORD;
                } else {
                    throw cli_exception_s(cli_exception_s::err_repeated_args);
                }
            } else if (strcmp(argv[i], "-c") == 0) {
                if (cli_type == CLI_UNDEFINED) {
                    cli_type = CLI_MAX_CHAR;
                } else {
                    throw cli_exception_s(cli_exception_s::err_repeated_args);
                }
            } else if (strcmp(argv[i], "-r") == 0) {
                if (!ring) {
                    ring = true;
                } else {
                    throw cli_exception_s(cli_exception_s::err_repeated_args);
                }
            } else if (strcmp(argv[i], "-h") == 0) {
                i += 1;
                if (head) {
                    throw cli_exception_s(cli_exception_s::err_repeated_args);
                } else if (i >= argc || strlen(argv[i]) > 1) {
                    throw cli_exception_s(cli_exception_s::err_missing_input_char);
                } else if (!isalpha(argv[i][0])) {
                    throw cli_exception_s(cli_exception_s::err_no_alpha_char);
                } else {
                    head = argv[i][0] | ('a' ^ 'A');
                }
            } else if (strcmp(argv[i], "-t") == 0) {
                i += 1;
                if (tail) {
                    throw cli_exception_s(cli_exception_s::err_repeated_args);
                } else if (i >= argc || strlen(argv[i]) > 1) {
                    throw cli_exception_s(cli_exception_s::err_missing_input_char);
                } else if (!isalpha(argv[i][0])) {
                    throw cli_exception_s(cli_exception_s::err_no_alpha_char);
                } else {
                    tail = argv[i][0] | ('a' ^ 'A');
                }
            } else if (strcmp(argv[i], "-j") == 0) {
                i += 1;
                if (ban) {
                    throw cli_exception_s(cli_exception_s::err_repeated_args);
                } else if (i >= argc || strlen(argv[i]) > 1) {
                    throw cli_exception_s(cli_exception_s::err_missing_input_char);
                } else if (!isalpha(argv[i][0])) {
                    throw cli_exception_s(cli_exception_s::err_no_alpha_char);
                } else {
                    ban = argv[i][0] | ('a' ^ 'A');
                }
            } else {
                // 打开输入文件
                if (file) {
                    throw cli_exception_s(cli_exception_s::err_repeated_args);
                } else {
                    file = fopen(argv[i], "r");
                    if (file == nullptr) {
                        throw cli_exception_s(cli_exception_s::err_file_error);
                    }
                }
            }
        }
        if (file == nullptr) {
            throw cli_exception_s(cli_exception_s::err_file_error);
        }
        if (cli_type == CLI_UNDEFINED) {
            throw cli_exception_s(cli_exception_s::err_no_mode_specified);
        }
        if (cli_type == CLI_COUNT_ALL && (head || tail || ban || ring)) {
            std::clog << "Warning: -h/-t/-j/-r options is ignored. " << std::endl;
        }
        // 获取文件大小
        fseek(file, 0, SEEK_END);
        file_size = ftell(file);

        // 检查文件大小并分配空间
        if (file_size > MAX_FILE_SIZE) {
            throw cli_exception_s(cli_exception_s::err_file_size);
        }
        input_buffer = (char *) (malloc(file_size + 1));
        fseek(file, 0, SEEK_SET);
        fread(input_buffer, 1, file_size, file);
        fclose(file);
        input_buffer[file_size] = '\0';
        // 进行分词 获得单词指针数组
        word_num = split_word(input_buffer, word_list);
        c_word_list = word_list.data();
        char **result = (char **) malloc(sizeof(char *) * 65536);
        if (cli_type == CLI_COUNT_ALL) {
            ret = gen_chains_all(c_word_list, word_num, result, malloc);
            printf("%d\n", ret);
        } else if (cli_type == CLI_MAX_CHAR) {
            ret = gen_chain_char(c_word_list, word_num, result, head, tail, ban, ring, malloc);
        } else if (cli_type == CLI_MAX_WORD) {
            ret = gen_chain_word(c_word_list, word_num, result, head, tail, ban, ring, malloc);
        }
        if (cli_type != CLI_COUNT_ALL) {
            file = fopen("solution.txt", "w");
        } else {
            file = stdout;
        }
        for (int i = 0; i < ret; i++) {
            fprintf(file, "%s\n", result[i]);
        }
        free(result[0]);
        free(result);
        if (cli_type != CLI_COUNT_ALL) {
            fclose(file);
        }
        free(input_buffer);
        return 0;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}