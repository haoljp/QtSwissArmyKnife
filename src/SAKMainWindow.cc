﻿/*
 * Copyright 2018-2020 Qter(qsak@foxmail.com). All rights reserved.
 *
 * The file is encoding with utf-8 (with BOM). It is a part of QtSwissArmyKnife
 * project(https://www.qsak.pro). The project is an open source project. You can
 * get the source of the project from: "https://github.com/qsak/QtSwissArmyKnife"
 * or "https://gitee.com/qsak/QtSwissArmyKnife". Also, you can join in the QQ
 * group which number is 952218522 to have a communication.
 */
#include <QDir>
#include <QFile>
#include <QRect>
#include <QLocale>
#include <QTabBar>
#include <QAction>
#include <QVariant>
#include <QSysInfo>
#include <QMetaEnum>
#include <QSettings>
#include <QStatusBar>
#include <QClipboard>
#include <QJsonArray>
#include <QScrollBar>
#include <QScrollArea>
#include <QJsonObject>
#include <QSpacerItem>
#include <QMessageBox>
#include <QStyleFactory>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDesktopServices>

#include "SAK.hh"
#include "SAKGlobal.hh"
#include "SAKSettings.hh"
#include "SAKSettings.hh"
#include "SAKDataStruct.hh"
#include "SAKMainWindow.hh"
#include "QtAppStyleApi.hh"
#include "SAKApplication.hh"
#include "SAKQRCodeDialog.hh"
#include "SAKUdpDebugPage.hh"
#include "QtStyleSheetApi.hh"
#ifdef SAK_IMPORT_HID_MODULE
#include "SAKHidDebugPage.hh"
#endif
#ifdef SAK_IMPORT_USB_MODULE
#include "SAKUsbDebugPage.hh"
#endif
#ifdef SAK_IMPORT_QRCODE_MODULE
#include "SAKQRCodeCreator.hh"
#endif
#include "SAKCRCCalculator.hh"
#include "SAKUpdateManager.hh"
#include "SAKMoreInformation.hh"
#include "SAKTcpClientDebugPage.hh"
#include "SAKTcpServerDebugPage.hh"
#ifdef SAK_IMPORT_COM_MODULE
#include "SAKSerialPortDebugPage.hh"
#endif
#ifdef SAK_IMPORT_WEBSOCKET_MODULE
#include "SAKWebSocketClientDebugPage.hh"
#include "SAKWebSocketServerDebugPage.hh"
#endif
#ifdef SAK_IMPORT_FILECHECKER_MODULE
#include "QtCryptographicHashController.hh"
#endif

#include "ui_SAKMainWindow.h"

SAKMainWindow::SAKMainWindow(QWidget *parent)
    :QMainWindow(parent)
    ,tabWidget(new QTabWidget)
    ,moreInformation(new SAKMoreInformation)
    ,ui(new Ui::SAKMainWindow)
{
    ui->setupUi(this);
    updateManager = new SAKUpdateManager(this);
    qrCodeDialog = new SAKQRCodeDialog(this);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(tabWidget);
#if 0
    setWindowFlags(Qt::FramelessWindowHint);
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    setCentralWidget(scrollArea);
    scrollArea->setLayout(layout);
    scrollArea->layout()->setContentsMargins(6, 6, 6, 6);
    scrollArea->setWidget(tabWidget);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QDesktopWidget *desktop = QApplication::desktop();
    QRect rect = desktop->screenGeometry(tabWidget);
    tabWidget->setFixedWidth(rect.width());
#else
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    centralWidget->setLayout(layout);

    centralWidget->layout()->setContentsMargins(6, 6, 6, 6);
#endif
    setWindowTitle(tr("瑞士军刀--开发调试工具集")
                   + " v" + SAK::instance()->version()
                   + " " + tr("用户交流QQ群") + " " + SAK::instance()->qqGroupNumber());

    tabWidget->setTabsClosable(true);
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &SAKMainWindow::closeDebugPage);

    /*
     * 以下代码是设置软件风格
     * 对于MACOS来说不设置为Windows（MACOS软件风格有两种，一种是Windows，另外一种好像是MACOS，不太确定）
     * 对于Windows或者Linux系统来说，设置Funsion为默认软件风格
     * 安卓系统不设置软件风格
     * 2020年02月09号 Qt5.12 --by Qter
     */
#ifndef Q_OS_ANDROID
    QString settingStyle = SAKSettings::instance()->appStyle();
    if (settingStyle.isEmpty()){
        QStringList styleKeys = QStyleFactory::keys();
#ifdef Q_OS_MACOS
        QString uglyStyle("Windows");
        for(auto var:styleKeys){
            if (var != uglyStyle){
                changeAppStyle(var);
                break;
            }
    }
#else
        QString defaultStyle("Fusion");
        if (styleKeys.contains(defaultStyle)){
            changeAppStyle(defaultStyle);
        }
#endif
    }
#endif

    addTab();
    initMenu();
    addTool();

    connect(QtStyleSheetApi::instance(), &QtStyleSheetApi::styleSheetChanged, this, &SAKMainWindow::changeStylesheet);
    connect(QtAppStyleApi::instance(), &QtAppStyleApi::appStyleChanged, this, &SAKMainWindow::changeAppStyle);
}

SAKMainWindow::~SAKMainWindow()
{
    delete ui;
}

bool SAKMainWindow::eventFilter(QObject *obj, QEvent *event)
{
    return QMainWindow::eventFilter(obj, event);
}

void SAKMainWindow::addTab()
{
    /// @brief 添加调试页面
#ifdef SAK_IMPORT_COM_MODULE
    this->tabWidget->addTab(new SAKSerialPortDebugPage, SAKGlobal::getNameOfDebugPage(SAKDataStruct::DebugPageTypeCOM));
#endif
#ifdef SAK_IMPORT_HID_MODULE
    this->tabWidget->addTab(new SAKHidDebugPage, SAKGlobal::getNameOfDebugPage(SAKDataStruct::DebugPageTypeHID));
#endif
#ifdef SAK_IMPORT_USB_MODULE
    this->tabWidget->addTab(new SAKUsbDebugPage, SAKGlobal::getNameOfDebugPage(SAKDataStruct::DebugPageTypeUSB));
#endif
    this->tabWidget->addTab(new SAKUdpDebugPage, SAKGlobal::getNameOfDebugPage(SAKDataStruct::DebugPageTypeUDP));
    this->tabWidget->addTab(new SAKTcpClientDebugPage, SAKGlobal::getNameOfDebugPage(SAKDataStruct::DebugPageTypeTCPClient));
    this->tabWidget->addTab(new SAKTcpServerDebugPage, SAKGlobal::getNameOfDebugPage(SAKDataStruct::DebugPageTypeTCPServer));
#ifdef SAK_IMPORT_WEBSOCKET_MODULE
    this->tabWidget->addTab(new SAKWebSocketClientDebugPage, SAKGlobal::getNameOfDebugPage(SAKDataStruct::DebugPageTypeWebSocketClient));
    this->tabWidget->addTab(new SAKWebSocketServerDebugPage, SAKGlobal::getNameOfDebugPage(SAKDataStruct::DebugPageTypeWebSocketServer));
#endif

    /// @brief 隐藏关闭按钮（必须在调用setTabsClosable()函数后设置，否则不生效）
    for (int i = 0; i < tabWidget->count(); i++){
        tabWidget->tabBar()->setTabButton(i, QTabBar::RightSide, Q_NULLPTR);
        tabWidget->tabBar()->setTabButton(i, QTabBar::LeftSide, Q_NULLPTR);
    }
}

void SAKMainWindow::addTool()
{
#ifdef SAK_IMPORT_FILECHECKER_MODULE
    addTool(tr("文件校验工具"),     new QtCryptographicHashController);
#endif

#ifdef SAK_IMPORT_QRCODE_MODULE
    addTool(tr("二维码生成工具"), new SAKQRCodeCreator);
#endif

    QAction *action = Q_NULLPTR;
    action = new QAction(tr("CRC计算器"), this);
    toolsMenu->addAction(action);
    connect(action, &QAction::triggered, this, &SAKMainWindow::createCRCCalculator);
}

void SAKMainWindow::about()
{
    QMessageBox::information(this, tr("关于"), QString("<font color=green>%1</font><br />%2<br />"
                                                     "<font color=green>%3</font><br />%4<br />"
                                                     "<font color=green>%5</font><br />%6<br />"
                                                     "<font color=green>%7</font><br /><a href=%8>%8</a><br />"
                                                     "<font color=green>%9</font><br />%10<br />"
                                                     "<font color=green>%11</font><br />%12<br />"
                                                     "<font color=green>%13</font><br />%14<br />"
                                                     "<font color=green>%15</font><br />%16<br />"
                                                     "<font color=green>%17</font><br />%18<br />"
                                                     "<font color=red>%19</font><br />%20")
                             .arg(tr("软件版本")).arg(SAK::instance()->version())
                             .arg(tr("软件作者")).arg(SAK::instance()->authorName())
                             .arg(tr("作者昵称")).arg(SAK::instance()->authorNickname())
                             .arg(tr("发布网站")).arg(SAK::instance()->officeUrl())
                             .arg(tr("联系邮箱")).arg(SAK::instance()->email())
                             .arg(tr("QQ账号")).arg(SAK::instance()->qqNumber())
                             .arg(tr("QQ交流群")).arg(SAK::instance()->qqGroupNumber())
                             .arg(tr("编译时间")).arg(SAK::instance()->buildTime())
                             .arg(tr("版权信息")).arg(SAK::instance()->copyright())
                             .arg(tr("业务合作")).arg(SAK::instance()->business()));
}

void SAKMainWindow::addTool(QString toolName, QWidget *toolWidget)
{
    QAction *action = new QAction(toolName, this);
    toolsMenu->addAction(action);
    connect(action, SIGNAL(triggered(bool)), toolWidget, SLOT(show()));
}

void SAKMainWindow::changeStylesheet(QString styleSheetName)
{
    SAKSettings::instance()->setAppStylesheet(styleSheetName);
    if (!styleSheetName.isEmpty()){
        defaultStyleSheetAction->setChecked(false);
    }
}

void SAKMainWindow::changeAppStyle(QString appStyle)
{
    SAKSettings::instance()->setAppStyle(appStyle);
}

void SAKMainWindow::initMenu()
{
    QMenuBar *menuBar = new QMenuBar(Q_NULLPTR);
    setMenuBar(menuBar);
    initFileMenu();
    initToolMenu();
    initOptionMenu();    
    initLanguageMenu();    
    initLinksMenu();
    initHelpMenu();
}

void SAKMainWindow::initFileMenu()
{
    QMenu *fileMenu = new QMenu(tr("文件"), this);
    menuBar()->addMenu(fileMenu);

    QMenu *tabMenu = new QMenu(tr("新建页面"), this);
    fileMenu->addMenu(tabMenu);
    QMetaEnum enums = QMetaEnum::fromType<SAKDataStruct::SAKEnumDebugPageType>();
    for (int i = 0; i < enums.keyCount(); i++){
        QAction *a = new QAction(SAKGlobal::getNameOfDebugPage(i), this);
        a->setObjectName(SAKGlobal::getNameOfDebugPage(i));
        QVariant var = QVariant::fromValue<int>(enums.value(i));
        a->setData(var);
        connect(a, &QAction::triggered, this, &SAKMainWindow::addRemovablePage);
        tabMenu->addAction(a);
    }

    QMenu *windowMenu = new QMenu(tr("新建窗口"), this);
    fileMenu->addMenu(windowMenu);
    for (int i = 0; i < enums.keyCount(); i++){
        QAction *a = new QAction(SAKGlobal::getNameOfDebugPage(i), this);
        a->setObjectName(SAKGlobal::getNameOfDebugPage(i));
        QVariant var = QVariant::fromValue<int>(enums.value(i));
        connect(a, &QAction::triggered, this, &SAKMainWindow::openIODeviceWindow);
        a->setData(var);
        windowMenu->addAction(a);
    }

    fileMenu->addSeparator();
    QAction *exitAction = new QAction(tr("退出"), this);
    fileMenu->addAction(exitAction);
    connect(exitAction, SIGNAL(triggered(bool)), this, SLOT(close()));
}

void SAKMainWindow::initToolMenu()
{
    QMenu *toolMenu = new QMenu(tr("工具"));
    menuBar()->addMenu(toolMenu);
    toolsMenu = toolMenu;
}

void SAKMainWindow::initOptionMenu()
{
    QMenu *optionMenu = new QMenu(tr("选项"));
    menuBar()->addMenu(optionMenu);

    /// @brief 软件样式，设置默认样式需要重启软件
    QMenu *stylesheetMenu = new QMenu(tr("皮肤"), this);
    optionMenu->addMenu(stylesheetMenu);
    defaultStyleSheetAction = new QAction(tr("Qt默认样式"), this);
    defaultStyleSheetAction->setCheckable(true);
    stylesheetMenu->addAction(defaultStyleSheetAction);
    connect(defaultStyleSheetAction, &QAction::triggered, [=](){
        for(auto var:QtStyleSheetApi::instance()->actions()){
            var->setChecked(false);
        }

        changeStylesheet(QString());
        defaultStyleSheetAction->setChecked(true);
        QMessageBox::information(this, tr("重启软件生效"), tr("软件样式已更改，重启软件后即可使用默认样式"));
    });

    stylesheetMenu->addSeparator();
    stylesheetMenu->addActions(QtStyleSheetApi::instance()->actions());
    QString styleSheetName = SAKSettings::instance()->appStylesheet();
    if (!styleSheetName.isEmpty()){
        QtStyleSheetApi::instance()->setStyleSheet(styleSheetName);
    }else{
        defaultStyleSheetAction->setChecked(true);
    }

    /// @brief 软件风格，默认使用Qt支持的第一种软件风格
    QMenu *appStyleMenu = new QMenu(tr("软件风格"), this);
    optionMenu->addMenu(appStyleMenu);
    appStyleMenu->addActions(QtAppStyleApi::instance()->actions());
    QString style = SAKSettings::instance()->appStyle();
    QtAppStyleApi::instance()->setStyle(style);
}

void SAKMainWindow::initLanguageMenu()
{
    QMenu *languageMenu = new QMenu(tr("语言"), this);
    menuBar()->addMenu(languageMenu);

    QString language = SAKSettings::instance()->language();

    QFile file(":/translations/sak/Translations.json");
    file.open(QFile::ReadOnly);
    QByteArray jsonData = file.readAll();

    QJsonParseError jsonError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData, &jsonError);
    if (jsonError.error == QJsonParseError::NoError){
        if (jsonDocument.isArray()){
            QJsonArray jsonArray = jsonDocument.array();
            struct info {
                QString locale;
                QString language;
                QString name;
            };
            QList<info> infoList;

            for (int i = 0; i < jsonArray.count(); i++){
                QJsonObject jsonObject = jsonArray.at(i).toObject();
                struct info languageInfo;
                languageInfo.locale = jsonObject.value("locale").toString();
                languageInfo.language = jsonObject.value("language").toString();
                languageInfo.name = jsonObject.value("name").toString();
                infoList.append(languageInfo);
            }

            QActionGroup *actionGroup = new QActionGroup(this);
            for(auto var:infoList){
                QAction *action = new QAction(var.name, languageMenu);
                languageMenu->addAction(action);
                action->setCheckable(true);
                actionGroup->addAction(action);
                action->setObjectName(var.language);
                action->setData(QVariant::fromValue<QString>(var.name));
                action->setIcon(QIcon(QString(":/translations/sak/%1").arg(var.locale).toLatin1()));
                connect(action, &QAction::triggered, this, &SAKMainWindow::installLanguage);

                if (var.language == language.split('-').first()){
                    action->setChecked(true);
                }
            }
        }
    }else{
        Q_ASSERT_X(false, __FUNCTION__, "Json file parsing failed!");
    }    
}

void SAKMainWindow::initHelpMenu()
{
    QMenu *helpMenu = new QMenu(tr("帮助"), this);
    menuBar()->addMenu(helpMenu);

    QAction *aboutQtAction = new QAction(tr("关于Qt"), this);
    helpMenu->addAction(aboutQtAction);
    connect(aboutQtAction, &QAction::triggered, [=](){QMessageBox::aboutQt(this, tr("关于Qt"));});

    QAction *aboutAction = new QAction(tr("关于软件"), this);
    helpMenu->addAction(aboutAction);
    connect(aboutAction, &QAction::triggered, this, &SAKMainWindow::about);

    QMenu *srcMenu = new QMenu(tr("获取源码"), this);
    helpMenu->addMenu(srcMenu);
    QAction *visitGitHubAction = new QAction(QIcon(":/resources/images/GitHub.png"), tr("GitHub"), this);
    connect(visitGitHubAction, &QAction::triggered, [](){QDesktopServices::openUrl(QUrl(QLatin1String("https://github.com/qsak/QtSwissArmyKnife")));});
    srcMenu->addAction(visitGitHubAction);
    QAction *visitGiteeAction = new QAction(QIcon(":/resources/images/Gitee.png"), tr("Gitee"), this);
    connect(visitGiteeAction, &QAction::triggered, [](){QDesktopServices::openUrl(QUrl(QLatin1String("https://gitee.com/qsak/QtSwissArmyKnife")));});
    srcMenu->addAction(visitGiteeAction);

    QAction *updateAction = new QAction(tr("检查更新"), this);
    helpMenu->addAction(updateAction);
    connect(updateAction, &QAction::triggered, updateManager, &SAKUpdateManager::show);

    QAction *moreInformationAction = new QAction(tr("更多信息"), this);
    helpMenu->addAction(moreInformationAction);
    connect(moreInformationAction, &QAction::triggered, moreInformation, &SAKMoreInformation::show);

    helpMenu->addSeparator();
    QAction *qrCodeAction = new QAction(tr("二维码"), this);
    helpMenu->addAction(qrCodeAction);
    connect(qrCodeAction, &QAction::triggered, qrCodeDialog, &SAKQRCodeDialog::show);
}

void SAKMainWindow::initLinksMenu()
{
    QMenu *linksMenu = new QMenu(tr("链接"), this);
    menuBar()->addMenu(linksMenu);

    struct Link {
        QString name;
        QString url;
    };
    QList<Link> linkList;
    linkList << Link{tr("Qt官方下载"), QString("http://download.qt.io/official_releases/qt")}
             << Link{tr("Qt官方博客"), QString("https://www.qt.io/blog")}
             << Link{tr("Qt发布信息"), QString("https://wiki.qt.io/Qt_5.12_Release")};

    for (auto var:linkList){
        QAction *action = new QAction(QIcon(":/resources/images/Qt.png"), var.name, this);
        action->setObjectName(var.url);
        linksMenu->addAction(action);

        connect(action, &QAction::triggered, this, [=](){
            QDesktopServices::openUrl(QUrl(sender()->objectName()));
        });
    }
}

void SAKMainWindow::installLanguage()
{
    if (!sender()){
        return;
    }

    QAction *action = reinterpret_cast<QAction*>(sender());
    action->setChecked(true);

    QString language = action->objectName();
    QString name = action->data().toString();
    SAKSettings::instance()->setLanguage(language+"-"+name);
    reinterpret_cast<SAKApplication*>(qApp)->installLanguage();
    QMessageBox::information(this, tr("重启生效"), tr("软件语言包已更改，重启软件生效！"));
}

void SAKMainWindow::addRemovablePage()
{
    int type = qobject_cast<QAction*>(sender())->data().value<int>();
    QWidget *widget = getDebugPage(type);
    widget->setAttribute(Qt::WA_DeleteOnClose, true);
    tabWidget->addTab(widget, sender()->objectName());
}

void SAKMainWindow::openIODeviceWindow()
{
    int type = qobject_cast<QAction*>(sender())->data().value<int>();
    QWidget *widget = getDebugPage(type);
    widget->setAttribute(Qt::WA_DeleteOnClose, true);
    widget->show();
}

QWidget *SAKMainWindow::getDebugPage(int type)
{
    QWidget *widget = Q_NULLPTR;
    switch (type) {
    case SAKDataStruct::DebugPageTypeUDP:
        widget = new SAKUdpDebugPage;
        break;
    case SAKDataStruct::DebugPageTypeTCPClient:
        widget = new SAKTcpClientDebugPage;
        break;
    case SAKDataStruct::DebugPageTypeTCPServer:
        widget = new SAKTcpServerDebugPage;
        break;
#ifdef SAK_IMPORT_WEBSOCKET_MODULE
    case SAKDataStruct::DebugPageTypeWebSocketClient:
        widget = new SAKWebSocketClientDebugPage;
        break;
    case SAKDataStruct::DebugPageTypeWebSocketServer:
        widget = new SAKWebSocketServerDebugPage;
        break;
#endif
#ifdef SAK_IMPORT_COM_MODULE
    case SAKDataStruct::DebugPageTypeCOM:
        widget = new SAKSerialPortDebugPage;
        break;
#endif

#ifdef SAK_IMPORT_HID_MODULE
    case SAKDataStruct::DebugPageTypeHID:
        widget = new SAKHidDebugPage;
        break;
#endif

#ifdef SAK_IMPORT_USB_MODULE
    case SAKDataStruct::DebugPageTypeUSB:
        widget = new SAKUsbDebugPage;
        break;
#endif
    default:
        Q_ASSERT_X(false, __FUNCTION__, "Unknow window type.");
        break;
    }

    return widget;
}

void SAKMainWindow::closeDebugPage(int index)
{
    QWidget *w = tabWidget->widget(index);
    tabWidget->removeTab(index);
    w->close();
}

void SAKMainWindow::createCRCCalculator()
{
    SAKCRCCalculator *cal = new SAKCRCCalculator;
    cal->show();
}
