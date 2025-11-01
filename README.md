<a name="readme-top"></a>

[![Lines of Code][tokei-url]][repo-url]

<div>
<h1 align="center">Helion</h1>

  <p align="center">
    Hobby operating system, written mainly in C
  </p>
</div>

<details>
  <summary>Table of contents</summary>
  <ol>
    <li>
      <a href="#about">About Helion</a>
    </li>
    <li><a href="#requisites">Requisites</a></li>
    <li><a href="#installation">Build and Installation</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#compatibility">Compatibility</a></li>
    <li><a href="#license">Licensing</a></li>
  </ol>
</details>

## The Project
<a name="about"></a>

**Helion is a hobby operating system**, in really early development, developed using C and ASMx86. The Operating System is far from being ready to use for any real-life purpose.

For supported OS and Distributions, please check **<a href="#compatibility">Compatibility</a>**.

This is completely a hobby project and was not born as a product.
<p align="right">(<a href="#readme-top">top</a>)</p>

### Requisites for building
<a name="requisites"></a>

Through the initial development, if you came want to try the OS, you can build the image and run it on QEMU through a process automated by the makefiles/scripts.

Every dependency requirement will try to be handled through scripts, be aware that there could be some compatibility errors as it's not fully tested.

---------------

To get started, you must clone and cd into the repository through git, either with **HTTPS**:
```
git clone https://github.com/OhFrancy/Helion.git && cd Helion
```
or **SSH**:
```
git clone git@github.com:OhFrancy/Helion.git && cd Helion
```
(Alternatively you can download it through your browser and cd into it).

For the building process to start, you must also install **make**, to do so on Ubuntu/Debian:
```
sudo apt install make
```
I can't list all packet managers here, so please, search for yours online.
<p align="right">(<a href="#readme-top">top</a>)</p>

### Build & Installation
<a name="installation"></a>

To build **Helion** you must simply run:
```
make build
```
If this is your first time building the OS, it'll take some extra time to install the dependencies and build the toolchain.

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

## Helion Roadmap
<a name="roadmap"></a>

**Multi-stage bootloader**
- [x] Set up the bootloader's first stage
- [x] Read and load from disk, thro the BIOS
- [x] Enter second stage (out of bootsector)
- [x] Build a working GDT
- [x] Enter Protected mode 32-bits, segmented memory
- [ ] Add another middle bootloader stage, to handle the file-loading in FAT32 with less restrictions 

  ###### Kernel Loader
  - [x] Build a working IDT
  - [x] Remap the PIC
  - [x] Handle IRQs
  - [ ] Write a PATA Driver
  - [ ] Write a FAT32 Driver upon the PATA Driver

**Kernel**

-- Coming


<p align="right">(<a href="#readme-top">top</a>)</p>

<!-- CONTRIBUTING -->
## Contributing

Contributions are really important for this project, and learning purposes.

Any contribution you make is **really appreciated**, just make sure you know what you are doing, useless PRs will be rejected.

## Compatibility
<a name="compatibility"></a>
**Helion** currently only supports **Linux distributions**.

It does **NOT** support **Windows** and **MacOS** for the development build installation process, and I don't think it'll ever support them.

Here is a list of currently known supported Linux distributions:
-  Ubuntu/Debian,
-  Fedora,
-  Arch Linux.

## License
<a name="license"></a>
Distributed under the MIT License. [LICENSE](LICENSE) for more info.

<!-- IMAGES & LINKS -->
[tokei-url]: https://tokei.rs/b1/github/OhFrancy/Helion?style=for-the-badge
[repo-url]: https://github.com/OhFrancy/Helion

