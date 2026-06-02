-- =====================================================
--
--   tiniLA
--
-- =====================================================

-- MiniLA Tiny ChangeLog
-- =====================
-- 2010-03-04 - entita prejmenovana na tiniLA, trigger 6bitovy, do epp pridan sram_adr, at muzeme zjistit, kde se zastavilo
-- 2010-02-10 - speed grade v TiniLA je -10, tzn. nejvyssi rychlost je 20MHz
-- 2010-01-22 - vaclavpe - FW verze 001 - vychazi z verze timeanalysis 1.5.2,
--     zakladni simulace projde, na konci vycitani dat se sample_cnt_top nastavi
--     prilis brzy - pred ctenim adresy 0x..FE
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
library work;
use work.all;

entity tiniLA is
   generic
   (
      CPLD_FW_VER	: std_logic_vector(2 downto 0) := "001";	-- CPLD firmware version
      TRIGGER_WIDTH     : integer := 6        -- sirka triggeru je tady kvuli testovani
   );
   port
   (
      -- EPP interface
      dd		: inout std_logic_vector(7 downto 0);
      dast_l		: in std_logic;
      ddst_l		: in std_logic;
      dwr_l		: in std_logic;
      dwait		: out std_logic;
      
      -- SRAM interface
      sram_d		: inout std_logic_vector(31 downto 0);
      sram_adr		: out std_logic_vector(16 downto 0);
      sram_ce_l		: out std_logic;
      sram_oe_l		: out std_logic;
      sram_we_l		: out std_logic;
      sram_adsc_l	: out std_logic;
      sram_clk		: out std_logic;

      -- channel input
      channel		: in std_logic_vector(31 downto 0);

      -- clock, trigger
      main_clk		: in std_logic;				-- 20 MHz clock input
      eclk		: in std_logic;				-- external clock
      --eoclk		: in std_logic;				-- NC
      
      --ex2		: in std_logic;				-- NC
      etrg1		: in std_logic;				-- external trigger input
      --etrg2		: in std_logic;				-- NC

      -- LED diodes
      st		: out std_logic_vector(3 downto 1);

      -- other signals
      --clk_i		: in std_logic; 			-- NC
      clk_o		: out std_logic;			-- clock output
      trig_o		: out std_logic				-- trigger output
   );
end entity tiniLA;

architecture rtl of tiniLA is

-- component BUFG 
--     port (I: in  STD_LOGIC;  
--           O: out STD_LOGIC); 
-- end component;

   signal dd_out	: std_logic_vector(7 downto 0);		-- EPP data out
   signal d_oe		: std_logic;				-- EPP data output enable
   
   signal run		: std_logic;				-- start sampling
   signal stop		: std_logic;				-- stop sampling
   signal done		: std_logic;				-- sampling completed
   signal clr		: std_logic;				-- miniLA reset
   
   signal clk_int	: std_logic;				-- internal clock
   signal clk_rd	: std_logic;				-- SRAM clock (from EPP)
   signal sram_clk_int	: std_logic;				-- SRAM clock (from timebase)
   
   --signal tcnt_reg	: std_logic_vector(3 downto 0); 	-- trigger event cnt.
   --signal tlen_reg	: std_logic_vector(3 downto 0); 	-- trigger length cnt.
   signal tsel_reg	: std_logic_vector(3 downto 0); 	-- timebase selector
   signal clk_falling_edge: std_logic;				-- falling edge enable
  -- signal psize_reg	: std_logic_vector(3 downto 0); 	-- pre/postrigger length selector
   --signal postrig_on	: std_logic;
   signal extrig_en	: std_logic;				-- external trigger enable
   signal extrig_val	: std_logic;				-- external trigger active level

   signal trig_on	: std_logic;				-- trigger on
   signal trig		: std_logic;				-- trigger hit

   signal sig_sram_adr : std_logic_vector(16 downto 0);
   signal sram_write	: std_logic;				-- SRAM write enable
   signal channel_reg	: std_logic_vector(31 downto 0);	-- channel data (delayed)
   signal adr_cnt_en	: std_logic;				-- address counter enable
   --signal sample_cnt_en	: std_logic;				-- sample counter enable
   --signal sample_cnt_wm : std_logic;				-- sample wm hit
   signal sample_cnt_top: std_logic;				-- sample top hit
   signal sample_cnt_top_buffered: std_logic;				-- sample top hit
   signal int_trig_mask	: std_logic_vector(TRIGGER_WIDTH downto 0); 	-- TiniLA interni trigger
   signal int_trig_val 	: std_logic_vector(TRIGGER_WIDTH downto 0); 	-- TiniLA interni trigger
   
   type t_state		is (st_idle, st_trig, st_fill, st_read, st_done);
   signal state		: t_state;
   signal state_comb		: t_state;
   --signal clk_100	: std_logic;

   signal sig_done : std_logic := '0';  -- interni DONE
   signal sig_done_buffered : std_logic := '0';  -- interni DONE
   signal sig_adr_cnt_en : std_logic := '0';  -- interni povoleni citace adresy
   signal sig_filling : std_logic := '0';  -- uspesny trigger a plnime pamet daty

   attribute BUFG	: string;
   attribute BUFG of main_clk: signal is "CLK";
   attribute keep : string;
   attribute keep of sig_done_buffered: signal is "TRUE";
   attribute keep of sample_cnt_top: signal is "TRUE";
   
   signal reset_cntr : std_logic;
   
begin


   -- 
   
   epp_I: entity epp
      generic map
      (
         CPLD_FW_VER	=> CPLD_FW_VER,
         TRIGGER_WIDTH => TRIGGER_WIDTH
      )
      port map
      (
	 -- LPT interface
	 i		=> dd,				-- in
	 o		=> dd_out,			-- out
	 a_stb_l	=> dast_l,			-- in
	 d_stb_l	=> ddst_l,			-- in
	 rd_wr_l	=> dwr_l,			-- in
	 wait_o		=> dwait,			-- out
	 d_oe		=> d_oe,

	 -- input signals
	 clk		=> main_clk,			-- in
	 sd		=> sram_d,			-- in
	 done		=> sig_done_buffered,			-- in
	 trig		=> sig_filling,			-- in
	 sample_cnt_top	=> sample_cnt_top_buffered,		-- in
	 sram_adr	=> sig_sram_adr,			-- out

	 -- output signals
	 run		=> run,				-- out
	 stop		=> stop,			-- out
	 clr		=> clr,				-- out
	 clk_rd		=> clk_rd,			-- out
	 --tcnt_reg	=> tcnt_reg,			-- out
	 --tlen_reg	=> tlen_reg,			-- out
	 tsel_reg	=> tsel_reg,			-- out
	 clk_falling_edge => clk_falling_edge,		-- out
	 --psize_reg	=> psize_reg,			-- out
	 --postrig_on	=> postrig_on,			-- out
         int_trig_mask => int_trig_mask,
         int_trig_val  => int_trig_val,
	 extrig_en	=> extrig_en,			-- out
	 extrig_val	=> extrig_val,			-- out
   reset_cntr => reset_cntr
      );

   -- output data tristate buffer
   dd <= dd_out when d_oe = '1' else
         (others => 'Z');

   timebase_I: entity timebase
      port map
      (
         clr		=> clr,				-- in
	 clk_rd		=> clk_rd,			-- in
	 clk_in		=> main_clk,			-- in
	 clk_ext	=> eclk,			-- in
	 clk_sel	=> tsel_reg,			-- in
	 clk_falling_edge => clk_falling_edge,		-- in
	 done		=> done,			-- in
	 clk_out	=> clk_int,			-- out
	 clk_out_e	=> clk_o,			-- out
	 sram_clk	=> sram_clk_int			-- out
      );

   sram_clk <= sram_clk_int;

   trigcond_I: entity trigcond
      generic map
      (
         TRIGGER_WIDTH => TRIGGER_WIDTH
      )
      port map
      (
	 --clr		=> clr,				-- in
	 --clk		=> clk_int,			-- in
	 
	 extrig		=> etrg1,			-- in
	 extrig_en	=> extrig_en,			-- in
	 extrig_val	=> extrig_val,			-- in

         channel	=> channel_reg(TRIGGER_WIDTH downto 0),	-- in
	 --tlen_reg	=> tlen_reg,			-- in
	 --tcnt_reg	=> tcnt_reg,			-- in
         int_trig_mask => int_trig_mask,
         int_trig_val  => int_trig_val,
	 trig_on	=> trig_on,			-- in
	 trig		=> trig				-- out
      );

   trig_o <= trig;

   sram_I: entity sram
      port map
      (
	 clr		=> clr,				-- in
	 sram_clk	=> sram_clk_int,		-- in
	 
         ce_l		=> sram_ce_l,			-- out
	 oe_l		=> sram_oe_l,			-- out
	 we_l		=> sram_we_l,			-- out
	 adsc_l		=> sram_adsc_l,			-- out
	 sram_write	=> sram_write,			-- in
	 adr_cnt_en	=> adr_cnt_en,			-- in
	 adr_cnt	=> sig_sram_adr,			-- out
	 trig		=> trig,				-- in
	 --psize_reg	=> psize_reg,			-- in
	 --sample_cnt_en	=> sample_cnt_en,		-- in
	 --sample_cnt_wm=> sample_cnt_wm,	-- out
	 sample_cnt_top	=> sample_cnt_top,		-- out
   reset_cntr => reset_cntr
      );

  sram_adr <= sig_sram_adr;
   -- channel data register
   process(clk_int)
   begin
      if clk_int'event and clk_int = '1' then
	 channel_reg <= channel;
      end if;
   end process;
   
   -- SRAM data bus output enable
   sram_d <= channel_reg when sram_write = '1' else
             (others => 'Z');


   -- main state-machine - vaclavpe - dvouprocesova
   proc_fsmcomb:process(state, stop, trig, sample_cnt_top, run)
     variable var_state : t_state;
     variable var_ledky : std_logic_vector(2 downto 0);
     variable var_sig_adr_cnt_en : std_logic;
     variable var_trig_on : std_logic;
     variable var_sram_write : std_logic;
     variable var_sig_done : std_logic;
     variable var_sig_filling : std_logic;
   begin
     var_state := state;
     var_ledky := "000";
     var_trig_on := '0';
     var_sram_write := '0';
     var_sig_adr_cnt_en := '0';
     var_sig_done := '0';
     var_sig_filling := '0';
     
     case state is
       when st_idle => var_ledky := "111"; -- v IDLE sviti vsechny 3 LEDky
         var_sram_write := '1';
         if run = '1' then
           var_state := st_trig;
         end if;

       when st_trig => var_ledky := "110";
         var_trig_on := '1';
         var_sram_write := '1';
         if stop = '1' then			-- miniLA stopped
           var_state := st_read;
         elsif trig = '1' then			-- trigger detected
           var_state := st_fill;
           var_sig_adr_cnt_en := '1';
         end if;
         
       when st_fill => var_ledky := "100";
         var_sram_write := '1';
         var_sig_adr_cnt_en := '1';
         var_sig_filling := '1';
         if stop = '1' then			-- miniLA stopped
           var_state := st_read;
         elsif sample_cnt_top = '1' then -- all data stored
           var_state := st_read;
           var_sig_done := '1';
         end if;
         
         -- all data stored, switch for memory read
       when st_read => var_ledky := "011";
         var_sig_adr_cnt_en := '1';
         var_sig_done := '1';
         
         -- tohle pod tim bude asi zbytecne
--         if sample_cnt_top = '1' then -- all data read
--           var_state := st_done;
--         end if;
--	      
--         -- transfer data via LPT
--       when st_done => var_ledky := "000";
--         var_sig_done := '1';

       when others => null;
     end case;
     state_comb <= var_state;
     st <= var_ledky;
     trig_on <= var_trig_on;
     sram_write <= var_sram_write;
     sig_adr_cnt_en <= var_sig_adr_cnt_en;
     sig_done <= var_sig_done;
     sig_filling <= var_sig_filling;
   end process;
     
   proc_seq:process(clr, clk_int)
   begin
      if clr = '1' then
         state <= st_idle;
      elsif clk_int'event and clk_int = '1' then
         state <= state_comb;
      end if;
   end process;
   
   done <= sig_done or sample_cnt_top;
   adr_cnt_en <= sig_adr_cnt_en; -- or trig;
   sample_cnt_top_buffered <= sample_cnt_top;
   sig_done_buffered <= sig_done;
end rtl;

			   

