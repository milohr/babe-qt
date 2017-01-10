#include "settings.h"
#include "ui_settings.h"
#include "QDebug"
#include <QDirIterator>
#include <QFileDialog>
#include <QDebug>
#include <fstream>
#include<iostream>
#include<QStringList>
#include <QThread>
#include<collectionDB.h>
#include<QMessageBox>

settings::settings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::settings)
{
    ui->setupUi(this);
    connect(this, SIGNAL(collectionPathChanged(QString)),
                         this, SLOT(populateDB(QString)));

    thread = new QThread(parent);
        // Do not set a parent. The object cannot be moved if it has a parent.
    collection_db.openCollection("../player/collection.db");
        collection_db.moveToThread(thread);

        connect(thread, SIGNAL(finished()), &collection_db, SLOT(deleteLater()));
        // connect(thread, SIGNAL(finished()), &collection_db, SLOT(closeConnection()));
        connect(&collection_db, SIGNAL(DBactionFinished(bool)),this, SLOT(finishedAddingTracks(bool)));

            connect(thread, SIGNAL(started()), &collection_db, SLOT(addTrack()));
            connect(&collection_db, SIGNAL(progress(int)), ui->progressBar, SLOT(setValue(int)));
ui->progressBar->hide();

}

settings::~settings()
{
    delete ui;
    collection_db.closeConnection();
}

CollectionDB& settings::getCollectionDB()
{

    return collection_db;
}

void settings::on_toolbarIconSize_activated(const QString &arg1)
{
    //qDebug () <<arg1;
    iconSize=arg1.toInt();
    setSettings({"toolbarIconSize=",arg1});

    emit toolbarIconSizeChanged(iconSize);
}



void settings::on_toolButton_clicked()
{
    QString url= QFileDialog::getExistingDirectory();

//qDebug()<<url;


if(collectionPath!=url && url.size()!=0)
{
    ui->collectionPath->setText(url);
collectionPath=url;
qDebug()<<"changed";
setSettings({"collectionPath=",collectionPath});

emit collectionPathChanged(collectionPath);
}
    /*QStringList urlCollection;
//QDir dir = new QDir(url);
    QDirIterator it(url, QStringList() << "*.mp4" << "*.mp3" << "*.wav" <<"*.flac" <<"*.ogg", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        urlCollection<<it.next();

        //qDebug() << it.next();
    }

    collection.add(urlCollection);
    //updateList();
    populateTableView();*/
}

void settings::setSettings(QStringList setting)

{
    std::string strNew;
    std::string strReplace;
    strNew = setting.at(0).toStdString()+setting.at(1).toStdString();
    bool replace=false;
    /**/
    qDebug()<<setting.at(0);

    std::ifstream settings(settingPath);
    QStringList newline;
    std::string line;
    while (std::getline(settings, line))
    {

        //qDebug()<<get_setting;
       if(QString::fromStdString(line).contains(setting.at(0)))
       {
           newline<<QString::fromStdString(strNew);
           replace=true;
       }else
       {
           newline<<QString::fromStdString(line);
       }


    }

    if(replace)
    {
        std::ofstream write(settingPath);

        for(auto ln : newline)
        {
             write << ln.toStdString() << std::endl;
        }


    }else
    {
        std::ofstream write(settingPath, std::ios::app);
        write << strNew<< std::endl;
    }


}

void settings::readSettings()
{
    std::ifstream settings(settingPath);
    std::string line;
    while (std::getline(settings, line))
    {

        auto get_setting = QString::fromStdString(line);
        //qDebug()<<get_setting;
       if(get_setting.contains("collectionPath="))
       {

           collectionPath=get_setting.replace("collectionPath=","");
           qDebug()<< "Setting the cPath: " << collectionPath ;
           ui->collectionPath->setText(get_setting.replace("collectionPath=",""));
       }
       if(get_setting.contains("toolbarIconSize="))
       {
           iconSize=get_setting.replace("toolbarIconSize=","").toInt();
           qDebug()<< "Setting the tSize: " << iconSize;

           switch(iconSize)
           {
               case 16: ui->toolbarIconSize->setCurrentIndex(0); break;
               case 22: ui->toolbarIconSize->setCurrentIndex(1); break;
               case 24: ui->toolbarIconSize->setCurrentIndex(2); break;
           default: qDebug()<<"error setting icon size";


           }


       }

    }
}

void settings::on_toolButton_2_clicked()
{
    readSettings();
}



bool settings::checkCollection()
{
    QString collection_db_path="../player/collection.db";
    QFileInfo check_db (collection_db_path);

    if (check_db.exists())
    {
        qDebug()<<"La base de datos existe. Ahora la voy a abrir";
       //collection_db.setCollectionDB(collection_db_path);
       qDebug()<<"Ahora obtener la informacion de ella y populate tableView";
       //populateTableView();

       return true;
    }else
    {
        collection_db.prepareCollectionDB(collection_db_path);
        qDebug()<<"Database doesn't exists. Going to create the database and tables";
        return false;
    }
}

void settings::populateDB(QString path)
{
    qDebug() << "Function Name: " << Q_FUNC_INFO << "new path for database action: "<< path;

    QStringList urlCollection;
//QDir dir = new QDir(url);
    QDirIterator it(path, QStringList() << "*.mp4" << "*.mp3" << "*.wav" <<"*.flac" <<"*.ogg", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        urlCollection<<it.next();

        //qDebug() << it.next();
    }

    collection.add(urlCollection);
    //updateList();
   collection_db.setTrackList(collection.getTracks());
    ui->progressBar->setMaximum(urlCollection.size());
   thread->start();
   ui->progressBar->show();



//collection_db.start();

    /*for(auto tr:urlCollection)
    {
        qDebug()<<tr;
    }
    //populateTableView();*/
}

void settings::finishedAddingTracks(bool state)
{
    if(state)
    {
        qDebug()<<"good to hear it g¿finished yay!";
        ui->progressBar->hide();
        emit collectionDBFinishedAdding(true);
    }
}



void settings::on_pushButton_clicked()
{
   QMessageBox::about(this, "Babe Tiny Music Player","Version: 0.0 Alpha\nWritten and designed\nby: Camilo Higuita");

}

