#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <fstream>
#include <vector>
#include <list>

extern std::vector<std::string> split(const std::string &s, char delim);
extern unsigned string_to_unsigned(const std::string& str);

MainWindow::MainWindow(unsigned dmin, unsigned dmax, decltype(packet_sequences_raw_) packet_sequences_raw, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    packet_sequences_raw_(packet_sequences_raw),
    scheduler_(dmin, dmax)
{
    ui->setupUi(this);

    setup_buttons();
    setup_labels();
    setup_pipelines();

    this->showFullScreen();
}

void MainWindow::align_buttons()
{
    auto size = QSize(this->size().width()/5, this->size().height()/10);
    auto pos = QPoint(this->size().width() - size.width(), this->size().height() - size.height());

    next_button_->setGeometry(QRect(pos, size));
}

void MainWindow::setup_buttons()
{
    next_button_ = new QPushButton("Next", this);
    connect(next_button_, SIGNAL (released()), this, SLOT (handle_next_button()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::setup_labels()
{
    time_label_ = new QLabel(this);
    total_frames_delivered_label_ = new QLabel(this);
    total_frames_rejected_label_ = new QLabel(this);
    act_label_ = new QLabel(this);
    act1_label_ = new QLabel(this);
    transmitted_label_ = new QLabel(this);

    refresh_label_contents();
}

void MainWindow::refresh_label_contents()
{
    time_label_->setText(("Time: " + std::to_string(scheduler_.get_time())).c_str());
    total_frames_delivered_label_->setText(("Frames delivered: " + std::to_string(scheduler_.get_total_frames_delivered())).c_str());
    total_frames_rejected_label_->setText(("Frames rejected: " + std::to_string(scheduler_.get_total_frames_rejected())).c_str());
    act_label_->setText(("Act: " + scheduler_.act_frames()).c_str());
    act1_label_->setText(("Act1: " + scheduler_.act1_frames()).c_str());
    transmitted_label_->setText(("Transmitted packet of: " + scheduler_.get_last_transfered_packet()).c_str());
}

void MainWindow::align_labels()
{
    auto size = QSize(this->size().width()/4, this->size().height()/25);
    auto base_pos = QPoint(this->size().width() - size.width(), 0);

    QFont font("Arial", size.height() / 2);

    time_label_->setGeometry(QRect(QPoint(base_pos.x(), base_pos.y() + 1*size.height()), size));
    time_label_->setFont(font);
    total_frames_delivered_label_->setGeometry(QRect(QPoint(base_pos.x(), base_pos.y() + 2*size.height()), size));
    total_frames_delivered_label_->setFont(font);
    total_frames_rejected_label_->setGeometry(QRect(QPoint(base_pos.x(), base_pos.y() + 3*size.height()), size));
    total_frames_rejected_label_->setFont(font);

    act_label_->setGeometry(this->size().width()/20,1*size.height(),this->size().width() - size.width(),size.height());
    act_label_->setFont(font);
    act1_label_->setGeometry(this->size().width()/20,2*size.height(),this->size().width() - size.width(),size.height());
    act1_label_->setFont(font);

    transmitted_label_->setGeometry(QRect(QPoint(this->size().width()/20, this->size().height()-2*size.height()), size));
    transmitted_label_->setFont(font);
}

void MainWindow::parse_packet_sequences()
{
    int slot_idx = 0;
    for(auto slot_raw : packet_sequences_raw_)
    {
        for(auto packet : slot_raw)
        {
            auto splitted = split(packet, ' ');
            std::string frame_id = splitted[0];
            if(splitted.size() > 1)
            {
                //format1: "frame_id packet_num value periodicity jitter slack"
                unsigned packet_num = string_to_unsigned(splitted[1]);
                unsigned periodicity = string_to_unsigned(splitted[3]);
                unsigned jitter = string_to_unsigned(splitted[4]);
                unsigned slack = string_to_unsigned(splitted[5]);
                unsigned tau = slot_idx + (packet_num-1) * periodicity + jitter;
                packet_sequences[frame_id].second = slot_interval(tau, tau + slack);
            }
            packet_sequences[frame_id].first.resize(slot_idx + 1, 0);
            packet_sequences[frame_id].first[slot_idx] = 1;
        }

        ++slot_idx;
    }

    for(auto& packet_seq : packet_sequences)
    {
        packet_seq.second.first.resize(packet_sequences_raw_.size() + col_count_, 0);
        for(unsigned i=packet_seq.second.second.first; i<packet_seq.second.second.second; ++i)
            packet_seq.second.first[i] = 2;
        packet_seq.second.first[packet_seq.second.second.second] = 3;
    }
}

void MainWindow::setup_pipelines()
{
    last_packet_reservations_.reserve(col_count_);
    QLabel* label;
    for(unsigned i = 0; i < col_count_; ++i)
    {
        label = new QLabel(this);
        last_packet_reservations_.push_back(label);
    }

    parse_packet_sequences();
    packet_labels_.resize(packet_sequences.size());
    for(unsigned j=0; j<packet_labels_.size(); ++j)
    {
        packet_labels_[j].reserve(col_count_);
        for(unsigned i = 0; i < col_count_; ++i)
        {
            label = new QLabel(this);
            packet_labels_[j].push_back(label);
        }
    }

    refresh_pipline_contents();
}

void MainWindow::refresh_pipline_contents()
{
    for(unsigned i = 0; i < col_count_; ++i)
    {
        unsigned slot = scheduler_.get_time() + i;

        if(scheduler_.get_pipeline().is_slot_reserved(slot))
        {
            last_packet_reservations_[i]->setText(scheduler_.get_pipeline().slot_reserved_by(slot)->id_.c_str());
            last_packet_reservations_[i]->setStyleSheet("QLabel { background-color : yellow; border-style: solid; border-width: 1px; border-color : black; }");
        }
        else
        {
            last_packet_reservations_[i]->setText("");
            last_packet_reservations_[i]->setStyleSheet("QLabel { background-color : white; border-style: solid; border-width: 1px; border-color : black; }");
        }
    }

    int j=0;
    for(const auto& packet_seq : packet_sequences)
    {
        for(unsigned i = 0; i < col_count_; ++i)
        {
            unsigned slot = scheduler_.get_time() + i;

            switch(packet_seq.second.first[slot])
            {
            case 0:
                packet_labels_[j][i]->setStyleSheet("QLabel { background-color : white; border-style: solid; border-width: 1px; border-color : black; }");
                break;
            case 1:
                packet_labels_[j][i]->setStyleSheet("QLabel { background-color : yellow; border-style: solid; border-width: 1px; border-color : black; }");
                break;
            case 2:
                packet_labels_[j][i]->setStyleSheet("QLabel { background-color : aqua; border-style: solid; border-width: 1px; border-color : black; }");
                break;
            case 3:
                packet_labels_[j][i]->setStyleSheet("QLabel { background-color : red; border-style: solid; border-width: 1px; border-color : black; }");
                break;
            }
        }
        ++j;
    }
}

void MainWindow::align_pipelines()
{
    auto size = QSize((this->size().width()-this->size().width()/10) / col_count_, this->size().height()/15);
    auto pos = QPoint(this->size().width()/20, this->size().height()/4);

    for(unsigned i = 0; i < col_count_; ++i)
    {
        last_packet_reservations_[i]->setGeometry(QRect(pos.x() + i*size.width(), pos.y(), size.width(), size.height()));
    }

    pos = QPoint(this->size().width()/20, this->size().height()/3);

    for(unsigned j=0; j<packet_labels_.size(); ++j)
    {
        for(unsigned i = 0; i < col_count_; ++i)
        {
            packet_labels_[j][i]->setGeometry(QRect(pos.x() + i*size.width(), pos.y()+j*size.height(), size.width(), size.height()));
        }
    }
}

//////////////////////
/// event handlers ///
//////////////////////

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    align_buttons();
    align_labels();
    align_pipelines();
}

void MainWindow::handle_next_button()
{
    if(!packet_sequences_raw_.empty())
    {
        scheduler_.schedule(packet_sequences_raw_.front());
        packet_sequences_raw_.pop_front();
        refresh_label_contents();
        refresh_pipline_contents();
    }
}
