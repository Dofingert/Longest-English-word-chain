//
// Created by 10901 on 2023/3/14.
//

#ifndef GUI_MAIN_UI_H
#define GUI_MAIN_UI_H

#include <QWidget>
#include <QThread>


QT_BEGIN_NAMESPACE
namespace Ui { class main_ui; }
QT_END_NAMESPACE

class pending_job_t : public QObject {
Q_OBJECT
public:
    int cal_time;
    int mode;
    char head;
    char tail;
    char jail;
    bool ring;
    std::string input_string;
    std::string ans;

    explicit pending_job_t(int in_mode, char in_head, char in_tail, char in_jail, bool in_ring, std::string in_string,
                           QObject *parent = nullptr);

public slots:

    void compute_load();

signals:

    void finish_signal();
};


class main_ui : public QWidget {
Q_OBJECT


public:
    Ui::main_ui *ui;
    pending_job_t *pending_cal = nullptr;
    QThread worker_thread;

    bool calculating = false;
    bool pending = false;

    explicit main_ui(QWidget *parent = nullptr);

    ~main_ui() override;


signals:

    void start_computation();

public slots:

    void input_update();

    void mode_update(int sel);

    void gui_openfile();

    void gui_savefile();

    void gui_compute();

    void head_limit();

    void tail_limit();

    void jail_limit();

    void computation_finish();

};


#endif //GUI_MAIN_UI_H
