extrn		DriverMain:proc

public		start

DGROUP          GROUP _TEXT, _DATA

_TEXT		segment byte public use16 'CODE'
		assume cs:DGROUP, ds:DGROUP
		org 0000h

start:
		jmp     DriverMain

_TEXT		ends
		end	start
