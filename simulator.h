#ifndef SIMULATOR_H
#define SIMULATOR_H
#include "state.h"
#include <QMainWindow>
#include <QImage>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QMainWindow>

namespace Ui {
class Simulator;
}

class Simulator : public QMainWindow {
    Q_OBJECT

public:
    explicit Simulator(QWidget *parent = 0);
    ~Simulator();
	void onTriggeredOpen();
	void setExcecuteSpeed();
	void init(Simulator *);
	void setConditionCode(int, int, int);
	void flush_Registers();
	void flush_Memory();
	void flush_all();
	void setRun();
	void setPause();
	void setReset();
	void setNext();
	void setPrevious();
	void WriteToFile();
	void GetTotalCycle();
	Ui::Simulator *ui;
	QString getHex(int);
private:
	QString output;
	QString getState(int);
	QImage *image;
	//All together statistic
	QFile *current_file;
	int Current_cycle, Execute_speed;
	int total_cycle;
	bool Continue_excecute;
	void Parse();
	void tick();
	State s;
	//To add(init, count, etc.)
	int Cache_hit, Cache_miss;
};

#endif // SIMULATOR_H
