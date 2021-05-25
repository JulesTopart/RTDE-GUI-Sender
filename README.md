# UR-RTDE Sender
This tool was made to facilitate 3D printing on an Universal robot.

## Installation

Dowload the realease [here]()



## For developers

**Only compatible with 64bit architecture, Please make sure your toolchain is setup in 64Bit**
Create a new project folder were you want

```
mkdir myProject
cd myProject
```

### Pre-requisite :
 - [Download](https://sourceforge.net/projects/boost/files/boost-binaries/1.76.0) and install Boost binairies according to your Visual Studio version in your project directory (14.2 x64 worked for me),
 - [Download](https://cmake.org/download/) and install CMake x64
 - [Download](https://visualstudio.microsoft.com/fr/vs/) and install Visual Studio Community
 - Install Qt VS Tools using visual studio extension manager. Then restart Visual studio
 
 
### Building dependencies

First of all we have to compile ur_rtde library. The most repetable process is to use CMake and MSBuild to do so.
Make sure you've installed CMake and VS, please also check your environnement variable, the should both be added to PATH.
MSBuild is usualy located into : ```C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin```



```
git clone https://gitlab.com/sdurobotics/ur_rtde.git
cd ur_rtde
mkdir build
cd build
cmake -DBOOST_ROOT="C:\local\boost_1_76_0" -DBOOST_LIBRARYDIR="C:\local\boost_1_76_0\lib64-msvc-14.2" -DPYTHON_BINDINGS=OFF ..
MSBuild.exe ur_rtde.sln /property:Configuration=Release /maxcpucount:8
```

ur_rtde should be compiled into Realease folder.

### Setting up the Visual Studio environnement.

Into your project folder clone this repo : ```https://github.com/JulesTopart/RTDE-GUI-Sender.git```
Then open the ```RTDE-fdm-gui.sln``` file.
In Visual studio, press ```Alt+Enter``` to open project properties.
In the upper dropdown select All Configuration and All Platform to avoid any errors...

 - Go into ```C/C++/Include directories``` and paste the followoing : ```C:\dev\boost_1_76_0;C:\dev\ur_rtde\include;%(AdditionalIncludeDirectories)```
 - Go into ```Linker/Additionnal Include Directories``` and paste the following : ```C:\dev\boost_1_76_0\lib64-msvc-14.2;C:\dev\ur_rtde\build\Release;%(AdditionalLibraryDirectories)```
 - Go into ```Linker/Input``` and paste the following : ```rtde.lib;%(AdditionalDependencies)```

The dependencies are now setup. Now we have to setup Qt VS Tools. In the upped nav bar, into ```Extensions/Qt VS Tools/Qt Version```
Add your 64bit Qmake Path. (ex : ```C:\Qt\5.15.2\msvc2019_64\bin\qmake.exe```)

Go back into project setting (```Alt+Enter```) And navigate to Qt Project setting. 
Select your fresh Qt installation and add those modules ```core;gui;widgets;charts;network;concurrent```
Set the build config to Realease and your good to go !
