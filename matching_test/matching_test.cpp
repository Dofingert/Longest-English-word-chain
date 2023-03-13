#include <iostream>
#include <windows.h>
#include <testlib.h>
#include <sstream>

typedef int (*max_cnt_f)(char *[], int, char *[], void *(*)(size_t));

typedef int (*max_fut_f)(char *[], int, char *[], char, char, char, bool, void *(*)(size_t));

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
    int loop_cnt = 40;
    max_fut_f max_word_func[argc - 1];
    max_fut_f max_char_func[argc - 1];
    max_cnt_f cnt_func[argc - 1];
    HINSTANCE hInstLib[argc - 1];

    for (int prog = 1; prog < argc; prog++) {
        hInstLib[prog - 1] = LoadLibrary((LPCTSTR) argv[prog]);
        if (hInstLib[prog - 1] == nullptr) {
            throw std::logic_error("Cannot load library: " + std::string(argv[prog]));
        }
        max_word_func[prog - 1] = (max_fut_f) GetProcAddress(hInstLib[prog - 1], "gen_chain_word");
        max_char_func[prog - 1] = (max_fut_f) GetProcAddress(hInstLib[prog - 1], "gen_chain_char");
        cnt_func[prog - 1] = (max_cnt_f) GetProcAddress(hInstLib[prog - 1], "gen_chains_all");
        if (max_word_func[prog - 1] == nullptr) {
            throw std::logic_error("Cannot load function max_word_func in library: " + std::string(argv[prog]));
        }
        if (max_char_func[prog - 1] == nullptr) {
            throw std::logic_error("Cannot load function max_char_func in library: " + std::string(argv[prog]));
        }
        if (cnt_func[prog - 1] == nullptr) {
            throw std::logic_error("Cannot load function cnt_func in library: " + std::string(argv[prog]));
        }
    }

    // Main loop, Generate data in every loop.;
    for (int loop_id = 0; loop_id < loop_cnt; loop_id++) {
        // NO RING TEST
        for (int ring = 0; ring < 2; ring++) {
            int word_cnt = 10;
            int cnt_result_table[argc - 1];
            int word_result_table[argc - 1];
            int char_result_table[argc - 1];
            std::set<int> cnt_result_set;
            std::set<int> word_result_set;
            std::set<int> char_result_set;
            for (int i = 0; i < argc - 1; i++) {
                char **input = generator(26, ring == 0, word_cnt, loop_id + ring * 65536);
                word_set_t dictionary = build_dictionary(word_cnt, input);
                char **result = (char **) malloc(32768LL * sizeof(char *));
                int link_cnt = -1;
                try {
                    link_cnt = cnt_func[i](input, word_cnt, result, malloc);
                    word_set_t appear;
                    int err = 0;
                    for (int line = 0; line < link_cnt; line++) {
                        err += check_validation(dictionary, appear, result[line]);
                    }
                    free(result[0]);
                    if (err) {
                        cnt_result_table[i] = -2;
                        cnt_result_set.insert(-2);
                    } else {
                        cnt_result_table[i] = link_cnt;
                        cnt_result_set.insert(link_cnt);
                    }
                } catch (std::exception &e) {
                    cnt_result_table[i] = -1;
                    cnt_result_set.insert(-1);
                }

                try {
                    link_cnt = max_word_func[i](input, word_cnt, result, '\0', '\0', '\0', ring, malloc);
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
                    free(result[0]);
                    if (err) {
                        word_result_table[i] = -2;
                        word_result_set.insert(-2);
                    } else {
                        word_result_table[i] = link_cnt;
                        word_result_set.insert(link_cnt);
                    }
                } catch (std::exception &e) {
                    word_result_table[i] = -1;
                    word_result_set.insert(-1);
                }

                try {
                    link_cnt = max_char_func[i](input, word_cnt, result, '\0', '\0', '\0', ring, malloc);
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
                    free(result[0]);
                    if (err) {
                        char_result_table[i] = -2;
                        char_result_set.insert(-2);
                    } else {
                        char_result_table[i] = link_cnt;
                        char_result_set.insert(link_cnt);
                    }
                } catch (std::exception &e) {
                    char_result_table[i] = -1;
                    char_result_set.insert(-1);
                }
                free(result);
                free(input[0]);
                free(input);
            }
            if (cnt_result_set.size() == 1 && cnt_result_set.find(-2) == cnt_result_set.end()) {
                std::cout << "loop: " << loop_id << " ring: " << ((ring == 1) ? "true " : "false ")
                          << "cnt_result_set match in all point!" << std::endl;
            } else {
                std::cout << "loop: " << loop_id << " ring: " << ((ring == 1) ? "true " : "false ")
                          << "cnt_result_set mismatch!" << std::endl;
                print_all(cnt_result_table, argc - 1);
            }
            if (word_result_set.size() == 1 && word_result_set.find(-2) == word_result_set.end()) {
                std::cout << "loop: " << loop_id << " ring: " << ((ring == 1) ? "true " : "false ")
                          << "word_result_set match in all point!" << std::endl;
            } else {
                std::cout << "loop: " << loop_id << " ring: " << ((ring == 1) ? "true " : "false ")
                          << "word_result_set mismatch!" << std::endl;
                print_all(word_result_table, argc - 1);
            }
            if (char_result_set.size() == 1 && char_result_set.find(-2) == char_result_set.end()) {
                std::cout << "loop: " << loop_id << " ring: " << ((ring == 1) ? "true " : "false ")
                          << "char_result_set match in all point!" << std::endl;
            } else {
                std::cout << "loop: " << loop_id << " ring: " << ((ring == 1) ? "true " : "false ")
                          << "char_result_set mismatch!" << std::endl;
                print_all(char_result_table, argc - 1);
            }
        }
    }

    // free all dll
    for (int i = 0; i < argc - 1; i++) {
        FreeLibrary(hInstLib[i]);
    }

    return 0;
}