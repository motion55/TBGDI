This is the NTVDM driver for the DOS program Orcad 386.

To install, run the "INSTALL.CMD" script in the BIN directory. This 
will copy two files (BT1024.DRV & TBGDI.DRV) to Orcad's DRV directory. 
The location is determined from the environment variable "ORCADESP". 
The file TBGDI.DLL will be copied to Windows' System32 directory.

For installing into Windows Vista/7 (32-bit versions only), open a 
command prompt using "Run as administrator" as option. Change 
directory to the BIN folder and run the "INSTALL.CMD" script.

To uninstall, run the "REMOVE.CMD" script. On Windows 7 and Vista, the 
script should be "Run as administrator".

To use the driver, type either "BT1024.DRV" or "TBGDI.DRV" in the 
"Configured Display Driver" box when you configure ESP, SDT386 or 
PCB386. Select "BT1024.DRV" for a windowed display. Select "TBGDI.DRV" 
for a full screen display without the caption and borders.

Left clicking anywhere on the display area will capture the mouse. 
Mouse movement will now be directed to Orcad. To release from capture, 
the user can use Alt+Tab, Alt+Esc or Alt+Enter keys. 

An added magnify feature has also been added. On SDT, when the scale 
has been zoomed in to the X1 limit, Pressing CTRL-1 will enable a 2X 
magnify mode. Pressing CTRL-1 will disable it. When magnfied 2X, menu 
areas may be zoomed out of view.

The driver performs its own CPU throttling when idle. NTVDM.exe also 
throttles down the CPU usage when it detects the DOS session is idle. 
This driver attempts to reduce the effect of this throttling. However, 
to minimize the two from interacting, adjust idle senstivity to LOW. A 
sample pif file to launch Orcad with this setting is included in the 
BIN directory.

