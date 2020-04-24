#include "mainwindow.h"
//#include "include/yaml-cpp/yaml.h"
#include <QApplication>

#include <iostream>

int main(int argc, char *argv[])
{
//    YAML::Node config = YAML::LoadFile("config.yaml");
//    std::cout<<"VoiceHoldTime:"<< config["System"]["RunParameter"]["VoiceHoldTime"].as<int>()<<std::endl;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
//    return 0;
}
