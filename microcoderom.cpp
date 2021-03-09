#include "microcoderom.h"
#include <QTableWidget>
#include <QMessageBox>
#include <QSpinBox>
#include <QLabel>


microcodeROM::microcodeROM(QWidget *parent)
{
    this->setWindowTitle("Microcode ROM");
    QWidget *widget = new QWidget;
    table = new QTableWidget(100, 19, this);
    QStringList hLabels;
    hLabels << "next" << "cond" << "alu.opcode" << "ir.we" << "pc.we" << "pc.oe" << "a.we" << "a.oe" << "x.we" << "y.we" << "z.we" << "z.oe" <<
               "mar.we" << "mar.oe" << "mdrin.we" << "mdrin.oe" << "mdrout.we" << "mem.r/-w" << "mem.en";
    table->setHorizontalHeaderLabels(hLabels);

    for (int j = 0; j < table->columnCount(); j++)
    {
        for (int i= 0; i < table->rowCount(); i++)
        {
            QSpinBox *spinBox = new QSpinBox(this);
            spinBox->setInputMethodHints(Qt::ImhDigitsOnly);
            spinBox->setDisplayIntegerBase(2);
            switch (j) {
            case 0:
                spinBox->setMaximum(table->rowCount());
                spinBox->setMinimum(0);
                break;
            case 1:
                spinBox->setMaximum(8);
                spinBox->setMinimum(0);
                break;
            case 2:
                spinBox->setMaximum(6); //ALU Operations: ADD, SUB, SHIFTLEFT, SHIFTRIGHT, PASS, COMPARE
                break;
            default:
                spinBox->setMaximum(32);
            }
            //   spinBox->setValue(0);
            connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &microcodeROM::cellChanged);
            table->setCellWidget(i, j, spinBox);
        }
    }

    table->setSortingEnabled(false);
    table->resizeColumnsToContents();
    this->resize(950, 500);

    okButton = new QPushButton("OK");
    applyButton = new QPushButton("Apply");
    cancelButton = new QPushButton("Cancel");
    resetButton = new QPushButton("Reset");
    addRowButton = new QPushButton("Add a row to the table");
    resetButton->setDisabled(true);
    currTable.resize(table->rowCount());
    for (int row = 0; row < table->rowCount(); row++)
    {
        currTable[row].resize(table->columnCount());
        for (int col = 0; col < table->columnCount(); col++)
        {
            currTable[row][col] = 0;
        }
    }

    connect(okButton, SIGNAL(clicked()), this, SLOT(on_okButton_clicked()));
    connect(applyButton, SIGNAL(clicked()), this, SLOT(on_applyButton_clicked()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(on_cancelButton_clicked()));
    connect(resetButton, SIGNAL(clicked()), this, SLOT(on_resetButton_clicked()));
    connect(addRowButton, SIGNAL(clicked()), this, SLOT(on_addRowButton_clicked()));



    QHBoxLayout *vLayout = new QHBoxLayout();
    QVBoxLayout *buttonsLayout = new QVBoxLayout();
    QLabel *aluInfo = new QLabel("ALU Operations\nDO NOTHING: 0\nADD: 1\nSUB: 10\nSHIFT LEFT: 11\nSHIFT RIGHT: 100\nPASS: 101\nCOMPARE: 110");
    buttonsLayout->addWidget(aluInfo);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(resetButton);
    buttonsLayout->addWidget(addRowButton);
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(applyButton);
    buttonsLayout->addWidget(cancelButton);
    vLayout->addWidget(table);
    vLayout->addLayout(buttonsLayout);

    widget->setLayout(vLayout);
    this->setCentralWidget(widget);

    //connect(table, SIGNAL(cellChanged(int, int)), this, SLOT(next_Cell_Changed(int, int)));

}

microcodeROM::~microcodeROM()
{
    delete table;
    delete applyButton;
    delete resetButton;
    delete okButton;
    delete cancelButton;
}

void microcodeROM::on_okButton_clicked()
{
    for (int row = 0; row < table->rowCount(); row++)
    {
        for (int column = 0; column < table->columnCount(); column++)
        {
            bool converted = true;
            currTable[row][column] = table->cellWidget(row, column)->property("value").toInt(&converted);
            if (!converted) currTable[row][column] = 0;
        }
    }
    this->hide();
}

void microcodeROM::on_applyButton_clicked()
{
    for (int row = 0; row < table->rowCount(); row++)
    {
        for (int column = 0; column < table->columnCount(); column++)
        {
            bool converted = true;
            currTable[row][column] = table->cellWidget(row, column)->property("value").toInt(&converted);
            if (!converted) currTable[row][column] = 0;
        }
    }
    applyButton->setDisabled(true);
}

void microcodeROM::on_cancelButton_clicked(){
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Revert changes", "Are you sure you want to cancel and quit the MicroCode ROM? The changes you've made will not be saved.");
    if (reply == QMessageBox::Yes)
    {
        for (int row = 0; row < table->rowCount(); row++)
        {
            for (int column = 0; column < table->columnCount(); column++)
            {
                table->cellWidget(row, column)->setProperty("value", currTable[row][column]);
            }
        }
        this->hide();
    }
}

void microcodeROM::on_resetButton_clicked(){
    QMessageBox::StandardButton reply = QMessageBox::question(this, "This will clear all elements in the table.", "Are you sure you want to reset the MicroCode ROM?");
    if (reply == QMessageBox::Yes)
    {
        for (int row = 0; row < table->rowCount(); row++)
        {
            for (int column = 0; column < table->columnCount(); column++)
            {
                table->cellWidget(row, column)->setProperty("value", 0);
                currTable[row][column] = 0;
            }
        }
        resetButton->setDisabled(true);
        applyButton->setDisabled(true);
    }
}

void microcodeROM::on_addRowButton_clicked()
{
    table->setRowCount(table->rowCount() + 1);
    for (int j = 0; j < table->columnCount(); j++)
    {
        QSpinBox *spinBox = new QSpinBox(this);
        spinBox->setDisplayIntegerBase(2);
        switch (j) {
        case 0:
            spinBox->setMaximum(table->rowCount());
            spinBox->setMinimum(0);
            break;
        case 1:
            spinBox->setMaximum(8);
            spinBox->setMinimum(0);
            break;
        case 2:
            spinBox->setMaximum(6); //ALU Operations: ADD, SUB, SHIFTLEFT, SHIFTRIGHT, PASS, COMPARE
            break;
        default:
            spinBox->setMaximum(32);
        }
        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &microcodeROM::cellChanged);
        table->setCellWidget(table->rowCount() - 1, j, spinBox);
    }
}

void microcodeROM::cellChanged(int value)
{
    qDebug("The Cell value was changed to %d", value);
    if (value != 0) resetButton->setEnabled(true);
    applyButton->setEnabled(true);
}


void microcodeROM::closeEvent(QCloseEvent *bar)
{
    on_cancelButton_clicked();
}
