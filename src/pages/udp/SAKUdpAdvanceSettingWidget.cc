﻿/*
 * Copyright 2018-2020 Qter(qsak@foxmail.com). All rights reserved.
 *
 * The file is encoding with utf-8 (with BOM). It is a part of QtSwissArmyKnife
 * project(https://www.qsak.pro). The project is an open source project. You can
 * get the source of the project from: "https://github.com/qsak/QtSwissArmyKnife"
 * or "https://gitee.com/qsak/QtSwissArmyKnife". Also, you can join in the QQ
 * group which number is 952218522 to have a communication.
 */
#include <QDialog>
#include <QMessageBox>
#include <QListWidgetItem>

#include "SAKUdpDevice.hh"
#include "SAKUdpAdvanceSettingWidget.hh"
#include "SAKUdpMulticastEditingDialog.hh"

#include "ui_SAKUdpAdvanceSettingWidget.h"

SAKUdpAdvanceSettingWidget::SAKUdpAdvanceSettingWidget(QWidget *parent)
    :QWidget (parent)
    ,ui (new Ui::SAKUdpAdvanceSettingsWidget)
    ,udpDevice(Q_NULLPTR)
    ,isInitUi (true)

{
    ui->setupUi(this);

    unicastCheckBox = ui->unicastCheckBox;
    broadcastCheckBox = ui->broadcastCheckBox;
    broadcastPortLineEdit = ui->broadcastPortLineEdit;
    multicastCheckBox = ui->multicastCheckBox;
    listWidget = ui->listWidget;
    deletePushButton = ui->deletePushButton;
    addPushButton = ui->addPushButton;

    isInitUi = false;   
}

SAKUdpAdvanceSettingWidget::~SAKUdpAdvanceSettingWidget()
{
    delete ui;
}

void SAKUdpAdvanceSettingWidget::setUdpDevice(SAKUdpDevice *device)
{
    if (device){
        udpDevice = device;
        udpDevice->setUnicastEnable(unicastCheckBox->isChecked());
        udpDevice->setMulticastEnable(multicastCheckBox->isChecked());
        udpDevice->setBroadcastEnable(broadcastCheckBox->isChecked());
    }
}

void SAKUdpAdvanceSettingWidget::on_unicastCheckBox_clicked()
{
    if (!isInitUi){
        bool enable = unicastCheckBox->isChecked();
        udpDevice->setUnicastEnable(enable);
    }
}

void SAKUdpAdvanceSettingWidget::on_broadcastCheckBox_clicked()
{
    if (!isInitUi){
        bool enable = broadcastCheckBox->isChecked();
        broadcastPortLineEdit->setEnabled(!enable);
        udpDevice->setBroadcastEnable(enable);
        udpDevice->setBroadcastPort(quint16(broadcastPortLineEdit->text().toInt()));
    }
}

void SAKUdpAdvanceSettingWidget::on_multicastCheckBox_clicked()
{
    if (!isInitUi){
        bool enable = multicastCheckBox->isChecked();
        udpDevice->setBroadcastEnable(enable);
    }
}

void SAKUdpAdvanceSettingWidget::on_deletePushButton_clicked()
{
    QListWidgetItem *item = listWidget->currentItem();
    if (!item){
        QMessageBox::warning(this, tr("未选择组播条目"), tr("请选择一条组播后重试"));
        return;
    }

    QString text = item->text();
    QString address = text.split(':').first();
    quint16 port = text.split(':').last().toUShort();
    udpDevice->removeMulticastInfo(address, port);
    delete item;
}

void SAKUdpAdvanceSettingWidget::on_addPushButton_clicked()
{
    SAKUdpMulticastEditingDialog dialog;
    dialog.show();
    int ret = dialog.exec();
    if (ret == QDialog::Accepted){
        QString address = dialog.address();
        quint16 port = dialog.port();
        udpDevice->addMulticastInfo(address, port);

        listWidget->addItem(QString("%1:%2").arg(address).arg(port));
    }
}
