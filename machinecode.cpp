#include "machinecode.h"

machineCode::machineCode(QWidget *parent) : QWidget(parent)
{
    this->setWindowTitle("Machine Code Instruction Set");
    instructionSet = new QTableWidget(100, 2, this);
    hLabels << "HEX" << "Operation Code";
    instructionSet->setHorizontalHeaderLabels(hLabels);

    for (int row = 0; row < instructionSet->rowCount(); row++)
    {
        QSpinBox *spinBox = new QSpinBox(this);
        spinBox->setInputMethodHints(Qt::ImhDigitsOnly);
        spinBox->setDisplayIntegerBase(16);
        spinBox->setMinimum(0);
        spinBox->setMaximum(instructionSet->rowCount());
        spinBox->setValue(row);
        instructionSet->setCellWidget(row, 0, spinBox);
        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &machineCode::cellChanged);

    }

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addWidget(instructionSet);
    this->setLayout(vLayout);
}


void machineCode::cellChanged(int value)
{

}
