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

typedef enum {
  IDLE = 0,
  ACCESS = 1,
  RESPONSE = 2
} state_t;

/*****************************************/
/*           Internal signals            */
/*****************************************/

state_t     state_d, state_q;

logic       wb_ack_d, wb_ack_q;

logic[31:0] mem_addr_d, mem_addr_q;
logic       mem_read_d, mem_read_q, 
            mem_write_d, mem_write_q;
logic[31:0] mem_read_data,
            mem_write_data_d, mem_write_data_q;
logic[3:0]  mem_sel_d, mem_sel_q;

logic[31:0] bram[SIZE];
logic[$clog2(SIZE)-1:0]  bram_cell_address;
logic[31:0] bram_data_q, bram_data_qq;
logic[31:0] bram_data_shift0, bram_data_shift1;

logic[7:0] read_data_bytes[4];

logic[3:0] sel_shift0, sel_shift1;
logic[31:0] mem_write_data_shift0, mem_write_data_shift1;

/*****************************************/

initial begin
  if(ENABLE_PRELOADING) begin
    $readmemh(PRELOAD_HEX_PATH, bram, 0); 
  end
end

always_comb begin : wishbone
  state_d = state_q;
  wb_ack_d = 0;
  mem_addr_d = mem_addr_q;
  mem_write_data_d = mem_write_data_q;
  mem_read_d = mem_read_q;
  mem_write_d = mem_write_q;
  mem_sel_d = mem_sel_q;

  case(state_q)
    IDLE: begin
      if(wb_stb_i && wb_cyc_i) begin
        state_d = ACCESS;
        mem_addr_d = wb_adr_i;
        mem_write_data_d = wb_dat_i;
        mem_read_d = !wb_we_i;
        mem_write_d = wb_we_i;
        mem_sel_d = wb_sel_i;
      end
    end
    ACCESS: begin
      state_d = RESPONSE;
      wb_ack_d = 1;
    end
    RESPONSE: begin
      state_d = IDLE; 
      mem_read_d = 0;
      mem_write_d = 0;
    end
    default: begin end
  endcase
end

always_comb begin : read
  bram_cell_address = {2'b0, wb_adr_i[$clog2(SIZE)-1:2]};

  // barrel shift the data according to the address
  bram_data_shift0 = mem_addr_q[0] ? {8'b0,  bram_data_qq[31:8]} : bram_data_qq;
  bram_data_shift1 = mem_addr_q[1] ? {16'b0, bram_data_shift0[31:16]} : bram_data_shift0;

  // Set unselected bytes to zero
  read_data_bytes[0] = mem_sel_q[0] ? bram_data_shift1[7:0]   : 8'h0;
  read_data_bytes[1] = mem_sel_q[1] ? bram_data_shift1[15:8]  : 8'h0;
  read_data_bytes[2] = mem_sel_q[2] ? bram_data_shift1[23:16] : 8'h0;
  read_data_bytes[3] = mem_sel_q[3] ? bram_data_shift1[31:24] : 8'h0;

  // Assemble the final read data
  mem_read_data = {read_data_bytes[3], read_data_bytes[2], read_data_bytes[1], read_data_bytes[0]};
end

always_comb begin : write
  // barrel shift the sel
  sel_shift0 =  mem_addr_q[0] ? {mem_sel_q[2:0], 1'b0} : mem_sel_q;
  sel_shift1 = mem_addr_q[1] ? {sel_shift0[1:0], 2'b0} : sel_shift0;

  // barrel shift the write data
  mem_write_data_shift0 =  mem_addr_q[0] ? {mem_write_data_q[23:0], 8'b0} : mem_write_data_q;
  mem_write_data_shift1 = mem_addr_q[1] ? {mem_write_data_shift0[15:0], 16'b0} : mem_write_data_shift0;
end

always_ff @(posedge clk_i) begin
  if(rst_i) begin
    state_q <= IDLE;  

    mem_addr_q              <=  '0;
    mem_read_q              <=  0;
    mem_write_q             <=  0;
    mem_write_data_q        <=  '0;
    mem_sel_q               <=  '0;

    wb_ack_q                <=  0;
  end else begin
    state_q <= state_d;

    mem_addr_q              <=  mem_addr_d;
    mem_read_q              <=  mem_read_d;
    mem_write_q             <=  mem_write_d;
    mem_write_data_q        <=  mem_write_data_d;
    mem_sel_q               <=  mem_sel_d;

    wb_ack_q                <=  wb_ack_d;
  end

  /* BRAM access */

  bram_data_q <= bram[bram_cell_address];
  /* Add a double registering to reduce bram timing constraints */
  if(state_q == ACCESS) begin
    bram_data_qq <= bram_data_q;
  end

  if(state_q == ACCESS && mem_write_q) begin
    if(sel_shift1[0]) begin
      bram[bram_cell_address][7:0] <= mem_write_data_shift1[7:0];
    end
    if(sel_shift1[1]) begin
      bram[bram_cell_address][15:8] <= mem_write_data_shift1[15:8];
    end
    if(sel_shift1[2]) begin
      bram[bram_cell_address][23:16] <= mem_write_data_shift1[23:16];
    end
    if(sel_shift1[3]) begin
      bram[bram_cell_address][31:24] <= mem_write_data_shift1[31:24];
    end
  end
end

assign wb_ack_o = wb_ack_q;
assign wb_dat_o = mem_read_data;

endmodule // ecap5_dwbmem_bram
