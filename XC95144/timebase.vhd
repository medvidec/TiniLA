-- =====================================================
--
--   Timebase
--
-- =====================================================

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity timebase is
   port
   (
      clr		: in std_logic;			-- miniLA reset
      clk_rd		: in std_logic;			-- memory read clock (from EPP)
      clk_in		: in std_logic;			-- main clock input
      clk_ext		: in std_logic;			-- external clock input
      clk_sel		: in std_logic_vector(3 downto 0);-- timebase selector
      clk_falling_edge	: in std_logic;			-- falling edge enable
      done		: in std_logic;			-- sampling completed
      clk_out		: out std_logic;		-- miniLA internal clock
      clk_out_e		: out std_logic;		-- clock output (to pin)
      sram_clk		: out std_logic			-- memory clock
   );
end entity timebase;

architecture rtl of timebase is
   signal clk_out_int	: std_logic;			-- clock
   signal clk_cnt	: std_logic_vector(16 downto 0);-- timebase counter
   signal clk_2		: std_logic;			-- divider by 2
   --signal clk_5f	: std_logic_vector(2 downto 0);	-- divider by 5
   signal sw_read_clk	: std_logic;			-- switch clock to EPP
   --signal sw_read_clk_f	: std_logic;			-- switch clock to EPP

begin

   -- clock divider
   process(clr, clk_in)
   begin
      if clr = '1' then
         clk_cnt <= (others => '0');
         clk_2 <= '0';
      elsif clk_in'event and clk_in = '1' then
         if clk_cnt = conv_std_logic_vector(0, 19) then
	    case clk_sel is                                                     -- kmitocet CLK/2
              -- pro oscilator 20MHz
              -- 0 - clk se uplatni primo
              -- 1 - platne "when others"
	       when "0010" => clk_cnt <= conv_std_logic_vector(       2-1, 17);	--  2 -> 5 MHz
	       when "0011" => clk_cnt <= conv_std_logic_vector(       5-1, 17);	--  3 -> 2 MHz
	       when "0100" => clk_cnt <= conv_std_logic_vector(      10-1, 17);	--  4 -> 1 MHz
	       when "0101" => clk_cnt <= conv_std_logic_vector(      20-1, 17);	--  5 -> 500 kHz
	       when "0110" => clk_cnt <= conv_std_logic_vector(      50-1, 17);	--  6 -> 200 kHz
	       when "0111" => clk_cnt <= conv_std_logic_vector(     100-1, 17);	--  7 -> 100 KHz
	       when "1000" => clk_cnt <= conv_std_logic_vector(     200-1, 17);	--  8 -> 50 KHz
	       when "1001" => clk_cnt <= conv_std_logic_vector(     500-1, 17);	--  9 -> 20 KHz
	       when "1010" => clk_cnt <= conv_std_logic_vector(    1000-1, 17);	-- 10 -> 10 KHz
	       when "1011" => clk_cnt <= conv_std_logic_vector(    2000-1, 17);	-- 11 -> 5 KHz
	       when "1100" => clk_cnt <= conv_std_logic_vector(    5000-1, 17);	-- 12 -> 2 KHz
	       when "1101" => clk_cnt <= conv_std_logic_vector(   10000-1, 17);	-- 13 -> 1 KHz
	       when "1110" => clk_cnt <= conv_std_logic_vector(   20000-1, 17);	-- 13 -> 500 Hz
	       when others => clk_cnt <= conv_std_logic_vector(       1-1, 17);	--  0  -> 10 MHz
	    end case; 
            
	    clk_2 <= not(clk_2);
	 else
	    clk_cnt <= clk_cnt - 1;
	    clk_2 <= clk_2;
	 end if;
      end if;
   end process;
   

   -- output mux
   process(clk_sel, clk_in, clk_cnt, clk_2, clk_ext, clk_rd)
   begin
      case clk_sel is
	 when "0000" =>                -- pro 20MHz
	    clk_out_int <= clk_in;
	 when "1111" =>                 -- externi hodiny
	    clk_out_int <= clk_ext;
	 when others =>                 -- ostatni hodiny
	    clk_out_int <= clk_2;
      end case;
   end process;


   -- switch to memory read clocks
   -- (in order to avoid glitches on clock signal, clock signal is switched in inactive level)
   
   -- rising  edge
   process(clr, clk_out_int)
   begin
      if clr = '1' then
	 sw_read_clk <= '0';
      elsif clk_out_int'event and clk_out_int = '1' then
         if done = '1' then
	    sw_read_clk <= '1';
	 else
	    sw_read_clk <= '0';
	 end if;
      end if;
   end process;

   -- vaclavpe - k cemu je dobra tato inverze ???
--     -- falling edge
--     process(clr, clk_out_int)
--     begin
--        if clr = '1' then
--  	 sw_read_clk_f <= '0';
--        elsif clk_out_int'event and clk_out_int = '0' then
--           if done = '1' then
--  	    sw_read_clk_f <= '1';
--  	 else
--  	    sw_read_clk_f <= '0';
--  	 end if;
--        end if;
--     end process;
   

   -- SRAM and memory counter clock
   sram_clk <= clk_rd when (sw_read_clk = '1') else --or (sw_read_clk_f = '1') else
   	       clk_out_int when clk_falling_edge = '0' else
               not(clk_out_int);

   -- internal miniLA clock
   clk_out <= clk_out_int when clk_falling_edge = '0' else
              not(clk_out_int);

   -- output clock (to pin)
   clk_out_e <= clk_out_int;
   
end rtl;

