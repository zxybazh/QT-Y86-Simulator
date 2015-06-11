#include "simulator.h"
#include "ui_simulator.h"

Simulator::Simulator(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Simulator)
{
    ui->setupUi(this);
}

Simulator::~Simulator()
{
    delete ui;
}
