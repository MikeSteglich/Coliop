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

#include "CmplRunner.hh"
#include <QDebug>


CmplRunner::CmplRunner(QObject *parent, QPlainTextEdit *clpOut, QPlainTextEdit *clpSol, ClpProblem *problem, QString url) {

    _url=url;
    _clpOut=clpOut;
    _clpSol=clpSol;
    _problem=problem;

    _parent=parent;

    _isCanceled=false;
    _isFinished=false;

    QString runningOs = QSysInfo::productType();
    _cmplPath = QCoreApplication::applicationDirPath();

    if (_url.isEmpty()) {
        if (runningOs == "windows" ) {
            _cmplBinary = "cmpl.exe";
        } else {
            _cmplBinary = "cmpl";
        }
    } else {
        if (runningOs == "windows" ) {
            _cmplBinary = "cmplOnServer.bat";
        } else
            _cmplBinary = "cmplOnServer";
    }


    if (runningOs == "osx" or runningOs== "macos") {
        _cmplPath = _cmplPath+"/../../../";
    }

    _cmplBinary = _cmplPath+_cmplBinary;

    _generalStatus="";
    _statusMessage="";
    _nrOfMessages=0;
    _solutionFound = false;
    _msgFound = false;

    QString fileName=_problem->getProblemPath()+"/"+_problem->getProblemBaseName()+".sol";
    QFile file(fileName);
    if (file.exists()) {
        file.remove();
    }

    fileName=_problem->getProblemPath()+"/"+_problem->getProblemBaseName()+".cmsg";
    file.setFileName(fileName);
    if (file.exists()) {
        file.remove();
    }
}

CmplRunner::~CmplRunner()
{
    if (_cmplHandler->isOpen() ) {
        _cmplHandler->close();
    }
    delete _cmplHandler;
}


void CmplRunner::run()
{
    if (QDir::setCurrent(_problem->getProblemPath())) {

        _cmplArgs.append( _problem->getProblem() );

        if (!_url.isEmpty())
            _cmplArgs.append("-url="+_url);

        _cmplArgs.append("-solutionAscii");
        _cmplArgs.append("-cmsg");

        _cmplHandler = new QProcess();
        _cmplHandler->setProcessChannelMode(QProcess::MergedChannels);

        _cmplHandler->setProgram(_cmplBinary);
        _cmplHandler->setArguments(_cmplArgs);

        _cmplHandler->start();


        if (!_cmplHandler->waitForStarted()) {
            _generalStatus="internalError";
            _cmplErrorMessage="Cannot execute <"+_cmplBinary+">";
            emit CmplRunnerError( );
        } else  {
            emit CmplRunnerStarted();
            connect(_cmplHandler, SIGNAL(readyReadStandardOutput()), this, SLOT(writeCmplOutput()) );
            connect(_cmplHandler, SIGNAL(finished(int)), this , SLOT(getSolutionAndMessages()));
            _cmplHandler->waitForFinished(-1); }
    } else {
        _generalStatus="internalError";
        _cmplErrorMessage="Cannot access the folder <"+_problem->getProblemPath()+">";
        emit CmplRunnerError();
    }
}

void CmplRunner::cancelProblem()
{
    emit CmplRunnerCanceled();
    _isCanceled=true;
    _cmplHandler->kill();

}

QString CmplRunner::getError()
{
    return _cmplErrorMessage;
}

QString CmplRunner::getGeneralStatus()
{
    return _generalStatus;
}

QString CmplRunner::getStatusMessage()
{
    return _statusMessage;
}

QList<cmplMessages> CmplRunner::getCmplMessages()
{
    return _cmplMessages;
}

int CmplRunner::getNrOfMessages()
{
    return _nrOfMessages;
}

bool CmplRunner::isSolution()
{
    return _solutionFound ;
}
bool CmplRunner::messagesAvailable() {
    return _msgFound;
}

bool CmplRunner::readCmplMessages()
{
    QDomDocument doc("cmplMessages");

    QString fName = _problem->getProblemPath()+"/"+_problem->getProblemBaseName()+".cmsg";

    QFile file(fName);
    if (file.open(QIODevice::ReadOnly)) {

        if (doc.setContent(&file)) {

            QDomElement docElem = doc.documentElement();

            if ( docElem.tagName() == "CmplMessages") {

                QDomNode child1 = docElem.firstChild();
                while(!child1.isNull()) {

                    QDomElement elemChild1 = child1.toElement();
                    QDomNode child2 = child1.firstChild();

                    if ( elemChild1.tagName()=="general" ) {
                        while(!child2.isNull()) {
                            QDomElement elemChild2 = child2.toElement();

                            if (elemChild2.tagName()=="generalStatus") _generalStatus=elemChild2.text();
                            if (elemChild2.tagName()=="message") _statusMessage=elemChild2.text();
                            child2=child2.nextSibling();
                        }
                    }

                    if ( elemChild1.tagName()=="messages" ) {
                        _nrOfMessages=elemChild1.attribute("numberOfMessages").toInt();

                        while(!child2.isNull()) {
                            QDomElement elemChild2 = child2.toElement();

                            cmplMessages cmplMessage;
                            cmplMessage.type=elemChild2.attribute("type");

                            QString loc = elemChild2.attribute("location");

                            int pos1=loc.indexOf(":");
                            cmplMessage.file= loc.left(pos1);

                            QString lineStr=loc.mid(pos1+1);
                            int pos2=lineStr.indexOf(".");
                            cmplMessage.line=lineStr.leftRef(pos2).toInt();
                            //cmplMessage.line=lineStr.left(pos2).toInt();

                            cmplMessage.description=elemChild2.attribute("description");

                            _cmplMessages.append(cmplMessage);

                            child2=child2.nextSibling();
                        }
                    }
                    child1=child1.nextSibling();
                }
            } else {
                file.close();
                _generalStatus="internalError";
                _cmplErrorMessage="CmplMessages cannot be read - File is not a CmplMessages file";
                return false;
            }
        } else {
            file.close();
            _generalStatus="internalError";
            _cmplErrorMessage="CmplMessages cannot be read - XML errors";
            return false;
        }
        file.close();
        file.remove();
    }  else {
        _generalStatus="internalError";
        _cmplErrorMessage="Something went wrong - CmplMessages cannot be read - Please check outputs";
        return false;
    }
    return true;
}


void CmplRunner::getSolutionAndMessages()
{
    qDebug() << "get msg and sol ";
    if (!_isCanceled) {

        if (_problem->getAsyncMode()==0 || _problem->getAsyncMode()==3)  {
            _msgFound=readCmplMessages();
        }  else {
            _generalStatus="normal";
            _statusMessage="";
            _nrOfMessages=0;
            _solutionFound = false;
        }

        if ( (_problem->getAsyncMode()==0  || _problem->getAsyncMode()==3) && _msgFound ) {
            QString fileName ;
            fileName=_problem->getProblemPath()+"/"+_problem->getProblemBaseName()+".sol";

            if (!fileName.isEmpty()) {
                QFile file(fileName);
                if (file.open(QFile::ReadOnly | QFile::Text)) {
                    _clpSol->setPlainText(file.readAll());
                    file.close();
                    file.remove();
                    _solutionFound = true;
                } else {
                    _solutionFound = false;
                }
            }
        }

        emit CmplRunnerFinished();
        _isFinished=true;
    }
}


void CmplRunner::writeCmplOutput() {
    _clpOut->appendPlainText(QString(_cmplHandler->readAllStandardOutput()));
}
