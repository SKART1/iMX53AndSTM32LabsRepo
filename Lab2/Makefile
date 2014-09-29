obj-m:= modulePWM.o
PWD = $(shell pwd)
#KDIR := /home/lucid/ltib/rpm/BUILD/linux-2.6.35.3
KDIR := /lib/modules/$(shell uname -r)/build

all: modulePWM.ko


modulePWM.ko: modulePWM.c
	make M=$(PWD) -C $(KDIR) 
	#ARCH=arm CROSS_COMPILE=arm-fsl-linux-gnueabi-
	
	
clean:
	rm -rf *.o *.ko
