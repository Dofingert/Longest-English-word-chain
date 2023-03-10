#include <iostream>
#include <cstring>

int main(int argc, char *argv[]) {
    enum {
        CLI_UNDEFINED,
        CLI_COUNT_ALL,
        CLI_MAX_WORD,
        CLI_MAX_CHAR
    } cli_type = CLI_UNDEFINED;
    char head = 0, tail = 0, ban = 0;
    bool ring = false;
    FILE *file;
    int ret = 0;
    for (int i = 1; i < argc; i += 1) {
        if (strcmp(argv[i], "-n") == 0) {
            if (cli_type == CLI_UNDEFINED) {
                cli_type = CLI_COUNT_ALL;
            } else {
                goto err_repeated_args;
            }
        } else if (strcmp(argv[i], "-w") == 0) {
            if (cli_type == CLI_UNDEFINED) {
                cli_type = CLI_MAX_WORD;
            } else {
                goto err_repeated_args;
            }
        } else if (strcmp(argv[i], "-c") == 0) {
            if (cli_type == CLI_UNDEFINED) {
                cli_type = CLI_MAX_CHAR;
            } else {
                goto err_repeated_args;
            }
        } else if (strcmp(argv[i], "-r") == 0) {
            if (!ring) {
                ring = true;
            } else {
                goto err_repeated_args;
            }
        } else if (strcmp(argv[i], "-h") == 0) {
            i += 1;
            if (head) {
                goto err_repeated_args;
            } else if (i >= argc || strlen(argv[i]) > 1) {
                goto err_missing_input_char;
            } else if (!isalpha(argv[i][0])) {
                goto err_no_alpha_char;
            } else {
                head = argv[i][0] | ('a' ^ 'A');
            }
        } else if (strcmp(argv[i], "-t") == 0) {
            i += 1;
            if (tail) {
                goto err_repeated_args;
            } else if (i >= argc || strlen(argv[i]) > 1) {
                goto err_missing_input_char;
            } else if (!isalpha(argv[i][0])) {
                goto err_no_alpha_char;
            } else {
                tail = argv[i][0] | ('a' ^ 'A');
            }
        } else if (strcmp(argv[i], "-j") == 0) {
            i += 1;
            if (ban) {
                goto err_repeated_args;
            } else if (i >= argc || strlen(argv[i]) > 1) {
                goto err_missing_input_char;
            } else if (!isalpha(argv[i][0])) {
                goto err_no_alpha_char;
            } else {
                ban = argv[i][0] | ('a' ^ 'A');
            }
        } else {
            // 打开输入文件
            if (file) {
                goto err_repeated_args;
            } else {
                file = fopen(argv[i], "r");
                if (file == nullptr) {
                    goto err_file_error;
                }
            }
        }
    }
    if (file == nullptr) {
        goto err_file_error;
    }
    if (cli_type == CLI_UNDEFINED) {
        goto err_no_mode_specified;
    }
    if (cli_type == CLI_COUNT_ALL && (head || tail || ban || ring)) {
        std::cerr << "Warning: -h/-t/-j/-r options is ignored. "<< std::endl;
    }
    // 调用计算函数

    goto exit_point;
    err_no_mode_specified:
    std::cerr << "No specified mode selected, use -n/-w/-c to select one. " << std::endl;
    ret = -1;
    goto exit_point;
    err_file_error:
    std::cerr << "Invalid input file. " << std::endl;
    ret = -1;
    goto exit_point;
    err_missing_input_char:
    std::cerr << "Missing a char, need a char after -h/-t/-j options. " << std::endl;
    ret = -1;
    goto exit_point;
    err_no_alpha_char:
    std::cerr << "Need an alpha char after -h/-t/-j options. " << std::endl;
    ret = -1;
    goto exit_point;
    err_repeated_args:
    std::cerr << "Repeated args, do not repeatedly use one args. " << std::endl;
    ret = -1;
    goto exit_point;
    exit_point:
    if(file) {
        fclose(file);
    }
    return ret;
}
