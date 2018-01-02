#ifndef RECIEVER_H
#define RECIEVER_H

#include <QWidget>
#include <QUdpSocket>
#include <QDebug>

class receiver
{
public:
    receiver(unsigned char* _buffer) : demodBuf_(_buffer){
        udpsocket_ = new QUdpSocket;
        udpsocket_->bind(45454);
        datagram_.resize(4096);
    }
    void reading(){
        while(true){
            if(udpsocket_->hasPendingDatagrams()){
                udpsocket_->readDatagram(datagram_.data(), datagram_.size());
                for(int j = 0; j < 4096; j++){
                    demodBuf_[j] = datagram_[j+1];
                    demodBuf_[j+1] = datagram_[j];
                }
                break;
            }
        }
    }
private:
    QUdpSocket* udpsocket_;
    unsigned char* demodBuf_;
    QByteArray datagram_;
};

#endif // RECIEVER_H
