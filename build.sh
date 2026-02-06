source build/envsetup_soc.sh
defconfig cv1812cp_wevb_0006a_spinor
# clean_all
cmake -B app/build -S app/
make -C app/build
cp app/build/app_demo ramdisk/rootfs/overlay/cv1812cp_wevb_0006a_spinor/bin
build_all
unzip -o install/soc_cv1812cp_wevb_0006a_spinor/upgrade.zip -d ~/share/Windows/upgrade/
# rm ~/share/Windows/upgrade/rootfs.spinor
# rm ~/share/Windows/upgrade/data.spinor
# rm ~/share/Windows/upgrade/boot.spinor

#  .\usb_dl.exe -c cv181x -s dual_os -i upgrade

# x 8000000 3C7000
# connect 0 EAGLE_EYES test123456
# ifconfig vnet0 hw ether 88:00:33:77:12:57
# ifconfig vnet0 192.168.31.68
# route add default gw 192.168.31.1
# ifconfig vnet0 up
