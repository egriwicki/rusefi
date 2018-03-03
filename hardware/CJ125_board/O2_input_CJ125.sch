EESchema Schematic File Version 2
LIBS:KICAD_Older_Version
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:cj125
LIBS:O2_input_CJ125-cache
EELAYER 25 0
EELAYER END
$Descr A 11000 8500
encoding utf-8
Sheet 1 1
Title "CJ125 Wide O2 "
Date "2017-01-30"
Rev "R0.3"
Comp "rusEFI.com"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Notes 5125 6700 0    60   ~ 0
82.5 ohm for LSU4.2\n200 ohm for LSU4.9
$Comp
L C C7
U 1 1 4E39E9D3
P 6725 4975
F 0 "C7" V 6765 5030 50  0000 L CNN
F 1 "0.1uF" V 6575 4900 50  0000 L CNN
F 2 "SM0805" V 1230 2500 60  0001 C CNN
F 3 "" H 6725 4975 60  0001 C CNN
F 4 "avx,08055C104KAT2A" V 1230 2500 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,478-1395-1-ND" V 1230 2500 60  0001 C CNN "vend1,vend1#"
	1    6725 4975
	0    1    1    0   
$EndComp
Text HLabel 8300 5250 2    60   Input ~ 0
CJ125_UA
$Comp
L R R15
U 1 1 4E39E9D2
P 7975 5175
F 0 "R15" V 7875 5175 50  0000 C CNN
F 1 "1k" V 7975 5175 50  0000 C CNN
F 2 "SM0805" V 1130 2600 60  0001 C CNN
F 3 "" H 7975 5175 60  0001 C CNN
F 4 "Panasonic,ERA-6AEB102V" V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,P1.0KDACT-ND" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    7975 5175
	0    1    -1   0   
$EndComp
$Comp
L R R13
U 1 1 4E39E9D1
P 6800 5275
F 0 "R13" V 6750 5525 50  0000 C CNN
F 1 "100k" V 6800 5275 50  0000 C CNN
F 2 "SM0805" V 1130 2600 60  0001 C CNN
F 3 "" H 6800 5275 60  0001 C CNN
F 4 "vishay,CRCW0805100KFKEA" V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,541-100KCCT-ND" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    6800 5275
	0    1    1    0   
$EndComp
$Comp
L C C6
U 1 1 4E39E9CF
P 5350 4575
F 0 "C6" V 5305 4635 50  0000 L CNN
F 1 "0.033uF" V 5300 4200 50  0000 L CNN
F 2 "SM0805" V 1130 2600 60  0001 C CNN
F 3 "" H 5350 4575 60  0001 C CNN
F 4 "tdk,CGA4J2C0G1H333J125AA" V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,445-6950-1-ND" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    5350 4575
	0    1    -1   0   
$EndComp
$Comp
L R R8
U 1 1 4E39E9CE
P 4550 6175
F 0 "R8" V 4650 6175 50  0000 C CNN
F 1 "1k" V 4550 6175 50  0000 C CNN
F 2 "SM0805" V 1130 2600 60  0001 C CNN
F 3 "" H 4550 6175 60  0001 C CNN
F 4 "Panasonic,ERA-6AEB102V" V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,P1.0KDACT-ND" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    4550 6175
	0    1    1    0   
$EndComp
$Comp
L GND #PWR01
U 1 1 4E39E9CD
P 5900 4525
F 0 "#PWR01" H 5900 4525 30  0001 C CNN
F 1 "GND" H 5900 4455 30  0001 C CNN
F 2 "" H 5900 4525 60  0001 C CNN
F 3 "" H 5900 4525 60  0001 C CNN
	1    5900 4525
	-1   0    0    1   
$EndComp
$Comp
L R R4
U 1 1 4E39E9CC
P 4300 5075
F 0 "R4" V 4250 5275 50  0000 C CNN
F 1 "61.9" V 4300 5075 50  0000 C CNN
F 2 "SM0805" V 1130 2600 60  0001 C CNN
F 3 "" H 4300 5075 60  0001 C CNN
F 4 "vishay,CRCW080561R9FKEA" V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,541-61.9CCT-ND" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    4300 5075
	0    1    -1   0   
$EndComp
$Comp
L R R5
U 1 1 4E39E9CB
P 4300 5275
F 0 "R5" V 4250 5075 50  0000 C CNN
F 1 "10k" V 4300 5275 50  0000 C CNN
F 2 "SM0805" V 1130 2600 60  0001 C CNN
F 3 "" H 4300 5275 60  0001 C CNN
F 4 "BOURNS,CR0805-FX-1002ELF" V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,CR0805-FX-1002ELFCT-ND" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    4300 5275
	0    -1   1    0   
$EndComp
$Comp
L C C2
U 1 1 4E39E9CA
P 3900 6375
F 0 "C2" H 3925 6275 50  0000 L CNN
F 1 "0.033uF" H 3550 6275 50  0000 L CNN
F 2 "SM0805" V 780 2600 60  0001 C CNN
F 3 "" H 3900 6375 60  0001 C CNN
F 4 "tdk,CGA4J2C0G1H333J125AA" V 780 2600 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,445-6950-1-ND" V 780 2600 60  0001 C CNN "vend1,vend1#"
	1    3900 6375
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 4E39E9C9
P 8300 5700
F 0 "#PWR02" H 8300 5700 30  0001 C CNN
F 1 "GND" H 8300 5630 30  0001 C CNN
F 2 "" H 8300 5700 60  0001 C CNN
F 3 "" H 8300 5700 60  0001 C CNN
	1    8300 5700
	1    0    0    -1  
$EndComp
$Comp
L R R12
U 1 1 4E39E9C8
P 6250 4975
F 0 "R12" V 6200 5225 50  0000 C CNN
F 1 "100k" V 6250 4975 50  0000 C CNN
F 2 "SM0805" V 1130 2600 60  0001 C CNN
F 3 "" H 6250 4975 60  0001 C CNN
F 4 "vishay,CRCW0805100KFKEA" V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,541-100KCCT-ND" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    6250 4975
	0    1    1    0   
$EndComp
$Comp
L C C1
U 1 1 4E39E9C7
P 3200 5775
F 0 "C1" H 3210 5675 50  0000 L CNN
F 1 "1000pF" H 2875 5675 50  0000 L CNN
F 2 "SM0805" V 1130 2600 60  0001 C CNN
F 3 "" H 3200 5775 60  0001 C CNN
F 4 "avx,08052C102KAT2A " V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,478-1339-1-ND" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    3200 5775
	1    0    0    -1  
$EndComp
$Comp
L R R6
U 1 1 4E39E9C6
P 4300 5475
F 0 "R6" V 4350 5675 50  0000 C CNN
F 1 "6.8k" V 4300 5475 50  0000 C CNN
F 2 "SM0805" V 1130 2600 60  0001 C CNN
F 3 "" H 4300 5475 60  0001 C CNN
F 4 "vishay,CRCW08056K80FKEA" V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,541-6.80KCCT-ND" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    4300 5475
	0    1    1    0   
$EndComp
$Comp
L C C5
U 1 1 4E39E9C5
P 4600 4775
F 0 "C5" H 4475 4875 50  0000 L CNN
F 1 "220pf" V 4500 4500 50  0000 L CNN
F 2 "SM0805" V 1130 2600 60  0001 C CNN
F 3 "" H 4600 4775 60  0001 C CNN
F 4 "avx,08051A221JAT2A" V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,478-1286-1-ND" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    4600 4775
	1    0    0    1   
$EndComp
$Comp
L GND #PWR03
U 1 1 4E39E9C4
P 4600 4575
F 0 "#PWR03" H 4600 4575 30  0001 C CNN
F 1 "GND" H 4600 4505 30  0001 C CNN
F 2 "" H 4600 4575 60  0001 C CNN
F 3 "" H 4600 4575 60  0001 C CNN
	1    4600 4575
	-1   0    0    1   
$EndComp
$Comp
L R R1
U 1 1 4E39E9C3
P 2750 5575
F 0 "R1" V 2850 5575 50  0000 C CNN
F 1 "10k" V 2750 5575 50  0000 C CNN
F 2 "SM0805" V 1130 2600 60  0001 C CNN
F 3 "" H 2750 5575 60  0001 C CNN
F 4 "BOURNS,CR0805-FX-1002ELF" V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,CR0805-FX-1002ELFCT-ND" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    2750 5575
	0    1    1    0   
$EndComp
$Comp
L C C4
U 1 1 4E39E9C2
P 4350 5925
F 0 "C4" V 4300 5750 50  0000 L CNN
F 1 "0.1uF" V 4400 6000 50  0000 L CNN
F 2 "SM0805" V 1130 2600 60  0001 C CNN
F 3 "" H 4350 5925 60  0001 C CNN
F 4 "avx,08055C104KAT2A" V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,478-1395-1-ND" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    4350 5925
	0    -1   -1   0   
$EndComp
$Comp
L R R7
U 1 1 4E39E9C1
P 4400 5775
F 0 "R7" V 4450 6000 50  0000 C CNN
F 1 "10k" V 4400 5775 50  0000 C CNN
F 2 "SM0805" V 1130 2600 60  0001 C CNN
F 3 "" H 4400 5775 60  0001 C CNN
F 4 "BOURNS,CR0805-FX-1002ELF" V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,CR0805-FX-1002ELFCT-ND" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    4400 5775
	0    1    1    0   
$EndComp
$Comp
L GND #PWR04
U 1 1 4E39E9C0
P 3200 6025
F 0 "#PWR04" H 3200 6025 30  0001 C CNN
F 1 "GND" H 3200 5955 30  0001 C CNN
F 2 "" H 3200 6025 60  0001 C CNN
F 3 "" H 3200 6025 60  0001 C CNN
	1    3200 6025
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR05
U 1 1 4E39E9BF
P 3900 6625
F 0 "#PWR05" H 3900 6625 30  0001 C CNN
F 1 "GND" H 3900 6555 30  0001 C CNN
F 2 "" H 3900 6625 60  0001 C CNN
F 3 "" H 3900 6625 60  0001 C CNN
	1    3900 6625
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR06
U 1 1 4E39E9BE
P 4055 5275
F 0 "#PWR06" H 4055 5275 30  0001 C CNN
F 1 "GND" H 4055 5205 30  0001 C CNN
F 2 "" H 4055 5275 60  0001 C CNN
F 3 "" H 4055 5275 60  0001 C CNN
	1    4055 5275
	0    1    1    0   
$EndComp
Text HLabel 3900 6175 0    60   Input ~ 0
CJ125_UR
$Comp
L R R9
U 1 1 4E39E9BD
P 5325 6775
F 0 "R9" V 5425 6750 50  0000 C CNN
F 1 "82.5" V 5325 6775 50  0000 C CNN
F 2 "SM0805" V 1130 2600 60  0001 C CNN
F 3 "" H 5325 6775 60  0001 C CNN
F 4 "vishay,CRCW080582R5FKEA" V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,541-82.5CCT-ND" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    5325 6775
	0    1    1    0   
$EndComp
$Comp
L R R14
U 1 1 4E39E9BC
P 6800 5375
F 0 "R14" V 6850 5625 50  0000 C CNN
F 1 "470k" V 6800 5375 50  0000 C CNN
F 2 "SM0805" V 1130 2600 60  0001 C CNN
F 3 "" H 6800 5375 60  0001 C CNN
F 4 "vishay,CRCW0805470KFKEA" V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,541-470KCCT-ND" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    6800 5375
	0    1    1    0   
$EndComp
$Comp
L R R11
U 1 1 4E39E9BB
P 6200 5375
F 0 "R11" V 6250 5625 50  0000 C CNN
F 1 "4.7k" V 6200 5375 50  0000 C CNN
F 2 "SM0805" V 1130 2600 60  0001 C CNN
F 3 "" H 6200 5375 60  0001 C CNN
F 4 "vishay,CRCW08054K70FKEA" V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,541-4.70KCCT-ND" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    6200 5375
	0    1    1    0   
$EndComp
$Comp
L GND #PWR07
U 1 1 4E39E9BA
P 7075 6025
F 0 "#PWR07" H 7075 6025 30  0001 C CNN
F 1 "GND" H 7075 5955 30  0001 C CNN
F 2 "" H 7075 6025 60  0001 C CNN
F 3 "" H 7075 6025 60  0001 C CNN
	1    7075 6025
	1    0    0    -1  
$EndComp
$Comp
L C C8
U 1 1 4E39E9B9
P 7075 5775
F 0 "C8" H 6800 5675 50  0000 L CNN
F 1 "0.033uF" H 6750 5875 50  0000 L CNN
F 2 "SM0805" V 1130 2600 60  0001 C CNN
F 3 "" H 7075 5775 60  0001 C CNN
F 4 "tdk,CGA4J2C0G1H333J125AA" V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,445-6950-1-ND" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    7075 5775
	1    0    0    1   
$EndComp
Text HLabel 6075 5975 2    60   Input ~ 0
CJ125_SCK
Text HLabel 6075 5875 2    60   Input ~ 0
CJ125_SO
Text HLabel 6075 5675 2    60   Input ~ 0
CJ125_SS
Text HLabel 6075 5775 2    60   Input ~ 0
CJ125_SI
$Comp
L GND #PWR08
U 1 1 4E39E9B8
P 6925 4975
F 0 "#PWR08" H 6925 4975 30  0001 C CNN
F 1 "GND" H 6925 4905 30  0001 C CNN
F 2 "" H 6925 4975 60  0001 C CNN
F 3 "" H 6925 4975 60  0001 C CNN
	1    6925 4975
	0    -1   -1   0   
$EndComp
Text Label 7175 5275 0    60   ~ 0
WO2-VS-UN
Text Label 7175 5375 0    60   ~ 0
WO2-Rcal-IA
Text Label 3275 5175 0    60   ~ 0
WO2-Rcal-IA
Text Label 3200 4900 0    60   ~ 0
WO2-VS-UN
$Comp
L CJ125 U1
U 1 1 4E39E9B7
P 5350 5425
F 0 "U1" H 5200 6075 70  0000 C CNN
F 1 "CJ125" H 5250 4725 70  0000 C CNN
F 2 "SOIC-24_7.5x15.4mm_Pitch1.27mm" V 1130 2600 60  0001 C CNN
F 3 "" H 5350 5425 60  0001 C CNN
F 4 "bosh,CJ125-1267379259" V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "future,1267379259" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    5350 5425
	1    0    0    -1  
$EndComp
Text Label 3950 5475 2    60   ~ 0
WO2-HEATER
Text Label 3950 5375 2    60   ~ 0
WO2-HEAT-GATE
Text Label 3200 5000 0    60   ~ 0
WO2-IP
Text HLabel 3275 5175 0    60   Input ~ 0
CJ125_IA
Text HLabel 3200 5000 0    60   Input ~ 0
CJ125_IP
Text HLabel 3200 4900 0    60   Input ~ 0
CJ125_UN
Text HLabel 1950 5475 0    60   Input ~ 0
CJ125_HEATER
Text HLabel 7275 6775 2    60   Input ~ 0
CJ125_VM
Text Label 7075 5575 2    60   ~ 0
5v_reg
Text Label 2500 5575 2    60   ~ 0
5v_reg
Text HLabel 2175 5575 0    60   Input ~ 0
CJ125_5v
$Comp
L R R10
U 1 1 5770A293
P 5900 6775
F 0 "R10" V 6000 6750 50  0000 C CNN
F 1 "117.5" V 5900 6775 50  0000 C CNN
F 2 "SM0805" V 1130 2600 60  0001 C CNN
F 3 "" H 5900 6775 60  0001 C CNN
F 4 "Panasonic,ERJ-6ENF1180V" V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,P118CCT-ND" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    5900 6775
	0    1    1    0   
$EndComp
$Comp
L FDS2734 Q1
U 1 1 577188B8
P 2300 5075
F 0 "Q1" H 2203 5325 70  0000 C CNN
F 1 "FDS2734" H 2100 4875 60  0001 C CNN
F 2 "SO8E_ST" V 1105 2400 60  0001 C CNN
F 3 "" H 2300 5075 60  0001 C CNN
F 4 "ST,VNS14NV04" V 1105 2400 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,497-11123-1-ND" V 1105 2400 60  0001 C CNN "vend1,vend1#"
	1    2300 5075
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR09
U 1 1 57718AF6
P 2100 4575
F 0 "#PWR09" H 2100 4575 30  0001 C CNN
F 1 "GND" H 2100 4505 30  0001 C CNN
F 2 "" H 2100 4575 60  0001 C CNN
F 3 "" H 2100 4575 60  0001 C CNN
	1    2100 4575
	-1   0    0    1   
$EndComp
$Comp
L TEST W2
U 1 1 57719ACF
P 5900 7025
F 0 "W2" H 5900 7085 40  0000 C CNN
F 1 "TEST" H 5900 6955 40  0001 C CNN
F 2 "R_0603" V 1130 2600 60  0001 C CNN
F 3 "" H 5900 7025 60  0001 C CNN
F 4 "dnp,dnp" V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "dnp,dnp" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    5900 7025
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 5771DC73
P 3800 5775
F 0 "R2" V 3900 5575 50  0000 C CNN
F 1 "21k6" V 3800 5775 50  0000 C CNN
F 2 "SM0805" V 1130 2600 60  0001 C CNN
F 3 "" H 3800 5775 60  0001 C CNN
F 4 "vishay,CRCW080521K5FKEA" V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,541-21.5KCCT-ND" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    3800 5775
	0    1    1    0   
$EndComp
$Comp
L TEST W1
U 1 1 5771E75B
P 3850 5925
F 0 "W1" H 3850 5985 40  0000 C CNN
F 1 "TEST" H 3850 5855 40  0001 C CNN
F 2 "R_0603" V 1130 2600 60  0001 C CNN
F 3 "" H 3850 5925 60  0001 C CNN
F 4 "dnp,dnp" V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "dnp,dnp" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    3850 5925
	1    0    0    -1  
$EndComp
Text Notes 2675 6350 0    60   ~ 0
10.0k LS4.2\n31.6k LS4.9
$Comp
L R R3
U 1 1 57722197
P 4025 4900
F 0 "R3" V 4100 4900 50  0000 C CNN
F 1 "0R" V 4025 4900 50  0000 C CNN
F 2 "SM0805" V 1130 2600 60  0001 C CNN
F 3 "" H 4025 4900 60  0001 C CNN
F 4 "Matsushita,ERJ6GEY0R00V" V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,P0.0ADKR-ND" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    4025 4900
	0    1    -1   0   
$EndComp
$Comp
L C C3
U 1 1 577227C9
P 4275 4700
F 0 "C3" H 4150 4800 50  0000 L CNN
F 1 "DNP" V 4175 4625 50  0000 R CNN
F 2 "SM0805" V 1130 2600 60  0001 C CNN
F 3 "" H 4275 4700 60  0001 C CNN
F 4 "avx,08055C104KAT2A" V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,478-1395-1-ND" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    4275 4700
	1    0    0    1   
$EndComp
$Comp
L GND #PWR010
U 1 1 57722891
P 4275 4500
F 0 "#PWR010" H 4275 4500 30  0001 C CNN
F 1 "GND" H 4275 4430 30  0001 C CNN
F 2 "" H 4275 4500 60  0001 C CNN
F 3 "" H 4275 4500 60  0001 C CNN
	1    4275 4500
	-1   0    0    1   
$EndComp
$Comp
L CONN_02X03 P4
U 1 1 5777931C
P 4050 3300
F 0 "P4" H 4050 3500 50  0000 C CNN
F 1 "CONN_02X03" H 4050 3100 50  0001 C CNN
F 2 "Socket_Strips:Socket_Strip_Straight_2x03" V 1130 2600 60  0001 C CNN
F 3 "" H 4050 3300 60  0001 C CNN
F 4 "dnp,dnp" V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "dnp,dnp" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    4050 3300
	-1   0    0    1   
$EndComp
Text Label 5900 5675 0    60   ~ 0
SS
Text Label 5900 5775 0    60   ~ 0
SI
Text Label 5900 5875 0    60   ~ 0
SO
Text Label 5900 5975 0    60   ~ 0
SCK
Text Label 3900 6175 0    60   ~ 0
WO2-UR
Text Label 8300 5175 0    60   ~ 0
WO2-UA
Text HLabel 4925 4575 1    60   Input ~ 0
WO2-12V
Text Label 5900 5475 0    60   ~ 0
VM
Text Label 5900 5575 0    60   ~ 0
VCC
Text Label 5900 5375 0    60   ~ 0
US
Text Label 5900 5275 0    60   ~ 0
UP
Text Label 5900 5175 0    60   ~ 0
UA
Text Label 5900 5075 0    60   ~ 0
CF
Text Label 5900 4975 0    60   ~ 0
RF
Text Label 5900 4875 0    60   ~ 0
GND
Text Label 4800 5975 2    60   ~ 0
UR
Text Label 4800 5875 2    60   ~ 0
CM
Text Label 4800 5775 2    60   ~ 0
RM
Text Label 4800 5675 2    60   ~ 0
RS
Text Label 4800 5575 2    60   ~ 0
RST
Text Label 4800 5475 2    60   ~ 0
DIAHD
Text Label 4800 5375 2    60   ~ 0
DIAHG
Text Label 4800 5275 2    60   ~ 0
OSZ
Text Label 4800 5175 2    60   ~ 0
IA
Text Label 4800 5075 2    60   ~ 0
IP
Text Label 4800 4975 2    60   ~ 0
UN
Text Label 4800 4875 2    60   ~ 0
UB
Text Label 4300 3300 0    60   ~ 0
WO2-HEATER
$Comp
L GND #PWR011
U 1 1 57781F45
P 7350 3325
F 0 "#PWR011" H 7350 3325 30  0001 C CNN
F 1 "GND" H 7350 3255 30  0001 C CNN
F 2 "" H 7350 3325 60  0001 C CNN
F 3 "" H 7350 3325 60  0001 C CNN
	1    7350 3325
	0    -1   -1   0   
$EndComp
Text Label 7350 3225 0    60   ~ 0
5v_reg
Text Label 7350 3525 0    60   ~ 0
SS
Text Label 6850 3525 2    60   ~ 0
SI
Text Label 7350 3625 0    60   ~ 0
SO
Text Label 6850 3625 2    60   ~ 0
SCK
Text Label 7200 6775 2    60   ~ 0
WO2-VS/IP-VM
Text Label 3800 3300 2    60   ~ 0
WO2-12V
Text Label 4300 3200 0    60   ~ 0
WO2-IP
Text Label 5150 4575 1    60   ~ 0
WO2-12V
Text Label 4300 3400 0    60   ~ 0
WO2-Rcal-IA
Text Label 3800 3400 2    60   ~ 0
WO2-VS-UN
Text Label 3800 3200 2    60   ~ 0
WO2-VS/IP-VM
Text Label 6850 3325 2    60   ~ 0
WO2-12V
Text Label 7350 3425 0    60   ~ 0
WO2-HEAT-GATE
Text Notes 3625 3025 0    60   ~ 0
LSU4.9 OR 4.2 SENSOR
Text Label 5800 3125 2    60   ~ 0
WO2-UA
Text Label 5800 3425 2    60   ~ 0
WO2-UR
$Comp
L CONN_02X05 P8
U 1 1 5779EC14
P 7100 3425
F 0 "P8" H 7100 3725 50  0000 C CNN
F 1 "CONN_02X05" H 7100 3125 50  0001 C CNN
F 2 "Socket_Strips:Socket_Strip_Straight_2x05" V 1130 2600 60  0001 C CNN
F 3 "" H 7100 3425 60  0001 C CNN
F 4 "dnp,dnp" V 1130 2600 60  0001 C CNN "mfg,mfg#"
F 5 "dnp,dnp" V 1130 2600 60  0001 C CNN "vend1,vend1#"
	1    7100 3425
	1    0    0    -1  
$EndComp
Text Notes 6925 3000 0    60   ~ 0
MCU signals
$Comp
L PWR_FLAG #FLG012
U 1 1 577A3704
P 700 7775
F 0 "#FLG012" H 700 7870 30  0001 C CNN
F 1 "PWR_FLAG" H 700 7955 30  0000 C CNN
F 2 "" H 700 7775 60  0000 C CNN
F 3 "" H 700 7775 60  0000 C CNN
	1    700  7775
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR013
U 1 1 577A3836
P 700 7775
F 0 "#PWR013" H 700 7775 30  0001 C CNN
F 1 "GND" H 700 7705 30  0001 C CNN
F 2 "" H 700 7775 60  0001 C CNN
F 3 "" H 700 7775 60  0001 C CNN
	1    700  7775
	1    0    0    -1  
$EndComp
$Comp
L R R16
U 1 1 57A0B3E9
P 2500 4825
F 0 "R16" V 2400 4825 50  0000 C CNN
F 1 "10k" V 2500 4825 50  0000 C CNN
F 2 "SM0805" V -670 2150 60  0001 C CNN
F 3 "" H 2500 4825 60  0001 C CNN
F 4 "BOURNS,CR0805-FX-1002ELF" V -670 2150 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,CR0805-FX-1002ELFCT-ND" V -670 2150 60  0001 C CNN "vend1,vend1#"
	1    2500 4825
	-1   0    0    -1  
$EndComp
Text Notes 3100 7025 0    60   ~ 0
1kHz low pass filter / slew rate limiter.
Text Notes 7775 4725 0    60   ~ 0
1kHz low pass filter / slew rate limiter.
$Comp
L C C9
U 1 1 58908DB7
P 8300 5450
F 0 "C9" H 8325 5350 50  0000 L CNN
F 1 "0.033uF" H 7950 5350 50  0000 L CNN
F 2 "SM0805" V 5180 1675 60  0001 C CNN
F 3 "" H 8300 5450 60  0001 C CNN
F 4 "tdk,CGA4J2C0G1H333J125AA" V 5180 1675 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,445-6950-1-ND" V 5180 1675 60  0001 C CNN "vend1,vend1#"
	1    8300 5450
	-1   0    0    1   
$EndComp
Connection ~ 4025 5175
Wire Wire Line
	3275 5175 4800 5175
Wire Wire Line
	5900 5075 6500 5075
Wire Wire Line
	7725 5175 5900 5175
Wire Wire Line
	8300 5650 8300 5700
Wire Wire Line
	7050 5275 7175 5275
Wire Wire Line
	6450 5375 6550 5375
Wire Wire Line
	6550 5375 6550 5275
Wire Wire Line
	5900 5575 7075 5575
Wire Wire Line
	3375 6775 3375 5675
Wire Wire Line
	3375 5675 4800 5675
Connection ~ 4600 4975
Wire Wire Line
	4275 4975 4800 4975
Wire Wire Line
	4025 5175 4025 5075
Wire Wire Line
	4050 5275 4055 5275
Wire Wire Line
	4800 5475 4550 5475
Wire Wire Line
	4550 5075 4800 5075
Wire Wire Line
	3900 6575 3900 6625
Wire Wire Line
	4800 6175 4800 5975
Connection ~ 3200 5575
Wire Wire Line
	3000 5575 4800 5575
Wire Wire Line
	4650 5775 4800 5775
Wire Wire Line
	5550 4575 5900 4575
Wire Wire Line
	4800 4875 4800 4575
Wire Wire Line
	4800 4575 5150 4575
Wire Wire Line
	5900 4525 5900 4875
Connection ~ 5900 4575
Wire Wire Line
	4550 5925 4650 5925
Wire Wire Line
	4650 5925 4650 5875
Wire Wire Line
	4650 5875 4800 5875
Wire Wire Line
	3200 5975 3200 6025
Wire Wire Line
	4550 5275 4800 5275
Wire Wire Line
	2500 5375 4800 5375
Wire Wire Line
	4025 5075 4050 5075
Wire Wire Line
	1950 5475 4050 5475
Wire Wire Line
	4550 5000 4550 5075
Connection ~ 4550 5075
Wire Wire Line
	3200 5000 4550 5000
Wire Wire Line
	3900 6175 4300 6175
Wire Wire Line
	5900 5475 7275 5475
Wire Wire Line
	7275 5475 7275 6775
Wire Wire Line
	5900 5975 6075 5975
Wire Wire Line
	5900 5875 6075 5875
Wire Wire Line
	5900 5675 6075 5675
Wire Wire Line
	5900 5775 6075 5775
Wire Wire Line
	7075 5975 7075 6025
Wire Wire Line
	6550 5275 5900 5275
Wire Wire Line
	5950 5375 5900 5375
Wire Wire Line
	7050 5375 7175 5375
Wire Wire Line
	8300 5250 8300 5175
Wire Wire Line
	5900 4975 6000 4975
Wire Wire Line
	8300 5175 8225 5175
Wire Wire Line
	2500 5575 2175 5575
Wire Wire Line
	5075 6775 3375 6775
Wire Wire Line
	7275 6775 6150 6775
Wire Wire Line
	5575 6775 5650 6775
Connection ~ 2100 5475
Wire Wire Line
	5650 6775 5650 7025
Wire Wire Line
	5650 7025 5700 7025
Wire Wire Line
	6150 6775 6150 7025
Wire Wire Line
	6150 7025 6100 7025
Wire Notes Line
	5100 6500 5100 7125
Wire Notes Line
	5100 7125 6200 7125
Wire Notes Line
	6200 7125 6200 6500
Wire Notes Line
	6200 6500 5100 6500
Wire Wire Line
	4050 5925 4050 5775
Wire Wire Line
	4050 5775 4150 5775
Wire Wire Line
	3550 5775 3550 6000
Wire Wire Line
	3550 5925 3650 5925
Wire Wire Line
	4150 5925 4150 6000
Wire Wire Line
	4150 6000 3550 6000
Connection ~ 3550 5925
Wire Notes Line
	3500 5700 4875 5700
Wire Notes Line
	4875 5700 4875 6050
Wire Notes Line
	4875 6050 3500 6050
Wire Notes Line
	3500 6050 3500 5700
Wire Notes Line
	3500 6050 3250 6175
Wire Wire Line
	4275 4975 4275 4900
Wire Wire Line
	3775 4900 3200 4900
Connection ~ 4275 4900
Connection ~ 6550 5375
Connection ~ 6550 5275
Wire Wire Line
	2100 5325 2100 5475
Wire Wire Line
	2500 5375 2500 5075
Wire Wire Line
	2500 4575 2100 4575
Wire Wire Line
	2100 4575 2100 4825
Wire Wire Line
	6500 4975 6525 4975
Wire Wire Line
	6500 5075 6500 4975
Wire Notes Line
	4600 6900 4425 6325
Wire Notes Line
	8175 5050 8300 4775
Connection ~ 3900 6175
$Comp
L R R17
U 1 1 5A774A0C
P 6050 3125
F 0 "R17" V 6000 3350 50  0000 C CNN
F 1 "6.8k" V 6050 3125 50  0000 C CNN
F 2 "rsuEFI_Lib:R_0805" V 2880 250 60  0001 C CNN
F 3 "" H 6050 3125 60  0001 C CNN
F 4 "vishay,CRCW08056K80FKEA" V 2880 250 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,541-6.80KCCT-ND" V 2880 250 60  0001 C CNN "vend1,vend1#"
	1    6050 3125
	0    1    1    0   
$EndComp
$Comp
L R R18
U 1 1 5A774AF8
P 6050 3225
F 0 "R18" V 6100 3450 50  0000 C CNN
F 1 "10k" V 6050 3225 50  0000 C CNN
F 2 "rsuEFI_Lib:R_0805" V 2780 50  60  0001 C CNN
F 3 "" H 6050 3225 60  0001 C CNN
F 4 "BOURNS,CR0805-FX-1002ELF" V 2780 50  60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,CR0805-FX-1002ELFCT-ND" V 2780 50  60  0001 C CNN "vend1,vend1#"
	1    6050 3225
	0    1    1    0   
$EndComp
$Comp
L R R19
U 1 1 5A775F61
P 6050 3425
F 0 "R19" V 6000 3650 50  0000 C CNN
F 1 "6.8k" V 6050 3425 50  0000 C CNN
F 2 "rsuEFI_Lib:R_0805" V 2880 550 60  0001 C CNN
F 3 "" H 6050 3425 60  0001 C CNN
F 4 "vishay,CRCW08056K80FKEA" V 2880 550 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,541-6.80KCCT-ND" V 2880 550 60  0001 C CNN "vend1,vend1#"
	1    6050 3425
	0    1    1    0   
$EndComp
$Comp
L R R20
U 1 1 5A775F69
P 6050 3525
F 0 "R20" V 6100 3750 50  0000 C CNN
F 1 "10k" V 6050 3525 50  0000 C CNN
F 2 "rsuEFI_Lib:R_0805" V 2780 350 60  0001 C CNN
F 3 "" H 6050 3525 60  0001 C CNN
F 4 "BOURNS,CR0805-FX-1002ELF" V 2780 350 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,CR0805-FX-1002ELFCT-ND" V 2780 350 60  0001 C CNN "vend1,vend1#"
	1    6050 3525
	0    1    1    0   
$EndComp
Wire Wire Line
	6300 3525 6300 3425
Wire Wire Line
	6300 3225 6300 3125
$Comp
L GND #PWR014
U 1 1 5A77742D
P 5800 3525
F 0 "#PWR014" H 5800 3525 30  0001 C CNN
F 1 "GND" H 5800 3455 30  0001 C CNN
F 2 "" H 5800 3525 60  0001 C CNN
F 3 "" H 5800 3525 60  0001 C CNN
	1    5800 3525
	0    1    1    0   
$EndComp
$Comp
L GND #PWR015
U 1 1 5A7774A1
P 5800 3225
F 0 "#PWR015" H 5800 3225 30  0001 C CNN
F 1 "GND" H 5800 3155 30  0001 C CNN
F 2 "" H 5800 3225 60  0001 C CNN
F 3 "" H 5800 3225 60  0001 C CNN
	1    5800 3225
	0    1    1    0   
$EndComp
Wire Wire Line
	6300 3425 6850 3425
Wire Wire Line
	6850 3225 6300 3225
Text Notes 5150 2975 0    60   ~ 0
Default 3.3V max ADC input\nFor 5V ADC input R17, R19 = 0R\nthen R18, R20 DNP
$Comp
L LED D2
U 1 1 5A7C8B1F
P 8275 3225
F 0 "D2" H 8275 3325 50  0000 C CNN
F 1 "LED" H 8275 3125 50  0001 C CNN
F 2 "LEDs:LED_0805" H 8275 3225 60  0001 C CNN
F 3 "" H 8275 3225 60  0001 C CNN
F 4 "lite-on,LTST-C170TBKT" H 8275 3225 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,160-1579-1-ND" H 8275 3225 60  0001 C CNN "vend1,vend1#"
	1    8275 3225
	1    0    0    -1  
$EndComp
$Comp
L R R22
U 1 1 5A7CA859
P 7825 3225
F 0 "R22" V 7725 3225 50  0000 C CNN
F 1 "1k" V 7825 3225 50  0000 C CNN
F 2 "rsuEFI_Lib:R_0805" V 980 650 60  0001 C CNN
F 3 "" H 7825 3225 60  0001 C CNN
F 4 "Panasonic,ERA-6AEB102V" V 980 650 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,P1.0KDACT-ND" V 980 650 60  0001 C CNN "vend1,vend1#"
	1    7825 3225
	0    -1   1    0   
$EndComp
$Comp
L GND #PWR016
U 1 1 5A7CAAFA
P 8475 3225
F 0 "#PWR016" H 8475 3225 30  0001 C CNN
F 1 "GND" H 8475 3155 30  0001 C CNN
F 2 "" H 8475 3225 60  0001 C CNN
F 3 "" H 8475 3225 60  0001 C CNN
	1    8475 3225
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7350 3225 7575 3225
$Comp
L LED D1
U 1 1 5A7D2D42
P 4300 3750
F 0 "D1" H 4300 3850 50  0000 C CNN
F 1 "LED" H 4300 3650 50  0001 C CNN
F 2 "LEDs:LED_0805" H 4300 3750 60  0001 C CNN
F 3 "" H 4300 3750 60  0001 C CNN
F 4 "lite-on,LTST-C170TBKT" H 4300 3750 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,160-1579-1-ND" H 4300 3750 60  0001 C CNN "vend1,vend1#"
	1    4300 3750
	1    0    0    -1  
$EndComp
Text Label 3600 3750 2    60   ~ 0
WO2-12V
Text Label 4500 3750 0    60   ~ 0
WO2-HEATER
$Comp
L R R21
U 1 1 5A7D3046
P 3850 3750
F 0 "R21" V 3900 4000 50  0000 C CNN
F 1 "4.7k" V 3850 3750 50  0000 C CNN
F 2 "rsuEFI_Lib:R_0805" V -1220 975 60  0001 C CNN
F 3 "" H 3850 3750 60  0001 C CNN
F 4 "vishay,CRCW08054K70FKEA" V -1220 975 60  0001 C CNN "mfg,mfg#"
F 5 "DIGI,541-4.70KCCT-ND" V -1220 975 60  0001 C CNN "vend1,vend1#"
	1    3850 3750
	0    1    1    0   
$EndComp
$EndSCHEMATC
