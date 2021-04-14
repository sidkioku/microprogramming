#ifndef MACHINECODE_H
#define MACHINECODE_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QSpinBox>

class machineCode : public QWidget
{
    Q_OBJECT
public:
    explicit machineCode(QWidget *parent = nullptr);

private:
    QTableWidget *instructionSet;
//    QPushButton *okButton;
//    QPushButton *cancelButton;
    QStringList hLabels;

private slots:
    void cellChanged(int value);
};

#endif // MACHINECODE_H
