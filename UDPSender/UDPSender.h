#ifndef UDPSENDER_H
#define UDPSENDER_H

#include <QUdpSocket>
#include <thread>
#include <chrono>
#include <QFile>
#include <string>

class SignalSource{
    QFile* file_;
public:
    SignalSource(std::string file_path){
        udpsocket_ = new QUdpSocket;
        file_ = new QFile(QString::fromStdString(file_path));
        file_->open(QIODevice::ReadOnly);
    }
    ~SignalSource(){
        file_->close();
    }
    void startSending(){
        static int counter = 0;
        while(true){
            if(counter * 4096 > file_->size()){
                file_->seek(0);
                counter = 0;
            }
            else{
                file_->seek(counter * 4096);
                counter++;
            }
            for(unsigned int i = 0; i < 50000000; i++);
            QByteArray datagram = file_->read(4096);
            udpsocket_->writeDatagram(datagram, QHostAddress::LocalHost, 45454);
        }
    }
private:
    QUdpSocket* udpsocket_;
};

#endif // UDPSENDER_H
