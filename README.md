cder is a cd helper

Version 1.0.0

## Why?

Navigating deeply nested project directories is tedious. 
You either type out long paths repeatedly (cd ~/code/work/clients/acme/backend/services/api), 
maintain a complex system of shell aliases, or waste time cd-ing through multiple levels.
Existing solutions like autojump and z try to solve this by tracking your history automatically, 
but they can be unpredictable - jumping to the wrong directory because you visited it recently, 
or failing to find directories you rarely visit but need occasionally.
cder takes a different approach: explicit bookmarks combined with flexible organization. 
You decide what's important enough to bookmark, organize it with categories, and jump there instantly. 
No surprises, no learning curve, just direct navigation to where you need to be.

## What is it

cder is a simple program that manages directory bookmarks and outputs paths in a structured protocol format.
It doesn't change directories itself - instead, it prints paths that shell wrappers can use to perform navigation.
The core philosophy: do one thing well. cder manages bookmarks and outputs paths. 
Your shell handles the actual directory changes. This separation makes cder composable - you can use it in scripts, 
integrate it with other tools, or build custom workflows around it.
The protocol-based output (see `PROTOCOL.md`) means the tool is both human-friendly and machine-parseable. 
Run it directly to see formatted output, or parse its structured responses in shell wrappers and scripts.

An example usage which implements cderm (go to directory from bookmark) can be found in `cder_def.sh`

## Features

- Bookmarking: Give your path an alias and jump without a second thought

- Categories: Organize your bookmarks under a root name 

- Clear protocol: Easily wrap it around a shell

- Lightweight: Just a CLI tool which manages a JSON DB

## Installation

Installation does require some efforts.

The first step is to build. You clone the repository and build it using cmake.
Below is the procedure on Linux-like environments:
```sh
cd ~
git clone https://github.com/ngtv2409/cder
cd cder
git submodule update --init --recursive
mkdir build
cmake -B build
make -C build # or other build systems
```
After it is built, you get an executable `build/cder`, this is the program we are talking about!
You may install it to `PATH`.
Finally you will need to set its database path. Add this go your 
`.bashrc` (or do the same on your specific shell).
```sh 
export CDER_DB_PATH=$(realpath ~/.local/share/cder) # or wherever you want
# Note: the variable is sent directly to filesystem, so it must be full absolute path
```
Now `cder` is available. The last step is to use it in your cd, an example 
on how to do that can be found in `cder_def.sh`.

Usage can be found in `--help`.
