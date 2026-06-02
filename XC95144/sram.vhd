--===========================================================================================================
--==                                                                                                       ==
--==		SyncSRAM interface, address counter, sample counter                                        ==
--==                                                                                                       ==
--===========================================================================================================


library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity sram is
   port
   (
      clr		: in std_logic;				-- 1-> counter resets
      sram_clk		: in std_logic;				-- clock input

      ce_l		: out std_logic;			-- SRAM chip enable
      oe_l		: out std_logic;			-- SRAM output enable
      we_l		: out std_logic;			-- SRAM write enable
      adsc_l		: out std_logic;			-- SRAM address strobe
      sram_write	: in std_logic;				-- write to SRAM enabled
      adr_cnt_en	: in std_logic;				-- address counter enable
      adr_cnt		: out std_logic_vector(16 downto 0);	-- SRAM address

      --psize_reg		: in std_logic_vector(3 downto 0);	-- pre/posttrigger length
      --sample_cnt_en	: in std_logic;				-- sample counter enable
      --sample_cnt_wm	: out std_logic;			-- sample counter wm hit
      trig		: in std_logic;				-- trigger output
      sample_cnt_top	: out std_logic;				-- sample counter top hit
      reset_cntr :in std_logic  -- reset of the sram counter

   );
end entity sram;

architecture rtl of sram is

   signal adr_cnt_int		: std_logic_vector(16 downto 0);-- SRAM address counter
   --signal sample_cnt_int	: std_logic_vector(16 downto 0);-- pretrigger/postrigger counter
   --signal sample_cnt_top_int	: std_logic;			-- sample counter top hit (internal)

begin

   -- 1 ns delays added to avoid hold-time violation errors in RTL simulation

   we_l <= not(sram_write) after 1 ns;
   oe_l <= sram_write after 1 ns;
   ce_l <= '0';
   adsc_l <= '0';

  
   -- address counter
   process(clr, reset_cntr, sram_clk)
   begin
      if clr = '1' or reset_cntr = '1' then
         adr_cnt_int <= (others => '0');
      elsif sram_clk'event and sram_clk = '1' then
         if adr_cnt_en = '1' or trig = '1' then
            adr_cnt_int <= adr_cnt_int + 1;
	 end if;
      end if;
   end process;

   adr_cnt <= adr_cnt_int after 1 ns;
   sample_cnt_top <= '1' when adr_cnt_int =  "11111111111111111" else '0';

   -- vaclavpe - tohle je kvuli zkraceni simulace
--   adr_cnt <= "000000000" & adr_cnt_int(7 downto 0)  after 1 ns;
--   sample_cnt_top <= '1' when adr_cnt_int(7 downto 0) =  "11111111" else  '0';
   
 
end rtl;   

