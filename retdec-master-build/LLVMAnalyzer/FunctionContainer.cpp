#include "FunctionContainer.h"



FunctionContainer::FunctionContainer(QWidget *parent)
	: QDialog(parent), plg(nullptr)
{
	ui.setupUi(this);
}
void FunctionContainer::SetupPlugin(retdec::plugin::RetDec& p, delfunc delhdl)
{
	plg = &p;
	delhandler = delhdl;
	for (auto& func : p.decfuncs)
	{
		const QString funcnm = func.getName().c_str();
		QListWidgetItem* item = new QListWidgetItem(funcnm);
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		item->setCheckState(Qt::Checked);
		uint64_t addr = func.getStart();
		item->setData(Qt::UserRole, QVariant::fromValue(addr));
		ui.listWidget->addItem(item);
		
	}

}

void FunctionContainer::FinalizeSelectedFunctions()
{
	for (int i = 0;i< ui.listWidget->count(); i++)
	{
		QListWidgetItem* item = ui.listWidget->item(i);
		if(item->checkState()==Qt::Checked)
		{
			QVariant val = item->data(Qt::UserRole);
			uint64_t addr = val.toULongLong();
			for (auto& func : plg->decfuncs)
			{
				uint64_t addrtmp = func.getStart();
				if (addr == addrtmp)
				{
					selfuncs.emplace(func);
					break;
				}
			}
		}
	}
}

void FunctionContainer::AllButtonClicked()
{
	for (int i = 0; i < ui.listWidget->count(); i++)
	{
		QListWidgetItem* item = ui.listWidget->item(i);
		item->setCheckState(Qt::Checked);
	}
	
}

void FunctionContainer::NoneButtonClicked()
{
	for (int i = 0; i < ui.listWidget->count(); i++)
	{
		QListWidgetItem* item = ui.listWidget->item(i);
		item->setCheckState(Qt::Unchecked);
	}

}
void FunctionContainer::DeleteButtonClicked()
{
	QListWidgetItem* item = ui.listWidget->currentItem();
	QVariant val = item->data(Qt::UserRole);
	uint64_t addr = val.toULongLong();
	delhandler(addr);
	ui.listWidget->removeItemWidget(item);
	delete item;
}

void FunctionContainer::OnStateChanged(int state)
{
	if (state == Qt::Checked) // "бЁжа"
	{
		AllButtonClicked();
	}else
	{
		NoneButtonClicked();
	}
}

void FunctionContainer::on_buttonBox_accepted()
{
	accept();
}

void FunctionContainer::on_buttonBox_rejected()
{
	reject();
}

FunctionContainer::~FunctionContainer()
{
}
