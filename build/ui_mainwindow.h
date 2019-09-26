/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionAdd;
    QAction *actionRemove;
    QAction *actionExit;
    QAction *actionAbout;
    QAction *actionCopy_Global_URL;
    QAction *actionGo_online;
    QAction *actionGo_offline;
    QAction *actionDownload_selected;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_3;
    QTreeWidget *localList;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout;
    QLineEdit *remoteAddress;
    QPushButton *Goto;
    QPushButton *refresh;
    QTreeWidget *remoteList;
    QWidget *tab_3;
    QTreeWidget *downloadsView;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QMenu *menuShares;
    QMenu *menuRemote;
    QStatusBar *statusView;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(560, 525);
        actionAdd = new QAction(MainWindow);
        actionAdd->setObjectName(QString::fromUtf8("actionAdd"));
        actionRemove = new QAction(MainWindow);
        actionRemove->setObjectName(QString::fromUtf8("actionRemove"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionCopy_Global_URL = new QAction(MainWindow);
        actionCopy_Global_URL->setObjectName(QString::fromUtf8("actionCopy_Global_URL"));
        actionCopy_Global_URL->setEnabled(true);
        actionCopy_Global_URL->setVisible(true);
        actionGo_online = new QAction(MainWindow);
        actionGo_online->setObjectName(QString::fromUtf8("actionGo_online"));
        actionGo_offline = new QAction(MainWindow);
        actionGo_offline->setObjectName(QString::fromUtf8("actionGo_offline"));
        actionGo_offline->setEnabled(false);
        actionGo_offline->setVisible(true);
        actionDownload_selected = new QAction(MainWindow);
        actionDownload_selected->setObjectName(QString::fromUtf8("actionDownload_selected"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout_2 = new QVBoxLayout(centralWidget);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        verticalLayout_3 = new QVBoxLayout(tab);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        localList = new QTreeWidget(tab);
        localList->setObjectName(QString::fromUtf8("localList"));
        localList->header()->setMinimumSectionSize(150);
        localList->header()->setDefaultSectionSize(160);

        verticalLayout_3->addWidget(localList);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        verticalLayout_4 = new QVBoxLayout(tab_2);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        remoteAddress = new QLineEdit(tab_2);
        remoteAddress->setObjectName(QString::fromUtf8("remoteAddress"));

        horizontalLayout->addWidget(remoteAddress);

        Goto = new QPushButton(tab_2);
        Goto->setObjectName(QString::fromUtf8("Goto"));
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        Goto->setFont(font);

        horizontalLayout->addWidget(Goto);

        refresh = new QPushButton(tab_2);
        refresh->setObjectName(QString::fromUtf8("refresh"));
        refresh->setFont(font);

        horizontalLayout->addWidget(refresh);


        verticalLayout_4->addLayout(horizontalLayout);

        remoteList = new QTreeWidget(tab_2);
        remoteList->setObjectName(QString::fromUtf8("remoteList"));
        remoteList->header()->setDefaultSectionSize(150);

        verticalLayout_4->addWidget(remoteList);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        downloadsView = new QTreeWidget(tab_3);
        downloadsView->setObjectName(QString::fromUtf8("downloadsView"));
        downloadsView->setGeometry(QRect(0, 0, 531, 441));
        downloadsView->setColumnCount(3);
        downloadsView->header()->setDefaultSectionSize(150);
        tabWidget->addTab(tab_3, QString());

        verticalLayout->addWidget(tabWidget);


        verticalLayout_2->addLayout(verticalLayout);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 560, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuShares = new QMenu(menuBar);
        menuShares->setObjectName(QString::fromUtf8("menuShares"));
        menuRemote = new QMenu(menuBar);
        menuRemote->setObjectName(QString::fromUtf8("menuRemote"));
        MainWindow->setMenuBar(menuBar);
        statusView = new QStatusBar(MainWindow);
        statusView->setObjectName(QString::fromUtf8("statusView"));
        MainWindow->setStatusBar(statusView);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuShares->menuAction());
        menuBar->addAction(menuRemote->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionAdd);
        menuFile->addAction(actionRemove);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuHelp->addAction(actionAbout);
        menuShares->addAction(actionCopy_Global_URL);
        menuShares->addAction(actionGo_online);
        menuShares->addAction(actionGo_offline);
        menuRemote->addAction(actionDownload_selected);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Universal Data Share", nullptr));
        actionAdd->setText(QApplication::translate("MainWindow", "Add", nullptr));
        actionRemove->setText(QApplication::translate("MainWindow", "Remove", nullptr));
        actionExit->setText(QApplication::translate("MainWindow", "Exit", nullptr));
        actionAbout->setText(QApplication::translate("MainWindow", "About...", nullptr));
        actionCopy_Global_URL->setText(QApplication::translate("MainWindow", "Copy Global URL", nullptr));
        actionGo_online->setText(QApplication::translate("MainWindow", "Go online", nullptr));
        actionGo_offline->setText(QApplication::translate("MainWindow", "Go offline", nullptr));
        actionDownload_selected->setText(QApplication::translate("MainWindow", "Download selected", nullptr));
        QTreeWidgetItem *___qtreewidgetitem = localList->headerItem();
        ___qtreewidgetitem->setText(2, QApplication::translate("MainWindow", "Path", nullptr));
        ___qtreewidgetitem->setText(1, QApplication::translate("MainWindow", "Size", nullptr));
        ___qtreewidgetitem->setText(0, QApplication::translate("MainWindow", "Name", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "Local", nullptr));
        Goto->setText(QApplication::translate("MainWindow", "Go", nullptr));
        refresh->setText(QApplication::translate("MainWindow", "Refresh", nullptr));
        QTreeWidgetItem *___qtreewidgetitem1 = remoteList->headerItem();
        ___qtreewidgetitem1->setText(1, QApplication::translate("MainWindow", "Size", nullptr));
        ___qtreewidgetitem1->setText(0, QApplication::translate("MainWindow", "Name", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "Remote", nullptr));
        QTreeWidgetItem *___qtreewidgetitem2 = downloadsView->headerItem();
        ___qtreewidgetitem2->setText(2, QApplication::translate("MainWindow", "Save Path", nullptr));
        ___qtreewidgetitem2->setText(1, QApplication::translate("MainWindow", "Progress", nullptr));
        ___qtreewidgetitem2->setText(0, QApplication::translate("MainWindow", "Name", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("MainWindow", "Downloads", nullptr));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", nullptr));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", nullptr));
        menuShares->setTitle(QApplication::translate("MainWindow", "Shares", nullptr));
        menuRemote->setTitle(QApplication::translate("MainWindow", "Remote", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
