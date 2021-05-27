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
#include <QLabel>
#include <QPainter>
#include <QPen>
#include <QThread>
#include <QCloseEvent>
#include <QPalette>
#include <QCheckBox>
#include <QFileDialog>
#include <QDir>
#include <QRandomGenerator>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QTableWidget>
#include <vector>
#include <algorithm>
#include <QRadioButton>
#include <iostream>
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
    int currentRow;
    int currentInstruction;

    QFrame *bus;
    QFrame *data;


    QFrame *leftBorder;
    QFrame *rightBorder;

    QPushButton *nextInstructionButton;
    QIcon *playIcon;
    QIcon *pauseIcon;
    QPushButton *nextStepButton;
    QPushButton *resetButton;
    QPushButton *openMicrocode;
    QPushButton *saveMicrocode;
    QPushButton *openRAM;
    QPushButton *saveRAM;

    QPushButton *ramButton;

    QPushButton *instructionReg;
    QPushButton *instructionReg1;
    QPushButton *instructionReg2;
    QPushButton *instructionReg3;
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

    QRadioButton *gpioOut1;
    QRadioButton *gpioOut2;

    QRadioButton *gpioIn1;
    QRadioButton *gpioIn2;

    QLabel *busLabel;
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
    void nextInstruction();
    void nextStep();
    void reset();
    void irExp();
    void pcExp();
    void aRegExp();
    void aluExp();
    void marExp();
    void mdrInExp();
    void mdrOutExp();
    void ramOpen();
    void microcodeFile();
    void saveRom();
    void ramFile();
    void saveRam();
    void gpioClick();

};
#endif // CPU_H
