-- =====================================================
--
--   MiniLA testbench
--
-- =====================================================
--
-- This is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 2, or (at your option)
-- any later version.
--
-- This software is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this package; see the file COPYING.  If not, write to
-- the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
-- Boston, MA 02111-1307, USA.

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
library work;
use work.all;
--library simprim;
--use simprim.all;


entity tbench is
end entity tbench;

architecture tb of tbench is

   --constant clk_per	: time := 12.5 ns;	-- period of clock
   constant clk_per	: time := 10 ns;	-- period of clock
   
   signal dd		: std_logic_vector(7 downto 0) := (others => 'Z');
   signal dast_l	: std_logic := '1';
   signal ddst_l	: std_logic := '1';
   signal dwr_l		: std_logic := '1';
   signal dwait		: std_logic;

   -- SRAM interface
   signal sram_d	: std_logic_vector(31 downto 0);
   signal sram_adr	: std_logic_vector(16 downto 0);
   signal sram_ce_l	: std_logic;
   signal sram_oe_l	: std_logic;
   signal sram_we_l	: std_logic;
   signal sram_adsc_l	: std_logic;
   signal sram_clk	: std_logic;

   signal channel	: std_logic_vector(31 downto 0);

   signal clk		: std_logic := '0';
   signal eclk		: std_logic;
   signal eoclk		: std_logic;

   signal ex2		: std_logic;
   signal etrg1		: std_logic;
   signal etrg2		: std_logic;

   -- LED diodes
   signal st		: std_logic_vector(3 downto 1);

   signal clk_i		: std_logic; 
   signal clk_o		: std_logic;
   signal trig_o	: std_logic;

   signal read_data	: std_logic_vector(31 downto 0);
   signal read_num	: integer := 0;
   signal DONE		: std_logic := '0';


begin

   uut: entity minila(rtl)
   --uut: entity work.minila(structure)
   port map
   (
      -- EPP interface
      dd	=> dd,			-- inout
      dast_l	=> dast_l,		-- in
      ddst_l	=> ddst_l,		-- in
      dwr_l	=> dwr_l,		-- in
      dwait	=> dwait,		-- out
      
      -- SRAM interface
      sram_d	=> sram_d,		-- inout
      sram_adr	=> sram_adr,		-- out
      sram_ce_l	=> sram_ce_l,		-- out
      sram_oe_l	=> sram_oe_l,		-- out
      sram_we_l	=> sram_we_l,		-- out
      sram_adsc_l=> sram_adsc_l,	-- out
      sram_clk	=> sram_clk,		-- out

      -- test. data
      channel	=> channel,		-- in

      -- clock, trigger
      clk_100	=> clk,			-- in
      eclk	=> eclk,		-- in
      clk_o	=> clk_o,		-- out
      etrg1	=> etrg1,		-- in
      trig_o	=> trig_o,		-- out

      -- LED diodes
      st	=> st			-- out
    );

   sram_I: entity work.AS7C33128FT32
   generic map
   (
      -- Setup Times
      tAS       => 2.0 ns,
      tADSCS    => 2.0 ns,
      tADSPS    => 2.0 ns,
      tADVS     => 2.0 ns,
      tWS       => 2.0 ns,
      tDS       => 2.0 ns,
      tCSS      => 2.0 ns,
      -- Hold Times
      tAH       => 0.5 ns,
      tADSCH    => 0.5 ns,
      tADSPH    => 0.5 ns,
      tADVH     => 0.5 ns,
      tWH       => 0.5 ns,
      tDH       => 0.5 ns,
      tCSH      => 0.5 ns,
      -- Clock
      tCYC      => 8.5 ns,
      tCH       => 3.0 ns,
      tCL       => 3.0 ns,
      -- Output Times
      tHZC      => 3.5 ns,
      
      -- Bus Width and Data Bus
      addr_bits => 17,
      data_bits => 32
   )
   port map
   (
        Dq      => sram_d,
        Addr    => sram_adr,
        Lbo_n   => '0',
        Adv_n   => '0',
        Clk     => sram_clk,
        Adsc_n  => sram_adsc_l,
        Adsp_n  => '1',
        Bwa_n   => '1',
        Bwb_n   => '1',
        Bwc_n   => '1',
        Bwd_n   => '1',
        Bwe_n   => '1',
        Gw_n    => sram_we_l,
        Ce0_n   => sram_ce_l,
        Ce1     => '1',
        Ce2_n   => '0',
        Oe_n    => sram_oe_l,
        Zz      => '0'
   );

   -- clock generator
   process
   begin
      clk <= not(clk);
      wait for clk_per/2;
      if DONE = '1' then
         wait;
      end if;
   end process;

   -- test data generator
   process
      variable data	: integer := 1;
   begin
      wait for 15 ns;
      loop
	 channel <= conv_std_logic_vector(data, 32) after 1 ns;
	 data := data + 1;
	 if data > (2**31 - 1) then
	    data := 0;
	 end if;
	 if DONE = '1' then
	    wait;
	 end if;
	 for i in 1 to 1 loop
	    wait until clk_o = '1';
	 end loop;
      end loop;
   end process;
   


   -- main process
   test_P: process
      constant p_address	: boolean := true;
      constant p_data		: boolean := false;

      -- register map
      -- read
      constant data_reg		: std_logic_vector(7 downto 0) := conv_std_logic_vector(0, 8);
      --constant sample_cnt_reg	: std_logic_vector(7 downto 0) := conv_std_logic_vector(0, 8);
      constant status_reg	: std_logic_vector(7 downto 0) := conv_std_logic_vector(1, 8);
      --constant status2_reg	: std_logic_vector(7 downto 0) := conv_std_logic_vector(3, 8);
      -- write
      constant control_reg	: std_logic_vector(7 downto 0) := conv_std_logic_vector(0, 8);
      constant trigval_reg		: std_logic_vector(7 downto 0) := conv_std_logic_vector(1, 8);
      constant trigmask_reg		: std_logic_vector(7 downto 0) := conv_std_logic_vector(2, 8);
      constant timebsel_reg		: std_logic_vector(7 downto 0) := conv_std_logic_vector(3, 8);
      --constant psize_reg	: std_logic_vector(7 downto 0) := conv_std_logic_vector(4, 8);
      --constant val0_reg		: std_logic_vector(7 downto 0) := conv_std_logic_vector(5, 8);
      --constant val1_reg		: std_logic_vector(7 downto 0) := conv_std_logic_vector(6, 8);
      --constant mask0_reg	: std_logic_vector(7 downto 0) := conv_std_logic_vector(9, 8);
      --constant mask1_reg	: std_logic_vector(7 downto 0) := conv_std_logic_vector(10, 8);
      --constant trig_ctrl_reg	: std_logic_vector(7 downto 0) := conv_std_logic_vector(13, 8);

      variable st_trig		: boolean := false;
      variable st_fill		: boolean := false;
      variable data		: std_logic_vector(31 downto 0);
      variable i		: natural;
      variable st_preread	: boolean := false;
   
      -- clock synchronized delay
      procedure delay(delay: time) is
      begin
         for i in 1 to (delay/clk_per) loop
	    wait until clk = '1';
	 end loop;
	 wait for 1 ns;
      end delay;
      
      -- write of data to EPP interface
      procedure epp_write(address_data_l: boolean; data: std_logic_vector) is
      begin
         dd <= data;
	 delay(90 ns);
	 dwr_l <= '0';
	 delay(90 ns);
	 if address_data_l then
	    dast_l <= '0';
	 else
	    ddst_l <= '0';
	 end if;
	 delay(90 ns);
	 dast_l <= '1';
	 ddst_l <= '1';
	 delay(90 ns);
	 dwr_l <= '1';
	 dd <= (others => 'Z');
	 delay(100 ns);
      end epp_write;
      
      -- reading of data from EPP interface
      procedure epp_read(address_data_l: boolean; data: out std_logic_vector) is
      begin
	 delay(90 ns);
	 if address_data_l then
	    dast_l <= '0';
	 else
	    ddst_l <= '0';
	 end if;
	 delay(90 ns);
	 data := dd;
	 dast_l <= '1';
	 ddst_l <= '1';
	 delay(100 ns);
      end epp_read;
  
   begin
      
     report "---=== Test sequence #1 ===---" severity note;
  
      -- initialization

      -- control register
      epp_write(p_address, control_reg);
      epp_write(p_data,
                '0' &		-- run
		'1' &		-- reset
		'0' &		-- dummy
		'0' &		-- auto increment
		'0' &		-- stop
		'0' &		-- dummy
		"00"		-- data bytesel
	       );
      epp_write(p_data,
                '0' &		-- run
		'0' &		-- reset
		'0' &		-- dummy
		'0' &		-- auto increment
		'0' &		-- stop
		'0' &		-- dummy
		"00"		-- data bytesel
	       );
      
--      -- min. trigger event length
--      epp_write(p_address, tlen_reg);
--      epp_write(p_data, X"01");
--
--      -- number of trigger events
--      epp_write(p_address, tcnt_reg);
--      epp_write(p_data, X"01");

      -- timebase selector + ext trigger
      epp_write(p_address, timebsel_reg);
      epp_write(p_data, X"01");

--      -- pre/posttrigger length
--      epp_write(p_address, psize_reg);
--      epp_write(p_data, X"1F");
--
--      -- value reg (7:0)
--      epp_write(p_address, val0_reg);
--      epp_write(p_data, X"FF");
--
--      -- value reg (15:8)
--      epp_write(p_address, val1_reg);
--      epp_write(p_data, X"80");

      -- val reg (5:0)
      epp_write(p_address, trigval_reg);
      epp_write(p_data, X"00");

      -- mask reg (5:0)
      epp_write(p_address, trigmask_reg);
      epp_write(p_data, X"3F");

--      -- externi trigger
--      epp_write(p_address, trig_ctrl_reg);
--      epp_write(p_data, X"00");

      wait for 18 ns;

      -- start the sampling
      -- control register
      epp_write(p_address, control_reg);
      epp_write(p_data,
                '0' &		-- run
		'1' &		-- reset
		'0' &		-- dummy
		'0' &		-- auto increment
		'0' &		-- stop
		'0' &		-- dummy
		"00"		-- data bytesel
	       );
      epp_write(p_data,
                '1' &		-- run
		'0' &		-- reset
		'0' &		-- dummy
		'0' &		-- auto increment
		'0' &		-- stop
		'0' &		-- dummy
		"00"		-- data bytesel
	       );

      -- wait until sampling finished
      wait for 100 us;
      epp_write(p_address, status_reg);
      data(7) := '0'; -- DONE in status reg
      while data(7) /= '1' loop
         epp_read(p_data, data(7 downto 0));
	 if data(4) = '0' and not(st_trig) then
	    st_trig := true;
	    report "waiting for trigger" severity note;
	 end if;
	 if data(4) = '1' and not(st_fill) then
	    st_fill := true;
	    report "postrigger" severity note;
	 end if;
      end loop;

      report "reading data" severity note;
      
      -- set data_reg address(= control_reg)
      epp_write(p_address, data_reg);

      -- read_clock pulse
      epp_write(p_data,
		'0' &	-- run
		'0' &	-- reset
		'0' & 	-- dummy
		'1' &	-- auto_increment
		'0' &	-- stop
		'0' &	-- dummy
		"00"	-- data bytesel
	       );

      read_num <= 0;
      -- read data
     data(3) := '0';
     while data(3) = '0' loop
       
       epp_write(p_address, data_reg);
       epp_read(p_data, data(7 downto 0));
       epp_read(p_data, data(15 downto 8));
       epp_read(p_data, data(23 downto 16));
       epp_read(p_data, data(31 downto 24));
       
       read_data <= data;
       read_num <= read_num + 1;
       
       epp_write(p_address, status_reg);
       epp_read(p_data, data(7 downto 0));
       
      end loop;

     -- precist finalni bajt
       epp_write(p_address, data_reg);
       epp_read(p_data, data(7 downto 0));
       epp_read(p_data, data(15 downto 8));
       epp_read(p_data, data(23 downto 16));
       epp_read(p_data, data(31 downto 24));
       
       read_data <= data;
       read_num <= read_num + 1;
      DONE <= '1';

      wait for 10 us;
      report "***** TEST FINISHED *****" severity failure;
      wait;
 
   end process;
   

end tb;
