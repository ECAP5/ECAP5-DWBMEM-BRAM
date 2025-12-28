/*           __        _
 *  ________/ /  ___ _(_)__  ___
 * / __/ __/ _ \/ _ `/ / _ \/ -_)
 * \__/\__/_//_/\_,_/_/_//_/\__/
 * 
 * Copyright (C) Clément Chaine
 * This file is part of ECAP5-DWBMEM-BRAM <https://github.com/ecap5/ECAP5-DWBMEM-BRAM>
 *
 * ECAP5-DWBMEM-BRAM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ECAP5-DWBMEM-BRAM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ECAP5-DWBMEM-BRAM.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include <svdpi.h>

#include "Vtb_ecap5_dwbmem_bram.h"
#include "testbench.h"

enum CondId {
  COND_read,
  COND_write,
  COND_preload,
  __CondIdEnd
};

enum StateId {
  T_R8 = 1,
  T_R16,
  T_R32,
  T_W8,
  T_W16,
  T_W32,
  T_PRELOAD
};

class TB_Ecap5_dwbmem_bram : public Testbench<Vtb_ecap5_dwbmem_bram> {
public:
  void reset() {
    this->_nop();

    this->core->rst_i = 1;
    for(int i = 0; i < 5; i++) {
      this->tick();
    }
    this->core->rst_i = 0;

    Testbench<Vtb_ecap5_dwbmem_bram>::reset();
  }
  
  void _nop() {
    this->core->wb_adr_i = 0;
    this->core->wb_dat_i = 0;
    this->core->wb_we_i = 0;
    this->core->wb_sel_i = 0;
    this->core->wb_stb_i = 0;
    this->core->wb_cyc_i = 0;
  }

  void read(uint32_t addr, uint8_t sel) {
    this->core->wb_adr_i = addr;
    this->core->wb_dat_i = 0;
    this->core->wb_we_i = 0;
    this->core->wb_sel_i = sel & 0xF;
    this->core->wb_stb_i = 1;
    this->core->wb_cyc_i = 1;
  }

  void write(uint32_t addr, uint32_t data, uint8_t sel) {
    this->core->wb_adr_i = addr;
    this->core->wb_dat_i = data;
    this->core->wb_we_i = 1;
    this->core->wb_sel_i = sel & 0xF;
    this->core->wb_stb_i = 1;
    this->core->wb_cyc_i = 1;
  }
};

void tb_ecap5_dwbmem_bram_r_32(TB_Ecap5_dwbmem_bram * tb) {
  Vtb_ecap5_dwbmem_bram * core = tb->core;
  core->testcase = T_R32;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  tb->write(0x40, 0x11223344, 0xF);

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->core->wb_stb_i = 0;

  //=================================
  //      Tick (2)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->core->wb_cyc_i = 0;

  //=================================
  //      Tick (3)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->read(0x40, 0xF);

  //=================================
  //      Tick (4)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_read, (core->wb_dat_o == 0x11223344));

  //`````````````````````````````````
  //      Set inputs
  
  tb->core->wb_stb_i = 0;

  //=================================
  //      Tick (5)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->core->wb_cyc_i = 0;

  //=================================
  //      Tick (6)
  
  tb->tick();

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbmem_bram.r_32.01",
      tb->conditions[COND_read],
      "Failed to implement the read function", tb->err_cycles[COND_read]);
}

void tb_ecap5_dwbmem_bram_r_16(TB_Ecap5_dwbmem_bram * tb) {
  Vtb_ecap5_dwbmem_bram * core = tb->core;
  core->testcase = T_R16;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  uint32_t data = 0x11223344;
  tb->write(0x40, data, 0xF);

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->core->wb_stb_i = 0;

  //=================================
  //      Tick (2)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->core->wb_cyc_i = 0;

  //=================================
  //      Tick (3)
  
  tb->tick();


  //=================================
  //      Tick (4-15)
  
  for(int i = 0; i < 2; i++) {
    tb->read(0x40 + (2*i), 0x3);

    tb->tick();

    uint32_t expected_data = (data >> (16*i)) & 0xFFFF;
    tb->check(COND_read, (core->wb_dat_o == expected_data));

    tb->core->wb_stb_i = 0;

    tb->tick();

    tb->core->wb_cyc_i = 0;

    tb->tick();
  }

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbmem_bram.r_16.01",
      tb->conditions[COND_read],
      "Failed to implement the read function", tb->err_cycles[COND_read]);
}

void tb_ecap5_dwbmem_bram_r_8(TB_Ecap5_dwbmem_bram * tb) {
  Vtb_ecap5_dwbmem_bram * core = tb->core;
  core->testcase = T_R8;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  uint32_t data = 0x11223344;
  tb->write(0x40, data, 0xF);

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->core->wb_stb_i = 0;

  //=================================
  //      Tick (2)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->core->wb_cyc_i = 0;

  //=================================
  //      Tick (3)
  
  tb->tick();

  //=================================
  //      Tick (4-15)
  
  for(int i = 0; i < 4; i++) {
    tb->read(0x40 + i, 0x1);

    tb->tick();

    uint32_t expected_data = (data >> (8*i)) & 0xFF;
    tb->check(COND_read, (core->wb_dat_o == expected_data));

    tb->core->wb_stb_i = 0;

    tb->tick();

    tb->core->wb_cyc_i = 0;

    tb->tick();
  }

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbmem_bram.r_8.01",
      tb->conditions[COND_read],
      "Failed to implement the read function", tb->err_cycles[COND_read]);
}

void tb_ecap5_dwbmem_bram_w_32(TB_Ecap5_dwbmem_bram * tb) {
  Vtb_ecap5_dwbmem_bram * core = tb->core;
  core->testcase = T_W32;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  uint32_t data = 0x11223344;
  tb->write(0x40, data, 0xF);

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->core->wb_stb_i = 0;

  //=================================
  //      Tick (2)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->core->wb_cyc_i = 0;

  //=================================
  //      Tick (3)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->write(0x40, 0xAABBCCDD, 0xF);

  //=================================
  //      Tick (5)
  
  tb->tick();


  //`````````````````````````````````
  //      Set inputs
  
  tb->core->wb_stb_i = 0;

  //=================================
  //      Tick (6)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->core->wb_cyc_i = 0;

  //=================================
  //      Tick (7)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->read(0x40, 0xF);

  //=================================
  //      Tick (8)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_write, (core->wb_dat_o == 0xAABBCCDD));

  //`````````````````````````````````
  //      Set inputs
  
  tb->core->wb_stb_i = 0;

  //=================================
  //      Tick (9)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->core->wb_cyc_i = 0;

  //=================================
  //      Tick (10)
  
  tb->tick();

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbmem_bram.w_32.01",
      tb->conditions[COND_write],
      "Failed to implement the write function", tb->err_cycles[COND_write]);
}

void tb_ecap5_dwbmem_bram_w_16(TB_Ecap5_dwbmem_bram * tb) {
  Vtb_ecap5_dwbmem_bram * core = tb->core;
  core->testcase = T_W16;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  uint32_t data = 0x11223344;
  tb->write(0x40, data, 0xF);
  
  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->core->wb_stb_i = 0;
  
  //=================================
  //      Tick (2)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->core->wb_cyc_i = 0;
  
  //=================================
  //      Tick (3)
  
  tb->tick();

  uint32_t expected_data = data;
  for(int i = 0; i < 2; i++) {
    uint32_t write_data = 0xAABBCCDD;
    tb->write(0x40 + 2*i, write_data, 0x3);
    tb->tick();
    tb->core->wb_stb_i = 0;
    tb->tick();
    tb->core->wb_cyc_i = 0;
    tb->tick();

    tb->read(0x40, 0xF);
    tb->tick();
  
    expected_data = expected_data & ~(0xFFFF << (16*i)) | ((write_data & 0xFFFF) << (16*i));
    tb->check(COND_write, (core->wb_dat_o == expected_data));

    tb->core->wb_stb_i = 0;
    tb->tick();
    tb->core->wb_cyc_i = 0;
    tb->tick();
  }

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbmem_bram.w_16.01",
      tb->conditions[COND_write],
      "Failed to implement the write function", tb->err_cycles[COND_write]);
}

void tb_ecap5_dwbmem_bram_w_8(TB_Ecap5_dwbmem_bram * tb) {
  Vtb_ecap5_dwbmem_bram * core = tb->core;
  core->testcase = T_W8;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  uint32_t data = 0x11223344;
  tb->write(0x40, data, 0xF);
  
  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->core->wb_stb_i = 0;
  
  //=================================
  //      Tick (2)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->core->wb_cyc_i = 0;
  
  //=================================
  //      Tick (3)
  
  tb->tick();

  uint32_t expected_data = data;
  for(int i = 0; i < 4; i++) {
    uint32_t write_data = 0xAABBCCDD;
    tb->write(0x40 + i, write_data, 0x1);
    tb->tick();
    tb->core->wb_stb_i = 0;
    tb->tick();
    tb->core->wb_cyc_i = 0;
    tb->tick();

    tb->read(0x40, 0xF);
    tb->tick();
  
    expected_data = expected_data & ~(0xFF << (8*i)) | ((write_data & 0xFF) << (8*i));
    tb->check(COND_write, (core->wb_dat_o == expected_data));

    tb->core->wb_stb_i = 0;
    tb->tick();
    tb->core->wb_cyc_i = 0;
    tb->tick();
  }

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbmem_bram.w_8.01",
      tb->conditions[COND_write],
      "Failed to implement the write function", tb->err_cycles[COND_write]);
}

void tb_ecap5_dwbmem_bram_preload(TB_Ecap5_dwbmem_bram * tb) {
  Vtb_ecap5_dwbmem_bram * core = tb->core;
  core->testcase = T_PRELOAD;

  tb->reset();

  for(int i = 0; i < 15; i++) {
    tb->read(4*i, 0xF);

    tb->tick();

    uint32_t expected_data = (i+1) | (i+1) << 4 | (i+1) << 8 | (i+1) << 12 | (i+1) << 16 | (i+1) << 20 | (i+1) << 24 | (i+1) << 28;
    tb->check(COND_preload, (core->wb_dat_o == expected_data));

    tb->core->wb_stb_i = 0;

    tb->tick();

    tb->core->wb_cyc_i = 0;
    tb->tick();
  }

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbmem_bram.preload.01",
      tb->conditions[COND_preload],
      "Failed to implement the preload function", tb->err_cycles[COND_preload]);
}

int main(int argc, char ** argv, char ** env) {
  srand(time(NULL));
  Verilated::traceEverOn(true);

  bool verbose = parse_verbose(argc, argv);

  TB_Ecap5_dwbmem_bram * tb = new TB_Ecap5_dwbmem_bram;
  tb->open_trace("waves/ecap5_dwbmem_bram.vcd");
  tb->open_testdata("testdata/ecap5_dwbmem_bram.csv");
  tb->set_debug_log(verbose);
  tb->init_conditions(__CondIdEnd);

  /************************************************************/

  tb_ecap5_dwbmem_bram_r_32(tb);
  tb_ecap5_dwbmem_bram_r_16(tb);
  tb_ecap5_dwbmem_bram_r_8(tb);
  tb_ecap5_dwbmem_bram_w_32(tb);
  tb_ecap5_dwbmem_bram_w_16(tb);
  tb_ecap5_dwbmem_bram_w_8(tb);
  tb_ecap5_dwbmem_bram_preload(tb);

  /************************************************************/

  printf("[ECAP5_DWBMEM-BRAM]: ");
  if(tb->success) {
    printf("Done\n");
  } else {
    printf("Failed\n");
  }

  delete tb;
  exit(EXIT_SUCCESS);
}
