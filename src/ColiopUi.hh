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

#ifndef COLIOPUI_H
#define COLIOPUI_H

#include <QMainWindow>
#include <QtGui>
#include <QTextDocumentWriter>
#include <QFileDialog>
#include <QInputDialog>
#include <QTextCursor>
#include <QPrinter>
#include <QPrintDialog>
#include <QMessageBox>
#include <QCoreApplication>
#include <QRegExp>
//#include <QRegularExpression>

#include "CmplHighlighter.hh"
#include "ClpAbout.hh"
#include "ClpProblem.hh"
#include "CmplRunner.hh"


namespace Ui {
    class coliopUi;
}

/**
 * @brief ColiopUi - UI Class for the Cmpl IDE
 */
class ColiopUi : public QMainWindow
{
    Q_OBJECT

public:

    /**
     * @brief Constructor
     * @param parent
     */
    explicit ColiopUi(QWidget *parent = 0);

    /**
     * @brief Destructor
     * @param parent
     */
    ~ColiopUi();

    /**
     * @brief Initial opening of a problem - used for automatical opening a problem by doupple clicking in Explorer/Finder/etc.
     * @param fileName      file name of the problem
     */
    void openFirstProblem( QString &fileName );

private:
    Ui::coliopUi *ui;                               // ui Object
    CmplHighlighter *cmplSyntaxHighlighter;         // object of the sintax highlighter
    ClpProblem *problem;                            // object of the Cmpl problem

    ClpAbout *coliopAbout;                          // object of the about dialog

    CmplRunner *_cmplHandler;                       // object of the CmplRunner class
    QList<cmplMessages> _cmplMessages;              // Cmpl Messages

    QProcess *_cmplProcessHandler;                   // Handler of the external process

    QString _cmplUrl;
    bool _isRemote;

    QString _cmplCwd;
    QString _cmplTmpFileName;
    QString _cmplPath;

    int _editorFontSize;

    QString _runningOs;

    /**
     * @brief saves problem with given file name
     * @param fileName  file name
     * @return true - if files was saved succesfully
     */
    bool saveProblem(QString &fileName);

    /**
     * @brief opens a problem with given file name
     * @param fileName  file name
     */
    void openProblem( QString &fileName );

    /**
     * @brief initial setup of the fonts
     */
    void setupFonts();

    /**
     * @brief editor action -  jumps to a selcted line in the editor
     * @param lineNumber    line number
     */
    void gotoLine(int lineNumber);

    /**
     * @brief enables all action to edit a problem - used while tunning a problem
     * @param enable    true or false
     */
    void enableEditorActions(bool enable);

    /**
     * @brief enables all action to save a problem - used while tunning a problem
     * @param enable    true or false
     */
    void enableSaveActions(bool enable);

    /**
     * @brief creates a list of all files involved - used a list in the editor
     */
    void cmplList();

    /**
     * @brief Simple dialog window with Yes/No option
     * @param title     Title of the dialog
     * @param msg       Message of the dialog
     * @return          True or false depending on the Yes/No answer
     */
    bool clpAnswerMessage(QString title, QString msg);

    /**
     * @brief Simple warning window with close option
     * @param title     Title of the dialog
     * @param msg       Message of the dialog
     */
    void clpWarning(QString title,QString msg);

    /**
     * @brief Saves Coliops current parameter in a tmpfile
     */
    void saveCmplTmp();


    /**
     * @brief Reads Coliops current parameter from a tmpfile
     * @return  True or false wether the file can be read or not
     */
    bool readCmplTmp();


    /**
     * @brief checks asynchronious mode (solve, send, knock, retrieve
     * @return  asynchronious mode
     */
    int checkAsyncMode();


private slots:
    /**
     * @brief Opens the file in editor which has been chosen in the file list
     */
    void on_cmplListClicked();

    /**
     * @brief Jumps to the line in editor which has been chosen in the message list
     */
    void on_cmplMessageListClicked();

    /**
     * @brief Runs the print dialog
     */
    void on_actionPrint();

    /**
     * @brief Paste action
     */
    void on_actionPaste();

    /**
     * @brief Cut action
     */
    void on_actionCut();

    /**
     * @brief Copy action
     */
    void on_actionCopy();

    /**
     * @brief Redo sction
     */
    void on_actionRedo();

    /**
     * @brief Undo action
     */
    void on_actionUndo();

    /**
     * @brief Runs some action after changing a problem in editor
     */
    void on_cmplEditorChanged();

    /**
     * @brief Enables or disables the editor menue entries if the tab was changed
     */
    void on_clpProblemTab_currentChanged();

    /**
     * @brief Zooms out editor/messages/solution
     */
    void on_actionZoom_out();

    /**
     * @brief Zooms in editor/messages/solution
     */
    void on_actionZoom_in();


    /**
     * @brief Action Save_Problem
     */
    void on_actionSave_Problem();

    /**
     * @brief Action Save_Problem
     */
    void on_actionSave_Problem_as();

    /**
     * @brief Opens the about dialog
     */
    void on_actionAbout();

    /**
     * @briefOpens a new Coliop window
     */
    void on_actionNewColiopWindow();

    /**
     * @brief Opens a problem
     */
    void on_actionOpen_Problem();

    /**
     * @brief creates a new problem
     */
    void on_actionNew_Problem();

    /**
     * @brief Runs optimisation
     */
    void on_actionSolve();

    /**
     * @brief Cancels optimisation
     */
    void on_actionCancel();

    /**
     * @brief actions after finishing optimisation
     */
    void on_cmplFinished();

    /**
     * @brief Some UI behaviour after starting optimisation
     */
    void on_cmplStarted();

    void on_cmplError();

    /**
     * @brief Some UI behaviour after canceling optimisation
     */
    void on_cmplCanceled();

    /**
     * @brief Opens the manual
     */
    void on_cmplHelp();

    /**
     * @brief Final activities while closing Coliop
     */
    void on_actionQuit();

    void on_actionOpen_CmplShell_triggered();

    void on_actionSolve_on_server();

    void on_actionGo_to_line_triggered();
};

#endif // COLIOPUI_H
