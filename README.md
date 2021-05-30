# CrazyTang

Build
===============
```shell
git clone https://github.com/ZhangFengze/CrazyTang.git
cd CrazyTang
git submodule update --init --recursive
python tools/server.py # for server
python tools/client_gui.py # for client
```

Build Requirements
===============
* a **C++20** compatible compiler
* cmake 3.19+
* python 3

Note that we intentionally take advantages of the latest features of C++ standard and other tools.  
It is recommended to keep toolchains up-to-date.  

### **Windows**
just install the latest `Visual Studio` `CMake` and `Python`.  

### **Linux**
usually we have to build from source.   
`https://hub.docker.com/r/rikorose/gcc-cmake` is here to help you
```shell
cd CrazyTang
docker run -it --rm -v `pwd`:/usr/src/myapp -w /usr/src/myapp rikorose/gcc-cmake
# now you have gcc cmake and python
```



