#include "util.h"
#include <thread>

QString inputDir = "./psin";

QString getInputDir()
{
    return inputDir;
}


/////////////////////////////// Gui Button //////////////////////////////////////////////

GuiButton::GuiButton(QWidget *parent) :
    QPushButton(parent),
    funptr(0)
{
    textBoxes = new std::list<QLineEdit*>();
}

GuiButton::GuiButton(const QString &text, QWidget *parent) :
    QPushButton(text,parent),
    funptr(0)
{
    textBoxes = new std::list<QLineEdit*>();
}

GuiButton::~GuiButton()
{
    delete textBoxes;
}

void GuiButton::addQLineEdit(QLineEdit *ledit)
{
    textBoxes->push_back(ledit);
}

void GuiButton::setFunptr(void (*func)())
{
    funptr = func;
}

void GuiButton::mousePressEvent(QMouseEvent *event)
{
    QPushButton::mousePressEvent(event);
    if (event->button() == Qt::LeftButton)
    {
        if (funptr != 0)
            (*funptr)();
    }
    else
    {
        printf("Incorrect mouse button clicked.\nExpected: %d\nReceived: %d\n", Qt::LeftButton, event->button());
        fflush(stdout);
    }
}

void GuiButton::keyPressEvent(QKeyEvent *event)
{
    QPushButton::keyPressEvent(event);
    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Enter)
    {
        if (funptr != 0)
            (*funptr)();
    }
    else
    {
        printf("Incorrect key was pressed.\nExpected: %d\nReceived: %d\n", Qt::Key_Space, event->key());
        fflush(stdout);
    }
}

/////////////////////////////// Target Rec //////////////////////////////////////////////


TargetRec::TargetRec() :
    imageConfirmed(false)
{
}

void TargetRec::recurseDir(QDir &dir, std::set<QString> *targetFolders)
{
//    recurseDir(dir, vc);
    dir.refresh();
    QFileInfoList dirList = dir.entryInfoList();
    QString ext(".jpg");
    for (int i = 0; i < dirList.size(); i++)
    {
        QFileInfo file = dirList.at(i);
        QString fName = file.fileName();

        if (file.isDir())
        {
            if (fName.compare(".") == 0 || fName.compare("..") == 0)
                ; // skip
            else {
                QDir newdir(file.absoluteFilePath());
                TargetRec::recurseDir(newdir, targetFolders);
            }
        }
        else if (!file.isDir())
        {
            if (targetFolders->find(file.absoluteFilePath()) == targetFolders->end())
            {
                // Use targetFolders to add only .png files that are new visited
                // absolute paths.
                targetFolders->insert(file.absoluteFilePath());

//                printf("emitting now\n");
//                fflush(stdout);
//                emit imageFound(file, vc, ext);
            }
        }
    }
}

void TargetRec::recurseTargets(QDir &dir, QLabel *lbl)
{
    std::set<QString> *targetFolders = new std::set<QString>();
    recurseDir(dir, targetFolders);
    std::set<QString>::iterator start = targetFolders->begin();
    std::set<QString>::iterator end = targetFolders->end();

    lbl->setPixmap(QPixmap(*start++).scaledToHeight(600));
    while (start != end)
    {
        while (!imageConfirmed)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        lbl->setPixmap(QPixmap(*start++).scaledToHeight(600));
        imageConfirmed = false;
    }

}




void TargetRec::nextImage(QLabel *lbl, QString fileStr)
{
    imageConfirmed = true;
    printf("nextImage called!\n");
}

//    QLabel *picLabel;

/////////////////////////////// Stalker Labels //////////////////////////////////////////////

StalkerLabels::StalkerLabels()
{
    this->name = new QLabel();
    this->lat = new QLabel();
    this->lon = new QLabel();
    this->pMap = new QLabel();
    this->absPath = "";
}

StalkerLabels::StalkerLabels(QString name, QString lat, QString lon,
              QString pMapPath)
{
    this->name = new QLabel(name);
    this->lat = new QLabel(lat);
    this->lon = new QLabel(lon);
    this->pMap = new QLabel(pMapPath);
    this->absPath = pMapPath;

//    QFont f( "Arial", 20, QFont::Bold);
//    this->name->setFont(f);
//    this->lat->setFont(f);
//    this->lon->setFont(f);

    QPixmap pixMap = QPixmap(pMapPath).scaledToHeight(200);
    if (pixMap.width() > 200)
        pixMap = pixMap.scaledToWidth(200);
    pMap->setPixmap(pixMap);
}

StalkerLabels::~StalkerLabels()
{
    delete this->name;
    delete this->lat;
    delete this->lon;
    delete this->pMap;
}

QLabel* StalkerLabels::getName()
{
    return this->name;
}

QLabel* StalkerLabels::getLat()
{
    return this->lat;
}

QLabel* StalkerLabels::getLon()
{
    return this->lon;
}

QLabel* StalkerLabels::getPMap()
{
    return this->pMap;
}

QString StalkerLabels::getAbsPath()
{
    return this->absPath;
}
