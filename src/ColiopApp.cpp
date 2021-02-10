/***********************************************************************
 *  This code is part of Coliop-IDE
 *
 *  Copyright (C) Mike Steglich
 *  Technical University of Applied Sciences
 *  Wildau, Germany
 *
 *  Coliop is a project of the Technical University of
 *  Applied Sciences Wildau.
 *  Please visit the project homepage <www.coliop.org>
 *
 *  Coliop is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Coliop is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "ColiopApp.hh"
#include "ColiopUi.hh"

#include <QDesktopWidget>
#include <QFileOpenEvent>

/**
 * @brief Constructor
 * @param argc
 * @param argv
 */
ColiopApp::ColiopApp(int &argc, char **argv)  : QApplication(argc, argv)
{
    QStringList args =  QGuiApplication::arguments();

    w.show();
    QRect rec = QGuiApplication::primaryScreen()->geometry();

    if (rec.width()<1200 || rec.height()<750)
        w.showMaximized();


    if (argc>1 && args.at(1).toLower().endsWith(".cmpl")) {
        QString fName= args.at(1);
        w.openFirstProblem(fName);
    }

}


/**
 * @brief Handels opening automatically a Cmpl problem with Coliop
 * @param event
 * @return      True or falase
 */
bool ColiopApp::event(QEvent *event)
{
    if (event->type() == QEvent::FileOpen) {
        QFileOpenEvent *openEvent = static_cast<QFileOpenEvent *>(event);
        fName=openEvent->file();
        w.openFirstProblem(fName);
    }
    return QApplication::event(event);
}
