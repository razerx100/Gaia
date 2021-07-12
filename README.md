# Gaia
3D Engine written using DirectX12 on Win32.

# External Dependencies
Assimp
## Instructions
Put the .dll along with the executable and put the .lib inside
***external/libs/*** folder.

# Third party library Used
ImGui
## Instructions
Use ***Setup.bat*** to include ImGui related files to VS Solution file.
Use ***Setup_No_ImGui.bat*** to include empty ImGui function. Or for 
command line add ***IMGUIFLAG:STRING=ENABLE*** to enable ImGui stuffs
or ***IMGUIFLAG:STRING=DISABLE*** to add empty ImGui functions.

# Tools requirements
Visual Studio 19.\
cmake 3.11+.\
Latest Windows 10 SDK.\
C++20 Standard.
