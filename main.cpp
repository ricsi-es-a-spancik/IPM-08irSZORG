#include "mainwindow.h"
#include <QApplication>
#include <sstream>
#include <fstream>
#include <iostream>

void parse_file(char* path, unsigned& dmin, unsigned& dmax, std::list<std::vector<std::string>>& packet_sequences_raw);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    unsigned dmin, dmax;
    std::list<std::vector<std::string>> packet_sequences_raw;

    parse_file(argv[1], dmin, dmax, packet_sequences_raw);
    MainWindow w(dmin, dmax, packet_sequences_raw);
    w.show();

    return a.exec();
}

/// File format:
/// DMIN DMAX
/// N1
/// packet1_1
/// packet1_2
/// ...
/// N2
/// packet2_1
/// packet2_2
/// ...
/// ...
/// NM
/// packetM_1
/// packetM_2
/// ...
/// --end of file--
///
/// packet format#1: "frame_id packet_num value periodicity jitter slack"
/// packet format#2: "frame_id"
/// NO ERROR CHECKING!
void parse_file(char* path, unsigned& dmin, unsigned& dmax, std::list<std::vector<std::string>>& packet_sequences_raw)
{
    std::ifstream file(path);

    if(!file.is_open())
    {
        std::cerr << path << std::endl;
        std::exit(1);
    }

    std::string line;
    std::getline(file, line);
    std::istringstream(line) >> dmin >> dmax;

    int packet_num;
    while (std::getline(file, line))
    {
        if(std::istringstream(line) >> packet_num)
        {
            std::vector<std::string> current_slot_packets;
            current_slot_packets.reserve(packet_num);
            for(int i=0; i<packet_num; ++i)
            {
                std::getline(file, line);
                current_slot_packets.push_back(line);
            }

            packet_sequences_raw.push_back(current_slot_packets);
        }
    }
}
