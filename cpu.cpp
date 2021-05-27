#include "cpu.h"

bool secondstepread = false;
bool secondstepwrite = false;
bool writeenable[9];
bool outputenable[9];
int condition = 0;
int phase = 0;
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
    QGridLayout *gpioLayout = new QGridLayout();


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

    ///Outer Data Bus
    data = new QFrame();
    data->setFrameShape(QFrame::HLine);
    data->setFrameShadow(QFrame::Plain);
    data->setLineWidth(3);

    QVBoxLayout *ramLayout = new QVBoxLayout();
    ramLayout->addWidget(ramButton);
    ramLayout->addStretch();
    ramLayout->addWidget(data);

    ///Inner Data Bus
    bus = new QFrame();
    bus->setFrameShape(QFrame::HLine);
    bus->setFrameShadow(QFrame::Plain);
    bus->setLineWidth(3);

    busLabel = new QLabel("Data Bus");
    busLabel->setAlignment(Qt::AlignCenter);
    busButton = new QPushButton("00000000");
    busButton->setStyleSheet("border: 3px solid black;");
    busButton->setMinimumSize(100, 23);
    busButton->setCursor(Qt::WhatsThisCursor);

    QHBoxLayout *busLayout = new QHBoxLayout();

    busLayout->addStretch();
    busLayout->addWidget(busButton);
    busLayout->addStretch();

    ///Instruction Register
    irLabel = new QLabel("Instruction Register");
    irLabel->setAlignment(Qt::AlignCenter);
    instructionReg = new QPushButton("0000");
    instructionReg->setStyleSheet("border: 3px solid black;");
    instructionReg->setCursor(Qt::WhatsThisCursor);
    instructionReg->setMinimumSize(25, 23);

    instructionReg1 = new QPushButton("0000");
    instructionReg1->setStyleSheet("border: 3px solid black; border-left: 0px; border-right: 1px solid black;");
    instructionReg1->setCursor(Qt::WhatsThisCursor);
    instructionReg1->setMinimumSize(25, 23);

    instructionReg2 = new QPushButton("0000");
    instructionReg2->setStyleSheet("border: 3px solid black; border-left: 0px; border-right: 1px solid black;");
    instructionReg2->setCursor(Qt::WhatsThisCursor);
    instructionReg2->setMinimumSize(25, 23);
    instructionReg3 = new QPushButton("0000");
    instructionReg3->setStyleSheet("border: 3px solid black; border-left: 0px");
    instructionReg3->setCursor(Qt::WhatsThisCursor);
    instructionReg3->setMinimumSize(25, 23);

    QHBoxLayout *instruction = new QHBoxLayout();
    instruction->addWidget(instructionReg);
    instruction->addWidget(instructionReg1);
    instruction->addWidget(instructionReg2);
    instruction->addWidget(instructionReg3);
    instruction->setSpacing(0);

    ///Next Instruction
    playIcon = new QIcon(":/playpause/icons/playIcon");
    pauseIcon = new QIcon(":/playpause/icons/pauseIcon");
    nextInstructionButton = new QPushButton(*playIcon, "Fetch - Execute Cycle", this);
    nextInstructionButton->setCheckable(true);
    nextInstructionButton->setChecked(false);
    nextInstructionButton->setEnabled(false);
    nextInstructionButton->setMinimumSize(100, 23);

    ///Next Step
    nextStepButton = new QPushButton("Fetch Step");
    nextStepButton->setDisabled(true);
    nextStepButton->setMinimumSize(100, 23);

    ///Reset
    resetButton = new QPushButton("Reset");
    resetButton->setMinimumSize(100, 23);

    ///Open Microcode ROM
    openMicrocode = new QPushButton("Open Microcode ROM...");
    openMicrocode->setMinimumSize(100, 23);
    openMicrocode->setCursor(Qt::PointingHandCursor);

    ///Save Microcode ROM
    saveMicrocode = new QPushButton("Save Microcode ROM...");
    saveMicrocode->setMinimumSize(100, 23);
    saveMicrocode->setCursor(Qt::PointingHandCursor);

    ///Program Counter
    pcLabel = new QLabel("Program Counter");
    pcLabel->setAlignment(Qt::AlignCenter);
    progCounter = new QPushButton("00000000");
    progCounter->setStyleSheet("border: 3px solid black;");
    progCounter->setCursor(Qt::WhatsThisCursor);
    progCounter->setMinimumSize(100, 23);


    pcLayout->addWidget(openMicrocode);
    pcLayout->addWidget(saveMicrocode);
    pcLayout->addStretch();
    pcLayout->addWidget(pcLabel);
    pcLayout->addWidget(progCounter);

    ///Open RAM
    openRAM = new QPushButton("Open RAM...");
    openRAM->setMinimumSize(100, 23);
    openRAM->setCursor(Qt::PointingHandCursor);

    ///Save RAM
    saveRAM = new QPushButton("Save RAM...");
    saveRAM->setMinimumSize(100, 23);
    saveRAM->setCursor(Qt::PointingHandCursor);

    ///A Register
    aLabel = new QLabel("A Register");
    aLabel->setAlignment(Qt::AlignCenter);
    aReg = new QPushButton("00000000");
    aReg->setStyleSheet("border: 3px solid black;");
    aReg->setCursor(Qt::WhatsThisCursor);
    aReg->setMinimumSize(100, 23);


    aLayout->addWidget(openRAM);
    aLayout->addWidget(saveRAM);
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
    marReg = new QPushButton("00000000");
    marReg->setCursor(Qt::WhatsThisCursor);
    marReg->setStyleSheet("border: 3px solid black;");
    marReg->setMinimumSize(100, 23);

    mdrInLabel = new QLabel("Memory Data Register In");
    mdrInLabel->setAlignment(Qt::AlignCenter);
    mdrInReg = new QPushButton("00000000");
    mdrInReg->setCursor(Qt::WhatsThisCursor);
    mdrInReg->setStyleSheet("border: 3px solid black;");
    mdrInReg->setMinimumSize(100, 23);

    mdrOutLabel = new QLabel("Memory Data Register Out");
    mdrOutLabel->setAlignment(Qt::AlignCenter);
    mdrOutReg = new QPushButton("00000000");
    mdrOutReg->setCursor(Qt::WhatsThisCursor);
    mdrOutReg->setStyleSheet("border: 3px solid black;");
    mdrOutReg->setMinimumSize(100, 23);

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

    ///GPIOs
    gpioOut1 = new QRadioButton("Output Pin 1");
    gpioOut2 = new QRadioButton("Output Pin 2");
    gpioOut1->setAutoExclusive(false);
    gpioOut2->setAutoExclusive(false);


    gpioIn1 = new QRadioButton("Input Pin 1");
    gpioIn2 = new QRadioButton("Input Pin 2");
    gpioIn1->setAutoExclusive(false);
    gpioIn2->setAutoExclusive(false);

    gpioLayout->addWidget(gpioIn1, 0, 0, Qt::AlignLeft);
    gpioLayout->addWidget(gpioIn2, 1, 0, Qt::AlignLeft);
    gpioLayout->addWidget(gpioOut1, 0, 1, Qt::AlignRight);
    gpioLayout->addWidget(gpioOut2, 1, 1, Qt::AlignRight);
    gpioLayout->setColumnStretch(2, 1);

    irLayout->addWidget(nextInstructionButton);
    irLayout->addWidget(nextStepButton);
    irLayout->addWidget(resetButton);
    irLayout->addStretch();
    irLayout->addWidget(irLabel);
    irLayout->addLayout(instruction);


    QVBoxLayout *romLayout = new QVBoxLayout();
    romLayout->addWidget(romButton);
    romLayout->addStretch();


    ///Put everything together
    buttonsLayout->addLayout(romLayout);
    buttonsLayout->addLayout(irLayout);
    buttonsLayout->addLayout(pcLayout);
    buttonsLayout->addLayout(aLayout);
    buttonsLayout->addLayout(alu);
    buttonsLayout->addLayout(memLayout);

    cpuLayout->addSpacing(5);
    cpuLayout->addLayout(buttonsLayout);
    cpuLayout->addSpacing(30);
    cpuLayout->addWidget(busLabel);
    cpuLayout->addLayout(busLayout);
    cpuLayout->addWidget(bus);

    QHBoxLayout *completeLayout = new QHBoxLayout();
    completeLayout->addLayout(cpuLayout);
    completeLayout->addSpacing(45);
    completeLayout->addLayout(ramLayout);

    QVBoxLayout *fullLayout = new QVBoxLayout();
    fullLayout->addLayout(gpioLayout);
    fullLayout->addLayout(completeLayout);
    widget->setLayout(fullLayout);
    this->setCentralWidget(widget);
    this->resize(1700, 1000);

    QFile file(":/Instruction Fetch.rom");
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    QString text = in.readAll();
    file.close();
    microcode->readRom(&text);

    ///Connections
    connect(romButton, SIGNAL(clicked()), this, SLOT(microcodeOpen()));
    connect(ramButton, SIGNAL(clicked()), this, SLOT(ramOpen()));
    connect(nextInstructionButton, SIGNAL(clicked()), this, SLOT(nextInstruction()));
    connect(nextStepButton, SIGNAL(clicked()), this, SLOT(nextStep()));
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
    QIcon *arrowDownRed = new QIcon(":/arrows/icons/arrowDownRed.png");
    QIcon *arrowLeftRed = new QIcon(":/arrows/icons/arrowLeftRed.png");
    QIcon *arrowUpRed = new QIcon(":/arrows/icons/arrowUpRed.png");
    QIcon *arrowRightRed = new QIcon(":/arrows/icons/arrowRightRed.png");
    QIcon *arrowDown = new QIcon(":/arrows/icons/arrowDown.png");
    QIcon *arrowLeft = new QIcon(":/arrows/icons/arrowLeft.png");
    QIcon *arrowUp = new QIcon(":/arrows/icons/arrowUp.png");
    QIcon *arrowRight = new QIcon(":/arrows/icons/arrowRight.png");
    redPen.setWidth(4);
    blackPen.setWidth(4);
    dashedPen.setWidth(2);
    dashedRedPen.setWidth(2);
    grayPen.setWidth(2);
    painter.setPen(blackPen);

    ///Arrows to the BUS

    //Arrow Instruction Register -> BUS
    if(outputenable[0] || outputenable[1] || outputenable[2])
    {
        painter.setPen(redPen);
        painter.drawPixmap(instructionReg->x() + instructionReg->width() / 4 - 9, bus->y() - 15, arrowDownRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawPixmap(instructionReg->x() + instructionReg->width() / 4 - 9, bus->y() - 15, arrowDown->pixmap(18, 18));
    }
    painter.drawLine(instructionReg->x() + instructionReg->width() / 4, instructionReg->y() + instructionReg->height(), instructionReg->x() + instructionReg->width() / 4, bus->y() - 5);

    //Arrow BUS -> Instruction Register
    if (writeenable[0])
    {
        painter.setPen(redPen);
        painter.drawPixmap(instructionReg->x() + 3 * instructionReg->width() / 4 - 9, instructionReg->y() + instructionReg->height() - 3, arrowUpRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawPixmap(instructionReg->x() + 3 * instructionReg->width() / 4 - 9, instructionReg->y() + instructionReg->height() - 3, arrowUp->pixmap(18, 18));
    }
    painter.drawLine(instructionReg->x() + 3 * instructionReg->width() / 4, bus->y(), instructionReg->x() + 3 * instructionReg->width() / 4, instructionReg->y() + instructionReg->height() + 5);



    //Arrow Program Counter <-> BUS
    if (writeenable[1])
    {
        painter.setPen(redPen);
        painter.drawLine(progCounter->x() + progCounter->width()/2,       bus->y() - 5, progCounter->x() + progCounter->width()/2,       progCounter->y() + progCounter->height() + 5);
        painter.drawPixmap(progCounter->x() + progCounter->width()/2 - 9,       progCounter->y() + progCounter->height() - 3, arrowUpRed->pixmap(18, 18));
        painter.setPen(blackPen);
        painter.drawPixmap(progCounter->x() + progCounter->width()/2 - 9,       bus->y() - 15, arrowDown->pixmap(18, 18));
    }
    else if (outputenable[3])
    {
        painter.setPen(redPen);
        painter.drawLine(progCounter->x() + progCounter->width()/2,       bus->y() - 5, progCounter->x() + progCounter->width()/2,       progCounter->y() + progCounter->height() + 5);
        painter.drawPixmap(progCounter->x() + progCounter->width()/2 - 9,       bus->y() - 15, arrowDownRed->pixmap(18, 18));
        painter.setPen(blackPen);
        painter.drawPixmap(progCounter->x() + progCounter->width()/2 - 9,       progCounter->y() + progCounter->height() - 3, arrowUp->pixmap(18, 18));
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
        painter.drawLine(aReg->x() + aReg->width()/2,                     bus->y() - 5,aReg->x() + aReg->width()/2,                     aReg->y() + aReg->height() + 5);
        painter.drawPixmap(aReg->x() + aReg->width()/2 - 9,               aReg->y() + aReg->height() - 3, arrowUpRed->pixmap(18, 18));
        painter.setPen(blackPen);
        painter.drawPixmap(aReg->x() + aReg->width()/2 - 9,                     bus->y() - 15, arrowDown->pixmap(18, 18));
    }
    else if (outputenable[4])
    {
        painter.setPen(redPen);
        painter.drawLine(aReg->x() + aReg->width()/2,                     bus->y() - 5,aReg->x() + aReg->width()/2,                     aReg->y() + aReg->height() + 5);
        painter.drawPixmap(aReg->x() + aReg->width()/2 - 9,                     bus->y() - 15, arrowDownRed->pixmap(18, 18));
        painter.setPen(blackPen);
        painter.drawPixmap(aReg->x() + aReg->width()/2 - 9,               aReg->y() + aReg->height() - 3, arrowUp->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawLine(aReg->x() + aReg->width()/2,                     bus->y() - 5,aReg->x() + aReg->width()/2,                     aReg->y() + aReg->height() + 5);
        painter.drawPixmap(aReg->x() + aReg->width()/2 - 9,                     bus->y() - 15, arrowDown->pixmap(18, 18));
        painter.drawPixmap(aReg->x() + aReg->width()/2 - 9,               aReg->y() + aReg->height() - 3, arrowUp->pixmap(18, 18));
    }

    //Arrow BUS -> X Register
    if (writeenable[3]){
        painter.setPen(redPen);
        painter.drawLine(xReg->x() + xReg->width()/2,                     bus->y(), xReg->x() + xReg->width()/2,                     xReg->y() + xReg->height() + 5);
        painter.drawPixmap(xReg->x() + xReg->width()/2 - 9,                     xReg->y() + xReg->height() - 1, arrowUpRed->pixmap(18, 18));
    } else
    {
        painter.setPen(blackPen);
        painter.drawLine(xReg->x() + xReg->width()/2,                     bus->y(), xReg->x() + xReg->width()/2,                     xReg->y() + xReg->height() + 5);
        painter.drawPixmap(xReg->x() + xReg->width()/2 - 9,                     xReg->y() + xReg->height() - 1, arrowUp->pixmap(18, 18));
    }

    //Arrow BUS -> Y Register
    if (writeenable[4])
    {
        painter.setPen(redPen);
        painter.drawLine(yReg->x() + yReg->width()/2,                     bus->y(),yReg->x() + yReg->width()/2,                     yReg->y() + yReg->height() + 5);
        painter.drawPixmap(yReg->x() + yReg->width()/2 - 9,                     yReg->y() + yReg->height() - 1, arrowUpRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawLine(yReg->x() + yReg->width()/2,                     bus->y(),yReg->x() + yReg->width()/2,                     yReg->y() + yReg->height() + 5);
        painter.drawPixmap(yReg->x() + yReg->width()/2 - 9,                     yReg->y() + yReg->height() - 1, arrowUp->pixmap(18, 18));
    }
    ///ALU Lines

    if (outputenable[7] & writeenable[5])
    {
        painter.setPen(redPen);
        // Arrow X -> ALU Button Red
        painter.drawLine(xLabel->x() + xLabel->width()/2,                     xLabel->y(),                                      xLabel->x() + xLabel->width()/2,                     aluButton->y() + aluButton->height() + 5);
        painter.drawPixmap(xLabel->x() + xLabel->width()/2 - 9,               aluButton->y() + aluButton->height() - 1,           arrowUpRed->pixmap(18, 18));

        //Arrow Y -> ALU Button Red
        painter.drawLine(yLabel->x() + yLabel->width()/2,                     yLabel->y(),                                      yLabel->x() + yLabel->width()/2,                     aluButton->y() + aluButton->height() + 5);
        painter.drawPixmap( yLabel->x() + yLabel->width()/2 - 9,              aluButton->y() + aluButton->height() - 1,           arrowUpRed->pixmap(18, 18));

        //Arrow ALU Button -> Z Red
        painter.drawLine(aluButton->x() + aluButton->width()/2,           aluButton->y(),                                 zReg->x() + zReg->width()/2,                     zReg->y() + zReg->height() + 5);
        painter.drawPixmap(zReg->x() + zReg->width()/2 - 9,                     zReg->y() + zReg->height() - 1, arrowUpRed->pixmap(18, 18));

    } else
    {
        painter.setPen(blackPen);
        // Arrow X -> ALU Button Black
        painter.drawLine(xLabel->x() + xLabel->width()/2,                     xLabel->y(),                                      xLabel->x() + xLabel->width()/2,                     aluButton->y() + aluButton->height() + 5);
        painter.drawPixmap(xLabel->x() + xLabel->width()/2 - 9,               aluButton->y() + aluButton->height() - 1,           arrowUp->pixmap(18, 18));

        //Arrow Y -> ALU Button Black
        painter.drawLine(yLabel->x() + yLabel->width()/2,                     yLabel->y(),                                      yLabel->x() + yLabel->width()/2,                     aluButton->y() + aluButton->height() + 5);
        painter.drawPixmap( yLabel->x() + yLabel->width()/2 - 9,              aluButton->y() + aluButton->height() - 1,           arrowUp->pixmap(18, 18));

        //Arrow ALU Button -> Z Black
        painter.drawLine(aluButton->x() + aluButton->width()/2,           aluButton->y(),                                 zReg->x() + zReg->width()/2,                     zReg->y() + zReg->height() + 5);
        painter.drawPixmap(zReg->x() + zReg->width()/2 - 9,                     zReg->y() + zReg->height() - 1, arrowUp->pixmap(18, 18));

    }

    //Arrow Z -> BUS
    if (outputenable[5])
    {
        painter.setPen(redPen);
        painter.drawLine(zReg->x() + zReg->width(),                       zReg->y() + zReg->height()/2,                   zReg->x() + zReg->width() + 100,    zReg->y() + zReg->height()/2);
        painter.drawLine(zReg->x() + zReg->width() + 100,    zReg->y() + zReg->height()/2,                   zReg->x() + zReg->width() + 100, bus->y() - 5);
        painter.drawPixmap(zReg->x() + zReg->width() + 100 - 9, bus->y() - 18, arrowDownRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawLine(zReg->x() + zReg->width(),                       zReg->y() + zReg->height()/2,                   zReg->x() + zReg->width() + 100,    zReg->y() + zReg->height()/2);
        painter.drawLine(zReg->x() + zReg->width() + 100,    zReg->y() + zReg->height()/2,                   zReg->x() + zReg->width() + 100, bus->y() - 5);
        painter.drawPixmap(zReg->x() + zReg->width() + 100 - 9, bus->y() - 18, arrowDown->pixmap(18, 18));
    }

    //Arrow Comparisons -> Microcode ROM & Z -> Comparisons
    if (condition > 0 && condition < 4)
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
    painter.drawLine(zReg->x(), zReg->y() + zReg->height()/2, comparisons->x() + comparisons->width()/2, zReg->y() + zReg->height()/2);
    painter.drawLine(comparisons->x() + comparisons->width()/2, zReg->y() + zReg->height()/2, comparisons->x() + comparisons->width()/2, comparisons->y() + comparisons->height() + 5);

    //Arrow IR -> Microcode ROM
    painter.drawLine(instructionReg->x(), instructionReg->y() + instructionReg->height()/2, romButton->x() + 3 *romButton->width()/4,  instructionReg->y() + instructionReg->height()/2);
    painter.drawLine(romButton->x() + 3 *romButton->width()/4, instructionReg->y() + instructionReg->height()/2, romButton->x() + 3 *romButton->width()/4, romButton->y() + romButton->height() + 5);
    painter.drawPixmap(romButton->x() + 3 *romButton->width()/4 - 9, romButton->y() + romButton->height(), arrowUp->pixmap(18, 18));

    ///Memory Lines
    //Arrow MDR IN -> BUS
    if (outputenable[6])
    {
        painter.setPen(redPen);
        painter.drawLine(mdrInReg->x(),                                   mdrInReg->y() + mdrInReg->height()/2,           mdrInReg->x() - 60,             mdrInReg->y() + mdrInReg->height()/2);
        painter.drawLine(mdrInReg->x() - 60,            mdrInReg->y() + mdrInReg->height()/2,           mdrInReg->x() - 60,             bus->y() - 5);
        painter.drawPixmap( mdrInReg->x() - 60 - 9,             bus->y() - 18, arrowDownRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawLine(mdrInReg->x(),                                   mdrInReg->y() + mdrInReg->height()/2,           mdrInReg->x() - 60,             mdrInReg->y() + mdrInReg->height()/2);
        painter.drawLine(mdrInReg->x() - 60,            mdrInReg->y() + mdrInReg->height()/2,           mdrInReg->x() - 60,             bus->y() - 5);
        painter.drawPixmap( mdrInReg->x() - 60 - 9,             bus->y() - 18, arrowDown->pixmap(18, 18));
    }

    //Arrow BUS -> MDR OUT
    if (writeenable[8])
    {
        painter.setPen(redPen);
        painter.drawLine(mdrOutReg->x() - 20,          bus->y(), mdrOutReg->x() - 20,          mdrOutReg->y() + mdrOutReg->height()/2);
        painter.drawLine(mdrOutReg->x() - 20,          mdrOutReg->y() + mdrOutReg->height()/2, mdrOutReg->x() + 5,                                  mdrOutReg->y() + mdrOutReg->height()/2);
        painter.drawPixmap(mdrOutReg->x() - 18,                          mdrOutReg->y() + mdrOutReg->height()/2 - 9, arrowRightRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawLine(mdrOutReg->x() - 20,          bus->y(), mdrOutReg->x() - 20,          mdrOutReg->y() + mdrOutReg->height()/2);
        painter.drawLine(mdrOutReg->x() - 20,          mdrOutReg->y() + mdrOutReg->height()/2, mdrOutReg->x() - 5,                                  mdrOutReg->y() + mdrOutReg->height()/2);
        painter.drawPixmap(mdrOutReg->x() - 18,                          mdrOutReg->y() + mdrOutReg->height()/2 - 9, arrowRight->pixmap(18, 18));
    }

    //Arrow BUS -> MAR
    if (writeenable[6])
    {
        painter.setPen(redPen);
        painter.drawPixmap(marReg->x() - 18,                                     marReg->y() + marReg->height()/2 - 9, arrowRightRed->pixmap(18, 18));
    }
    else
    {
        painter.setPen(blackPen);
        painter.drawPixmap(marReg->x() - 18,                                     marReg->y() + marReg->height()/2 - 9, arrowRight->pixmap(18, 18));
    }
    painter.drawLine(marReg->x() - 100,                 bus->y(), marReg->x() - 100,                 marReg->y() + marReg->height()/2);
    painter.drawLine(marReg->x() - 100,                 marReg->y() + marReg->height()/2, marReg->x() - 5,                                     marReg->y() + marReg->height()/2);

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
        painter.drawLine(mdrInReg->x() + mdrInReg->width() + 40,  mdrInReg->y() + mdrInReg->height()/2, mdrInReg->x() + mdrInReg->width() + 5, mdrInReg->y() + mdrInReg->height()/2);
        painter.drawPixmap(mdrInReg->x() + mdrInReg->width(), mdrInReg->y() + mdrInReg->height()/2 - 9, arrowLeftRed->pixmap(18, 18));
    }
    else {
        painter.setPen(dashedPen);
        painter.drawLine(data->x() + 20, data->y(), data->x() + 20, ramButton->y() + ramButton->height() + 10);
        painter.drawLine( data->x() + 20, ramButton->y() + ramButton->height() + 10, mdrInReg->x() + mdrInReg->width() + 40,  ramButton->y() + ramButton->height() + 10);
        painter.drawLine(mdrInReg->x() + mdrInReg->width() + 40,  ramButton->y() + ramButton->height() + 10, mdrInReg->x() + mdrInReg->width() + 40,  mdrInReg->y() + mdrInReg->height()/2);
        painter.drawLine(mdrInReg->x() + mdrInReg->width() + 40,  mdrInReg->y() + mdrInReg->height()/2, mdrInReg->x() + mdrInReg->width() + 5, mdrInReg->y() + mdrInReg->height()/2);
        painter.drawPixmap(mdrInReg->x() + mdrInReg->width(), mdrInReg->y() + mdrInReg->height()/2 - 9, arrowLeft->pixmap(18, 18));
    }

    //Arrow MDR Out -> Outer Data Bus
    if (outputenable[8])
    {
        painter.setPen(dashedRedPen);
        painter.drawLine(mdrOutReg->x() + mdrOutReg->width(), mdrOutReg->y() + mdrOutReg->height()/2, data->x() + 10, mdrOutReg->y() + mdrOutReg->height()/2);
        painter.drawLine( data->x() + 10, mdrOutReg->y() + mdrOutReg->height()/2,  data->x() + 10, data->y() - 5);
        painter.drawPixmap( data->x() + 10 - 9, data->y() - 18, arrowDownRed->pixmap(18, 18));
    } else
    {
        painter.setPen(dashedPen);
        painter.drawLine(mdrOutReg->x() + mdrOutReg->width(), mdrOutReg->y() + mdrOutReg->height()/2, data->x() + 10, mdrOutReg->y() + mdrOutReg->height()/2);
        painter.drawLine( data->x() + 10, mdrOutReg->y() + mdrOutReg->height()/2,  data->x() + 10, data->y() - 5);
        painter.drawPixmap( data->x() + 10 - 9, data->y() - 18, arrowDown->pixmap(18, 18));
    }

    //Arrow RAM <-> Outer Data Bus
    if (outputenable[8])
    {
        painter.setPen(dashedRedPen);
        painter.drawPixmap(ramButton->x() + ramButton->width()/2 - 9, data->y() - 18, arrowDown->pixmap(18, 18));
        painter.drawPixmap(ramButton->x() + ramButton->width()/2 - 9, ramButton->y() + ramButton->height(), arrowUpRed->pixmap(18, 18));

    }
    else if (writeenable[7])
    {
        painter.setPen(dashedRedPen);
        painter.drawPixmap(ramButton->x() + ramButton->width()/2 - 9, data->y() - 18, arrowDownRed->pixmap(18, 18));
        painter.drawPixmap(ramButton->x() + ramButton->width()/2 - 9, ramButton->y() + ramButton->height(), arrowUp->pixmap(18, 18));
    }
    else
    {
        painter.setPen(dashedPen);
        painter.drawPixmap(ramButton->x() + ramButton->width()/2 - 9, data->y() - 18, arrowDown->pixmap(18, 18));
        painter.drawPixmap(ramButton->x() + ramButton->width()/2 - 9, ramButton->y() + ramButton->height(), arrowUp->pixmap(18, 18));
    }
    painter.drawLine(ramButton->x() + ramButton->width()/2, ramButton->y() + ramButton->height() + 5, ramButton->x() + ramButton->width()/2, data->y() - 5);

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

void CPU::gpioClick()
{
    QRadioButton *clickedButton = qobject_cast<QRadioButton *>(sender());
    clickedButton->setChecked(!clickedButton->isChecked());
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
    switch (phase)
    {
    case 0: //Fetch
        while (phase == 0) nextStep();
        nextInstruction();
        break;
    case 1: //Execute
        while (phase == 1) nextStep();
        break;
    }

 nextStep();
}

void CPU::nextStep()
{
    int nextRow = microcode->currentMROM[currentRow][0];
    switch (phase) {
    case 0: //Fetch
        if (nextRow == 0)
        {
            nextStepButton->setText("Execute Step");
            phase++;
        }
        if (currentRow == 5)
        {
            bool vFound = false;
            uint i = 0;
            while (i < ram->currentInstructions.size() && !vFound)
            {
                if (instructionReg->text() == ram->currentInstructions.at(i).at(0))
                {
                    currentRow = ram->currentInstructions.at(i).at(2).toInt();
                    vFound = true;
                }
                i++;
            }
        }
        break;
    case 1: //Execute
        if (nextRow == 0)
        {
            nextStepButton->setText("Fetch Step");
            phase--;
        }

        ///Next Row Condition
        bool values[microcode->currentMROM[currentRow].size() - 2];
        for (int column = 0; column < (int)microcode->currentMROM[currentRow].size() - 2; column++)
        {
            values[column] = microcode->currentMROM[currentRow][column];
        }
        bool lsb = nextRow & 1;
        condition = microcode->currentMROM[currentRow][1];
        bool cond = 0;
        switch (condition)
        {
        case 1: // == 0 ?
            cond = zReg->text().toInt(nullptr, 2) == 0;
            break;
        case 2: // > 0 ?
            cond = zReg->text().toInt(nullptr, 2) > 0;
            break;
        case 3:// < 0 ?
            cond = zReg->text().toInt(nullptr, 2) < 0;
            break;
        case 4: //GPIO In 1
            cond = gpioIn1->isChecked();
            break;
        case 5: //GPIO In 2
            cond = gpioIn2->isChecked();
            break;
        }
        if (!lsb && cond) nextRow++;
        break;
    }

    ///Operation Code
    if (microcode->currentMROM[currentRow][13])
    {
        int opcode = microcode->currentMROM[currentRow][2];
        switch (opcode)
        {
        case 1: //ADD
            zReg->setText(QString("%1").arg(xReg->text().toInt(nullptr, 2) + yReg->text().toInt(nullptr, 2), 8, 2, QChar('0')));
            break;
        case 2: //SUB
            zReg->setText(QString("%1").arg(xReg->text().toInt(nullptr, 2) - yReg->text().toInt(nullptr, 2), 8, 2, QChar('0')));
            break;
        case 3: //BITSHIFT X LEFT
            zReg->setText(QString("%1").arg(xReg->text().toInt(nullptr, 2) << 1, 8, 2, QChar('0')));
            break;
        case 4: //BITSHIFT X RIGHT
            zReg->setText(QString("%1").arg(xReg->text().toInt(nullptr, 2) >> 1, 8, 2, QChar('0')));
            break;
        case 5: //PASS X
            zReg->setText(xReg->text());
            break;
        case 6: //INCREMENT X
            zReg->setText(QString("%1").arg(xReg->text().toInt(nullptr, 2) + 1, 8, 2, QChar('0')));
            break;
        case 7: //DECREMENT X
            zReg->setText(QString("%1").arg(xReg->text().toInt(nullptr, 2) - 1, 8, 2, QChar('0')));
            break;
        }
    }

    ///Output Enable
    int outputcounter = 0;
    outputenable[0] = microcode->currentMROM[currentRow][4]; //instruction register 1
    outputenable[1] = microcode->currentMROM[currentRow][5]; //instruction register 2
    outputenable[2] = microcode->currentMROM[currentRow][6]; //instruction register 3
    outputenable[3] = microcode->currentMROM[currentRow][8]; //program counter
    outputenable[4] = microcode->currentMROM[currentRow][10]; //a register
    outputenable[5] = microcode->currentMROM[currentRow][14]; //z register
    outputenable[6] = microcode->currentMROM[currentRow][17]; //mdr in register
    outputenable[7] = microcode->currentMROM[currentRow][2]; //opcode
    outputenable[8] = microcode->currentMROM[currentRow][19]; //mdr out register
    QString outputtext = busButton->text();
    if (microcode->currentMROM[currentRow][4])
    {
        outputtext = instructionReg1->text();
        if (microcode->currentMROM[currentRow][5]) outputtext.append(instructionReg2->text());
        if (microcode->currentMROM[currentRow][6]) outputtext.append(instructionReg3->text());
        outputcounter++;
    }
    else if (microcode->currentMROM[currentRow][5])
    {
        outputtext = instructionReg2->text();
        if (microcode->currentMROM[currentRow][6]) outputtext.append(instructionReg3->text());
        outputcounter++;
    }
    else if (microcode->currentMROM[currentRow][6])
    {
        outputtext = instructionReg3->text();
        outputcounter++;
    }
    if (microcode->currentMROM[currentRow][8])
    {
        outputtext = progCounter->text();
        outputcounter++;
    }
    if (microcode->currentMROM[currentRow][10])
    {
        outputtext = aReg->text();
        outputcounter++;
    }
    if (microcode->currentMROM[currentRow][14]){
        outputtext = zReg->text();
        outputcounter++;
    }
    if (microcode->currentMROM[currentRow][17])
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
    writeenable[1] = microcode->currentMROM[currentRow][7]; //pc
    writeenable[2] = microcode->currentMROM[currentRow][9]; //a
    writeenable[3] = microcode->currentMROM[currentRow][11]; //x
    writeenable[4] = microcode->currentMROM[currentRow][12]; //y
    writeenable[5] = microcode->currentMROM[currentRow][13]; //z
    writeenable[6] = microcode->currentMROM[currentRow][15]; //mar
    writeenable[7] = microcode->currentMROM[currentRow][16]; //mdrin
    writeenable[8] = microcode->currentMROM[currentRow][18]; //mdrout

    if (microcode->currentMROM[currentRow][3])
    {
        instructionReg->setText(outputtext); //instruction register
    }

    if (microcode->currentMROM[currentRow][7]) progCounter->setText(outputtext); //program counter
    if (microcode->currentMROM[currentRow][9]) aReg->setText(outputtext); // a register
    if (microcode->currentMROM[currentRow][11]) xReg->setText(outputtext); // x register
    if (microcode->currentMROM[currentRow][12]) yReg->setText(outputtext); // y register
    if (microcode->currentMROM[currentRow][15]) marReg->setText(outputtext); // mar register
    if (microcode->currentMROM[currentRow][18]) mdrOutReg->setText(outputtext); // mdr out register
    gpioOut1->setChecked(microcode->currentMROM[currentRow][20]);
    gpioOut2->setChecked(microcode->currentMROM[currentRow][21]);

    ///Communication with external RAM
    if (microcode->currentMROM[currentRow][23]) //mem.en
    {
        if (microcode->currentMROM[currentRow][22]) //mem.r
        {
            if (secondstepread)
            {
                if (microcode->currentMROM[currentRow][16]) //mdrin.we
                {
                    int address = marReg->text().toInt(nullptr, 2);
                    int row = address / 4;
                    int col = address % 4;

                    QString val = QString("%1").arg(ram->currentRAM[row][col], 4, 2, QChar('0'));
                    if (col == 0)
                    {
                        val.append(QString("%1").arg(ram->currentRAM[row][col + 1], 4, 2, QChar('0')));
                        val.append(QString("%1").arg(ram->currentRAM[row][col + 2], 4, 2, QChar('0')));
                        val.append(QString("%1").arg(ram->currentRAM[row][col + 3], 4, 2, QChar('0')));

                    }
                    else if (col == 1)
                    {
                        val.append(QString("%1").arg(ram->currentRAM[row][col + 1], 4, 2, QChar('0')));
                        val.append(QString("%1").arg(ram->currentRAM[row][col + 2], 4, 2, QChar('0')));
                        val.append(QString("%1").arg(ram->currentRAM[row + 1][0], 4, 2, QChar('0')));

                    }
                    else if (col == 2)
                    {
                        val.append(QString("%1").arg(ram->currentRAM[row][col + 1], 4, 2, QChar('0')));
                        val.append(QString("%1").arg(ram->currentRAM[row + 1][0], 4, 2, QChar('0')));
                        val.append(QString("%1").arg(ram->currentRAM[row + 1][1], 4, 2, QChar('0')));
                    }
                    else
                    {
                        val.append(QString("%1").arg(ram->currentRAM[row + 1][0], 4, 2, QChar('0')));
                        val.append(QString("%1").arg(ram->currentRAM[row + 1][1], 4, 2, QChar('0')));
                        val.append(QString("%1").arg(ram->currentRAM[row + 1][2], 4, 2, QChar('0')));
                    }
                    mdrInReg->setText(val); // mdr in register
                }
            }
            else secondstepread = true;
        }
        else {
            if (secondstepwrite)
            {
                if (microcode->currentMROM[currentRow][20]) //mdrout.oe
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
    currentRow = 1;
    xReg->setText(QString("%1").arg(generator->bounded(256), 8, 2, QChar('0')));
    yReg->setText(QString("%1").arg(generator->bounded(256), 8, 2, QChar('0')));
    zReg->setText(QString("%1").arg(generator->bounded(256), 8, 2, QChar('0')));
    marReg->setText(QString("%1").arg(generator->bounded(256), 8, 2, QChar('0')));
    mdrInReg->setText(QString("%1").arg(generator->bounded(256), 8, 2, QChar('0')));
    mdrOutReg->setText(QString("%1").arg(generator->bounded(256), 8, 2, QChar('0')));
    busButton->setText(QString("%1").arg(generator->bounded(256), 8, 2, QChar('0')));
    instructionReg->setText(QString("%1").arg(generator->bounded(256), 8, 2, QChar('0')));
    aReg->setText(QString("%1").arg(generator->bounded(256), 8, 2, QChar('0')));
    progCounter->setText("00000000");
    nextStepButton->setEnabled(true);
    nextInstructionButton->setEnabled(true);

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

    xReg = new QPushButton("00000000");
    xReg->setStyleSheet("border: 3px solid black;");
    xReg->setCursor(Qt::WhatsThisCursor);
    xReg->setMinimumSize(100, 23);

    yReg = new QPushButton("00000000");
    yReg->setStyleSheet("border: 3px solid black;");
    yReg->setCursor(Qt::WhatsThisCursor);
    yReg->setMinimumSize(100, 23);


    aluInLayout->addStretch();
    aluInLayout->addWidget(xReg);
    aluInLayout->addWidget(yReg);
    aluInLayout->addStretch();

    zReg = new QPushButton("00000000");
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
