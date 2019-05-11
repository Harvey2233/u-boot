#!/bin/bash
make clean
make -j8
./mkbl2 spl/u-boot-spl.bin bl2.bin 14336
# bl1
sudo dd iflag=dsync oflag=dsync if=E4412_N.bl1.bin of=/dev/sdb seek=1;
# bl2
sudo dd iflag=dsync oflag=dsync if=bl2.bin of=/dev/sdb seek=17;
# uboot
sudo dd iflag=dsync oflag=dsync if=u-boot.bin of=/dev/sdb seek=81;

sync
