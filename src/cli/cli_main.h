//
// Created by 10901 on 2023/3/11.
//

#ifndef CORE_CLI_MAIN_H
#define CORE_CLI_MAIN_H

#define MAX_FILE_SIZE (1024LL * 1024 * 1024) // 1G

#include "core.h"

struct cli_exception_s : public std::exception {
    enum exception_type_e {
        err_no_mode_specified,
        err_file_error,
        err_repeated_mode
    } err_type;

    explicit cli_exception_s(exception_type_e type) {
        err_type = type;
    }

    const char *what() const noexcept override {
        static std::string exception[] = {
                "Error: No specified mode selected, use -n/-w/-c!",
                "Error: No input file found!",
                "Error: Multiple mode selected, only support one -n/-w/-c argument!"
        };
        return exception[err_type].c_str();
    }
};

#endif //CORE_CLI_MAIN_H
