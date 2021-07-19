#include "cpu.h"
#include "microcoderom.h"

microcodeROM::microcodeROM(QWidget *parent) : QWidget(parent)
{
    this->setWindowTitle("Microcode ROM");
    table = new QTableWidget(100, 29, this);
    hLabels << "next" << "cond" << "alu.opcode" << "ir.we" << "ir.oe" << "pc.we" << "pc.oe" << "a.we" << "a.oe" << "b.we" << "b.oe" << "c.we" << "c.oe" << "d.we" << "d.oe" << "x.we" << "y.we" << "z.we" << "z.oe" << "carry=0" <<
               "mar.we"  << "mdrin.we" << "mdrin.oe" << "mdrout.we" << "mdrout.oe" << "gpioOut1" << "gpioOut2" << "mem.r/-w" << "mem.en";
    table->setHorizontalHeaderLabels(hLabels);
    table->setShowGrid(false);
    QHeaderView *header = table->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);
    table->setSortingEnabled(false);
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
    QLabel *aluInfo = new QLabel("<b><i>ALU Operations</i></b><br/>0: Do nothing<br/>1: X + Y<br/>10: X + Y + Carry<br/>11: X - Y<br/>100: Shift X left<br/>101: Shift X right<br/>110: Pass X<br/>111: Increment X<br/>1000: Decrement X<br/>1001: X AND Y<br/>1010: X OR Y<br/>1011: X XOR Y<br/>1100: Invert X");
    QLabel *condInfo = new QLabel("<b><i>Next Row Conditions</i></b><br/>0: Do nothing<br/>1: Z == 0<br/>10: Z &gt; 0<br/>11: Z &lt; 0<br/>100: Z &gt;= 0<br/>101: Z &lt;= 0<br/>110: 4 LSBs of Z<br/>111: 4 MSBs of IR<br/>1000: GPIO In 1<br/>1001: GPIO In 2<br/>");
    buttonsLayout->addWidget(aluInfo);
    buttonsLayout->addWidget(condInfo);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(resetButton);
    buttonsLayout->addWidget(addRowButton);
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(applyButton);
    buttonsLayout->addWidget(cancelButton);
    vLayout->addWidget(table);
    vLayout->addLayout(buttonsLayout);

    applied = false;
    this->setLayout(vLayout);
}

QString microcodeROM::saveRom()
{
    QString text = QStringLiteral("%1\n").arg(table->rowCount());
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
    table->setRowCount(lines[0].toInt());
    currentMROM.resize(table->rowCount(), std::vector<int>(table->columnCount()));
    table->setHorizontalHeaderLabels(hLabels);
    vLabelsBinary.clear();
    vLabelsHex.clear();
    for (int row = 0; row < table->rowCount(); row++)
    {
        vLabelsBinary << QString::number(row, 2);
        vLabelsHex << QString("0x%1").arg(row, 2, 16, QChar('0'));
        QStringList line = lines[row + 1].split("\t", Qt::SkipEmptyParts);
        for (int column = 0; column < table->columnCount(); column++)
        {
            QSpinBox *spinBox = new QSpinBox(this);
            spinBox->setInputMethodHints(Qt::ImhDigitsOnly);
            QString tooltip = table->horizontalHeaderItem(column)->text();
            spinBox->setToolTip(tooltip);
            spinBox->setDisplayIntegerBase(2);
            spinBox->setSpecialValueText(" ");
            switch (column)
            {
            case 0:
                //next
                spinBox->setMaximum(65535);
                spinBox->setMinimum(0);
                break;
            case 1:
                //condition
                spinBox->setMaximum(9); // ==0, >0, <0, >=0, <=0, LSBs of Z, MSBs of R, GPIO In 1, GPIO In 2
                spinBox->setMinimum(0);
                break;
            case 2:
                //ALU Operations
                spinBox->setMaximum(12); //ADD, ADD with Carry, SUB, SHIFTLEFT, SHIFTRIGHT, PASS, COMPARE, INCREMENT, DECREMENT, AND, OR, XOR, INVERT (in this order)
                break;
            default:
                spinBox->setMaximum(1);
            }
            spinBox->setValue(line[column].toInt());
            connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &microcodeROM::cellChanged);
            if (row % 2 == 0) spinBox->setStyleSheet("QSpinBox {background-color: rgb(204,204,204);}");
            table->setCellWidget(row, column, spinBox);
        }
    }
    table->setVerticalHeaderLabels(vLabelsBinary);
    table->resizeColumnsToContents();
    apply();
    resetButton->setEnabled(true);
}

void microcodeROM::changeBase(bool binary)
{
    if (binary)
    {
        for (int row = 0; row < table->rowCount(); row++)
        {
            for (int column = 0; column < 3; column++)
            {
                table->cellWidget(row, column)->setProperty("displayIntegerBase", 2);
                table->cellWidget(row, column)->setProperty("prefix", "");
            }
        }
        table->setVerticalHeaderLabels(vLabelsBinary);
    }
    else
    {
        for (int row = 0; row < table->rowCount(); row++)
        {
            for (int column = 0; column < 3; column++)
            {
                table->cellWidget(row, column)->setProperty("displayIntegerBase", 16);
                table->cellWidget(row, column)->setProperty("prefix", "0x");
            }
        }
        table->setVerticalHeaderLabels(vLabelsHex);
    }
}


void microcodeROM::ok()
{
    apply();
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
    applied = true;

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
        for (int row = 6; row < table->rowCount(); row++)
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
        currentMROM.resize(table->rowCount(), std::vector<int>(table->columnCount()));
        vLabelsBinary << QString::number(table->rowCount() - 1, 2);
        vLabelsHex << QString("0x%1").arg(table->rowCount() - 1, 2, 16, QChar('0'));
        for (int column = 0; column < table->columnCount(); column++)
        {
            QSpinBox *spinBox = new QSpinBox(this);
            spinBox->setInputMethodHints(Qt::ImhDigitsOnly);
            QString tooltip = table->horizontalHeaderItem(column)->text();
            spinBox->setToolTip(tooltip);
            spinBox->setDisplayIntegerBase(table->cellWidget(0, 0)->property("displayIntegerBase").toInt());
            spinBox->setSpecialValueText(" ");
            switch (column)
            {
            case 0:
                //next
                spinBox->setMaximum(65535);
                spinBox->setMinimum(0);
                break;
            case 1:
                //condition
                spinBox->setMaximum(9); // ==0, >0, <0, >=0, <=0, LSBs of Z, MSBs of R, GPIO In 1, GPIO In 2
                spinBox->setMinimum(0);
                break;
            case 2:
                //ALU Operations
                spinBox->setMaximum(12); //ADD, ADD with Carry, SUB, SHIFTLEFT, SHIFTRIGHT, PASS, COMPARE, INCREMENT, DECREMENT, AND, OR, XOR, INVERT (in this order)
                break;
            default:
                spinBox->setMaximum(1);
            }
            connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &microcodeROM::cellChanged);
            if ((table->rowCount() - 1) % 2 == 0) spinBox->setStyleSheet("QSpinBox {background-color: rgb(204,204,204);}");
            table->setCellWidget(table->rowCount() - 1, column, spinBox);
        }
        if (table->cellWidget(0, 0)->property("displayIntegerBase").toInt() == 2) {
            table->setVerticalHeaderLabels(vLabelsBinary);
        } else table->setVerticalHeaderLabels(vLabelsHex);
        apply();
        if (table->rowCount() == 65535) addRowButton->setDisabled(true);
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
