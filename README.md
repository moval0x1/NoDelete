# NoDelete
NoDelete is a tool that assists in malware analysis by locking a folder where malware drops files before deleting them.

[![Version](https://img.shields.io/badge/version-0.1.0-blue.svg)](https://github.com/moval0x1/NoDelete)
[![Platform](https://img.shields.io/badge/platform-Windows-blue.svg)](https://www.microsoft.com/windows)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
![GitHub all releases](https://img.shields.io/github/downloads/moval0x1/NoDelete/total)
![GitHub release downloads](https://img.shields.io/github/downloads/moval0x1/NoDelete/latest/total)

## Note
> This project is being created mainly for my personal studies of Qt and C++. As soon as I learn new things, I will improve it all.

If you have any suggestions, feel free to contact me.

## Let's work!
### Update Notes 12/2024
- Added `config.ini` 
- Added logging for monitored folders
- Added ListView with paths
- Added option to open folders from ListView
- Implemented multi-threading
- Added functionality to restore original folder permissions
- Read from environment variable
- Added ``app.manifest``

### How it looks like now?
Now you can set the folder you would like to lock in a **``config.ini``** file.

### Configuration
* **Directories:** All the directories you want to monitor and lock.
* **LogFile:** Name of the log file (default is in the same path as the binary).

![NoDelete-Config-INI](/imgs/NoDelete-config-ini.png)

When **NoDelete** loads the file, it will convert the environment variables and display them in a user-friendly format.

![NoDelete-Main](/imgs/NoDelete-main.png)

You can also open the directory to inspect the files that the malware wrote there. Just right-click on the line and select **Open Directory**.

![NoDelete-OpenDirectory](/imgs/NoDelete-OpenDirectory.png)

### Before Execution
Before running **NoDelete**, you will have full permissions on the target folder:

![NoDelete-PublicFolder-Before](/imgs/NoDelete-PublicFolder-Before.png)

### After Execution
Once all folders are locked, you will see that only "Everyone" is allowed to perform specific actions:

![NoDelete-PublicFolder-After](/imgs/NoDelete-PublicFolder-After.png)

A log will help you validate if anything went wrong and will also provide details about the success of locking the folders.

![NoDelete-logFile](/imgs/NoDelete-logFile.png)

After using **NoDelete**, files inside the locked folder cannot be deleted. This allows you to lock a folder used by malware to drop files, ensuring the files remain intact for further investigation.

![FakeMalware](/imgs/FakeMalware.png)

You can see all activities recorded in the log file:

![NoDelete-FinalLog](/imgs/NoDelete-FinalLog.png)

## TO DO
- [ ] Save events to **EventViewer**
- [ ] **CLI** option
