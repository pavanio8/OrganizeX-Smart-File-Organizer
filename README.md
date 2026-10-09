📁 OrganizeX – Smart File Organizer

OrganizeX is a C++-based smart file organization tool designed to automatically arrange files into categorized folders based on their file types. It helps users keep their directories clean, structured, and easy to manage.

🚀 Features

- 📂 Automatically organizes files into categories
- 📝 Supports common file types such as documents, images, videos, audio, and archives
- ⚡ Fast and efficient file organization using C++
- 🗂️ Creates category folders automatically
- 🔄 Reduces manual file management
- 🛡️ Handles files safely and avoids unnecessary changes
- 💻 Simple command-line interface

 Technologies Used

- C++
- C++17 / Modern C++
- STL
- File System Library ("<filesystem>")

📁 Project Structure

OrganizeX-Smart-File-Organizer/
│
├── src/
│   └── main.cpp
│
├── include/
│   └── organizer.h
│
├── README.md
├── .gitignore
└── LICENSE

⚙️ How It Works

OrganizeX scans the selected directory, identifies files based on their extensions, and moves them into appropriate folders.

Example:

Before:

Downloads/
├── photo.jpg
├── resume.pdf
├── song.mp3
├── video.mp4
└── project.zip

After running OrganizeX:

Downloads/
├── Images/
│   └── photo.jpg
│
├── Documents/
│   └── resume.pdf
│
├── Audio/
│   └── song.mp3
│
├── Videos/
│   └── video.mp4
│
└── Archives/
    └── project.zip

▶️ How to Run

1. Clone the repository

git clone https://github.com/pavanio8/OrganizeX-Smart-File-Organizer.git

2. Open the project

Open the project in Visual Studio, VS Code, Code::Blocks, or another C++ IDE.

3. Compile

Using g++:

g++ -std=c++17 src/main.cpp -o OrganizeX

4. Run

./OrganizeX

On Windows:

OrganizeX.exe

🎯 Objective

The main objective of OrganizeX is to automate repetitive file-management tasks and provide a simple solution for maintaining an organized digital workspace.

🔮 Future Enhancements

- 🔍 Duplicate file detection
- ✏️ Bulk file renaming
- 📊 File statistics and storage analysis
- 🖥️ Graphical User Interface (GUI)
- ⚙️ Custom folder/category configuration
- 🧠 Smarter file classification
- 📅 Automatic scheduled organization


⭐ Contribute

Suggestions, improvements, and contributions are welcome. Feel free to fork the repository and submit a pull request.

📄 License

This project is open-source and available under the MIT License.
