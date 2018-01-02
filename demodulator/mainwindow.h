#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "demodulator.h"
#include <QTimer>
#include "qcustomplot.h"
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void plotSpectrumGraph();
    void plotSignalGraph();
    void plotPhaseDiagramm();
    //widget for write filepath
    QWidget* filePathWidget_;
    QPushButton* filePathButton_;
    QLabel* filePathLabel_;
    QLineEdit* filePathLine_;
    QHBoxLayout* hbLayout_;
    //widget for create udp connect
private slots:
    void on_start_button_clicked();
    void timer_overflow();
    void file_resourse();
    void socket_resourse();
private:
    Ui::MainWindow *ui_;
    QTimer* timer_;
    demodulator* demodulator_;
};

#endif // MAINWINDOW_H
