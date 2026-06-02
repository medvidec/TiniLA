-- =====================================================
--
--   Trigger
--
-- =====================================================

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity trigcond is
  generic
   (
      TRIGGER_WIDTH     : integer := 6        -- sirka triggeru je tady kvuli testovani
   );

   port
   (
      --clr		: in std_logic;				-- miniLA reset
      --clk		: in std_logic;				-- clock input

      extrig		: in std_logic;				-- external trigger input
      extrig_en		: in std_logic;				-- external trigger enable
      extrig_val	: in std_logic;				-- external trigger value

      channel		: in std_logic_vector(TRIGGER_WIDTH downto 0);	-- channel input
      --tlen_reg		: in std_logic_vector(3 downto 0);	-- trigger length
      --tcnt_reg		: in std_logic_vector(3 downto 0);	-- trigger event cnt.
      int_trig_val           : in std_logic_vector(TRIGGER_WIDTH downto 0);  -- interni trigger hodnota
      int_trig_mask          : in std_logic_vector(TRIGGER_WIDTH downto 0);  -- interni trigger maska
--      int_trigger           : in std_logic_vector(7 downto 0);  -- interni trigger
      trig_on		: in std_logic;				-- trigger enable
      trig		: out std_logic				-- trigger output
   );
end entity trigcond;

architecture rtl of trigcond is

   signal trig_hit_int	: std_logic;
   signal trig_hit	: std_logic;

    function wide_nor(sigin : std_logic_vector) return std_logic is
       variable or_i	: std_logic;
    begin
       or_i := '0';
       for i in sigin'low to sigin'high loop
          or_i := or_i or sigin(i);
       end loop;
       return(not(or_i));
    end function wide_nor;

begin

		 
   trig_hit_int <= wide_nor( int_trig_mask and (channel xor int_trig_val));
				 
   trig_hit <= '1' when    (extrig_en = '0' and  trig_hit_int = '1')		-- internal trigger
                        or (extrig_en = '1' and (extrig = extrig_val)) else	-- external trigger
	       '0';

   trig <= trig_hit when trig_on = '1' else '0';

--    -- trigger-hit length counter
--    process(clr, clk)
--    begin
--       if clr = '1' then
--          tlen_cnt <= (others => '0');
--       elsif clk'event and clk = '1' then
-- 	 if (trig_on = '0') or (trig_hit = '0') then 
--             tlen_cnt <= conv_std_logic_vector(0, 4);
-- 	 elsif tlen_top = '0' then
--             tlen_cnt <= tlen_cnt + conv_std_logic_vector(1, 4);
-- 	 else
-- 	    tlen_cnt <= tlen_cnt;
-- 	 end if;
--       end if;
--    end process;

--    tlen_top <= '1' when tlen_cnt = tlen_reg else
--                '0';

--    -- t_len_top delay
--    process(clr, clk)
--    begin
--       if clr = '1' then
-- 	 tlen_top_d <= '0';
--       elsif clk'event and clk = '1' then
-- 	 tlen_top_d <= tlen_top;
--       end if;
--    end process;

--    -- trigger-hit counter
--    process(clr, clk)
--    begin
--       if clr = '1' then
--          trig_cnt <= (others => '0');
--       elsif clk'event and clk = '1' then
--          if (tlen_top = '1') and (tlen_top_d = '0') then
--             trig_cnt <= trig_cnt + conv_std_logic_vector(1, 4);
-- 	 else
-- 	    trig_cnt <= trig_cnt;
-- 	 end if;
--       end if;
--    end process;

end rtl;
 
