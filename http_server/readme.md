# README #
This is a HTTP Server example showing how haystack's protocol query ops, serialization and, serialization can be used.

This project depends on Poco HTTP classes so you'll need Poco to build this server.

### Build env setup ###

* Get Poco [http://pocoproject.org/download/index.html] and read the following:
Getting started [http://pocoproject.org/docs/00200-GettingStarted.html]

### A short guide for Win32 with Visual Studio ###

* extract Poco to a folder.
* follow the instruction on how to build, for example:
	buildwin.cmd 110 build shared both Win32 nosamples
* add an user env entry: POCO_ROOT={path\to\poco}
* run `cmake ..\` in the root `haystack-cpp\vs` folder, or use Cmakegui and set source code to 
  `[path\to\]haystack-cpp` and build binaries to `[path\to\]haystack-cpp\vs` and click 'Configure' and if all is good click on 'Generate'
* open the 'haystack-cpp.sln' from `haystack-cpp\vs`, select http_server and build the project.
* The executable can be found on `haystack-cpp\vs\bin`