#include "settings.h"
#include "ui_settings.h"
#include "QDebug"
#include <QDirIterator>
#include <QFileDialog>
#include <QDebug>
#include <fstream>
#include<iostream>
#include<QStringList>


settings::settings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::settings)
{
    ui->setupUi(this);
}

settings::~settings()
{
    delete ui;
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


if(collectionPath!=url)
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


