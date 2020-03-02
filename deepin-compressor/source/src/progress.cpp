/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     dongsen <dongsen@deepin.com>
 *
 * Maintainer: dongsen <dongsen@deepin.com>
 *             AaronZhang <ya.zhang@archermind.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "progress.h"

#include <QFileIconProvider>
#include <QVBoxLayout>
#include <QDebug>
#include <DDialog>
#include <QGraphicsDropShadowEffect>
#include "DFontSizeManager"
#include "utils.h"

Progress::Progress(QWidget *parent)
    : QWidget(parent)
{
    m_filename = "新建归档文件.rar";
    m_progressfile = "设计图111.jpg";
    InitUI();
    InitConnection();
}

void Progress::InitUI()
{
//    m_compressicon = Utils::renderSVG(":/images/Compression Packet.svg", QSize(128, 128));
    m_pixmaplabel = new DLabel(this);
    m_filenamelabel = new DLabel(this);
    m_filenamelabel->setText(m_filename);
    DFontSizeManager::instance()->bind(m_filenamelabel, DFontSizeManager::T5, QFont::DemiBold);
    m_filenamelabel->setForegroundRole(DPalette::ToolTipText);

    m_progressbar = new DProgressBar(this);
    m_progressbar->setRange(0, 100);
    m_progressbar->setFixedSize(240, 8);
    m_progressbar->setValue(0);
    m_progressbar->setOrientation(Qt::Horizontal);  //水平方向
    m_progressbar->setAlignment(Qt::AlignVCenter);
    m_progressbar->setTextVisible(false);
    m_shadow = new DLabel(this);
    m_shadow->setFixedSize(8, 1);
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
    effect->setOffset(0, 4);
    effect->setColor(QColor(44, 167, 248, 120));
    effect->setBlurRadius(6);
    m_shadow->setGraphicsEffect(effect);

    m_progressfilelabel = new DLabel(this);
    m_progressfilelabel->setMaximumWidth(520);
    m_progressfilelabel->setForegroundRole(DPalette::TextTips);
//    m_progressfilelabel->setFont(DFontSizeManager::instance()->get(DFontSizeManager::T8));
    DFontSizeManager::instance()->bind(m_progressfilelabel, DFontSizeManager::T8);
    m_progressfilelabel->setText(tr("Calculating..."));

    m_cancelbutton = new DPushButton(this);
    m_cancelbutton->setMinimumSize(340, 36);
    m_cancelbutton->setText(tr("Cancel"));
    m_cancelbutton->setFocusPolicy(Qt::ClickFocus);

    //add speed and time label
    m_speedlabel = new QLabel;
    m_speedlabel->setText(tr("Speed") + ": " + tr("Calculating..."));
    DFontSizeManager::instance()->bind(m_speedlabel, DFontSizeManager::T8);

    m_resttimelabel = new QLabel;
    m_resttimelabel->setText(tr("The rest time") + ": " + tr("Calculating..."));
    DFontSizeManager::instance()->bind(m_resttimelabel, DFontSizeManager::T8);

    QHBoxLayout *m_layout = new QHBoxLayout;
    m_layout->addStretch();
    m_layout->addWidget(m_speedlabel);
    m_layout->addSpacing(15);
    m_layout->addWidget(m_resttimelabel);
    m_layout->addStretch();


    QVBoxLayout *mainlayout = new QVBoxLayout(this);
    mainlayout->addStretch();
    mainlayout->addWidget(m_pixmaplabel, 0, Qt::AlignHCenter | Qt::AlignVCenter);
    mainlayout->addSpacing(5);
    mainlayout->addWidget(m_filenamelabel, 0, Qt::AlignHCenter | Qt::AlignVCenter);
    mainlayout->addSpacing(25);
    mainlayout->addWidget(m_progressbar, 0, Qt::AlignHCenter | Qt::AlignVCenter);
    mainlayout->addWidget(m_shadow, 0, Qt::AlignLeft | Qt::AlignTop);
    mainlayout->addSpacing(5);
    mainlayout->addWidget(m_progressfilelabel, 0, Qt::AlignHCenter | Qt::AlignVCenter);
    //mainlayout->addStretch();
    mainlayout->addSpacing(5);

    mainlayout->addLayout(m_layout);
    mainlayout->addStretch();

    QHBoxLayout *buttonHBoxLayout = new QHBoxLayout;
    buttonHBoxLayout->addStretch(1);
    buttonHBoxLayout->addWidget(m_cancelbutton, 2);
    buttonHBoxLayout->addStretch(1);

    mainlayout->addLayout(buttonHBoxLayout);
    mainlayout->setContentsMargins(12, 6, 20, 20);

    setBackgroundRole(DPalette::Base);
}

void Progress::InitConnection()
{
    connect(m_cancelbutton, &DPushButton::clicked, this, &Progress::cancelbuttonPressedSlot);
}

void Progress::setprogress(uint percent)
{
    m_progressbar->setValue(percent);
    m_progressbar->update();
    isStart = true;
}

//add speed and time
void Progress::setspeed(double speed)
{
    m_speed = speed;
}

void Progress::setresttime(double resttime)
{
    m_resttime = resttime;
    int hour = m_resttime / 3600;
    int minute = (m_resttime - hour *3600) / 60;
    int seconds = m_resttime - hour * 3600 - minute * 60;

    hh = QString("%1").arg(hour, 2, 10, QLatin1Char('0'));
    mm = QString("%1").arg(minute, 2, 10, QLatin1Char('0'));
    ss = QString("%1").arg(seconds, 2, 10, QLatin1Char('0'));
}

void Progress::setFilename(QString filename)
{
    QFileInfo fileinfo(filename);
    setTypeImage(fileinfo.completeSuffix());
    m_filenamelabel->setText(filename);
}

void Progress::setTypeImage(QString type)
{
    QFileIconProvider provider;
    QIcon icon = provider.icon(QFileInfo("temp."+type));
    m_pixmaplabel->setPixmap(icon.pixmap(128, 128));
}

void Progress::setProgressFilename(QString filename)
{
    if(filename.isEmpty())
    {
        return;
    }

    QFontMetrics elideFont(m_progressfilelabel->font());
    if (m_type == COMPRESSING) {
        m_progressfilelabel->setText(elideFont.elidedText(tr("Compressing") + ": " + filename, Qt::ElideMiddle, 520));

        //add update speed and time label
        if (isStart)
        {
            if (m_speed < 1024) {
                m_speedlabel->setText(tr("Compression speed") + ": " + QString::number(m_speed, 'f', 2) + "KB/S");
            } else if (m_speed > 1024 && m_speed < 1024 * 300) {
                m_speedlabel->setText(tr("Compression speed") + ": " + QString::number((m_speed / 1024), 'f', 2) + "MB/S");
            } else {
                m_speedlabel->setText(tr("Compression speed") + ": " + ">300MB/S");
            }
            m_resttimelabel->setText(tr("The rest time") + ": " + hh + ":" + mm + ":" + ss);
        }

    } else {
        m_progressfilelabel->setText(elideFont.elidedText(tr("Extracting") + ": " + filename, Qt::ElideMiddle, 520));
        if (isStart)
        {
            if (m_speed < 1024) {
                m_speedlabel->setText(tr("Decompression speed") + ": " + QString::number(m_speed, 'f', 2) + "KB/S");
            } else if (m_speed > 1024 && m_speed < 1024 * 300) {
                m_speedlabel->setText(tr("Decompression speed") + ": " + QString::number((m_speed / 1024), 'f', 2) + "MB/S");
            } else {
                m_speedlabel->setText(tr("Decompression speed") + ": " + ">300MB/S");
            }
            m_resttimelabel->setText(tr("The rest time") + ": " + hh + ":" + mm + ":" + ss);
        }

    }
}

void Progress::settype(COMPRESS_TYPE type)
{
    m_type = type;
}

int Progress::showConfirmDialog()
{
    DDialog *dialog = new DDialog(this);

    QPixmap pixmap = Utils::renderSVG(":/icons/deepin/builtin/icons/compress_warning_32px.svg", QSize(32, 32));
    dialog->setIcon(pixmap);
    DPalette pa, pa2;
    DLabel *strlabel = new DLabel(dialog);
    strlabel->setFixedHeight(20);
    strlabel->setForegroundRole(DPalette::WindowText);

    DFontSizeManager::instance()->bind(strlabel, DFontSizeManager::T6, QFont::Medium);
    DLabel *strlabel2 = new DLabel(dialog);
    strlabel2->setFixedHeight(18);
    strlabel2->setForegroundRole(DPalette::WindowText);

    DFontSizeManager::instance()->bind(strlabel2, DFontSizeManager::T7, QFont::Medium);

    if (m_type == COMPRESSING)
    {
        //strlabel->setText(tr("Stop compressing "));
        strlabel2->setText(tr("Are you sure you want to stop the compression?"));
    } else
    {
        //strlabel->setText(tr("Stop extracting "));
        strlabel2->setText(tr("Are you sure you want to stop the extraction?"));
    }

    dialog->addButton(tr("Cancel"));
    dialog->addButton(tr("Confirm"), true, DDialog::ButtonRecommend);
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(dialog);
    effect->setOffset(0, 4);
    effect->setColor(QColor(0, 145, 255, 76));
    effect->setBlurRadius(4);
    dialog->getButton(1)->setGraphicsEffect(effect);

    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->setContentsMargins(0, 0, 0, 0);
    //mainlayout->addWidget(strlabel, 0, Qt::AlignHCenter | Qt::AlignVCenter);
    mainlayout->addWidget(strlabel2, 0, Qt::AlignHCenter | Qt::AlignVCenter);
    mainlayout->addSpacing(15);

    DWidget *widget = new DWidget(dialog);

    widget->setLayout(mainlayout);
    dialog->addContent(widget);
    int res = dialog->exec();
    delete dialog;

    return res;
}

void Progress::resetProgress()
{
    m_progressfilelabel->setText(tr("Calculating..."));
}

void Progress::cancelbuttonPressedSlot()
{
    if (1 == showConfirmDialog())
    {
        emit sigCancelPressed(m_type);
    }
}
