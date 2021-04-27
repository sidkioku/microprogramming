#include "ramwindow.h"

ramWindow::ramWindow(QWidget *parent) : QWidget(parent)
{
    instructions = new instructionSet();
    this->setWindowTitle("External Random Access Memory (RAM)");
    table = new QTableWidget(2048, 4, this);
    hLabels << "Operation Code" << "Register 1" << "Register 2" << "Register 3";
    table->setHorizontalHeaderLabels(hLabels);
    table->verticalHeader()->hide();
    table->setSelectionMode(QAbstractItemView::SingleSelection);
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

    currentByte = new QLabel("Current Address: ");

    QVBoxLayout *vLayout = new QVBoxLayout();
    QHBoxLayout *hLayout = new QHBoxLayout();
    vLayout->addWidget(machineCodeButton);
    vLayout->addStretch();
    vLayout->addWidget(currentByte);
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
    connect(table, SIGNAL(itemSelectionChanged()), this, SLOT(byte()));
}

ramWindow::~ramWindow()
{
    delete instructions;
}

QString ramWindow::saveRam()
{
    QString text = QString("%1\n").arg(table->rowCount());

    for (int row = 0; row < table->rowCount(); row++)
    {
        for (int col = 0; col < table->columnCount(); col++)
        {
            if (currentRAM[row][col] == 0) text.append("0");
            else text.append(currentRAM[row][col]);
            text.append("\t");
        }
        text.append("\n");
    }

    QString instructionSetInfo = QString("%1").arg(instructions->instructionsTable->rowCount());
    qDebug() << instructionSetInfo;
    text.append(instructionSetInfo);
    text.append("\n");

    for(int row = 0; row < instructions->instructionsTable->rowCount(); row++)
    {
        for (int col = 0; col < 3; col++)
        {

            text.append(instructions->currentInstructions[row][col]);
            text.append("\t");
        }
        text.append("\n");
    }
    qDebug() << text;
    return text;
}

void ramWindow::readRam(QString *text)
{
    table->clear();
    instructions->instructionsTable->clear();
    QStringList lines = text->split("\n", Qt::SkipEmptyParts);
    int row = 0;
    table->setRowCount(lines[row].toInt());
    row++;
    while (row <= table->rowCount())
    {
        QStringList columns = lines[row].split("\t");
        for (int column = 0; column < table->columnCount(); column++)
        {
            QSpinBox *spinBox = new QSpinBox(this);
            spinBox->setInputMethodHints(Qt::ImhDigitsOnly);
            spinBox->setDisplayIntegerBase(16);
            spinBox->setMaximum(255);
            spinBox->setMinimum(0);
            spinBox->setPrefix("0x");
            spinBox->setValue(columns[column].toInt());
            connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ramWindow::cellChanged);
            table->setCellWidget(row, column, spinBox);
        }
        row++;
    }
    //<=2053 tablerowcount 2048 + instructionsrowcount 4 = 2052
    instructions->instructionsTable->setRowCount(lines[row].toInt());
    row++;
    while (row <= table->rowCount() + instructions->instructionsTable->rowCount() + 1)
    {
        QStringList columns = lines[row].split("\t");

        QSpinBox *opcode = new QSpinBox(this);
        opcode->setInputMethodHints(Qt::ImhDigitsOnly);
        opcode->setPrefix("0x");
        opcode->setDisplayIntegerBase(16);
        opcode->setMinimum(0);
        opcode->setMaximum(255);
        opcode->setValue(columns[0].toInt());
        instructions->instructionsTable->setCellWidget(row, 0, opcode);


        QTableWidgetItem *mnemonic = new QTableWidgetItem(columns[1]);
        instructions->instructionsTable->setItem(row, 1, mnemonic);

        QSpinBox *microcodeRow = new QSpinBox(this);
        microcodeRow->setInputMethodHints(Qt::ImhDigitsOnly);
        microcodeRow->setValue(columns[2].toInt());
        instructions->instructionsTable->setCellWidget(row, 2, microcodeRow);

        row++;
    }

}

int ramWindow::getMicrocodeRow(int opcode)
{
    for (int row = 0; row < (int) instructions->currentInstructions.size(); row++)
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
    for (int row = 0; row < table->rowCount(); row++)
    {
        for (int column = 0; column < table->columnCount(); column++)
        {
            bool converted = true;
            currentRAM[row][column] = table->cellWidget(row, column)->property("value").toInt(&converted);
            qDebug() << currentRAM[row][column];
            if (currentRAM[row][column]) currentRAM[row][column] = 0;

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

void ramWindow::byte()
{
    QItemSelectionModel *itemModel = table->selectionModel();
    QModelIndexList indexList = itemModel->selectedIndexes();
    if (indexList.isEmpty())  return;
    int row = indexList.at(0).row();
    int col = indexList.at(0).column();
    int cell = row * 4 + col;
    currentByte->setText(QString("Current Address: 0x%1").arg(cell, 4, 16, QChar('0')));
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
