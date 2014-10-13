obj-m:= modulePWM.o
PWD = $(shell pwd)
KDIR := /home/lucid/ltib/rpm/BUILD/linux-2.6.35.3





all: modulePWM.c
	make M=$(PWD) -C $(KDIR) ARCH=arm CROSS_COMPILE=arm-fsl-linux-gnueabi-
	
	
clean:
	rm -rf *.o *.ko
