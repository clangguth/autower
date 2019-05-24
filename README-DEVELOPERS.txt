This is a short guide about how to set up your system for compiling autower.
I have done the below steps on a completely fresh Windows XP system, so I should
not have forgotten anything. This worked as of April 17, 2010.
Your mileage may vary as newer versions of tools get released, but in principle
the approach should roughly remain the same.

You will need to have the following software installed:
	- Java JDK, for running Eclipse
	- Eclipse IDE for C/C++ Developers
	- MinGW32 C compiler
	- optional: UPX (ultimate packer for executables)
	- optional: MinGW32 gdb Debugger
	- optional: some file extraction tool that can handle .tar.gz files,
	  e.g., 7Zip, if you want to install the debugger
	  
Note: you can use http://ninite.com/ to install some of the required software in one go.
At the time of writing, the Java JDK and 7Zip could be chosen from there.
This approach could be faster than the steps mentioned below, which install everything
separately. Skip the affected steps, if you've already installed some tools.
IMPORTANT: do NOT install Eclipse from ninite, as we want the C development version, not
the Java development one.

STEP 1: INSTALL JAVA
====================
http://java.sun.com/javase/downloads/
download JDK only (no JFX or other bullshit required)

STEP 2: INSTALL ECLIPSE IDE
===========================
http://www.eclipse.org/downloads/
download "Eclipse IDE for C/C++ Developers"
extract to c:\eclipse-cdt or so

STEP 3: INSTALL COMPILER
========================
http://sourceforge.net/projects/mingw/files/Automated%20MinGW%20Installer
store .exe to (e.g.) c:\temp\mingw\, then start it
the "Minimal" version will do.
install to whichever folder you want, I used the default c:\mingw\
set up the system-wide PATH variable to include mingw\bin folder.
Test it by opening a command prompt and running "gcc". You should get the output
"gcc: no input files".

STEP 4: CHECK IDE AND COMPILER INSTALLATION
===========================================
start up Eclipse, select New->C Project; name it "test", select Executable->
Hello World ANSI C
make sure "MinGW GCC" Toolchain is selected

make sure you can compile and run test.exe, should output "!!!Hello World!!!"
delete the test project.

STEP 5: IMPORT AUTOWER PROJECT
==============================
in Eclipse, select File -> Import..., General -> Import existing projects into workspace,
choose "archive", "autower2-src.zip"

The project provides 3 build configurations:
- Debug: debug version of the .exe
- Release-EXE: release version of the .exe
- Release-DLL: release version of the .dll

Press Ctrl+B to quickly compile the currently active configuration.

STEP 6: INSTALL UPX (optional)
==============================

While not absolutely needed, the release versions are compressed using UPX. The compression
is part of the build process and expects upx.exe to be present in the base directory (same
directory as this file is in). If you don't want to use UPX, delete the corresponding lines
in the project properties, C/C++ build settings -> build steps. Otherwise, download UPX
from http://upx.sourceforge.net/ and put the executable in autower's base directory.

STEP 7: SET UP THE DEBUGGER (optional)
======================================
Download GDB and a required dependency from http://sourceforge.net/projects/mingw/files/
download latest gdb-*-mingw32-bin.tar.gz (under "GNU Source-level debugger")
download latest libexpat-*-mingw32-dll-1.tar.gz (under "MinGW expat")

extract both files (e.g. using 7Zip), and copy the resulting files to your mingw
installation directory (should result a couple of files that go into the "bin" and "share"
folders)

test it using a console: running "gdb" from any directory should work. (press Q to quit)
restart Eclipse, set the "Debug" build configuration as active, build it
right-click on Debug\autower.exe and select Debug As->Local C/C++ Configuration.

Happy coding! :-)
