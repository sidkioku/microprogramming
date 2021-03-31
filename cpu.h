#ifndef CPU_H
#define CPU_H

#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <vector>
#include <QMessageBox>
#include <QtCore>
#include <QGroupBox>
#include "microcoderom.h"
#include "ramwindow.h"

class CPU : public QMainWindow
{
    Q_OBJECT

public:
    CPU(QWidget *parent = nullptr);
    ~CPU();

private:
    bool running;
    microcodeROM *microcode;
    ramWindow *ram;
    QPushButton *romButton;

    QFrame *bus;
    QFrame *data;

    QPushButton *gpioIn1;
    QPushButton *gpioIn2;

    QPushButton *gpioOut1;
    QPushButton *gpioOut2;
    QPushButton *gpioOut3;

    QFrame *leftBorder;
    QFrame *rightBorder;

    QPushButton *playpauseButton;
    QIcon *playIcon;
    QIcon *pauseIcon;
    QPushButton *nextStepButton;
    QPushButton *resetButton;
    QPushButton *readMicrocode;
    QPushButton *readRAM;
    QPushButton *writeRAM;

    QPushButton *ramButton;

    QPushButton *instructionReg;
    QPushButton *progCounter;
    QPushButton *aReg;

    QPushButton *xReg;
    QPushButton *yReg;
    QPushButton *aluButton;
    QPushButton *comparisons;
    QPushButton *zReg;

    QPushButton *marReg;
    QPushButton *mdrInReg;
    QPushButton *mdrOutReg;

    QLabel *irLabel;
    QLabel *pcLabel;
    QLabel *aLabel;
    QLabel *xLabel;
    QLabel *yLabel;
    QLabel *zLabel;
    QLabel *marLabel;
    QLabel *mdrInLabel;
    QLabel *mdrOutLabel;
    QPushButton *busButton;
    QBoxLayout* drawAlu();

    void closeEvent(QCloseEvent *event);

protected:
    void paintEvent(QPaintEvent *e);
private slots:
    void microcodeOpen();
    void playPause(bool checked);
    void nextStep();
    void irExp();
    void pcExp();
    void aRegExp();
    void aluExp();
    void marExp();
    void mdrInExp();
    void mdrOutExp();
    void ramOpen();
    void microcodeFile();
    void ramFile();
    void saveRam();

};
#endif // CPU_H
