#include "simulator.h"
#include "ui_simulator.h"
#include <QTextStream>
#include <QElapsedTimer>
#include <map>

void Simulator::Parse() {

	QStringList lines;
	if (current_file->open(QIODevice::ReadOnly|QIODevice::Text)) {
		QTextStream stream( current_file );
		QString line;
		bool flag;
		int tmp, k;
		while ( !stream.atEnd() ) {
			line = stream.readLine(); // 不包括“\n”的一行文本
			lines = line.split('|');
			if (lines[0].contains(":")) {
				lines = lines[0].split(':');
				flag = false;
				tmp = 0;
				for (int i = 0; i < lines[0].size(); i++) {
					if (flag)
						if (lines[0].toStdString()[i] >='0' && lines[0].toStdString()[i] <='9')
							tmp = tmp * 16 + lines[0].toStdString()[i] - '0';
						else
							tmp = tmp * 16 + lines[0].toStdString()[i] - 'a' + 10;
					else {};
					if (lines[0][i] == 'x') flag = true;
				}
				if (lines.size() < 2) continue;
				line = lines[1];
				flag = false; k = 0;
				for (int i = 0; i < line.size(); i++) {
					if (line.toStdString()[i] >= '0' && line.toStdString()[i] <= '9') {
						k = k * 16 + line.toStdString()[i] - '0';
					} else if (line.toStdString()[i] >= 'a' && line.toStdString()[i] <= 'f') {
						k = k * 16 + line.toStdString()[i] - 'a' + 10;
					}
					if (flag) {
						s.mem[tmp++] = k;
						k = 0;
					}
					if ((line.toStdString()[i] >= '0' && line.toStdString()[i] <= '9') || (line.toStdString()[i] >= 'a' && line.toStdString()[i] <= 'f')) {
						flag ^=1;
					}
				}
			}
		}
		current_file->close();
	}
}

void Simulator::GetTotalCycle() {
	total_cycle = 0;
	while (!s.EndOfProgram()) {
		tick();
		++total_cycle;
		if (total_cycle >= 10000) break;
	}
}

QString Simulator::getHex(int x) {
	QString ans = "0x";
	int tmp;
	for (int i = 7; i >= 0; i--) {
		tmp = x >>(i*4) & (0xf);
		if (tmp <= 9) ans = ans + (tmp + '0');
		else ans = ans + (tmp - 10 + 'a');
	}
	return ans;
}
QString Simulator::getHexNumber(int x) {
	if (x >= 0 && x <= 9) return QVariant(x).toString();
	if (x >= 10 && x <= 15) return QString(x-10+'a');
	return "*";
}

void Simulator::flush_Registers() {
	ui->text_eax->setText(getHex(s.eax));
	ui->text_ecx->setText(getHex(s.ecx));
	ui->text_edx->setText(getHex(s.edx));
	ui->text_ebx->setText(getHex(s.ebx));
	ui->text_esp->setText(getHex(s.esp));
	ui->text_ebp->setText(getHex(s.ebp));
	ui->text_esi->setText(getHex(s.esi));
	ui->text_edi->setText(getHex(s.edi));
	ui->ZF->setText(s.ZF?"1":"0");
	ui->SF->setText(s.SF?"1":"0");
	ui->OF->setText(s.OF?"1":"0");
}

void Simulator::flush_Memory() {
	std::string ss[4];
	unsigned int tmp = 0;
	int la = -1, k = 0;

	for (std::map<int, int>::iterator it = s.mem.begin(); it != s.mem.end(); ++it) {
		if (it->first <= la) continue;
		tmp = 0;
		for (int i = (it->first >> 2) << 2, j = 0; j < 4; j++) {
			tmp <<= 8;
			if (s.mem.find(i+j) != s.mem.end())
				tmp += s.mem[i+j];
			la = i+j;
		}
		if (tmp == 0) continue;
		ss[k % 4] += getHex((it->first >> 2) << 2).toStdString() + ":\n"+ getHex(tmp).toStdString()+"\n\n";
		//ss[k % 4] += QVariant(tmp).toString().toStdString()+"\n";
		++k;
	}

	ui->memory1->setText(ss[0].c_str());
	ui->memory2->setText(ss[1].c_str());
	ui->memory3->setText(ss[2].c_str());
	ui->memory4->setText(ss[3].c_str());
}

QString Simulator::getState(int stat) {
	if (stat == s.SAOK) return "SAOK";
	if (stat == s.SADR) return "SADR";
	if (stat == s.SINS) return "SINS";
	if (stat == s.SHLT) return "SHLT";
	return "SBUB";
}

void Simulator::flush_all() {
	if (s.F_stall)
		ui->F_stall->setStyleSheet("QTextBrowser { background-color : yellow; color : black; }");
	else
		ui->F_stall->setStyleSheet("QTextBrowser { background-color : white; color : black; }");
	if (s.F_bubble)
		ui->F_bubble->setStyleSheet("QTextBrowser { background-color : yellow; color : black; }");
	else
		ui->F_bubble->setStyleSheet("QTextBrowser { background-color : white; color : black; }");
	if (s.D_stall)
		ui->D_stall->setStyleSheet("QTextBrowser { background-color : yellow; color : black; }");
	else
		ui->D_stall->setStyleSheet("QTextBrowser { background-color : white; color : black; }");
	if (s.D_bubble)
		ui->D_bubble->setStyleSheet("QTextBrowser { background-color : yellow; color : black; }");
	else
		ui->D_bubble->setStyleSheet("QTextBrowser { background-color : white; color : black; }");
	if (s.E_stall)
		ui->E_stall->setStyleSheet("QTextBrowser { background-color : yellow; color : black; }");
	else
		ui->E_stall->setStyleSheet("QTextBrowser { background-color : white; color : black; }");
	if (s.E_bubble)
		ui->E_bubble->setStyleSheet("QTextBrowser { background-color : yellow; color : black; }");
	else
		ui->E_bubble->setStyleSheet("QTextBrowser { background-color : white; color : black; }");
	if (s.M_stall)
		ui->M_stall->setStyleSheet("QTextBrowser { background-color : yellow; color : black; }");
	else
		ui->M_stall->setStyleSheet("QTextBrowser { background-color : white; color : black; }");
	if (s.M_bubble)
		ui->M_bubble->setStyleSheet("QTextBrowser { background-color : yellow; color : black; }");
	else
		ui->M_bubble->setStyleSheet("QTextBrowser { background-color : white; color : black; }");
	if (s.W_stall)
		ui->W_stall->setStyleSheet("QTextBrowser { background-color : yellow; color : black; }");
	else
		ui->W_stall->setStyleSheet("QTextBrowser { background-color : white; color : black; }");
	if (s.W_bubble)
		ui->W_bubble->setStyleSheet("QTextBrowser { background-color : yellow; color : black; }");
	else
		ui->W_bubble->setStyleSheet("QTextBrowser { background-color : white; color : black; }");

	ui->Stat->setText(getState(s.Stat));
	ui->cycle_num->setText(QVariant(Current_cycle).toString());
	ui->F_stat->setText(getState(s.f_stat));
	ui->F_prePC->setText(getHex(s.F_predPC));
	ui->PC->setText(getHex(s.f_pc));
	ui->F_ifun->setText(QVariant(s.f_ifun).toString());
	ui->F_icode->setText(getHexNumber(s.f_icode));

	ui->D_stat->setText(getState(s.D_stat));
	ui->D_icode->setText(getHexNumber(s.D_icode));
	ui->D_ifun->setText(QVariant(s.D_ifun).toString());
	ui->D_rA->setText(getHex(s.D_rA));
	ui->D_rB->setText(getHex(s.D_rB));
	ui->D_valC->setText(getHex(s.D_valC));
	ui->D_valP->setText(getHex(s.D_valP));

	ui->E_stat->setText(getState(s.E_stat));
	ui->E_icode->setText(getHexNumber(s.E_icode));
	ui->E_ifun->setText(QVariant(s.E_ifun).toString());
	ui->E_valC->setText(getHex(s.E_valC));
	ui->E_valA->setText(getHex(s.E_valA));
	ui->E_valB->setText(getHex(s.E_valB));
	ui->E_srcA->setText(getHex(s.E_srcA));
	ui->E_srcB->setText(getHex(s.E_srcB));
	ui->E_dstE->setText(getHex(s.E_dstE));
	ui->E_dstM->setText(getHex(s.E_dstM));

	ui->M_stat->setText(getState(s.M_stat));
	ui->M_icode->setText(getHexNumber(s.M_icode));
	ui->M_cnd->setText((s.M_Cnd?"true":"false"));
	ui->M_valE->setText(getHex(s.M_valE));
	ui->M_valA->setText(getHex(s.M_valA));
	ui->M_dstE->setText(getHex(s.M_dstE));
	ui->M_dstM->setText(getHex(s.M_dstM));

	ui->W_stat->setText(getState(s.W_stat));
	ui->W_icode->setText(getHexNumber(s.W_icode));
	ui->W_valE->setText(getHex(s.W_valE));
	ui->W_valM->setText(getHex(s.W_valM));
	ui->W_dstE->setText(getHex(s.W_dstE));
	ui->W_dstM->setText(getHex(s.W_dstM));

	ui->PC->setText(getHex(s.PC));

	ui->execute_progress->setValue(Current_cycle/(double)total_cycle*100);
}

void Simulator::tick() {
	s.Register_Control();
	s.Write();
	s.Memory();
	s.Execute();
	s.Decode();
	s.Fetch();

	output += "Cycle_"+QVariant(Current_cycle).toString()+"\n--------------------\n";
	output += "FETCH:\n";
	output += "	F_predPC	= "+getHex(s.f_pc)+"\n";
	output += "\n";

	output += "DECODE:\n";
	output += "	D_icode		= "+getHex(s.D_icode)+"\n";
	output += "	D_ifun		= "+getHex(s.D_ifun)+"\n";
	output += "	D_rA		= "+getHex(s.D_rA)+"\n";
	output += "	D_rB		= "+getHex(s.D_rB)+"\n";
	output += "	D_valC		= "+getHex(s.D_valC)+"\n";
	output += "	D_valP		= "+getHex(s.D_valP)+"\n";
	output += "\n";

	output += "EXECUTE:\n";
	output += "	E_icode		= "+getHex(s.E_icode)+"\n";
	output += "	E_ifun		= "+getHex(s.E_ifun)+"\n";
	output += "	E_valC		= "+getHex(s.E_valC)+"\n";
	output += "	E_valA		= "+getHex(s.E_valA)+"\n";
	output += "	E_valB		= "+getHex(s.E_valB)+"\n";
	output += "	E_dstE		= "+getHex(s.E_dstE)+"\n";
	output += "	E_dstM		= "+getHex(s.E_dstM)+"\n";
	output += "	E_srcA		= "+getHex(s.E_srcA)+"\n";
	output += "	E_srcB		= "+getHex(s.E_srcB)+"\n";
	output += "\n";

	output += "MEMORY:\n";
	output += "	M_icode		= "+getHex(s.M_icode)+"\n";
	output += "	M_Bch		= "+(s.M_Cnd?(QString)"true":(QString)"false")+"\n";
	output += "	M_valE		= "+getHex(s.M_valE)+"\n";
	output += "	M_valA		= "+getHex(s.M_valA)+"\n";
	output += "	M_dstE		= "+getHex(s.M_dstE)+"\n";
	output += "	M_dstM		= "+getHex(s.M_dstM)+"\n";
	output += "\n";

	output += "WRITE BACK:\n";
	output += "	W_icode		= "+getHex(s.W_icode)+"\n";
	output += "	W_valE		= "+getHex(s.W_valE)+"\n";
	output += "	W_valM		= "+getHex(s.W_valM)+"\n";
	output += "	W_dstE		= "+getHex(s.W_dstE)+"\n";
	output += "	W_dstM		= "+getHex(s.W_dstM)+"\n";
	output += "\n";

	Current_cycle += 1;
}

void Simulator::init(Simulator* cur) {
	output = "";
	Current_cycle = 0;
	Continue_excecute = false;
	s.init();
	s.Father = cur;
	Parse();
	GetTotalCycle();

	output = "";
	Current_cycle = 0;
	Continue_excecute = false;
	s.init();
	s.Father = cur;
	Parse();

	flush_Registers();
	flush_Memory();
	flush_all();
}

Simulator::Simulator(QWidget *parent) : QMainWindow(parent),
ui(new Ui::Simulator) {
	ui->setupUi(this);

	this->image = new QImage();

	QString fileName = ":image/y86.png";

	if(image->load(fileName)) {
		QGraphicsScene *scene = new QGraphicsScene;
		scene->addPixmap(QPixmap::fromImage(*image));
		ui->graphicsView->setScene(scene);
		ui->graphicsView->resize(611, 561);
		ui->graphicsView->show();
	}

	//Y86 Structure map drawn

	this->Execute_speed = 50;
	this->current_file = new QFile(":asum.yo");
	//Init

	connect(ui->execute_speed, &QSlider::valueChanged, this, &Simulator::setExcecuteSpeed);
	//Change Execution speed

	connect(ui->newfile, &QAction::triggered, this, &Simulator::onTriggeredOpen);
	//OpenFile Trigger

	connect(ui->Next, &QPushButton::clicked, this, &Simulator::setNext);
	connect(ui->Run, &QPushButton::clicked, this, &Simulator::setRun);
	connect(ui->Pause, &QPushButton::clicked, this, &Simulator::setPause);
	connect(ui->Reset, &QPushButton::clicked, this, &Simulator::setReset);
	connect(ui->Previous, &QPushButton::clicked, this, &Simulator::setPrevious);
}

void Simulator::WriteToFile() {
	FILE* file = fopen("output.txt", "w");
	fprintf(file, "%s", output.toStdString().c_str());
	fclose(file);
	/*
	QMessageBox msg;
	msg.setText(output);
	msg.exec();
	*/
	//output = "";
}

void Simulator::setNext() {
	if (s.EndOfProgram()) {
		WriteToFile();
		QMessageBox Msgbox;
		Msgbox.setText("Program terminated.");
		Msgbox.exec();
		return;
	}
	tick();
	//WriteToFile();

	flush_Memory();
	flush_Registers();
	flush_all();
}

void Simulator::setRun() {
	Continue_excecute = true;
	while (true) {
		if (!Continue_excecute) return;

		QElapsedTimer t;
		t.start();
		while(t.elapsed()<1000/Execute_speed)
			QCoreApplication::processEvents();
		if (!Continue_excecute) return;
		if (s.EndOfProgram()) {
			WriteToFile();
			QMessageBox Msgbox;
			Msgbox.setText("Program terminated.");
			Msgbox.exec();
			return;
		}
		tick();
		//WriteToFile();
		flush_all();
		flush_Memory();
		flush_Registers();
	}
}

void Simulator::setPause() {
	Continue_excecute = false;
}

void Simulator::setReset() {
	Continue_excecute = false;
	init(this);
}

void Simulator::setConditionCode(int a, int b, int x) {
	s.ZF = (x == 0);
	s.SF = (x < 0);
	s.OF = ((a < 0) == (b < 0)) && ((x < 0) != (a < 0));
	ui->ZF->setText(QVariant(s.ZF).toString());
	ui->SF->setText(QVariant(s.SF).toString());
	ui->OF->setText(QVariant(s.OF).toString());
}

void Simulator::setExcecuteSpeed() {
	ui->actual_speed->setNum(Execute_speed = ui->execute_speed->value());
}

void Simulator::setPrevious() {
	int tmp = Current_cycle;
	setReset();
	for (int i = 0; i < tmp-1; i++)
		tick();
	flush_all();
	flush_Memory();
	flush_Registers();
}

void Simulator::onTriggeredOpen() {
	/*QString fileName = QFileDialog::getOpenFileName(this, "open *.ys/yo file",
		".","y86 files (*.ys *.yo);;All files (*.*)");*/
	QString fileName = QFileDialog::getOpenFileName(this, "open *.yo file",
		".","yo files (*.yo);;All files (*.*)");
	//printf("%s", fileName.toStdString().c_str());
	if (fileName == "") return;
	if (/*!fileName.endsWith(".ys") && */(!fileName.endsWith(".yo"))) {
		QMessageBox Msgbox;
		Msgbox.setText("Fail to open the selected file");
		Msgbox.exec();
		return;
	}
	if (fileName.endsWith(".ys")) {
		//todo Compile the ys to yo
	} else {
		current_file = new QFile(fileName);
		current_file->open(QIODevice::ReadOnly|QIODevice::Text);
		QString data = QString(current_file->readAll());
		ui->codebox->setText(data);
		current_file->close();
		Parse();
		init(this);
	}
}

Simulator::~Simulator() {
	delete ui;
}
