/**
  @page USBH_MSC  USB Host MSC example
  
  @verbatim
  ******************** (C) COPYRIGHT 2012 STMicroelectronics *******************
  * @file    readme.txt 
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   Description of the USB Host MSC example
  ******************************************************************************
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the Licens
  *   
  ******************************************************************************
   @endverbatim

   
@par Example Description 

This example provides a description of how to use the USB OTG host peripheral 
on the STM32F2xx, STM32F4xx and STM32F105/7 devices.

The STM32 behave as a mass storage Host that can enumerate, show content and display
the supported BMP image in the attached USB flash disk.
When the user press the user key button [B4], the application explore the USB flash
disk content and display it on the LCD. The user has to press the user key button
to display the whole disk flash (recursion level 2).
Once the entire disk flash content is explored, the user has to press the user key
button to write a small file (less to 1 KB) on the disk.
After writing the file to the disk, the user can press the user key button and start 
the Image slide show (BMP file located in the USB Disk root).
Once all the images are displayed, the application explores again the disk flash.

 @note Only the BMP files with the following format are supported : 
         - width  : 320
         - Height : 240
         - BPP    :  16
         - Compression : RGB bitmap with RGB masks  
       To use the Image browser, copy the BMP files from "\Utilities\Binary\Media"
       to the root of the USB flash disk to be used. 
       
This example works
  - in full speed (FS) when the STM32F4DISCOVERY board and the USB OTG FS peripheral are used

@par Hardware and Software environment 

   - This example runs on STM32F4xx devices.
  
   - This example has been tested with Devkit407. 

   - STM32F4DISCOVERY Set-up
    - Use CN5 connector to connect the board to a PC host when using USB OTG FS peripheral.


@par How to use it ?

   + MDK-ARM
    - Open the MDK project
    - Rebuild all files: Project->Rebuild all target files
    - Load project image: Debug->Start/Stop Debug Session
    - Run program: Debug->Run (F5)
    
@note Known Limitations
      This example retargets the C library printf() function to the EVAL board’s LCD
      screen (C library I/O redirected to LCD) to display some Library and user debug
      messages. TrueSTUDIO Lite version does not support I/O redirection, and instead
      have do-nothing stubs compiled into the C runtime library. 
      As consequence, when using this toolchain no debug messages will be displayed
      on the LCD (only some control messages in green will be displayed in bottom of
      the LCD). To use printf() with TrueSTUDIO Professional version, just include the
      TrueSTUDIO Minimal System calls file "syscalls.c" provided within the toolchain.
      It contains additional code to support printf() redirection.
    
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
