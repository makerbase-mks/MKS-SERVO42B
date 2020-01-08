# MKS-SERVO42B(Include hardware and firmware)
  ![SERVO42B](https://github.com/makerbase-mks/MKS-SERVO42B/master/SERVO42B.png "MKS SERVO42B")
  CPU is STM32F103C8T6 ARM 32-bit Cortex™-M3 CPU Core,48 MHz maximum frequency,20k RAM. 64k Flash
  A1333LLETR-T Contactless 0° to 360° angle sensor IC ,12bit
  A4954 stepper drive,support max current 2.0A,firmware max current 1.5A
  Support OLED0.96(SPI interface)
  Parameter modification, can be set through oled screen
  Works with all motherboards with MKS SV_EXT V1.1
  
  https://www.aliexpress.com/item/33032696338.html?spm=a2g0o.store_home.productList_8356958.pic_1

## Firmware(Open source code)
  This project support build and upload by platformio, you can use Atom or Code editor (need install platformio) build it or upload  firmware. Please refer to the following method:
### How to Build
  ATOM editor
  Open platformio.ini file, set default_envs = mks_servo42b
  ![MKS SERVO42B Build](https://github.com/makerbase-mks/MKS-SERVO42B/AtomBuild.png "MKS SERVO42B Build")
  
  Code editor
  Open platformio.ini file, set default_envs = mks_servo42b
  ![MKS SERVO42B Build](https://github.com/makerbase-mks/MKS-SERVO42B/CodeBuild.png "MKS SERVO42B Build")
  
### How to upload firmware
  You can use jlink or stlink upload firmware,the two upload methods are just different in platformio.ini settings
  Use jlink,you need upload_protocol = jlink
  Use stlink,you need upload_protocol = stlink
  ![jlink_stlink](https://github.com/makerbase-mks/MKS-SERVO42B/jlink_stlink.png "jlink_stlink")
#### Use jlink connect MKS SERVO42B
  Use jlink connect MKS SERVO42B
  ![MKS SERVO42B SWD](https://github.com/makerbase-mks/MKS-SERVO42B/SWDPIN.png "MKS SERVO42B SWD")
  ![MKS SERVO42B SWDPIN](https://github.com/makerbase-mks/MKS-SERVO42B/MKSSERVO42BSWDPIN.png "MKS SERVO42B SWDPIN")
### Connect PC,upload firmware
  You may need to remove the motherboard, because the programming interface is located on the bottom of the PCB. I am sorry here, and we will correct it in the new version. MKS SERVO42B SWD MKS SERVO42B SWDPIN Need connect jlink's SWDIO,SWCLK,RESET,GND to MKS SERVO42B motherboard's SWDIO,SWCLK,RESET,GND.
  Connect PC,upload firmware
If you are using jlink for the first time, you may need to install the driver Upload firmware by jlink jlink connect PC Because MKS SERVO42B motherboard encrypted by Jflash,we need unsecure chip Usecure chip by Jflah
  ![jlink connect PC](https://github.com/makerbase-mks/MKS-SERVO42B/jlink.png "jlink connect PC")
  ![Unsecurechip](https://github.com/makerbase-mks/MKS-SERVO42B/Unsecurechip.png "Unsecurechip")
  Upload firmware by jlink Upload firmware by jlink Upload firmware success Upload firmware success we need reinstall the motherboard on the back of the motor. 
  ![uploadsuccess](https://github.com/makerbase-mks/MKS-SERVO42B/uploadsuccess.jpg "uploadsuccess")
  MKS SERVO57B upload firmware similar operation.If you have any problems, you can feedback to us and we will assist to solve it.
  

