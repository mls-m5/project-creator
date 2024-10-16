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

## Add 'goto' to bashrc

After installing add the following line to ~/.bashrc

```bash
source project-goto
```

This will define a function called goto that you could use to goto your
selected project§§.


## Using `mopen`

`mopen` tries to guess the kind of project and open an appropriate application.

