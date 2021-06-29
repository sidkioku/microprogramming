#ifndef RAMWINDOW_H
#define RAMWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QListWidget>
#include <QTableWidget>
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
#include <QLabel>
#include <QDebug>
#include <QAbstractItemView>
#include <QSizePolicy>
#include <QSplitter>

class ramWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ramWindow(QWidget *parent = nullptr);
    ~ramWindow();
    QString saveRam();
    void readRam(QString *text);
    int  currentRAM[2048][4];
    QTableWidget *ramTable;
    void changeValue(int row, int col, int value);
    void changeBase(bool state);


private:
    QPushButton *okButton;
    QPushButton *applyButton;
    QPushButton *cancelButton;
    QPushButton *resetButton;
    QStringList hLabels;
    void closeEvent(QCloseEvent *bar);



private slots:
    void ok();
    void apply();
    void cancel();
    void reset();
    void cellChanged(int value);


};

#endif // RAMWINDOW_H
