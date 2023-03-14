#include <QApplication>
#include <QMessageBox>
#include <windows.h>
#include "main_ui.h"

typedef int (*max_cnt_f)(char *[], int, char *[], void *(*)(size_t));

typedef int (*max_fut_f)(char *[], int, char *[], char, char, char, bool, void *(*)(size_t));

max_cnt_f gen_chains_all;
max_fut_f gen_chain_word;
max_fut_f gen_chain_char;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    // 加载dll
    HINSTANCE core_lib;
    core_lib = LoadLibrary("core.dll");
    // 加载失败时弹窗报错
    if (core_lib == nullptr) {
        QMessageBox::critical(nullptr, "严重错误", "不能正确加载core.dll, 请检查运行环境",
                              QMessageBox::Escape, QMessageBox::Escape);
        return -1;
    }
    gen_chains_all = (max_cnt_f) GetProcAddress(core_lib, "gen_chains_all");
    gen_chain_word = (max_fut_f) GetProcAddress(core_lib, "gen_chain_word");
    gen_chain_char = (max_fut_f) GetProcAddress(core_lib, "gen_chain_char");

    if(gen_chains_all == nullptr || gen_chain_word == nullptr || gen_chain_char == nullptr) {
        QMessageBox::critical(nullptr, "严重错误", "core.dll不含必须的函数",
                              QMessageBox::Escape, QMessageBox::Escape);
    }

    // 加载成功后进入QT 主循环
    main_ui main_win;
    main_win.show();
    int ret = QApplication::exec();
    FreeLibrary(core_lib);
    return ret;
}
