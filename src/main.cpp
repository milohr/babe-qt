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

#include <QApplication>
#include "views/babewindow.h"
#include <QStyleFactory>
#include "src/utils/bae.h"
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QStringList files;

    if(argc>1)
        for (int i = 1; i < argc; ++i)
            files << argv[i];

    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Breeze"));

    a.setApplicationName("Babe");
    a.setDesktopFileName("Babe");
    a.setApplicationVersion(BABE_VERSION_STR);

    QCommandLineParser parser;
    parser.setApplicationDescription("Babe music player");
    const QCommandLineOption versionOption = parser.addVersionOption();
    parser.process(a);

    const QStringList args = parser.positionalArguments();
    bool version = parser.isSet(versionOption);

    if(version)
    {
        printf("%s %s\n", qPrintable(QCoreApplication::applicationName()),
               qPrintable(QCoreApplication::applicationVersion()));
        return 0;
    }

    BabeWindow w(files);
    w.start();
    w.show();

    return a.exec();
}
