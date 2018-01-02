#include "UDPSender.h"

int main(){
    /* необходимо указать путь к файлу, информация из которого будет поступать по сети на демодулятор*/
    std::string file_path = ".../1.pcm";
    SignalSource signalSource(file_path);
    signalSource.startSending();
}
