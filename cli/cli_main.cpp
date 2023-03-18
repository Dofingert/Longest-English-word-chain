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
        size_t file_size;
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
                    throw cli_exception_s(cli_exception_s::err_repeated_mode);
                }
            } else if (strcmp(argv[i], "-w") == 0) {
                if (cli_type == CLI_UNDEFINED) {
                    cli_type = CLI_MAX_WORD;
                } else {
                    throw cli_exception_s(cli_exception_s::err_repeated_mode);
                }
            } else if (strcmp(argv[i], "-c") == 0) {
                if (cli_type == CLI_UNDEFINED) {
                    cli_type = CLI_MAX_CHAR;
                } else {
                    throw cli_exception_s(cli_exception_s::err_repeated_mode);
                }
            } else if (strcmp(argv[i], "-r") == 0) {
                if (!ring) {
                    ring = true;
                } else {
                    throw std::logic_error("Error: Repeated args, do not repeatedly use -r!");
                }
            } else if (strcmp(argv[i], "-h") == 0) {
                i += 1;
                if (head) {
                    throw std::logic_error("Error: Repeated args, do not repeatedly use -h!");
                } else if (i >= argc || strlen(argv[i]) > 1 || !isalpha(argv[i][0])) {
                    throw std::logic_error("Error: Need letter after -h!");
                } else {
                    head = argv[i][0] | ('a' ^ 'A');
                }
            } else if (strcmp(argv[i], "-t") == 0) {
                i += 1;
                if (tail) {
                    throw std::logic_error("Error: Repeated args, do not repeatedly use -t!");
                } else if (i >= argc || strlen(argv[i]) > 1 || !isalpha(argv[i][0])) {
                    throw std::logic_error("Error: Need letter after -t!");
                } else {
                    tail = argv[i][0] | ('a' ^ 'A');
                }
            } else if (strcmp(argv[i], "-j") == 0) {
                i += 1;
                if (ban) {
                    throw std::logic_error("Error: Repeated args, do not repeatedly use -j!");
                } else if (i >= argc || strlen(argv[i]) > 1 || !isalpha(argv[i][0])) {
                    throw std::logic_error("Error: Need letter after -j!");
                } else {
                    ban = argv[i][0] | ('a' ^ 'A');
                }
            } else {
                // 检查输入文件名
                char file_name_end[5];
                if (strlen(argv[i]) >= 4) {
                    memcpy(file_name_end, &argv[i][strlen(argv[i]) - 4], 4);
                }
                file_name_end[4] = '\0';
                for (int c = 1; c < 4; c++) {
                    file_name_end[c] = tolower(file_name_end[c]);
                }
                if (strcmp(file_name_end, ".txt") != 0) {
                    throw std::logic_error(
                            "Error: \"" + std::string(argv[i]) + "\" is neither a text file nor an argument!");
                }
                // 打开输入文件
                if (file) {
                    throw std::logic_error("Error: Only support one input file!");
                } else {
                    file = fopen(argv[i], "rb");
                    if (file == nullptr) {
                        throw std::logic_error("Error: Cannot find input file: \"" + std::string(argv[i]) + "\"!");
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
            throw std::logic_error("Error: -n mode doesn't support -h/-t/-j/-r options!");
        }
        // 获取文件大小
        _fseeki64(file, 0, SEEK_END);
        file_size = _ftelli64(file);

        // 检查文件大小并分配空间
        if (file_size > MAX_FILE_SIZE) {
            throw std::logic_error("Error: Input filesize " + std::to_string(file_size / 1024) +
                                   "kb is too large. Support up to " + std::to_string(MAX_FILE_SIZE / 1024) + "kb!");
        }
        input_buffer = (char *) (malloc(file_size + 4096));
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
        if (ret != 0) {
            free(result[0]);
        }
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