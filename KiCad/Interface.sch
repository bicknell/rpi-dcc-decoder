EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector_Generic:Conn_01x01 J1
U 1 1 622A298D
P 1300 1500
F 0 "J1" H 1218 1275 50  0000 C CNN
F 1 "Rail-A" H 1218 1366 50  0000 C CNN
F 2 "" H 1300 1500 50  0001 C CNN
F 3 "~" H 1300 1500 50  0001 C CNN
	1    1300 1500
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J2
U 1 1 622A31CA
P 1300 2000
F 0 "J2" H 1218 1775 50  0000 C CNN
F 1 "Rail-B" H 1218 1866 50  0000 C CNN
F 2 "" H 1300 2000 50  0001 C CNN
F 3 "~" H 1300 2000 50  0001 C CNN
	1    1300 2000
	-1   0    0    1   
$EndComp
$Comp
L Isolator:4N35 U1
U 1 1 622A3A60
P 2800 1750
F 0 "U1" H 2800 2075 50  0000 C CNN
F 1 "4N35" H 2800 1984 50  0000 C CNN
F 2 "Package_DIP:DIP-6_W7.62mm" H 2600 1550 50  0001 L CIN
F 3 "https://www.vishay.com/docs/81181/4n35.pdf" H 2800 1750 50  0001 L CNN
	1    2800 1750
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J3
U 1 1 622A893D
P 4200 1500
F 0 "J3" H 4280 1542 50  0000 L CNN
F 1 "RPI 3.3v" H 4280 1451 50  0000 L CNN
F 2 "" H 4200 1500 50  0001 C CNN
F 3 "~" H 4200 1500 50  0001 C CNN
	1    4200 1500
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J4
U 1 1 622A905E
P 4200 1750
F 0 "J4" H 4280 1792 50  0000 L CNN
F 1 "RPI GPIO Pin" H 4280 1701 50  0000 L CNN
F 2 "" H 4200 1750 50  0001 C CNN
F 3 "~" H 4200 1750 50  0001 C CNN
	1    4200 1750
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J5
U 1 1 622A9EBC
P 4200 2000
F 0 "J5" H 4280 2042 50  0000 L CNN
F 1 "RPI Ground" H 4280 1951 50  0000 L CNN
F 2 "" H 4200 2000 50  0001 C CNN
F 3 "~" H 4200 2000 50  0001 C CNN
	1    4200 2000
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small_US R2
U 1 1 622AE15E
P 3550 1600
F 0 "R2" H 3618 1646 50  0000 L CNN
F 1 "10K" H 3618 1555 50  0000 L CNN
F 2 "" H 3550 1600 50  0001 C CNN
F 3 "~" H 3550 1600 50  0001 C CNN
	1    3550 1600
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small_US R1
U 1 1 622AEF4E
P 2000 1500
F 0 "R1" V 1795 1500 50  0000 C CNN
F 1 "1K3" V 1886 1500 50  0000 C CNN
F 2 "" H 2000 1500 50  0001 C CNN
F 3 "~" H 2000 1500 50  0001 C CNN
	1    2000 1500
	0    1    1    0   
$EndComp
Wire Wire Line
	3100 1750 3550 1750
Wire Wire Line
	3550 1500 4000 1500
Wire Wire Line
	3550 1700 3550 1750
Connection ~ 3550 1750
Wire Wire Line
	3550 1750 4000 1750
Wire Wire Line
	3100 1850 3550 1850
Wire Wire Line
	3550 1850 3550 2000
Wire Wire Line
	3550 2000 4000 2000
Wire Wire Line
	2350 1850 2500 1850
$Comp
L Device:D_Small D1
U 1 1 622B7F56
P 2350 1750
F 0 "D1" V 2300 1550 50  0000 L CNN
F 1 "1N4007" V 2400 1350 50  0000 L CNN
F 2 "" V 2350 1750 50  0001 C CNN
F 3 "~" V 2350 1750 50  0001 C CNN
	1    2350 1750
	0    1    1    0   
$EndComp
Wire Wire Line
	1500 1500 1900 1500
Wire Wire Line
	2100 1500 2350 1500
Wire Wire Line
	2350 1500 2350 1650
Wire Wire Line
	2350 1650 2500 1650
Connection ~ 2350 1650
Wire Wire Line
	2350 1850 2350 2000
Wire Wire Line
	2350 2000 1500 2000
Connection ~ 2350 1850
Text Notes 3550 1050 0    50   ~ 0
R2 is a pull up resistor.
Text Notes 1900 1050 0    50   ~ 0
R1 limits the current to the diodes.\nD1 provides reverse protection \n    to the 4N35 diode.
$Comp
L Connector_Generic:Conn_01x01 J1
U 1 1 622BCB94
P 1300 3500
F 0 "J1" H 1218 3275 50  0000 C CNN
F 1 "Rail-A" H 1218 3366 50  0000 C CNN
F 2 "" H 1300 3500 50  0001 C CNN
F 3 "~" H 1300 3500 50  0001 C CNN
	1    1300 3500
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J2
U 1 1 622BCB9E
P 1300 4000
F 0 "J2" H 1218 3775 50  0000 C CNN
F 1 "Rail-B" H 1218 3866 50  0000 C CNN
F 2 "" H 1300 4000 50  0001 C CNN
F 3 "~" H 1300 4000 50  0001 C CNN
	1    1300 4000
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J3
U 1 1 622BCBB2
P 4200 3250
F 0 "J3" H 4280 3292 50  0000 L CNN
F 1 "RPI 3.3v" H 4280 3201 50  0000 L CNN
F 2 "" H 4200 3250 50  0001 C CNN
F 3 "~" H 4200 3250 50  0001 C CNN
	1    4200 3250
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J4
U 1 1 622BCBBC
P 4200 3650
F 0 "J4" H 4280 3692 50  0000 L CNN
F 1 "RPI GPIO Pin" H 4280 3601 50  0000 L CNN
F 2 "" H 4200 3650 50  0001 C CNN
F 3 "~" H 4200 3650 50  0001 C CNN
	1    4200 3650
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J5
U 1 1 622BCBC6
P 4200 3850
F 0 "J5" H 4280 3892 50  0000 L CNN
F 1 "RPI Ground" H 4280 3801 50  0000 L CNN
F 2 "" H 4200 3850 50  0001 C CNN
F 3 "~" H 4200 3850 50  0001 C CNN
	1    4200 3850
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small_US R3
U 1 1 622BCBD0
P 3550 3450
F 0 "R3" H 3618 3496 50  0000 L CNN
F 1 "10K" H 3618 3405 50  0000 L CNN
F 2 "" H 3550 3450 50  0001 C CNN
F 3 "~" H 3550 3450 50  0001 C CNN
	1    3550 3450
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small_US R1
U 1 1 622BCBDA
P 1750 3500
F 0 "R1" V 1545 3500 50  0000 C CNN
F 1 "1K3" V 1636 3500 50  0000 C CNN
F 2 "" H 1750 3500 50  0001 C CNN
F 3 "~" H 1750 3500 50  0001 C CNN
	1    1750 3500
	0    1    1    0   
$EndComp
Wire Wire Line
	2350 3850 2500 3850
$Comp
L Device:D_Small D1
U 1 1 622BCBED
P 2350 3750
F 0 "D1" V 2300 3550 50  0000 L CNN
F 1 "1N4007" V 2400 3350 50  0000 L CNN
F 2 "" V 2350 3750 50  0001 C CNN
F 3 "~" V 2350 3750 50  0001 C CNN
	1    2350 3750
	0    1    1    0   
$EndComp
Wire Wire Line
	1500 3500 1650 3500
Wire Wire Line
	2350 3650 2500 3650
Text Notes 3550 3050 0    50   ~ 0
R2&R3 are pull up resistors.
Text Notes 1900 3050 0    50   ~ 0
R1 limits the current to the diodes.\nD1 provides reverse protection \n    to the 4N35 diode.\nC1 reduces ringing into the 6N137
$Comp
L Isolator:6N137 U?
U 1 1 622C34CD
P 2800 3650
F 0 "U?" H 2800 4117 50  0000 C CNN
F 1 "6N137" H 2800 4026 50  0000 C CNN
F 2 "Package_DIP:DIP-8_W7.62mm" H 2800 3150 50  0001 C CNN
F 3 "https://docs.broadcom.com/docs/AV02-0940EN" H 1950 4200 50  0001 C CNN
	1    2800 3650
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C1
U 1 1 622CB8DE
P 1850 3750
F 0 "C1" H 1650 3800 50  0000 L CNN
F 1 "270pf" H 1550 3700 50  0000 L CNN
F 2 "" H 1850 3750 50  0001 C CNN
F 3 "~" H 1850 3750 50  0001 C CNN
	1    1850 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	1500 4000 1850 4000
Wire Wire Line
	1850 4000 1850 3850
Wire Wire Line
	2350 3850 1850 3850
Connection ~ 2350 3850
Connection ~ 1850 3850
Wire Wire Line
	1850 3500 1850 3650
Wire Wire Line
	1850 3650 2350 3650
Connection ~ 1850 3650
Connection ~ 2350 3650
$Comp
L Device:R_Small_US R2
U 1 1 622DFFC2
P 3250 3450
F 0 "R2" H 3318 3496 50  0000 L CNN
F 1 "10K" H 3318 3405 50  0000 L CNN
F 2 "" H 3250 3450 50  0001 C CNN
F 3 "~" H 3250 3450 50  0001 C CNN
	1    3250 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	3100 3450 3100 3250
Wire Wire Line
	3100 3250 3250 3250
Wire Wire Line
	3100 3550 3250 3550
Wire Wire Line
	3250 3350 3250 3250
Connection ~ 3250 3250
Wire Wire Line
	3250 3250 3550 3250
Wire Wire Line
	3100 3650 3550 3650
Wire Wire Line
	3550 3650 3550 3550
Wire Wire Line
	3550 3350 3550 3250
Connection ~ 3550 3250
Wire Wire Line
	3550 3250 4000 3250
Wire Wire Line
	3100 3850 4000 3850
Wire Wire Line
	3550 3650 4000 3650
Connection ~ 3550 3650
$EndSCHEMATC
