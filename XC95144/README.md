# TiniLA protocol

## version 2010-03-07

TiniLA GUI uses FT2232 interface only !

## EPP registers' map

    Address          Write               Read
    0x0         Control register    Data register
    0x1         Trigger value      Status register
    0x2         Trigger mask     Address in SRAM 8..1
    0x3         Time base        Address in SRAM 16..9

## Reg description - write

###         Control register

    D7 - RUN - data acqusition start
    D6 - CLR - TiniLA reset
    D5 - SRAMCNTR_CLEAR - SRAM address counter reset
    D4 - AUTO_INC - sets autoincrement and rtead the first sample
    D3 - STOP - data acqusition stop
    D2 - FALLEDGE - falling edge sampling
    D1-D0 - BYTESEL - not necessary, resetting is enough

###      Trigger value

    D6-D0 - trigger level

###      Trigger mask

    D6-D0 - '1' means that the trigger value is checked

###      Time base - including ext trigger enable

    D7 - ext trigger level
    D6 - '1' - ext trigger enable
    D3-D0 - time base settings

## Reg description - read

###         Data register

    D7-D0 - acquired data
          - most significant byte of 32-bit word goes first

###       Status register

    D7 - DONE - end of acquisition
    D6 - RUN - TiniLA is running
    D5 - CLR - TiniLA in reset
    D4 - TRIG - trigger condition fulfilled
    D3 - SRAM_A0 - Bit 0 of the SRAM address
    D2-D0 - FW_REV - firmware revision - current is "001"

## TiniLA recommended command sequence

The current command sequence is in the code. Below is the old one.

    CTRL_REG <=  "01000000" -- Reset
    CTRL_REG <=  "00000000" -- Reset off
    TIMEB_REG <= "00000010" -- Time base settings
    TRVAL_REG <= "01010101" -- Trigger value settings
    TRMSK_REG <= "00011111" -- Trigger mask settings
    CTRL_REG <=  "01000000" -- Reset
    CTRL_REG <=  "10000000" -- Run the acquisition

    wait until STATUS_REG(7) = '1'

    CTRL_REG <=  "00010000" -- autoincrement
    read 4 bytes
    until STATUS_REG(3) = '0'
    read 4 bytes - 0x..FE
    read 4 bytes - 0x..FF - this is bug in CPLD


