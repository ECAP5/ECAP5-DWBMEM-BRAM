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
  parameter int    SIZE = 512,
  parameter logic  ENABLE_PRELOADING = 0,
  parameter string PRELOAD_HEX_PATH = ""
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

logic       mem_write_q;

logic[31:0] bram[SIZE];
logic[$clog2(SIZE)-1:0]  cell_address;
logic[31:0] bram_data_q;
logic[31:0] bram_data_shift0;
logic[31:0] bram_data_shift1;

logic[7:0] read_data_bytes[4];

logic[3:0] sel_shift0;
logic[3:0] sel_shift1;

/*****************************************/

initial begin
  if(ENABLE_PRELOADING) begin
    $readmemh(PRELOAD_HEX_PATH, bram, 0); 
  end
end

ecap5_dwbmmsc wb_interface_inst (
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

assign cell_address = {2'b0, mem_addr[$clog2(SIZE)-1:2]};

always_comb begin : read

  // barrel shift the data according to the address
  bram_data_shift0 = mem_addr[0] ? {8'b0, bram_data_q[31:8]} : bram_data_q;
  bram_data_shift1 = mem_addr[1] ? {16'b0, bram_data_shift0[31:16]} : bram_data_shift0;

  // Set unselected bytes to zero
  read_data_bytes[0] = mem_sel[0] ? bram_data_shift1[7:0]   : 8'h0;
  read_data_bytes[1] = mem_sel[1] ? bram_data_shift1[15:8]  : 8'h0;
  read_data_bytes[2] = mem_sel[2] ? bram_data_shift1[23:16] : 8'h0;
  read_data_bytes[3] = mem_sel[3] ? bram_data_shift1[31:24] : 8'h0;

  // Assemble the final read data
  mem_read_data_q = {read_data_bytes[3], read_data_bytes[2], read_data_bytes[1], read_data_bytes[0]};
end

always_comb begin : write
  // barrel shift the sel
  sel_shift0 = mem_addr[0] ? {mem_sel[2:0], 1'b0} : mem_sel;
  sel_shift1 = mem_addr[1] ? {sel_shift0[1:0], 2'b0} : sel_shift0;
end

always_ff @(posedge clk_i) begin
  if(mem_read) begin
    bram_data_q <= bram[cell_address];
  end

  if(mem_write) begin
    if(sel_shift1[0]) begin
      bram[cell_address][7:0] <= mem_write_data[7:0];
    end
    if(sel_shift1[1]) begin
      bram[cell_address][15:8] <= mem_write_data[15:8];
    end
    if(sel_shift1[2]) begin
      bram[cell_address][23:16] <= mem_write_data[23:16];
    end
    if(sel_shift1[3]) begin
      bram[cell_address][31:24] <= mem_write_data[31:24];
    end
  end
end

endmodule // ecap5_dwbmem_bram
