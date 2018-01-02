#ifndef DEMODULATOR_H
#define DEMODULATOR_H

#include <string>
#include <fstream>
#include <iterator>
#include <iostream>
#include <vector>
#include <QVector>
#include <thread>
#include <future>
#include <cmath>
#include <QFile>
#include <QDebug>
#include <functional>
#include <QUdpSocket>
#include <QWidget>
#include "receiver.h"

#define M_PI 3.14159265358979323846

using namespace std;

struct maxParameters{
    maxParameters() : maxSignalValue_(0), maxSpectrumValue_(0), numberOfMaxSpectrumValue_(0){}
    double maxSignalValue_;
    double maxSpectrumValue_;
    int numberOfMaxSpectrumValue_;
};
struct demodulatorParameters{ /* все параметры демодулятора объединяются структурой demodulator_parameters */
    demodulatorParameters(){
        time_ = 0;
        k1_ = 0;
        k2_ = 0.065;
        k_ = 0.1;
        accumulator_ = 0;
        phaseAccumulator_ = 0;
        phase_ = 0;
        argument_ = 0;
        symbolspeed_ = 500;
        phaseInFapchSS_ = 0;
        for(int i = 0; i < 16; i++){
            register_[i] = 0;
            registerCos_[i] = 0;
            registerSin_[i] = 0;
        }
        for(int i = 0; i < 41; i++){
            tmpArrS[i] = 0;
            tmpArrC[i] = 0;
        }
    }
    double k1_;
    double k2_;
    double accumulator_;
    double phaseAccumulator_;
    double phase_;
    double argument_;
    short symbolspeed_;
    double phaseInFapchSS_;
    double k_;
    double filterCoefficients_[41] = { //коэффициенты фильтра
        0.000384306464999,0.0009923015776609, 0.001918968555272, 0.002911646088696,
          0.003478496698612, 0.002918454789223,0.0005014938157125,-0.004236500166047,
          -0.01115602384904, -0.01924668839116, -0.02656130592003, -0.03044101202312,
          -0.02804580633345, -0.01709753587153, 0.003349250100982,  0.03234188499905,
           0.06687729369448,   0.1022722898413,   0.1330200266942,    0.153944797603,
            0.1613639316308,    0.153944797603,   0.1330200266942,   0.1022722898413,
           0.06687729369448,  0.03234188499905, 0.003349250100982, -0.01709753587153,
          -0.02804580633345, -0.03044101202312, -0.02656130592003, -0.01924668839116,
          -0.01115602384904,-0.004236500166047,0.0005014938157125, 0.002918454789223,
          0.003478496698612, 0.002911646088696, 0.001918968555272,0.0009923015776609,
          0.000384306464999
          };
    double tmpArrS[41];
    double tmpArrC[41];
    double register_[16];
    double registerCos_[16], registerSin_[16];
    double time_;
};

class visitor{
protected:
    unsigned char* visitorBuf_;
public:
    virtual ~visitor() = default;
    visitor(unsigned char* _buf) : visitorBuf_(_buf){}
    virtual void read() = 0;
};

class readFromFile : public visitor{
    QFile *file_;
public:
    readFromFile(unsigned char* _buf, QString file_path) : visitor(_buf){
        file_ = new QFile(file_path);
        file_->open(QIODevice::ReadOnly);
    }
    ~readFromFile(){
        file_->close();
        delete file_;
    }
    void read() override{
        static int counter = 0;
        if(counter * 4096 > file_->size()){
            file_->seek(0);
            counter = 0;
        }
        else{
            file_->seek(counter * 4096);
            counter++;
        }
        QByteArray part = file_->read(4096);
        for(int j = 0; j < 4096; j++){
            visitorBuf_[j] = part[j+1];
            visitorBuf_[j+1] = part[j];
        }
    }
};
class readFromUdpSocket : public visitor{
    receiver* receiver_;
    bool flagStart_;
public:
    readFromUdpSocket(unsigned char* _buf) : visitor(_buf){
        receiver_ = new receiver(_buf);
        flagStart_ = false;
        qDebug() << "Created";
    }
    void read() override{
        receiver_->reading();
    }
};


class demodulator
{
public:
    demodulator();
    ~demodulator();
    void proccessNextPart();
    double getCarrierFrequency(){return carrierFrequency_;}
    void readSource();
    void setVisitor(visitor* _visitor){
        visitor_ = _visitor;
    }
public:
    unsigned char* buffer;
    vector<double> vectorForSignal;
    vector<double> vectorForSpectrum;
    vector<double> pointsForPhaseDiagramSin;
    vector<double> pointsForPhaseDiagramCos;
    vector<double> frequencyForSpectrumGraph;
    vector<double> coordinateXForSignalGraph;
private:
    packaged_task<double(double ,int )> task_;
    demodulatorParameters parameters_;
    double carrierFrequency_;
    maxParameters* maxParameters_;
    visitor* visitor_;
    vector<short> vectorForFftCalc_;
private:
    void handler();
    void demodulates();
    double filtration(double* p, int a = -1);
    inline void moveRegister(double value, double* p, int size);
    inline maxParameters getMaxParameters();
    inline void findCarrierFrequency(maxParameters& );
    void fft(int nn);
};

#endif // DEMODULATOR_H
