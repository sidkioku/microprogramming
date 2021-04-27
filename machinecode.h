#ifndef MACHINECODE_H
#define MACHINECODE_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QSpinBox>
#include <QHeaderView>

class instructionSet : public QWidget
{
    Q_OBJECT
public:
    explicit instructionSet(QWidget *parent = nullptr);
    std::vector<std::vector<QString>> currentInstructions;
    QTableWidget *instructionsTable;

private:
    QPushButton *okButton;
    QPushButton *cancelButton;
    QPushButton *addRowButton;
    QStringList hLabels;
    void closeEvent(QCloseEvent *event);
private slots:
    void addRow();
    void ok();
    void cancel();
};

#endif // MACHINECODE_H
