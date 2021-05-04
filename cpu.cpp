#include "cpu.h"

#include <QMenu>
#include <QMenuBar>
#include "ramwindow.h"

bool secondstepread = false;
bool secondstepwrite = false;
bool writeenable[9];
bool outputenable[9];
QRandomGenerator *generator = new QRandomGenerator();


CPU::CPU(QWidget *parent) : QMainWindow(parent)
{
    this->setWindowTitle("MicroCode Simulator");
    QWidget *widget = new QWidget();
    microcode = new microcodeROM();
    ram = new ramWindow();

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    QVBoxLayout *irLayout = new QVBoxLayout();
    QVBoxLayout *pcLayout = new QVBoxLayout();
    QVBoxLayout *aLayout = new QVBoxLayout();
    QVBoxLayout *memLayout = new QVBoxLayout();
    QVBoxLayout *cpuLayout = new QVBoxLayout();


    ///RAM Button
    ramButton = new QPushButton("External RAM");
    QPalette pal = ramButton->palette();
    ramButton->setFlat(true);
    ramButton->setAutoFillBackground(true);
    ramButton->setCursor(Qt::PointingHandCursor);
    pal.setColor(QPalette::Button, QColor(Qt::gray));
    ramButton->setPalette(pal);
    QFont font = ramButton->font();
    font.setPointSize(10);
    font.setBold(true);
    font.setUnderline(true);
    ramButton->setFont(font);
    ramButton->setMinimumSize(100, 600);
    ramButton->setStyleSheet("border: 3px solid black;");

    data = new QFrame();
    data->setFrameShape(QFrame::HLine);
    data->setFrameShadow(QFrame::Plain);
    data->setLineWidth(3);

    QVBoxLayout *ramLayout = new QVBoxLayout();
    ramLayout->addSpacing(20);
    ramLayout->addWidget(ramButton);
    ramLayout->addStretch();
    ramLayout->addWidget(data);

    //    QVBoxLayout *gpio1Layout = new QVBoxLayout();
    //    QVBoxLayout *gpio2Layout = new QVBoxLayout();
    //    gpio1->setStyleSheet("background-color: gray");
    //    gpio2->setStyleSheet("background-color: gray");
    //    gpio1Layout->addStretch();
    //    gpio1Layout->addWidget(gpio1);
    //    gpio2Layout->addStretch();
    //    gpio2Layout->addWidget(gpio2);


    bus = new QFrame();
    bus->setFrameShape(QFrame::HLine);
    bus->setFrameShadow(QFrame::Plain);
    bus->setLineWidth(3);

    irLabel = new QLabel("Instruction Register");
    irLabel->setAlignment(Qt::AlignCenter);
    instructionRegOpcode = new QPushButton("0x00");
    instructionRegOpcode->setStyleSheet("border: 3px solid black; border-right: 1px solid black;");
    instructionRegOpcode->setCursor(Qt::WhatsThisCursor);
    instructionRegOpcode->setMinimumSize(25, 23);

    instructionReg1 = new QPushButton("0x00");
    instructionReg1->setStyleSheet("border: 3px solid black; border-left: 0px; border-right: 1px solid black;");
    instructionReg1->setCursor(Qt::WhatsThisCursor);
    instructionReg1->setMinimumSize(25, 23);

    instructionReg2 = new QPushButton("0x00");
    instructionReg2->setStyleSheet("border: 3px solid black; border-left: 0px; border-right: 1px solid black;");
    instructionReg2->setCursor(Qt::WhatsThisCursor);
    instructionReg2->setMinimumSize(25, 23);
    instructionReg3 = new QPushButton("0x00");
    instructionReg3->setStyleSheet("border: 3px solid black; border-left: 0px");
    instructionReg3->setCursor(Qt::WhatsThisCursor);
    instructionReg3->setMinimumSize(25, 23);

    QHBoxLayout *instruction = new QHBoxLayout();
    instruction->addWidget(instructionRegOpcode);
    instruction->addWidget(instructionReg1);
    instruction->addWidget(instructionReg2);
    instruction->addWidget(instructionReg3);
    instruction->setSpacing(0);
    playIcon = new QIcon(":/playpause/icons/playIcon");
    pauseIcon = new QIcon(":/playpause/icons/pauseIcon");
    nextInstructionButton = new QPushButton(*playIcon, "Play", this);
    nextInstructionButton->setCheckable(true);
    nextInstructionButton->setChecked(false);
    nextInstructionButton->setEnabled(false);
    nextInstructionButton->setMinimumSize(100, 23);
    nextStepButton = new QPushButton("Next Step");
    nextStepButton->setDisabled(true);
    nextStepButton->setMinimumSize(100, 23);
    resetButton = new QPushButton("Reset");
    resetButton->setMinimumSize(100, 23);
    irLayout->addSpacing(20);
    irLayout->addWidget(nextInstructionButton);
    irLayout->addWidget(nextStepButton);
    irLayout->addWidget(resetButton);
    irLayout->addStretch();
    irLayout->addWidget(irLabel);
    irLayout->addLayout(instruction);


    readMicrocode = new QPushButton("Read Microcode ROM...");
    readMicrocode->setMinimumSize(100, 23);
    readMicrocode->setCursor(Qt::PointingHandCursor);
    writeMicrocode = new QPushButton("Write Microcode ROM...");
    writeMicrocode->setMinimumSize(100, 23);
    writeMicrocode->setCursor(Qt::PointingHandCursor);
    pcLabel = new QLabel("Program Counter");
    pcLabel->setAlignment(Qt::AlignCenter);
    progCounter = new QPushButton("0x00000000");
    progCounter->setStyleSheet("border: 3px solid black;");
    progCounter->setCursor(Qt::WhatsThisCursor);
    progCounter->setMinimumSize(100, 23);


    pcLayout->addSpacing(20);
    pcLayout->addWidget(readMicrocode);
    pcLayout->addWidget(writeMicrocode);
    pcLayout->addStretch();
    pcLayout->addWidget(pcLabel);
    pcLayout->addWidget(progCounter);

    readRAM = new QPushButton("Read RAM...");
    readRAM->setMinimumSize(100, 23);
    readRAM->setCursor(Qt::PointingHandCursor);
    writeRAM = new QPushButton("Write RAM...");
    writeRAM->setMinimumSize(100, 23);
    writeRAM->setCursor(Qt::PointingHandCursor);
    aLabel = new QLabel("A Register");
    aLabel->setAlignment(Qt::AlignCenter);
    aReg = new QPushButton("0x00000000");
    aReg->setStyleSheet("border: 3px solid black;");
    aReg->setCursor(Qt::WhatsThisCursor);
    aReg->setMinimumSize(100, 23);
    aLayout->addSpacing(20);
    aLayout->addWidget(readRAM);
    aLayout->addWidget(writeRAM);
    aLayout->addStretch();
    aLayout->addWidget(aLabel);
    aLayout->addWidget(aReg);

    ///Arithmetic Logic Unit
    QBoxLayout *aluLayout = drawAlu();
    QVBoxLayout *alu = new QVBoxLayout();
    alu->addStretch();
    alu->addLayout(aluLayout);
    alu->addStretch();

    ///Memory Access Registers
    marLabel = new QLabel("Memory Address Register");
    marLabel->setAlignment(Qt::AlignCenter);
    marReg = new QPushButton("0x00000000");
    marReg->setCursor(Qt::WhatsThisCursor);
    marReg->setStyleSheet("border: 3px solid black;");
    marReg->setMinimumSize(100, 23);

    mdrInLabel = new QLabel("Memory Data Register In");
    mdrInLabel->setAlignment(Qt::AlignCenter);
    mdrInReg = new QPushButton("0x00000000");
    mdrInReg->setCursor(Qt::WhatsThisCursor);
    mdrInReg->setStyleSheet("border: 3px solid black;");
    mdrInReg->setMinimumSize(100, 23);

    mdrOutLabel = new QLabel("Memory Data Register Out");
    mdrOutLabel->setAlignment(Qt::AlignCenter);
    mdrOutReg = new QPushButton("0x00000000");
    mdrOutReg->setCursor(Qt::WhatsThisCursor);
    mdrOutReg->setStyleSheet("border: 3px solid black;");
    mdrOutReg->setMinimumSize(100, 23);

    memLayout->addSpacing(20);
    memLayout->addWidget(marLabel);
    memLayout->addWidget(marReg);
    memLayout->addStretch();
    memLayout->addWidget(mdrInLabel);
    memLayout->addWidget(mdrInReg);
    memLayout->addStretch();
    memLayout->addWidget(mdrOutLabel);
    memLayout->addWidget(mdrOutReg);

    ///Microcode ROM
    romButton = new QPushButton("Microcode ROM");
    romButton->setFlat(true);
    romButton->setFont(font);
    romButton->setCursor(Qt::PointingHandCursor);
    romButton->setMinimumSize(100, 600);
    romButton->setStyleSheet("border: 3px solid black;");

    QVBoxLayout *romLayout = new QVBoxLayout();
    romLayout->addSpacing(20);
    romLayout->addWidget(romButton);
    romLayout->addStretch();
    QLabel *busLabel = new QLabel("Data Bus");
    busLabel->setAlignment(Qt::AlignCenter);
    busButton = new QPushButton("0x00000000");
    busButton->setStyleSheet("border: 3px solid black;");
    busButton->setMinimumSize(100, 23);
    busButton->setCursor(Qt::WhatsThisCursor);

    QHBoxLayout *busLayout = new QHBoxLayout();

    busLayout->addStretch();
    busLayout->addWidget(busButton);
    busLayout->addStretch();

    ///Put everything together
    //    buttonsLayout->addWidget(rombutton);
    buttonsLayout->addLayout(romLayout);
    buttonsLayout->addLayout(irLayout);
    buttonsLayout->addLayout(pcLayout);
    buttonsLayout->addLayout(aLayout);
    buttonsLayout->addLayout(alu);
    buttonsLayout->addLayout(memLayout);
    //    buttonsLayout->addWidget(ramButton);



    cpuLayout->addSpacing(5);
    cpuLayout->addLayout(buttonsLayout);
    cpuLayout->addSpacing(30);
    cpuLayout->addWidget(busLabel);
    cpuLayout->addLayout(busLayout);
    cpuLayout->addWidget(bus);

    QHBoxLayout *completeLayout = new QHBoxLayout();
    completeLayout->addLayout(cpuLayout);
    completeLayout->addSpacing(50);
    completeLayout->addLayout(ramLayout);

    widget->setLayout(completeLayout);
    this->setCentralWidget(widget);
    this->resize(1700, 1000);

    ///Connections
    connect(romButton, SIGNAL(clicked()), this, SLOT(microcodeOpen()));
    connect(ramButton, SIGNAL(clicked()), this, SLOT(ramOpen()));

    connect(nextInstructionButton, SIGNAL(clicked()), this, SLOT(nextInstruction()));
    connect(nextStepButton, SIGNAL(clicked()), this, SLOT(nextStep()));
    connect(resetButton, SIGNAL(clicked()), this, SLOT(reset()));

    connect(readMicrocode, SIGNAL(clicked()), this, SLOT(microcodeFile()));
    connect(writeMicrocode, SIGNAL(clicked()), this, SLOT(saveRom()));
    connect(readRAM, SIGNAL(clicked()), this, SLOT(ramFile()));
    connect(writeRAM, SIGNAL(clicked()), this, SLOT(saveRam()));
    connect(progCounter, SIGNAL(clicked()), this, SLOT(pcExp()));
    connect(instructionRegOpcode, SIGNAL(clicked()), this, SLOT(irExp()));
    connect(aReg, SIGNAL(clicked()), this, SLOT(aRegExp()));
    connect(marReg, SIGNAL(clicked()), this, SLOT(marExp()));
    connect(mdrInReg, SIGNAL(clicked()), this, SLOT(mdrInExp()));
    connect(mdrOutReg, SIGNAL(clicked()), this, SLOT(mdrOutExp()));
}

CPU::~CPU(){}

void CPU::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    QPen blackPen(Qt::black);
    QPen dashedPen(Qt::black);
    QPen dashedRedPen(Qt::red);
    QPen grayPen(Qt::gray);
    QPen redPen(Qt::red);
    dashedPen.setStyle(Qt::DashLine);
    dashedRedPen.setStyle(Qt::DashLine);
    QIcon *arrowDownRed = new QIcon(":/icons/arrowDownRed.png");
    QIcon *arrowLeftRed = new QIcon(":/icons/arrowLeftRed.png");
    QIcon *arrowUpRed = new QIcon(":/icons/arrowUpRed.png");
    QIcon *arrowRightRed = new QIcon(":/icons/arrowRightRed.png");
    QIcon *arrowDown = new QIcon(":/icons/arrowDown.png");
    QIcon *arrowLeft = new QIcon(":/icons/arrowLeft.png");
    QIcon *arrowUp = new QIcon(":/icons/arrowUp.png");
    QIcon *arrowRight = new QIcon(":/icons/arrowRight.png");
    redPen.setWidth(4);
    blackPen.setWidth(4);
    dashedPen.setWidth(2);
    dashedRedPen.setWidth(2);
    grayPen.setWidth(2);
    painter.setPen(blackPen);

    ///Arrows to the BUS

    //Arrow Instruction Register -> BUS
    if(outputenable[0])
    {
        painter.setPen(redPen);
        painter.drawPixmap(instructionRegOpcode->x() + instructionRegOpcode->width() - 9, bus->y() - 15, arrowDownRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawPixmap(instructionRegOpcode->x() + instructionRegOpcode->width() - 9, bus->y() - 15, arrowDown->pixmap(18, 18));
    }
    painter.drawLine(instructionRegOpcode->x() + instructionRegOpcode->width(), instructionRegOpcode->y() + instructionRegOpcode->height(), instructionRegOpcode->x() + instructionRegOpcode->width(), bus->y());

    //Arrow BUS -> Instruction Register
    if (writeenable[0])
    {
        painter.setPen(redPen);
        painter.drawPixmap(instructionReg2->x() + instructionReg2->width() - 9, instructionReg2->y() + instructionReg2->height() - 3, arrowUpRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawPixmap(instructionReg2->x() + instructionReg2->width() - 9, instructionReg2->y() + instructionReg2->height() - 3, arrowUp->pixmap(18, 18));
    }
    painter.drawLine(instructionReg2->x() + instructionReg2->width(), instructionReg2->y() + instructionReg2->height(), instructionReg2->x() + instructionReg2->width(), bus->y());



    //Arrow Program Counter <-> BUS
    if (writeenable[1])
    {
        painter.setPen(redPen);
        painter.drawLine(progCounter->x() + progCounter->width()/2,       progCounter->y() + progCounter->height(),       progCounter->x() + progCounter->width()/2,       bus->y());
        painter.drawPixmap(progCounter->x() + progCounter->width()/2 - 9,       progCounter->y() + progCounter->height() - 3, arrowUpRed->pixmap(18, 18));
        painter.setPen(blackPen);
        painter.drawPixmap(progCounter->x() + progCounter->width()/2 - 9,       bus->y() - 15, arrowDown->pixmap(18, 18));
    }
    else if (outputenable[1])
    {
        painter.setPen(redPen);
        painter.drawLine(progCounter->x() + progCounter->width()/2,       progCounter->y() + progCounter->height(),       progCounter->x() + progCounter->width()/2,       bus->y());
        painter.drawPixmap(progCounter->x() + progCounter->width()/2 - 9,       bus->y() - 15, arrowDownRed->pixmap(18, 18));
        painter.setPen(blackPen);
        painter.drawPixmap(progCounter->x() + progCounter->width()/2 - 9,       progCounter->y() + progCounter->height() - 3, arrowUp->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawLine(progCounter->x() + progCounter->width()/2,       progCounter->y() + progCounter->height(),       progCounter->x() + progCounter->width()/2,       bus->y());
        painter.drawPixmap(progCounter->x() + progCounter->width()/2 - 9,       bus->y() - 15, arrowDown->pixmap(18, 18));
        painter.drawPixmap(progCounter->x() + progCounter->width()/2 - 9,       progCounter->y() + progCounter->height() - 3, arrowUp->pixmap(18, 18));
    }

    //Arrow A Register <-> BUS
    if (writeenable[2])
    {
        painter.setPen(redPen);
        painter.drawLine(aReg->x() + aReg->width()/2,                     aReg->y() + aReg->height(),                     aReg->x() + aReg->width()/2,                     bus->y());
        painter.drawPixmap(aReg->x() + aReg->width()/2 - 9,               aReg->y() + aReg->height() - 3, arrowUpRed->pixmap(18, 18));
        painter.setPen(blackPen);
        painter.drawPixmap(aReg->x() + aReg->width()/2 - 9,                     bus->y() - 15, arrowDown->pixmap(18, 18));
    }
    else if (outputenable[2])
    {
        painter.setPen(redPen);
        painter.drawLine(aReg->x() + aReg->width()/2,                     aReg->y() + aReg->height(),                     aReg->x() + aReg->width()/2,                     bus->y());
        painter.drawPixmap(aReg->x() + aReg->width()/2 - 9,                     bus->y() - 15, arrowDownRed->pixmap(18, 18));
        painter.setPen(blackPen);
        painter.drawPixmap(aReg->x() + aReg->width()/2 - 9,               aReg->y() + aReg->height() - 3, arrowUp->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawLine(aReg->x() + aReg->width()/2,                     aReg->y() + aReg->height(),                     aReg->x() + aReg->width()/2,                     bus->y());
        painter.drawPixmap(aReg->x() + aReg->width()/2 - 9,                     bus->y() - 15, arrowDown->pixmap(18, 18));
        painter.drawPixmap(aReg->x() + aReg->width()/2 - 9,               aReg->y() + aReg->height() - 3, arrowUp->pixmap(18, 18));
    }

    //Arrow BUS -> X Register
    if (writeenable[3]){
        painter.setPen(redPen);
        painter.drawLine(xReg->x() + xReg->width()/2,                     xReg->y() + xReg->height(),                     xReg->x() + xReg->width()/2,                     bus->y());
        painter.drawPixmap(xReg->x() + xReg->width()/2 - 9,                     xReg->y() + xReg->height() - 1, arrowUpRed->pixmap(18, 18));
    } else
    {
        painter.setPen(blackPen);
        painter.drawLine(xReg->x() + xReg->width()/2,                     xReg->y() + xReg->height(),                     xReg->x() + xReg->width()/2,                     bus->y());
        painter.drawPixmap(xReg->x() + xReg->width()/2 - 9,                     xReg->y() + xReg->height() - 1, arrowUp->pixmap(18, 18));
    }

    //Arrow BUS -> Y Register
    if (writeenable[4])
    {
        painter.setPen(redPen);
        painter.drawLine(yReg->x() + yReg->width()/2,                     yReg->y() + yReg->height(),                     yReg->x() + yReg->width()/2,                     bus->y());
        painter.drawPixmap(yReg->x() + yReg->width()/2 - 9,                     yReg->y() + yReg->height() - 1, arrowUpRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawLine(yReg->x() + yReg->width()/2,                     yReg->y() + yReg->height(),                     yReg->x() + yReg->width()/2,                     bus->y());
        painter.drawPixmap(yReg->x() + yReg->width()/2 - 9,                     yReg->y() + yReg->height() - 1, arrowUp->pixmap(18, 18));
    }
    ///ALU Lines

    if (outputenable[5] & writeenable[5])
    {
        painter.setPen(redPen);
        // Arrow X -> ALU Button Red
        painter.drawLine(xLabel->x() + xLabel->width()/2,                     xLabel->y(),                                      xLabel->x() + xLabel->width()/2,                     aluButton->y() + aluButton->height());
        painter.drawPixmap(xLabel->x() + xLabel->width()/2 - 9,               aluButton->y() + aluButton->height() - 1,           arrowUpRed->pixmap(18, 18));

        //Arrow Y -> ALU Button Red
        painter.drawLine(yLabel->x() + yLabel->width()/2,                     yLabel->y(),                                      yLabel->x() + yLabel->width()/2,                     aluButton->y() + aluButton->height());
        painter.drawPixmap( yLabel->x() + yLabel->width()/2 - 9,              aluButton->y() + aluButton->height() - 1,           arrowUpRed->pixmap(18, 18));

        //Arrow ALU Button -> Z Red
        painter.drawLine(aluButton->x() + aluButton->width()/2,           aluButton->y(),                                 zReg->x() + zReg->width()/2,                     zReg->y() + zReg->height());
        painter.drawPixmap(zReg->x() + zReg->width()/2 - 9,                     zReg->y() + zReg->height() - 1, arrowUpRed->pixmap(18, 18));

    } else
    {
        painter.setPen(blackPen);
        // Arrow X -> ALU Button Black
        painter.drawLine(xLabel->x() + xLabel->width()/2,                     xLabel->y(),                                      xLabel->x() + xLabel->width()/2,                     aluButton->y() + aluButton->height());
        painter.drawPixmap(xLabel->x() + xLabel->width()/2 - 9,               aluButton->y() + aluButton->height() - 1,           arrowUp->pixmap(18, 18));

        //Arrow Y -> ALU Button Black
        painter.drawLine(yLabel->x() + yLabel->width()/2,                     yLabel->y(),                                      yLabel->x() + yLabel->width()/2,                     aluButton->y() + aluButton->height());
        painter.drawPixmap( yLabel->x() + yLabel->width()/2 - 9,              aluButton->y() + aluButton->height() - 1,           arrowUp->pixmap(18, 18));

        //Arrow ALU Button -> Z Black
        painter.drawLine(aluButton->x() + aluButton->width()/2,           aluButton->y(),                                 zReg->x() + zReg->width()/2,                     zReg->y() + zReg->height());
        painter.drawPixmap(zReg->x() + zReg->width()/2 - 9,                     zReg->y() + zReg->height() - 1, arrowUp->pixmap(18, 18));

    }

    //Arrow Z -> BUS
    if (outputenable[3])
    {
        painter.setPen(redPen);
        painter.drawLine(zReg->x() + zReg->width(),                       zReg->y() + zReg->height()/2,                   zReg->x() + zReg->width() + 100,    zReg->y() + zReg->height()/2);
        painter.drawLine(zReg->x() + zReg->width() + 100,    zReg->y() + zReg->height()/2,                   zReg->x() + zReg->width() + 100, bus->y());
        painter.drawPixmap(zReg->x() + zReg->width() + 100 - 9, bus->y() - 18, arrowDownRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawLine(zReg->x() + zReg->width(),                       zReg->y() + zReg->height()/2,                   zReg->x() + zReg->width() + 100,    zReg->y() + zReg->height()/2);
        painter.drawLine(zReg->x() + zReg->width() + 100,    zReg->y() + zReg->height()/2,                   zReg->x() + zReg->width() + 100, bus->y());
        painter.drawPixmap(zReg->x() + zReg->width() + 100 - 9, bus->y() - 18, arrowDown->pixmap(18, 18));
    }

    //TODO: Comparisons Arrows Change Colors?
    //Arrow Comparisons -> Microcode ROM
    painter.setPen(blackPen);
    painter.drawLine(comparisons->x(), comparisons->y() + comparisons->height()/2, romButton->x() + romButton->width(), comparisons->y() + comparisons->height()/2);
    painter.drawPixmap(romButton->x() + romButton->width(), comparisons->y() + comparisons->height()/2 - 9, arrowLeft->pixmap(18, 18));

    //Arrow Z -> Comparisons
    painter.drawLine(comparisons->x() + comparisons->width()/2, zReg->y() + zReg->height()/2, comparisons->x() + comparisons->width()/2, comparisons->y() + comparisons->height());
    painter.drawLine(comparisons->x() + comparisons->width()/2, zReg->y() + zReg->height()/2, zReg->x(), zReg->y() + zReg->height()/2);
    painter.drawPixmap(comparisons->x() + comparisons->width()/2 - 9, comparisons->y() + comparisons->height() - 1, arrowUp->pixmap(18, 18));

    //Arrow IR -> Microcode ROM
    painter.drawLine(instructionRegOpcode->x(), instructionRegOpcode->y() + instructionRegOpcode->height()/2, romButton->x() + 3 *romButton->width()/4,  instructionRegOpcode->y() + instructionRegOpcode->height()/2);
    painter.drawLine(romButton->x() + 3 *romButton->width()/4, instructionRegOpcode->y() + instructionRegOpcode->height()/2, romButton->x() + 3 *romButton->width()/4, romButton->y() + romButton->height());
    painter.drawPixmap(romButton->x() + 3 *romButton->width()/4 - 9, romButton->y() + romButton->height(), arrowUp->pixmap(18, 18));

    ///Memory Lines
    //Arrow MDR IN -> BUS
    if (outputenable[4])
    {
        painter.setPen(redPen);
        painter.drawLine(mdrInReg->x(),                                   mdrInReg->y() + mdrInReg->height()/2,           mdrInReg->x() - 60,             mdrInReg->y() + mdrInReg->height()/2);
        painter.drawLine(mdrInReg->x() - 60,            mdrInReg->y() + mdrInReg->height()/2,           mdrInReg->x() - 60,             bus->y());
        painter.drawPixmap( mdrInReg->x() - 60 - 9,             bus->y() - 18, arrowDownRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawLine(mdrInReg->x(),                                   mdrInReg->y() + mdrInReg->height()/2,           mdrInReg->x() - 60,             mdrInReg->y() + mdrInReg->height()/2);
        painter.drawLine(mdrInReg->x() - 60,            mdrInReg->y() + mdrInReg->height()/2,           mdrInReg->x() - 60,             bus->y());
        painter.drawPixmap( mdrInReg->x() - 60 - 9,             bus->y() - 18, arrowDown->pixmap(18, 18));
    }

    //Arrow BUS -> MDR OUT
    if (writeenable[8])
    {
        painter.setPen(redPen);
        painter.drawLine(mdrOutReg->x(),                                  mdrOutReg->y() + mdrOutReg->height()/2,         mdrOutReg->x() - 20,          mdrOutReg->y() + mdrOutReg->height()/2);
        painter.drawLine(mdrOutReg->x() - 20,          mdrOutReg->y() + mdrOutReg->height()/2,         mdrOutReg->x() - 20,          bus->y());
        painter.drawPixmap(mdrOutReg->x() - 18,                          mdrOutReg->y() + mdrOutReg->height()/2 - 9, arrowRightRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawLine(mdrOutReg->x(),                                  mdrOutReg->y() + mdrOutReg->height()/2,         mdrOutReg->x() - 20,          mdrOutReg->y() + mdrOutReg->height()/2);
        painter.drawLine(mdrOutReg->x() - 20,          mdrOutReg->y() + mdrOutReg->height()/2,         mdrOutReg->x() - 20,          bus->y());
        painter.drawPixmap(mdrOutReg->x() - 18,                          mdrOutReg->y() + mdrOutReg->height()/2 - 9, arrowRight->pixmap(18, 18));
    }

    //Arrow BUS -> MAR
    if (writeenable[6])
    {
        painter.setPen(redPen);
        painter.drawLine(marReg->x(),                                     marReg->y() + marReg->height()/2,               marReg->x() - 100,                 marReg->y() + marReg->height()/2);
        painter.drawLine(marReg->x() - 100,                 marReg->y() + marReg->height()/2,               marReg->x() - 100,                 bus->y());
        painter.drawPixmap(marReg->x() - 18,                                     marReg->y() + marReg->height()/2 - 9, arrowRightRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawLine(marReg->x(),                                     marReg->y() + marReg->height()/2,               marReg->x() - 100,                 marReg->y() + marReg->height()/2);
        painter.drawLine(marReg->x() - 100,                 marReg->y() + marReg->height()/2,               marReg->x() - 100,                 bus->y());
        painter.drawPixmap(marReg->x() - 18,                                     marReg->y() + marReg->height()/2 - 9, arrowRight->pixmap(18, 18));
    }


    //Gray background for outer part
    painter.setPen(grayPen);
    QPointF topLeft(ramButton->x() - 10, 0);
    QPointF bottomRight(this->width(), this->height());
    QRectF rectangle(topLeft, bottomRight);
    painter.fillRect(rectangle, Qt::gray);


    painter.setPen(dashedPen);

    //Arrow MAR -> RAM
    painter.drawLine(marReg->x() + marReg->width(), marReg->y() + marReg->height()/2, ramButton->x(), marReg->y() + marReg->height()/2);
    painter.drawPixmap(ramButton->x() - 17, marReg->y() + marReg->height()/2 - 9, arrowRight->pixmap(18, 18));

    //Arrow Outer Data Bus -> MDR In
    if (writeenable[7])
    {
        painter.setPen(dashedRedPen);
        painter.drawLine(data->x() + 20, data->y(), data->x() + 20, ramButton->y() + ramButton->height() + 10);
        painter.drawLine( data->x() + 20, ramButton->y() + ramButton->height() + 10, mdrInReg->x() + mdrInReg->width() + 40,  ramButton->y() + ramButton->height() + 10);
        painter.drawLine(mdrInReg->x() + mdrInReg->width() + 40,  ramButton->y() + ramButton->height() + 10, mdrInReg->x() + mdrInReg->width() + 40,  mdrInReg->y() + mdrInReg->height()/2);
        painter.drawLine(mdrInReg->x() + mdrInReg->width() + 40,  mdrInReg->y() + mdrInReg->height()/2, mdrInReg->x() + mdrInReg->width(), mdrInReg->y() + mdrInReg->height()/2);
        painter.drawPixmap(mdrInReg->x() + mdrInReg->width(), mdrInReg->y() + mdrInReg->height()/2 - 9, arrowLeftRed->pixmap(18, 18));
    }
    else {
        painter.setPen(dashedPen);
        painter.drawLine(data->x() + 20, data->y(), data->x() + 20, ramButton->y() + ramButton->height() + 10);
        painter.drawLine( data->x() + 20, ramButton->y() + ramButton->height() + 10, mdrInReg->x() + mdrInReg->width() + 40,  ramButton->y() + ramButton->height() + 10);
        painter.drawLine(mdrInReg->x() + mdrInReg->width() + 40,  ramButton->y() + ramButton->height() + 10, mdrInReg->x() + mdrInReg->width() + 40,  mdrInReg->y() + mdrInReg->height()/2);
        painter.drawLine(mdrInReg->x() + mdrInReg->width() + 40,  mdrInReg->y() + mdrInReg->height()/2, mdrInReg->x() + mdrInReg->width(), mdrInReg->y() + mdrInReg->height()/2);
        painter.drawPixmap(mdrInReg->x() + mdrInReg->width(), mdrInReg->y() + mdrInReg->height()/2 - 9, arrowLeft->pixmap(18, 18));
    }

    //Arrow MDR Out -> Outer Data Bus
    if (outputenable[6])
    {
        painter.setPen(dashedRedPen);
        painter.drawLine(mdrOutReg->x() + mdrOutReg->width(), mdrOutReg->y() + mdrOutReg->height()/2, data->x() + 10, mdrOutReg->y() + mdrOutReg->height()/2);
        painter.drawLine( data->x() + 10, mdrOutReg->y() + mdrOutReg->height()/2,  data->x() + 10, data->y());
        painter.drawPixmap( data->x() + 10 - 9, data->y() - 18, arrowDownRed->pixmap(18, 18));
    } else
    {
        painter.setPen(dashedPen);
        painter.drawLine(mdrOutReg->x() + mdrOutReg->width(), mdrOutReg->y() + mdrOutReg->height()/2, data->x() + 10, mdrOutReg->y() + mdrOutReg->height()/2);
        painter.drawLine( data->x() + 10, mdrOutReg->y() + mdrOutReg->height()/2,  data->x() + 10, data->y());
        painter.drawPixmap( data->x() + 10 - 9, data->y() - 18, arrowDown->pixmap(18, 18));
    }

    //Arrow RAM <-> Outer Data Bus
    painter.drawLine(ramButton->x() + ramButton->width()/2, ramButton->y() + ramButton->height(), ramButton->x() + ramButton->width()/2, data->y());
    painter.drawPixmap(ramButton->x() + ramButton->width()/2 - 9, ramButton->y() + ramButton->height(), arrowUp->pixmap(18, 18));
    painter.drawPixmap(ramButton->x() + ramButton->width()/2 - 9, data->y() - 18, arrowDown->pixmap(18, 18));

    //    //Arrow GPIO 1 <-> Outer Data Bus
    //    painter.drawLine(gpio1->x() + gpio1->width()/2, gpio1->y() + gpio1->height(), gpio1->x() + gpio1->width()/2, data->y());
    //    painter.drawPixmap(gpio1->x() + gpio1->width()/2 - 6, gpio1->y() + gpio1->height(), arrowUp->pixmap(18, 18));
    //    painter.drawPixmap(gpio1->x() + gpio1->width()/2 - 6, data->y() - 12, arrowDown->pixmap(18, 18));

    //    //Arrow GPIO 2 <-> Outer Data Bus
    //    painter.drawLine(gpio2->x() + gpio2->width()/2, gpio2->y() + gpio2->height(), gpio2->x() + gpio2->width()/2, data->y());
    //    painter.drawPixmap(gpio2->x() + gpio2->width()/2 - 6, gpio2->y() + gpio2->height(), arrowUp->pixmap(18, 18));
    //    painter.drawPixmap(gpio2->x() + gpio2->width()/2 - 6, data->y() - 12, arrowDown->pixmap(18, 18));


}

void CPU::microcodeFile()
{
    QString readMicrocode = QFileDialog::getOpenFileName(this, "Open Microcode ROM...", QDir::homePath(), "ROM Files (*.rom)");
    QFile file(readMicrocode);
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    QString text = in.readAll();
    file.close();
    microcode->readRom(&text);
    nextStepButton->setDisabled(true);
    nextInstructionButton->setDisabled(true);
}

void CPU::saveRom()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save Microcode ROM...", QDir::homePath(), "ROM Files (*.rom)");
    QFile f(filename);
    f.open(QIODevice::WriteOnly);
    QTextStream out(&f);
    QString text = microcode->saveRom();
    out << text;
    f.flush();
    f.close();
}

void CPU::ramFile() {
    QString readRAM = QFileDialog::getOpenFileName(this, "Open RAM File...", QDir::homePath(), "RAM Files (*.ram)");
    QFile file(readRAM);
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    QString text = in.readAll();
    file.close();
    ram->readRam(&text);
    nextStepButton->setDisabled(true);
    nextInstructionButton->setDisabled(true);
}

void CPU::saveRam()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save RAM File...", QDir::homePath(), "RAM File (*.ram)");
    QFile f(filename);
    f.open(QIODevice::WriteOnly);
    QTextStream out(&f);
    QString text = ram->saveRam();
    out << text;
    f.flush();
    f.close();
}

void CPU::microcodeOpen()
{
    microcode->show();
}

void CPU::ramOpen()
{
    ram->show();
}

void CPU::nextInstruction()
{
    QString pc = progCounter->text();
    while (pc == progCounter->text())
    {
        nextStep();
    }
}

void CPU::nextStep()
{
    bool converted = true;
    qDebug() << currentRow;
    //    int opcode =  ram->currentRAM[progCounter->text().toInt(&converted, 2)][0];
    //    int microcodeRow = ram->getMicrocodeRow(opcode);
    int nextRow = microcode->currentMROM[currentRow][0];
    //    if (currentRow != microcodeRow) currentRow = microcodeRow;
    //    else
    //    {
    //        currentRow = nextRow;
    //    }
    ///Next Row Condition
    bool values[microcode->currentMROM[currentRow].size() - 2];
    for (int column = 0; column < (int)microcode->currentMROM[currentRow].size() - 2; column++)
    {
        values[column] = microcode->currentMROM[currentRow][column];
    }
    bool lsb = nextRow & 1;
    int condition = microcode->currentMROM[currentRow][1];
    bool cond = 0;
    switch (condition)
    {
    case 1: // == 0 ?
        cond = zReg->text().toInt(&converted, 2) == 0;
        break;
    case 2: // > 0 ?
        cond = zReg->text().toInt(&converted, 2) > 0;
        break;
    case 3:// < 0 ?
        cond = zReg->text().toInt(&converted, 2) < 0;
        break;
    }
    if (!lsb && cond) nextRow++;

    ///Operation Code
    if (microcode->currentMROM[currentRow][11])
    {
        int opcode = microcode->currentMROM[currentRow][2];
        switch (opcode)
        {
        case 1: //ADD
            zReg->setText(QString("%1").arg(xReg->text().toInt(&converted, 2) + yReg->text().toInt(&converted, 2), 8, 2, QChar('0')));
            break;
        case 2: //SUB
            zReg->setText(QString("%1").arg(xReg->text().toInt(&converted, 2) - yReg->text().toInt(&converted, 2), 8, 2, QChar('0')));
            break;
        case 3: //BITSHIFT X LEFT
            zReg->setText(QString("%1").arg(xReg->text().toInt(&converted, 2) << 1, 8, 2, QChar('0')));
            break;
        case 4: //BITSHIFT X RIGHT
            zReg->setText(QString("%1").arg(xReg->text().toInt(&converted, 2) >> 1, 8, 2, QChar('0')));
            break;
        case 5: //PASS X
            zReg->setText(xReg->text());
            break;
        case 6: //INCREMENT X
            zReg->setText(QString("%1").arg(xReg->text().toInt(&converted, 2) + 1, 8, 2, QChar('0')));
            break;
        case 7: //DECREMENT X
            zReg->setText(QString("%1").arg(xReg->text().toInt(&converted, 2) - 1, 8, 2, QChar('0')));
            break;
        }
    }
    enum reg
    {
        none, ir, pc, a, b, x, y, z, mar, mdrin, mdrout
    };

    ///Output Enable
    int outputcounter = 0;
    outputenable[0] = microcode->currentMROM[currentRow][4]; //instruction register
    outputenable[1] = microcode->currentMROM[currentRow][6]; //program counter
    outputenable[2] = microcode->currentMROM[currentRow][8]; //a register
    outputenable[3] = microcode->currentMROM[currentRow][12]; //z register
    outputenable[4] = microcode->currentMROM[currentRow][15]; //mdr in register
    outputenable[5] = microcode->currentMROM[currentRow][2]; //opcode
    outputenable[6] = microcode->currentMROM[currentRow][17]; //mdr out register
    QString outputtext = busButton->text();
    if (microcode->currentMROM[currentRow][4])
    {
        outputtext = instructionRegOpcode->text();
        outputcounter++;
    }
    if (microcode->currentMROM[currentRow][6])
    {
        outputtext = progCounter->text();
        outputcounter++;
    }
    if (microcode->currentMROM[currentRow][8])
    {
        outputtext = aReg->text();
        outputcounter++;
    }
    if (microcode->currentMROM[currentRow][12]){
        outputtext = zReg->text();
        outputcounter++;
    }
    if (microcode->currentMROM[currentRow][15])
    {
        outputtext = mdrInReg->text();
        outputcounter++;
    }
    if (outputcounter > 1)
    {
        QString row = QString::number(currentRow);
        QString output = "Multiple registers putting data out on data bus.\nReset initiated. Error in Microcode row: ";
        output.append(row);
        QMessageBox::critical(this, "Conflict on the data bus", output);
        busButton->setText("??????????");
        reset();
        return;
    }
    else busButton->setText(outputtext);

    ///Write Enable

    writeenable[0] = microcode->currentMROM[currentRow][3]; //ir
    writeenable[1] = microcode->currentMROM[currentRow][5]; //pc
    writeenable[2] = microcode->currentMROM[currentRow][7]; //a
    writeenable[3] = microcode->currentMROM[currentRow][9]; //x
    writeenable[4] = microcode->currentMROM[currentRow][10]; //y
    writeenable[5] = microcode->currentMROM[currentRow][11]; //z
    writeenable[6] = microcode->currentMROM[currentRow][13]; //mar
    writeenable[7] = microcode->currentMROM[currentRow][14]; //mdrin
    writeenable[8] = microcode->currentMROM[currentRow][16]; //mdrout

    if (microcode->currentMROM[currentRow][3]) instructionRegOpcode->setText(outputtext); //instruction register
    if (microcode->currentMROM[currentRow][5]) progCounter->setText(outputtext); //program counter
    if (microcode->currentMROM[currentRow][7]) aReg->setText(outputtext); // a register
    if (microcode->currentMROM[currentRow][9]) xReg->setText(outputtext); // x register
    if (microcode->currentMROM[currentRow][10]) yReg->setText(outputtext); // y register
    if (microcode->currentMROM[currentRow][13]) marReg->setText(outputtext); // mar register
    if (microcode->currentMROM[currentRow][16]) mdrOutReg->setText(outputtext); // mdr out register



    ///Communication with external RAM
    if (microcode->currentMROM[currentRow][19]) //mem.en
    {
        if (microcode->currentMROM[currentRow][18]) //mem.r
        {
            if (secondstepread)
            {
                if (microcode->currentMROM[currentRow][14]) //mdrin.we
                {
                    int address = marReg->text().toInt(nullptr, 2);
                    int row = address / 4;
                    int col = address % 4;
                    mdrInReg->setText(QString("%1").arg(ram->currentRAM[row][col], 8, 2, QChar('0'))); // mdr in register
                }
            }
            else secondstepread = true;
        }
        else {
            if (secondstepwrite)
            {
                if (microcode->currentMROM[currentRow][17])
                {
                    int address = marReg->text().toInt(nullptr, 2);
                    int row = address / 4;
                    int col = address % 4;
                    ram->currentRAM[row][col] = mdrOutReg->text().toInt(nullptr, 2);
                }
            }
            else secondstepwrite = true;
        }
    }
    currentRow = nextRow;
    this->update(); //update colors for writeEnable/outputEnable
}

void CPU::reset()
{
    currentInstruction = 0;
    bool vFound = false;
    qDebug() << ram->currentInstructions.size();
    qDebug() << ram->currentInstructions.at(0).at(0).toInt();
    uint i = 0;
    while (i < ram->currentInstructions.size() && !vFound)
    {
        if (ram->currentRAM[0][0] == ram->currentInstructions.at(i).at(0).toInt())
        {
            currentRow = ram->currentInstructions.at(i).at(2).toInt();
            vFound = true;
        }
        i++;
    }

    if (vFound == false)
    {
        QMessageBox::warning(this, "RAM is corrupted", "The Operation Code is not set. Please fix the RAM and Instruction Set or load one of the demoes before resetting.");

    }
    else
    {
        xReg->setText(QString("0x%1").arg(generator->generate(), 8, 16, QChar('0')));
        yReg->setText(QString("0x%1").arg(generator->generate(), 8, 16, QChar('0')));
        zReg->setText(QString("0x%1").arg(generator->generate(), 8, 16, QChar('0')));
        marReg->setText(QString("0x%1").arg(generator->generate(), 8, 16, QChar('0')));
        mdrInReg->setText(QString("0x%1").arg(generator->generate(), 8, 16, QChar('0')));
        mdrOutReg->setText(QString("0x%1").arg(generator->generate(), 8, 16, QChar('0')));
        busButton->setText(QString("0x%1").arg(generator->generate(), 8, 16, QChar('0')));
        instructionRegOpcode->setText(QString("0x%1").arg(generator->bounded(256), 2, 16, QChar('0')));
        instructionReg1->setText(QString("0x%1").arg(generator->bounded(256), 2, 16, QChar('0')));
        instructionReg2->setText(QString("0x%1").arg(generator->bounded(256), 2, 16, QChar('0')));
        instructionReg3->setText(QString("0x%1").arg(generator->bounded(256), 2, 16, QChar('0')));
        aReg->setText(QString("0x%1").arg(generator->generate(), 8, 16, QChar('0')));
        progCounter->setText("0x00000000");
        nextStepButton->setEnabled(true);
        nextInstructionButton->setEnabled(true);
    }
    this->update();
}

void CPU::irExp()
{
    QMessageBox::information(this, "Instruction Register", "This is the Instruction Register. After the CPU fetches the instruction from the address that the PC provided, the instruction is\n"
"copied to the Instruction Register so that it can be decoded.");
}

void CPU::pcExp()
{
    QMessageBox::information(this, "Program Counter Register", "This is the Program Counter. It provides where the computer is in its program sequence. "
            "After the instruction is fetched through the MAR/MDR, the PC normally increments.");
}

void CPU::aRegExp()
{
    QMessageBox::information(this, "A Register", "This is just a register to cache data (e.g. a result computed by the ALU) for a short while if it is needed.");
}

void CPU::aluExp()
{
    QMessageBox::information(this, "Arithmetic Logic Unit", "This is the Arithmetic Logic Unit (ALU). It takes the Inputs X and Y and gives the Output Z. "
            "It can add, substract, shift left or right, store or halt.");
}

void CPU::mdrInExp()
{
    QMessageBox::information(this, "Memory Data Register In", "This is the inward Memory Data Register. It is used to fetch data from the memory. The data on the address the MAR provides will be read by the MDR In so it can be used by the CPU.");
}

void CPU::mdrOutExp()
{
    QMessageBox::information(this, "Memory Data Register Out", "This is the outward Memory Data Register. It is used to write data on the memory. The data on the MDR Out will be written on the memory to the address the MAR provides.");
}

void CPU::marExp()
{
    QMessageBox::information(this, "Memory Address Register", "This is the Memory Address Register. It provides the address in the memory that the MDR has to read from/write to.");
}

QBoxLayout* CPU::drawAlu()
{
    ///Arithmetic Logic Operator
    QVBoxLayout *aluLayout = new QVBoxLayout();
    QHBoxLayout *aluInLayout = new QHBoxLayout();
    QHBoxLayout *labelsLayout = new QHBoxLayout();
    QHBoxLayout *alu = new QHBoxLayout();
    QHBoxLayout *compLayout = new QHBoxLayout();

    zLabel = new QLabel("Z Register");
    zLabel->setAlignment(Qt::AlignCenter);
    zLabel->setMinimumSize(100, 23);

    xLabel = new QLabel("X Register");
    xLabel->setAlignment(Qt::AlignHCenter|Qt::AlignBottom);
    xLabel->setMinimumSize(100, 23);

    yLabel = new QLabel("Y Register");
    yLabel->setAlignment(Qt::AlignHCenter|Qt::AlignBottom);
    yLabel->setMinimumSize(100, 23);

    labelsLayout->addStretch();
    labelsLayout->addWidget(xLabel);
    labelsLayout->addWidget(yLabel);
    labelsLayout->addStretch();


    comparisons = new QPushButton("== 0, < 0, > 0 ?");
    comparisons->setStyleSheet("border: 3px solid black;");
    comparisons->setCursor(Qt::WhatsThisCursor);
    compLayout->addWidget(comparisons);
    compLayout->addStretch();
    aluButton = new QPushButton("Arithmetic Logic Unit");
    aluButton->setStyleSheet("border: 3px solid black;");
    aluButton->setCursor(Qt::WhatsThisCursor);
    QFont font = aluButton->font();
    font.setPointSize(10);
    font.setBold(true);
    font.setUnderline(true);
    aluButton->setFont(font);
    aluButton->setMinimumSize(200, 23);
    alu->addStretch();
    alu->addWidget(aluButton);
    alu->addStretch();

    xReg = new QPushButton("0x00000000");
    xReg->setStyleSheet("border: 3px solid black;");
    xReg->setCursor(Qt::WhatsThisCursor);
    xReg->setMinimumSize(100, 23);

    yReg = new QPushButton("0x00000000");
    yReg->setStyleSheet("border: 3px solid black;");
    yReg->setCursor(Qt::WhatsThisCursor);
    yReg->setMinimumSize(100, 23);


    aluInLayout->addStretch();
    aluInLayout->addWidget(xReg);
    aluInLayout->addWidget(yReg);
    aluInLayout->addStretch();

    zReg = new QPushButton("0x00000000");
    zReg->setStyleSheet("border: 3px solid black;");
    zReg->setCursor(Qt::WhatsThisCursor);
    zReg->setMinimumSize(100, 23);
    QHBoxLayout *zLayout = new QHBoxLayout();
    zLayout->addStretch();
    zLayout->addWidget(zReg);
    zLayout->addStretch();
    aluLayout->addLayout(compLayout);
    aluLayout->addStretch();
    aluLayout->addWidget(zLabel);
    aluLayout->addLayout(zLayout);
    aluLayout->addStretch();
    aluLayout->addLayout(alu);
    aluLayout->addStretch();
    aluLayout->addLayout(labelsLayout);
    aluLayout->addLayout(aluInLayout);

    connect(xReg, SIGNAL(clicked()), this, SLOT(aluExp()));
    connect(yReg, SIGNAL(clicked()), this, SLOT(aluExp()));
    connect(zReg, SIGNAL(clicked()), this, SLOT(aluExp()));

    return aluLayout;
}

void CPU::closeEvent(QCloseEvent* event)
{
    if (QMessageBox::No == QMessageBox::warning(this,
                                                "Microcode Simulator",
                                                "Are you sure you want to quit?",
                                                QMessageBox::Yes,
                                                QMessageBox::No)) {
        event->ignore();
    } else {
        event->accept();
        delete microcode;
        delete ram;
    }
}
