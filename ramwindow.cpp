#include "ramwindow.h"
#include <QHeaderView>

ramWindow::ramWindow(QWidget *parent) : QWidget(parent)
{
    this->setWindowTitle("External Random Access Memory (RAM)");
    ramTable = new QTableWidget(2048, 4, this);

    ramTable->setToolTip("8kB RAM");
    ramTable->horizontalHeader()->hide();
    ramTable->setShowGrid(false);
    ramTable->verticalHeader()->hide();
    for (int row = 0; row < ramTable->rowCount(); row++) {
        for (int column = 0; column < ramTable->columnCount(); column++)
        {
            QSpinBox *spinBox = new QSpinBox(this);
            spinBox->setInputMethodHints(Qt::ImhDigitsOnly);
            spinBox->setDisplayIntegerBase(2);
            spinBox->setMaximum(255);
            spinBox->setMinimum(0);
            spinBox->setToolTip(QString("Address: %1").arg(row * 4 + column));
            connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ramWindow::cellChanged);
            ramTable->setCellWidget(row, column, spinBox);
            if (row % 2 == 0) spinBox->setStyleSheet("QSpinBox {background-color: rgb(204,204,204);}");
        }
    }
    ramTable->setSortingEnabled(false);
    ramTable->resizeColumnsToContents();
    okButton = new QPushButton("OK");
    cancelButton = new QPushButton("Cancel");
    applyButton = new QPushButton("Apply");
    resetButton = new QPushButton("Reset");
    resetButton->setDisabled(true);
    okButton->setCursor(Qt::PointingHandCursor);
    cancelButton->setCursor(Qt::PointingHandCursor);
    applyButton->setCursor(Qt::PointingHandCursor);
    resetButton->setCursor(Qt::PointingHandCursor);


    QVBoxLayout *buttonsLayout = new QVBoxLayout();
    QHBoxLayout *fullLayout = new QHBoxLayout();
    buttonsLayout->addWidget(resetButton);
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(applyButton);
    buttonsLayout->addWidget(cancelButton);

    instructionsTable = new QTableWidget(1, 4, this);
    instructionsTable->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QStringList instructionsHeaderLabels;
    instructionsHeaderLabels << "Op Code" << "Mnemonic" << "Length in Bytes" << "Microcode Row";
    instructionsTable->setHorizontalHeaderLabels(instructionsHeaderLabels);
    instructionsTable->verticalHeader()->hide();

    QSpinBox *opcode = new QSpinBox(this);
    opcode->setInputMethodHints(Qt::ImhDigitsOnly);
    opcode->setDisplayIntegerBase(2);
    opcode->setMinimum(0);
    opcode->setMaximum(255);
    instructionsTable->setCellWidget(0, 0, opcode);

    QTableWidgetItem *mnemonic = new QTableWidgetItem("");
    instructionsTable->setItem(0, 1, mnemonic);

    QSpinBox *length = new QSpinBox(this);
    length->setInputMethodHints(Qt::ImhDigitsOnly);
    length->setMaximum(4);
    length->setMinimum(1);
    instructionsTable->setCellWidget(0, 2, length);

    QSpinBox *microcodeRow = new QSpinBox(this);
    microcodeRow->setInputMethodHints(Qt::ImhDigitsOnly);
    microcodeRow->setDisplayIntegerBase(2);
    instructionsTable->setCellWidget(0, 3, microcodeRow);


    QPushButton *addMnemonic = new QPushButton("Add another mnemonic");
    addMnemonic->setCursor(Qt::PointingHandCursor);
    ramTable->setMinimumSize(1, 1);
    instructionsTable->setMinimumSize(1, 1);

    buttonsLayout->addStretch();
    buttonsLayout->addWidget(addMnemonic);
    QSplitter *splitter = new QSplitter();
    QHeaderView *ramHeader = ramTable->horizontalHeader();
    ramHeader->setSectionResizeMode(QHeaderView::Stretch);
    QHeaderView *instructionsHeader = instructionsTable->horizontalHeader();
    instructionsHeader->setSectionResizeMode(QHeaderView::Stretch);
    splitter->setOrientation(Qt::Vertical);
    splitter->addWidget(ramTable);
    splitter->addWidget(instructionsTable);

    fullLayout->addWidget(splitter);
    fullLayout->addLayout(buttonsLayout);
    fullLayout->setStretchFactor(buttonsLayout, 0);
    fullLayout->setStretchFactor(splitter, 1);
    this->setLayout(fullLayout);

    this->resize(400, 500);


    currentInstructions.resize(instructionsTable->rowCount());
    for (int row = 0; row < instructionsTable->rowCount(); row++)
    {
        currentInstructions[row].resize(instructionsTable->columnCount());
    }

    connect(okButton, SIGNAL(clicked()), this, SLOT(ok()));
    connect(applyButton, SIGNAL(clicked()), this, SLOT(apply()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(resetButton, SIGNAL(clicked()), this, SLOT(reset()));
    connect(addMnemonic, SIGNAL(clicked()), this, SLOT(addInstruction()));
}

ramWindow::~ramWindow(){}

QString ramWindow::saveRam()
{
    QString text = QString("%1\n").arg(ramTable->rowCount());

    for (int row = 0; row < ramTable->rowCount(); row++)
    {
        for (int col = 0; col < ramTable->columnCount(); col++)
        {
            if (currentRAM[row][col] == NULL) text.append("0");
            else text.append(QString::number(currentRAM[row][col], 10));
            text.append("\t");
        }
        text.append("\n");
    }

    QString instructionSetInfo = QString("%1").arg(instructionsTable->rowCount());
    text.append(instructionSetInfo);
    text.append("\n");
    qDebug() << instructionsTable->columnCount();
    for(int row = 0; row < instructionsTable->rowCount(); row++)
    {
        for (int col = 0; col < instructionsTable->columnCount(); col++)
        {

            text.append(currentInstructions[row][col]);
            text.append("\t");
        }
        text.append("\n");
    }
    return text;
}

void ramWindow::readRam(QString *text)
{
    ///Read RAM Table
    ramTable->clear();
    ramTable->setShowGrid(false);

    QStringList lines = text->split("\n", Qt::SkipEmptyParts);

    int row = 0;
    ramTable->setRowCount(lines[row].toInt());
    row++;
    while (row <= ramTable->rowCount())
    {
        QStringList columns = lines[row].split("\t");
        for (int column = 0; column < ramTable->columnCount(); column++)
        {
            QSpinBox *spinBox = new QSpinBox(this);
            spinBox->setInputMethodHints(Qt::ImhDigitsOnly);
            spinBox->setDisplayIntegerBase(2);
            spinBox->setMaximum(15);
            spinBox->setMinimum(0);
            spinBox->setToolTip(QString("Address: %1").arg((row - 1) * 4 + column));
            spinBox->setValue(columns[column].toInt());
            connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ramWindow::cellChanged);
            ramTable->setCellWidget(row - 1, column, spinBox);
            if ((row - 1) % 2 == 0) spinBox->setStyleSheet("QSpinBox {background-color: rgb(204,204,204);}");
        }
        row++;
    }
    ramTable->setSortingEnabled(false);
    ramTable->resizeColumnsToContents();

    ///Read Instructions Table
    instructionsTable->clear();
    instructionsTable->setColumnCount(4);

    QStringList instructionsHeaderLabels;
    instructionsHeaderLabels << "Op Code" << "Mnemonic" << "Length in Bytes" << "Microcode Row";
    instructionsTable->setHorizontalHeaderLabels(instructionsHeaderLabels);
    instructionsTable->setRowCount(lines[row].toInt());
    row++;
    while (row <= ramTable->rowCount() + instructionsTable->rowCount() + 1)
    {
        QStringList columns = lines[row].split("\t");

        QSpinBox *opcode = new QSpinBox(this);
        opcode->setInputMethodHints(Qt::ImhDigitsOnly);
        opcode->setDisplayIntegerBase(2);
        opcode->setMinimum(0);
        opcode->setMaximum(15);
        opcode->setValue(columns[0].toInt());
        instructionsTable->setCellWidget(row - ramTable->rowCount() - 2, 0, opcode);


        QTableWidgetItem *mnemonic = new QTableWidgetItem(columns[1]);
        instructionsTable->setItem(row - ramTable->rowCount() - 2, 1, mnemonic);

        QSpinBox *length = new QSpinBox(this);
        length->setInputMethodHints(Qt::ImhDigitsOnly);
        length->setMinimum(1);
        length->setMaximum(4);
        length->setValue(columns[2].toInt());
        instructionsTable->setCellWidget(row - ramTable->rowCount() - 2, 2, length);

        QSpinBox *microcodeRow = new QSpinBox(this);
        microcodeRow->setInputMethodHints(Qt::ImhDigitsOnly);
        microcodeRow->setDisplayIntegerBase(2);
        microcodeRow->setValue(columns[3].toInt());
        instructionsTable->setCellWidget(row - ramTable->rowCount() - 2, 3, microcodeRow);

        row++;
        if ((row - ramTable->rowCount() - 2) % 2 == 0)
        {
            opcode->setStyleSheet("QSpinBox {background-color: rgb(204,204,204);}");
            microcodeRow->setStyleSheet("QSpinBox {background-color: rgb(204,204,204);}");
            length->setStyleSheet("QSpinBox {background-color: rgb(204,204,204);}");
            mnemonic->setBackground(QBrush(qRgb(204, 204, 204)));
        }
    }
    currentInstructions.resize(instructionsTable->rowCount());
    for (int row = 0; row < instructionsTable->rowCount(); row++)
    {
        qDebug() << instructionsTable->columnCount();
        currentInstructions[row].resize(instructionsTable->columnCount());

    }
    apply();
    resetButton->setEnabled(true);

}

int ramWindow::getMicrocodeRow(int opcode)
{
    for (int row = 0; row < (int) currentInstructions.size(); row++)
    {
        if (currentInstructions[row][0] == opcode)
        {
            return currentInstructions[row][3].toInt(nullptr, 10);
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
    for (int row = 0; row < ramTable->rowCount(); row++)
    {
        for (int column = 0; column < ramTable->columnCount(); column++)
        {
            currentRAM[row][column] = ramTable->cellWidget(row, column)->property("value").toInt(nullptr);

        }
    }
    for (int row = 0; row < instructionsTable->rowCount(); row++)
    {
            currentInstructions[row][0] = instructionsTable->cellWidget(row, 0)->property("value").toString();
            currentInstructions[row][1] = instructionsTable->item(row, 1)->text();
            currentInstructions[row][2] = instructionsTable->cellWidget(row, 2)->property("value").toString();
            currentInstructions[row][3] = instructionsTable->cellWidget(row, 3)->property("value").toString();
    }
    applyButton->setDisabled(true);
}

void ramWindow::cancel()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Revert changes", "Are you sure you want to cancel and quit the RAM? The changes you've made will not be saved.");
    if (reply == QMessageBox::Yes)
    {
        for (int row = 0; row < ramTable->rowCount(); row++)
        {
            for(int column = 0; column < ramTable->columnCount(); column++)
            {
                ramTable->cellWidget(row, column)->setProperty("value", currentRAM[row][column]);
            }
        }
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
}

void ramWindow::reset()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "This will clear all elements in the table.", "Are you sure you want to reset the RAM?");
    if (reply == QMessageBox::Yes)
    {
        for (int row = 0; row < ramTable->rowCount(); row++)
        {
            for (int column = 0; column < ramTable->columnCount(); column ++)
            {
                ramTable->cellWidget(row, column)->setProperty("value", 0);
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


void ramWindow::addInstruction()
{
    instructionsTable->setRowCount(instructionsTable->rowCount() + 1);
    currentInstructions.resize(instructionsTable->rowCount());
    currentInstructions[instructionsTable->rowCount() - 1].resize(instructionsTable->columnCount());
    QSpinBox *opcode = new QSpinBox(this);
    opcode->setInputMethodHints(Qt::ImhDigitsOnly);
    opcode->setDisplayIntegerBase(2);
    opcode->setMinimum(0);
    opcode->setMaximum(15);
    instructionsTable->setCellWidget(instructionsTable->rowCount() - 1, 0, opcode);

    QTableWidgetItem *mnemonic = new QTableWidgetItem("");
    instructionsTable->setItem(instructionsTable->rowCount() - 1, 1, mnemonic);

    QSpinBox *length = new QSpinBox(this);
    length->setInputMethodHints(Qt::ImhDigitsOnly);
    length->setMinimum(1);
    length->setMaximum(4);
    instructionsTable->setCellWidget(instructionsTable->rowCount() - 1, 2, length);
    QSpinBox *microcodeRow = new QSpinBox(this);
    microcodeRow->setInputMethodHints(Qt::ImhDigitsOnly);
    microcodeRow->setDisplayIntegerBase(2);
    instructionsTable->setCellWidget(instructionsTable->rowCount() - 1, 3, microcodeRow);
    if (instructionsTable->rowCount() % 2 == 0)
    {
        opcode->setStyleSheet("QSpinBox {background-color: rgb(204,204,204);}");
        microcodeRow->setStyleSheet("QSpinBox {background-color: rgb(204,204,204);}");
        length->setStyleSheet("QSpinBox {background-color: rgb(204,204,204);}");
        mnemonic->setBackground(QBrush(qRgb(204, 204, 204)));
    }
}

void ramWindow::closeEvent(QCloseEvent *bar)
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Revert changes", "Are you sure you want to cancel and quit the RAM? The changes you've made will not be saved.");
    if (reply == QMessageBox::Yes)
    {
        for (int row = 0; row < ramTable->rowCount(); row++)
        {
            for(int column = 0; column < ramTable->columnCount(); column++)
            {
                ramTable->cellWidget(row, column)->setProperty("value", currentRAM[row][column]);
            }
        }
        this->hide();
        bar->accept();
    }
    else {
        bar->ignore();
    }
}


void ramWindow::changeValue(int row, int col, int value)
{
    QSpinBox *spinBox = new QSpinBox(this);
    spinBox->setInputMethodHints(Qt::ImhDigitsOnly);
    spinBox->setDisplayIntegerBase(2);
    spinBox->setMaximum(15);
    spinBox->setMinimum(0);
    spinBox->setToolTip(QString("Address: %1").arg(row * 4 + col));
    if (row % 2 == 0) spinBox->setStyleSheet("QSpinBox {background-color: rgb(204,204,204);}");
    ramTable->setCellWidget(row, col, spinBox);
    spinBox->setValue(value);

}
