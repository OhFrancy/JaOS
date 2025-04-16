<a name="readme-top"></a>

<!-- SETS THE LINES COUNTER, SEE 'tokei' ON GITHUB FOR MORE INFO -->
[![Lines of Code][tokei-url]][repo-url]

<!-- UPCOMING LOGO -->
<div>
<h1 align="center">JaOS</h1>

  <p align="center">
    Hobby operating system, written mainly in C
  </p>
</div>

<details>
  <summary>Table of contents</summary>
  <ol>
    <li>
      <a href="#about">About JaOS</a>
    </li>
    <li><a href="#requisites">Requisites</a></li>
    <li><a href="#installation">Build and Installation</a></li>
    <li><a href="#roadmap">Usage</a></li>
    <li><a href="#license">Roadmap</a></li>
  </ol>
</details>

<!-- ABOUT THE PROJECT -->
## The Project
<a name="about"></a>

<!-- Image here -->

**JaOS is a hobby operating system**, in really early development, developed using C and ASM. The Operating System is far from being ready to use for any real-life purpose.

It currently works on these distributions: **Ubuntu, Debian, Fedora, Arch**, it might also work on **RHEL** and **CentOS** but it's NOT tested.

This is completely a hobby project and was not born as a product.
<p align="right">(<a href="#readme-top">top</a>)</p>

### Requisites for building
<a name="requisites"></a>
This step is only required if you plan to do a **manual installation**.

Start by cloning and cd'ing into the repo:
```
git clone https://github.com/OhFrancy/JaOS.git && cd JaOS
```
Then run this script for the dependencies and follow the prompts (you'll be asked for root permissions):
```
build_scripts/install_dependencies.sh
```

### Build & Installation
<a name="installation"></a>

#### Automated Building
If you didn't already, you must clone and cd into the repo:
```
git clone https://github.com/OhFrancy/JaOS.git && cd JaOS
```
Execute the script with flag '--toolchain', to build the toolchain for the first time, build and run the OS:
```
./run --toolchain
```
This will take some time; after building the toolchain for the first time, you can omit the '--toolchain' to only build the OS and run it through QEMU.

##### Supported shells
If you got a shell related error, while building the toolchain, it's likely that your shell is not supported.

You can easily fix this by adding this line of code to your shell configuration file.
```
export PATH="$HOME/opt/cross/bin/i686-elf/bin:$PATH"
```
(If you don't know what your shell configuration file is, search online for '```<Shell> configuration file```')

Then to source the file to update your session, ```source <Path/To/ShellConf>```.

You don't need to re-build the toolchain every time, from now on you can just execute 'run' without flags to run the OS.
<p align="right">(<a href="#readme-top">top</a>)</p>

#### Manual Building

**Make sure you installed the dependencies (see 'Requisites for building')**

First, we'll clone and cd in to the repo:
```
git clone https://github.com/OhFrancy/JaOS.git && cd JaOS
```
To build the toolchain and then the OS, you'll need to run:
```
make toolchain all
```
You'll also want to find your shell configuration file and add this line:
```
export PATH="$HOME/opt/cross/bin/i686-elf/bin:$PATH"
```
And source it, ```source <Path/To/ShellConf>```.

To finally run QEMU, you can use this command that will create 'qemu.sh', with all the flags for it to work already set up:
```
( echo "#\!/bin/bash" && echo "qemu-system-i386 -cpu core2duo -m 1024M -fda build/jaos.img -vga std" ) > qemu.sh && chmod a+x qemu.sh
```
Now simply run it with ```./qemu.sh``` whenever you want.

<p align="right">(<a href="#readme-top">top</a>)</p>

## JaOS Roadmap
<a name="roadmap"></a>

**Multi-stage bootloader**
- [x] Basic utility functionalities
- [x] Read and load from disk, using the BIOS
- [x] Enter second stage (out of bootsector)
- [x] Build a GDT (Global Descriptor Table)
- [x] Enter Protected mode 32-bits, segmented memory
- [ ] Jump to C code in 2th stage

**Kernel**

<p align="right">(<a href="#readme-top">top</a>)</p>

<!-- CONTRIBUTING -->
## Contributing

Contributions are really important for this project, and learning purposes, it helps both you and me!

Any contribution you make is **really appreciated**.

If you think you have something to add to the project, that would make it better, fork the repo and make a pull request.

Remember to give the repository a **star**, as it'd help to involve and interest more people towards the project!

How to contribute with git: (if you are still not sure, search for a tutorial online, there are a lot!)

0. Fork the Project
1. Create your branch (`git checkout -b AmazingFeature`)
2. Commit your changes (`git commit -m 'Add AmazingFeature, ...(add more info)'`)
3. Push to the branch (`git push origin AmazingFeature`)
4. Open a pull request

After this, your PR will be checked: if found with no conflicts and a useful request, it will be merged to the project.

## License
<a name="license"></a>
Distributed under the MIT License. `LICENSE` for more info.

<!-- IMAGES & LINKS -->
[tokei-url]: https://tokei.rs/b1/github/OhFrancy/JaOS?style=for-the-badge
[repo-url]: https://github.com/OhFrancy/JaOS

