#include "model.h"

#include <QFileInfoList>
#include <QFile>
#include <QTextStream>
#include <QFont>
#include <thread>
#include <iostream>

using std::cout;
using std::endl;

QString extension = "jpg";

Model::Model() :
    vc(0)
//    targetFiles(new std::set<QString>())
{
}

//Model::Model(ViewController *vc) :
//    vc(vc)
//{
//}

void Model::setViewController(ViewController *vc)
{
    this->vc = vc;
}

ViewController* Model::getViewController()
{
    return vc;
}

/**
 * @brief Model::addImage
 *      A slot method to add an image to the view in vc.
 * @param file
 *      QFileInfo object that represents the file image file.
 * @param vc
 *      ViewController object to add the image to its view.
 * @param ext
 *      The extension that file needs to have to be added to the ViewController
 *      (note it is probably best to handle this before using this slot, so
 *      ext may be unnecessary).
 */
void Model::addImage(QFileInfo file, ViewController *vc, QString ext)
{
    // Calls another method because this one cannot return a boolean.
    // The code can probably be copied here if that is not needed.
    addValidImage(file, vc, ext);
}

/**
 * @brief Model::addValidImage
 *      Adds an image to the view in vc if the file has the proper extension.
 * @param file
 *      QFileInfo object that represents the file image file.
 * @param vc
 *      ViewController object to add the image to its view.
 * @param ext
 *      The extension that file needs to have to be added to the ViewController
 *      (note it is probably best to handle this before using this slot, so
 *      ext may be unnecessary).
 * @return
 *      A boolean value representing whether the image was actually added or not
 */
bool Model::addValidImage(QFileInfo &file, ViewController *vc, QString ext)
{
    QString fileName = file.fileName();
//    int count = 1;
    if (fileName.contains(ext))
    {
//        printf("got %d!\n%s\n", count++, file.absoluteFilePath().toStdString().c_str());

        // Creates file object for txt file corresponding to the jpg or png
        QString infoFileStr = file.absoluteFilePath();
        int extLen = extension.size();
        infoFileStr = infoFileStr.remove(infoFileStr.size()-extLen,
                                         extLen).append("txt");
        QFile infoFile(infoFileStr);

        // Prepare for updating QLabels to be put into the gui
        QLabel *charT;
        QLabel *charC;
        QLabel *shapeT;
        QLabel *shapeC;
        QLabel *lat;
        QLabel *lon;
        QLabel *orien;

        // Properties is an array for ease of reading the info into the appropriate QLabel
        QLabel** properties[] = {&charT, &charC, &shapeT, &shapeC, &lat, &lon, &orien};
        int propLen = (sizeof properties)/(sizeof *properties);

        if (!infoFile.open(QIODevice::ReadOnly))
        {
            // File for the picture attributes did not open correctly, but fill in default vals
            printf("Problems finding file with name %s\n", infoFileStr.toStdString().c_str());
            charT = new QLabel("N/A");
            charC = new QLabel("N/A");
            shapeT = new QLabel("N/A");
            shapeC = new QLabel("N/A");
            lat = new QLabel("N/A");
            lon = new QLabel("N/A");
            orien = new QLabel("N/A");
        }
        else
        {
            QTextStream infoFileStream(&infoFile);

            int i = 0;
//            int j = sizeof properties;
//            int k = sizeof *properties;
            while (!infoFileStream.atEnd() &&
                   i < propLen)
            {
                QString line = infoFileStream.readLine();
                QLabel *temp;
                if (line.isEmpty()) {
                    temp = new QLabel("N/A");
                } else {
                    temp = new QLabel(line);
                }
                QFont f( "Arial", 14, QFont::Bold);
                temp->setFont( f);
                *(properties[i++]) = temp;
            }

            // Ensure each QLabel is not null
            while (i < propLen)
            {
                QLabel *temp = new QLabel("N/A");
                QFont f( "Arial", 14, QFont::Bold);
                temp->setFont( f);
                *(properties[i++]) = temp;
            }
        }
//        printf("size of properties: %d\n", sizeof(properties));

        infoFile.close();

        // prep for addition to the gui through vc
        std::list<QLabel*> *lst = new std::list<QLabel*>();
        QLabel *picLbl = new QLabel();
        picLbl->setPixmap(QPixmap(file.absoluteFilePath()).scaledToWidth(150));

        // add the QLabels to a lst that will be added in the gui
        lst->push_back(picLbl);

        for (int i = 0; i < propLen; i++)
        {
            lst->push_back(*properties[i]);
        }

        // actually add all the stuff to the gui
        vc->addRow(lst);

        delete lst;
        return true;
    }
    else
        return false;
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
//    recurseDir(dir, vc);
    dir.refresh();
    QFileInfoList dirList = dir.entryInfoList();
    QString ext(extension);
    for (int i = 0; i < dirList.size(); i++)
    {
        QFileInfo file = dirList.at(i);
        QString fName = file.fileName();

        // Don't need the recursive check so file is dir path is
        // just commented out since we just wanted to skip that case
//        if (file.isDir())
//        {
//            if (fName.compare(".") == 0 || fName.compare("..") == 0)
//                ; // skip
//            else
//                fileChecker(QDir(file.absoluteFilePath()), vc);
//        } else
        if (!file.isDir())
        {
            if (targetFiles.find(file.absoluteFilePath()) == targetFiles.end())
            {
                // Use targetFiles to add only .png files that are new visited
                // absolute paths.
                targetFiles.insert(file.absoluteFilePath());

//                printf("emitting now\n");
//                fflush(stdout);
                emit imageFound(file, vc, ext);
            }
        }
    }
}


