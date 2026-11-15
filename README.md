<<<<<<< HEAD
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

This project was developed collaboratively by:
1. Saranya Sai
2. Rajiv
3. Rithwik Siva Sai
4. Pavani Thulasi

Each team member contributed to the development, implementation, and improvement of OrganizeX – Smart File Organizer.

📄 License

This project is open-source and available for educational and development purposes.
=======
# OrganizeX — Smart File Organizer

OrganizeX is a local web application with a C++17 HTTP backend and a responsive, vanilla-JavaScript dashboard. Uploaded files are first saved to `OrganizeX_Data/Inbox`; the **Organize inbox** action moves them into category folders after confirmation. Files are not deleted by the application.

## Requirements

- CMake 3.16 or newer
- A C++17 compiler (for example, MSVC 2019+, GCC 8+, or Clang 7+)
- Git and an internet connection for the first CMake configure, to fetch cpp-httplib and nlohmann/json
- Node.js 18 or newer to serve the frontend

The backend binds only to `127.0.0.1:8080`. Its default storage root is `OrganizeX_Data` in the process working directory. Uploads are limited to 50 MB per file, 100 MB per request, and 100 files per request.

## Run on Windows

From the project directory, double-click `start-organizex.bat`, or run:

```powershell
cmake -S backend -B backend\build
cmake --build backend\build --config Release
node frontend\server.js
```

In a second terminal, start the backend from the project root (so its data folder is created in the expected location):

```powershell
.\backend\build\Release\organizeX_server.exe
```

For single-configuration CMake generators, the executable may instead be at `backend\build\organizeX_server.exe`. Open the frontend at [http://127.0.0.1:5500](http://127.0.0.1:5500); its configured API base URL is `http://127.0.0.1:8080`. The batch launcher builds the backend, starts the frontend in a second window, and runs the backend in the current window.

## API

| Method | Route | Purpose |
|---|---|---|
| `GET` | `/api/status` | Backend connection and status |
| `POST` | `/api/upload` | Upload one or more multipart fields named `files` |
| `GET` | `/api/preview` | Preview inbox category and relative destination without moving files |
| `POST` | `/api/organize` | Move inbox files after user confirmation; reports each result |
| `GET` | `/api/statistics` | File totals, storage, pending/organized counts, and category counts |

Example checks:

```powershell
Invoke-RestMethod http://127.0.0.1:8080/api/status
Invoke-RestMethod http://127.0.0.1:8080/api/preview
Invoke-RestMethod http://127.0.0.1:8080/api/statistics
```

The API does not return absolute filesystem paths. Unrecognized extensions are safely categorized as `Others`. Duplicate names receive a numeric suffix. Symlinks are excluded from file listings, and failed moves leave the original in Inbox.

## Storage layout

```text
OrganizeX_Data/
├── Inbox/
└── Organized/
    ├── Documents/
    ├── Images/
    ├── Videos/
    ├── Audio/
    ├── Archives/
    ├── Code/
    └── Others/
```

## End-to-end smoke test

1. Start the server and open `http://127.0.0.1:5500`.
2. Confirm the green backend indicator appears.
3. Select several local files, including repeated filenames from separate folders if available, then upload.
4. Confirm the files appear in `OrganizeX_Data\Inbox` and in the organization preview.
5. Check the detected categories and destination paths before organizing.
6. Click **Organize inbox**, accept the confirmation, and verify files move into `OrganizeX_Data\Organized\<Category>`.
7. Refresh the dashboard and check totals, storage, and per-category counts.
8. Try an unknown extension; it is categorized as `Others`. Try a file over 50 MB, an empty upload selection, and a disconnected backend to verify the error and reconnect states.

The upload endpoint uses multipart form data and partial success: any files accepted are saved, while rejected file results are returned in `errors`. Each result includes the file's zero-based index in the submitted `files` fields so the frontend can retain only failed selections for retry. The frontend reports these results and refreshes from the API.
>>>>>>> 8b99b2b (Initial OrganizeX project)
