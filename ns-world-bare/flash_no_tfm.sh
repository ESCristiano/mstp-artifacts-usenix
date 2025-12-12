#!/usr/bin/env bash
sn_option=""
if [ $# -eq 1 ]; then
sn_option="sn=$1"
fi

stm32programmercli="STM32_Programmer_CLI"

# Absolute path to this script
SCRIPT=$(readlink -f $0)
SCRIPTPATH=`dirname $SCRIPT`

# Absolute path this script
BIN_S="$SCRIPTPATH/src/Secure/build/STM32L5-Single-step_Cache_110MHz_S.elf"
BIN_NS="$SCRIPTPATH/src/NonSecure/build/STM32L5-Single-step_Cache_110MHz_NS.elf"

cubedir="`which STM32_Programmer_CLI`"
cubedir="$(dirname "${cubedir}")"
external_loader="-el $cubedir/ExternalLoader/MX25LM51245G_STM32L562E-DK.stldr"

connect_no_reset="-c port=SWD "$sn_option" mode=UR $external_loader"
connect="-c port=SWD "$sn_option" mode=UR $external_loader"

echo "Write TFM_Appli Secure"
# part ot be updated according to flash_layout.h

$stm32programmercli $connect -d $BIN_S -v
echo "Secure Written"

$stm32programmercli $connect -d $BIN_NS -v
echo "NonSecure Written"
