// Copyright 2020 Petr Petrovich Petrov. All rights reserved.
// License: https://github.com/PetrPPetrov/gkm-ship-cad/blob/master/LICENSE

#include <QMenu>
#include <QToolBar>
#include <QDockWidget>
#include <QTextEdit>
#include <QStatusBar>
#include <QLineEdit>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QHeaderView>
#include <QPushButton>
#include "main_window.h"

extern MainWindow* g_main_window = nullptr;

MainWindow::MainWindow()
{
    g_main_window = this;
    main_window.setupUi(this);

    auto cube = std::make_shared<Gkm::Solid::Cube>();
    auto sphere = std::make_shared<Gkm::Solid::Sphere>();
    auto translate = std::make_shared<Gkm::Solid::TransformOperator>();
    translate->translate = Eigen::Vector3d(1.0, 1.0, 1.0);
    translate->solid = sphere;
    auto difference = std::make_shared<Gkm::Solid::DifferenceOperator>();
    difference->left = cube;
    difference->right = translate;

    solid = difference;
    std::srand(static_cast<unsigned int>(time(0)));

    view_3d_widget = new View3DWidget(main_window.centralwidget);
    view_3d_window = main_window.centralwidget->addSubWindow(view_3d_widget);
    view_3d_window->setWindowTitle(tr("3D View"));

    log_view = new QPlainTextEdit(main_window.centralwidget);
    log_view->setReadOnly(true);
    log_view->ensureCursorVisible();
    log_view->setCenterOnScroll(true);
    log_window = main_window.centralwidget->addSubWindow(log_view);
    log_window->setWindowTitle(tr("Log"));
}

Gkm::Solid::ISolid::Ptr MainWindow::getSolid() const
{
    return solid;
}

void MainWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);

    if (first_show)
    {
        first_show = false;

        view_3d_window->resize(QSize(main_window.centralwidget->width() / 3, main_window.centralwidget->height() * 3 / 4));
        view_3d_window->move(main_window.centralwidget->width() * 2 / 3, 0);
    }
}
