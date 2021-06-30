#include "cpu.h"
#include "microcoderom.h"
microcodeROM::microcodeROM(QWidget *parent) : QWidget(parent)
{
    this->setWindowTitle("Microcode ROM");
    table = new QTableWidget(100, 28, this);
    hLabels << "next" << "cond" << "alu.opcode" << "ir.we" << "ir.oe" << "pc.we" << "pc.oe" << "a.we" << "a.oe" << "b.we" << "b.oe" << "c.we" << "c.oe" << "d.we" << "d.oe" << "x.we" << "y.we" << "z.we" << "z.oe" <<
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
    QLabel *aluInfo = new QLabel("<b><i>ALU Operations</i></b><br/>Do nothing: 0<br/>X + Y: 1<br/>X - Y: 2<br/>Shift X left: 3<br/>Shift X right: 4<br/>Pass X: 5<br/>Increment X: 6<br/>Decrement X: 7<br/>X AND Y: 8<br/>X OR Y: 9<br/>X XOR Y: 10<br/>Invert X: 11");
    QLabel *condInfo = new QLabel("<b><i>Next Row Conditions</i></b><br/>Do nothing: 0<br/>Z == 0: 1<br/>Z &gt; 0: 2<br/>Z &lt; 0: 3<br/>Z &gt;= 0: 4<br/>Z &lt;= 0: 5<br/>3 LSBs of Z: 6<br/>3 MSBs of IR: 7<br/>GPIO In 1: 8<br/>GPIO In 2: 9<br/>");
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
//            spinBox->setToolTip(table->takeHorizontalHeaderItem(column)->text());
            spinBox->setDisplayIntegerBase(2);
            spinBox->setSpecialValueText(" ");
            switch (column)
            {
            case 0:
                //next
                spinBox->setMaximum(table->rowCount() - 1);
                spinBox->setMinimum(0);
                break;
            case 1:
                //condition
                spinBox->setMaximum(9); // ==0, >0, <0, >=0, <=0, LSBs of Z, MSBs of R, GPIO In 1, GPIO In 2
                spinBox->setMinimum(0);
                break;
            case 2:
                //ALU Operations
                spinBox->setMaximum(12); //ADD, SUB, SHIFTLEFT, SHIFTRIGHT, PASS, COMPARE, INCREMENT, DECREMENT, AND, OR, XOR, INVERT (in this order)
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
                table->setVerticalHeaderLabels(vLabelsBinary);
            }
        }
    }
    else
    {
        for (int row = 0; row < table->rowCount(); row++)
        {
            for (int column = 0; column < 3; column++)
            {
                table->cellWidget(row, column)->setProperty("displayIntegerBase", 16);
                table->cellWidget(row, column)->setProperty("prefix", "0x");
                table->setVerticalHeaderLabels(vLabelsHex);
            }
        }
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
    if (table->rowCount() < 255)
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
        for (int column = 0; column < table->columnCount(); column++)
        {
            QSpinBox *spinBox = new QSpinBox(this);
    //        spinBox->setToolTip(table->takeHorizontalHeaderItem(column)->text());
            spinBox->setDisplayIntegerBase(2);
            spinBox->setSpecialValueText(" ");
            switch (column) {
            case 0:
                spinBox->setMaximum(table->rowCount() - 1);
                spinBox->setMinimum(0);
                break;
            case 1:
                spinBox->setMaximum(8);
                spinBox->setMinimum(0);
                break;
            case 2:
                spinBox->setMaximum(8); //ALU Operations: ADD, SUB, SHIFTLEFT, SHIFTRIGHT, PASS, COMPARE, INCREMENT, DECREMENT
                break;
            default:
                spinBox->setMaximum(32);
            }
            connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &microcodeROM::cellChanged);
            table->setCellWidget(table->rowCount() - 1, column, spinBox);
        }
    }
    else
    {
        //TODO: Window that says that it can't be done
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
