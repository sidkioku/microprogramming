#include "machinecode.h"

#include <QDebug>

//TODO: change the opcode column to the way it is in ram
//TODO: OK and Cancel don't work properly
instructionSet::instructionSet(QWidget *parent) : QWidget(parent)
{
    this->setWindowTitle("Machine Code Instruction Set");
    instructionsTable = new QTableWidget(1, 3, this);
    hLabels << "Op Code HEX" << "Mnemonic" << "Microcode Row";
    instructionsTable->setHorizontalHeaderLabels(hLabels);
    instructionsTable->verticalHeader()->hide();

    QSpinBox *opcode = new QSpinBox(this);
    opcode->setInputMethodHints(Qt::ImhDigitsOnly);
    opcode->setPrefix("0x");
    opcode->setDisplayIntegerBase(16);
    opcode->setMinimum(0);
    opcode->setMaximum(255);
    instructionsTable->setCellWidget(0, 0, opcode);
    QTableWidgetItem *mnemonic = new QTableWidgetItem("");
    instructionsTable->setItem(0, 1, mnemonic);
    QSpinBox *microcodeRow = new QSpinBox(this);
    microcodeRow->setInputMethodHints(Qt::ImhDigitsOnly);
    instructionsTable->setCellWidget(0, 2, microcodeRow);


    currentInstructions.resize(instructionsTable->rowCount());
    for (int row = 0; row < instructionsTable->rowCount(); row++)
    {
        currentInstructions[row].resize(instructionsTable->columnCount());
    }
    addRowButton = new QPushButton("Add a row");
    addRowButton->setCursor(Qt::PointingHandCursor);
    okButton = new QPushButton("OK");
    okButton->setCursor(Qt::PointingHandCursor);
    cancelButton = new QPushButton("Cancel");
    cancelButton->setCursor(Qt::PointingHandCursor);
    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(addRowButton);
    hLayout->addStretch();
    hLayout->addWidget(okButton);
    hLayout->addWidget(cancelButton);
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addWidget(instructionsTable);
    vLayout->addLayout(hLayout);
    this->setLayout(vLayout);

    connect(addRowButton, SIGNAL(clicked()), this, SLOT(addRow()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(ok()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
}


void instructionSet::addRow()
{
    instructionsTable->setRowCount(instructionsTable->rowCount() + 1);
    currentInstructions.resize(instructionsTable->rowCount());
    for (int row = 0; row < instructionsTable->rowCount(); row++)
    {
        currentInstructions[row].resize(instructionsTable->columnCount());
        for (int col = 0; col < instructionsTable->columnCount(); col++)
        {
            currentInstructions[row][col] = "0";
        }
    }
    QTableWidgetItem *item = new QTableWidgetItem();
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    QString hex = QString("0x%1").arg(instructionsTable->rowCount() - 1, 2, 16, QChar('0'));
    item->setText(hex);
    instructionsTable->setItem(instructionsTable->rowCount() - 1, 0, item);
    QSpinBox *spinBox = new QSpinBox(this);
    spinBox->setInputMethodHints(Qt::ImhDigitsOnly);
    instructionsTable->setCellWidget(instructionsTable->rowCount() - 1, 2, spinBox);
}

void instructionSet::ok()
{
    for (int row = 0; row < instructionsTable->rowCount(); row++)
    {
            currentInstructions[row][0] = instructionsTable->cellWidget(row, 0)->property("value").toString();
            currentInstructions[row][1] = instructionsTable->item(row, 1)->text();
            currentInstructions[row][2] = instructionsTable->cellWidget(row, 2)->property("value").toString();
    }
    this->hide();
}

void instructionSet::cancel()
{
    for (int row = 0; row < instructionsTable->rowCount(); row++)
    {
        for (int col = 0; col < instructionsTable->columnCount(); col++)
        {
            instructionsTable->cellWidget(row, 0)->setProperty("value", currentInstructions[row][0].toInt(nullptr, 0));
            instructionsTable->item(row, 1)->setText(currentInstructions[row][1]);
            instructionsTable->cellWidget(row, 2)->setProperty("value", currentInstructions[row][2].toInt(nullptr, 10));
        }
    }
    this->hide();
}
