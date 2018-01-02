#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);
    demodulator_ = new demodulator;
    timer_ = new QTimer;
    filePathWidget_ = new QWidget;
    filePathButton_ = new QPushButton("OK");
    filePathLabel_ = new QLabel("Input file path:");
    filePathLine_ = new QLineEdit;
    hbLayout_ = new QHBoxLayout;
    hbLayout_->addWidget(filePathLabel_);
    hbLayout_->addWidget(filePathLine_);
    hbLayout_->addWidget(filePathButton_);
    filePathWidget_->setLayout(hbLayout_);
    ui_->start_button->setEnabled(0);
    connect(timer_, SIGNAL(timeout()), this, SLOT(timer_overflow()));
    connect(ui_->action_3, SIGNAL(triggered(bool)), filePathWidget_, SLOT(show()));
    connect(ui_->action_4, SIGNAL(triggered(bool)), this, SLOT(socket_resourse()));
    connect(filePathButton_, SIGNAL(clicked()), SLOT(file_resourse()));
    connect(filePathButton_, SIGNAL(clicked()), filePathWidget_, SLOT(close()));
}

void MainWindow::file_resourse(){
    demodulator_->setVisitor(new readFromFile(demodulator_->buffer, filePathLine_->text()));
    ui_->start_button->setEnabled(1);
}
void MainWindow::socket_resourse(){
    demodulator_->setVisitor(new readFromUdpSocket(demodulator_->buffer));
    ui_->start_button->setEnabled(1);
}

MainWindow::~MainWindow(){
    delete ui_;
}

void MainWindow::on_start_button_clicked(){
    timer_->start(20);
}

void MainWindow::timer_overflow(){
    demodulator_->proccessNextPart();
    plotSpectrumGraph();
    plotSignalGraph();
    plotPhaseDiagramm();
    ui_->line_edit_frequency->setText(QString::number(demodulator_->getCarrierFrequency()));
}

void MainWindow::plotPhaseDiagramm(){
    ui_->phase_diagramm_widget->clearGraphs();
    ui_->phase_diagramm_widget->addGraph();
    ui_->phase_diagramm_widget->graph(0)->setData(QVector<double>::fromStdVector(demodulator_->pointsForPhaseDiagramCos), QVector<double>::fromStdVector(demodulator_->pointsForPhaseDiagramSin));
    ui_->phase_diagramm_widget->xAxis->setRange(-1, 1);
    ui_->phase_diagramm_widget->yAxis->setRange(-1, 1);
    ui_->phase_diagramm_widget->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui_->phase_diagramm_widget->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,1));
    ui_->phase_diagramm_widget->replot();
}
void MainWindow::plotSignalGraph(){
    ui_->signal_widget->clearGraphs();
    ui_->signal_widget->addGraph();
    ui_->signal_widget->graph(0)->setData(QVector<double>::fromStdVector(demodulator_->coordinateXForSignalGraph), QVector<double>::fromStdVector(demodulator_->vectorForSignal));
    ui_->signal_widget->xAxis->setRange(0, 2048);
    ui_->signal_widget->yAxis->setRange(-1.1,1.1);
    ui_->signal_widget->replot();
}
void MainWindow::plotSpectrumGraph(){
    ui_->spectrum_widget->clearGraphs();
    ui_->spectrum_widget->addGraph();
    ui_->spectrum_widget->graph(0)->setData(QVector<double>::fromStdVector(demodulator_->frequencyForSpectrumGraph), QVector<double>::fromStdVector(demodulator_->vectorForSpectrum));
    ui_->spectrum_widget->xAxis->setRange(0,4000);
    ui_->spectrum_widget->yAxis->setRange(-200,5);
    ui_->spectrum_widget->xAxis->setLabel("Frequency, Hz");
    ui_->spectrum_widget->yAxis->setLabel("dB");
    ui_->spectrum_widget->replot();
}
