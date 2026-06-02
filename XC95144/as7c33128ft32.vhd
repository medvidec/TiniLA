------------------------------------------------------------------------------------
--
--    File Name: AS7C33128FT32.vhd 
--      Version:  1.0
--         Date:  7 May 2004
--        Model:  BUS Functional
--    Simulator:  Cadence leapfrog
--
--
--       Author:  kvijay
--        Email:  kvijay@alsc.com
--        Phone:  91-80-22129841
--      Company:  Alliance Semiconductor pvt ltd.
--  Part Number:  AS7C33128FT32 (128K x 32)
--
--  Description:  Alliance 4.5Mb Syncburst SRAM (Flowthrough )
--
------------------------------------------------------------------------------------
--
-- Changes:
--
-- 01/24/05 JS: memory array changed from std_logic_vector to integer
--		in order to minimize memory requirements during simulation
--
------------------------------------------------------------------------------------


LIBRARY ieee;
    USE ieee.std_logic_1164.all;
    USE ieee.std_logic_unsigned.all;
    USE ieee.std_logic_arith.all;

ENTITY AS7C33128FT32 IS
    GENERIC (
        
	-- Setup Times
        tAS       : TIME    :=  2.0 ns;
        tADSCS    : TIME    :=  2.0 ns;
	tADSPS    : TIME    :=  2.0 ns;
        tADVS     : TIME    :=  2.0 ns;
        tWS       : TIME    :=  2.0 ns;
        tDS       : TIME    :=  2.0 ns;
        tCSS      : TIME    :=  2.0 ns;
        -- Hold Times
        tAH       : TIME    :=  0.5 ns;
        tADSCH    : TIME    :=  0.5 ns;
	tADSPH    : TIME    :=  0.5 ns;
        tADVH     : TIME    :=  0.5 ns;
        tWH       : TIME    :=  0.5 ns;
        tDH       : TIME    :=  0.5 ns;
        tCSH      : TIME    :=  0.5 ns;
	-- Clock
        tCYC      : TIME    :=  8.5 ns;  -- Timing are for 8.5nsec
        tCH       : TIME    :=  3.0 ns;
        tCL       : TIME    :=  3.0 ns;
        -- Output Times
        tHZC      : TIME    :=  3.5 ns;
        
        -- Bus Width and Data Bus
        addr_bits : INTEGER := 17;
        data_bits : INTEGER := 32
    );
    PORT (
        Dq        : INOUT STD_LOGIC_VECTOR (data_bits - 1 DOWNTO 0) := (OTHERS => 'Z');
        Addr      : IN    STD_LOGIC_VECTOR (addr_bits - 1 DOWNTO 0);
        Lbo_n     : IN    STD_LOGIC;
        Adv_n     : IN    STD_LOGIC;
        Clk       : IN    STD_LOGIC;
        Adsc_n    : IN    STD_LOGIC;
        Adsp_n    : IN    STD_LOGIC;
        Bwa_n     : IN    STD_LOGIC;
        Bwb_n     : IN    STD_LOGIC;
        Bwc_n     : IN    STD_LOGIC;
        Bwd_n     : IN    STD_LOGIC;
        Bwe_n     : IN    STD_LOGIC;
        Gw_n      : IN    STD_LOGIC;
        Ce0_n     : IN    STD_LOGIC;
        Ce1       : IN    STD_LOGIC;
        Ce2_n     : IN    STD_LOGIC;
        Oe_n      : IN    STD_LOGIC;
        Zz        : IN    STD_LOGIC
    );
END AS7C33128FT32;

ARCHITECTURE behave OF AS7C33128FT32 IS
   --TYPE   memory IS ARRAY (2 ** addr_bits - 1 DOWNTO 0) OF STD_LOGIC_VECTOR (data_bits / 4 - 1 DOWNTO 0);
   TYPE   memory IS ARRAY (2 ** addr_bits - 1 DOWNTO 0) OF integer;

   SIGNAL doe : STD_LOGIC;
   SIGNAL dout : STD_LOGIC_VECTOR (data_bits - 1 DOWNTO 0) := (OTHERS => 'Z');
   SIGNAL bwan, bwbn, bwcn, bwdn, ce, clr : STD_LOGIC;

   
   function conv_integer_X01(sig_in: std_logic_vector) return integer is
      variable valid	: boolean := true;
      variable sig_X01	: std_logic;
      variable retval	: integer;
   begin
      for i in sig_in'range loop
         sig_X01 := to_X01(sig_in(i));
         if (sig_X01 /= '0') and (sig_X01 /= '1') then
	    valid := false;
	 end if;
      end loop;
      if valid then
         retval := conv_integer(sig_in);
      else
         retval := -1;
      end if;
      return(retval);
   end function;

   function conv_std_logic_vector_X01(val: integer; size: natural) return std_logic_vector is
      variable retsig	: std_logic_vector(size-1 downto 0);
   begin
      if (val /= -1) then
         retsig := conv_std_logic_vector(val, size);
      else
         retsig := (others => 'X');
      end if;
      return(retsig);
   end function;


BEGIN
    bwan <= ((Bwa_n OR Bwe_n) AND Gw_n) OR (NOT(Ce0_n) AND NOT(Adsp_n));
    bwbn <= ((Bwb_n OR Bwe_n) AND Gw_n) OR (NOT(Ce0_n) AND NOT(Adsp_n));
    bwcn <= ((Bwc_n OR Bwe_n) AND Gw_n) OR (NOT(Ce0_n) AND NOT(Adsp_n));
    bwdn <= ((Bwd_n OR Bwe_n) AND Gw_n) OR (NOT(Ce0_n) AND NOT(Adsp_n));
    ce   <= NOT(Ce0_n) AND Ce1 AND NOT(Ce2_n);
    clr  <= NOT(Adsc_n) OR (NOT(Adsp_n) AND NOT(Ce0_n));

    main : PROCESS
        -- Memory Array
        VARIABLE bank0, bank1, bank2, bank3 : memory;
    
        -- Address Registers
        VARIABLE addr_reg_in    : STD_LOGIC_VECTOR (addr_bits - 1 DOWNTO 0) := (OTHERS => '0');
	VARIABLE addr_reg_out   : STD_LOGIC_VECTOR (addr_bits - 1 DOWNTO 0) := (OTHERS => '0');
        VARIABLE addr_reg_read  : STD_LOGIC_VECTOR (addr_bits - 1 DOWNTO 0) := (OTHERS => '0');
        VARIABLE addr_reg_write : STD_LOGIC_VECTOR (addr_bits - 1 DOWNTO 0) := (OTHERS => '0');

        -- Burst Counter
        VARIABLE bcount : STD_LOGIC_VECTOR (1 DOWNTO 0) := "00";
        VARIABLE baddr0 : STD_LOGIC;
        VARIABLE baddr1 : STD_LOGIC;

        -- Other Registers
        VARIABLE din      : STD_LOGIC_VECTOR (data_bits - 1 DOWNTO 0) := (OTHERS => 'Z');
        VARIABLE ce_reg   : STD_LOGIC;
        VARIABLE pipe_reg : STD_LOGIC;
        VARIABLE bwa_reg  : STD_LOGIC;
        VARIABLE bwb_reg  : STD_LOGIC;
        VARIABLE bwc_reg  : STD_LOGIC;
        VARIABLE bwd_reg  : STD_LOGIC;
	
	VARIABLE Ft_n     : STD_LOGIC  := '0';
    BEGIN
        WAIT ON Clk;
        IF Clk'EVENT AND Clk = '1' AND Zz = '0' THEN
            -- Address Register
            IF clr = '1' THEN
                addr_reg_in := Addr;
            END IF;
    
            -- Binary Counter and Logic
            IF Lbo_n = '1' AND clr = '1' THEN
                bcount := "00";
            ELSIF Lbo_n = '0' AND clr = '1' THEN
                bcount := Addr (1 DOWNTO 0);
            ELSIF Adv_n = '0' AND clr = '0' THEN
                bcount(1) := bcount(0) XOR bcount(1);
                bcount(0) := NOT(bcount(0));
            END IF;
    
            -- Burst Address Decode
            IF Lbo_n = '1' THEN
                baddr0 := bcount(0) XOR addr_reg_in(0);
                baddr1 := bcount(1) XOR addr_reg_in(1);
            ELSE
                baddr0 := bcount(0);
                baddr1 := bcount(1);
            END IF;

            -- Pipeline Read Address
            addr_reg_read := addr_reg_out;

            --  Address
            addr_reg_out (addr_bits - 1 DOWNTO 2) := addr_reg_in (addr_bits - 1 DOWNTO 2);
            addr_reg_out (1) := baddr1;
            addr_reg_out (0) := baddr0;
    
            --  Write Address
	    addr_reg_write   := addr_reg_out;
    
            -- Flow-Through Read Address
	    IF Ft_n = '0' THEN
	       addr_reg_read := addr_reg_out;
	    END IF;   
    
    
            -- Byte Write Register
            bwa_reg := NOT(bwan);
            bwb_reg := NOT(bwbn);
            bwc_reg := NOT(bwcn);
            bwd_reg := NOT(bwdn);
    
            -- Pipelined Enable
            pipe_reg := ce_reg;

            -- Enable Register
            IF clr = '1' THEN
                ce_reg := ce;
            END IF;
    
            -- Input Register
            IF (ce_reg = '1' AND (bwa_reg = '1' OR bwb_reg = '1' OR bwc_reg = '1' OR bwd_reg = '1')) THEN
                din := Dq;
            END IF;
            
            -- Byte Write Driver
            IF ce_reg = '1' AND bwa_reg = '1' THEN
                bank0 (CONV_INTEGER(addr_reg_write)) := conv_integer_X01(din ( 7 DOWNTO  0));
            END IF;
            IF ce_reg = '1' AND bwb_reg = '1' THEN
                bank1 (CONV_INTEGER(addr_reg_write)) := conv_integer_X01(din (15 DOWNTO  8));
            END IF;
            IF ce_reg = '1' AND bwc_reg = '1' THEN
                bank2 (CONV_INTEGER(addr_reg_write)) := conv_integer_X01(din (23 DOWNTO 16));
            END IF;
            IF ce_reg = '1' AND bwd_reg = '1' THEN
                bank3 (CONV_INTEGER(addr_reg_write)) := conv_integer_X01(din (31 DOWNTO 24));
            END IF;

            -- Output Register
            IF (NOT(bwa_reg = '1' OR bwb_reg = '1' OR bwc_reg = '1' OR bwd_reg = '1')) THEN
                dout ( 7 DOWNTO  0) <= conv_std_logic_vector_X01(bank0 (CONV_INTEGER(addr_reg_read)), 8);
                dout (15 DOWNTO  8) <= conv_std_logic_vector_X01(bank1 (CONV_INTEGER(addr_reg_read)), 8);
                dout (23 DOWNTO 16) <= conv_std_logic_vector_X01(bank2 (CONV_INTEGER(addr_reg_read)), 8);
                dout (31 DOWNTO 24) <= conv_std_logic_vector_X01(bank3 (CONV_INTEGER(addr_reg_read)), 8);
            END IF;

            -- Data Out Enable
	    IF Ft_n = '0' THEN
                doe <= ce_reg AND (NOT(bwa_reg OR bwb_reg OR bwc_reg OR bwd_reg));
	    ELSE
	        doe <= ce_reg AND pipe_reg AND (NOT(bwa_reg OR bwb_reg OR bwc_reg OR bwd_reg)); 
	    END IF;	
	    
        END IF;
    END PROCESS main;

    -- Output buffer
    WITH (NOT(Oe_n) AND NOT(Zz) AND doe) SELECT
        Dq <= TRANSPORT dout AFTER tHZC WHEN '1',
             (OTHERS => 'Z') AFTER tHZC WHEN '0',
             (OTHERS => 'Z') AFTER tHZC WHEN OTHERS;

    -- Checking for setup time violation
    Setup_check : PROCESS
    BEGIN
        WAIT ON Clk;
        IF Clk'EVENT AND Clk = '1' THEN
            ASSERT(Addr'LAST_EVENT >= tAS)
                REPORT "Addr Setup time violation -- tAS"
                SEVERITY WARNING;
            ASSERT(Adsc_n'LAST_EVENT >= tADSCS)
                REPORT "Adsc_n Setup time violation -- tADSCS"
                SEVERITY WARNING;
            ASSERT(Adsp_n'LAST_EVENT >= tADSPS)
                REPORT "Adsp_n Setup time violation -- tADSPS"
                SEVERITY WARNING;
            ASSERT(Adv_n'LAST_EVENT >= tADVS)
                REPORT "Adv_n Setup time violation -- tADVS"
                SEVERITY WARNING;
            ASSERT(Bwa_n'LAST_EVENT >= tWS)
                REPORT "Bwa_n Setup time violation -- tWS"
                SEVERITY WARNING;
            ASSERT(Bwb_n'LAST_EVENT >= tWS)
                REPORT "Bwb_n Setup time violation -- tWS"
                SEVERITY WARNING;
            ASSERT(Bwc_n'LAST_EVENT >= tWS)
                REPORT "Bwc_n Setup time violation -- tWS"
                SEVERITY WARNING;
            ASSERT(Bwd_n'LAST_EVENT >= tWS)
                REPORT "Bwd_n Setup time violation -- tWS"
                SEVERITY WARNING;
            ASSERT(Bwe_n'LAST_EVENT >= tWS)
                REPORT "Bwe_n Setup time violation -- tWS"
                SEVERITY WARNING;
            ASSERT(Gw_n'LAST_EVENT >= tWS)
                REPORT "Gw_n Setup time violation -- tWS"
                SEVERITY WARNING;
            ASSERT(Ce0_n'LAST_EVENT >= tCSS)
                REPORT "Ce0_n Setup time violation -- tCSS"
                SEVERITY WARNING;
            ASSERT(Ce2_n'LAST_EVENT >= tCSS)
                REPORT "Ce2_n Setup time violation -- tCSS"
                SEVERITY WARNING;
            ASSERT(Ce1'LAST_EVENT >= tCSS)
                REPORT "Ce1 Setup time violation -- tCSS"
                SEVERITY WARNING;
        END IF;
    END PROCESS;

    -- Checking for hold time violation
    Hold_check : PROCESS
    BEGIN
        WAIT ON Clk'DELAYED(tAH), Clk'DELAYED(tADSCH), Clk'DELAYED(tADVH), Clk'DELAYED(tWH), Clk'DELAYED(tCSH);
        IF Clk'DELAYED(tAH)'EVENT AND Clk'DELAYED(tAH) = '1' THEN
            ASSERT(Addr'LAST_EVENT > tAH)
                REPORT "Addr Hold time violation -- tAH"
                SEVERITY WARNING;
        END IF;
        IF Clk'DELAYED(tADSCH)'EVENT AND Clk'DELAYED(tADSCH) = '1' THEN
            ASSERT(Adsc_n'LAST_EVENT > tADSCH)
                REPORT "Adsc_n Hold time violation -- tADSCH"
                SEVERITY WARNING;
            ASSERT(Adsp_n'LAST_EVENT > tADSPH)
                REPORT "Adsp_n Hold time violation -- tADSPH"
                SEVERITY WARNING;
        END IF;
        IF Clk'DELAYED(tADVH)'EVENT AND Clk'DELAYED(tADVH) = '1' THEN
            ASSERT(Adv_n'LAST_EVENT > tADVH)
                REPORT "Adv_n Hold time violation -- tADVH"
                SEVERITY WARNING;
        END IF;
        IF Clk'DELAYED(tWH)'EVENT AND Clk'DELAYED(tWH) = '1' THEN
            ASSERT(Bwa_n'LAST_EVENT > tWH)
                REPORT "Bwa_n Hold time violation -- tWH"
                SEVERITY WARNING;
            ASSERT(Bwb_n'LAST_EVENT > tWH)
                REPORT "Bwb_n Hold time violation -- tWH"
                SEVERITY WARNING;
            ASSERT(Bwc_n'LAST_EVENT > tWH)
                REPORT "Bwc_n Hold time violation -- tWH"
                SEVERITY WARNING;
            ASSERT(Bwd_n'LAST_EVENT > tWH)
                REPORT "Bwd_n Hold time violation -- tWH"
                SEVERITY WARNING;
            ASSERT(Bwe_n'LAST_EVENT > tWH)
                REPORT "Bwe_n Hold time violation -- tWH"
                SEVERITY WARNING;
            ASSERT(Gw_n'LAST_EVENT > tWH)
                REPORT "Gw_n Hold time violation -- tWH"
                SEVERITY WARNING;
        END IF;
        IF Clk'DELAYED(tCSH)'EVENT AND Clk'DELAYED(tCSH) = '1' THEN
            ASSERT(Ce0_n'LAST_EVENT > tCSH)
                REPORT "Ce0_n Hold time violation -- tCSH"
                SEVERITY WARNING;
            ASSERT(Ce2_n'LAST_EVENT > tCSH)
                REPORT "Ce2_n Hold time violation -- tCSH"
                SEVERITY WARNING;
            ASSERT(Ce1'LAST_EVENT > tCSH)
                REPORT "Ce1 Hold time violation -- tCSH"
                SEVERITY WARNING;
        END IF;
    END PROCESS;

END behave;
