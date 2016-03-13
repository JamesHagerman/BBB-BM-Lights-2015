Build Notes
======

My build notes for a working BBB image. This notes were written down while following:

```
https://eewiki.net/display/linuxonarm/BeagleBone+Black
```

Linaro/ARM Build Toolchain
======

To get the Linaro ARM toolchain to work, we need to set CC EVERY time we start working on this crap. It will probably end up looking like this most of the time:

```
/home/jamis/dev/build-bbb-image/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-
```

But to set it manually:

```
cd ./build-bbb-image
export CC=`pwd`/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-
```

Test it with:

```
${CC}gcc --version
```


U-Boot/uboot/Universal Boot Loader
=======

To Build it:
```
make ARCH=arm CROSS_COMPILE=${CC} distclean
make ARCH=arm CROSS_COMPILE=${CC} am335x_evm_defconfig
make ARCH=arm CROSS_COMPILE=${CC}
```


Kernel
======

The two kernel mainlines that are available differ in a few ways. Basically, `bb-kernel` is the Linus mainline... with patches to get it working on the BBB. `ti-linux-kernel` uses TI's 3.14 repo as it's base to provide full silicon/chip/feature support.  

I tried compiling `bb-kernel` first for kicks and got a message telling me to install a bunch of other tools. I installed them. I hope I didn't break something doing so:

```
sudo apt-get install device-tree-compiler lzma lzop u-boot-tools libncurses5-dev:amd64 
```

Here's my bb-kernel build time log on my desktop VirtualBox Ubuntu 14.04 install:

```
Started build of bb-kernel at 1:33:00 AM
Downloading the Torvalds git repo took until 1:37:32
Downloading the stable repo took until 1:39
Patching took until 1:41
actual compile started at 1:59 (after I modified a bunch of configs to add stuff to the kernel)
kernel compile ended at 2:19
modules ended soon after.
export kernel_version=4.1.3-bone14
```




```
Started build of ti-linux-kernel at 2:20 AM
Downloading Linus repo too until 2:26 AM
Downloading stable repo took until 2:28
Patching too until: 2:29
Actual compile started at: 2:39
kernel compile ended at: 3:01
export kernel_version=4.1.3-ti-r5.1
```















