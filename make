#!/bin/bash
# minos build script

if [ $1 == "img"  ]; then
	echo "Building minos disk image..."
	rm imgs/build.img
	cd build
	./build_img
	cd ..
	if [ ! -f imgs/build.img ]; then
		echo "Build failed :("
	else
		k_size=`stat -c %s imgs/build.img`
		echo "Built imgs/build.img - Size: $k_size bytes"
	fi
fi

if [ $1 == "k" ]; then
	echo "Building minos multiboot kernel..."

        rm imgs/minos.BIN
        cd build
        ./build_kernel
        cd ..
        if [ ! -f grub/minos.BIN ]; then
                echo "Build failed :("
        else
                k_size=`stat -c %s grub/minos.BIN`
                echo "Built grub/minos.BIN - Size: $k_size bytes"
        fi
fi	
