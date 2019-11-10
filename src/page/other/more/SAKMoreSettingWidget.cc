﻿/*
 * Copyright (C) 2018-2019 wuuhii. All rights reserved.
 *
 * The file is encoding with utf-8 (with BOM). It is a part of QtSwissArmyKnife
 * project. The project is a open source project, you can get the source from:
 *     https://github.com/wuuhii/QtSwissArmyKnife
 *     https://gitee.com/wuuhii/QtSwissArmyKnife
 *
 * If you want to know more about the project, please join our QQ group(952218522).
 * In addition, the email address of the project author is wuuhii@outlook.com.
 */
#include "SAKDebugPage.hh"
#include "MoreOtherSettingsWidget.hh"
#include "ui_MoreOtherSettingsWidget.h"


MoreOtherSettingsWidget::MoreOtherSettingsWidget(SAKDebugPage *debugPage, QWidget *parent)
    :QWidget (parent)
    ,_debugPage (debugPage)
    ,ui (new Ui::MoreOtherSettingsWidget)
{
    ui->setupUi(this);
    setWindowTitle(tr("更多设置"));
}

MoreOtherSettingsWidget::~MoreOtherSettingsWidget()
{
    delete ui;
}