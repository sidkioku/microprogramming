#ifndef MICROCODEROM_H
#define MICROCODEROM_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidgetItem>
#include <vector>

class microcodeROM : public QWidget
{
    Q_OBJECT
public:
    explicit microcodeROM(QWidget *parent = nullptr);

private:
    QTableWidget *table;
    QPushButton *okButton;
    QPushButton *applyButton;
    QPushButton *cancelButton;
    QPushButton *resetButton;
    QPushButton *addRowButton;
    std::vector<std::vector<int>> currentMROM;
    std::vector<std::vector<int>> tempMROM;
    void closeEvent(QCloseEvent *bar);

private slots:
    void ok();
    void apply();
    void cancel();
    void reset();
    void addRow();
    void cellChanged(int value);

};

#endif // MICROCODEROM_H
