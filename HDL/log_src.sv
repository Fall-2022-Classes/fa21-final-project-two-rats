module log_src 
   #(
    parameter CD = 12,      // color depth
              ADDR = 10,    // number of address bits
              KEY_COLOR =0  // chroma key
   )
   (
    input  logic clk,
    input  logic [10:0] x, y,   // x-and  y-coordinate    
    input  logic [10:0] x0, y0, // origin of sprite 
    input  logic [10:0] x1, y1, // origin of sprite 
    input  logic [10:0] x2, y2, // origin of sprite 
    input  logic [10:0] x3, y3, // origin of sprite 
    input  logic [10:0] x4, y4, // origin of sprite 
    input  logic [10:0] x5, y5, // origin of sprite 
    input  logic [10:0] x6, y6, // origin of sprite 
    input  logic [10:0] x7, y7, // origin of sprite 
    input  logic [10:0] x8, y8, // origin of sprite 
    input  logic [10:0] x9, y9, // origin of sprite 
    input  logic [10:0] x10, y10, // origin of sprite 
    input  logic [10:0] x11, y11, // origin of sprite
    input  logic [10:0] x12, y12, // origin of sprite 
    input  logic [10:0] x13, y13, // origin of sprite 
    input  logic [10:0] x14, y14, // origin of sprite 
    input  logic [10:0] x15, y15, // origin of sprite 
    input  logic [10:0] x16, y16, // origin of sprite 
    input  logic [10:0] x17, y17, // origin of sprite 
    input  logic [10:0] x18, y18, // origin of sprite 
    input  logic [10:0] x19, y19, // origin of sprite 
    input  logic [3:0] ctrl_0,    // sprite control 
    input  logic [3:0] ctrl_1,    // sprite control 
    input  logic [3:0] ctrl_2,    // sprite control 
    input  logic [3:0] ctrl_3,    // sprite control 
    input  logic [3:0] ctrl_4,    // sprite control 
    input  logic [3:0] ctrl_5,    // sprite control 
    input  logic [3:0] ctrl_6,    // sprite control 
    input  logic [3:0] ctrl_7,    // sprite control 
    input  logic [3:0] ctrl_8,    // sprite control 
    input  logic [3:0] ctrl_9,    // sprite control 
    input  logic [3:0] ctrl_10,    // sprite control 
    input  logic [3:0] ctrl_11,    // sprite control 
    input  logic [3:0] ctrl_12,    // sprite control 
    input  logic [3:0] ctrl_13,    // sprite control 
    input  logic [3:0] ctrl_14,    // sprite control 
    input  logic [3:0] ctrl_15,    // sprite control 
    input  logic [3:0] ctrl_16,    // sprite control 
    input  logic [3:0] ctrl_17,    // sprite control 
    input  logic [3:0] ctrl_18,    // sprite control 
    input  logic [3:0] ctrl_19,    // sprite control 
    // sprite ram write 
    input  logic we ,
    input  logic [ADDR-1:0] addr_w,
    input  logic [1:0] pixel_in,
    // pixel output
    output logic [CD-1:0] sprite_rgb
   );
   
   // localparam declaration
   localparam H_SIZE = 64; // horizontal size of sprite
   localparam V_SIZE = 32; // vertical size of sprite
   // signal delaration
   logic signed [11:0] xr0, yr0, xr1, yr1, xr2, yr2, xr3, yr3,  // relative x/y position
                        xr4, yr4, xr5, yr5, xr6, yr6, xr7, yr7,
                        xr8, yr8, xr9, yr9, xr10, yr10, xr11, yr11,
                        xr12, yr12, xr13, yr13, xr14, yr14, xr15, yr15,
                        xr16, yr16, xr17, yr17, xr18, yr18, xr19, yr19;  
   logic in_region, in_region0, in_region1, in_region2, in_region3,
            in_region4, in_region5, in_region6, in_region7,
            in_region8, in_region9, in_region10, in_region11,
            in_region12, in_region13, in_region14, in_region15,
            in_region16, in_region17, in_region18, in_region19;
   logic [ADDR-1:0] addr_r;
   logic [1:0] plt_code;      
        
   logic [10:0] x_d1_reg;
   logic [CD-1:0]  out_rgb;
   logic [CD-1:0] full_rgb, sel_rgb;
   logic [CD-1:0] out_rgb_d1_reg;
   logic [1:0] color_sel, color_sel_0, color_sel_1, color_sel_2, color_sel_3,    
                    color_sel_4, color_sel_5, color_sel_6, color_sel_7,  
                    color_sel_8, color_sel_9, color_sel_10, color_sel_11,  
                    color_sel_12, color_sel_13, color_sel_14, color_sel_15,  
                    color_sel_16, color_sel_17, color_sel_18, color_sel_19;
   logic [1:0] id_sel_0, id_sel_1, id_sel_2, id_sel_3, 
                id_sel_4, id_sel_5, id_sel_6, id_sel_7,   
                id_sel_8, id_sel_9, id_sel_10, id_sel_11, 
                id_sel_12, id_sel_13, id_sel_14, id_sel_15, 
                id_sel_16, id_sel_17, id_sel_18, id_sel_19;
   
   // body 
   //******************************************************************
   // continuous assignments
   //******************************************************************
   // color and id selects
    assign color_sel_0 = ctrl_0[3:2];
    assign id_sel_0 = ctrl_0[1:0];
    assign color_sel_1 = ctrl_1[3:2];
    assign id_sel_1 = ctrl_1[1:0];
    assign color_sel_2 = ctrl_2[3:2];
    assign id_sel_2 = ctrl_2[1:0];
    assign color_sel_3 = ctrl_3[3:2];
    assign id_sel_3 = ctrl_3[1:0];
    assign color_sel_4 = ctrl_4[3:2];
    assign id_sel_4 = ctrl_4[1:0];
    assign color_sel_5 = ctrl_5[3:2];
    assign id_sel_5 = ctrl_5[1:0];
    assign color_sel_6 = ctrl_6[3:2];
    assign id_sel_6 = ctrl_6[1:0];
    assign color_sel_7 = ctrl_7[3:2];
    assign id_sel_7 = ctrl_7[1:0];
    assign color_sel_8 = ctrl_8[3:2];
    assign id_sel_8 = ctrl_8[1:0];
    assign color_sel_9 = ctrl_9[3:2];
    assign id_sel_9 = ctrl_9[1:0];
    assign color_sel_10 = ctrl_10[3:2];
    assign id_sel_10 = ctrl_10[1:0];
    assign color_sel_11 = ctrl_11[3:2];
    assign id_sel_11 = ctrl_11[1:0];
    assign color_sel_12 = ctrl_12[3:2];
    assign id_sel_12 = ctrl_12[1:0];
    assign color_sel_13 = ctrl_13[3:2];
    assign id_sel_13 = ctrl_13[1:0];
    assign color_sel_14 = ctrl_14[3:2];
    assign id_sel_14 = ctrl_14[1:0];
    assign color_sel_15 = ctrl_15[3:2];
    assign id_sel_15 = ctrl_15[1:0];
    assign color_sel_16 = ctrl_16[3:2];
    assign id_sel_16 = ctrl_16[1:0];
    assign color_sel_17 = ctrl_17[3:2];
    assign id_sel_17 = ctrl_17[1:0];
    assign color_sel_18 = ctrl_18[3:2];
    assign id_sel_18 = ctrl_18[1:0];
    assign color_sel_19 = ctrl_19[3:2];
    assign id_sel_19 = ctrl_19[1:0];

   //******************************************************************
   // sprite RAM
   //******************************************************************
   // instantiate sprite RAM
   log_ram_lut #(.ADDR_WIDTH(ADDR), .DATA_WIDTH(2)) ram_unit (
      .clk(clk), .we(we), .addr_w(addr_w), .din(pixel_in),
      .addr_r(addr_r), .dout(plt_code));
 
   //******************************************************************
   // sprite color control
   //******************************************************************
   // sprite color selection
  always_comb
      case (color_sel)
         2'b00:   sel_rgb = 12'hda6;  // brown
         2'b01:   sel_rgb = 12'hf00;  // red 
         2'b10:   sel_rgb = 12'hfa0;  // orange
         default: sel_rgb = 12'h0ff;  // cyan
      endcase   
   // palette table
   always_comb
      case (plt_code)
         2'b00:   full_rgb = 12'h000;   // chrome key
         2'b01:   full_rgb = 12'h742;   // shadow
         2'b10:   full_rgb = sel_rgb;   // sprite body color
         default: full_rgb = 12'hfed;   // hilite
      endcase 
   //******************************************************************
   // in-region circuit
   //******************************************************************
   // relative coordinate calculation
    assign xr0 = $signed({1'b0, x}) - $signed({1'b0, x0});
    assign yr0 = $signed({1'b0, y}) - $signed({1'b0, y0});
    assign xr1 = $signed({1'b0, x}) - $signed({1'b0, x1});
    assign yr1 = $signed({1'b0, y}) - $signed({1'b0, y1});
    assign xr2 = $signed({1'b0, x}) - $signed({1'b0, x2});
    assign yr2 = $signed({1'b0, y}) - $signed({1'b0, y2});
    assign xr3 = $signed({1'b0, x}) - $signed({1'b0, x3});
    assign yr3 = $signed({1'b0, y}) - $signed({1'b0, y3});
    assign xr4 = $signed({1'b0, x}) - $signed({1'b0, x4});
    assign yr4 = $signed({1'b0, y}) - $signed({1'b0, y4});
    assign xr5 = $signed({1'b0, x}) - $signed({1'b0, x5});
    assign yr5 = $signed({1'b0, y}) - $signed({1'b0, y5});
    assign xr6 = $signed({1'b0, x}) - $signed({1'b0, x6});
    assign yr6 = $signed({1'b0, y}) - $signed({1'b0, y6});
    assign xr7 = $signed({1'b0, x}) - $signed({1'b0, x7});
    assign yr7 = $signed({1'b0, y}) - $signed({1'b0, y7});
    assign xr8 = $signed({1'b0, x}) - $signed({1'b0, x8});
    assign yr8 = $signed({1'b0, y}) - $signed({1'b0, y8});
    assign xr9 = $signed({1'b0, x}) - $signed({1'b0, x9});
    assign yr9 = $signed({1'b0, y}) - $signed({1'b0, y9});
    assign xr10 = $signed({1'b0, x}) - $signed({1'b0, x10});
    assign yr10 = $signed({1'b0, y}) - $signed({1'b0, y10});
    assign xr11 = $signed({1'b0, x}) - $signed({1'b0, x11});
    assign yr11 = $signed({1'b0, y}) - $signed({1'b0, y11});
    assign xr12 = $signed({1'b0, x}) - $signed({1'b0, x12});
    assign yr12 = $signed({1'b0, y}) - $signed({1'b0, y12});
    assign xr13 = $signed({1'b0, x}) - $signed({1'b0, x13});
    assign yr13 = $signed({1'b0, y}) - $signed({1'b0, y13});
    assign xr14 = $signed({1'b0, x}) - $signed({1'b0, x14});
    assign yr14 = $signed({1'b0, y}) - $signed({1'b0, y14});
    assign xr15 = $signed({1'b0, x}) - $signed({1'b0, x15});
    assign yr15 = $signed({1'b0, y}) - $signed({1'b0, y15});
    assign xr16 = $signed({1'b0, x}) - $signed({1'b0, x16});
    assign yr16 = $signed({1'b0, y}) - $signed({1'b0, y16});
    assign xr17 = $signed({1'b0, x}) - $signed({1'b0, x17});
    assign yr17 = $signed({1'b0, y}) - $signed({1'b0, y17});
    assign xr18 = $signed({1'b0, x}) - $signed({1'b0, x18});
    assign yr18 = $signed({1'b0, y}) - $signed({1'b0, y18});
    assign xr19 = $signed({1'b0, x}) - $signed({1'b0, x19});
    assign yr19 = $signed({1'b0, y}) - $signed({1'b0, y19});
   // in-region comparison and multiplexing 
    assign in_region0 = ((0<= xr0) && (xr0<H_SIZE) && (0<=yr0) && (yr0<V_SIZE));
    assign in_region1 = ((0<= xr1) && (xr1<H_SIZE) && (0<=yr1) && (yr1<V_SIZE));
    assign in_region2 = ((0<= xr2) && (xr2<H_SIZE) && (0<=yr2) && (yr2<V_SIZE));
    assign in_region3 = ((0<= xr3) && (xr3<H_SIZE) && (0<=yr3) && (yr3<V_SIZE));
    assign in_region4 = ((0<= xr4) && (xr4<H_SIZE) && (0<=yr4) && (yr4<V_SIZE));
    assign in_region5 = ((0<= xr5) && (xr5<H_SIZE) && (0<=yr5) && (yr5<V_SIZE));
    assign in_region6 = ((0<= xr6) && (xr6<H_SIZE) && (0<=yr6) && (yr6<V_SIZE));
    assign in_region7 = ((0<= xr7) && (xr7<H_SIZE) && (0<=yr7) && (yr7<V_SIZE));
    assign in_region8 = ((0<= xr8) && (xr8<H_SIZE) && (0<=yr8) && (yr8<V_SIZE));
    assign in_region9 = ((0<= xr9) && (xr9<H_SIZE) && (0<=yr9) && (yr9<V_SIZE));
    assign in_region10 = ((0<= xr10) && (xr10<H_SIZE) && (0<=yr10) && (yr10<V_SIZE));
    assign in_region11 = ((0<= xr11) && (xr11<H_SIZE) && (0<=yr11) && (yr11<V_SIZE));
    assign in_region12 = ((0<= xr12) && (xr12<H_SIZE) && (0<=yr12) && (yr12<V_SIZE));
    assign in_region13 = ((0<= xr13) && (xr13<H_SIZE) && (0<=yr13) && (yr13<V_SIZE));
    assign in_region14 = ((0<= xr14) && (xr14<H_SIZE) && (0<=yr14) && (yr14<V_SIZE));
    assign in_region15 = ((0<= xr15) && (xr15<H_SIZE) && (0<=yr15) && (yr15<V_SIZE));
    assign in_region16 = ((0<= xr16) && (xr16<H_SIZE) && (0<=yr16) && (yr16<V_SIZE));
    assign in_region17 = ((0<= xr17) && (xr17<H_SIZE) && (0<=yr17) && (yr17<V_SIZE));
    assign in_region18 = ((0<= xr18) && (xr18<H_SIZE) && (0<=yr18) && (yr18<V_SIZE));
    assign in_region19 = ((0<= xr19) && (xr19<H_SIZE) && (0<=yr19) && (yr19<V_SIZE));
   
    always_comb 
    begin
        if (in_region0) begin addr_r = {id_sel_0, yr0[4:0], xr0[5:0]}; color_sel = color_sel_0; end
        else if (in_region1) begin addr_r = {id_sel_1, yr1[4:0], xr1[5:0]}; color_sel = color_sel_1; end
        else if (in_region2) begin addr_r = {id_sel_2, yr2[4:0], xr2[5:0]}; color_sel = color_sel_2; end
        else if (in_region3) begin addr_r = {id_sel_3, yr3[4:0], xr3[5:0]}; color_sel = color_sel_3; end
        else if (in_region4) begin addr_r = {id_sel_4, yr4[4:0], xr4[5:0]}; color_sel = color_sel_4; end
        else if (in_region5) begin addr_r = {id_sel_5, yr5[4:0], xr5[5:0]}; color_sel = color_sel_5; end
        else if (in_region6) begin addr_r = {id_sel_6, yr6[5:0], xr6[5:0]}; color_sel = color_sel_6; end
        else if (in_region7) begin addr_r = {id_sel_7, yr7[4:0], xr7[5:0]}; color_sel = color_sel_7; end
        else if (in_region8) begin addr_r = {id_sel_8, yr8[4:0], xr8[5:0]}; color_sel = color_sel_8; end
        else if (in_region9) begin addr_r = {id_sel_9, yr9[4:0], xr9[5:0]}; color_sel = color_sel_9; end
        else if (in_region10) begin addr_r = {id_sel_10, yr10[4:0], xr10[5:0]}; color_sel = color_sel_10; end
        else if (in_region11) begin addr_r = {id_sel_11, yr11[4:0], xr11[5:0]}; color_sel = color_sel_11; end
        else if (in_region12) begin addr_r = {id_sel_12, yr12[4:0], xr12[5:0]}; color_sel = color_sel_12; end
        else if (in_region13) begin addr_r = {id_sel_13, yr13[4:0], xr13[5:0]}; color_sel = color_sel_13; end
        else if (in_region14) begin addr_r = {id_sel_14, yr14[4:0], xr14[5:0]}; color_sel = color_sel_14; end
        else if (in_region15) begin addr_r = {id_sel_15, yr15[4:0], xr15[5:0]}; color_sel = color_sel_15; end
        else if (in_region16) begin addr_r = {id_sel_16, yr16[4:0], xr16[5:0]}; color_sel = color_sel_16; end
        else if (in_region17) begin addr_r = {id_sel_17, yr17[4:0], xr17[5:0]}; color_sel = color_sel_17; end
        else if (in_region18) begin addr_r = {id_sel_18, yr18[4:0], xr18[5:0]}; color_sel = color_sel_18; end
        else begin addr_r = {id_sel_19, yr19[4:0], xr19[5:0]}; color_sel = color_sel_19; end
    end
    
   assign in_region = (in_region0) | (in_region1) | (in_region2) | (in_region3) |
            (in_region4) | (in_region5) | (in_region6) | (in_region7) |
            (in_region8) | (in_region9) | (in_region10) | (in_region11) |
            (in_region12) | (in_region13) | (in_region14) | (in_region15) |
            (in_region16) | (in_region17) | (in_region18) | (in_region19);
   assign out_rgb = in_region ? full_rgb : KEY_COLOR;
   //******************************************************************
   // animation timing control
   //******************************************************************
   // counters 
   always_ff @(posedge clk) begin
      x_d1_reg <= x;
   end
   //******************************************************************
   // delay line (one clock) 
   //******************************************************************
   // output with a-stage delay line
   always_ff @(posedge clk) 
      out_rgb_d1_reg <= out_rgb;
   assign sprite_rgb = out_rgb_d1_reg;
endmodule
  