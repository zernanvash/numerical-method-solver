# Numerical Method Solver

## Project Overview

**Numerical Method Solver** is a C++ desktop application that solves nonlinear equations using three common root-finding methods:

- Bisection Method
- Newton-Raphson Method
- Secant Method

The application includes a graphical user interface built with **Qt 6**. It allows the user to enter a mathematical function, choose a numerical method, provide the required input values, and view the result through both a table and a graph.

The interface uses a retro CRT-inspired design with configurable color themes, scanline effects, graph animations, and export features.

---

## Purpose of the Project

The purpose of this project is to help users understand how numerical root-finding methods work. Instead of only showing the final answer, the application also displays the iteration process so users can see how each method approaches the root.

This makes the application useful for:

- learning numerical methods
- comparing different root-finding algorithms
- visualizing function behavior
- exporting computed results for documentation or reports

---

## Technologies Used

| Component | Technology |
|---|---|
| Programming Language | C++ |
| GUI Framework | Qt 6 Widgets |
| Build System | CMake |
| Compiler | MinGW / MSVC compatible |
| Graph Rendering | Custom `QPainter` widget |
| Settings System | Qt-based in-app configuration |
| Export Formats | TXT and CSV |

---

## Main Features

## 1. Function Input

The user can enter a mathematical expression such as:

```txt
x^3 - x - 2
```

The function parser supports common operators and functions:

```txt
+  -  *  /  ^
sin(x), cos(x), tan(x)
sqrt(x), log(x), ln(x), log10(x)
exp(x), abs(x)
pi, e
```

Example supported inputs:

```txt
x^2 - 4
sin(x) - 0.5
exp(x) - 3
x^3 - x - 2
```

---

## 2. Supported Numerical Methods

## Bisection Method

The Bisection Method is a bracketing method. It needs two starting values, `a` and `b`, where the function changes sign.

This means:

```txt
f(a) and f(b) must have opposite signs
```

The method repeatedly divides the interval in half until the root is found within the given tolerance.

### Required inputs:

```txt
a
b
tolerance
maximum iterations
```

### Best used when:

- the root is known to be inside an interval
- reliability is more important than speed
- the function changes sign between two points

---

## Newton-Raphson Method

The Newton-Raphson Method uses one starting guess and the slope of the function to move toward the root.

The application estimates the derivative numerically, so the user does not need to manually enter the derivative.

### Required inputs:

```txt
x0
tolerance
maximum iterations
```

### Best used when:

- the starting guess is close to the root
- faster convergence is needed
- the function behaves smoothly near the root

---

## Secant Method

The Secant Method uses two starting guesses. It is similar to Newton-Raphson, but it does not require computing the derivative directly.

Instead, it uses a line through two points on the function to estimate the next root approximation.

### Required inputs:

```txt
x0
x1
tolerance
maximum iterations
```

### Best used when:

- two starting guesses are available
- a derivative is not known
- faster convergence than bisection is desired

---

## 3. Graphical Visualization

The application includes a custom graph panel that displays:

- the plotted function curve
- x-axis and y-axis
- grid lines
- current root approximation
- method-specific construction lines

Each method has a different visual behavior:

| Method | Graph Visualization |
|---|---|
| Bisection | Shows the shrinking interval between `a` and `b` |
| Newton-Raphson | Shows tangent-line movement toward the root |
| Secant | Shows secant lines formed by two approximation points |

The graph includes CRT-style effects such as scanlines, dithering, vignette shading, and animated graph reveal.

---

## 4. Iteration Table

Every computation produces an iteration table.

The table includes:

```txt
Iteration number
Current x value
f(x)
Error
Status
```

Example:

| Iteration | X Value | f(X) | Error | Status |
|---|---:|---:|---:|---|
| 1 | 1.500000 | -0.125000 | 0.500000 | INIT |
| 2 | 1.750000 | 1.609375 | 0.250000 | UPDATE |
| 3 | 1.625000 | 0.666016 | 0.125000 | UPDATE |

Possible status values:

```txt
INIT
UPDATE
CONVERGED
MAX_ITER
```

---

## 5. In-App Settings

The application includes an in-app settings panel. This allows users to customize the interface without editing the source code.

Available settings include:

```txt
Theme color
UI scale
Font size
Animation speed
Scanlines on/off
Vignette on/off
Graph dithering on/off
Boot splash on/off
Default tolerance
Default maximum iterations
```

---

## 6. Color Themes

The user can switch between multiple monochrome CRT-style themes.

Available themes:

```txt
Amber
Yellow
Green
Red
Blue
White
```

These themes change the visual style of the interface while keeping the same layout and functionality.

---

## 7. Export Features

The application can export computation results into external files.

Supported export formats:

```txt
TXT
CSV
```

### TXT Export

The TXT export creates a readable report containing:

- function used
- selected method
- tolerance
- maximum iterations
- final root
- final function value
- iteration table

### CSV Export

The CSV export saves the iteration table in a spreadsheet-friendly format.

CSV files can be opened using:

- Microsoft Excel
- Google Sheets
- LibreOffice Calc
- other spreadsheet tools

Example CSV output:

```csv
Iteration,X Value,f(x),Error,Status
1,1.500000,-0.125000,0.500000,INIT
2,1.750000,1.609375,0.250000,UPDATE
3,1.625000,0.666016,0.125000,UPDATE
```

---

## Project Folder Structure

The project uses a clean structure that separates headers, source files, and build configuration.

```txt
NumericalRootFinder/
│
├── CMakeLists.txt
│
├── include/
│   ├── AppConfig.h
│   ├── BisectionSolver.h
│   ├── CRTOverlay.h
│   ├── ExportManager.h
│   ├── FunctionParser.h
│   ├── GraphWidget.h
│   ├── IterationRecord.h
│   ├── MainWindow.h
│   ├── NewtonSolver.h
│   ├── SecantSolver.h
│   └── ThemeManager.h
│
├── src/
│   ├── AppConfig.cpp
│   ├── BisectionSolver.cpp
│   ├── CRTOverlay.cpp
│   ├── ExportManager.cpp
│   ├── FunctionParser.cpp
│   ├── GraphWidget.cpp
│   ├── main.cpp
│   ├── MainWindow.cpp
│   ├── NewtonSolver.cpp
│   ├── SecantSolver.cpp
│   └── ThemeManager.cpp
│
└── README.md
```

---

## Important Source Files

## `main.cpp`

This is the entry point of the program. It creates the Qt application, shows the boot splash screen if enabled, and opens the main window.

---

## `MainWindow.h` and `MainWindow.cpp`

These files control the main graphical interface.

They handle:

- function input
- method selection
- parameter fields
- solve, step, reset buttons
- settings dialog
- export buttons
- result display
- iteration table
- connection between UI and solver logic

---

## `FunctionParser.h` and `FunctionParser.cpp`

These files convert the user's typed mathematical expression into a callable function.

For example, this input:

```txt
x^3 - x - 2
```

is converted into a function that the solver can evaluate at different `x` values.

The parser also validates expressions and provides numerical derivative support for Newton-Raphson Method.

---

## `BisectionSolver.h` and `BisectionSolver.cpp`

These files contain the implementation of the Bisection Method.

The solver returns a list of iteration records instead of only returning the final root. This allows the GUI to show the full process in the table and graph.

---

## `NewtonSolver.h` and `NewtonSolver.cpp`

These files contain the implementation of the Newton-Raphson Method.

The derivative is computed using central difference approximation.

---

## `SecantSolver.h` and `SecantSolver.cpp`

These files contain the implementation of the Secant Method.

The solver uses two previous points to estimate the next root approximation.

---

## `IterationRecord.h`

This file defines the structure used to store every iteration.

Each record stores:

```txt
iteration number
x value
f(x)
error
method-specific values
status
```

---

## `GraphWidget.h` and `GraphWidget.cpp`

These files draw the graph manually using Qt's painting system.

The graph handles:

- function curve rendering
- grid and axes
- root marker
- bisection interval shading
- Newton tangent lines
- Secant lines
- scanline reveal animation
- CRT-style graph effects

---

## `CRTOverlay.h` and `CRTOverlay.cpp`

These files create the CRT overlay effect on top of the window.

The overlay can display:

- horizontal scanlines
- corner vignette shading

These effects can be enabled or disabled through the settings dialog.

---

## `AppConfig.h` and `AppConfig.cpp`

These files store and manage the user's configuration.

The configuration includes:

```txt
theme
UI scale
font size
animation speed
visual effect toggles
default tolerance
default maximum iterations
```

---

## `ThemeManager.h` and `ThemeManager.cpp`

These files generate the Qt stylesheet used by the application.

Instead of using one fixed color theme, the app builds the stylesheet dynamically based on the selected theme.

---

## `ExportManager.h` and `ExportManager.cpp`

These files handle exporting results.

They create:

- TXT report files
- CSV spreadsheet files

---

## How to Build the Project

## Requirements

Before building the project, install:

```txt
Qt 6
Qt Creator
CMake
MinGW or MSVC compiler
```

The easiest setup is:

```txt
Qt 6 + Qt Creator + MinGW 64-bit
```

---

## Build Steps in Qt Creator

1. Open Qt Creator.
2. Click **File > Open File or Project**.
3. Select `CMakeLists.txt`.
4. Choose a Qt 6 Desktop Kit.
5. Click **Configure Project**.
6. Build and run the project.

Recommended rebuild sequence after major changes:

```txt
Build > Clean All
Build > Run CMake
Build > Rebuild All
```

---

## How to Run the Application

After building, run the application from Qt Creator using the green Run button.

Basic usage:

1. Enter a function.
2. Select a method.
3. Enter the required parameters.
4. Set tolerance and maximum iterations.
5. Click **Solve**.
6. View the graph and iteration table.
7. Export results if needed.

---

## Example Use Case

### Function

```txt
x^3 - x - 2
```

### Method

```txt
Bisection
```

### Parameters

```txt
a = 1
b = 2
tolerance = 0.0001
maximum iterations = 50
```

### Expected behavior

The application will repeatedly divide the interval between `1` and `2` until it gets close to the root.

The graph will show the interval shrinking, and the table will show the computed values for each iteration.

---

## Portability and Deployment

When sharing the source code, only include project files such as:

```txt
CMakeLists.txt
include/
src/
README.md
.gitignore
```

Do not include build files such as:

```txt
build/
*.exe
*.dll
CMakeFiles/
CMakeCache.txt
```

For running the compiled application on another Windows device, use Qt's deployment tool:

```bash
windeployqt NumericalRootFinder.exe --release
```

This copies the Qt runtime files required by the executable.

---

## Recommended `.gitignore`

Use a `.gitignore` file to avoid uploading build files to GitHub.

```gitignore
build/
cmake-build-*/
out/
debug/
release/
Debug/
Release/

CMakeCache.txt
CMakeFiles/
cmake_install.cmake
Makefile
*.cmake

*.user
*.user.*
*.autosave

*.exe
*.dll
*.lib
*.a
*.o
*.obj
*.pdb

platforms/
styles/
imageformats/
iconengines/
translations/
tls/
networkinformation/
generic/
sqldrivers/
printsupport/
Qt6*.dll
D3Dcompiler_*.dll
libgcc*.dll
libstdc++*.dll
libwinpthread*.dll

.DS_Store
Thumbs.db
```

---

## Limitations

The project currently has some limitations:

- It only supports single-variable functions using `x`.
- It does not support implicit multiplication such as `2x`; the user must type `2*x`.
- Newton-Raphson Method may fail if the derivative is too close to zero.
- Bisection Method requires opposite signs at the interval endpoints.
- CSV export is supported, but direct `.xlsx` export is not included yet.

---

## Possible Future Improvements

Future versions may include:

- direct `.xlsx` export
- PDF report export
- graph image export
- more numerical methods
- better expression autocomplete
- saved computation history
- light/dark layout modes
- zoom and pan controls for the graph
- comparison mode for all three methods

---

## Conclusion

The Numerical Method Solver combines numerical computation, graph visualization, and a customizable retro-style interface. It provides an interactive way to solve equations and understand how different root-finding methods approach a solution.

By showing the graph, iteration table, final result, and exportable reports, the application is useful for both learning and presenting numerical method computations.
