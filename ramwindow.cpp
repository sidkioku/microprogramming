#include "ramwindow.h"
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QSpinBox>
#include <QCloseEvent>
#include <algorithm>

ramWindow::ramWindow(QWidget *parent) : QWidget(parent)
{
    instructions = new instructionSet();
    this->setWindowTitle("External Random Access Memory (RAM)");
    table = new QTableWidget(2000, 4, this);
    hLabels << "Operation Code" << "Register 1" << "Register 2" << "Register 3";
    table->setHorizontalHeaderLabels(hLabels);
    table->verticalHeader()->hide();
    for (int column = 0; column < table->columnCount(); column++) {
        for (int row = 0; row < table->rowCount(); row++)
        {
            QSpinBox *spinBox = new QSpinBox(this);
            spinBox->setInputMethodHints(Qt::ImhDigitsOnly);
            spinBox->setDisplayIntegerBase(16);
            spinBox->setMaximum(255);
            spinBox->setMinimum(0);
            spinBox->setPrefix("0x");
            connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ramWindow::cellChanged);
            table->setCellWidget(row, column, spinBox);
        }
    }


    table->setSortingEnabled(false);
    table->resizeColumnsToContents();
    machineCodeButton = new QPushButton("Instruction\nSet");
    okButton = new QPushButton("OK");
    cancelButton = new QPushButton("Cancel");
    applyButton = new QPushButton("Apply");
    resetButton = new QPushButton("Reset");
    resetButton->setDisabled(true);
    okButton->setCursor(Qt::PointingHandCursor);
    cancelButton->setCursor(Qt::PointingHandCursor);
    applyButton->setCursor(Qt::PointingHandCursor);
    resetButton->setCursor(Qt::PointingHandCursor);

    QVBoxLayout *vLayout = new QVBoxLayout();
    QHBoxLayout *hLayout = new QHBoxLayout();
    vLayout->addWidget(machineCodeButton);
    vLayout->addStretch();
    vLayout->addWidget(resetButton);
    vLayout->addWidget(okButton);
    vLayout->addWidget(applyButton);
    vLayout->addWidget(cancelButton);
    hLayout->addWidget(table);
    hLayout->addLayout(vLayout);
    this->setLayout(hLayout);
    this->resize(400, 500);

    connect(okButton, SIGNAL(clicked()), this, SLOT(ok()));
    connect(applyButton, SIGNAL(clicked()), this, SLOT(apply()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(resetButton, SIGNAL(clicked()), this, SLOT(reset()));
    connect(machineCodeButton, SIGNAL(clicked()), this, SLOT(machineCodeWindow()));
}

ramWindow::~ramWindow()
{
    delete instructions;
}

QString ramWindow::saveRam()
{
    //TODO: Save Instruction Set
    QString text = QStringLiteral("%1\n\n").arg(table->rowCount());

    for (int row = 0; row < table->rowCount(); row++)
    {
        text.append(table->cellWidget(row, 0)->property("value").toString());
        text.append("\n");
    }

    return text;
}

void ramWindow::readRam(QString *text)
{
    //TODO: Read Instruction Set
    table->clear();
    QStringList lines = text->split("\n", Qt::SkipEmptyParts);
    table->setRowCount(lines[0].toInt());

    for (int row = 0; row < table->rowCount(); row++)
    {
        for (int column = 0; column < table->columnCount(); column++)
        {
            QSpinBox *spinBox = new QSpinBox(this);
            spinBox->setInputMethodHints(Qt::ImhDigitsOnly);
            spinBox->setDisplayIntegerBase(16);
            spinBox->setMaximum(255);
            spinBox->setMinimum(0);
            spinBox->setValue(lines[row + 1].toInt());
            connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ramWindow::cellChanged);
            table->setCellWidget(row, column, spinBox);
        }

    }
}

int ramWindow::getMicrocodeRow(int opcode)
{
    for (int row = 0; row < instructions->currentInstructions.size(); row++)
    {
        if (instructions->currentInstructions[row][0] == opcode)
        {
            return instructions->currentInstructions[row][3].toInt(nullptr, 10);
        }
    }
    return 0;
}


void ramWindow::ok()
{
    apply();
    this->hide();
}

void ramWindow::apply()
{
    //TODO: Save Instruction Set
    for (int row = 0; row < table->rowCount(); row++)
    {
        for (int column = 0; column < table->columnCount(); column++)
        {
            bool converted = true;
            currentRAM[row][column] = table->cellWidget(row, column)->property("value").toInt(&converted);
            if (!converted) currentRAM[row][column] = 0;

        }
    }
    applyButton->setDisabled(true);
}

void ramWindow::cancel()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Revert changes", "Are you sure you want to cancel and quit the RAM? The changes you've made will not be saved.");
    if (reply == QMessageBox::Yes)
    {
        for (int row = 0; row < table->rowCount(); row++)
        {
            for(int column = 0; column < table->columnCount(); column++)
            {
                table->cellWidget(row, column)->setProperty("value", currentRAM[row][column]);
            }
        }
        this->hide();
    }
}

void ramWindow::reset()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "This will clear all elements in the table.", "Are you sure you want to reset the RAM?");
    if (reply == QMessageBox::Yes)
    {
        for (int row = 0; row < table->rowCount(); row++)
        {
            for (int column = 0; column < table->columnCount(); column ++)
            {
                table->cellWidget(row, column)->setProperty("value", 0);
                currentRAM[row][column] = 0;

            }
        }
        resetButton->setDisabled(true);
        applyButton->setDisabled(true);
    }
}

void ramWindow::cellChanged(int value)
{
    if (value != 0) resetButton->setEnabled(true);
    applyButton->setEnabled(true);
}

void ramWindow::machineCodeWindow()
{
    instructions->show();
}

void ramWindow::closeEvent(QCloseEvent *bar)
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Revert changes", "Are you sure you want to cancel and quit the RAM? The changes you've made will not be saved.");
    if (reply == QMessageBox::Yes)
    {
        for (int row = 0; row < table->rowCount(); row++)
        {
            for(int column = 0; column < table->columnCount(); column++)
            {
                table->cellWidget(row, column)->setProperty("value", currentRAM[row][column]);
            }
        }
        this->hide();
        bar->accept();
        instructions->hide();
    }
    else {
        bar->ignore();
    }
}
