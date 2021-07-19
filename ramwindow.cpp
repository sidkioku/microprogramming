#include "ramwindow.h"
#include <QHeaderView>

ramWindow::ramWindow(QWidget *parent) : QWidget(parent)
{
    this->setWindowTitle("External Random Access Memory (RAM)");
    ramTable = new QTableWidget(2048, 4, this);

    ramTable->setToolTip("8kB RAM");
    ramTable->horizontalHeader()->hide();
    ramTable->setShowGrid(false);
    for (int row = 0; row < ramTable->rowCount(); row++) {
        vLabelsBin << QString("%1").arg(row * 4, 0, 2);
        vLabelsHex << QString("0x%1").arg(row * 4, 0, 16);
        for (int column = 0; column < ramTable->columnCount(); column++)
        {
            QSpinBox *spinBox = new QSpinBox(this);
            spinBox->setDisplayIntegerBase(2);
            spinBox->setMaximum(255);
            spinBox->setMinimum(0);
            spinBox->setSpecialValueText(" ");
            spinBox->setToolTip(QString("Address: %1").arg(row * 4 + column));
            connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ramWindow::cellChanged);
            ramTable->setCellWidget(row, column, spinBox);
            if (row % 2 == 0) spinBox->setStyleSheet("QSpinBox {background-color: rgb(204,204,204);}");
        }
    }
    ramTable->setVerticalHeaderLabels(vLabelsBin);
    ramTable->verticalHeader()->setDefaultAlignment(Qt::AlignRight);
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


    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    QVBoxLayout *fullLayout = new QVBoxLayout();
    buttonsLayout->addWidget(resetButton);
    buttonsLayout->addWidget(cancelButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(applyButton);
    buttonsLayout->addWidget(okButton);

    QSplitter *splitter = new QSplitter();
    QHeaderView *ramHeader = ramTable->horizontalHeader();
    ramHeader->setSectionResizeMode(QHeaderView::Stretch);
    splitter->setOrientation(Qt::Vertical);
    splitter->addWidget(ramTable);

    fullLayout->addWidget(splitter);
    fullLayout->addLayout(buttonsLayout);
    fullLayout->setStretchFactor(buttonsLayout, 0);
    fullLayout->setStretchFactor(splitter, 1);
    this->setLayout(fullLayout);

    this->resize(400, 500);


    connect(okButton, SIGNAL(clicked()), this, SLOT(ok()));
    connect(applyButton, SIGNAL(clicked()), this, SLOT(apply()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(resetButton, SIGNAL(clicked()), this, SLOT(reset()));

    applied = false;
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

    return text;
}

void ramWindow::readRam(QString *text)
{
    ///Read RAM Table
    ramTable->clear();
    ramTable->setShowGrid(false);
    ramTable->setHorizontalHeaderLabels(vLabelsBin);

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
            spinBox->setSpecialValueText(" ");
            spinBox->setDisplayIntegerBase(2);
            spinBox->setMaximum(255);
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
    apply();
    resetButton->setEnabled(true);

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
    applyButton->setDisabled(true);
    applied = true;
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
    ramTable->cellWidget(row, col)->setProperty("value", value);
}

void ramWindow::changeBase(bool binary)
{
    if (binary)
    {
        ramTable->setVerticalHeaderLabels(vLabelsBin);
        for (int row = 0; row < ramTable->rowCount(); row++)
        {
            for (int column = 0; column < ramTable->columnCount(); column++)
            {
                ramTable->cellWidget(row, column)->setProperty("displayIntegerBase", 2);
                ramTable->cellWidget(row, column)->setProperty("prefix", "");
            }
        }
    }
    else
    {
        ramTable->setVerticalHeaderLabels(vLabelsHex);
        for (int row = 0; row < ramTable->rowCount(); row++)
        {
            for (int column = 0; column < ramTable->columnCount(); column++)
            {
                ramTable->cellWidget(row, column)->setProperty("displayIntegerBase", 16);
                ramTable->cellWidget(row, column)->setProperty("prefix", "0x");
            }
        }
    }

}
