#!/bin/bash

chmod u+x make.sh
#sudo apt-add-repository ppa:ubuntu-sdk-team/ppa
#sudo apt-get update
sudo apt-get install libxtst-dev build-essential libqt4-dev qt5-qmake qt5-default qttools5-dev-tools
qmake
make
