//
// Created by 10901 on 2023/3/14.
//

// You may need to build the project (run Qt uic code generator) to get "ui_main_ui.h" resolved

#include "main_ui.h"
#include "ui_main_ui.h"
#include <QThread>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <utility>

#define XXH_INLINE_ALL

#include "xxhash.h"
#include <QTime>

typedef int (*max_cnt_f)(char *[], int, char *[], void *(*)(size_t));

typedef int (*max_fut_f)(char *[], int, char *[], char, char, char, bool, void *(*)(size_t));

extern max_cnt_f gen_chains_all;
extern max_fut_f gen_chain_word;
extern max_fut_f gen_chain_char;

typedef std::set<XXH64_hash_t> word_set_t;

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

pending_job_t::pending_job_t(int in_mode, char in_head, char in_tail, char in_jail, bool in_ring, std::string in_string,
                             QObject *parent) : QObject(parent) {
    mode = in_mode;
    head = in_head;
    tail = in_tail;
    jail = in_jail;
    ring = in_ring;
    input_string = std::move(in_string);
}

void pending_job_t::compute_load() {
//    QThread::sleep(1); // 测试延迟
    QTime start_time = QTime::currentTime();
    std::vector<char *> word_list;
    char *input_buffer = new char[input_string.size() + 1];
    strcpy(input_buffer, input_string.c_str());
    int word_num = (int) split_word(input_buffer, word_list);
    char **result = new char *[65536];
    int ret = 0;
    std::stringstream result_str;
    try {
        if (mode == 0) {
            // count all words
            ret = gen_chains_all(word_list.data(), word_num, result, malloc);
        } else if (mode == 1) {
            // find max word
            ret = gen_chain_word(word_list.data(), word_num, result, head, tail, jail, ring,
                                 malloc);
        } else {
            // find max char
            ret = gen_chain_char(word_list.data(), word_num, result, head, tail, jail, ring,
                                 malloc);
        }
        // generate result string
        char split_char = (mode == 0) ? '\n' : '\n';
        for (int i = 0; i < ret; i++) {
            result_str << result[i];
            if (i != ret - 1) {
                result_str << split_char;
            }
        }
        if (ret == 0) {
            result_str << "<font color = #66CFCF>No Link !</font>" << std::endl;
        }
    } catch (std::exception &e) {
        result_str << "<font color = #FF0000>Error: " << e.what() << "</font>" << std::endl;
    }
    ans = result_str.str();
    // 检查状态，是否有必要写回
    if (ret != 0) {
        free(result[0]);
    }
    delete[] input_buffer;
    delete[] result;
    cal_time = start_time.msecsTo(QTime::currentTime());
    emit finish_signal();
}

void main_ui::gui_compute() {
    // 获取操作模式
    char head = 0, tail = 0, jail = 0;
    bool ring = ui->option_ring->checkState();
    int mode = ui->box_mode->currentIndex();
    if (isalpha(ui->box_head->toPlainText().toStdString()[0])) {
        head = (char) tolower(ui->box_head->toPlainText().toStdString()[0]);
    }
    if (isalpha(ui->box_tail->toPlainText().toStdString()[0])) {
        tail = (char) tolower(ui->box_tail->toPlainText().toStdString()[0]);
    }
    if (isalpha(ui->box_jail->toPlainText().toStdString()[0])) {
        jail = (char) tolower(ui->box_jail->toPlainText().toStdString()[0]);
    }
    if (pending_cal == nullptr) {
        pending_cal = new pending_job_t(mode, head, tail, jail, ring, ui->text_input->toPlainText().toStdString(),
                                        nullptr);
        pending_cal->moveToThread(&worker_thread);
        connect(&worker_thread, &QThread::finished, pending_cal, &QObject::deleteLater);
        connect(this, &main_ui::start_computation, pending_cal, &pending_job_t::compute_load);
        connect(pending_cal, &pending_job_t::finish_signal, this, &main_ui::computation_finish);
        worker_thread.start();
    } else {
        pending_cal->input_string = ui->text_input->toPlainText().toStdString();
        pending_cal->mode = mode;
        pending_cal->head = head;
        pending_cal->tail = tail;
        pending_cal->jail = jail;
        pending_cal->ring = ring;
    }
    if (calculating && !pending) {
        ui->status_label->setText("Status: <font color = #FF5809 >Pending...</font>");
        pending = true;
    } else if (!calculating) {
        ui->status_label->setText("Status: <font color = #FF2400 >Calculating...</font>");
        calculating = true;
        pending = false;
        // 开启新的计算线程
        emit start_computation();
    }
}


void main_ui::computation_finish() {
    if (pending) {
        ui->status_label->setText("Status: <font color = #FF2400 >Calculating...</font>");
        calculating = true;
        pending = false;
        // 开启新的计算线程
        emit start_computation();
    } else {
        calculating = false;
        std::stringstream builder;
        builder << "Status: <font color = #4896FA >Ready</font> (in " << pending_cal->cal_time << "ms)";
        ui->status_label->setText(QString::fromStdString(builder.str()));
        // 更新UI
        ui->text_output->setText(QString::fromStdString(pending_cal->ans));
    }
}


void main_ui::input_update() {
    if (ui->text_input->toPlainText().size() == 0) {
        ui->bottom_compute->setDisabled(true);
    } else {
        ui->bottom_compute->setDisabled(false);
    }
}

void main_ui::head_limit() {
    if (ui->box_head->toPlainText().size() > 1) {
        ui->box_head->setPlainText(ui->box_head->toPlainText().at(ui->box_head->toPlainText().size() - 1));
    }
}

void main_ui::tail_limit() {
    if (ui->box_tail->toPlainText().size() > 1) {
        ui->box_tail->setPlainText(ui->box_tail->toPlainText().at(ui->box_tail->toPlainText().size() - 1));
    }
}

void main_ui::jail_limit() {
    if (ui->box_jail->toPlainText().size() > 1) {
        ui->box_jail->setPlainText(ui->box_jail->toPlainText().at(ui->box_jail->toPlainText().size() - 1));
    }
}

void main_ui::mode_update(int sel) {
    if (sel == 0) {
        ui->option_ring->setDisabled(true);
        ui->box_head->setDisabled(true);
        ui->box_tail->setDisabled(true);
        ui->box_jail->setDisabled(true);
    } else {
        ui->option_ring->setDisabled(false);
        ui->box_head->setDisabled(false);
        ui->box_tail->setDisabled(false);
        ui->box_jail->setDisabled(false);
    }
}

main_ui::main_ui(QWidget *parent) :
        QWidget(parent), ui(new Ui::main_ui) {
    ui->setupUi(this);
    ui->bottom_compute->setDisabled(true);
    ui->option_ring->setDisabled(true);
    ui->box_head->setDisabled(true);
    ui->box_tail->setDisabled(true);
    ui->box_jail->setDisabled(true);
    ui->status_label->setText("Status: <font color = #4896FA >Ready</font>");
//    ui->status_label->setText("Status: <font color = #FF5809 >Pending...</font>");
    connect(ui->text_input, SIGNAL(textChanged()), this, SLOT(input_update()));
    connect(ui->box_mode, SIGNAL(activated(int)), this, SLOT(mode_update(int)));
    connect(ui->box_head, SIGNAL(textChanged()), this, SLOT(head_limit()));
    connect(ui->box_tail, SIGNAL(textChanged()), this, SLOT(tail_limit()));
    connect(ui->box_jail, SIGNAL(textChanged()), this, SLOT(jail_limit()));
    connect(ui->bottom_compute, SIGNAL(pressed()), this, SLOT(gui_compute()));
}

main_ui::~main_ui() {
    delete ui;
    worker_thread.quit();
    worker_thread.wait();
    delete pending_cal;
}