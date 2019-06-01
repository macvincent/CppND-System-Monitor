# CppND-System-Monitor

## To setup and compile in an Ubuntu workspace:

1. Clone repository this repository using the link below
```
git clone https://github.com/udacity/CppND-System-Monitor
```
2. Install the `ncurses` package
```
sudo apt-get install libncurses5-dev libncursesw5-dev
```
3. Compile and run main.cpp while dynamically linking the ncurses header file
```
g++ -std="c++17" main.cpp -lncurses && ./a.out
```
4. You should get an output on the terminal showing something like this
<p align="center">
  <img src = "https://github.com/macvincent/CppND-System-Monitor/blob/master/demo.png">
</p>
