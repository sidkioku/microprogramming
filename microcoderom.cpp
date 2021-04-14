#include "microcoderom.h"


microcodeROM::microcodeROM(QWidget *parent) : QWidget(parent)
{
    this->setWindowTitle("Microcode ROM");
    table = new QTableWidget(100, 19, this);
    hLabels << "next" << "cond" << "alu.opcode" << "ir.we" << "pc.we" << "pc.oe" << "a.we" << "a.oe" << "x.we" << "y.we" << "z.we" << "z.oe" <<
               "mar.we" << "mar.oe" << "mdrin.we" << "mdrin.oe" << "mdrout.we" << "mem.r/-w" << "mem.en";
    table->setHorizontalHeaderLabels(hLabels);

    for (int column = 0; column < table->columnCount(); column++)
    {
        for (int row = 0; row < table->rowCount(); row++)
        {
            QSpinBox *spinBox = new QSpinBox(this);
            spinBox->setInputMethodHints(Qt::ImhDigitsOnly);
            spinBox->setDisplayIntegerBase(2);
            switch (column) {
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
                spinBox->setMaximum(1);
            }
            //   spinBox->setValue(0);
            connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &microcodeROM::cellChanged);
            table->setCellWidget(row, column, spinBox);
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
    currentMROM.resize(table->rowCount());
    for (int row = 0; row < table->rowCount(); row++)
    {
        currentMROM[row].resize(table->columnCount());
        for (int col = 0; col < table->columnCount(); col++)
        {
            currentMROM[row][col] = 0;
        }
    }

    connect(okButton, SIGNAL(clicked()), this, SLOT(ok()));
    connect(applyButton, SIGNAL(clicked()), this, SLOT(apply()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(resetButton, SIGNAL(clicked()), this, SLOT(reset()));
    connect(addRowButton, SIGNAL(clicked()), this, SLOT(addRow()));



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

    this->setLayout(vLayout);


    //connect(table, SIGNAL(cellChanged(int, int)), this, SLOT(next_Cell_Changed(int, int)));

}

QString microcodeROM::saveRom()
{
    QString text = QStringLiteral("%1\t").arg(table->rowCount());
    text.append(QStringLiteral("%1\n").arg(table->columnCount()));
    for (int column = 0; column < table->columnCount(); column++)
    {
        text.append(hLabels[column]);
        text.append("\t");
    }
    text.append("\n");
    for (int row = 0; row < table->rowCount(); row++)
    {
        for (int column = 0; column < table->columnCount(); column++)
        {
            text.append(table->cellWidget(row, column)->property("value").toString());
            text.append("\t");
        }
        text.append("\n");
    }
    return text;
}

void microcodeROM::readRom(QString *text)
{
    table->clear();
    QStringList lines = text->split("\n", Qt::SkipEmptyParts);
    QStringList firstRow = lines[0].split("\t", Qt::SkipEmptyParts);
    table->setRowCount(firstRow[0].toInt());
    table->setColumnCount(firstRow[1].toInt());
    table->setHorizontalHeaderLabels(lines[1].split("\t", Qt::SkipEmptyParts));
    for (int row = 0; row < table->rowCount(); row++)
    {
        QStringList line = lines[row + 2].split("\t", Qt::SkipEmptyParts);
        for (int column = 0; column < table->columnCount(); column++)
        {
            QSpinBox *spinBox = new QSpinBox(this);
            spinBox->setInputMethodHints(Qt::ImhDigitsOnly);
            spinBox->setDisplayIntegerBase(2);
            switch (column)
            {
            case 0:
                //next
                spinBox->setMaximum(table->rowCount());
                spinBox->setMinimum(0);
                break;
            case 1:
                //condition
                spinBox->setMaximum(8);
                spinBox->setMinimum(0);
                break;
            case 2:
                //ALU Operations
                spinBox->setMaximum(6); //ADD, SUB, SHIFTLEFT, SHIFTRIGHT, PASS, COMPARE (in this order)
                break;
            default:
                spinBox->setMaximum(32);
            }
            spinBox->setValue(line[column].toInt());
            connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &microcodeROM::cellChanged);
            table->setCellWidget(row, column, spinBox);
        }
    }
}


void microcodeROM::ok()
{
    for (int row = 0; row < table->rowCount(); row++)
    {
        for (int column = 0; column < table->columnCount(); column++)
        {
            bool converted = true;
            currentMROM[row][column] = table->cellWidget(row, column)->property("value").toInt(&converted);
            if (!converted) currentMROM[row][column] = 0;
        }
    }
    this->hide();
}

void microcodeROM::apply()
{

    for (int row = 0; row < table->rowCount(); row++)
    {
        for (int column = 0; column < table->columnCount(); column++)
        {
            bool converted = true;

            currentMROM[row][column] = table->cellWidget(row, column)->property("value").toInt(&converted);
            if (!converted) currentMROM[row][column] = 0;
        }
    }
    applyButton->setDisabled(true);
}

void microcodeROM::cancel(){
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Revert changes", "Are you sure you want to cancel and quit the MicroCode ROM? The changes you've made will not be saved.");
    if (reply == QMessageBox::Yes)
    {
        for (int row = 0; row < table->rowCount(); row++)
        {
            for (int column = 0; column < table->columnCount(); column++)
            {
                table->cellWidget(row, column)->setProperty("value", currentMROM[row][column]);
            }
        }
        this->hide();
    }
}

void microcodeROM::reset(){
    QMessageBox::StandardButton reply = QMessageBox::question(this, "This will clear all elements in the table.", "Are you sure you want to reset the MicroCode ROM?");
    if (reply == QMessageBox::Yes)
    {
        for (int row = 0; row < table->rowCount(); row++)
        {
            for (int column = 0; column < table->columnCount(); column++)
            {
                table->cellWidget(row, column)->setProperty("value", 0);
                currentMROM[row][column] = 0;
            }
        }
        resetButton->setDisabled(true);
        applyButton->setDisabled(true);
    }
}

void microcodeROM::addRow()
{
    table->setRowCount(table->rowCount() + 1);
    currentMROM.resize(table->rowCount());
    for (int row = 0; row < table->rowCount(); row++)
    {
        currentMROM[row].resize(table->columnCount());
        for (int col = 0; col < table->columnCount(); col++)
        {
            currentMROM[row][col] = 0;
        }
    }
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
    if (value != 0) resetButton->setEnabled(true);
    applyButton->setEnabled(true);
}


void microcodeROM::closeEvent(QCloseEvent *bar)
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Revert changes", "Are you sure you want to cancel and quit the MicroCode ROM? The changes you've made will not be saved.");
    if (reply == QMessageBox::Yes)
    {
        for (int row = 0; row < table->rowCount(); row++)
        {
            for (int column = 0; column < table->columnCount(); column++)
            {
                table->cellWidget(row, column)->setProperty("value", currentMROM[row][column]);
            }
        }
        bar->accept();
    }
    else
    {
        bar->ignore();
    }
}
