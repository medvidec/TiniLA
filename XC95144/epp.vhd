--==============================================================================
--==                                                                          ==
--==		EPP interface                                                 ==
--==                                                                          ==
--==============================================================================


library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;


entity epp is
   generic
   (
      CPLD_FW_VER	: std_logic_vector(2 downto 0);		-- CPLD firmware version
      TRIGGER_WIDTH     : integer := 6        -- sirka triggeru je tady kvuli testovani
   );
   port
   (
      -- LPT interface
      i			: in std_logic_vector(7 downto 0);	-- EPP input data bus
      o			: out std_logic_vector(7 downto 0);	-- EPP output data bus
      a_stb_l		: in std_logic;				-- EPP - address strobe
      d_stb_l		: in std_logic;				-- EPP - data strobe
      rd_wr_l		: in std_logic;				-- EPP - read/write
      wait_o		: out std_logic;			-- EPP - wait states
      d_oe		: out std_logic;			-- EPP - data output enable

      -- input signals
      clk		: in std_logic;				-- clock input
      sd		: in std_logic_vector(31 downto 0);	-- SRAM data bus
      done		: in std_logic;				-- sampling completed
      trig		: in std_logic;				-- trigger detected
      sample_cnt_top	: in std_logic;				-- all data stored(read)
      sram_adr		: in std_logic_vector(16 downto 0);

      -- output signals
      run		: out std_logic;			-- start sampling
      stop		: out std_logic;			-- stop sampling
      clr		: out std_logic;			-- miniLA reset
      clk_rd		: out std_logic;			-- memory read clock
      --tcnt_reg		: out std_logic_vector(3 downto 0); 	-- trigger event counter
      --tlen_reg		: out std_logic_vector(3 downto 0); 	-- trigger length counter
      tsel_reg		: out std_logic_vector(3 downto 0); 	-- timebase selector
      clk_falling_edge	: out std_logic;			-- falling edge enable
      --psize_reg		: out std_logic_vector(3 downto 0); 	-- pre/posttrigger length
      --postrig_on	: out std_logic;
      int_trig_val           : out std_logic_vector(TRIGGER_WIDTH downto 0);  -- interni trigger hodnota
      int_trig_mask          : out std_logic_vector(TRIGGER_WIDTH downto 0);  -- interni trigger maska
      extrig_en 	: out std_logic;			-- external trigger enable
      extrig_val	: out std_logic;        -- external trigger active level
      reset_cntr	: out std_logic			-- reset of sram_adr counter to read the values from beginning
    );
end entity epp;

architecture rtl of epp is

   signal adr		: std_logic_vector(2 downto 0);		-- EPP address
   signal run_int	: std_logic;
   signal stop_int	: std_logic;
   signal clr_int	: std_logic;
   signal clk_rd_int	: std_logic;
   signal auto_inc	: std_logic;
   signal bytesel	: std_logic_vector(1 downto 0);		-- output data bus byte selector
   signal a_stb_l_i	: std_logic;
   signal d_stb_l_i	: std_logic;
   signal rd_wr_l_i	: std_logic;
   --signal a_wr_i	: std_logic;				-- address write strobe
   signal d_wr_i	: std_logic;				-- data write strobe
   signal d_rd_i	: std_logic;				-- data read strobe
   signal d_rd_i_d	: std_logic;

begin
   
   -- registering for miniLA time domain
   process(clk)
   begin
      if clk'event and clk='0' then
         a_stb_l_i <= a_stb_l;
	 d_stb_l_i <= d_stb_l;
	 rd_wr_l_i <= rd_wr_l;
         d_rd_i_d <= d_rd_i;
      end if;
   end process;
    
   -- WAIT
   process(clk)
   begin
      if clk'event and clk='1' then
         if (a_stb_l_i = '0') or (d_stb_l_i = '0') then
	    wait_o <= '1';
	 else
   	    wait_o <= '0';
	 end if;
      end if;
   end process;
   
   -- data write strobe
   d_wr_i <= '1' when (d_stb_l_i = '0') and (rd_wr_l_i = '0') else
   	     '0';
   
   -- data read strobe
   d_rd_i <= '1' when (d_stb_l_i = '0') and (rd_wr_l_i = '1') else
             '0';
   
   -- EPP address register
   process(clk)
   begin
      if clk'event and clk='1' then
         if a_stb_l_i  = '0' and rd_wr_l_i = '0' then
            adr <= i(2 downto 0);
	 end if;
      end if;
   end process;

   process(clk)
   begin
      if clk'event and clk='1' then
         if (adr = "000") then
	    if d_wr_i = '1' then				-- control register write
	       bytesel <= i(1 downto 0);
               -- vaclavpe - na konci samplovani pred zacatkem cteni musim
               -- nacist adresu 0x00000
               if i(7) = '0' and i(4) = '1' then
                 clk_rd_int <= '1';
               end if;
	    elsif (d_rd_i = '0') and (d_rd_i_d = '1') then	-- end of read of data register
	       bytesel <= bytesel + 1;                          -- autoincrementation of bytesel
	       if (bytesel = "11") and (auto_inc = '1') then	-- memory counter clock generation
	          clk_rd_int <= '1';
	       else
	          clk_rd_int <= '0';
	       end if;
	    else
	       clk_rd_int <= '0';
	       bytesel <= bytesel;
	    end if;
	 end if;
      end if;
   end process;

   -- tsel_reg (b)
   process(clk)
   begin
      if clk'event and clk='0' then
         if (d_wr_i = '1') then
           case adr is
             when "000" => run_int <= i(7);
                           clr_int <= i(6);
                           reset_cntr <= i(5);
                           auto_inc <= i(4);
                           stop_int <= i(3);
                           clk_falling_edge <= i(2);
             when "001" => int_trig_val <= i(TRIGGER_WIDTH downto 0);
             when "010" => int_trig_mask <= i(TRIGGER_WIDTH downto 0);
             when "011" => extrig_val <= i(7);
                           extrig_en <= i(6);
                           tsel_reg(3 downto 0)  <= i(3 downto 0);
             when others => null;
           end case;
	 end if;
      end if;
   end process;

   -- output multiplexer
   -- vaclavpe -  pouze 2 cteci registry, "0" je datovy, a vse ostatni je status
   process(d_rd_i, adr, bytesel, sd, done, run_int, clr_int, trig, sample_cnt_top, sram_adr)
   begin
      if d_rd_i = '1' then
	 case adr is
	    when "000" =>
	       d_oe <= '1';
	       case bytesel is
	          when "00" =>
		         o <= sd(7 downto 0);
	      	  when "01" =>
		         o <= sd(15 downto 8);
	      	  when "10" => 
	      	     o <= sd(23 downto 16);
	      	  when others =>
		           o <= sd(31 downto 24);
	       end case;
       when "010" =>
	       d_oe <= '1';
          o <= sram_adr(8 downto 1);
       when "011" =>
	       d_oe <= '1';
          o <= sram_adr(16 downto 9);
	    when others => -- vaclavpe - uprava pro TiniLA; HW verzi nepotrebujeme, misto ni veci z druheho statusu
	       d_oe <= '1';
	       o <= done & run_int & clr_int & trig & sram_adr(0) & CPLD_FW_VER;
	 end case;
      else
         d_oe <= '0';
         o <= (others => '1');
      end if;
   end process;
   
   -- assign internal signals to outputs
   run <= run_int;
   clr <= clr_int;
   clk_rd <= clk_rd_int;
   stop <= stop_int;

end rtl;
