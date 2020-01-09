# MKS SERVO42B
## Features
Based on the open project of nano_stepper by Misfittech:[https://github.com/Misfittech/nano_stepper](http:://github.com/Misfittech/nano_stepper "https://github.com/Misfittech/nano_stepper").

To improve performance, Makerbase Team has made the following changes:

- Ported the project to STMicroelectronics' 32-bit MCU, STM32F103C8T6 ARM 32-bit Cortex™-M3 CPU Core,72 MHz maximum frequency,20k RAM. 64k Flash
- Changed the magnetic encoder to Allegro's A1333LLETR-T Contactless 0° to 360° angle sensor IC ,12bit
- Ported the compilation platform from arduino to PlatformIO —— Convenient  for STM32 MCU compilation
- Change the socket with motherboard, now it can work with much 3D printer motherboards using MKS SV_EXT V1.1 by Makerbase



## Firmware
  This project support build and upload by platformio, you can use Atom or Code editor (need install platformio) build it or upload  firmware. Please refer to the following method:
### How to Build
  ATOM editor
  Open platformio.ini file, set default_envs = mks_servo42b
  ![MKS SERVO42B Build](https://github.com/makerbase-mks/MKS-SERVO42B/blob/master/Picture/AtomBuild.png "MKS SERVO42B Build")
  
  Code editor
  Open platformio.ini file, set default_envs = mks_servo42b
  ![MKS SERVO42B Build](https://github.com/makerbase-mks/MKS-SERVO42B/blob/master/Picture/CodeBuild.png "MKS SERVO42B Build")
  
### How to upload firmware
  You can use jlink or stlink upload firmware,the two upload methods are just different in platformio.ini settings
  Use jlink,you need upload_protocol = jlink
  Use stlink,you need upload_protocol = stlink
  ![jlink_stlink](https://github.com/makerbase-mks/MKS-SERVO42B/blob/master/Picture/jlink_stlink.png "jlink_stlink")
#### Use jlink connect MKS SERVO42B
  Use jlink connect MKS SERVO42B
  ![MKS SERVO42B SWD](https://github.com/makerbase-mks/MKS-SERVO42B/blob/master/Picture/SWDPIN.png "MKS SERVO42B SWD")
  ![MKS SERVO42B SWDPIN](https://github.com/makerbase-mks/MKS-SERVO42B/blob/master/Picture/MKSSERVO42BSWDPIN.png "MKS SERVO42B SWDPIN")
### Connect PC upload firmware
Need to connect jlink's SWDIO,SWCLK,RESET,GND to MKS SERVO42B motherboard's SWDIO,SWCLK,RESET,GND.

  If you are using jlink for the first time, you may need to install the driver,Upload firmware by jlink
  ,jlink connect PC Because MKS SERVO42B motherboard encrypted by Jflash,we need unsecure chip Usecure chip by Jflah
  ![jlink connect PC](https://github.com/makerbase-mks/MKS-SERVO42B/blob/master/Picture/jlink.png "jlink connect PC")
  ![Unsecurechip](https://github.com/makerbase-mks/MKS-SERVO42B/blob/master/Picture/Unsecurechip.png "Unsecurechip")
  Upload firmware by jlink 
  Upload firmware success, we need reinstall the motherboard on the back of the motor. 
  ![uploadsuccess](https://github.com/makerbase-mks/MKS-SERVO42B/blob/master/Picture/uploadsuccess.jpg "uploadsuccess")
  MKS SERVO57B upload firmware similar operation.If you have any problems, you can feedback to us and we will assist to solve it.

**Note: **
For the V1.0 version, you may need to remove the board from the motor, because the programming interface is located on the bottom of the PCB. Sorry for the mistake, we have improved in subsequent versions. 

## How to buy the MKS SERVO42B  
  https://www.aliexpress.com/item/33032696338.html?spm=a2g0o.store_home.productList_8356958.pic_1
![MKS SERVO42B](https://github.com/makerbase-mks/MKS-SERVO42B/blob/master/Picture/SERVO42B.png "MKS SERVO42B")

## License
The hardware is under the Creative Commons Attribution Share-Alike 4.0 License as much of the work is based on Mechaduino project by J. Church.
 [https://github.com/jcchurch13/Mechaduino-Firmware](https://github.com/jcchurch13/Mechaduino-Firmware "https://github.com/jcchurch13/Mechaduino-Firmware"). 

The firmware is based on nano_stepper project by Misfittech：[https://github.com/Misfittech/nano_stepper](http:://github.com/Misfittech/nano_stepper "https://github.com/Misfittech/nano_stepper"), which is licensed as GPL V3 for non-commercial use. If you want to release a closed source version of this product, please contact MisfitTech.net for licensing details.
  

