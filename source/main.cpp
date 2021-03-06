// Copyright 2020 Petr Petrovich Petrov. All rights reserved.
// License: https://github.com/PetrPPetrov/gkm-ship-cad/blob/master/LICENSE

#include <QDesktopWidget>
#include <QSplashScreen>
#include <QApplication>
#include <QStringList>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include <QDebug>
#include <QtGlobal>
#include "main_window.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    MainWindow main_window;

    // Center main window on desktop
    QDesktopWidget screen;
    QRect screen_rect = screen.screenGeometry(&main_window);
    main_window.resize(screen_rect.width() * 3 / 4, screen_rect.height() * 3 / 4);
    QPoint position((screen_rect.width() - main_window.width()) / 2, (screen_rect.height() - main_window.height()) / 2);
    main_window.move(position);
    main_window.show();
    return application.exec();
}
