#include "demodulator.h"

demodulator::demodulator(){
    maxParameters_ = new maxParameters;
    buffer = new unsigned char[4096];
    for(int i = 0; i < 2048; i++){
        frequencyForSpectrumGraph.push_back(2*(4000./4096)*i);
        coordinateXForSignalGraph.push_back(i);
    }
    carrierFrequency_ = 0;
    vectorForSignal.reserve(2048);
    vectorForSpectrum.reserve(2048);
    vectorForFftCalc_.reserve(2048);
}

demodulator::~demodulator(){
    delete visitor_;
    delete buffer;
}

void demodulator::readSource(){
    visitor_->read();
}
void demodulator::proccessNextPart(){
    readSource();
    handler();
}

void demodulator::handler(){
    vectorForSignal.clear();
    vectorForSpectrum.clear();
    vectorForFftCalc_.clear();
    short *converseBuffer;
    converseBuffer = (short*)buffer;
    for (int i=0; i < 2048; i++){
        vectorForFftCalc_.push_back(converseBuffer[i]);
        vectorForSignal.push_back(converseBuffer[i]);
    }
    fft(4096);
    auto maxValues = getMaxParameters();
    findCarrierFrequency(maxValues);
    demodulates();
}

maxParameters demodulator::getMaxParameters(){
    maxParameters object{};
    // поиск максимальных параметров
    for(int i = 0; i < 2048; i++){
        if(vectorForSpectrum[i] > object.maxSpectrumValue_){
            object.maxSpectrumValue_ = vectorForSpectrum[i];
            object.numberOfMaxSpectrumValue_ = i;
        }
        if(abs(vectorForSignal[i]) > object.maxSignalValue_)
            object.maxSignalValue_ = abs(vectorForSignal[i]);
    }
    // нормировка
    for(int i = 0; i < 2048; i++){
        vectorForSpectrum[i] = vectorForSpectrum[i]/object.maxSpectrumValue_;
        vectorForSignal[i] = vectorForSignal[i]/object.maxSignalValue_;
    }
    // перевод в дБ
    for(int i = 0; i < 2048; i++){
        if(vectorForSpectrum[i] < 0.0001)
            vectorForSpectrum[i] = 0.0001;
        vectorForSpectrum[i] = 20*log(vectorForSpectrum[i]);
    }
    return object;
}
void demodulator::findCarrierFrequency(maxParameters &object){
    if(maxParameters_->maxSpectrumValue_ == 0){
        maxParameters_->maxSpectrumValue_ = object.maxSpectrumValue_;
        maxParameters_->numberOfMaxSpectrumValue_ = object.numberOfMaxSpectrumValue_;
        carrierFrequency_ = 2 * (4000./4096) * maxParameters_->numberOfMaxSpectrumValue_;
        return;
    }
    if(object.maxSpectrumValue_ - maxParameters_->maxSpectrumValue_ >= 0){
        maxParameters_->maxSpectrumValue_ = object.maxSpectrumValue_;
        maxParameters_->numberOfMaxSpectrumValue_ = object.numberOfMaxSpectrumValue_;
        carrierFrequency_ = 2 * (4000./4096) * maxParameters_->numberOfMaxSpectrumValue_;
    }
}
void demodulator::fft(int nn){
    int j, n, m, mmax, istep, beginData = 0;
    unsigned int i;
    double tempr, tempi, wtemp, theta, wpr, wpi, wr, wi;
    int isign = -1;
    vector<double> data(nn*2 + 1);
    j = 0;
    for (i = beginData; i < beginData + nn; i++){
        if (i < vectorForFftCalc_.size()){
            data[j*2]   = 0;
            data[j*2+1] = vectorForFftCalc_[i];
        }
        else{
            data[j*2]   = 0;
            data[j*2+1] = 0;
        }
        j++;
    }
    n = nn << 1;
    j = 1;
    i = 1;
    while (i < n){
        if (j > i){
            tempr = data[i];   data[i]   = data[j];   data[j]   = tempr;
            tempr = data[i+1]; data[i+1] = data[j+1]; data[j+1] = tempr;
        }
        m = n >> 1;
        while ((m >= 2) && (j > m)){
            j = j - m;
            m = m >> 1;
        }
        j = j + m;
        i = i + 2;
    }
    mmax = 2;
    while (n > mmax){
        istep = 2 * mmax;
        theta = 2.0*M_PI / (isign * mmax);
        wtemp = sin(0.5 * theta);
        wpr   = -2.0 * wtemp * wtemp;
        wpi   = sin(theta);
        wr    = 1.0;
        wi    = 0.0;
        m    = 1;
        while (m < mmax){
            i = m;
            while (i < n){
                j         = i + mmax;
                tempr     = wr * data[j] - wi * data[j+1];
                tempi     = wr * data[j+1] + wi * data[j];
                data[j]   = data[i] - tempr;
                data[j+1] = data[i+1] - tempi;
                data[i]   = data[i] + tempr;
                data[i+1] = data[i+1] + tempi;
                i         = i + istep;
            }
            wtemp = wr;
            wr    = wtemp * wpr - wi * wpi + wr;
            wi    = wi * wpr + wtemp * wpi + wi;
            m     = m + 2;
        }
        mmax = istep;
    }
    for (i = 0; i < (nn / 2); i++)
        vectorForSpectrum.push_back(sqrt( data[i*2] * data[i*2] + data[i*2+1] * data[i*2+1] ));
}
void demodulator::demodulates(){
        double sinus, cosinus;
        double arrFiltrationSin[2048], arrFiltrationCos[2048], __g1[2048], __g2[2048];
        int alfa;
        double differ;
        pointsForPhaseDiagramCos.clear();
        pointsForPhaseDiagramSin.clear();
        for(int j = 0; j < 2048; j++){
            // FAPCH
            sinus = vectorForSignal[j] * sin(parameters_.argument_);
            cosinus = vectorForSignal[j] * cos(parameters_.argument_);
            moveRegister(cosinus, parameters_.register_, 16);
            if(filtration(parameters_.register_) < 0)
                alfa = 1;
            else
                alfa = -1;
            parameters_.accumulator_ += parameters_.k1_ * sinus;
            parameters_.phase_ =  parameters_.accumulator_ + parameters_.k2_ * sinus;
            parameters_.phaseAccumulator_ += alfa * parameters_.phase_;
            parameters_.argument_ = 2 * M_PI * carrierFrequency_ * parameters_.time_ + parameters_.phaseAccumulator_;
            // STS
            moveRegister(sinus, parameters_.registerSin_, 16);
            moveRegister(cosinus, parameters_.registerCos_, 16);
            arrFiltrationSin[j] = filtration(parameters_.registerSin_, 1);
            arrFiltrationCos[j] = filtration(parameters_.registerCos_, 0);
            __g1[j] = filtration(parameters_.registerSin_, 1);
            __g2[j] = filtration(parameters_.registerCos_, 0);
            if(j!=0){
                if(fabs(arrFiltrationCos[j]) >= fabs(arrFiltrationCos[j-1]) && fabs(arrFiltrationCos[j]) >= fabs(arrFiltrationCos[j+1])){
                    differ = -(fabs(arrFiltrationSin[j - 1]) - fabs(arrFiltrationSin[j + 1]));
                    parameters_.phaseInFapchSS_ = parameters_.phaseInFapchSS_ + parameters_.symbolspeed_ * parameters_.k_ * alfa * differ;
                    pointsForPhaseDiagramSin.push_back(arrFiltrationSin[j]);
                    pointsForPhaseDiagramCos.push_back(arrFiltrationCos[j]);
                }
            }
            parameters_.time_ += 1.0f/8000;
        }
}

double demodulator::filtration(double* p, int a){
    double sum = 0, sum1 = 0;
    for (int j = 0; j < 16; j++)
        sum = sum + p[j];
    sum /= 16;
    if(a == 0){
        moveRegister(sum, parameters_.tmpArrC, 41);
        for(int i = 0; i < 41; i++)
            sum1 += parameters_.filterCoefficients_[i] * parameters_.tmpArrC[i];
    }
    if(a == 1){
        moveRegister(sum, parameters_.tmpArrS, 41);
        for(int i = 0; i < 41; i++)
            sum1 += parameters_.filterCoefficients_[i] * parameters_.tmpArrS[i];
    }
    if(a == 1 || a == 0)
        return sum1;
    return sum;
}
void demodulator::moveRegister(double value, double *p, int size){
    for(int i = size - 1; i > 0; i--)
       p[i] = p[i-1];
    p[0] = value;
}




