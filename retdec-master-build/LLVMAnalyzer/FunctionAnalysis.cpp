#include "FunctionAnalysis.h"


FunctionAnalysis::FunctionAnalysis(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

FunctionAnalysis::~FunctionAnalysis()
{
}

void FunctionAnalysis::set_function_start_addr(QString addr)
{
	ui.funcaddr->setText(addr);
}

void FunctionAnalysis::init_seg_range(QMap<int, QString>& seg_map)
{
	QString llvmseg = "_llvm";
	for (QMap<int, QString>::const_iterator seg = seg_map.constBegin(); seg != seg_map.constEnd();seg++) {
		if (llvmseg.compare(seg.value(), Qt::CaseInsensitive) == 0)
		{
			ui.segcombox->addItem(seg.value(), seg.key());
		}
	}
	for (QMap<int, QString>::const_iterator seg = seg_map.constBegin(); seg != seg_map.constEnd(); seg++) {
		if (llvmseg.compare(seg.value(), Qt::CaseInsensitive) != 0)
		{
			ui.segcombox->addItem(seg.value(), seg.key());
		}
	}
}

int FunctionAnalysis::get_seg()
{
	return ui.segcombox->currentData().value<int>();
}

bool FunctionAnalysis::get_range(ea_t* start, ea_t* end)
{
	bool ok;
	*start=ui.rangestart->text().toInt(&ok, 16);
	if(!ok)
	{
		return false;
	}
	*end=ui.rangeend->text().toInt(&ok, 16);
	return ok;
}

void FunctionAnalysis::on_buttonBox_accepted()
{
	accept();
}

void FunctionAnalysis::on_buttonBox_rejected()
{
	reject();
}
