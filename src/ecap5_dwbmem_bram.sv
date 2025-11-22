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

module ecap5_dwbmem_bram #(
  parameter ENABLE_PRELOADING = 0,
  parameter PRELOAD_HEX_PATH = ""
)(
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

/*****************************************/
/*           Internal signals            */
/*****************************************/

logic[31:0] mem_addr;
logic       mem_read, mem_write;
logic[31:0] mem_read_data_q, 
            mem_write_data;
logic[3:0]  mem_sel;

logic[31:0] bram[512];
logic[31:0] bram_data_q;

logic[7:0] read_data_bytes[4];

/*****************************************/

initial begin
  if(ENABLE_PRELOADING) begin
    $readmemh(PRELOAD_HEX_PATH, bram, 0); 
  end
end

TMP_wb_interface wb_interface_inst (
  .clk_i (clk_i),   .rst_i (rst_i),
  
  .wb_adr_i (wb_adr_i),  .wb_dat_o (wb_dat_o),  .wb_dat_i   (wb_dat_i),
  .wb_we_i  (wb_we_i),   .wb_sel_i (wb_sel_i),  .wb_stb_i   (wb_stb_i),
  .wb_ack_o (wb_ack_o),  .wb_cyc_i (wb_cyc_i),  .wb_stall_o (wb_stall_o),

  .addr_o       (mem_addr),
  .read_o       (mem_read),
  .read_data_i  (mem_read_data_q),
  .write_o      (mem_write),
  .write_data_o (mem_write_data),
  .sel_o        (mem_sel)
);

always_comb begin : read
  read_data_bytes[0] = mem_sel[0] ? bram_data_q[7:0]   : 8'h0;
  read_data_bytes[1] = mem_sel[1] ? bram_data_q[15:8]  : 8'h0;
  read_data_bytes[2] = mem_sel[2] ? bram_data_q[23:16] : 8'h0;
  read_data_bytes[3] = mem_sel[3] ? bram_data_q[31:24] : 8'h0;

  mem_read_data_q = {read_data_bytes[3], read_data_bytes[2], read_data_bytes[1], read_data_bytes[0]};
end

always_ff @(posedge clk_i) begin
  if(mem_read) begin
    bram_data_q <= bram[mem_addr[8:0]];
  end else begin
    bram_data_q <= 32'h0;
  end

  if(mem_write) begin
    if(mem_sel[0]) begin
      bram[mem_addr[8:0]][7:0] <= mem_write_data[7:0];
    end
    if(mem_sel[1]) begin
      bram[mem_addr[8:0]][15:8] <= mem_write_data[15:8];
    end
    if(mem_sel[2]) begin
      bram[mem_addr[8:0]][23:16] <= mem_write_data[23:16];
    end
    if(mem_sel[3]) begin
      bram[mem_addr[8:0]][31:24] <= mem_write_data[31:24];
    end
  end
end

endmodule // ecap5_dwbmem_bram
