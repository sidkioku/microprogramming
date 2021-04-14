#ifndef RAMWINDOW_H
#define RAMWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QListWidget>
#include <QTableWidget>
#include "machinecode.h"
class ramWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ramWindow(QWidget *parent = nullptr);
    ~ramWindow();
    QString saveRam();
    void readRam(QString *text);

private:
    QTableWidget *table;
    QPushButton *machineCodeButton;
    QPushButton *okButton;
    QPushButton *applyButton;
    QPushButton *cancelButton;
    QPushButton *resetButton;
    machineCode *instructions;
    int  currentRAM[16];
    int  tempRAM[16];
    void closeEvent(QCloseEvent *bar);



private slots:
    void ok();
    void apply();
    void cancel();
    void reset();
    void cellChanged(int value);
    void machineCodeWindow();


};

#endif // RAMWINDOW_H
