# UniTrack
#### The University Activity Tracker


### About
Project is under active developing now. This section will be updated as soon as the project will leave the prototype stage.


### Если вы говорите по-русски (If you speak Russian)
Пожалуйста, читайте [README-ru.md](https://github.com/DmitryKuk/UniTrack/blob/master/README-ru.md).


### Lisence
The project is under my own license:

- You can use the project for free for any non-commercial purposes.
- You can also change the source code, if you want, for non-commercial purposes.
- For use in any commercial purposes, please, contact me: <d1021976@gmail.com>.
- You can not distribute changed version of this program (as source code or binary or etc.) without reference to original version and it's authors. It means also, that you should not remove comments with information about the authors and/or the lisence from the source code.

---


### Requirements:
- g++-5 or best (or clang++-3.6 or best, or any other C++14 compiler supporting std::regex, move-semantics and many more, *full support is best*).
- Boost (tested version: 1.58).
- GNU Make (and a piece of shell) for building this.
- Scons for building [Mongo-cxx-driver (Legacy with C++11)](https://github.com/mongodb/mongo-cxx-driver/tree/legacy).


### Building and Installation
1. Get the sources:
	- `git clone --recursive https://github.com/DmitryKuk/UniTrack`
        + или `git clone https://github.com/DmitryKuk/UniTrack` (third-party dependencies will be loaded when building)
	- and `cd UniTrack/`
2. Build and install:
	- just do `make happy` (uses `sudo` for installing; you will be required to enter the password)
	- or:
        + `make third-party`
        + `sudo make install third-party`
        + `make`
        + `sudo make install`
    - *any of these methods will install default configuration and site data*
3. Running:
	- `unitrack` (by default, `/usr/bin/unitrack`)
4. Upgrading:
	- `make upgrade` for automatic uninstalling the old version, dowloading updates, building and installing the new one (uses `sudo`)
5. Uninstalling:
	- `sudo make uninstall` for uninstalling all libs and executables from your system
    - or `sudo make uninstall-all` for completely uninstalling (executables, default configuration, WWW-data and third-party dependencies)
6. Manual management (**do not use, if you don't know, why you need it**):
    - **Use only if you want to recover corrupted configuration file or site!**
    - **NOT required with any of installation method above!**
    - `make install-config` to install configuration by default
    - `make install-www` to install site


### Dependencies
- [Boost](http://www.boost.org/) *(tested version: 1.58)*
- [JSON for Modern C++](https://github.com/nlohmann/json) *Niels Lohmann*
    + Automatically managed dependency (via git submodules).
- [Mongo-cxx-driver (Legacy with C++11)](https://github.com/mongodb/mongo-cxx-driver/tree/legacy)
    + Automatically managed dependency (via git submodules).


### Development
- The project consists of sub-modules (see `src/module-name`).
- Please, build the project only from root directory of the project.
- You can try to run the program without installation: `make run` in the root of the project. But this might not work on some systems. If it is so, try `export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path-to-project/libs`. Installed version runs normally.


### Authors
- [Dmitry Kukovinets](https://github.com/DmitryKuk): <d1021976@gmail.com>
    + Author of the server-side part of the project.
- [Maxim Ivanov](https://github.com/splincode): <https://vk.com/splincode>
    + Author of the web-part of the project
- [Vera Produvnova](https://github.com/VeraProd)
    + Author of some modules for Templatizer
