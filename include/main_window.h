// Copyright 2020 Petr Petrovich Petrov. All rights reserved.
// License: https://github.com/PetrPPetrov/gkm-ship-cad/blob/master/LICENSE

#pragma once

#include <memory>
#include <QObject>
#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QDockWidget>
#include <QPlainTextEdit>
#include <QTreeView>
#include <QTableView>
#include <QThread>
#include <QMdiSubWindow>
#include <QListWidget>
#include "ui_main_window.h"
#include "view_3d_widget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void showEvent(QShowEvent* event) override;

private:
    bool first_show = true;
    Ui::MainWindow main_window;

    View3DWidget* view_3d_widget = nullptr;
    QMdiSubWindow* view_3d_window = nullptr;
    QPlainTextEdit* log_view = nullptr;
    QMdiSubWindow* log_window = nullptr;
};

extern MainWindow* g_main_window;
