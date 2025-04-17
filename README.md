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
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#compatibility">Compatibility</a></li>
    <li><a href="#license">Licensing</a></li>
  </ol>
</details>

<!-- ABOUT THE PROJECT -->
## The Project
<a name="about"></a>

<!-- Image here -->

**JaOS is a hobby operating system**, in really early development, developed using C and ASM. The Operating System is far from being ready to use for any real-life purpose.

For supported OS and Distributions, please check **<a href="#compatibility">Compatibility</a>**.

This is completely a hobby project and was not born as a product.
<p align="right">(<a href="#readme-top">top</a>)</p>

### Requisites for building
<a name="requisites"></a>

The project was built with the idea of automating as much as possible, every dependency requirement will be handled through the building process.

Though, to get started, you must clone and cd into the repository through git, either with **HTTPS**:
```
git clone https://github.com/OhFrancy/JaOS.git && cd JaOS
```
or **SSH**:
```
git clone git@github.com:OhFrancy/JaOS.git && cd JaOS
```
For the building process to start, you must also install **make**, to do so on Ubuntu/Debian:
```
sudo apt install make
```
I can't list all packet managers here, so please, search for yours online.
<p align="right">(<a href="#readme-top">top</a>)</p>

### Build & Installation
<a name="installation"></a>

To build **JaOS** you can simply run:
```
make build
```
If this is your first time building the Operating System, it'll take some extra time to install the dependencies and build the toolchain.

Now you're ready to run the OS, it currently uses **QEMU** to do so, simply run:
```
make run
```
A window will pop up and the OS will boot.

### Installation Errors
If any error occurs, it might be related to your **distribution** or **packet manager**.

Please, make sure to read <a href="#compatibility">Compatibility</a> for what's supported and what's not.

If the error persists, you can open a **Github Issue** and report it.

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

## Compatibility
<a name="compatibility"></a>
**JaOS** currently only supports **Linux distributions** for the installation process.

It does **NOT** support **Windows** and **MacOS**, even though in the near future I'm hoping to have that.

Here is a list of currently known supported Linux distributions:
-  Ubuntu/Debian,
-  Fedora,
-  Arch Linux.

## License
<a name="license"></a>
Distributed under the MIT License. [LICENSE](LICENSE) for more info.

<!-- IMAGES & LINKS -->
[tokei-url]: https://tokei.rs/b1/github/OhFrancy/JaOS?style=for-the-badge
[repo-url]: https://github.com/OhFrancy/JaOS

