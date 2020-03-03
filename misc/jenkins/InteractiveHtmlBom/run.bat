@echo off

set kicad_path=c:\progra~1\KiCad
set ibom_cmd=%kicad_path%\bin\python.exe C:\stuff\InteractiveHtmlBom\InteractiveHtmlBom\generate_interactive_bom.py --no-browser --name-format %%f_latest --dest-dir ../ibom 
echo "ibom_cmd=%ibom_cmd%"

pwd
if not exist hardware/frankenso/frankenso.kicad_pcb echo hardware/frankenso/frankenso.kicad_pcb not found. Was this invoked from wrong folder?
if not exist hardware/frankenso/frankenso.kicad_pcb exit -1

%ibom_cmd% hardware/Common_Rail_MC33816/Common_Rail_MC33816.kicad_pcb
%ibom_cmd% hardware/brain_board/brain_board_STM32F407.kicad_pcb
%ibom_cmd% hardware/brain_board_176-pin/176-pin_board.kicad_pcb
%ibom_cmd% --extra-fields "mfg\,mfg#,vend1\,vend1#" hardware/CJ125_board/O2_input_CJ125.kicad_pcb --netlist-file hardware/CJ125_board/O2_input_CJ125.net
%ibom_cmd% --extra-fields "MFG\,MFG#,VEND1\,VEND1#" hardware/frankenso/frankenso.kicad_pcb --netlist-file hardware/frankenso/frankenso.net
%ibom_cmd% --extra-fields "mfg\,mfg#,vend1\,vend1#" hardware/frankenstein/frankenstein.kicad_pcb --netlist-file hardware/frankenstein/frankenstein.net
%ibom_cmd% hardware/mini48-stm32/mini48-stm32.kicad_pcb
%ibom_cmd% hardware/HighSideSwitch/VN750PS_E.kicad_pcb
%ibom_cmd% hardware/can_board/can_brd_1.kicad_pcb