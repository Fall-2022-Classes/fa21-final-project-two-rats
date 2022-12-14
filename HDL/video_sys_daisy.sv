/*-- =================================================================
--  I/O space in video system 
-- =================================================================
--    * 21-bit word address space (passed to this module) 
--    * 1 xxxx xxxx xxxx xxxx xxxx (frame buffer - 1M)
--    * 0 0000 00xx xxxx xxxx xxxx (video slot #0, vga sync)
--    * 0 0000 01xx xxxx xxxx xxxx (video slot #1, mouse)
--    * 0 0000 10xx xxxx xxxx xxxx (video slot #2, osd)
--    * 0 0000 11xx xxxx xxxx xxxx (video slot #3, bar)
-- =================================================================
--    ** 24-bit byte I/O address within the I/O system (used C++ driver)
--    * 1_1xx xxxx xxxx xxxx xxxx xx00 (frame buffer - 1M)
--    * 1_01s ssss xxxx xxxx xxxx xx00 (32 sprites - 32*4K)
--    * 1_000 0000 xxxx xxxx xxxx xx00 (video slot #0, vga sync)
--    * 1_000 0001 xxxx xxxx xxxx xx00 (video slot #1, mouse)
--    * 1_000 0011 xxxx xxxx xxxx xx00 (video slot #3, bar)
*/

`include "chu_io_map.svh"
module video_sys_daisy 
#(
   parameter CD = 12,            // color depth
   parameter VRAM_DATA_WIDTH = 9 //frame buffer data width
)
(
   input logic clk_sys,
   input logic clk_25M,
   input logic reset_sys,
   // FPro bus 
   input logic video_cs,
   input logic video_wr,
   input logic [20:0] video_addr, 
   input logic [31:0] video_wr_data,
   // to vga monitor  
   output logic vsync, hsync,
   output logic [11:0] rgb 
);
   // costant declaration
   localparam KEY_COLOR = 0;
   // signal declaration
   // video data stream
   logic [CD-1:0] frame_rgb8, bar_rgb7, dance_rgb6, turtle_rgb5;         
   logic [CD-1:0] log_rgb4, car_rgb3, osd_rgb2, frog_rgb1;
   logic [CD:0] line_data_in;
   // frame counter
   logic inc, frame_start;
   logic [10:0] x, y;
   // delay line
   logic frame_start_d1_reg, frame_start_d2_reg;
   logic inc_d1_reg, inc_d2_reg;
   // frame interface
   logic frame_wr, frame_cs;
   logic [19:0] frame_addr;
   logic [31:0] frame_wr_data;
   // video core slot interface 
   logic [7:0] slot_cs_array;
   logic [7:0] slot_mem_wr_array;
   logic [13:0] slot_reg_addr_array [7:0];
   logic [31:0] slot_wr_data_array [7:0];
   
   // 2-stage delay line for start signal
   always_ff @(posedge clk_sys) begin
      frame_start_d1_reg <= frame_start;
      frame_start_d2_reg <= frame_start_d1_reg;
      inc_d1_reg <= inc;
      inc_d2_reg <= inc_d1_reg;
   end

   // instantiate frame counter
   frame_counter #(.HMAX(640), .VMAX(480)) frame_counter_unit
      (.clk(clk_sys), .reset(reset_sys), 
       .sync_clr(0), .inc(inc), .hcount(x), .vcount(y), 
       .frame_start(frame_start), .frame_end());
   // instantiate video decoding circuit 
   chu_video_controller ctrl_unit (
      .video_cs(video_cs),
      .video_wr(video_wr),
      .video_addr(video_addr),
      .video_wr_data(video_wr_data),
      .frame_cs(frame_cs),
      .frame_wr(frame_wr),
      .frame_addr(frame_addr),
      .frame_wr_data(frame_wr_data),
      .slot_cs_array(slot_cs_array),
      .slot_mem_wr_array(slot_mem_wr_array),
      .slot_reg_addr_array(slot_reg_addr_array),
      .slot_wr_data_array(slot_wr_data_array)
      );

   // instantiate frame buffer
   chu_frame_buffer_core #(.CD(CD), .DW(VRAM_DATA_WIDTH)) buf_unit (
      .clk(clk_sys),
      .reset(reset_sys),
      .x(x),
      .y(y),
      .cs(frame_cs),
      .write(frame_wr),
      .addr(frame_addr),
      .wr_data(video_wr_data),
      .si_rgb(12'h008),        // blue screen
      .so_rgb(frame_rgb8)
     );

   // instantiate bar generator 
//   chu_vga_bar_core v7_bar_unit (
//      .clk(clk_sys),
//      .reset(reset_sys),
//      .x(x),
//      .y(y),
//      .cs(slot_cs_array[`V7_BAR]),
//      .write(slot_mem_wr_array[`V7_BAR]),
//      .addr(slot_reg_addr_array[`V7_BAR]),
//      .wr_data(slot_wr_data_array[`V7_BAR]),
//      .si_rgb(12'h008),        // blue screen
//      .so_rgb(bar_rgb7)
//   );
   // instantiate dancing frog sprite
   vga_sprite_dance_core 
      #(.CD(CD), .ADDR_WIDTH(13), .KEY_COLOR(KEY_COLOR)) 
   v6_dance_unit (
      .clk(clk_sys),
      .reset(reset_sys),
      .x(x),
      .y(y),
      .cs(slot_cs_array[`V6_DANCE]),
      .write(slot_mem_wr_array[`V6_DANCE]),
      .addr(slot_reg_addr_array[`V6_DANCE]),
      .wr_data(slot_wr_data_array[`V6_DANCE]),
      .si_rgb(frame_rgb8),      
      .so_rgb(dance_rgb6)
   );
   // instantiate turtle sprite
   vga_sprite_turtle_core #(.CD(CD), .ADDR_WIDTH(13), .KEY_COLOR(KEY_COLOR)) 
        v5_turtle_unit (
      .clk(clk_sys),
      .reset(reset_sys),
      .x(x),
      .y(y),
      .cs(slot_cs_array[`V5_TURTLE]),
      .write(slot_mem_wr_array[`V5_TURTLE]),
      .addr(slot_reg_addr_array[`V5_TURTLE]),
      .wr_data(slot_wr_data_array[`V5_TURTLE]),
      .si_rgb(dance_rgb6),     
      .so_rgb(turtle_rgb5)
   );
   // instantiate log sprite
   vga_sprite_log_core #(.CD(CD), .ADDR_WIDTH(13), .KEY_COLOR(KEY_COLOR)) v4_log_unit (
      .clk(clk_sys),
      .reset(reset_sys),
      .x(x),
      .y(y),
      .cs(slot_cs_array[`V4_LOG]),
      .write(slot_mem_wr_array[`V4_LOG]),
      .addr(slot_reg_addr_array[`V4_LOG]),
      .wr_data(slot_wr_data_array[`V4_LOG]),
      .si_rgb(turtle_rgb5),       
      .so_rgb(log_rgb4)
   );
   // instantiate car sprite
   vga_sprite_car_core 
       #(.CD(CD), .ADDR_WIDTH(13), .KEY_COLOR(KEY_COLOR)) v3_car_unit (
      .clk(clk_sys),
      .reset(reset_sys),
      .x(x),
      .y(y),
      .cs(slot_cs_array[`V3_CAR]),
      .write(slot_mem_wr_array[`V3_CAR]),
      .addr(slot_reg_addr_array[`V3_CAR]),
      .wr_data(slot_wr_data_array[`V3_CAR]),
      .si_rgb(log_rgb4),
      .so_rgb(car_rgb3)
   );
   // instantiate osd 
   chu_vga_osd_core#(.CD(CD), .KEY_COLOR(KEY_COLOR)) v2_osd_unit (
      .clk(clk_sys),
      .reset(reset_sys),
      .x(x),
      .y(y),
      .cs(slot_cs_array[`V2_OSD]),
      .write(slot_mem_wr_array[`V2_OSD]),
      .addr(slot_reg_addr_array[`V2_OSD]),
      .wr_data(slot_wr_data_array[`V2_OSD]),
      .si_rgb(car_rgb3),
      .so_rgb(osd_rgb2)
   );
   // instantiate frog sprite
   vga_sprite_frog_core 
      #(.CD(CD), .ADDR_WIDTH(13), .KEY_COLOR(KEY_COLOR)) 
   v1_frog_unit (
      .clk(clk_sys),
      .reset(reset_sys),
      .x(x),
      .y(y),
      .cs(slot_cs_array[`V1_FROG]),
      .write(slot_mem_wr_array[`V1_FROG]),
      .addr(slot_reg_addr_array[`V1_FROG]),
      .wr_data(slot_wr_data_array[`V1_FROG]),
      .si_rgb(osd_rgb2),
      .so_rgb(frog_rgb1)
   );
   // merge start signal to rgb data stream
   assign line_data_in = {frog_rgb1, frame_start_d2_reg};
   // instantiate sync_core
   chu_vga_sync_core #(.CD(CD)) v0_vga_sync_unit (
      .clk_sys(clk_sys),
      .clk_25M(clk_25M),
      .reset(reset_sys),
      .cs(slot_cs_array[`V0_SYNC]),
      .write(slot_mem_wr_array[`V0_SYNC]),
      .addr(slot_reg_addr_array[`V0_SYNC]),
      .wr_data(slot_wr_data_array[`V0_SYNC]),
      .si_data(line_data_in),
      .si_valid(inc_d2_reg),
      .si_ready(inc),
      .hsync(hsync),
      .vsync(vsync),
      .rgb(rgb)
   );
endmodule

