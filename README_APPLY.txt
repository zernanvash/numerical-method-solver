NumericalRootFinder UI Config + Export Update
=============================================

Copy these files into your project:

1) Copy all files inside include/ to:
   NumericalRootFinder/include/

2) Copy all files inside src/ to:
   NumericalRootFinder/src/

3) Replace your root CMakeLists.txt with the included CMakeLists.txt.

4) In Qt Creator:
   Build > Clean All
   Build > Run CMake
   Build > Rebuild All

Added features:
- In-app [ SETTINGS ] dialog
- Theme selector: Amber, Yellow, Green, Red, Blue, White
- UI scale and font size config
- Animation speed config
- Toggle window scanlines/vignette
- Toggle graph scanlines/dither/vignette
- Boot splash on/off
- Default tolerance and max iteration config
- Export TXT report
- Export CSV spreadsheet-compatible data

Notes:
- CSV can be opened directly in Microsoft Excel.
- The app writes config.ini beside the .exe at runtime.
