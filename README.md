# Project utils

## Project creator

A program for initializing ide-files after checking cloning from git repository.

Can also create simple cmake projects

Currently only qtcreator projects are supported (built with cmake)

Usage

```
create-project project-name
```

## Build project

Configure and build project

In source dir:
```
build
```


## Install

Build project with cmake
```

mkdir -p build/default
cd build/default
cmake ../..
make
./install.sh

```

