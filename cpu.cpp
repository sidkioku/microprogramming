#include "cpu.h"

bool secondstepread, secondstepwrite = false;
bool writeenable[15];
bool outputenable[13];
int condition, phase, nextRow, length, steps = 0;
int base = 2;
int fieldWidth = 8;
bool deadend, decoded, play = false;
bool fetching = true;
bool binary = true;
QRandomGenerator *generator = new QRandomGenerator();


CPU::CPU(QWidget *parent) : QMainWindow(parent)
{
    this->setWindowTitle("MicroCode Simulator");
    QWidget *widget = new QWidget();
    microcode = new microcodeROM();
    ram = new ramWindow();

    QVBoxLayout *romLayout = new QVBoxLayout();
    QHBoxLayout *stepsLayout = new QHBoxLayout();
    QVBoxLayout *irLayout = new QVBoxLayout();
    QVBoxLayout *pcLayout = new QVBoxLayout();
    QVBoxLayout *abLayout = new QVBoxLayout();
    QVBoxLayout *cdLayout = new QVBoxLayout();
    QBoxLayout *aluLayout = drawAlu();
    QVBoxLayout *alu = new QVBoxLayout();
    QVBoxLayout *memLayout = new QVBoxLayout();

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    QVBoxLayout *cpuLayout = new QVBoxLayout();
    QVBoxLayout *ramLayout = new QVBoxLayout();
    QHBoxLayout *completeLayout = new QHBoxLayout();

    ///Microcode ROM
    romButton = new QPushButton("Microcode ROM");
    romButton->setFlat(true);
    QFont font = romButton->font();
    font.setPointSize(10);
    font.setBold(true);
    font.setUnderline(true);
    romButton->setFont(font);
    romButton->setCursor(Qt::PointingHandCursor);
    romButton->setMinimumSize(125, 500);
    romButton->setStyleSheet("border: 3px solid black;");

    romLayout->addWidget(romButton);
    romLayout->addStretch();

    ///Multiple Steps
    stepCounter = new QSpinBox();
    stepCounter->setMinimumSize(60, 23);
    stepCounter->setAlignment(Qt::AlignRight);
    multipleStepsButton = new QPushButton("Steps", this);
    multipleStepsButton->setDisabled(true);
    multipleStepsButton->setMinimumSize(60, 23);

    stepsLayout->addWidget(stepCounter);
    stepsLayout->addWidget(multipleStepsButton);

    ///Single Step
    singleStepButton = new QPushButton("Single Step");
    singleStepButton->setDisabled(true);
    singleStepButton->setMinimumSize(125, 23);

    ///Reset
    resetButton = new QPushButton("&Reset");
    resetButton->setMinimumSize(125, 23);

    ///Instruction Register
    irLabel = new QLabel("Instruction Register");
    irLabel->setAlignment(Qt::AlignCenter);
    instructionReg = new QPushButton("00000000");
    instructionReg->setStyleSheet("border: 3px solid black;");
    instructionReg->setCursor(Qt::WhatsThisCursor);
    instructionReg->setMinimumSize(125, 23);

    irLayout->addLayout(stepsLayout);
    irLayout->addWidget(singleStepButton);
    irLayout->addWidget(resetButton);
    irLayout->addStretch();
    irLayout->addWidget(irLabel);
    irLayout->addWidget(instructionReg);


    ///Open Microcode ROM
    openMicrocode = new QPushButton("&Open Microcode ROM...");
    openMicrocode->setMinimumSize(125, 23);
    openMicrocode->setCursor(Qt::PointingHandCursor);

    ///Save Microcode ROM
    saveMicrocode = new QPushButton("&Save Microcode ROM...");
    saveMicrocode->setMinimumSize(125, 23);
    saveMicrocode->setCursor(Qt::PointingHandCursor);

    ///GPIO Out
    gpioOut1 = new QRadioButton("Output Pin 1");
    gpioOut2 = new QRadioButton("Output Pin 2");
    gpioOut1->setStyleSheet("border: 1px solid black;");
    gpioOut2->setStyleSheet("border: 1px solid black;");
    gpioOut1->setMinimumSize(125, 23);
    gpioOut2->setMinimumSize(125, 23);
    gpioOut1->setAutoExclusive(false);
    gpioOut2->setAutoExclusive(false);



    ///Program Counter
    pcLabel = new QLabel("Program Counter");
    pcLabel->setAlignment(Qt::AlignCenter);
    progCounter = new QPushButton("00000000");
    progCounter->setStyleSheet("border: 3px solid black;");
    progCounter->setCursor(Qt::WhatsThisCursor);
    progCounter->setMinimumSize(125, 23);


    pcLayout->addWidget(openMicrocode);
    pcLayout->addWidget(saveMicrocode);
    pcLayout->addSpacing(25);
    pcLayout->addWidget(gpioOut1);
    pcLayout->addWidget(gpioOut2);
    pcLayout->addStretch();
    pcLayout->addWidget(pcLabel);
    pcLayout->addWidget(progCounter);

    ///Open RAM
    openRAM = new QPushButton("Open &RAM...");
    openRAM->setMinimumSize(125, 23);
    openRAM->setCursor(Qt::PointingHandCursor);

    ///Save RAM
    saveRAM = new QPushButton("Save RA&M...");
    saveRAM->setMinimumSize(125, 23);
    saveRAM->setCursor(Qt::PointingHandCursor);


    ///GPIO In
    gpioIn1 = new QRadioButton("Input Pin 1");
    gpioIn2 = new QRadioButton("Input Pin 2");
    gpioIn1->setStyleSheet("border: 1px solid black;");
    gpioIn2->setStyleSheet("border: 1px solid black;");
    gpioIn1->setMinimumSize(125, 23);
    gpioIn2->setMinimumSize(125, 23);
    gpioIn1->setAutoExclusive(false);
    gpioIn2->setAutoExclusive(false);


    ///A Register & B Register
    aLabel = new QLabel("A Register");
    aLabel->setAlignment(Qt::AlignCenter);
    aReg = new QPushButton("00000000");
    aReg->setStyleSheet("border: 3px solid black;");
    aReg->setCursor(Qt::WhatsThisCursor);
    aReg->setMinimumSize(125, 23);

    bLabel = new QLabel("B Register");
    bLabel->setAlignment(Qt::AlignCenter);
    bReg = new QPushButton("00000000");
    bReg->setStyleSheet("border: 3px solid black;");
    bReg->setCursor(Qt::WhatsThisCursor);
    bReg->setMinimumSize(125, 23);

    abLayout->addWidget(openRAM);
    abLayout->addWidget(saveRAM);
    abLayout->addSpacing(25);
    abLayout->addWidget(gpioIn1);
    abLayout->addWidget(gpioIn2);
    abLayout->addStretch();
    abLayout->addWidget(aLabel);
    abLayout->addWidget(aReg);
    abLayout->addWidget(bLabel);
    abLayout->addWidget(bReg);

    ///Comparisons
    comparisons = new QPushButton("== 0, < 0, > 0 ?");
    comparisons->setStyleSheet("border: 3px solid black;");
    comparisons->setCursor(Qt::WhatsThisCursor);

    ///C and D Registers
    cLabel = new QLabel("C Register");
    cLabel->setAlignment(Qt::AlignCenter);
    cReg = new QPushButton("00000000");
    cReg->setStyleSheet("border: 3px solid black;");
    cReg->setCursor(Qt::WhatsThisCursor);
    cReg->setMinimumSize(125, 23);

    dLabel = new QLabel("D Register");
    dLabel->setAlignment(Qt::AlignCenter);
    dReg = new QPushButton("00000000");
    dReg->setStyleSheet("border: 3px solid black;");
    dReg->setCursor(Qt::WhatsThisCursor);
    dReg->setMinimumSize(125, 23);

    cdLayout->addStretch();
    cdLayout->addWidget(comparisons);
    cdLayout->addStretch();
    cdLayout->addWidget(cLabel);
    cdLayout->addWidget(cReg);
    cdLayout->addWidget(dLabel);
    cdLayout->addWidget(dReg);

    ///Arithmetic Logic Unit
    alu->addStretch();
    alu->addLayout(aluLayout);


    hexbin = new QPushButton("Hexadecimal", this);
    ///Memory Access Registers
    marLabel = new QLabel("Memory Address Register", this);
    marLabel->setAlignment(Qt::AlignCenter);
    marReg = new QPushButton("00000000");
    marReg->setCursor(Qt::WhatsThisCursor);
    marReg->setStyleSheet("border: 3px solid black;");
    marReg->setMinimumSize(125, 23);

    mdrInLabel = new QLabel("Memory Data Register In", this);
    mdrInLabel->setAlignment(Qt::AlignCenter);
    mdrInReg = new QPushButton("00000000");
    mdrInReg->setCursor(Qt::WhatsThisCursor);
    mdrInReg->setStyleSheet("border: 3px solid black;");
    mdrInReg->setMinimumSize(125, 23);

    mdrOutLabel = new QLabel("Memory Data Register Out");
    mdrOutLabel->setAlignment(Qt::AlignCenter);
    mdrOutReg = new QPushButton("00000000");
    mdrOutReg->setCursor(Qt::WhatsThisCursor);
    mdrOutReg->setStyleSheet("border: 3px solid black;");
    mdrOutReg->setMinimumSize(125, 23);

    memLayout->addWidget(hexbin);
    memLayout->addStretch();
    memLayout->addWidget(marLabel);
    memLayout->addWidget(marReg);
    memLayout->addStretch();
    memLayout->addWidget(mdrInLabel);
    memLayout->addWidget(mdrInReg);
    memLayout->addWidget(mdrOutLabel);
    memLayout->addWidget(mdrOutReg);

    ///RAM Button
    ramButton = new QPushButton("External RAM");
    QPalette pal = ramButton->palette();
    ramButton->setFlat(true);
    ramButton->setAutoFillBackground(true);
    ramButton->setCursor(Qt::PointingHandCursor);
    pal.setColor(QPalette::Button, QColor(Qt::gray));
    ramButton->setPalette(pal);
    ramButton->setFont(font);
    ramButton->setMinimumSize(100, 500);
    ramButton->setStyleSheet("border: 3px solid black;");
    ramButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    ///Outer Data Bus
    data = new QFrame(this);
    data->setFrameShape(QFrame::HLine);
    data->setFrameShadow(QFrame::Plain);
    data->setLineWidth(3);
    data->setMinimumSize(100, 3);
    data->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    ramLayout->addWidget(ramButton);
    ramLayout->addStretch();
    ramLayout->addWidget(data);

    buttonsLayout->addLayout(romLayout);
    buttonsLayout->addLayout(irLayout);
    buttonsLayout->addLayout(pcLayout);
    buttonsLayout->addSpacing(50);
    buttonsLayout->addLayout(abLayout);
    buttonsLayout->addLayout(cdLayout);
    buttonsLayout->addSpacing(50);
    buttonsLayout->addLayout(alu);
    buttonsLayout->addSpacing(75);
    buttonsLayout->addLayout(memLayout);


    ///Inner Data Bus
    bus = new QFrame(this);
    bus->setFrameShape(QFrame::HLine);
    bus->setFrameShadow(QFrame::Plain);
    bus->setLineWidth(3);

    busLabel = new QLabel("Data Bus");
    busLabel->setAlignment(Qt::AlignCenter);
    busButton = new QPushButton("00000000");
    busButton->setStyleSheet("border: 3px solid black;");
    busButton->setMinimumSize(125, 23);
    busButton->setCursor(Qt::WhatsThisCursor);

    cpuLayout->addSpacing(5);
    cpuLayout->addLayout(buttonsLayout);
    cpuLayout->addSpacing(40);
    cpuLayout->addWidget(bus);
    cpuLayout->addWidget(busButton);
    cpuLayout->setAlignment(busButton, Qt::AlignCenter);


    completeLayout->addLayout(cpuLayout);
    completeLayout->addSpacing(45);
    completeLayout->addLayout(ramLayout);


    widget->setLayout(completeLayout);
    this->setCentralWidget(widget);
    this->setMinimumSize(1400, 665);


    QFile file(":/demoROMs/instruction fetch.rom");
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    QString text = in.readAll();
    file.close();
    microcode->readRom(&text);

    ///Connections
    connect(romButton, SIGNAL(clicked()), this, SLOT(microcodeOpen()));
    connect(ramButton, SIGNAL(clicked()), this, SLOT(ramOpen()));
    connect(multipleStepsButton, SIGNAL(clicked()), this, SLOT(multipleSteps()));
    connect(singleStepButton, SIGNAL(clicked()), this, SLOT(singleStep()));
    connect(resetButton, SIGNAL(clicked()), this, SLOT(reset()));
    connect(openMicrocode, SIGNAL(clicked()), this, SLOT(microcodeFile()));
    connect(saveMicrocode, SIGNAL(clicked()), this, SLOT(saveRom()));
    connect(openRAM, SIGNAL(clicked()), this, SLOT(ramFile()));
    connect(saveRAM, SIGNAL(clicked()), this, SLOT(saveRam()));
    connect(progCounter, SIGNAL(clicked()), this, SLOT(pcExp()));
    connect(instructionReg, SIGNAL(clicked()), this, SLOT(irExp()));
    connect(aReg, SIGNAL(clicked()), this, SLOT(aRegExp()));
    connect(marReg, SIGNAL(clicked()), this, SLOT(marExp()));
    connect(mdrInReg, SIGNAL(clicked()), this, SLOT(mdrInExp()));
    connect(mdrOutReg, SIGNAL(clicked()), this, SLOT(mdrOutExp()));
    connect(gpioOut1, SIGNAL(clicked()), this, SLOT(gpioClick()));
    connect(gpioOut2, SIGNAL(clicked()), this, SLOT(gpioClick()));
    connect(hexbin, SIGNAL(clicked()), this, SLOT(changeBase()));
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
    redPen.setWidth(4);
    blackPen.setWidth(4);
    dashedPen.setWidth(2);
    dashedRedPen.setWidth(2);
    grayPen.setWidth(2);
    painter.setPen(blackPen);

    ///Arrows to the BUS

    //Arrow Instruction Register <-> BUS
    if(outputenable[0])
    {
        painter.setPen(redPen);
        painter.drawPixmap(instructionReg->x() + instructionReg->width()/2 - 9, bus->y() - 15, arrowDownRed->pixmap(18, 18));
        //        painter.drawPixmap(instructionReg->x() + instructionReg->width()/2 - 9, instructionReg->y() + instructionReg->height() - 3, arrowUp->pixmap(18, 18));

    }
    else if (writeenable[0])
    {
        painter.setPen(redPen);
        painter.drawPixmap(instructionReg->x() + instructionReg->width()/2 - 9, instructionReg->y() + instructionReg->height() - 3, arrowUpRed->pixmap(18, 18));
        //        painter.drawPixmap(instructionReg->x() + instructionReg->width()/2 - 9, bus->y() - 15, arrowDown->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawPixmap(instructionReg->x() + instructionReg->width()/2 - 9, instructionReg->y() + instructionReg->height() - 3, arrowUp->pixmap(18, 18));
        painter.drawPixmap(instructionReg->x() + instructionReg->width()/2 - 9, bus->y() - 15, arrowDown->pixmap(18, 18));
    }
    painter.drawLine(instructionReg->x() + instructionReg->width()/2, instructionReg->y() + instructionReg->height() + 5, instructionReg->x() + instructionReg->width()/2, bus->y() - 5);

    //Arrow Program Counter <-> BUS
    if (writeenable[1])
    {
        painter.setPen(redPen);
        painter.drawLine(progCounter->x() + progCounter->width()/2,       bus->y() - 5, progCounter->x() + progCounter->width()/2,       progCounter->y() + progCounter->height() + 5);
        painter.drawPixmap(progCounter->x() + progCounter->width()/2 - 9,       progCounter->y() + progCounter->height() - 3, arrowUpRed->pixmap(18, 18));
        painter.setPen(blackPen);
        //        painter.drawPixmap(progCounter->x() + progCounter->width()/2 - 9,       bus->y() - 15, arrowDown->pixmap(18, 18));
    }
    else if (outputenable[1])
    {
        painter.setPen(redPen);
        painter.drawLine(progCounter->x() + progCounter->width()/2,       bus->y() - 5, progCounter->x() + progCounter->width()/2,       progCounter->y() + progCounter->height() + 5);
        painter.drawPixmap(progCounter->x() + progCounter->width()/2 - 9,       bus->y() - 15, arrowDownRed->pixmap(18, 18));
        painter.setPen(blackPen);
        //        painter.drawPixmap(progCounter->x() + progCounter->width()/2 - 9,       progCounter->y() + progCounter->height() - 3, arrowUp->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawLine(progCounter->x() + progCounter->width()/2,       bus->y() - 5, progCounter->x() + progCounter->width()/2,       progCounter->y() + progCounter->height() + 5);
        painter.drawPixmap(progCounter->x() + progCounter->width()/2 - 9,       bus->y() - 15, arrowDown->pixmap(18, 18));
        painter.drawPixmap(progCounter->x() + progCounter->width()/2 - 9,       progCounter->y() + progCounter->height() - 3, arrowUp->pixmap(18, 18));
    }

    //Arrow A Register <-> BUS
    if (writeenable[2])
    {
        painter.setPen(redPen);
        //        painter.drawPixmap(aReg->x() - 25 - 9, bus->y() - 15, arrowDown->pixmap(18, 18));
        painter.drawPixmap(aReg->x() - 18, aReg->y() + aReg->height()/2 - 9, arrowRightRed->pixmap(18, 18));
    }
    else if (outputenable[2])
    {
        painter.setPen(redPen);
        painter.drawPixmap(aReg->x() - 25 - 9, bus->y() - 15, arrowDownRed->pixmap(18, 18));
        //        painter.drawPixmap(aReg->x() - 18, aReg->y() + aReg->height()/2 - 9, arrowRight->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawPixmap(aReg->x() - 25 - 9, bus->y() - 15, arrowDown->pixmap(18, 18));
        painter.drawPixmap(aReg->x() - 18, aReg->y() + aReg->height()/2 - 9, arrowRight->pixmap(18, 18));
    }
    painter.drawLine(aReg->x() - 25, bus->y() - 5, aReg->x() - 25, aReg->y() + aReg->height()/2);
    painter.drawLine(aReg->x() - 25, aReg->y() + aReg->height()/2, aReg->x() - 5, aReg->y() + aReg->height() / 2);

    //Arrow B Register <-> BUS
    if (writeenable[3])
    {
        painter.setPen(redPen);
        painter.drawPixmap(bReg->x() + bReg->width()/2 - 9, bReg->y() + bReg->height() - 3, arrowUpRed->pixmap(18, 18));
        //        painter.drawPixmap(bReg->x() +bReg->width()/2 - 9, bus->y() - 15, arrowDown->pixmap(18, 18));

    }
    else if (outputenable[3])
    {
        painter.setPen(redPen);
        //        painter.drawPixmap(bReg->x() + bReg->width()/2 - 9, bReg->y() + bReg->height() - 3, arrowUp->pixmap(18, 18));
        painter.drawPixmap(bReg->x() +bReg->width()/2 - 9, bus->y() - 15, arrowDownRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawPixmap(bReg->x() + bReg->width()/2 - 9, bReg->y() + bReg->height() - 3, arrowUp->pixmap(18, 18));
        painter.drawPixmap(bReg->x() +bReg->width()/2 - 9, bus->y() - 15, arrowDown->pixmap(18, 18));
    }
    painter.drawLine(bReg->x() + bReg->width()/2, bus->y() - 5, bReg->x() + bReg->width()/2, bReg->y() + bReg->height() + 5);

    //Arrow C Register <-> BUS
    if(writeenable[4])
    {
        painter.setPen(redPen);
        painter.drawPixmap(cReg->x() + cReg->width(), cReg->y() + cReg->height()/2 - 9, arrowLeftRed->pixmap(18, 18));
        //        painter.drawPixmap(cReg->x() + cReg->width() + 25 - 9, bus->y() - 15, arrowDown->pixmap(18, 18));
    }
    else if (outputenable[4])
    {
        painter.setPen(redPen);
        //        painter.drawPixmap(cReg->x() + cReg->width(), cReg->y() + cReg->height()/2 - 9, arrowLeft->pixmap(18, 18));
        painter.drawPixmap(cReg->x() + cReg->width() + 25 - 9, bus->y() - 15, arrowDownRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawPixmap(cReg->x() + cReg->width(), cReg->y() + cReg->height()/2 - 9, arrowLeft->pixmap(18, 18));
        painter.drawPixmap(cReg->x() + cReg->width() + 25 - 9, bus->y() - 15, arrowDown->pixmap(18, 18));
    }
    painter.drawLine(cReg->x() + cReg->width() + 5, cReg->y() + cReg->height()/2, cReg->x() + cReg->width() + 25, cReg->y() + cReg->height()/2);
    painter.drawLine(cReg->x() + cReg->width() + 25, cReg->y() + cReg->height()/2, cReg->x() + cReg->width() + 25, bus->y() - 5);

    //Arrow D Register <-> BUS
    if (writeenable[5])
    {
        painter.setPen(redPen);
        painter.drawPixmap(dReg->x() + dReg->width()/2 - 9, dReg->y() + dReg->height() - 3, arrowUpRed->pixmap(18, 18));
        //        painter.drawPixmap(dReg->x() +dReg->width()/2 - 9, bus->y() - 15, arrowDown->pixmap(18, 18));

    }
    else if (outputenable[5])
    {
        painter.setPen(redPen);
        //        painter.drawPixmap(dReg->x() + dReg->width()/2 - 9, dReg->y() + dReg->height() - 3, arrowUp->pixmap(18, 18));
        painter.drawPixmap(dReg->x() +dReg->width()/2 - 9, bus->y() - 15, arrowDownRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawPixmap(dReg->x() + dReg->width()/2 - 9, dReg->y() + dReg->height() - 3, arrowUp->pixmap(18, 18));
        painter.drawPixmap(dReg->x() +dReg->width()/2 - 9, bus->y() - 15, arrowDown->pixmap(18, 18));
    }
    painter.drawLine(dReg->x() + dReg->width()/2, bus->y() - 5, dReg->x() + dReg->width()/2, dReg->y() + dReg->height() + 5);

    //Arrow BUS -> X Register
    if (writeenable[6]){
        painter.setPen(redPen);
        painter.drawPixmap(xReg->x() + xReg->width()/2 - 9,                     xReg->y() + xReg->height() - 1, arrowUpRed->pixmap(18, 18));
    } else
    {
        painter.setPen(blackPen);
        painter.drawPixmap(xReg->x() + xReg->width()/2 - 9,                     xReg->y() + xReg->height() - 1, arrowUp->pixmap(18, 18));
    }
    painter.drawLine(xReg->x() + xReg->width()/2,                     bus->y(), xReg->x() + xReg->width()/2,                     xReg->y() + xReg->height() + 5);

    //Arrow BUS -> Y Register
    if (writeenable[7])
    {
        painter.setPen(redPen);
        painter.drawPixmap(yReg->x() + yReg->width()/2 - 9,                     yReg->y() + yReg->height() - 1, arrowUpRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawPixmap(yReg->x() + yReg->width()/2 - 9,                     yReg->y() + yReg->height() - 1, arrowUp->pixmap(18, 18));
    }
    painter.drawLine(yReg->x() + yReg->width()/2,                     bus->y(),yReg->x() + yReg->width()/2,                     yReg->y() + yReg->height() + 5);

    ///ALU Lines

    if (outputenable[7] & writeenable[8]) //TODO: Check which enables are needed
    {
        painter.setPen(redPen);
        // Arrow X -> ALU Button Red
        painter.drawPixmap(xReg->x() + 3 * xReg->width()/4 - 9, aluButton->y() + aluButton->height() - 1,           arrowUpRed->pixmap(18, 18));

        //Arrow Y -> ALU Button Red
        painter.drawPixmap(yReg->x() + yReg->width()/4 - 9, aluButton->y() + aluButton->height() - 1,           arrowUpRed->pixmap(18, 18));

        //Arrow ALU Button -> Z Red
        painter.drawPixmap(aluButton->x() + aluButton->width()/2 - 9, zReg->y() + zReg->height(), arrowUpRed->pixmap(18, 18));

    } else
    {
        painter.setPen(blackPen);
        // Arrow X -> ALU Button Black
        painter.drawPixmap(xReg->x() + 3 * xReg->width()/4 - 9,               aluButton->y() + aluButton->height() - 1,           arrowUp->pixmap(18, 18));

        //Arrow Y -> ALU Button Black
        painter.drawPixmap(yReg->x() + yReg->width()/4 - 9,              aluButton->y() + aluButton->height() - 1,           arrowUp->pixmap(18, 18));

        //Arrow ALU Button -> Z Black
        painter.drawPixmap(aluButton->x() + aluButton->width()/2 - 9, zReg->y() + zReg->height(), arrowUp->pixmap(18, 18));

    }
    painter.drawLine(xReg->x() + 3 * xReg->width()/4,                     xReg->y(),                                      xReg->x() + 3 * xReg->width()/4,                     aluButton->y() + aluButton->height() + 5);
    painter.drawLine(yReg->x() + yReg->width()/4,                     yReg->y(),                                      yReg->x() + yReg->width()/4,                     aluButton->y() + aluButton->height() + 5);
    painter.drawLine(aluButton->x() + aluButton->width()/2,           aluButton->y(),                                 aluButton->x() + aluButton->width()/2,                     zReg->y() + zReg->height() + 5);

    //Arrow Z -> BUS
    if (outputenable[6])
    {
        painter.setPen(redPen);
        painter.drawPixmap(xReg->x() - 10 - 9, bus->y() - 15, arrowDownRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawPixmap(xReg->x() - 10 - 9, bus->y() - 15, arrowDown->pixmap(18, 18));
    }
    painter.drawLine(zReg->x(), zReg->y() + 3 * zReg->height()/4, xReg->x() - 10,    zReg->y() + 3 * zReg->height()/4);
    painter.drawLine(xReg->x() - 10,    zReg->y() + 3 * zReg->height()/4, xReg->x() - 10, bus->y() - 5);

    //Arrow Comparisons -> Microcode ROM & Z -> Comparisons
    if (condition > 0 && condition < 7)
    {
        painter.setPen(redPen);
        painter.drawPixmap(romButton->x() + romButton->width(), comparisons->y() + comparisons->height()/2 - 9, arrowLeftRed->pixmap(18, 18));
        painter.drawPixmap(comparisons->x() + comparisons->width()/2 - 9, comparisons->y() + comparisons->height() - 1, arrowUpRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawPixmap(romButton->x() + romButton->width(), comparisons->y() + comparisons->height()/2 - 9, arrowLeft->pixmap(18, 18));
        painter.drawPixmap(comparisons->x() + comparisons->width()/2 - 9, comparisons->y() + comparisons->height() - 1, arrowUp->pixmap(18, 18));
    }
    painter.drawLine(comparisons->x(), comparisons->y() + comparisons->height()/2, romButton->x() + romButton->width() + 5, comparisons->y() + comparisons->height()/2);
    painter.drawLine(zReg->x(), zReg->y() + zReg->height()/4, comparisons->x() + comparisons->width()/2, zReg->y() + zReg->height()/4);
    painter.drawLine(comparisons->x() + comparisons->width()/2, zReg->y() + zReg->height()/4, comparisons->x() + comparisons->width()/2, comparisons->y() + comparisons->height() + 5);

    //Arrow Carry -> ALU
    if (outputenable[9])
    {
        painter.setPen(redPen);
        painter.drawPixmap(aluButton->x() + aluButton->width(), aluButton->y() + aluButton->height()/2 - 9, arrowLeftRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawPixmap(aluButton->x() + aluButton->width(), aluButton->y() + aluButton->height()/2 - 9, arrowLeft->pixmap(18, 18));
    }
    painter.drawLine(carryReg->x() + 3 * carryReg->width() / 4, carryReg->y() + carryReg->height(), carryReg->x() + 3 * carryReg->width() / 4, aluButton->y() + aluButton->height() / 2);
    painter.drawLine(carryReg->x() + 3 * carryReg->width() / 4, aluButton->y() + aluButton->height() / 2, aluButton->x() + aluButton->width() + 4, aluButton->y() + aluButton->height() / 2);

    //Arrow BUS -> Carry
    if (writeenable[12])
    {
        painter.setPen(redPen);
        painter.drawPixmap(carryReg->x() + carryReg->width(), carryReg->y() + carryReg->height() / 2 - 9, arrowLeftRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawPixmap(carryReg->x() + carryReg->width(), carryReg->y() + carryReg->height() / 2 - 9, arrowLeft->pixmap(18, 18));
    }
    painter.drawLine(carryReg->x() + carryReg->width() + 5, carryReg->y() + carryReg->height() / 2, carryReg->x() + carryReg->width() + 20, carryReg->y() + carryReg->height() / 2);
    painter.drawLine(carryReg->x() + carryReg->width() + 20, carryReg->y() + carryReg->height() / 2, carryReg->x() + carryReg->width() + 20, bus->y());
    ///Memory Lines
    //Arrow MDR IN -> BUS
    if (outputenable[7])
    {
        painter.setPen(redPen);
        painter.drawPixmap( mdrInReg->x() - 35 - 9,             bus->y() - 18, arrowDownRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawPixmap( mdrInReg->x() - 35 - 9,             bus->y() - 18, arrowDown->pixmap(18, 18));
    }
    painter.drawLine(mdrInReg->x(),                                   mdrInReg->y() + mdrInReg->height()/2,           mdrInReg->x() - 35,             mdrInReg->y() + mdrInReg->height()/2);
    painter.drawLine(mdrInReg->x() - 35,            mdrInReg->y() + mdrInReg->height()/2,           mdrInReg->x() - 35,             bus->y() - 5);

    //Arrow BUS -> MDR OUT
    if (writeenable[11])
    {
        painter.setPen(redPen);
        painter.drawPixmap(mdrOutReg->x() - 18,                          mdrOutReg->y() + mdrOutReg->height()/2 - 9, arrowRightRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawPixmap(mdrOutReg->x() - 18,                          mdrOutReg->y() + mdrOutReg->height()/2 - 9, arrowRight->pixmap(18, 18));
    }
    painter.drawLine(mdrOutReg->x() - 20,          bus->y(), mdrOutReg->x() - 20,          mdrOutReg->y() + mdrOutReg->height()/2);
    painter.drawLine(mdrOutReg->x() - 20,          mdrOutReg->y() + mdrOutReg->height()/2, mdrOutReg->x() - 5,                                  mdrOutReg->y() + mdrOutReg->height()/2);

    //Arrow BUS -> MAR
    if (writeenable[9])
    {
        painter.setPen(redPen);
        painter.drawPixmap(marReg->x() - 18,                                     marReg->y() + marReg->height()/2 - 9, arrowRightRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawPixmap(marReg->x() - 18,                                     marReg->y() + marReg->height()/2 - 9, arrowRight->pixmap(18, 18));
    }
    painter.drawLine(marReg->x() - 50,                 bus->y(), marReg->x() - 50,                 marReg->y() + marReg->height()/2);
    painter.drawLine(marReg->x() - 50,                 marReg->y() + marReg->height()/2, marReg->x() - 5,                                     marReg->y() + marReg->height()/2);

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
    if (writeenable[10])
    {
        painter.setPen(dashedRedPen);
        painter.drawPixmap(mdrInReg->x() + mdrInReg->width(), mdrInReg->y() + mdrInReg->height()/2 - 9, arrowLeftRed->pixmap(18, 18));
    }
    else {
        painter.setPen(dashedPen);
        painter.drawPixmap(mdrInReg->x() + mdrInReg->width(), mdrInReg->y() + mdrInReg->height()/2 - 9, arrowLeft->pixmap(18, 18));
    }
    painter.drawLine( data->x() + data->width()/4, mdrInReg->y() + mdrInReg->height()/2,  data->x() + data->width()/4, data->y() - 15);
    painter.drawLine(mdrInReg->x() + mdrInReg->width() + 5, mdrInReg->y() + mdrInReg->height()/2, data->x() + data->width()/4, mdrInReg->y() + mdrInReg->height()/2);

    //Arrow MDR Out -> Outer Data Bus
    if (outputenable[8])
    {
        painter.setPen(dashedRedPen);
        painter.drawPixmap( data->x() + data->width()/4 - 9, data->y() - 18, arrowDownRed->pixmap(18, 18));
    } else
    {
        painter.setPen(dashedPen);
        painter.drawPixmap( data->x() + data->width()/4 - 9, data->y() - 18, arrowDown->pixmap(18, 18));
    }
    painter.drawLine(mdrOutReg->x() + mdrOutReg->width(), mdrOutReg->y() + mdrOutReg->height()/2,data->x() + data->width()/4, mdrOutReg->y() + mdrOutReg->height()/2);
    painter.drawLine( data->x() + data->width()/4, mdrOutReg->y() + mdrOutReg->height()/2,  data->x() + data->width()/4, data->y() - 5);

    //Arrow RAM <-> Outer Data Bus
    if (outputenable[8])
    {
        painter.setPen(dashedRedPen);
        painter.drawPixmap(data->x() + 3 * data->width()/4 - 9, data->y() - 18, arrowDown->pixmap(18, 18));
        painter.drawPixmap(data->x() + 3 * data->width()/4 - 9, ramButton->y() + ramButton->height(), arrowUpRed->pixmap(18, 18));

    }
    else if (writeenable[10])
    {
        painter.setPen(dashedRedPen);
        painter.drawPixmap(data->x() + 3 * data->width()/4 - 9, data->y() - 18, arrowDownRed->pixmap(18, 18));
        painter.drawPixmap(data->x() + 3 * data->width()/4 - 9, ramButton->y() + ramButton->height(), arrowUp->pixmap(18, 18));
    }
    else
    {
        painter.setPen(dashedPen);
        painter.drawPixmap(data->x() + 3 * data->width()/4 - 9, data->y() - 18, arrowDown->pixmap(18, 18));
        painter.drawPixmap(data->x() + 3 * data->width()/4 - 9, ramButton->y() + ramButton->height(), arrowUp->pixmap(18, 18));
    }
    painter.drawLine(data->x() + 3 * data->width()/4, ramButton->y() + ramButton->height() + 15, data->x() + 3 * data->width()/4, data->y() - 15);


    //Microcode ROM -> GPIO Out

    if (outputenable[10])
    {
        painter.setPen(redPen);
        painter.drawPixmap(gpioOut1->x() - 18, gpioOut1->y() + gpioOut1->height() / 2 - 9, arrowRightRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawPixmap(gpioOut1->x() - 18, gpioOut1->y() + gpioOut1->height() / 2 - 9, arrowRight->pixmap(18, 18));
    }
    painter.drawLine(romButton->x() + romButton->width(), gpioOut1->y() + gpioOut1->height() / 2, gpioOut1->x() - 9, gpioOut1->y() + gpioOut1->height() / 2);

    if (outputenable[11])
    {
        painter.setPen(redPen);
        painter.drawPixmap(gpioOut2->x() - 18, gpioOut2->y() + gpioOut2->height() / 2 - 9, arrowRightRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawPixmap(gpioOut2->x() - 18, gpioOut2->y() + gpioOut2->height() / 2 - 9, arrowRight->pixmap(18, 18));
    }
    painter.drawLine(romButton->x() + romButton->width(), gpioOut2->y() + gpioOut2->height() / 2, gpioOut2->x() - 9, gpioOut2->y() + gpioOut2->height() / 2);


    //GPIO In -> Microcode ROM
    if (writeenable[13])
    {
        painter.setPen(redPen);
        painter.drawPixmap(romButton->x() + romButton->width(), gpioIn2->y() + gpioIn2->height() + 50 - 9, arrowLeftRed->pixmap(18, 18));
    }

    else
    {
        painter.setPen(blackPen);
        painter.drawPixmap(romButton->x() + romButton->width(), gpioIn2->y() + gpioIn2->height() + 50 - 9, arrowLeft->pixmap(18, 18));
    }
    painter.drawLine(gpioIn1->x() + gpioIn1->width(), gpioIn1->y() + gpioIn1->height() / 2, gpioIn1->x() + gpioIn1->width() + 50, gpioIn1->y() + gpioIn1->height() / 2);
    painter.drawLine(gpioIn1->x() + gpioIn1->width() + 50, gpioIn1->y() + gpioIn1->height() / 2, gpioIn1->x() + gpioIn1->width() + 50, gpioIn2->y() + gpioIn2->height() + 50);
    painter.drawLine(gpioIn1->x() + gpioIn1->width() + 50, gpioIn2->y() + gpioIn2->height() + 50, romButton->x() + romButton->width() + 9, gpioIn2->y() + gpioIn2->height() + 50);

    if (writeenable[14])
    {
        painter.setPen(redPen);
        painter.drawPixmap(romButton->x() + romButton->width(), gpioIn2->y() + gpioIn2->height() + 25 - 9, arrowLeftRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawPixmap(romButton->x() + romButton->width(), gpioIn2->y() + gpioIn2->height() + 25 - 9, arrowLeft->pixmap(18, 18));
    }
    painter.drawLine(gpioIn2->x() + gpioIn2->width(), gpioIn2->y() + gpioIn2->height() / 2, gpioIn2->x() + gpioIn2->width() + 25, gpioIn2->y() + gpioIn2->height() / 2);
    painter.drawLine(gpioIn2->x() + gpioIn2->width() + 25, gpioIn2->y() + gpioIn2->height() / 2, gpioIn2->x() + gpioIn2->width() + 25, gpioIn2->y() + gpioIn2->height() + 25);
    painter.drawLine(gpioIn2->x() + gpioIn2->width() + 25, gpioIn2->y() + gpioIn2->height() + 25, romButton->x() + romButton->width() + 9, gpioIn2->y() + gpioIn2->height() + 25);
}

void CPU::microcodeFile()
{
    QString readMicrocode = QFileDialog::getOpenFileName(this, "Open Microcode ROM...", QDir::homePath(), "ROM Files (*.rom)");
    QFile file(readMicrocode);
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);
        QString text = in.readAll();
        file.close();
        microcode->readRom(&text);
        singleStepButton->setDisabled(true);
        multipleStepsButton->setDisabled(true);
    }
}

void CPU::saveRom()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save Microcode ROM...", QDir::homePath(), "ROM Files (*.rom)");
    QFile f(filename);
    if (f.open(QIODevice::WriteOnly))
    {
        QTextStream out(&f);
        QString text = microcode->saveRom();
        out << text;
        f.flush();
        f.close();
    }
}

void CPU::ramFile() {
    QString readRAM = QFileDialog::getOpenFileName(this, "Open RAM File...", QDir::homePath(), "RAM Files (*.ram)");
    QFile file(readRAM);
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);
        QString text = in.readAll();
        file.close();
        ram->readRam(&text);
        singleStepButton->setDisabled(true);
        multipleStepsButton->setDisabled(true);
    }
}

void CPU::saveRam()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save RAM File...", QDir::homePath(), "RAM File (*.ram)");
    QFile f(filename);
    if (f.open(QIODevice::WriteOnly))
    {
        QTextStream out(&f);
        QString text = ram->saveRam();
        out << text;
        f.flush();
        f.close();
    }
}

void CPU::gpioClick()
{
    QRadioButton *clickedButton = qobject_cast<QRadioButton *>(sender());
    clickedButton->setChecked(!clickedButton->isChecked());
}

void CPU::changeBase()
{
    binary = !binary;
    if (binary) //Binary
    {
        base = 2;
        fieldWidth = 8;
        hexbin->setText("Hexadecimal");
        instructionReg->setText(QString("%1").arg(instructionReg->text().toInt(nullptr, 16), 8, 2, QChar('0')));
        progCounter->setText(QString("%1").arg(progCounter->text().toInt(nullptr, 16), 8, 2, QChar('0')));
        aReg->setText(QString("%1").arg(aReg->text().toInt(nullptr, 16), 8, 2, QChar('0')));
        bReg->setText(QString("%1").arg(bReg->text().toInt(nullptr, 16), 8, 2, QChar('0')));
        cReg->setText(QString("%1").arg(cReg->text().toInt(nullptr, 16), 8, 2, QChar('0')));
        dReg->setText(QString("%1").arg(dReg->text().toInt(nullptr, 16), 8, 2, QChar('0')));
        xReg->setText(QString("%1").arg(xReg->text().toInt(nullptr, 16), 8, 2, QChar('0')));
        yReg->setText(QString("%1").arg(yReg->text().toInt(nullptr, 16), 8, 2, QChar('0')));
        zReg->setText(QString("%1").arg(zReg->text().toInt(nullptr, 16), 8, 2, QChar('0')));
        mdrInReg->setText(QString("%1").arg(mdrInReg->text().toInt(nullptr, 16), 8, 2, QChar('0')));
        mdrOutReg->setText(QString("%1").arg(mdrOutReg->text().toInt(nullptr, 16), 8, 2, QChar('0')));
        marReg->setText(QString("%1").arg(marReg->text().toInt(nullptr, 16), 8, 2, QChar('0')));
        busButton->setText(QString("%1").arg(busButton->text().toInt(nullptr, 16), 8, 2, QChar('0')));
    }
    else //Hexadecimal
    {
        base = 16;
        fieldWidth = 2;
        hexbin->setText("Binary");
        instructionReg->setText(QString("0x%1").arg(instructionReg->text().toInt(nullptr, 2), 2, 16, QChar('0')));
        progCounter->setText(QString("0x%1").arg(progCounter->text().toInt(nullptr, 2), 2, 16, QChar('0')));
        aReg->setText(QString("0x%1").arg(aReg->text().toInt(nullptr, 2), 2, 16, QChar('0')));
        bReg->setText(QString("0x%1").arg(bReg->text().toInt(nullptr, 2), 2, 16, QChar('0')));
        cReg->setText(QString("0x%1").arg(cReg->text().toInt(nullptr, 2), 2, 16, QChar('0')));
        dReg->setText(QString("0x%1").arg(dReg->text().toInt(nullptr, 2), 2, 16, QChar('0')));
        xReg->setText(QString("0x%1").arg(xReg->text().toInt(nullptr, 2), 2, 16, QChar('0')));
        yReg->setText(QString("0x%1").arg(yReg->text().toInt(nullptr, 2), 2, 16, QChar('0')));
        zReg->setText(QString("0x%1").arg(zReg->text().toInt(nullptr, 2), 2, 16, QChar('0')));
        mdrInReg->setText(QString("0x%1").arg(mdrInReg->text().toInt(nullptr, 2), 2, 16, QChar('0')));
        mdrOutReg->setText(QString("0x%1").arg(mdrOutReg->text().toInt(nullptr, 2), 2, 16, QChar('0')));
        marReg->setText(QString("0x%1").arg(marReg->text().toInt(nullptr, 2), 2, 16, QChar('0')));
        busButton->setText(QString("0x%1").arg(busButton->text().toInt(nullptr, 2), 2, 16, QChar('0')));
    }
    ram->changeBase(binary);
    microcode->changeBase(binary);
}

void CPU::microcodeOpen()
{
    microcode->raise();
    microcode->activateWindow();
    microcode->showNormal();
}

void CPU::ramOpen()
{
    ram->show();
    ram->raise();
    ram->activateWindow();
}

void CPU::multipleSteps()
{
    if (microcode->applied || ram->applied)
    {
        multipleStepsButton->setDisabled(true);
        singleStepButton->setDisabled(true);
    }
    else
    {
        steps = stepCounter->value();
        while (steps-- > 0)
        {
            singleStep();
        }
    }
}


void CPU::singleStep()
{
    if (microcode->applied || ram->applied)
    {
        multipleStepsButton->setDisabled(true);
        singleStepButton->setDisabled(true);
    }
    else
    {
        nextRow = microcode->currentMROM[currentRow][0];

        ///Next Row Condition
        condition = microcode->currentMROM[currentRow][1];
        int nextRowPossibilities = 0;
        int cond = 0;
        switch (condition)
        {
        case 1: // == 0 ?
            cond = zReg->text().toInt(nullptr, base) == 0;
            nextRowPossibilities = 1;
            break;
        case 2: // > 0 ?
            cond = zReg->text().toInt(nullptr, base) > 0;
            nextRowPossibilities = 1;
            break;
        case 3:// < 0 ?
            cond = carryReg->text().toInt();
            nextRowPossibilities = 1;
            break;
        case 4:// >= 0 ?
            cond = ! carryReg->text().toInt();
            nextRowPossibilities = 1;
            break;
        case 5:// <= 0 ?
            cond = carryReg->text().toInt();
            nextRowPossibilities = 1;
            if (!cond)
            {
                cond = zReg->text().toInt(nullptr, base) == 0;
            }
            break;
        case 6:// 4 LSBs of Z
            cond = zReg->text().toInt(nullptr, base);
            nextRowPossibilities = 15;
            cond = cond & 15;
            break;
        case 7:// 4 MSBs of IR
            cond = instructionReg->text().toInt(nullptr, base);
            nextRowPossibilities = 15;
            cond = (cond & 240) >> 4;
            break;
        case 8: //GPIO In 1
            cond = gpioIn1->isChecked();
            nextRowPossibilities = 1;
            break;
        case 9: //GPIO In 2
            cond = gpioIn2->isChecked();
            nextRowPossibilities = 1;
            break;
        }
        microcode->currentRowLabel->setText(QString("Current Row: %1").arg(currentRow));
        QString nextRowText = QString("Next Row: %1").arg(nextRow);

        if (nextRowPossibilities == 1)
        {
            if (!(nextRow & 1)) nextRowText.append(QString(" | %1").arg(nextRow + 1));
        }
        else if (nextRowPossibilities == 15)
        {
            int nextRowLSBs = nextRow & 0b1111;
            int count = 0;

            for (int i = 0; i < (int) sizeof(int) * 8; i++) {
                if (nextRowLSBs & (1 << i))
                    count++;
            }
            switch(count)
            {
            case 0:
                while (nextRowPossibilities > 0)
                {
                    nextRowText.append(QString(" | %1").arg((nextRow + 0b10000) - nextRowPossibilities));
                    nextRowPossibilities--;
                }
                break;
            case 1:
                nextRowPossibilities = 7;
                if (nextRow & 0b1)
                {
                    while (nextRowPossibilities > 0)
                    {
                        nextRowText.append(QString(" | %1").arg((nextRow + 0b10000) - (nextRowPossibilities * 2)));
                    }
                }
                else if (nextRow & 0b10)
                {
                    int othernextRow = nextRow;
                    while (nextRowPossibilities > 0)
                    {
                        othernextRow++;
                        if (othernextRow & 0b10)
                        {
                            nextRowText.append(QString(" | %1").arg(othernextRow));
                            nextRowPossibilities--;
                        }
                    }
                }
                else if (nextRow & 0b100)
                {
                    int othernextRow = nextRow;
                    while (nextRowPossibilities > 0)
                    {
                        othernextRow++;
                        if (othernextRow & 0b100)
                        {
                            nextRowText.append(QString(" | %1").arg(othernextRow));
                            nextRowPossibilities--;
                        }
                    }
                }
                else
                {
                    int othernextRow = nextRow;
                    while (nextRowPossibilities > 0)
                    {
                        othernextRow++;
                        if (othernextRow & 0b1000)
                        {
                            nextRowText.append(QString(" | %1").arg(othernextRow));
                            nextRowPossibilities--;
                        }
                    }
                }
                break;
            case 2:
                nextRowPossibilities = 3;
                if (nextRow & 1)
                {
                    if (nextRow & 0b10)
                    {
                        nextRowText.append(QString(" | %1").arg(nextRow + 0b100));
                        nextRowText.append(QString(" | %1").arg(nextRow + 0b1000));
                        nextRowText.append(QString(" | %1").arg(nextRow + 0b1100));

                    }
                    else if (nextRow & 0b100)
                    {
                        nextRowText.append(QString(" | %1").arg(nextRow + 0b10));
                        nextRowText.append(QString(" | %1").arg(nextRow + 0b1000));
                        nextRowText.append(QString(" | %1").arg(nextRow + 0b1010));
                    }
                    else {
                        nextRowText.append(QString(" | %1").arg(nextRow + 0b10));
                        nextRowText.append(QString(" | %1").arg(nextRow + 0b100));
                        nextRowText.append(QString(" | %1").arg(nextRow + 0b110));

                    }
                }
                else
                {
                    nextRowText.append(QString(" | %1").arg(nextRow + 0b1));
                    if (nextRow & 0b10)
                    {
                        if (nextRow & 0b100)
                        {
                            nextRowText.append(QString(" | %1").arg(nextRow + 0b1000));
                            nextRowText.append(QString(" | %1").arg(nextRow + 0b1001));
                        }
                        else
                        {
                            nextRowText.append(QString(" | %1").arg(nextRow + 0b101));
                            nextRowText.append(QString(" | %1").arg(nextRow + 0b100));

                        }
                    }
                    else
                    {
                        nextRowText.append(QString(" | %1").arg(nextRow + 0b10));
                        nextRowText.append(QString(" | %1").arg(nextRow + 0b11));
                    }

                }
                break;
            case 3:
                nextRowPossibilities = 1;
                if (!(nextRow & 1)) nextRowText.append(QString(" | %1").arg(nextRow + 1));
                else if ((nextRow & 0b10) != 0b10) nextRowText.append(QString(" | %1").arg(nextRow + 0b10));
                else if ((nextRow & 0b100) != 0b100) nextRowText.append(QString(" | %1").arg(nextRow + 0b100));
                else nextRowText.append(QString(" | %1").arg(nextRow + 0b1000));
                break;
            case 4:
                break;
            }
            if (nextRowLSBs == 0b1111)
                if (!(nextRow & 0b1111)) nextRowText.append(QString(" | %1").arg(nextRow + 1));

        }
        microcode->nextRowLabel->setText(nextRowText);

        nextRow = nextRow | cond;

        ///Output Enable
        int outputcounter = 0;
        outputenable[0] = microcode->currentMROM[currentRow][4]; //instruction register
        outputenable[1] = microcode->currentMROM[currentRow][6]; //program counter
        outputenable[2] = microcode->currentMROM[currentRow][8]; //a register
        outputenable[3] = microcode->currentMROM[currentRow][10]; //b register
        outputenable[4] = microcode->currentMROM[currentRow][12]; //c register
        outputenable[5] = microcode->currentMROM[currentRow][14]; //d register
        outputenable[6] = microcode->currentMROM[currentRow][18]; //z register
        outputenable[7] = microcode->currentMROM[currentRow][22]; //mdr in register
        outputenable[8] = microcode->currentMROM[currentRow][24]; //mdr out register
        outputenable[9] = (microcode->currentMROM[currentRow][2] == 2);
        outputenable[10] = microcode->currentMROM[currentRow][25]; //GPIO Out 1
        outputenable[11] = microcode->currentMROM[currentRow][26]; //GPIO Out 2
        outputenable[12] = microcode->currentMROM[currentRow][19]; //Set Carry = 0

        QString outputtext = busButton->text();

        if (outputenable[0]) //Instruction Register
        {
            outputtext = instructionReg->text();
            outputcounter++;
        }
        if (outputenable[1]) //Program Counter
        {
            outputtext = progCounter->text();
            outputcounter++;
        }
        if (outputenable[2]) //A Register
        {
            outputtext = aReg->text();
            outputcounter++;
        }
        if (outputenable[3]) //B Register
        {
            outputtext = bReg->text();
            outputcounter++;
        }
        if (outputenable[4]) //C Register
        {
            outputtext = cReg->text();
            outputcounter++;
        }
        if (outputenable[5]) //D Register
        {
            outputtext = dReg->text();
            outputcounter++;
        }
        if (outputenable[6]) //Z Register
        {
            outputtext = zReg->text();
            outputcounter++;
        }
        if (outputenable[7]) //MDR In Register
        {
            outputtext = mdrInReg->text();
            outputcounter++;
        }
        if (outputenable[12])
        {
            if (binary) outputtext = "00000000";
            else outputtext = "0x00";
            outputcounter++;
        }
        if (outputcounter > 1)
        {
            QString row = QString::number(currentRow);
            QString output = "Multiple registers putting data out on data bus.\nReset initiated. Error in Microcode row: ";
            output.append(row);
            QMessageBox::critical(this, "Conflict on the data bus", output);
            reset();

            return;
        }
        else busButton->setText(outputtext);

        ///Write Enable

        writeenable[0] = microcode->currentMROM[currentRow][3]; //ir
        writeenable[1] = microcode->currentMROM[currentRow][5]; //pc
        writeenable[2] = microcode->currentMROM[currentRow][7]; //a
        writeenable[3] = microcode->currentMROM[currentRow][9]; //b
        writeenable[4] = microcode->currentMROM[currentRow][11]; //c
        writeenable[5] = microcode->currentMROM[currentRow][13]; //d
        writeenable[6] = microcode->currentMROM[currentRow][15]; //x
        writeenable[7] = microcode->currentMROM[currentRow][16]; //y
        writeenable[8] = microcode->currentMROM[currentRow][17]; //z
        writeenable[9] = microcode->currentMROM[currentRow][20]; //mar
        writeenable[10] = microcode->currentMROM[currentRow][21]; //mdrin
        writeenable[11] = microcode->currentMROM[currentRow][23]; //mdrout
        writeenable[12] = microcode->currentMROM[currentRow][19]; //carry reg used by ALU
        writeenable[13] = microcode->currentMROM[currentRow][1] == 8; //GPIO In 1
        writeenable[14] = microcode->currentMROM[currentRow][1] == 9; //GPIO In 2

        if (writeenable[0]) instructionReg->setText(outputtext); //instruction register
        if (writeenable[1]) progCounter->setText(outputtext); //program counter
        if (writeenable[2]) aReg->setText(outputtext); //a register
        if (writeenable[3]) bReg->setText(outputtext); //b register
        if (writeenable[4]) cReg->setText(outputtext); //c register
        if (writeenable[5]) dReg->setText(outputtext); //d register
        if (writeenable[6]) xReg->setText(outputtext); //x register
        if (writeenable[7]) yReg->setText(outputtext); //y register
        if (writeenable[9]) marReg->setText(outputtext); //mar register
        if (writeenable[11]) mdrOutReg->setText(outputtext); //mdr out register


        gpioOut1->setChecked(microcode->currentMROM[currentRow][25]);
        gpioOut2->setChecked(microcode->currentMROM[currentRow][26]);

        ///Operation Code
        if (writeenable[8])
        {
            int opcode = microcode->currentMROM[currentRow][2];
            int result = zReg->text().toInt(nullptr, base);
            switch (opcode)
            {
            case 1: //ADD
                result = xReg->text().toInt(nullptr, base) + yReg->text().toInt(nullptr, base);
                if (result > 255)
                {
                    result = result - 256;
                    carryReg->setText("1");
                }
                break;
            case 2: //ADD with Carry Flag
                result = xReg->text().toInt(nullptr, base) + yReg->text().toInt(nullptr, base) + carryReg->text().toInt(nullptr, base);
                if (result > 255)
                {
                    result = result - 256;
                    carryReg->setText("1");
                }
                break;
            case 3: //SUB
                result = xReg->text().toInt(nullptr, base) - yReg->text().toInt(nullptr, base);
                if (result < 0)
                {
                    result = 256 + result;
                    carryReg->setText("1");
                }
                break;
            case 4: //BITSHIFT X LEFT
                result = xReg->text().toInt(nullptr, base) << 1;
                if (result > 255)
                {
                    result = result - 256;
                    carryReg->setText("1");
                }
                break;
            case 5: //BITSHIFT X RIGHT
                result = xReg->text().toInt(nullptr, base) >> 1;
                if (result > 255)
                {
                    result = result - 256;
                    carryReg->setText("1");
                }
                break;
            case 6: //PASS X
                result = xReg->text().toInt(nullptr, base);
                break;
            case 7: //INCREMENT X
                result = xReg->text().toInt(nullptr, base) + 1;
                if (result > 255)
                {
                    result = result - 256;
                    carryReg->setText("1");
                }
                break;
            case 8: //DECREMENT X
                result = xReg->text().toInt(nullptr, base) - 1;
                if (result < 0)
                {
                    result = 256 + result;
                    carryReg->setText("1");
                }
                break;
            case 9: //X AND Y
                result = xReg->text().toInt(nullptr, base) & yReg->text().toInt(nullptr, base);
                break;
            case 10: //X OR Y
                result = xReg->text().toInt(nullptr, base) | yReg->text().toInt(nullptr, base);
                break;
            case 11: //X XOR Y
                result = xReg->text().toInt(nullptr, base) ^ yReg->text().toInt(nullptr, base);
                break;
            case 12: //NOT X
                result = ~xReg->text().toInt(nullptr, base);
                break;
            }
            if (binary) zReg->setText(QString("%1").arg(result, fieldWidth, base, QChar('0')));
            else zReg->setText(QString("0x%1").arg(result, fieldWidth, base, QChar('0')));
        }
        ///Communication with external RAM
        if (microcode->currentMROM[currentRow][28]) //mem.en
        {
            if (microcode->currentMROM[currentRow][27]) //mem.r
            {

                if (secondstepread)
                {
                    if (writeenable[10]) //mdrin.we
                    {
                        int address = marReg->text().toInt(nullptr, 2);
                        int row = address / 4;
                        int col = address % 4;
                        int val = ram->currentRAM[row][col];
                        if (binary) mdrInReg->setText(QString("%1").arg(val, fieldWidth, base, QChar('0'))); // mdr in register
                        else mdrInReg->setText(QString("0x%1").arg(val, fieldWidth, base, QChar('0')));
                    }
                }
                else secondstepread = true;
            }
            else {
                if (secondstepwrite)
                {
                    if (outputenable[8]) //mdrout.oe
                    {
                        int address = marReg->text().toInt(nullptr, 2);
                        int row = address / 4;
                        int col = address % 4;
                        ram->currentRAM[row][col] = mdrOutReg->text().toInt(nullptr, base);
                        ram->changeValue(row, col, mdrOutReg->text().toInt(nullptr, base));
                    }
                }
                else secondstepwrite = true;
            }
        }

        if (microcode->currentMROM[currentRow][19]) carryReg->setText("0");
        currentRow = nextRow;
        qDebug() << "Next Row: " << nextRow;
        this->update(); //update colors in GUI
    }
}


void CPU::reset()
{
    microcode->apply();
    ram->apply();
    microcode->applied = false;
    ram->applied = false;
    currentInstruction = 0;
    currentRow = 1;
    nextRow = 2;
    if (binary)
    {
        xReg->setText(QString("%1").arg(generator->bounded(255), 8, 2, QChar('0')));
        yReg->setText(QString("%1").arg(generator->bounded(255), 8, 2, QChar('0')));
        zReg->setText(QString("%1").arg(generator->bounded(255), 8, 2, QChar('0')));
        marReg->setText(QString("%1").arg(generator->bounded(255), 8, 2, QChar('0')));
        mdrInReg->setText(QString("%1").arg(generator->bounded(255), 8, 2, QChar('0')));
        mdrOutReg->setText(QString("%1").arg(generator->bounded(255), 8, 2, QChar('0')));
        busButton->setText(QString("%1").arg(generator->bounded(255), 8, 2, QChar('0')));
        instructionReg->setText(QString("%1").arg(generator->bounded(255), 8, 2, QChar('0')));
        aReg->setText(QString("%1").arg(generator->bounded(255), 8, 2, QChar('0')));
        bReg->setText(QString("%1").arg(generator->bounded(255), 8, 2, QChar('0')));
        cReg->setText(QString("%1").arg(generator->bounded(255), 8, 2, QChar('0')));
        dReg->setText(QString("%1").arg(generator->bounded(255), 8, 2, QChar('0')));
        progCounter->setText("00000000");
    }
    else
    {
        xReg->setText(QString("0x%1").arg(generator->bounded(255), 2, 16, QChar('0')));
        yReg->setText(QString("0x%1").arg(generator->bounded(255), 2, 16, QChar('0')));
        zReg->setText(QString("0x%1").arg(generator->bounded(255), 2, 16, QChar('0')));
        marReg->setText(QString("0x%1").arg(generator->bounded(255), 2, 16, QChar('0')));
        mdrInReg->setText(QString("0x%1").arg(generator->bounded(255), 2, 16, QChar('0')));
        mdrOutReg->setText(QString("0x%1").arg(generator->bounded(255), 2, 16, QChar('0')));
        busButton->setText(QString("0x%1").arg(generator->bounded(255), 2, 16, QChar('0')));
        instructionReg->setText(QString("0x%1").arg(generator->bounded(255), 2, 16, QChar('0')));
        aReg->setText(QString("0x%1").arg(generator->bounded(255), 2, 16, QChar('0')));
        bReg->setText(QString("0x%1").arg(generator->bounded(255), 2, 16, QChar('0')));
        cReg->setText(QString("0x%1").arg(generator->bounded(255), 2, 16, QChar('0')));
        dReg->setText(QString("0x%1").arg(generator->bounded(255), 2, 16, QChar('0')));
        progCounter->setText("0x00");

    }
    carryReg->setText("0");
    steps = 0;
    singleStepButton->setEnabled(true);
    multipleStepsButton->setEnabled(true);
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
    QVBoxLayout *xLayout = new QVBoxLayout();
    QVBoxLayout *yLayout = new QVBoxLayout();
    QHBoxLayout *aluInLayout = new QHBoxLayout();
    QVBoxLayout *aluOutLayout = new QVBoxLayout();
    QHBoxLayout *regLayout = new QHBoxLayout();
    QHBoxLayout *labelLayout = new QHBoxLayout();


    zReg = new QPushButton("00000000");
    zReg->setStyleSheet("border: 3px solid black;");
    zReg->setCursor(Qt::WhatsThisCursor);
    zReg->setMinimumSize(100, 23);

    carryReg = new QPushButton("0");
    carryReg->setCursor(Qt::WhatsThisCursor);
    carryReg->setFixedSize(25, 23);
    carryReg->setStyleSheet("border: 3px solid black;");

    regLayout->addWidget(zReg);
    regLayout->addWidget(carryReg);


    zLabel = new QLabel("Z Register");
    zLabel->setAlignment(Qt::AlignCenter);
    zLabel->setMinimumSize(100, 23);

    carryLabel = new QLabel("C");
    carryLabel->setAlignment(Qt::AlignCenter);
    carryLabel->setFixedSize(25, 23);

    labelLayout->addWidget(zLabel);
    labelLayout->addWidget(carryLabel);

    aluOutLayout->addLayout(labelLayout);
    aluOutLayout->addLayout(regLayout);

    xLabel = new QLabel("X Register");
    xLabel->setAlignment(Qt::AlignHCenter|Qt::AlignBottom);
    xLabel->setMinimumSize(125, 23);

    xReg = new QPushButton("00000000");
    xReg->setStyleSheet("border: 3px solid black;");
    xReg->setCursor(Qt::WhatsThisCursor);
    xReg->setMinimumSize(125, 23);

    xLayout->addWidget(xLabel);
    xLayout->addWidget(xReg);

    yLabel = new QLabel("Y Register");
    yLabel->setAlignment(Qt::AlignHCenter|Qt::AlignBottom);
    yLabel->setMinimumSize(125, 23);

    yReg = new QPushButton("00000000");
    yReg->setStyleSheet("border: 3px solid black;");
    yReg->setCursor(Qt::WhatsThisCursor);
    yReg->setMinimumSize(125, 23);

    yLayout->addWidget(yLabel);
    yLayout->addWidget(yReg);

    aluInLayout->addLayout(xLayout);
    aluInLayout->addLayout(yLayout);


    aluButton = new QPushButton("Arithmetic Logic Unit");
    aluButton->setStyleSheet("border: 3px solid black;");
    aluButton->setCursor(Qt::WhatsThisCursor);
    QFont font = aluButton->font();
    font.setPointSize(10);
    font.setBold(true);
    font.setUnderline(true);
    aluButton->setFont(font);
    aluButton->setMinimumSize(200, 23);



    aluLayout->addLayout(aluOutLayout);
    aluLayout->addSpacing(20);
    aluLayout->addWidget(aluButton);
    aluLayout->addLayout(aluInLayout);
    aluLayout->setAlignment(zLabel, Qt::AlignCenter);
    aluLayout->setAlignment(zReg, Qt::AlignCenter);
    aluLayout->setAlignment(aluButton, Qt::AlignCenter);
    aluLayout->addSpacing(100);
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
