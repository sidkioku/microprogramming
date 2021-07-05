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
#include <QTimer>

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

    QPushButton *multipleStepsButton;
    QSpinBox *stepCounter;
    QPushButton *singleStepButton;
    QPushButton *resetButton;
    QPushButton *openMicrocode;
    QPushButton *saveMicrocode;
    QPushButton *openRAM;
    QPushButton *saveRAM;

    QPushButton *ramButton;

    QPushButton *instructionReg;
    QPushButton *progCounter;
    QPushButton *aReg;
    QPushButton *bReg;
    QPushButton *cReg;
    QPushButton *dReg;

    QPushButton *xReg;
    QPushButton *yReg;
    QPushButton *aluButton;
    QPushButton *comparisons;
    QPushButton *zReg;
    QPushButton *carryReg;

    QPushButton *marReg;
    QPushButton *mdrInReg;
    QPushButton *mdrOutReg;

    QPushButton *hexbin;

    QRadioButton *gpioOut1;
    QRadioButton *gpioOut2;

    QRadioButton *gpioIn1;
    QRadioButton *gpioIn2;

    QLabel *busLabel;
    QLabel *irLabel;
    QLabel *pcLabel;
    QLabel *aLabel;
    QLabel *bLabel;
    QLabel *cLabel;
    QLabel *dLabel;
    QLabel *xLabel;
    QLabel *yLabel;
    QLabel *zLabel;
    QLabel *carryLabel;
    QLabel *marLabel;
    QLabel *mdrInLabel;
    QLabel *mdrOutLabel;
    QPushButton *busButton;
    QBoxLayout* drawAlu();
    void executeMicrocodeRow();



    void closeEvent(QCloseEvent *event);

protected:
    void paintEvent(QPaintEvent *e);
private slots:
    void microcodeOpen();
    void multipleSteps();
    void singleStep();
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
    void changeBase();


};
#endif // CPU_H
