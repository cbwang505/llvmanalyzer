#pragma once

#include <QDialog>
#include "ui_FunctionAnalysis.h"
#include "utils.h"

class FunctionAnalysis : public QDialog
{
	Q_OBJECT

public:
	FunctionAnalysis(QWidget *parent = Q_NULLPTR);
	~FunctionAnalysis();

	void set_function_start_addr(QString addr);
	void init_seg_range(QMap<int, QString>& seg_map);
	int get_seg();
	bool get_range(ea_t* start, ea_t* end);
private slots:
        void on_buttonBox_accepted();

        void on_buttonBox_rejected();

private:
	Ui::FunctionAnalysis ui;
	
};
