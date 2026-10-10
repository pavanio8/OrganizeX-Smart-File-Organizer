#include "mainwindow.h"

#include <algorithm>

#include <QFileDialog>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QWidget>
#include <QDir>
#include <QAbstractItemView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {

    setWindowTitle("OrganizeX - Smart File Organizer");
    resize(1100, 720);

    auto *central = new QWidget(this);

    auto *root = new QVBoxLayout(central);

    root->setContentsMargins(28, 24, 28, 22);
    root->setSpacing(16);

    // Header
    auto *top = new QHBoxLayout();

    auto *titleBox = new QVBoxLayout();

    auto *title = new QLabel("OrganizeX");
    title->setObjectName("title");

    auto *subtitle = new QLabel(
        "A smarter, safer way to organize your files"
    );

    subtitle->setObjectName("subtitle");

    titleBox->addWidget(title);
    titleBox->addWidget(subtitle);

    top->addLayout(titleBox);
    top->addStretch();

    themeButton = new QPushButton("Toggle theme");
    themeButton->setObjectName("secondaryButton");

    top->addWidget(themeButton);

    root->addLayout(top);

    // Folder selection
    auto *folderRow = new QHBoxLayout();

    folderEdit = new QLineEdit();

    folderEdit->setPlaceholderText(
        "Select a folder to organize..."
    );

    folderEdit->setMinimumHeight(42);

    browseButton = new QPushButton("Browse Folder");
    browseButton->setMinimumHeight(42);

    folderRow->addWidget(folderEdit, 1);
    folderRow->addWidget(browseButton);

    root->addLayout(folderRow);

    // Dashboard statistics
    auto *stats = new QHBoxLayout();

    auto *totalCard = new QWidget();
    totalCard->setObjectName("card");

    auto *totalLayout = new QVBoxLayout(totalCard);

    totalLayout->addWidget(new QLabel("FILES FOUND"));

    totalValue = new QLabel("0");
    totalValue->setObjectName("statValue");

    totalLayout->addWidget(totalValue);

    auto *categoryCard = new QWidget();
    categoryCard->setObjectName("card");

    auto *categoryLayout = new QVBoxLayout(categoryCard);

    categoryLayout->addWidget(new QLabel("CATEGORIES"));

    categoryValue = new QLabel("0");
    categoryValue->setObjectName("statValue");

    categoryLayout->addWidget(categoryValue);

    stats->addWidget(totalCard);
    stats->addWidget(categoryCard);

    root->addLayout(stats);

    // Preview section
    auto *sectionTitle = new QLabel("Organization Preview");
    sectionTitle->setObjectName("sectionTitle");

    root->addWidget(sectionTitle);

    fileTable = new QTableWidget(0, 4);

    fileTable->setHorizontalHeaderLabels({
        "File Name",
        "Category",
        "Destination Folder",
        "Status"
    });

    fileTable->horizontalHeader()->setSectionResizeMode(
        0, QHeaderView::Stretch
    );

    fileTable->horizontalHeader()->setSectionResizeMode(
        1, QHeaderView::ResizeToContents
    );

    fileTable->horizontalHeader()->setSectionResizeMode(
        2, QHeaderView::Stretch
    );

    fileTable->horizontalHeader()->setSectionResizeMode(
        3, QHeaderView::ResizeToContents
    );

    fileTable->verticalHeader()->setVisible(false);

    fileTable->setEditTriggers(
        QAbstractItemView::NoEditTriggers
    );

    fileTable->setSelectionBehavior(
        QAbstractItemView::SelectRows
    );

    fileTable->setAlternatingRowColors(true);

    root->addWidget(fileTable, 1);

    // Action buttons
    auto *bottom = new QHBoxLayout();

    confirmCheck = new QCheckBox(
        "I checked the preview and want to move these files"
    );

    previewButton = new QPushButton("Preview Files");
    previewButton->setObjectName("secondaryButton");

    organizeButton = new QPushButton("Organize Files");
    organizeButton->setObjectName("primaryButton");

    bottom->addWidget(confirmCheck, 1);
    bottom->addWidget(previewButton);
    bottom->addWidget(organizeButton);

    root->addLayout(bottom);

    // Status
    statusLabel = new QLabel(
        "Choose a folder and preview its files. Nothing moves during preview."
    );

    statusLabel->setObjectName("status");

    root->addWidget(statusLabel);

    setCentralWidget(central);

    // Connect buttons to functions.
    connect(
        browseButton,
        &QPushButton::clicked,
        this,
        &MainWindow::chooseFolder
    );

    connect(
        previewButton,
        &QPushButton::clicked,
        this,
        &MainWindow::loadPreview
    );

    connect(
        organizeButton,
        &QPushButton::clicked,
        this,
        &MainWindow::organizeFiles
    );

    connect(
        themeButton,
        &QPushButton::clicked,
        this,
        &MainWindow::toggleTheme
    );

    toggleTheme();
}


// Choose a folder.
void MainWindow::chooseFolder() {

    QString folder = QFileDialog::getExistingDirectory(
        this,
        "Choose Folder"
    );

    if (!folder.isEmpty()) {

        folderEdit->setText(
            QDir::toNativeSeparators(folder)
        );

        confirmCheck->setChecked(false);

        loadPreview();
    }
}


// Update dashboard statistics.
void MainWindow::refreshStats(
    const std::vector<FileItem>& items
) {

    totalValue->setText(
        QString::number(items.size())
    );

    std::vector<std::string> categories;

    for (const auto& item : items) {

        if (
            std::find(
                categories.begin(),
                categories.end(),
                item.category
            ) == categories.end()
        ) {

            categories.push_back(item.category);
        }
    }

    categoryValue->setText(
        QString::number(categories.size())
    );
}


// Preview files.
void MainWindow::loadPreview() {

    QString folder = folderEdit->text().trimmed();

    if (folder.isEmpty()) {

        QMessageBox::information(
            this,
            "Choose Folder",
            "Please choose a folder first."
        );

        return;
    }

    if (!QDir(folder).exists()) {

        QMessageBox::warning(
            this,
            "Invalid Folder",
            "The selected folder does not exist."
        );

        return;
    }

    auto items = organizer.preview(
        folder.toStdString()
    );

    fileTable->setRowCount(0);

    for (const auto& item : items) {

        int row = fileTable->rowCount();

        fileTable->insertRow(row);

        fileTable->setItem(
            row, 0,
            new QTableWidgetItem(
                QString::fromStdString(
                    item.source.filename().string()
                )
            )
        );

        fileTable->setItem(
            row, 1,
            new QTableWidgetItem(
                QString::fromStdString(item.category)
            )
        );

        fileTable->setItem(
            row, 2,
            new QTableWidgetItem(
                QString::fromStdString(
                    item.category + "/"
                )
            )
        );

        fileTable->setItem(
            row, 3,
            new QTableWidgetItem(
                item.conflict
                    ? "Name Conflict"
                    : "Ready"
            )
        );
    }

    refreshStats(items);

    confirmCheck->setChecked(false);

    setStatus(
        QString("Preview ready: %1 file(s). Nothing has been moved.")
            .arg(items.size())
    );
}


// Organize files.
void MainWindow::organizeFiles() {

    if (folderEdit->text().trimmed().isEmpty()) {

        QMessageBox::information(
            this,
            "Choose Folder",
            "Please choose a folder first."
        );

        return;
    }

    if (!confirmCheck->isChecked()) {

        QMessageBox::warning(
            this,
            "Preview Required",
            "Preview the files and tick the confirmation checkbox first."
        );

        return;
    }

    auto answer = QMessageBox::question(
        this,
        "Confirm Organization",
        "Move the files into their category folders now?",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );

    if (answer != QMessageBox::Yes) {
        return;
    }

    auto result = organizer.organize(
        folderEdit->text().trimmed().toStdString()
    );

    QString report = QString(
        "Organization completed.\nFiles moved: %1\nFiles skipped: %2"
    )
    .arg(result.moved)
    .arg(result.skipped);

    setStatus(report);

    loadPreview();

    QMessageBox::information(
        this,
        "OrganizeX Report",
        report
    );
}


// Display status.
void MainWindow::setStatus(
    const QString& message
) {

    statusLabel->setText(message);
}


// Switch between dark and light themes.
void MainWindow::toggleTheme() {

    darkTheme = !darkTheme;

    if (darkTheme) {

        setStyleSheet(R"(
            QMainWindow, QWidget {
                background: #111827;
                color: #e5e7eb;
                font-family: "Segoe UI";
                font-size: 10pt;
            }

            QLabel#title {
                font-size: 28pt;
                font-weight: 700;
                color: #f9fafb;
            }

            QLabel#subtitle {
                color: #9ca3af;
                font-size: 11pt;
            }

            QLabel#sectionTitle {
                font-size: 16pt;
                font-weight: 600;
            }

            QWidget#card {
                background: #1f2937;
                border: 1px solid #374151;
                border-radius: 12px;
            }

            QLabel#statValue {
                color: #a78bfa;
                font-size: 25pt;
                font-weight: 700;
            }

            QLineEdit, QTableWidget {
                background: #1f2937;
                border: 1px solid #374151;
                border-radius: 7px;
                padding: 8px;
                gridline-color: #374151;
            }

            QHeaderView::section {
                background: #263244;
                color: #e5e7eb;
                padding: 9px;
                border: none;
            }

            QPushButton {
                border-radius: 8px;
                padding: 10px 16px;
                font-weight: 600;
                background: #374151;
                color: white;
            }

            QPushButton#primaryButton {
                background: #7c3aed;
            }

            QPushButton#secondaryButton {
                background: #374151;
            }

            QLabel#status {
                color: #a5b4fc;
                padding: 5px;
            }
        )");

        themeButton->setText("Switch to Light Theme");

    } else {

        setStyleSheet(R"(
            QMainWindow, QWidget {
                background: #f3f4f6;
                color: #111827;
                font-family: "Segoe UI";
                font-size: 10pt;
            }

            QLabel#title {
                font-size: 28pt;
                font-weight: 700;
                color: #111827;
            }

            QLabel#subtitle {
                color: #6b7280;
                font-size: 11pt;
            }

            QLabel#sectionTitle {
                font-size: 16pt;
                font-weight: 600;
            }

            QWidget#card {
                background: white;
                border: 1px solid #e5e7eb;
                border-radius: 12px;
            }

            QLabel#statValue {
                color: #6d28d9;
                font-size: 25pt;
                font-weight: 700;
            }

            QLineEdit, QTableWidget {
                background: white;
                border: 1px solid #d1d5db;
                border-radius: 7px;
                padding: 8px;
            }

            QHeaderView::section {
                background: #e5e7eb;
                color: #111827;
                padding: 9px;
                border: none;
            }

            QPushButton {
                border-radius: 8px;
                padding: 10px 16px;
                font-weight: 600;
                background: #e5e7eb;
                color: #111827;
            }

            QPushButton#primaryButton {
                background: #6d28d9;
                color: white;
            }

            QLabel#status {
                color: #5b21b6;
                padding: 5px;
            }
        )");

        themeButton->setText("Switch to Dark Theme");
    }
}