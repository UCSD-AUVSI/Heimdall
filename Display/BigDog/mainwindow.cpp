#include "mainwindow.h"
#include "maintab.h"
#include "model.h"

#include <QVBoxLayout>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QMenuBar>
#include <QTabBar>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)

{
    setupAsMainWindow();
}

MainWindow::~MainWindow()
{

}

/**
 * @brief MainWindow::setupAsMainWindow
 *      Creates clickable actions added to a menu and toolbar
 */
void MainWindow::setupAsMainWindow()
{
    QAction *newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    QAction *openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
//    Model *myMod = new Model();
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();

    QToolBar *fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
}

/**
 * @brief MainWindow::newFile
 *      Slot method to be triggered by a signal.
 *      (not currently useful).
 */
void MainWindow::newFile()
{
    printf("newFile called!\n");
    fflush(stdout);
}

/**
 * @brief MainWindow::open
 *      Slot method to be triggered by a signal.
 *      (not currently useful).
 */
void MainWindow::open()
{
    printf("open called!\n");
    fflush(stdout);
}

//void MainWindow::closeEvent(QCloseEvent *event)
//{
//    exit(0);
//}
