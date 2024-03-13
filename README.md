# switch-controller-client

## Introduction
MCU接收攝像頭的影像資料(JPEG)透過網路傳輸晶片傳輸給server端。

規格:
* MCU - ATMega1284p
* 攝像頭 - OV2640
* 網路傳輸晶片 - W5100

## OV2640MyRegs.h
暫存器初始化狀態

## OV2640Setting.cpp
操作OV2640的methods，包含了寫入暫存器、初始化、亮度、資料讀出等等

## Client_W5100_OV2640.ino
client端主程式需要做的事情有
* 讀取攝像頭影像
* 撰寫封包，並傳出至server
