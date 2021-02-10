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

#ifndef CMPLRUNNER_H
#define CMPLRUNNER_H

#include <QObject>
#include <QThread>
#include <QProcess>
#include <QPlainTextEdit>
#include <QString>
#include <QDir>
#include <QDomDocument>
#include <QCoreApplication>


#include "ClpProblem.hh"

/**
  * Needed for the handling of CmplMessages
  */
typedef struct
{
    QString file;
    int line;
    QString type;
    QString description;
} cmplMessages;

/**
 * @brief The CmplRunner class - Runs Cmpl
 */
class CmplRunner : public QThread
{
    Q_OBJECT
public:

    explicit CmplRunner(QObject *parent = 0, QPlainTextEdit *clpOut=0, QPlainTextEdit *cmplSol=0, ClpProblem *problem=0, QString url="");

    ~CmplRunner();

    void run( ) Q_DECL_OVERRIDE ;
    void cancelProblem();

    QString getError();
    QString getGeneralStatus();
    QString getStatusMessage();
    QList<cmplMessages> getCmplMessages();
    int getNrOfMessages();
    bool isSolution();
    bool messagesAvailable();


private:
    QObject *_parent;

    QString _url;
    bool _isCanceled;
    bool _isFinished;

    QPlainTextEdit *_clpOut;
    QPlainTextEdit *_clpSol;
    ClpProblem *_problem;

    QString _cmplBinary;
    QString _cmplPath;
    QStringList  _cmplArgs;
    QProcess * _cmplHandler;

    QString _cmplErrorMessage;

    QList<cmplMessages> _cmplMessages;
    QString _generalStatus;
    QString _statusMessage;
    int _nrOfMessages;
    bool _solutionFound ;
    bool _msgFound;

    bool readCmplMessages();


signals:
    void CmplRunnerError();
    void CmplRunnerStarted();
    void CmplRunnerFinished();
    void CmplRunnerCanceled();


public slots:
    void writeCmplOutput();
    void getSolutionAndMessages();


};

#endif // CMPLRUNNER_H
