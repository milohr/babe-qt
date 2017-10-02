/*
   Babe - tiny music player
   Copyright (C) 2017  Camilo Higuita
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

   */




#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{

    QStringList files;

    if(argc>1)
    {

        for (int i = 1; i < argc; ++i)
        {
            files << argv[i];
        }
    }

    QApplication a(argc, argv);
    a.setApplicationName("Babe");
    a.setDesktopFileName("Babe");
    a.setApplicationVersion("0.5 Beta");
    MainWindow w(files);
    w.start();
    w.show();
    return a.exec();
}
