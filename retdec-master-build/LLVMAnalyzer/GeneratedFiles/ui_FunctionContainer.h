/********************************************************************************
** Form generated from reading UI file 'FunctionContainer.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FUNCTIONCONTAINER_H
#define UI_FUNCTIONCONTAINER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FunctionContainer
{
public:
    QListWidget *listWidget;
    QDialogButtonBox *buttonBox;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QCheckBox *checkBox;
    QPushButton *Button1;
    QPushButton *Button2;
    QPushButton *Button3;

    void setupUi(QDialog *FunctionContainer)
    {
        if (FunctionContainer->objectName().isEmpty())
            FunctionContainer->setObjectName(QStringLiteral("FunctionContainer"));
        FunctionContainer->resize(334, 300);
        listWidget = new QListWidget(FunctionContainer);
        listWidget->setObjectName(QStringLiteral("listWidget"));
        listWidget->setGeometry(QRect(0, 41, 331, 221));
        buttonBox = new QDialogButtonBox(FunctionContainer);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(80, 270, 156, 23));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        layoutWidget = new QWidget(FunctionContainer);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(12, 10, 322, 25));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        checkBox = new QCheckBox(layoutWidget);
        checkBox->setObjectName(QStringLiteral("checkBox"));

        horizontalLayout->addWidget(checkBox);

        Button1 = new QPushButton(layoutWidget);
        Button1->setObjectName(QStringLiteral("Button1"));

        horizontalLayout->addWidget(Button1);

        Button2 = new QPushButton(layoutWidget);
        Button2->setObjectName(QStringLiteral("Button2"));

        horizontalLayout->addWidget(Button2);

        Button3 = new QPushButton(layoutWidget);
        Button3->setObjectName(QStringLiteral("Button3"));

        horizontalLayout->addWidget(Button3);


        retranslateUi(FunctionContainer);
        QObject::connect(Button1, SIGNAL(clicked()), FunctionContainer, SLOT(AllButtonClicked()));
        QObject::connect(Button2, SIGNAL(clicked()), FunctionContainer, SLOT(NoneButtonClicked()));
        QObject::connect(Button3, SIGNAL(clicked()), FunctionContainer, SLOT(DeleteButtonClicked()));
        QObject::connect(checkBox, SIGNAL(stateChanged(int)), FunctionContainer, SLOT(OnStateChanged(int)));

        QMetaObject::connectSlotsByName(FunctionContainer);
    } // setupUi

    void retranslateUi(QDialog *FunctionContainer)
    {
        FunctionContainer->setWindowTitle(QApplication::translate("FunctionContainer", "FunctionContainer", 0));
        checkBox->setText(QApplication::translate("FunctionContainer", "SelectAll", 0));
        Button1->setText(QApplication::translate("FunctionContainer", "All", 0));
        Button2->setText(QApplication::translate("FunctionContainer", "None", 0));
        Button3->setText(QApplication::translate("FunctionContainer", "delete", 0));
    } // retranslateUi

};

namespace Ui {
    class FunctionContainer: public Ui_FunctionContainer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FUNCTIONCONTAINER_H
