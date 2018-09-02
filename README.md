# continuum

continuum is a simple tool designed to let me stay constantly connected to the internet without having to log all the time on the captive portal of my beloved ISP. 

It was written in C so that I can run it on my router which is a MikroTik RouterBOARD 941-2nD (MIPS based SoC) running OpenWrt. Build instructions for that machine are below but as it is pure C code, you may port it to whatever machine you want as long as you're also able to compile OpenSSL (or equivalent), libcurl and myhtml libs on it. 

This program is licenced under the terms of the GNU GPL v3 License.

### Disclaimers

* This program is still being tested and might not work at all as is.
* This is one of my first C projects so the quality of the code might shock you.



### Roadmap

* [ ] Find a way to enforce SSL peers verification with the system's CA bundle.
* [ ] Check for captive portal feedback after logging in.
* [ ] Improve the overall quality of code.



### Installing continuum on the RouterBOARD : the easy way

* Get the binary pre-built from [this](https://github.com/darkgallium/continuum/releases) page.

* Extract the archive.

* Transfer the binary onto your router. You can transfer it via scp if ssh is enabled on your router.

* Move the binary to `/usr/bin/` .

* Create a service to launch continuum at startup (more on that to come).


### Building from source and installing continuum on the RouterBOARD

This really could be easier. In fact, you can't compile continnum on the RouterBOARD itself because it does not have enough ROM to install gcc and the usual build tools. In consequence, we have no other choice than cross-compiling the binary on another machine. 

#### Setting-up the cross-compiling environment 

I recommand you to have a dedicated environment to cross-compile binary. I'm going to use a debian environment for this. You can use anything else but note that the following instructions would probably have to be adapted.

* After installing debian on you VM, you will need to install the basic build tools :

  ```bash
  sudo apt-get install gcc make
  ```

* Then, you'll need to get the Mikrotik gcc toolchain that is provided by OpenWrt. You can build it from source or you can download it prebuilt from OpenWrt's website https://downloads.openwrt.org/releases/ (select the latest release, then choose the Mikrotik ar71xx target and then download the tarball with sdk in the name).

*Before using the OpenWrt toolchain, I tried to use the standard `mips-linux-gnu-gcc` compiler to compile continuum for the RouterBOARD. Unfortunately, it did not work, leaving me with an obscure `Illegal instruction` error. I'm not quite sure why native MIPS binary ELFs won't work on the RouterBOARD but it may have something to do with a custom/incomplete processor implementation of the MIPS standard according to [this StackExchange answer](https://unix.stackexchange.com/questions/220894/illegal-instruction-on-a-static-mips-binary).*

* Once you have the toolchain, look for a folder called `staging_dir` inside the toolchain folder. In this folder, you'll find a folder whose name contains `toolchain-mips_24kc_gcc`. You'll need the full path of that folder in the next step.

* In order to use the cross-compiler, we need to change our PATH variable to point to the `toolchain-mips_24kc_gcc` folder :

  ```bash
  export STAGING_DIR = /replace/by/the/full/path/of/the/toolchain-mips_24kc_folder
  export PATH = $PATH:$STAGING_DIR/bin
  ```

We are now ready to build code that can run on the RouterBOARD. 

Let's start by building the dependancies.

#### Building the SSL backend

You may use whatever SSL backend that is compatible with libcurl (complete list [there](https://curl.haxx.se/docs/ssl-compared.html))  but I recommend you to use wolfSSL because it is the one I'll be using.

- Download wolfSSL latest source tarball.

- Extract the tarball.

- Configure the build using the following command :

  ```bash
  ./configure --prefix=$(pwd)/build --disable-shared --host=mips-openwrt-linux
  ```

- Build the library using the following command :

  ```bash
  make && make install
  ```

#### Building libcurl

* Download libcurl latest source tarball from the official website.

* Extract the tarball.

* Configure the build using the following command :

  ```bash
  ./configure --with-ssl=insert/path/to/wolfssl/prefix/directory --prefix=$(pwd)/build --disable-ftp --disable-file --disable-ldap --disable-dict --disable-telnet --disable-tftp --disable-rtsp --disable-pop3 --disable-imap --disable-smtp --disable-gopher --disable-smb --disable-shared --host=mips-openwrt-linux
  ```

* Build the library using the following command :

  ```bash
  make && make install
  ```
  

#### Building myhtml

- Download myhtml latest source tarball from the github repository.

- Extract the tarball.

- Edit the `Makefile` and replace the following instruction :

  ```makefile
  CC ?= gcc
  ```

  by :

  ```makefile
  CC ?= mips-openwrt-linux-gcc
  ```

- Build the library using the following command :

  ```bash
  make && make install
  ```


We are done with the dependancies. 

#### Building continuum

- Download the latest source tarball from this github repository.

- Extract the tarball

- Edit the `Makefile.MikroTik` file and replace the values of the variables `PATH_TO_SSL`, `PATH_TO_CURL` and `PATH_TO_MYHTML`by the complete paths of the locations where each library was installed (prefix directory). 

- Rename `Makefile.MikroTik` to `Makefile`

- Build continuum by issuing this command :

  ```bash
  make
  ```

  *You may see some warnings related to functions that are declared but never defined. I'm aware of that even though I don't really know why they happen. They don't have any effect neither on the compilation process nor the execution of the program.*

  Now that we have built the program, we need to install the binary onto the 

#### Installing continuum on the RouterBOARD

- We now need to transfer the binary built in the last step onto your router. It is located under the `bin` folder. You can transfer it via `scp` if ssh is enabled on your router.
- Move the binary to /usr/bin/ .
- Create a service to launch continuum at startup (more on that to come).