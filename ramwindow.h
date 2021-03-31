#ifndef RAMWINDOW_H
#define RAMWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QListWidget>
#include <QTableWidget>
class ramWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ramWindow(QWidget *parent = nullptr);
    QString saveRam();
    void readRam(QString *text);

private:
    QTableWidget *table;
    QPushButton *okButton;
    QPushButton *applyButton;
    QPushButton *cancelButton;
    QPushButton *resetButton;
    int  currentRAM[16];
    int  tempRAM[16];
    void closeEvent(QCloseEvent *bar);



private slots:
    void ok();
    void apply();
    void cancel();
    void reset();
    void cellChanged(int value);


};

#endif // RAMWINDOW_H
