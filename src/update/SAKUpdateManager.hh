﻿/*
 * Copyright 2018-2020 Qter(qsak@foxmail.com). All rights reserved.
 *
 * The file is encoding with utf-8 (with BOM). It is a part of QtSwissArmyKnife
 * project(https://www.qsak.pro). The project is an open source project. You can
 * get the source of the project from: "https://github.com/qsak/QtSwissArmyKnife"
 * or "https://gitee.com/qsak/QtSwissArmyKnife". Also, you can join in the QQ
 * group which number is 952218522 to have a communication.
 */
#ifndef SAKUPDATEMANAGER_HH
#define SAKUPDATEMANAGER_HH

#include <QTimer>
#include <QLabel>
#include <QDialog>
#include <QGroupBox>
#include <QCheckBox>
#include <QListWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QTextBrowser>
#include <QNetworkReply>
#include <QNetworkAccessManager>

namespace Ui {
    class SAKUpdateManager;
}

class SAKUpdateManager:public QDialog
{
    Q_OBJECT
public:
    SAKUpdateManager(QWidget *parent =  Q_NULLPTR);
    ~SAKUpdateManager();

    void checkForUpdate();
    bool enableAutoCheckedForUpdate();
private:
    Ui::SAKUpdateManager *ui;
    QLabel *currentVersionLabel;
    QLabel *newVersionLabel;
    QLabel *updateProgressLabel;
    QProgressBar *updateProgressBar;
    QLabel *noNewVersionTipLabel;
    QGroupBox *newVersionCommentsGroupBox;
    QTextBrowser *newVersionCommentsTextBrowser;
    QListWidget *downloadListListWidget;
    QCheckBox *autoCheckForUpdateCheckBox;
    QPushButton *visitWebPushButton;
    QPushButton *checkForUpdatePushButton;
    QLabel *infoLabel;

private slots:
    void on_autoCheckForUpdateCheckBox_clicked();
    void on_visitWebPushButton_clicked();
    void on_checkForUpdatePushButton_clicked();

private:
    struct UpdateInfo{
        bool isValid;                   // 是否可用
        QString errorString;            // 错误信息

        QString htmlUrl;                // 发布页面地址
        QString name;                   // 最新版本号
        QStringList browserDownloadUrl; // 下载链接
        QString body;                   // 发布描述
        QString tarballUrl;             // 源码包（tar.tz）
        QString zipballUrl;             // 源码包（zip）
    }updateInfo;

    QTimer clearInfoTimer;
    QNetworkAccessManager networkAccessManager;
    QNetworkReply *networkReply;

private:
    void outputInfo(QString info, bool isError = false);
    void clearInfo();
    void checkForUpdateFinished();

    UpdateInfo extractUpdateInfo(QByteArray jsonObjectData);
    QStringList extractBrowserDownloadUrl(QJsonArray jsonArray);
    bool isNewVersion(QString remoteVersion);
    void setupDownloadList(UpdateInfo info);
    void clearDownloadList();
};

#endif
