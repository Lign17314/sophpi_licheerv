# aic8800端
波特率：921600
连接WiFi：connect 1 ssid pwd
烧录指令：x 8000000 3C7000

# Linux端
设置Mac地址
ifconfig vnet0 down
ifconfig vnet0 hw ether 88:00:33:77:12:57
ifconfig vnet0 up
