﻿/***********************************************************************
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

#ifndef CLPPROBLEM_H
#define CLPPROBLEM_H

#include <QString>
#include <QStringList>
#include <QFileInfo>

class ClpProblem
{
public:
    ClpProblem();

    QString getProblemName();
    QString getProblemBaseName();
    QString getProblem();
    QString getProblemPath();
    QString getCmplListElement(int idx);
    QStringList getCmplList();

    void setDefaults();
    void setProblem(QString &prob);

    int getActiveCmplIndex();
    void setActiveCmplIndex(int idx);

    int cmplListLength();
    void addCmplList(QString name);
    void delCmplListItem(int idx);
    void clearCmplList();

    bool isProblemChanged() const;
    void setProblemChanged(bool isChanged);

    void setAsyncMode(int mode);
    int getAsyncMode();

private:
    QString _problemName;
    QString _problemBaseName;
    QString _problem;
    QString _problemPath;

    int _activeCmplIndex;
    QStringList _cmplList;

    bool _isChanged;
    bool _isSolved;

    int _asyncMode;
};

#endif // CLPPROBLEM_H
