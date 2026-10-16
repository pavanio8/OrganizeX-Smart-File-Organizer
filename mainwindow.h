#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QCheckBox>

#include "organizer.h"

class MainWindow : public QMainWindow {

    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void chooseFolder();
    void loadPreview();
    void organizeFiles();
    void toggleTheme();

private:
    void refreshStats(const std::vector<FileItem>& items);
    void setStatus(const QString& message);

    Organizer organizer;

    QLineEdit *folderEdit;

    QPushButton *browseButton;
    QPushButton *previewButton;
    QPushButton *organizeButton;
    QPushButton *themeButton;

    QCheckBox *confirmCheck;

    QTableWidget *fileTable;

    QLabel *totalValue;
    QLabel *categoryValue;
    QLabel *statusLabel;

    bool darkTheme = true;
};

#endif