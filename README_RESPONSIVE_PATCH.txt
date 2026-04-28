Responsive UI Patch for NumericalRootFinder
==========================================

Replace ONLY these files in your project:

include/MainWindow.h
src/MainWindow.cpp

Then in Qt Creator:
1. Build > Clean All
2. Build > Run CMake
3. Build > Rebuild All

What changed:
- Left control panel is now inside a QScrollArea.
- Main body uses a QSplitter, so the left panel and graph/table area resize better.
- Parameter fields no longer use fixed widths.
- Buttons expand to the available width.
- Graph and table use flexible size policies.
- Window minimum size reduced to 760x520 but prevents the layout from being crushed too much.
- If the window gets short, the left panel scrolls instead of destroying the layout.
