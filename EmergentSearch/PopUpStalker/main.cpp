#include "mainwindow.h"
#include "maintab.h"
#include "viewcontroller.h"
#include "model.h"

#include <thread>
#include <signal.h>
#include <iostream>

#include <QApplication>
#include <QTableWidget>
#include <QWidget>
#include <QLabel>
#include <QScrollArea>
#include <QString>
#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>

//extern QString inputDir;

/**
 * @brief rFindImages
 *      Goes through the root directory, dir, recursively looking
 *      for .png files to add to the display. The absolute path of
 *      file is used to see if it has or has not already been added.
 * @param mod
 *      The Model object needed to recursive file checking
 * @param dir
 *      The root directory we want to recursively check
 * @param vc
 *      The ViewController needed as input after finding an image
 */
void rFindImages(Model *mod, QDir dir, ViewController *vc)
{
//    int i = 0;
    while (true)
    {
//        printf("Call number %d\n", i);
//        fflush(stdout);
//        i++;
        mod->fileChecker(dir, vc);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    }
    //std::terminate();
}

int main(int argc, char *argv[])
{
    // Needed to allow a signal to be sent with QFileInfo
    // objects as parameters since it needs the information
    // to be added to a queue... or something
    qRegisterMetaType<QFileInfo>("QFileInfo");

    QApplication a(argc, argv);

    // Used to delegate between front end and backend
    ViewController *vc = new ViewController();

    // Initial setup stuff for ViewController so that it will
    // be ready for looking like the gui we want that can just
    // have rows added to it
    vc->setModel(new Model());
    vc->setUpStalker();

    // Connect signals and slots so that when an image is found
    // the Model object can send a signal to be processed by the
    // same object but on the main thread (at least I think that
    // is how it works...)
    Model *currMod = vc->getModel();
//    QObject::connect(currMod, SIGNAL(imageFound(QFileInfo , ViewController *, QString )),
//                     currMod, SLOT(addImage(QFileInfo , ViewController *, QString )));

    QObject::connect(currMod, SIGNAL(queueAdd(QString, QString)),
                     currMod, SLOT(addToNextQ(QString, QString)));

    // The input directory in which we look for files
    QString inDir = getInputDir();
    // Setup for continuously finding images on a separate thread
    QDir currDir(inDir);
    std::thread myt(rFindImages, currMod, currDir, vc);
    myt.detach();

    return a.exec();
}


//void sigabrt(int i)
//{
//    printf("Abort signaled with %d\n", i);
//    exit(0);
//}

//    void (*sigabrtPtr)(int);
//    sigabrtPtr = signal(SIGABRT, SIG_IGN);
//    std::thread myt(callFromThread, 0);
//    myt.detach();
