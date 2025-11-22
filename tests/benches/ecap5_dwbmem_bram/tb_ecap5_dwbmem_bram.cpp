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
  __CondIdEnd
};

enum StateId {
  T_R8 = 1,
  T_R16,
  T_R32,
  T_W8,
  T_W16,
  T_W32
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

  tb->reset();

  tb->write(0x4, 0x11111111, 0xF);
  tb->tick();

  tb->core->wb_stb_i = 0;
  tb->tick();

  tb->core->wb_cyc_i = 0;
  tb->tick();

  tb->read(0x4, 0xF);
  tb->tick();

  tb->core->wb_stb_i = 0;

  tb->tick();

  tb->core->wb_cyc_i = 0;
  tb->tick();
}

void tb_ecap5_dwbmem_bram_r_16(TB_Ecap5_dwbmem_bram * tb) {
  Vtb_ecap5_dwbmem_bram * core = tb->core;
  core->testcase = T_R16;

  tb->reset();

  tb->write(0x4, 0x22222222, 0xF);
  tb->tick();

  tb->core->wb_stb_i = 0;
  tb->tick();

  tb->core->wb_cyc_i = 0;
  tb->tick();

  tb->read(0x4, 0x3);
  tb->tick();

  tb->core->wb_stb_i = 0;

  tb->tick();

  tb->core->wb_cyc_i = 0;
  tb->tick();
}

void tb_ecap5_dwbmem_bram_r_8(TB_Ecap5_dwbmem_bram * tb) {
  Vtb_ecap5_dwbmem_bram * core = tb->core;
  core->testcase = T_R8;

  tb->reset();

  tb->write(0x4, 0x33333333, 0xF);
  tb->tick();

  tb->core->wb_stb_i = 0;
  tb->tick();

  tb->core->wb_cyc_i = 0;
  tb->tick();

  tb->read(0x4, 0x1);
  tb->tick();

  tb->core->wb_stb_i = 0;

  tb->tick();

  tb->core->wb_cyc_i = 0;
  tb->tick();
}

void tb_ecap5_dwbmem_bram_w_32(TB_Ecap5_dwbmem_bram * tb) {
  Vtb_ecap5_dwbmem_bram * core = tb->core;
  core->testcase = T_W32;

  tb->reset();

  tb->write(0x4, 0x44444444, 0xF);
  tb->tick();

  tb->core->wb_stb_i = 0;
  tb->tick();

  tb->core->wb_cyc_i = 0;
  tb->tick();

  tb->read(0x4, 0xF);
  tb->tick();

  tb->core->wb_stb_i = 0;

  tb->tick();

  tb->core->wb_cyc_i = 0;
  tb->tick();
}

void tb_ecap5_dwbmem_bram_w_16(TB_Ecap5_dwbmem_bram * tb) {
  Vtb_ecap5_dwbmem_bram * core = tb->core;
  core->testcase = T_W16;

  tb->reset();

  tb->write(0x4, 0x55555555, 0x3);
  tb->tick();

  tb->core->wb_stb_i = 0;
  tb->tick();

  tb->core->wb_cyc_i = 0;
  tb->tick();

  tb->read(0x4, 0xF);
  tb->tick();

  tb->core->wb_stb_i = 0;

  tb->tick();

  tb->core->wb_cyc_i = 0;
  tb->tick();
}

void tb_ecap5_dwbmem_bram_w_8(TB_Ecap5_dwbmem_bram * tb) {
  Vtb_ecap5_dwbmem_bram * core = tb->core;
  core->testcase = T_W8;

  tb->reset();

  tb->write(0x4, 0x66666666, 0x1);
  tb->tick();

  tb->core->wb_stb_i = 0;
  tb->tick();

  tb->core->wb_cyc_i = 0;
  tb->tick();

  tb->read(0x4, 0xF);
  tb->tick();

  tb->core->wb_stb_i = 0;

  tb->tick();

  tb->core->wb_cyc_i = 0;
  tb->tick();
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
