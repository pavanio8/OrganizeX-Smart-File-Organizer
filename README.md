OrganizeX – Smart File Organizer

OrganizeX is a desktop-based Smart File Organizer developed using C++17 and Qt 6. It helps users organize files automatically into categorized folders based on their file extensions.

The application provides a simple graphical user interface with preview functionality, file statistics, and dark/light themes to make file management easier and safer.

✨ Features

- Automatic File Organization: Sorts files into appropriate folders based on file extensions.
- File Preview: Displays the files and their destination categories before organizing.
- Smart Categorization: Supports documents, images, videos, audio, archives, programming files, and other file types.
- Duplicate Detection: Detects existing files with the same destination name and skips conflicts instead of overwriting them.
- Dashboard Statistics: Displays the total number of files found and categories detected.
- Dark and Light Themes: Switch between two interface themes.
- User Confirmation: Requires confirmation before moving files.
- Graphical User Interface: Built with Qt 6 for a user-friendly experience.

🗂️ Supported File Categories

Category| Supported Extensions
Documents| PDF, DOC, DOCX, TXT, PPT, PPTX, XLS, XLSX
Images| JPG, JPEG, PNG, GIF, BMP, SVG, WEBP
Videos| MP4, MKV, AVI, MOV
Audio| MP3, WAV, AAC, FLAC
Archives| ZIP, RAR, 7Z, TAR, GZ
Code| C, C++, H, HPP, Java, Python, JS, HTML, CSS
Others| All other unsupported extensions

🛠️ Technologies Used

- Programming Language: C++17
- GUI Framework: Qt 6 Widgets
- Build System: CMake
- File Management: C++ STL and "std::filesystem"
- Data Structures: Vectors and Unordered Maps

📁 Project Structure

OrganizeX/
│
├── include/
│   └── organizer.h
│
├── src/
│   ├── main.cpp
│   ├── mainwindow.cpp
│   ├── mainwindow.h
│   └── organizer.cpp
│
├── CMakeLists.txt
├── .gitignore
└── README.md

⚙️ Requirements

Before running the project, make sure you have:

- C++17 compatible compiler
- Qt 6 installed
- CMake 3.16 or later
- Qt 6 Widgets component

🚀 Installation and Setup

1. Clone the Repository

git clone YOUR_REPOSITORY_URL

2. Navigate to the Project Folder

cd OrganizeX

3. Configure the Project

cmake -S . -B build

4. Build the Project

cmake --build build

5. Run the Application

Windows:

build/Debug/OrganizeX.exe

If your build configuration produces the executable directly inside the build folder:

build/OrganizeX.exe

Linux:

./build/OrganizeX

📖 How to Use

1. Launch the OrganizeX application.
2. Click Browse Folder to select the folder you want to organize.
3. Click Preview Files to view the files and their destination categories.
4. Review the preview carefully.
5. Tick the confirmation checkbox.
6. Click Organize Files.
7. Confirm the action when prompted.
8. Your files will be moved into their respective category folders.

Note: Previewing files does not move or modify them. Actual file movement happens only after confirmation.

🔒 Safety and File Handling

- Existing destination files are not overwritten.
- Files with name conflicts are skipped.
- Users can review the preview before organizing.
- The application processes regular files in the selected folder, not files inside nested subfolders.

Important: Organizing moves files from their original locations. Keep a backup of important files before performing the operation.

🔮 Future Improvements

- Recursive folder organization.
- Undo and restore functionality.
- Custom file categories.
- Search and filter options.
- File size and storage statistics.
- Organization history and activity logs.
- Drag-and-drop folder selection.

👥 Contributors

Developed as a collaborative C++ desktop application project.

Add the GitHub usernames of all team members here.

📄 License

This project is open-source and available for educational and development purposes.
