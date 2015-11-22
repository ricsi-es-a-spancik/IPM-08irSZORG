#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <vector>

typedef std::vector<std::string> pseq;

//format1: "frame_id packet_num value periodicity jitter slack"
//format2: "frame_id"
const std::vector<pseq> packets_sequences = {
    pseq {},
    pseq {"f1 6 6 4 0 3", "f3 6 6 4 0 2"},
    pseq {"f2 4 4 5 0 6", "f5 5 5 5 0 2"},
    pseq {"f4 6 6 4 0 1"},
    pseq {"f6 3 3 5 0 3"},
    pseq {"f1", "f3"},
    pseq {},
    pseq {"f2", "f5", "f4"},
    pseq {},
    pseq {"f1", "f3", "f6"},
    pseq {},
    pseq {"f4"},
    pseq {"f2", "f5"},
    pseq {"f1", "f3"},
    pseq {"f6"},
    pseq {"f4"},
    pseq {},
    pseq {"f1", "f3", "f2", "f5"},
    pseq {},
    pseq {"f4"},
    pseq {},
    pseq {"f1", "f3"},
    pseq {"f5"},
    pseq {"f4"},
    pseq {},
    pseq {}
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    scheduler(4, 5)
{
    ui->setupUi(this);

    for(auto e : packets_sequences)
    {
        scheduler.schedule(e);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
