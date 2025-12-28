cder is a cd helper

Version 1.0.1

Demo:
![Demo GIF](https://github.com/user-attachments/assets/7f29dd3e-6ecc-4ce3-8023-1acec52c1a2f)

## Why?

Because navigating directories is tedious. Cder allows you to easily
navigate using specialized features. Currently, it supports bookmark or starring
your directories with an alias (plus category) and use it in a shell wrapper to do the cd, I call 
them clients, though it probably was not a correct terminology. Anyway, it means
you can do this.
```sh
cda cder /home/me/cder # cder will automatically expand absolute so full path is not mandatory

cdls # this list the aliases in default category

cdm cder # this cd to /home/me/cder
```
For more infomation, see `cder --help` or if you want to customize the shell wrapper,
see [PROTOCOL.md](PROTOCOL.md) and [API.md](API.md) to see what you would expect to get.
End users only need to interact with the shell wrapper and I have made a decent one in 
[clients](clients/). Only bash shell for now but I will expand later.

## Dependencies

If dependencies mean things you need on your computer to use, it is totally standalone
beside the LibC++17 which allows it to work with filesystem portably. It actually depends 
also on CLI11 and RapidJson for interface and database, but only during the build process
(these libraries are header-only).

## Installation

To build it from source, follow the following instructions:
```sh
cd ~
git clone --recurse-submodules https://github.com/ngtv2409/cder
cd cder
mkdir build
cmake -B build
make -C build
```
After the build process terminate, you get `build/cder`. Install it to your `PATH`.

Then, get one of the wrapper from `clients/` and source it in your shell rc file.

The database will be set up in `$CDER_DB_PATH`, do not forget to set it and 
make sure it is full path or relative to a stable point.
```sh
export CDER_DB_PATH="~/.local/share/cder/"
```
