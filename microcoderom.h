#ifndef MICROCODEROM_H
#define MICROCODEROM_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QSpinBox>
#include <QLabel>
#include <QCloseEvent>
#include <vector>

class microcodeROM : public QWidget
{
    Q_OBJECT
public:
    explicit microcodeROM(QWidget *parent = nullptr);
    QString saveRom();
    void readRom(QString *text);
    std::vector<std::vector<int>> currentMROM;
    void changeBase(bool binary);
    bool applied;

public slots:
    void apply();



private:
    QTableWidget *table;
    QPushButton *okButton;
    QPushButton *applyButton;
    QPushButton *cancelButton;
    QPushButton *resetButton;
    QPushButton *addRowButton;
    QStringList hLabels;
    QStringList vLabelsBinary;
    QStringList vLabelsHex;
    std::vector<std::vector<int>> tempMROM;
    void closeEvent(QCloseEvent *bar);

private slots:
    void ok();
    void cancel();
    void reset();
    void addRow();
    void cellChanged(int value);

};

#endif // MICROCODEROM_H
