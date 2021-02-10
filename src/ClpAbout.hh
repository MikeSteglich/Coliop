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

#ifndef CLPABOUT_H
#define CLPABOUT_H

#include <QDialog>
#include <QtGui>

namespace Ui {
    class clpAbout;
}

/**
 * @brief Colip about dialog class
 */
class ClpAbout : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent    The main window
     */
    explicit ClpAbout(QWidget *parent = 0);

    /**
     * @brief Destructor
     */
    ~ClpAbout();

private:
    Ui::clpAbout *ui;

private slots:
    /**
     * @brief Action on clickimg the OK button
     */
    void on_aboutOK_clicked();
};

#endif // CLPABOUT_H
