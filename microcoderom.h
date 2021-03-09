#ifndef MICROCODEROM_H
#define MICROCODEROM_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidgetItem>
#include <vector>

class microcodeROM : public QMainWindow
{
    Q_OBJECT
public:
    microcodeROM(QWidget *parent = nullptr);
    ~microcodeROM();

private:
    QTableWidget *table;
    QPushButton *okButton;
    QPushButton *applyButton;
    QPushButton *cancelButton;
    QPushButton *resetButton;
    QPushButton *addRowButton;
    std::vector<std::vector<int>> currTable;
    std::vector<std::vector<int>> tempTable;
    void reject();
    void closeEvent(QCloseEvent *bar);

private slots:
    void on_okButton_clicked();
    void on_applyButton_clicked();
    void on_cancelButton_clicked();
    void on_resetButton_clicked();
    void on_addRowButton_clicked();
    void cellChanged(int i);

};

#endif // MICROCODEROM_H
