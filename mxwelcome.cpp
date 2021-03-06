/**********************************************************************
 *  mxwelcome.cpp
 **********************************************************************
 * Copyright (C) 2015 MX Authors
 *
 * Authors: Adrian
 *          Paul David Callahan
 *          Dolphin Oracle
 *          MX Linux <http://mxlinux.org>
 *
 * This file is part of mx-welcome.
 *
 * mx-welcome is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * mx-welcome is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with mx-welcome.  If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/


#include "mxwelcome.h"
#include "ui_mxwelcome.h"
#include "flatbutton.h"

#include <QDebug>

mxwelcome::mxwelcome(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mxwelcome)
{
    ui->setupUi(this);
    setup();
}

mxwelcome::~mxwelcome()
{
    delete ui;
}

// setup versious items first time program runs
void mxwelcome::setup()
{
    version = getVersion("mx-welcome");
    this->setWindowTitle(tr("MX Welcome"));
    system("rm ~/.config/autostart/mx-welcome.desktop >/dev/null 2>&1");
    // if running live
    QString test = runCmd("df -T / |tail -n1 |awk '{print $2}'").output;
    qDebug() << test;
    if ( test == "aufs" || test == "overlay" ) {
        ui->checkBox->hide();
    } else {
        ui->buttonLogininfo->hide();
    }
    this->adjustSize();
}

// Util function for getting bash command output and error code
Result mxwelcome::runCmd(QString cmd)
{
    QEventLoop loop;
    proc = new QProcess(this);
    proc->setReadChannelMode(QProcess::MergedChannels);
    connect(proc, SIGNAL(finished(int)), &loop, SLOT(quit()));
    proc->start("/bin/bash", QStringList() << "-c" << cmd);
    loop.exec();
    disconnectAll();
    Result result = {proc->exitCode(), proc->readAll().trimmed()};
    delete proc;
    return result;
}

// disconnect all connections
void mxwelcome::disconnectAll()
{
    disconnect(proc, SIGNAL(started()), 0, 0);
    disconnect(proc, SIGNAL(finished(int)), 0, 0);
}

// Get version of the program
QString mxwelcome::getVersion(QString name)
{
    QString cmd = QString("dpkg -l %1 | awk 'NR==6 {print $3}'").arg(name);
    return runCmd(cmd).output;
}

// set proc and timer connections
void mxwelcome::setConnections()
{
    connect(proc, SIGNAL(started()), SLOT(procStart()));
    connect(proc, SIGNAL(finished(int)), SLOT(procDone(int)));
}

//// slots ////

// About button clicked
void mxwelcome::on_buttonAbout_clicked()
{
    this->hide();
    QMessageBox msgBox(QMessageBox::NoIcon,
                       tr("About MX Welcome"), "<p align=\"center\"><b><h2>" +
                       tr("MX Welcome") + "</h2></b></p><p align=\"center\">" + tr("Version: ") + version + "</p><p align=\"center\"><h3>" +
                       tr("Program for displaying a welcome screen in MX Linux") +
                       "</h3></p><p align=\"center\"><a href=\"http://www.mxlinux.org/mx\">http://www.mxlinux.org/mx</a><br /></p><p align=\"center\">" +
                       tr("Copyright (c) MX Linux") + "<br /><br /></p>", 0, this);
    msgBox.addButton(tr("License"), QMessageBox::AcceptRole);
    msgBox.addButton(tr("Cancel"), QMessageBox::NoRole);
    if (msgBox.exec() == QMessageBox::AcceptRole) {
        QString cmd = QString("mx-viewer http://mxlinux.org/wiki/licenses/license-mx-welcome '%1'").arg(tr("MX Welcome"));
        system(cmd.toUtf8());
    }
    this->show();
}

// Add/remove autostart at login
void mxwelcome::on_checkBox_clicked(bool checked)
{
    if (checked) {
        system("cp /usr/share/mx-welcome/mx-welcome.desktop ~/.config/autostart/mx-welcome.desktop");
    } else {
        system("rm ~/.config/autostart/mx-welcome.desktop >/dev/null 2>&1");
    }
}

// Start MX-Tools
void mxwelcome::on_buttonTools_clicked()
{
    this->hide();
    system("mx-tools");
    this->show();
}

// Launch Manual in browser
void mxwelcome::on_buttonManual_clicked()
{
    this->hide();
    system("mx-viewer /usr/local/share/doc/mxum.html");
    this->show();
}

// Launch Forum in browser
void mxwelcome::on_buttonForum_clicked()
{
    this->hide();
    system("mx-viewer http://forum.mxlinux.org/index.php");
    this->show();
}

// Launch Wiki in browser
void mxwelcome::on_buttonWiki_clicked()
{
    this->hide();
    system("mx-viewer http://www.mxlinux.org/wiki");
    this->show();
}

// Launch Video links in browser
void mxwelcome::on_buttonVideo_clicked()
{
    system("exo-open --launch WebBrowser http://www.mxlinux.org/videos/");
}

// Launch Contribution page
void mxwelcome::on_buttonContribute_clicked()
{
    this->hide();
    system("mx-viewer http://www.mxlinux.org/donate");
    this->show();
}

// Launch Help in browser
void mxwelcome::on_buttonLogininfo_clicked()
{
    this->hide();
    system("mx-viewer file:///usr/local/share/doc/mxum.html#toc-Subsection-2.4");
    this->show();
}

void mxwelcome::on_buttonPanelOrient_clicked()
{
    this->hide();
    system("mx-defaultlook");
    this->show();
}

void mxwelcome::on_buttonPackageInstall_clicked()
{
    this->hide();
    system("su-to-root -X -c mx-packageinstaller");
    this->show();
}
