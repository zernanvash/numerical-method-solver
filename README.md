# Numerical Method Solver

Numerical Method Solver is an interactive numerical methods program for learning how equations are solved step by step. It has two main modes:

- **Root-Finding Mode** for solving `f(x) = 0`
- **Differential Equation Simulation Mode** for solving initial value problems of the form `dy/dt = f(t, y)`

Download the portable Windows build from the release page:

https://github.com/zernanvash/numerical-method-solver/releases/tag/v1.1.0

Extract the zip from the release and run `NumericalRootFinder.exe`.

---

## Root-Finding Mode

Root-Finding Mode solves equations where the goal is to find an `x` value that makes:

```txt
f(x) = 0
```

The program shows the full iteration process instead of only showing the final answer. Each method updates its approximation differently, and the table records every step.

### Function Input

Enter a function using `x` as the variable:

```txt
x^3 - x - 2
sin(x) - 0.5
exp(x) - 3
x^2 - 4
```

Supported operators and functions:

```txt
+  -  *  /  ^
sin, cos, tan
sqrt, log, ln, log10
exp, abs
pi, e
```

Use explicit multiplication. For example, type `2*x`, not `2x`.

---

## Bisection Method

Bisection is a bracketing method. It needs two starting values, `a` and `b`, where the function changes sign:

```txt
f(a) * f(b) < 0
```

The root must be inside the interval `[a, b]`. The method repeatedly cuts the interval in half.

Formula:

```txt
c = (a + b) / 2
```

Then the program checks which side still contains the sign change:

```txt
if f(a) * f(c) < 0:
    root is between a and c
else:
    root is between c and b
```

Required inputs:

```txt
f(x)
a
b
tolerance
maximum iterations
```

Table meaning:

```txt
Iteration: current step number
X Value: midpoint c
f(X): function value at c
Error: interval/approximation error
Status: INIT, UPDATE, CONVERGED, or MAX_ITER
```

Graph behavior:

- Shows the function curve
- Highlights the shrinking interval
- Marks each midpoint approximation
- Shows the final/root approximation when available

---

## Newton-Raphson Method

Newton-Raphson starts from one guess, `x0`, and uses the slope of the function to jump toward the root.

Formula:

```txt
x(n+1) = x(n) - f(x(n)) / f'(x(n))
```

In this program, the derivative is estimated numerically with a central difference:

```txt
f'(x) ≈ (f(x + h) - f(x - h)) / (2h)
```

Required inputs:

```txt
f(x)
x0
tolerance
maximum iterations
```

Table meaning:

```txt
Iteration: current step number
X Value: current approximation
f(X): function value at the approximation
Error: distance from the previous approximation
Status: UPDATE, CONVERGED, or MAX_ITER
```

Graph behavior:

- Shows the function curve
- Draws tangent-line movement
- Marks the current approximation
- Shows how the tangent intersects the x-axis to produce the next guess

Newton-Raphson is fast when the starting guess is good, but it can fail when the derivative is close to zero or the function behaves badly near the guess.

---

## Secant Method

The Secant Method uses two starting guesses instead of a derivative. It draws a line through two function points and uses that line to estimate the next root approximation.

Formula:

```txt
x(n+1) = x(n) - f(x(n)) * (x(n) - x(n-1)) / (f(x(n)) - f(x(n-1)))
```

Required inputs:

```txt
f(x)
x0
x1
tolerance
maximum iterations
```

Table meaning:

```txt
Iteration: current step number
X Value: current approximation
f(X): function value at the approximation
Error: distance from the previous approximation
Status: UPDATE, CONVERGED, or MAX_ITER
```

Graph behavior:

- Shows the function curve
- Draws secant lines between approximation points
- Shows how each secant line estimates the next root
- Marks the current approximation

The Secant Method is often faster than Bisection and does not need a derivative, but it is less guaranteed than Bisection.

---

## Differential Equation Simulation Mode

Differential Equation Simulation Mode solves initial value problems:

```txt
dy/dt = f(t, y)
y(t0) = y0
```

The program approximates the solution step by step. In this mode, the expression uses:

```txt
t = independent variable
y = current solution value
```

Example expressions:

```txt
0.5*y
t + y
-y
sin(t) + y
-0.07*(y - 25)
```

Required inputs:

```txt
dy/dt expression: f(t, y)
initial t: t0
initial y: y0
step size: h
steps: number of iterations
method: Euler or RK4
```

Graph behavior:

- Uses `t` on the x-axis
- Uses `y` on the y-axis
- Draws points for each approximation
- Connects the points with line segments
- Marks the final approximation

---

## Euler Method

Euler's Method uses the current slope to move forward one step.

Formula:

```txt
y(n+1) = y(n) + h * f(t(n), y(n))
t(n+1) = t(n) + h
```

Where:

```txt
h = step size
f(t, y) = slope at the current point
```

Table columns:

```txt
Iteration
t
y
f(t,y)
Next t
Next y
Status
```

Example:

```txt
dy/dt = 0.5*y
t0 = 0
y0 = 100
h = 0.1
steps = 5
```

The first step is:

```txt
f(0, 100) = 0.5 * 100 = 50
next y = 100 + 0.1 * 50 = 105
next t = 0.1
```

Euler is simple and useful for learning, but it is usually less accurate than RK4 for the same step size.

---

## RK4 Method

Runge-Kutta 4th Order, or RK4, samples the slope four times during each step. It combines those slopes into a more accurate update.

Formula:

```txt
k1 = f(tn, yn)
k2 = f(tn + h/2, yn + h*k1/2)
k3 = f(tn + h/2, yn + h*k2/2)
k4 = f(tn + h, yn + h*k3)

y(n+1) = yn + (h/6) * (k1 + 2*k2 + 2*k3 + k4)
t(n+1) = tn + h
```

Table columns:

```txt
Iteration
t
y
k1
k2
k3
k4
Next t
Next y
Status
```

RK4 is more accurate than Euler because it estimates the behavior inside the step, not only at the beginning of the step.

---

## ODE Example Models

Population growth:

```txt
f(t,y) = 0.5*y
t0 = 0
y0 = 100
h = 0.1
steps = 20
```

Cooling model:

```txt
f(t,y) = -0.07*(y - 25)
t0 = 0
y0 = 90
h = 1
steps = 20
```

Constant velocity motion:

```txt
f(t,y) = 5
t0 = 0
y0 = 0
h = 1
steps = 20
```

---

## Output Table

The table is meant to make the method readable as a computation, not just a final answer.

For root-finding methods, the table tracks:

```txt
iteration
current x approximation
f(x)
error
status
```

For ODE methods, the table tracks:

```txt
iteration
current t
current y
slope or RK4 slope samples
next t
next y
status
```

Statuses indicate whether a row is an update, a convergence result, or an error/max-iteration condition.

---

## Graph Panel

The graph changes depending on the active mode.

Root-Finding Mode:

- Plots `f(x)`
- Shows the x-axis root target
- Draws method-specific visual steps
- Marks the best/current root approximation

ODE Simulation Mode:

- Plots the approximation path as `t` vs `y`
- Draws connected step points
- Marks the final point

The graph uses a CRT-style scan reveal, grid, pixel-like text, and theme colors to match the rest of the interface.

---

## Exporting Results

The program can export results as:

```txt
TXT report
CSV table
```

Root-finding exports include:

```txt
function
method
tolerance
maximum iterations
final root approximation
final f(x)
iteration table
```

ODE exports include:

```txt
differential equation
method
initial condition
step size
number of steps
final t
final y
full iteration table
```

---

## Notes And Limits

- Expressions must use explicit multiplication, such as `2*x`.
- Root-finding expressions use `x`.
- ODE expressions use `t` and `y`.
- Bisection requires the starting interval to contain a sign change.
- Newton-Raphson can fail when the numerical derivative is too close to zero.
- Very large ODE values can grow quickly depending on the step size and equation.

---

## Basic Workflow

1. Choose a mode.
2. Enter the expression.
3. Choose the numerical method.
4. Enter the required parameters.
5. Click **Solve** to fill the table and graph.
6. Use **Step** to reveal the computation one row at a time.
7. Export TXT or CSV if you need a report.
