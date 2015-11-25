#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>

#include <memory>
#include <vector>
#include <map>
#include <list>

#include "scheduler.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;

    std::list<std::vector<std::string>> packet_sequences_raw_;
    Scheduler scheduler_;

    QPushButton* next_button_;
    void align_buttons();
    void setup_buttons();

    QLabel* time_label_;
    QLabel* total_frames_delivered_label_;
    QLabel* total_frames_rejected_label_;
    QLabel* act_label_;
    QLabel* act1_label_;
    QLabel* transmitted_label_;
    void setup_labels();
    void refresh_label_contents();
    void align_labels();

    const unsigned col_count_ = 20;
    std::vector<QLabel*> last_packet_reservations_;
    std::map<std::string, std::pair<std::vector<unsigned>, slot_interval>> packet_sequences;
    std::vector<std::vector<QLabel*>> packet_labels_;
    void parse_packet_sequences();
    void setup_pipelines();
    void refresh_pipline_contents();
    void align_pipelines();

public:
    explicit MainWindow(unsigned dmin, unsigned dmax, decltype(packet_sequences_raw_), QWidget *parent = 0);
    ~MainWindow();

private slots:
    void resizeEvent(QResizeEvent* event);
    void handle_next_button();


};

#endif // MAINWINDOW_H
