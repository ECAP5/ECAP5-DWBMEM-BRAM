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

module tb_ecap5_dwbmem_bram
(
  input   int          testcase,

  input   logic         clk_i,
  input   logic         rst_i,

  //=================================
  //    Memory interface

  input   logic[31:0]  wb_adr_i,
  output  logic[31:0]  wb_dat_o,
  input   logic[31:0]  wb_dat_i,
  input   logic        wb_we_i,
  input   logic[3:0]   wb_sel_i,
  input   logic        wb_stb_i,
  output  logic        wb_ack_o,
  input   logic        wb_cyc_i,
  output  logic        wb_stall_o
);

ecap5_dwbmem_bram #(
  .ENABLE_PRELOADING (1),
  .PRELOAD_HEX_PATH ("/home/ubuntu/ecap5/design/wbmem-bram/tests/benches/ecap5_dwbmem_bram/program.hex")
) dut (
  .clk_i      (clk_i),
  .rst_i      (rst_i),

  .wb_adr_i   (wb_adr_i),
  .wb_dat_o   (wb_dat_o),
  .wb_dat_i   (wb_dat_i),
  .wb_we_i    (wb_we_i),
  .wb_sel_i   (wb_sel_i),
  .wb_stb_i   (wb_stb_i),
  .wb_ack_o   (wb_ack_o),
  .wb_cyc_i   (wb_cyc_i),
  .wb_stall_o (wb_stall_o)
);

endmodule // tb_ecap5_dwbmem_bram

`verilator_config

public -module "ecap5_dwbmem_bram" -var "bram"

