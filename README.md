# New Mugen
```
Updated on 01/12/2020
```

## Table of Contents

* [New Mugen](#new-mugen)
* [Get Source](#get-source)
* [Requirements to build](#requirements-to-build)
* [Compilation](#compilation)

## New Mugen

The [Paintown](https://github.com/kazzmir/paintown) project was used as the basis for this project.

Reverse engineering was carried out in [Paintown](https://github.com/kazzmir/paintown) to obtain only Mugen separately.

## Get Source

If you are reading this you might already have the source. If not get it from the
[New Mugen Repository](https://github.com/gui-rocha/newmugen).

Additional data which has collections of characters and stages can be obtained in the [New Mugen Data Repository](https://github.com/gui-rocha/newmugen-data).
```
Note to distribution maintainers it is recommended to omit the data from the above repository since it may contain copyrighted material.
The included data in the New Mugen Repository should be sufficient for running the application.
```
  
## Requirements to build

You need the following things to compile New Mugen:
* [GNU build utils (gcc and g++)](http://www.gnu.org/software/gcc/)
* Software construction tools (either or)
	* [Scons](http://www.scons.org)
	* [Cmake](http://www.cmake.org)
* [zlib](http://www.zlib.net/)
* [libpng](http://www.libpng.org/pub/png/libpng.html)
* pthreads - If you are on Unix you should have this
* Scripting
	* [python](http://www.python.org) and its headers (usually this comes with python)
* [Allegro 5.x](https://github.com/liballeg/allegro5)
* [R-Tech1](http://github.com/kazzmir/r-tech1)

## Compilation
You can build New Mugen with either Scons or Cmake.

#### Build Environment Flags
* **DEBUG** - set to 0/1 (on/off in cmake) to enable or disable debugging
* **LLVM** - set to 0/1 (on/off in cmake) to build with clang
* **DATA_PATH** - set this to the desired data path
	* With Scons it will default to data
	* With Cmake it will default to `${CMAKE_SOURCE_DIR}/data`

#### Kubuntu 18.04
```
$ git clone https://github.com/kazzmir/r-tech1
$ cd r-tech1
$ sudo make
$ export DEBUG=1
$ export PREFIX=/usr /local
$ sudo scons install

$ cd newmugen
$ sudo make
$ ./Mugen
```
