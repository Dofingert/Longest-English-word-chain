#include <iostream>
#include <windows.h>
#include <testlib.h>
#include <sstream>

typedef int (*max_cnt_f)(char *[], int, char *[], void *(*)(size_t));

typedef int (*max_fut_f)(char *[], int, char *[], char, char, char, bool, void *(*)(size_t));

const int word_cnt = 50; // 单词个数
const int loop_cnt = 100; // 测试点个数
const int circle_allow = 1; // 是否允许测试点有环，1允许，0不允许

void print_all_time(double result[], int cnt) {
    for (int i = 0; i < cnt; i++) {
        std::cout << i << " exec time: ";
        std::cout << result[i] * 1000 << " ms" << std::endl;
    }
}

void print_all(int result[], int cnt) {
    for (int i = 0; i < cnt; i++) {
        std::cout << "prog " << i << ": ";
        if (result[i] == -2) {
            std::cout << "not valid result" << std::endl;
        } else if (result[i] == -1) {
            std::cout << "exception" << std::endl;
        } else {
            std::cout << result[i] << std::endl;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Usage: maching_test dll_under_test{2,} ..." << std::endl;
        return -1;
    }

    max_fut_f max_word_func[argc - 1];
    max_fut_f max_char_func[argc - 1];
    max_cnt_f cnt_func[argc - 1];
    // 动态链接库
    HINSTANCE hInstLib[argc - 1];
    // windows 计时器
    double time_cnt[argc - 1];
    for (int i = 0; i < argc - 1; i++) {
        time_cnt[i] = 0.f;
    }
    LARGE_INTEGER cnt_freq;
    QueryPerformanceFrequency(&cnt_freq);

    for (int prog = 1; prog < argc; prog++) {
        hInstLib[prog - 1] = LoadLibrary((LPCTSTR) argv[prog]);
        if (hInstLib[prog - 1] == nullptr) {
            throw std::logic_error("Cannot load library: " + std::string(argv[prog]));
        }
        // 获取函数地址
        max_word_func[prog - 1] = (max_fut_f) GetProcAddress(hInstLib[prog - 1], "gen_chain_word");
        max_char_func[prog - 1] = (max_fut_f) GetProcAddress(hInstLib[prog - 1], "gen_chain_char");
        cnt_func[prog - 1] = (max_cnt_f) GetProcAddress(hInstLib[prog - 1], "gen_chains_all");
        if (max_word_func[prog - 1] == nullptr) {
            throw std::logic_error("Cannot load function gen_chain_word in library: " + std::string(argv[prog]));
        }
        if (max_char_func[prog - 1] == nullptr) {
            throw std::logic_error("Cannot load function gen_chain_char in library: " + std::string(argv[prog]));
        }
        if (cnt_func[prog - 1] == nullptr) {
            throw std::logic_error("Cannot load function cnt_func in library: " + std::string(argv[prog]));
        }
    }

    // Main loop, Generate data in every loop.;
    for (int loop_id = 0; loop_id < loop_cnt; loop_id++) {
        // NO RING TEST
        for (int ring = 0; ring <= circle_allow; ring++) {
            // 0 无环，1 有环
            int cnt_result_table[argc - 1];
            double cnt_time[argc - 1];
            int word_result_table[argc - 1];
            double word_time[argc - 1];
            int char_result_table[argc - 1];
            double char_time[argc - 1];
            std::set<int> cnt_result_set;
            std::set<int> word_result_set;
            std::set<int> char_result_set;
            for (int i = 0; i < argc - 1; i++) {
                LARGE_INTEGER time_1, time_2;
                char **input = generator(26, ring == 0, word_cnt, loop_id + ring * 65536);
                word_set_t dictionary = build_dictionary(word_cnt, input);
                char **result = (char **) malloc(32768LL * sizeof(char *));
                int link_cnt = -1;
                try {
                    QueryPerformanceCounter(&time_1);
                    link_cnt = cnt_func[i](input, word_cnt, result, malloc);
                    QueryPerformanceCounter(&time_2);
                    word_set_t appear;
                    int err = 0;
                    for (int line = 0; line < link_cnt; line++) {
                        err += check_validation(dictionary, appear, result[line]);
                    }
                    if (link_cnt != 0) {
                        free(result[0]);
                    }
                    if (err) {
                        cnt_result_table[i] = -2;
                        cnt_result_set.insert(-2);
                    } else {
                        cnt_result_table[i] = link_cnt;
                        cnt_result_set.insert(link_cnt);
                    }
                } catch (std::exception &e) {
                    QueryPerformanceCounter(&time_2);
                    cnt_result_table[i] = -1;
                    cnt_result_set.insert(-1);
                }
                cnt_time[i] = (double) (time_2.QuadPart - time_1.QuadPart) / (double) cnt_freq.QuadPart;

                try {
                    QueryPerformanceCounter(&time_1);
                    link_cnt = max_word_func[i](input, word_cnt, result, '\0', '\0', '\0', ring, malloc);
                    QueryPerformanceCounter(&time_2);
                    word_set_t appear;
                    int err = 0;
                    std::stringstream result_str;
                    for (int line = 0; line < link_cnt; line++) {
                        result_str << result[line];
                        if (line != link_cnt - 1) {
                            result_str << ' ';
                        }
                    }
                    err = check_validation(dictionary, appear, result_str.str().c_str());
                    if (link_cnt != 0) {
                        free(result[0]);
                    }
                    if (err) {
                        word_result_table[i] = -2;
                        word_result_set.insert(-2);
                    } else {
                        word_result_table[i] = link_cnt;
                        word_result_set.insert(link_cnt);
                    }
                } catch (std::exception &e) {
                    QueryPerformanceCounter(&time_2);
                    word_result_table[i] = -1;
                    word_result_set.insert(-1);
                }
                word_time[i] = (double) (time_2.QuadPart - time_1.QuadPart) / (double) cnt_freq.QuadPart;

                try {
                    QueryPerformanceCounter(&time_1);
                    link_cnt = max_char_func[i](input, word_cnt, result, '\0', '\0', '\0', ring, malloc);
                    QueryPerformanceCounter(&time_2);
                    word_set_t appear;
                    int err = 0;
                    std::stringstream result_str;
                    for (int line = 0; line < link_cnt; line++) {
                        result_str << result[line];
                        if (line != link_cnt - 1) {
                            result_str << ' ';
                        }
                    }
                    err = check_validation(dictionary, appear, result_str.str().c_str());
                    if (link_cnt != 0) {
                        free(result[0]);
                    }
                    if (err) {
                        char_result_table[i] = -2;
                        char_result_set.insert(-2);
                    } else {
                        char_result_table[i] = link_cnt;
                        char_result_set.insert(link_cnt);
                    }
                } catch (std::exception &e) {
                    QueryPerformanceCounter(&time_2);
                    char_result_table[i] = -1;
                    char_result_set.insert(-1);
                }
                char_time[i] = (double) (time_2.QuadPart - time_1.QuadPart) / (double) cnt_freq.QuadPart;

                free(result);
                free(input[0]);
                free(input);
                time_cnt[i] += cnt_time[i] + word_time[i] + char_time[i];
            }
            if (cnt_result_set.size() == 1 && cnt_result_set.find(-2) == cnt_result_set.end()) {
                std::cout << "loop: " << loop_id << " ring: " << ((ring == 1) ? "true " : "false ")
                          << "cnt_result_set match in all point!" << std::endl;
                print_all_time(cnt_time, argc - 1);
            } else {
                std::cout << "loop: " << loop_id << " ring: " << ((ring == 1) ? "true " : "false ")
                          << "cnt_result_set mismatch!" << std::endl;
                print_all(cnt_result_table, argc - 1);
                FILE *fp = fopen("err_data.txt", "w");
                char **data = generator(26, ring == 0, word_cnt, loop_id + ring * 65536);
                for (int i = 0; i < word_cnt; i++) {
                    fprintf(fp, "%s\n", data[i]);
                }
                free(data[0]);
                free(data);
                fclose(fp);
                return -1;
            }
            if (word_result_set.size() == 1 && word_result_set.find(-2) == word_result_set.end()) {
                std::cout << "loop: " << loop_id << " ring: " << ((ring == 1) ? "true " : "false ")
                          << "word_result_set match in all point!" << std::endl;
                print_all_time(word_time, argc - 1);
            } else {
                std::cout << "loop: " << loop_id << " ring: " << ((ring == 1) ? "true " : "false ")
                          << "word_result_set mismatch!" << std::endl;
                print_all(word_result_table, argc - 1);
                FILE *fp = fopen("err_data.txt", "w");
                char **data = generator(26, ring == 0, word_cnt, loop_id + ring * 65536);
                for (int i = 0; i < word_cnt; i++) {
                    fprintf(fp, "%s\n", data[i]);
                }
                free(data[0]);
                free(data);
                fclose(fp);
                return -1;
            }
            if (char_result_set.size() == 1 && char_result_set.find(-2) == char_result_set.end()) {
                std::cout << "loop: " << loop_id << " ring: " << ((ring == 1) ? "true " : "false ")
                          << "char_result_set match in all point!" << std::endl;
                print_all_time(char_time, argc - 1);
            } else {
                std::cout << "loop: " << loop_id << " ring: " << ((ring == 1) ? "true " : "false ")
                          << "char_result_set mismatch!" << std::endl;
                print_all(char_result_table, argc - 1);
                FILE *fp = fopen("err_data.txt", "w");
                char **data = generator(26, ring == 0, word_cnt, loop_id + ring * 65536);
                for (int i = 0; i < word_cnt; i++) {
                    fprintf(fp, "%s\n", data[i]);
                }
                free(data[0]);
                free(data);
                fclose(fp);
                return -1;
            }
        }
    }
    // print all time
    // free all dll
    for (int i = 0; i < argc - 1; i++) {
        std::cout << "0 total exec time: " << time_cnt[i] * 1000 << "ms" << std::endl;
        FreeLibrary(hInstLib[i]);
    }

    return 0;
}