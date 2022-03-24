#pragma once

#include <QDialog>
#include "ui_FunctionContainer.h"
#include "retdecida.h"

typedef void (* delfunc)(uint64_t addr);


class FunctionContainer : public QDialog
{
	Q_OBJECT

public:
	FunctionContainer(QWidget *parent = Q_NULLPTR);
	~FunctionContainer();
     void SetupPlugin(retdec::plugin::RetDec& p, delfunc delhdl);
     void FinalizeSelectedFunctions();
	 retdec::common::FunctionContainer selfuncs;
public slots:
	 void AllButtonClicked();
	 void NoneButtonClicked();
	 void DeleteButtonClicked();
	 void OnStateChanged(int state);
	 void on_buttonBox_accepted();
	 void on_buttonBox_rejected();
private:
	Ui::FunctionContainer ui;
	retdec::plugin::RetDec* plg;
	delfunc delhandler;
	
};
