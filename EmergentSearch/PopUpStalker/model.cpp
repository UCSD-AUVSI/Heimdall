#include "model.h"

#include <QFileInfoList>
#include <QFile>
#include <QTextStream>
#include <QFont>
#include <thread>

QString extension = "jpg";

Model::Model() :
    vc(0),
    prev(0),
//    nextQLock(new std::unique_lock<std::mutex>(*qMutex)),
    qFull(new std::condition_variable()),
    qSize(new std::condition_variable()),
    qMutex(new std::mutex()),
    setMutex(new std::mutex()),
    curr(new std::vector<StalkerLabels*>()),
    nextQ(new std::vector<StalkerLabels*>()),
    prev1(new std::vector<StalkerLabels*>()),
    prev2(new std::vector<StalkerLabels*>()),
    prev3(new std::vector<StalkerLabels*>()),
    prev4(new std::vector<StalkerLabels*>()),
    prev5(new std::vector<StalkerLabels*>()),
    targetFiles(new std::unordered_set<std::string>())
{
}

void Model::setViewController(ViewController *vc)
{
    this->vc = vc;
}

ViewController* Model::getViewController()
{
    return vc;
}

// Called when the next button is clicked to get the next set of pictures
void Model::nextClicked()
{
    // ensure the vc is not null
    if (!vc)
        return;

    // use the int variable prev to determine current state
    // and find which to switch to
    std::vector<StalkerLabels*> *tmp = 0;
    switch(prev) {
        case 1:
            tmp = curr;
            break;
        case 2:
            tmp = prev1;
            break;
        case 3:
            tmp = prev2;
            break;
        case 4:
            tmp = prev3;
            break;
        case 5:
            tmp = prev4;
            break;
    }

    // If tmp is not null we are going to one of the states
    // we already have information for, so we just need to
    // update the prev variable
    if (tmp != 0)
    {
        prev--;
    }
    else
    {
        // lock the mutex before using nextQ
        qMutex->lock();

        // if the queue is empty don't do anything
        if (nextQ->empty())
            return;

        tmp = new std::vector<StalkerLabels*>();
        int i = 0;

        // get up to 10 StalkerLabels from the queue to
        // add to the gui, and fill the other spaces with
        // empty StalkerLabels
        while (i < 10)
        {
            if (!nextQ->empty())
            {
                StalkerLabels *lbls = nextQ->front();
                tmp->push_back(lbls);
                nextQ->erase(nextQ->begin());
            }
            else
            {
                tmp->push_back(new StalkerLabels());
            }
            i++;
        }

        // done with nextQ so unlock it's mutex
        qMutex->unlock();

        // TODO: synchronize setMutex and targetFiles set
        MainTab *anotherTab = new MainTab();
        anotherTab->setStalkerLabels(tmp);

        QWidget *remove = vc->mainDisp->widget(5);
        if (remove)
            vc->mainDisp->removeWidget(remove);

        vc->mainDisp->insertWidget(0, anotherTab);

        // Before clear of prev5, delete input files
        for (std::vector<StalkerLabels*>::iterator start = prev5->begin();
             start != prev5->end(); start++)
        {
            // get the pictures path
            QString picPath = (*start)->getAbsPath();

            // Don't delete an empty path
            if (picPath.isEmpty())
                continue;

            // get the path of the pictures corresponding txt file
            QString infoPath = picPath;
            int extLen = extension.size();
            infoPath = infoPath.remove(infoPath.size()-extLen,extLen).append("txt");

            // remove the picture and text file
            if ( !QFile::remove(picPath) )
            {
                printf("Model.cpp: Something went wrong deleting a picture\n");
            }
            if ( !QFile::remove(infoPath) )
            {
                printf("Model.cpp: Something went wrong deleting a text file\n");
            }
            fflush(stdout);

            // since we are deleting the file we can delete the file path from our
            // set so we know if a new file shows up with the same name at a later
            // time (this may be unnecessary)
            setMutex->lock();
            std::unordered_set<std::string>::iterator it = targetFiles->find(infoPath.toStdString());
            if ( !(it == targetFiles->end()) )
                targetFiles->erase(it);
            setMutex->unlock();
        }

        // update the vectors appropriately
        prev5->clear();
        delete prev5;
        prev5 = prev4;
        prev4 = prev3;
        prev3 = prev2;
        prev2 = prev1;
        prev1 = curr;
        curr = tmp;

        // wake up the thread waiting on qFull if sleeping
        qFull->notify_all();
    }

    // display the appropriate vector of pictures
    vc->mainDisp->setCurrentIndex(prev);


}


// Called when theprev button is clicked to get a previous set of pictures
void Model::prevClicked()
{
    // ensure the vc is not null
    if (!vc)
        return;


    // use the int variable prev to determine current state
    // and find which to switch to
    std::vector<StalkerLabels*> *tmp = 0;
    switch(prev) {
        case 0:
            if (prev1->size() != 0)
                tmp = prev1;
            break;
        case 1:
            if (prev2->size() != 0)
                tmp = prev2;
            break;
        case 2:
            if (prev3->size() != 0)
                tmp = prev3;
            break;
        case 3:
            if (prev4->size() != 0)
                tmp = prev4;
            break;
        case 4:
            if (prev5->size() != 0)
                tmp = prev5;
            break;
        default:
            break;
    }

    // if tmp is not null update the view
    // the only case tmp would be null is if we were already
    // at the furthest prev vector
    if (tmp != 0)
    {
        prev++;

        vc->mainDisp->setCurrentIndex(prev);
    }
}

// The slot which handles all adding of the picture files to nextQ
// to consume in the clickedNext slot
void Model::addToNextQ(QString fp, QString fName)
{
    // get the file path for the txt file
    QString infoFileStr = fp;
    int extLen = extension.size();
    infoFileStr = infoFileStr.remove(infoFileStr.size()-extLen,extLen).append("txt");
    QFile infoFile(infoFileStr);

//    printf("File Name: %s\n", fName.toStdString().c_str());
//    fflush(stdout);

    // used to hold the attributes lat and lon of the picture
    QString attribs[2] = {0,0};

    // open the txt file and read from it
    if (!infoFile.open(QIODevice::ReadOnly))
    {
        printf("Problems finding file with name %s\n", infoFileStr.toStdString().c_str());

        return;
    }
    else
    {
        // create a stream to read the file contents
        QTextStream infoFileStream(&infoFile);

        // forget the first 4 lines based on the file
        // specifications
        int i = 0;
        while (!infoFileStream.atEnd() && i < 4)
        {
            infoFileStream.readLine();
            i++;
        }

        // the next 2 lines should be lat and lon
        i = 0;
        while (!infoFileStream.atEnd() && i < 2)
        {
            QString line = infoFileStream.readLine();
            if (line.isEmpty())
                continue;
            attribs[i] = line;
            i++;
        }
    }

    infoFile.close();

    // put in the proper fields to the new StalkerLabels object
    StalkerLabels *lbls = new StalkerLabels(
                fName, attribs[0], attribs[1], fp);

    // lock the mutex for nextQ
    std::unique_lock<std::mutex> localLock(*qMutex);

    nextQ->push_back(lbls);

    // wake up any other thread waiting on synchronizing the q's size
    qSize->notify_all();


}


/**
 * @brief Model::fileChecker
 *      Recursively goes through dir and looks for .png files.
 *      When one is found, it emits a signal that an image is found.
 * @param dir
 *      QDir object representing the directory to be recursed.
 * @param vc
 *      ViewController object to pass to the emitted signal
 *      for adding the image to the gui.
 */
void Model::fileChecker(QDir dir, ViewController *vc)
{
    dir.refresh();
    QFileInfoList dirList = dir.entryInfoList();
    QString ext(".png");
    for (int i = 0; i < dirList.size(); i++)
    {
        QFileInfo file = dirList.at(i);
        QString fName = file.fileName();
        QString absFP = file.absoluteFilePath();
        std::string stdAbsFP = std::string(absFP.toStdString());

        // Don't need the recursive check so file is dir path is
        // just commented out since we just wanted to skip that case
//        if (file.isDir())
//        {
//            if (fName.compare(".") == 0 || fName.compare("..") == 0)
//                ; // skip
//            else
//                fileChecker(QDir(absFP), vc);
//        }
        if (!file.isDir())
        {
            // ensure either branch of the if unlocks the mutex afterwards
            setMutex->lock();

            if (fName.contains(extension) &&
                    targetFiles->find(stdAbsFP) == targetFiles->end())
            {

                setMutex->unlock();

                // Need a lock for condition variables
                std::unique_lock<std::mutex> localLock(*qMutex);

                QString infoFileStr = file.absoluteFilePath();
                int extLen = extension.size();
                infoFileStr = infoFileStr.remove(infoFileStr.size()-extLen,extLen).append("txt");
                QFile infoFile(infoFileStr);

                if (!infoFile.open(QIODevice::ReadOnly))
                {
                    infoFile.close();
                    printf("Problems finding file with name %s\n",
                           infoFileStr.toStdString().c_str());

                }
                else
                {
                    // Use targetFiles to add only new files that match
                    // the required extension
                    setMutex->lock();
                    targetFiles->insert(stdAbsFP);
                    setMutex->unlock();

                    infoFile.close();

                    int size = nextQ->size();
                    int sizeThresh = 20;
                    if ( size >= sizeThresh )
                    {
                        printf("Size is now %d!\n", size);
                        fflush(stdout);
                        qFull->wait(localLock);
                    }

                    emit queueAdd(absFP, fName);

                    // synchronize nextQ's size
                    qSize->wait(localLock);

                }

            } // end if ext check
            else
            {
                setMutex->unlock();
            }
        }
    }
}


