/********************************************************************************
** Form generated from reading UI file 'FunctionAnalysis.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FUNCTIONANALYSIS_H
#define UI_FUNCTIONANALYSIS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_FunctionAnalysis
{
public:
    QLineEdit *funcaddr;
    QLineEdit *depth;
    QLineEdit *rangestart;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QDialogButtonBox *buttonBox;
    QLineEdit *rangeend;
    QLabel *label_4;
    QLabel *label_5;
    QComboBox *segcombox;

    void setupUi(QDialog *FunctionAnalysis)
    {
        if (FunctionAnalysis->objectName().isEmpty())
            FunctionAnalysis->setObjectName(QStringLiteral("FunctionAnalysis"));
        FunctionAnalysis->resize(470, 114);
        funcaddr = new QLineEdit(FunctionAnalysis);
        funcaddr->setObjectName(QStringLiteral("funcaddr"));
        funcaddr->setGeometry(QRect(10, 30, 113, 20));
        depth = new QLineEdit(FunctionAnalysis);
        depth->setObjectName(QStringLiteral("depth"));
        depth->setGeometry(QRect(160, 30, 113, 20));
        rangestart = new QLineEdit(FunctionAnalysis);
        rangestart->setObjectName(QStringLiteral("rangestart"));
        rangestart->setGeometry(QRect(10, 80, 113, 20));
        label = new QLabel(FunctionAnalysis);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 10, 121, 16));
        label_2 = new QLabel(FunctionAnalysis);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(160, 10, 111, 16));
        label_3 = new QLabel(FunctionAnalysis);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(310, 10, 111, 16));
        buttonBox = new QDialogButtonBox(FunctionAnalysis);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(300, 80, 156, 23));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        rangeend = new QLineEdit(FunctionAnalysis);
        rangeend->setObjectName(QStringLiteral("rangeend"));
        rangeend->setGeometry(QRect(160, 80, 113, 20));
        label_4 = new QLabel(FunctionAnalysis);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(10, 60, 111, 16));
        label_5 = new QLabel(FunctionAnalysis);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(160, 60, 111, 16));
        segcombox = new QComboBox(FunctionAnalysis);
        segcombox->setObjectName(QStringLiteral("segcombox"));
        segcombox->setGeometry(QRect(300, 30, 151, 22));

        retranslateUi(FunctionAnalysis);

        QMetaObject::connectSlotsByName(FunctionAnalysis);
    } // setupUi

    void retranslateUi(QDialog *FunctionAnalysis)
    {
        FunctionAnalysis->setWindowTitle(QApplication::translate("FunctionAnalysis", "FunctionAnalysis", 0));
        depth->setText(QApplication::translate("FunctionAnalysis", "100", 0));
        label->setText(QApplication::translate("FunctionAnalysis", "Function Start Addr", 0));
        label_2->setText(QApplication::translate("FunctionAnalysis", "Search Depth", 0));
        label_3->setText(QApplication::translate("FunctionAnalysis", "Allocate Segment", 0));
        label_4->setText(QApplication::translate("FunctionAnalysis", "Range Start", 0));
        label_5->setText(QApplication::translate("FunctionAnalysis", "Range End", 0));
    } // retranslateUi

};

namespace Ui {
    class FunctionAnalysis: public Ui_FunctionAnalysis {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FUNCTIONANALYSIS_H
