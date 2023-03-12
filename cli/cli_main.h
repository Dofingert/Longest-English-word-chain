//
// Created by 10901 on 2023/3/11.
//

#ifndef CORE_CLI_MAIN_H
#define CORE_CLI_MAIN_H

#define MAX_FILE_SIZE (1024LL * 1024 * 1024) // 1G

#include <core.h>

struct cli_exception_s : public std::exception {
    enum exception_type_e {
        err_out_of_mem,
        err_file_size,
        err_no_mode_specified,
        err_file_error,
        err_missing_input_char,
        err_no_alpha_char,
        err_repeated_args
    } err_type;

    cli_exception_s(exception_type_e type) {
        err_type = type;
    }

    const char *what() const noexcept override {
        static std::string exception[] = {
                "Out of memory.",
                "Opening file size is too large.",
                "No specified mode selected, use -n/-w/-c to select one.",
                "Invalid input file. ",
                "Missing a char, need a char after -h/-t/-j options. ",
                "Need an alpha char after -h/-t/-j options. ",
                "Repeated args, do not repeatedly use one args. "
        };
        return exception[err_type].c_str();
    }
};

#endif //CORE_CLI_MAIN_H
