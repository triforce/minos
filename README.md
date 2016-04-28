minOS
=====

A minimal bare bones operating system based on a monolithic kernel design written purely in Assembly language.

Legacy boots from a pre-built disk image with planned support for GRUB multiboot.

Kernel size: 8 KiB

System Requirements:

* RAM: 3 MiB
* CPU: x86-64

Microprocessors tested on:

* Intel Pentium 4
* Intel Core 2

I want to say a big thanks to the Flat Assembler community (http://flatassembler.net) for their help and advice - The boards are full of great examples and I hope they inspire more to take up assembly programming.

Quickstart
----------
<p>Want to jump right in and run minOS? Follow these simple steps...</p>

<p><strong>Install the required software on your GNU/Linux distribution:</strong></p>
<p>Ubuntu/Debian: <code>apt-get install bochs fasm</code></p>
<p>Arch: <code>pacman -S bochs</code> | (install fasm from the aur, for example using aurget: <code>aurget -S fasm</code>)</p>

<p><strong>Clone this repo:</strong></p>
<code>git clone https://github.com/triforce/minos.git</code>

<p><strong>Switch to the minos directory:</strong></p>
<code>cd minos</code>

<p><strong>Make the img:</strong></p>
<code>make</code>

<p><strong>Run minOS in the Bochs emulator:</strong></p>
<code>bochs -f emulators/bochs_config</code>
