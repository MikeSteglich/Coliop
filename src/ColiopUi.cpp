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

#include "ColiopUi.hh"
#include "ui_coliopui.h"

/**
 * @brief ColiopUi - Constructor
 * @param parent
 */
ColiopUi::ColiopUi(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::coliopUi)
{
    ui->setupUi(this);

    problem = new ClpProblem();
    coliopAbout = new ClpAbout();

    _cmplTmpFileName=QDir::homePath()+QDir::separator()+".cmpl";
    if (!readCmplTmp()) {
        _cmplUrl="http://";
        _cmplCwd="";
        _editorFontSize=-1;
    }
    _isRemote=false;

    _runningOs = QSysInfo::productType();
    _cmplPath = QCoreApplication::applicationDirPath();
    if (_runningOs == "osx" or _runningOs== "macos") {
        _cmplPath = _cmplPath+"/../../../";
    }

    if (!_cmplPath.endsWith(QDir::separator())) {
        _cmplPath+=QDir::separator();
    }

    if (_runningOs!="osx" and _runningOs!="windows"){
        ui->actionOpen_CmplShell->setVisible(false);
    }

    ui->actionSolve_on_server->setVisible(false);

    setupFonts();
    problem->setProblemChanged(false);

    ui->cmplList->hide();

    connect(ui->actionNew_Coliop4_Window,  SIGNAL(triggered()), this, SLOT(on_actionNewColiopWindow())) ;

    connect(ui->actionNew_Problem,  SIGNAL(triggered()), this, SLOT(on_actionNew_Problem()) );
    connect(ui->actionOpen_Problem, SIGNAL(triggered()), this, SLOT(on_actionOpen_Problem()) );
    connect(ui->actionSave_Problem, SIGNAL(triggered()), this, SLOT(on_actionSave_Problem()) );
    connect(ui->actionSave_Problem_as, SIGNAL(triggered()), this, SLOT(on_actionSave_Problem_as()) );
    connect(ui->actionPrint, SIGNAL(triggered()), this, SLOT(on_actionPrint()) );
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(on_actionAbout()) );
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(on_actionQuit()) );

    connect(ui->cmplEditor, SIGNAL(textChanged()), this, SLOT(on_cmplEditorChanged()));


    connect(ui->actionPaste, SIGNAL(triggered()), this, SLOT(on_actionPaste()) );
    connect(ui->actionCut, SIGNAL(triggered()), this, SLOT(on_actionCut()) );

    connect(ui->actionCopy, SIGNAL(triggered()), this, SLOT(on_actionCopy()) );

    connect(ui->actionRedo, SIGNAL(triggered()), this, SLOT(on_actionRedo()) );
    connect(ui->actionUndo, SIGNAL(triggered()), this, SLOT(on_actionUndo()) );

    connect(ui->actionZoom_out, SIGNAL(triggered()), this, SLOT(on_actionZoom_out()) );
    connect(ui->actionZoom_in,  SIGNAL(triggered()), this, SLOT(on_actionZoom_in()) );
    //connect(ui->actionGo_to_line,SIGNAL(triggered()), this, SLOT(on_actionGo_to_line_triggered()) );

    connect(ui->cmplList,&QListWidget::itemClicked, this, &ColiopUi::on_cmplListClicked );
    connect(ui->cmplMessages,&QListWidget::itemClicked, this, &ColiopUi::on_cmplMessageListClicked );
    connect(ui->clpProblemTab, &QTabWidget::currentChanged, this, &ColiopUi::on_clpProblemTab_currentChanged);


    connect(ui->actionSolve, SIGNAL(triggered()), this, SLOT(on_actionSolve()));
    connect(ui->actionSolve_on_server, SIGNAL(triggered()), this, SLOT(on_actionSolve_on_server()) );
    connect(ui->actionCancel,SIGNAL(triggered()), this, SLOT(on_actionCancel()));

    connect(ui->actionCMPL_Manual, SIGNAL(triggered()), this, SLOT(on_cmplHelp()));

    enableSaveActions(false);
    enableEditorActions(true);

    ui->actionCancel->setEnabled(false);
    ui->actionSolve->setEnabled(false);
    ui->actionSolve_on_server->setEnabled(false);
    ui->clpMessageTab->setVisible(false);

}

/**
 * @brief Constructor
 * @param parent
 */
ColiopUi::~ColiopUi()
{
    saveCmplTmp();
    delete coliopAbout;
    delete problem;
    delete ui;

}

/**
 * @brief Initial opening of a problem - used for automatical opening a problem by doupple clicking in Explorer/Finder/etc.
 * @param fileName      file name of the problem
 */
void ColiopUi::openFirstProblem(QString &fileName)
{
    problem->setProblem(fileName);
    openProblem(fileName);

    problem->setProblemChanged(false);
    enableSaveActions(false);
    ui->cmplMessages->clear();
    ui->cmplOutPut->setPlainText("");
    ui->cmplSolution->setPlainText("");
    ui->clpMessageTab->setVisible(false);

    problem->setActiveCmplIndex(0);

    setWindowTitle("Coliop - "+problem->getProblemName());
    ui->clpProblemTab->setCurrentIndex(0);
    ui->actionSolve->setEnabled(true);
    ui->actionSolve_on_server->setEnabled(true);
}

/**
 * @brief initial setup of the fonts
 */
void ColiopUi::setupFonts()
{
    QFont font;
    QStringList substFonts( QFont::substitutes("Monaco"));

    font.setFamily("Monaco");
    font.setFixedPitch(true);

    if (_editorFontSize!=-1) {
        font.setPointSize(_editorFontSize);
    }

    ui->cmplEditor->setFont(font);
    cmplSyntaxHighlighter = new CmplHighlighter(ui->cmplEditor->document());
    ui->cmplList->setFont(font);
    ui->cmplOutPut->setFont(font);
    ui->cmplSolution->setFont(font);
    ui->cmplMessages->setFont(font);

}


/**
 * @brief Opens a new Coliop window
 */
void ColiopUi::on_actionNewColiopWindow() {

    QString runProg;

    if (_runningOs == "windows" ) {
         runProg = "Coliop.exe";
    } else if (_runningOs == "osx" or _runningOs== "macos") {
        runProg = "Coliop.app";
    } else {
        runProg = "../Coliop";
    }

    runProg = _cmplPath+runProg;
    _cmplProcessHandler = new QProcess(this);
    _cmplProcessHandler->setProgram(runProg);
    _cmplProcessHandler->start();

    if (!_cmplProcessHandler->waitForStarted()) {
         clpWarning("ERROR", "Could not open a new Coliop window <"+runProg+">") ;
    }
}

/**
 * @brief creates a new problem
 */
void ColiopUi::on_actionNew_Problem()
{
    ui->cmplEditor->clear();
    ui->cmplEditor->setPlainText("par:\n\nvar:\n\nobj:\n\ncon:\n\n");
    problem->setDefaults();

    ui->actionSolve->setEnabled(false);
    ui->actionSolve_on_server->setEnabled(false);

    problem->setProblemChanged(true);
    enableSaveActions(true);
    ui->cmplMessages->clear();
    ui->cmplOutPut->setPlainText("");
    ui->cmplSolution->setPlainText("");
    ui->clpMessageTab->setVisible(false);
    problem->setActiveCmplIndex(0);

    setWindowTitle("Coliop - New problem");
}

/**
 * @brief Runs optimisation
 */
void ColiopUi::on_actionSolve()
{
    if (problem->isProblemChanged()) {
        QString sName = problem->getProblemPath()+"/"+problem->getCmplListElement(problem->getActiveCmplIndex());
        if (clpAnswerMessage("Save file", "File <"+sName+"> is changed. \n\nDo you want to save it?") == true) {
            saveProblem(sName);
          }
    }

    QString url="";
    if (_isRemote)
        url=_cmplUrl;

    checkAsyncMode();
    //qDebug() << "asyncMode" << problem->getAsyncMode() ;

    _cmplHandler = new CmplRunner(this, ui->cmplOutPut, ui->cmplSolution, problem, url);

    connect(_cmplHandler, &CmplRunner::CmplRunnerStarted, this, &ColiopUi::on_cmplStarted);
    connect(_cmplHandler, &CmplRunner::CmplRunnerFinished, this, &ColiopUi::on_cmplFinished);
    connect(_cmplHandler, &CmplRunner::CmplRunnerError, this, &ColiopUi::on_cmplError);

    _cmplHandler->start();

    ui->cmplOutPut->setPlainText("");
    ui->cmplSolution->setPlainText("");
    ui->clpProblemTab->setCurrentIndex(1);
    ui->cmplMessages->clear();
    _cmplMessages.clear();
    ui->clpMessageTab->setVisible(false);
}



void ColiopUi::on_actionSolve_on_server()
{
    bool ok;
    QString url = QInputDialog::getText(this, "CmplServerUrl",
                                         "CmplServerUrl:", QLineEdit::Normal,
                                         _cmplUrl, &ok);
    if (ok && !url.isEmpty()) {
        _cmplUrl=url;
        _isRemote=true;
        on_actionSolve();
    } else {
        _isRemote=false;
    }
}



/**
 * @brief Cancels optimisation
 */
void ColiopUi::on_actionCancel()
{
    _cmplHandler->cancelProblem();
    qDebug() << "### isrunning after cancelProblem";

    ui->actionSolve->setEnabled(true);
    ui->actionSolve_on_server->setEnabled(true);
    ui->actionCancel->setEnabled(false);
    ui->cmplOutPut->appendPlainText("\nCmpl has been canceled");
    ui->cmplMessages->addItem("Cmpl has been canceled");

    _isRemote=false;
}


/**
 * @brief actions after finishing optimisation
 */
void ColiopUi::on_cmplFinished()
{
    ui->actionSolve->setEnabled(true);
    ui->actionSolve_on_server->setEnabled(true);
    ui->actionCancel->setEnabled(false);

    if (_cmplHandler->getGeneralStatus()=="internalError") {
        ui->clpMessageTab->setVisible(true);
        QList <int> horizontalSplitterSize;
        horizontalSplitterSize.append(round(0.7*ui->clpUiSplitter->height()));
        horizontalSplitterSize.append(ui->clpUiSplitter->height()-round(0.7*ui->clpUiSplitter->height()));
        ui->clpUiSplitter->setSizes(horizontalSplitterSize);

        ui->clpProblemTab->setCurrentIndex(0);
        ui->cmplMessages->addItem(_cmplHandler->getError());

    } else if (_cmplHandler->getGeneralStatus()=="normal")  {
        if (problem->getAsyncMode()==0 || problem->getAsyncMode()==3)  {
            if (!_cmplHandler->isSolution()  )  {
                ui->clpProblemTab->setCurrentIndex(1);
            } else {
                ui->clpProblemTab->setCurrentIndex(2);
            }
        }
    } else if (_cmplHandler->getGeneralStatus()=="error") {
        ui->clpMessageTab->setVisible(true);
        QList <int> horizontalSplitterSize;
        horizontalSplitterSize.append(round(0.7*ui->clpUiSplitter->height()));
        horizontalSplitterSize.append(ui->clpUiSplitter->height()-round(0.7*ui->clpUiSplitter->height()));
        ui->clpUiSplitter->setSizes(horizontalSplitterSize);

        ui->clpProblemTab->setCurrentIndex(0);
        ui->cmplMessages->addItem(_cmplHandler->getStatusMessage());

        if (_cmplHandler->getNrOfMessages()>0) {
            _cmplMessages=_cmplHandler->getCmplMessages();
        }
        ui->cmplMessages->addItem("Double-click an error message to access it in your code.");
        for (int i=0; i<_cmplMessages.length(); i++) {
            ui->cmplMessages->addItem(_cmplMessages.at(i).type+" in file <"+_cmplMessages.at(i).file+"> at line "+QString::number(_cmplMessages.at(i).line)+" : "+_cmplMessages.at(i).description);
        }
    }

    _isRemote=false;
    delete _cmplHandler;
}

/**
 * @brief Handles Cmpl's error messages
 */
void ColiopUi::on_cmplError() {

    qDebug() << "### running error ";

    ui->actionSolve->setEnabled(true);
    ui->actionSolve_on_server->setEnabled(true);
    ui->actionCancel->setEnabled(false);
    ui->cmplOutPut->appendPlainText(  _cmplHandler->getGeneralStatus() );
    ui->cmplMessages->addItem( _cmplHandler->getError() );
    ui->clpMessageTab->setVisible(true);

    delete _cmplHandler;

}



/**
 * @brief Some UI behaviour after starting optimisation
 */
void ColiopUi::on_cmplStarted()
{
    ui->actionSolve->setEnabled(false);
    ui->actionSolve_on_server->setEnabled(false);
    ui->actionCancel->setEnabled(true);
    ui->clpProblemTab->setCurrentIndex(1);
}

/**
 * @brief Some UI behaviour after canceling optimisation
 */
void ColiopUi::on_cmplCanceled()
{
    qDebug() << "### isrunning in cmplCanceled";
    delete _cmplHandler;
    ui->actionSolve->setEnabled(true);
    ui->actionSolve_on_server->setEnabled(true);
    ui->actionCancel->setEnabled(false);
    ui->cmplOutPut->appendPlainText("\nCmpl has been canceled");
    ui->cmplMessages->addItem("Cmpl has been canceled");
}

/**
 * @brief Opens the manual
 */
void ColiopUi::on_cmplHelp()
{

    QString runProg;

    if (_runningOs == "windows" ) {
        runProg = "manual.bat";
    } else  {
        runProg = "manual";
    }

    runProg = _cmplPath+runProg;
    _cmplProcessHandler = new QProcess(this);
    _cmplProcessHandler->setProgram(runProg);
    _cmplProcessHandler->start();

    if (!_cmplProcessHandler->waitForStarted()) {
        clpWarning("ERROR", "Cannot find manual handler <"+runProg+">") ;
    }
}


/**
 * @brief Opens a problem
 */
void ColiopUi::on_actionOpen_Problem()
{
    QString fileName ;

    fileName = QFileDialog::getOpenFileName(this,  "Open File", _cmplCwd , "CMPL Files (*.cmpl )");

    if (!fileName.isEmpty()) {
        problem->setActiveCmplIndex(0);
        ui->clpProblemTab->setCurrentIndex(0);

        problem->setProblem(fileName);
        openProblem(fileName);

        problem->setProblemChanged(false);
        enableSaveActions(false);
        ui->cmplMessages->clear();
        ui->cmplOutPut->setPlainText("");
        ui->cmplSolution->setPlainText("");
        ui->clpMessageTab->setVisible(false);

        setWindowTitle("Coliop - "+problem->getProblemName());

        ui->actionSolve->setEnabled(true);
        ui->actionSolve_on_server->setEnabled(true);

        _cmplCwd=QFileInfo(fileName).absolutePath();
    }
}

/**
 * @brief opens a problem with given file name
 * @param fileName  file name
 */
void ColiopUi::openProblem( QString &fileName ){

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QFile::ReadOnly | QFile::Text)) {

            ui->cmplEditor->setPlainText(file.readAll());
            file.close();

            problem->setProblemChanged(false);
            enableEditorActions(true);
            enableSaveActions(false);

           } else {
           clpWarning("ERROR", "Cannot find file <"+fileName+">") ;
        }
    }
}

/**
 * @brief saves problem with given file name
 * @param fileName  file name
 * @return true - if files was saved succesfully
 */
bool ColiopUi::saveProblem(QString &fileName) {
    QFile cmplFile (fileName);
    QTextStream cmplS(&cmplFile);
    if (!cmplFile.open(QIODevice::Truncate | QIODevice::Text | QIODevice::ReadWrite)) {
            clpWarning("ERROR", "Cannot save file <"+fileName+">") ;
            return false;
    }
    cmplS << ui->cmplEditor->toPlainText();
    cmplFile.close();
    problem->setProblemChanged(false);
    enableSaveActions(false);
    return true;
}


/**
 * @brief editor action -  jumps to a selcted line in the editor
 * @param lineNumber    line number
 */
void ColiopUi::gotoLine(int lineNumber) {

      const QTextBlock &block = ui->cmplEditor->document()->findBlockByNumber(lineNumber-1);
      QTextCursor cursor(block);
      cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, 0);
      cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor );
      ui->cmplEditor->setTextCursor(cursor);
      ui->cmplEditor->centerCursor();
      ui->cmplEditor->setFocus();

}


/**
 * @brief Opens the about dialog
 */
void ColiopUi::on_actionAbout()
{
    coliopAbout->show();
}


/**
 * @brief Final activities while closing Coliop
 */
void ColiopUi::on_actionQuit()
{
    QString sName;

    if (problem->isProblemChanged() && !ui->cmplEditor->toPlainText().trimmed().isEmpty()) {
        if (problem->getProblemName().isEmpty()) {
            if (clpAnswerMessage("Save file", "New Problem is changed. \n\nDo you want to save it?") == true) {
               sName  = QFileDialog::getSaveFileName(this,  "Save File", "", "CMPL or CMPLData Files (*.cmpl *.cdat )");
               if (!sName.isEmpty()) {
                    saveProblem( sName );
               }
            }
        } else {
            sName = problem->getProblemPath()+"/"+problem->getCmplListElement(problem->getActiveCmplIndex());
            if (clpAnswerMessage("Save file", "File <"+sName+"> is changed. \n\nDo you want to save it?") == true) {
                saveProblem(sName);
            }
        }
    }
    this->close();
}

/**
 * @brief Action Save_Problem
 */
void ColiopUi::on_actionSave_Problem()
{   QString fn;
    if (problem->getProblemName().isEmpty()) {
        fn  = QFileDialog::getSaveFileName(this,  "Save File", "", "CMPL or CMPLData Files (*.cmpl *.cdat )");
    } else {
        fn = problem->getProblemPath()+"/"+problem->getCmplListElement(problem->getActiveCmplIndex());
    }
    if (!fn.isEmpty()) {
        saveProblem( fn );
        if (problem->getActiveCmplIndex()==0) {
            problem->setProblem(fn);
            setWindowTitle("Coliop - "+problem->getProblemName());
            ui->clpProblemTab->setCurrentIndex(0);
            ui->actionSolve->setEnabled(true);
            ui->actionSolve_on_server->setEnabled(true);
        }
        _cmplCwd=QFileInfo(fn).absolutePath();

    }
}


/**
 * @brief Action Save_Problem
 */
void ColiopUi::on_actionSave_Problem_as()
{   QString fn  = QFileDialog::getSaveFileName(this,  "Save File", "", "CMPL or CMPLData Files (*.cmpl *.cdat )");
    if (!fn.isEmpty()) {
        saveProblem( fn );
        if (problem->getActiveCmplIndex()==0) {
            problem->setProblem(fn);
            setWindowTitle("Coliop - "+problem->getProblemName());
            ui->clpProblemTab->setCurrentIndex(0);
            ui->actionSolve->setEnabled(true);
            cmplList();
        }
        _cmplCwd=QFileInfo(fn).absolutePath();



    }
}



void ColiopUi::on_actionGo_to_line_triggered()
{
    QTextCursor cursor = ui->cmplEditor->textCursor();
    int lineNumber = cursor.blockNumber();
    int blockCount = ui->cmplEditor->blockCount();
    bool ok;
    int line = QInputDialog::getInt(this,"Line number", "Enter a line number"  , lineNumber,1,blockCount ,1,&ok);
    if (ok)
        gotoLine(line);
}



/**
 * @brief Zooms in editor/messages/solution
 */
void ColiopUi::on_actionZoom_in()
{
    int curPointSize=ui->cmplEditor->fontInfo().pointSize();
    QString curFontFamily = ui->cmplEditor->fontInfo().family();
    QFont font;
    font.setPointSize(curPointSize+1);
    font.setFamily(curFontFamily);
    ui->cmplEditor->setFont(font);
    ui->cmplOutPut->setFont(font);
    ui->cmplSolution->setFont(font);
    ui->cmplMessages->setFont(font);
    ui->cmplList->setFont(font);

}


/**
* @brief Zooms out editor/messages/solution
*/
void ColiopUi::on_actionZoom_out()
{
    int curPointSize=ui->cmplEditor->fontInfo().pointSize();
    QString curFontFamily = ui->cmplEditor->fontInfo().family();
    QFont font;
    font.setPointSize(curPointSize-1);
    font.setFamily(curFontFamily);
    ui->cmplEditor->setFont(font);
    ui->cmplOutPut->setFont(font);
    ui->cmplSolution->setFont(font);
    ui->cmplMessages->setFont(font);
    ui->cmplList->setFont(font);

}


/**
 * @brief enables all action to edit a problem - used while tunning a problem
 * @param enable    true or false
 */
void ColiopUi::enableEditorActions(bool enable) {

    if (enable) {
        ui->actionCopy->setEnabled(true);
        ui->actionCut->setEnabled(true);
        ui->actionPaste->setEnabled(true);
        ui->actionUndo->setEnabled(true);
        ui->actionRedo->setEnabled(true);
        ui->actionGo_to_line->setEnabled(true);
        if (!ui->cmplEditor->toPlainText().isEmpty()) {
            ui->actionPrint->setEnabled(true);
        } else {
            ui->actionPrint->setEnabled(false);
        }

    } else {
        ui->actionCopy->setDisabled(false);
        ui->actionCut->setDisabled(true);
        ui->actionPaste->setDisabled(true);
        ui->actionUndo->setDisabled(true);
        ui->actionRedo->setDisabled(true);
        ui->actionGo_to_line->setDisabled(true);

        if ( ui->clpProblemTab->currentIndex()==1) {
            if (!ui->cmplOutPut->toPlainText().isEmpty()) {
                ui->actionPrint->setEnabled(true);
            } else {
                ui->actionPrint->setEnabled(false);
            }
        } else  if ( ui->clpProblemTab->currentIndex()==2) {
            if (!ui->cmplSolution->toPlainText().isEmpty()) {
                ui->actionPrint->setEnabled(true);
            } else {
                ui->actionPrint->setEnabled(false);
            }
        }
    }
}

/**
 * @brief Enables or disables the editor menue entries if the tab was changed
 */
void ColiopUi::on_clpProblemTab_currentChanged()
{
    if ( ui->clpProblemTab->currentIndex()==0)
        enableEditorActions(true);
    else
        enableEditorActions(false);

}


/**
 * @brief enables all action to save a problem - used while tunning a problem
 * @param enable    true or false
 */
void ColiopUi::enableSaveActions(bool enable){
     if (enable) {
         if (!ui->cmplEditor->document()->isEmpty()) {
            ui->actionSave_Problem->setEnabled(true);
            ui->actionSave_Problem_as->setEnabled(true);
         }
     }
     else {
         ui->actionSave_Problem->setDisabled(true);
     }
}

/**
 * @brief creates a list of all files involved - used a list in the editor
 */
void ColiopUi::cmplList() {

    if (!problem->getProblemName().isEmpty() && problem->getActiveCmplIndex()==0) {
        problem->clearCmplList();
        ui->cmplList->clear();

        problem->addCmplList(problem->getProblemName());

        QStringList actCmplCodeList =  ui->cmplEditor->document()->toPlainText().split("\n");

        bool commentSec = false;
        QString cap;

        for ( const QString& actCmplCode : actCmplCodeList ) {


            if (actCmplCode.trimmed().startsWith("#") or actCmplCode.trimmed().startsWith("//") ) {
               continue;
            }

            if (actCmplCode.trimmed().startsWith("/*")  ) {
               commentSec=true;
            }


            if (commentSec ) {
                if (actCmplCode.trimmed().contains("*/") )
                    commentSec=false;
                else continue;
            }

            cap="";
            QRegExp rx("%data(\\s*)[\\:\n\r]{1}");
            if(rx.indexIn(actCmplCode, 0) != -1) {
                problem->addCmplList(problem->getProblemBaseName()+".cdat");
                continue;
            }

            rx.setPattern("%data\\s*([a-zA-Z0-9\\-_\\\\./]*){1}");
            int pos = 0;
            if ((pos = rx.indexIn(actCmplCode, pos)) != -1) {
                cap=rx.cap(1).trimmed();
                if (!cap.isEmpty()) {
                    problem->addCmplList(cap);

                } else {
                    problem->addCmplList(problem->getProblemBaseName()+".cdat");
                }
                continue;
            }

            rx.setPattern("%xlsdata(\\s*)[\\:\n\r]{1}");
            if(rx.indexIn(actCmplCode, 0) != -1) {
                problem->addCmplList(problem->getProblemBaseName()+".xdat");
                continue;
            }

            rx.setPattern("%xlsdata\\s*([a-zA-Z0-9\\-_\\\\./]*){1}");
            pos = 0;
            if ((pos = rx.indexIn(actCmplCode, pos)) != -1) {
                cap=rx.cap(1).trimmed();
                if (!cap.isEmpty()) {
                    problem->addCmplList(cap);

                } else {
                    problem->addCmplList(problem->getProblemBaseName()+".xdat");
                }
                continue;
            }

            rx.setPattern("%include([ a-zA-Z0-9\\-_\\\\./]*){1}");
            pos = 0;
            if ((pos = rx.indexIn(actCmplCode, pos)) != -1) {
                cap=rx.cap(1).trimmed();
                if (!cap.isEmpty()) {
                    problem->addCmplList( cap );
                }
            }
            if (pos != -1 )
                continue;

            // Not implemended in Cmpl yet
            /*rx.setPattern("readcsv\\s*\\(\"([a-zA-Z0-9\-_\\\\./]*)\"\\)");
            pos = 0;
            while ((pos = rx.indexIn(actCmplCode, pos)) != -1) {
                problem->addCmplList( rx.cap(1) );
                pos += rx.matchedLength();
            }*/

        }

        if (problem->cmplListLength()>1) {
            for (int i=0; i< problem->cmplListLength(); i++)
                 ui->cmplList->addItem(problem->getCmplListElement(i));

            ui->cmplList->show();

            QList <int> splitterSizeList;
            splitterSizeList.append(round(0.75*ui->splitter->width()));
            splitterSizeList.append(ui->splitter->width()-round(0.75*ui->splitter->width()));
            ui->splitter->setSizes(splitterSizeList);

        } else {
            ui->cmplList->hide();
        }
    }
}

/**
 * @brief Simple dialog window with Yes/No option
 * @param title     Title of the dialog
 * @param msg       Message of the dialog
 * @return          True or false depending on the Yes/No answer
 */
bool ColiopUi::clpAnswerMessage(QString title, QString msg)
{   bool ret;
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, title, msg,QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes) {
        ret= true;
      } else {
        ret = false;
      }
    return ret;

}

/**
 * @brief Simple warning window with close option
 * @param title     Title of the dialog
 * @param msg       Message of the dialog
 */
void ColiopUi::clpWarning(QString title, QString msg) {
    QMessageBox::critical(this, title , msg ,QMessageBox::Ok);
}


/**
 * @brief Runs some action after changing a problem in editor
 */
void ColiopUi::on_cmplEditorChanged()
{
    enableSaveActions(true);
    problem->setProblemChanged(true);

    if (!problem->getProblemName().isEmpty() && problem->getActiveCmplIndex()==0) {
         cmplList();
    }
}


/**
 * @brief Undo action
 */
void ColiopUi::on_actionUndo()
{
    ui->cmplEditor->undo();
}


/**
 * @brief Redo sction
 */
void ColiopUi::on_actionRedo()
{
     ui->cmplEditor->redo();
}


/**
 * @brief Copy action
 */
void ColiopUi::on_actionCopy()
{
    if (ui->cmplEditor->hasFocus())
        ui->cmplEditor->copy();
    else if (ui->cmplOutPut->hasFocus())
        ui->cmplOutPut->copy();
    else if (ui->cmplSolution->hasFocus())
        ui->cmplSolution->copy();

}

/**
 * @brief Cut action
 */
void ColiopUi::on_actionCut()
{
     ui->cmplEditor->cut();
}

/**
 * @brief Paste action
 */
void ColiopUi::on_actionPaste()
{
     ui->cmplEditor->paste();
}

/**
 * @brief Runs the print dialog
 */
void ColiopUi::on_actionPrint()
{
    QPrinter printer;
    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Document"));
    if (dialog->exec() == QDialog::Accepted) {
        int tab = ui->clpProblemTab->currentIndex();
        switch (tab) {
        case 0:
            ui->cmplEditor->print(&printer);
            break;
        case 1:
            ui->cmplOutPut->print(&printer);
            break;
        case 2:
            ui->cmplSolution->print(&printer);
            break;
        default:
            break;
        }
    }
}

/**
 * @brief Opens the file in editor which has been chosen in the file list
 */
void ColiopUi::on_cmplListClicked()
{
    int actRow =ui->cmplList->currentRow();

    if (actRow!=problem->getActiveCmplIndex()) {
        if (problem->isProblemChanged()) {
            QString sName = problem->getProblemPath()+"/"+problem->getCmplListElement(problem->getActiveCmplIndex());
            if (clpAnswerMessage("Save file", "File <"+sName+"> is changed. \n\nDo you want to save it?") == true) {
                saveProblem(sName);
              }
        }
        QString fName = problem->getProblemPath()+"/"+problem->getCmplListElement(actRow);
        QFile file(fName);
        if (!file.exists()) {
            if (clpAnswerMessage("Save file", "File <"+fName+"> does not exist. \n\nDo you want to create it?") == true) {
                if (!file.open(QIODevice::Truncate | QIODevice::Text | QIODevice::ReadWrite)) {
                    clpWarning("Error","Cannot create file <"+fName+">");
                    actRow=problem->getActiveCmplIndex();
                    ui->cmplList->item(actRow)->setSelected(true);

                } else {
                    file.close();
                    problem->setActiveCmplIndex(actRow);
                    openProblem(fName);
                }

             } else {
                actRow=problem->getActiveCmplIndex();
                ui->cmplList->item(actRow)->setSelected(true);
                problem->setActiveCmplIndex(actRow);
            }
        } else {
                problem->setActiveCmplIndex(actRow);
                openProblem(fName);
        }
    }

    if (problem->getActiveCmplIndex()==0) {
        ui->actionSave_Problem_as->setEnabled(true);

    } else {
        ui->actionSave_Problem_as->setDisabled(true);
    }

}


/**
 * @brief Jumps to the line in editor which has been chosen in the message list
 */
void ColiopUi::on_cmplMessageListClicked()
{
    int actRow =ui->cmplMessages->currentRow()-2;
    int actCmplIdx =0;

    if (actRow>=0) {
        QString fileName = _cmplMessages.at(actRow).file;

        for (int i=0; i<problem->cmplListLength(); i++ ) {
            if (problem->getCmplListElement(i).contains(fileName)) {
                fileName=problem->getProblemPath()+"/"+problem->getCmplListElement(i);
                actCmplIdx=i;
                break;
            }
        }

        ui->cmplList->setCurrentRow(actCmplIdx);
        problem->setActiveCmplIndex(actCmplIdx);

        openProblem(fileName);

        ui->cmplList->setCurrentRow(actCmplIdx);
        problem->setActiveCmplIndex(actCmplIdx);

        ui->clpProblemTab->setCurrentIndex(0);
        gotoLine(_cmplMessages.at(actRow).line);
    }
}

/**
 * @brief Opens a Cmpl shell
 */
void ColiopUi::on_actionOpen_CmplShell_triggered()
{
    QString runProg;
    QStringList progArgs = QStringList();


    if (_runningOs == "windows" ) {

        runProg = "rundll32";
        progArgs.append("url.dll,FileProtocolHandler");
        progArgs.append( _cmplPath+".."+QDir::separator()+"cmplShell.bat");

    } else if (_runningOs == "osx" or _runningOs== "macos") {
        runProg = "open";
        progArgs.append(_cmplPath+"../cmplShell");
    } else {
       runProg = _cmplPath+"../cmplShell";

    }


    _cmplProcessHandler = new QProcess(this);
    _cmplProcessHandler->setProgram(runProg);
    _cmplProcessHandler->setArguments(progArgs);
    _cmplProcessHandler->start();

    if (!_cmplProcessHandler->waitForStarted()) {
        clpWarning("ERROR", "Cannot start CmplShell <"+runProg+">") ;
    }

}

/**
 * @brief Saves CMPL's tmp file
 */
void ColiopUi::saveCmplTmp() {

    _editorFontSize=ui->cmplEditor->fontInfo().pointSize();

    QFile tmpFile(_cmplTmpFileName);
    if (tmpFile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
        QTextStream stream(&tmpFile);
        stream << _cmplCwd << Qt::endl;
        stream << _cmplUrl << Qt::endl;
        stream << _editorFontSize << Qt::endl;
    }
}

/**
 * @brief Opens CMPL's tmp file
 */
bool ColiopUi::readCmplTmp() {
    bool ret=false;
    QString endLine="";

    if ( QSysInfo::productType() == "windows" ) {
        endLine="\r\n";
    } else {
        endLine="\n";
    }

    QFile tmpFile(_cmplTmpFileName);
    if (tmpFile.open(QIODevice::ReadOnly)) {

        int row=1;
        while (!tmpFile.atEnd()) {
            switch(row) {
            case 1:
                _cmplCwd = tmpFile.readLine().replace(endLine,"");
                break;
            case 2:
                _cmplUrl = tmpFile.readLine().replace(endLine,"");
                break;
           case 3:
                _editorFontSize = tmpFile.readLine().replace(endLine,"").toInt();
                break;
            }
        row++;
        }

        tmpFile.close();
        ret=true;
    }
    return ret;
}

/**
 * @brief checks asynchronious mode (solve, send, knock, retrieve
 * @return  asynchronious mode
 */
int ColiopUi::checkAsyncMode() {

    QStringList probText = ui->cmplEditor->toPlainText().split(QRegExp("[\r\n]"),QString::SkipEmptyParts);

    QString line;
    bool isComment;
    isComment=false;

    foreach( line, probText) {
        if (!isComment && !line.trimmed().startsWith("#") && line.contains("/*"))
            isComment=true;

        if (isComment && line.contains("*/"))
            isComment=false;

        if (!isComment) {
            if (!line.trimmed().startsWith("#")) {
                if (line.contains("-url"))
                    problem->setAsyncMode(0);
                if (line.contains("-send"))
                    problem->setAsyncMode(1);
                if (line.contains("-knock"))
                    problem->setAsyncMode(2);
                if (line.contains("-retrieve"))
                    problem->setAsyncMode(3);
            }
        }
    }
}
