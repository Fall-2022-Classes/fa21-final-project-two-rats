module vga_sprite_frog_core
   #(parameter CD = 12,   // color depth
               ADDR_WIDTH = 10,
               KEY_COLOR = 0
   )
   (
    input  logic clk, reset,
    // frame counter
    input logic [10:0] x, y,
    // video slot interface
    input  logic cs,      
    input  logic write,  
    input  logic [13:0] addr,    
    input  logic [31:0] wr_data,
    // stream interface
    input  logic [11:0] si_rgb,
    output logic [11:0] so_rgb
   );

   // delaration
   logic wr_en, wr_ram, wr_reg, wr_bypass;  
   logic wr_ctrl_0, wr_x0, wr_y0, wr_ctrl_1, wr_x1, wr_y1, wr_ctrl_2, wr_x2, wr_y2, wr_ctrl_3, wr_x3, wr_y3,
            wr_ctrl_4, wr_x4, wr_y4, wr_ctrl_5, wr_x5, wr_y5, wr_ctrl_6, wr_x6, wr_y6, wr_ctrl_7, wr_x7, wr_y7,
            wr_ctrl_8, wr_x8, wr_y8, wr_ctrl_9, wr_x9, wr_y9, wr_ctrl_10, wr_x10, wr_y10, wr_ctrl_11, wr_x11, wr_y11,
            wr_ctrl_12, wr_x12, wr_y12, wr_ctrl_13, wr_x13, wr_y13, wr_ctrl_14, wr_x14, wr_y14, wr_ctrl_15, wr_x15, wr_y15,
            wr_ctrl_16, wr_x16, wr_y16, wr_ctrl_17, wr_x17, wr_y17, wr_ctrl_18, wr_x18, wr_y18, wr_ctrl_19, wr_x19, wr_y19;
   logic [CD-1:0] sprite_rgb, chrom_rgb;
   logic [10:0] x0_reg, y0_reg, x1_reg, y1_reg, x2_reg, y2_reg, x3_reg, y3_reg, 
                x4_reg, y4_reg, x5_reg, y5_reg, x6_reg, y6_reg, x7_reg, y7_reg, 
                x8_reg, y8_reg, x9_reg, y9_reg, x10_reg, y10_reg, x11_reg, y11_reg, 
                x12_reg, y12_reg, x13_reg, y13_reg, x14_reg, y14_reg, x15_reg, y15_reg,
                x16_reg, y16_reg, x17_reg, y17_reg, x18_reg, y18_reg, x19_reg, y19_reg;
   logic [3:0] ctrl_0_reg, ctrl_1_reg, ctrl_2_reg, ctrl_3_reg,
                ctrl_4_reg, ctrl_5_reg, ctrl_6_reg, ctrl_7_reg,
                ctrl_8_reg, ctrl_9_reg, ctrl_10_reg, ctrl_11_reg,
                ctrl_12_reg, ctrl_13_reg, ctrl_14_reg, ctrl_15_reg,
            ctrl_16_reg, ctrl_17_reg, ctrl_18_reg, ctrl_19_reg;
   logic bypass_reg;

   // body
   // instantiate sprite generator
    frog_src #(.CD(12), .KEY_COLOR(0), .ADDR(ADDR_WIDTH)) frog_src_unit (
       .clk(clk), .x(x), .y(y), 
       .x0(x0_reg), .y0(y0_reg), .x1(x1_reg), .y1(y1_reg), .x2(x2_reg), .y2(y2_reg), .x3(x3_reg), .y3(y3_reg),
        .x4(x4_reg), .y4(y4_reg), .x5(x5_reg), .y5(y5_reg), .x6(x6_reg), .y6(y6_reg), .x7(x7_reg), .y7(y7_reg),
        .x8(x8_reg), .y8(y8_reg), .x9(x9_reg), .y9(y9_reg), .x10(x10_reg), .y10(y10_reg), .x11(x11_reg), .y11(y11_reg),
        .x12(x12_reg), .y12(y12_reg), .x13(x13_reg), .y13(y13_reg), .x14(x14_reg), .y14(y14_reg), .x15(x15_reg), .y15(y15_reg),
        .x16(x16_reg), .y16(y16_reg), .x17(x17_reg), .y17(y17_reg), .x18(x18_reg), .y18(y18_reg), .x19(x19_reg), .y19(y19_reg),
        .ctrl_0(ctrl_0_reg), .ctrl_1(ctrl_1_reg), .ctrl_2(ctrl_2_reg), .ctrl_3(ctrl_3_reg), 
        .ctrl_4(ctrl_4_reg), .ctrl_5(ctrl_5_reg), .ctrl_6(ctrl_6_reg), .ctrl_7(ctrl_7_reg), 
        .ctrl_8(ctrl_8_reg), .ctrl_9(ctrl_9_reg), .ctrl_10(ctrl_10_reg), .ctrl_11(ctrl_11_reg), 
        .ctrl_12(ctrl_12_reg), .ctrl_13(ctrl_13_reg), .ctrl_14(ctrl_14_reg), .ctrl_15(ctrl_15_reg), 
        .ctrl_16(ctrl_16_reg), .ctrl_17(ctrl_17_reg), .ctrl_18(ctrl_18_reg), .ctrl_19(ctrl_19_reg), 
       .we(wr_ram), .addr_w(addr[ADDR_WIDTH-1:0]),
       .pixel_in(wr_data[1:0]), .sprite_rgb(sprite_rgb));
       
   // register  
   always_ff @(posedge clk, posedge reset)
      if (reset) begin
        x0_reg <= 0;
        y0_reg <= 0;
        x1_reg <= 0;
        y1_reg <= 0;
        x2_reg <= 0;
        y2_reg <= 0;
        x3_reg <= 0;
        y3_reg <= 0;
        x4_reg <= 0;
        y4_reg <= 0;
        x5_reg <= 0;
        y5_reg <= 0;
        x6_reg <= 0;
        y6_reg <= 0;
        x7_reg <= 0;
        y7_reg <= 0;
        x8_reg <= 0;
        y8_reg <= 0;
        x9_reg <= 0;
        y9_reg <= 0;
        x10_reg <= 0;
        y10_reg <= 0;
        x11_reg <= 0;
        y11_reg <= 0;
        x12_reg <= 0;
        y12_reg <= 0;
        x13_reg <= 0;
        y13_reg <= 0;
        x14_reg <= 0;
        y14_reg <= 0;
        x15_reg <= 0;
        y15_reg <= 0;
        x16_reg <= 0;
        y16_reg <= 0;
        x17_reg <= 0;
        y17_reg <= 0;
        x18_reg <= 0;
        y18_reg <= 0;
        x19_reg <= 0;
        y19_reg <= 0;
        ctrl_0_reg <= 4'b0000;   // right-facing sprite, default color
        ctrl_1_reg <= 4'b0000;
        ctrl_2_reg <= 4'b0000;
        ctrl_3_reg <= 4'b0000;
        ctrl_4_reg <= 4'b0000;
        ctrl_5_reg <= 4'b0000;
        ctrl_6_reg <= 4'b0000;
        ctrl_7_reg <= 4'b0000;
        ctrl_8_reg <= 4'b0000;
        ctrl_9_reg <= 4'b0000;
        ctrl_10_reg <= 4'b0000;
        ctrl_11_reg <= 4'b0000;
        ctrl_12_reg <= 4'b0000;
        ctrl_13_reg <= 4'b0000;
        ctrl_14_reg <= 4'b0000;
        ctrl_15_reg <= 4'b0000;
        ctrl_16_reg <= 4'b0000;
        ctrl_17_reg <= 4'b0000;
        ctrl_18_reg <= 4'b0000;
        ctrl_19_reg <= 4'b0000;
        bypass_reg <= 0;
      end   
      else begin
        if (wr_x0)
            x0_reg <= wr_data[10:0];
        if (wr_y0)
            y0_reg <= wr_data[10:0];
        if (wr_x1)
            x1_reg <= wr_data[10:0];
        if (wr_y1)
            y1_reg <= wr_data[10:0];
        if (wr_x2)
            x2_reg <= wr_data[10:0];
        if (wr_y2)
            y2_reg <= wr_data[10:0];
        if (wr_x3)
            x3_reg <= wr_data[10:0];
        if (wr_y3)
            y3_reg <= wr_data[10:0];
        if (wr_x4)
            x4_reg <= wr_data[10:0];
        if (wr_y4)
            y4_reg <= wr_data[10:0];
        if (wr_x5)
            x5_reg <= wr_data[10:0];
        if (wr_y5)
            y5_reg <= wr_data[10:0];
        if (wr_x6)
            x6_reg <= wr_data[10:0];
        if (wr_y6)
            y6_reg <= wr_data[10:0];
        if (wr_x7)
            x7_reg <= wr_data[10:0];
        if (wr_y7)
            y7_reg <= wr_data[10:0];
        if (wr_x8)
            x8_reg <= wr_data[10:0];
        if (wr_y8)
            y8_reg <= wr_data[10:0];
        if (wr_x9)
            x9_reg <= wr_data[10:0];
        if (wr_y9)
            y9_reg <= wr_data[10:0];
        if (wr_x10)
            x10_reg <= wr_data[10:0];
        if (wr_y10)
            y10_reg <= wr_data[10:0];
        if (wr_x11)
            x11_reg <= wr_data[10:0];
        if (wr_y11)
            y11_reg <= wr_data[10:0];
        if (wr_x12)
            x12_reg <= wr_data[10:0];
        if (wr_y12)
            y12_reg <= wr_data[10:0];
        if (wr_x13)
            x13_reg <= wr_data[10:0];
        if (wr_y13)
            y13_reg <= wr_data[10:0];
        if (wr_x14)
            x14_reg <= wr_data[10:0];
        if (wr_y14)
            y14_reg <= wr_data[10:0];
        if (wr_x15)
            x15_reg <= wr_data[10:0];
        if (wr_y15)
            y15_reg <= wr_data[10:0];
        if (wr_x16)
            x16_reg <= wr_data[10:0];
        if (wr_y16)
            y16_reg <= wr_data[10:0];
        if (wr_x17)
            x17_reg <= wr_data[10:0];
        if (wr_y17)
            y17_reg <= wr_data[10:0];
        if (wr_x18)
            x18_reg <= wr_data[10:0];
        if (wr_y18)
            y18_reg <= wr_data[10:0];
        if (wr_x19)
            x19_reg <= wr_data[10:0];
        if (wr_y19)
            y19_reg <= wr_data[10:0];
         if (wr_bypass)
            bypass_reg <= wr_data[0];
        if (wr_ctrl_0)
            ctrl_0_reg <= wr_data[3:0];
        if (wr_ctrl_1)
            ctrl_1_reg <= wr_data[3:0];
        if (wr_ctrl_2)
            ctrl_2_reg <= wr_data[3:0];
        if (wr_ctrl_3)
            ctrl_3_reg <= wr_data[3:0];
        if (wr_ctrl_4)
            ctrl_4_reg <= wr_data[3:0];
        if (wr_ctrl_5)
            ctrl_5_reg <= wr_data[3:0];
        if (wr_ctrl_6)
            ctrl_6_reg <= wr_data[3:0];
        if (wr_ctrl_7)
            ctrl_7_reg <= wr_data[3:0];
        if (wr_ctrl_8)
            ctrl_8_reg <= wr_data[3:0];
        if (wr_ctrl_9)
            ctrl_9_reg <= wr_data[3:0];
        if (wr_ctrl_10)
            ctrl_10_reg <= wr_data[3:0];
        if (wr_ctrl_11)
            ctrl_11_reg <= wr_data[3:0];
        if (wr_ctrl_12)
            ctrl_12_reg <= wr_data[3:0];
        if (wr_ctrl_13)
            ctrl_13_reg <= wr_data[3:0];
        if (wr_ctrl_14)
            ctrl_14_reg <= wr_data[3:0];
        if (wr_ctrl_15)
            ctrl_15_reg <= wr_data[3:0];
        if (wr_ctrl_16)
            ctrl_16_reg <= wr_data[3:0];
        if (wr_ctrl_17)
            ctrl_17_reg <= wr_data[3:0];
        if (wr_ctrl_18)
            ctrl_18_reg <= wr_data[3:0];
        if (wr_ctrl_19)
            ctrl_19_reg <= wr_data[3:0];
      end      
   // decoding 
    assign wr_en = write & cs;
    assign wr_ram = ~addr[13] && wr_en;
    assign wr_reg = addr[13] && wr_en;
    assign wr_bypass = wr_reg && (addr[5:0]==6'b000000);
    assign wr_x0 = wr_reg && (addr[5:0]==1);
    assign wr_y0 = wr_reg && (addr[5:0]==2);
    assign wr_ctrl_0 = wr_reg && (addr[5:0]==3);
    assign wr_x1 = wr_reg && (addr[5:0]==4);
    assign wr_y1 = wr_reg && (addr[5:0]==5);
    assign wr_ctrl_1 = wr_reg && (addr[5:0]==6);
    assign wr_x2 = wr_reg && (addr[5:0]==7);
    assign wr_y2 = wr_reg && (addr[5:0]==8);
    assign wr_ctrl_2 = wr_reg && (addr[5:0]==9);
    assign wr_x3 = wr_reg && (addr[5:0]==10);
    assign wr_y3 = wr_reg && (addr[5:0]==11);
    assign wr_ctrl_3 = wr_reg && (addr[5:0]==12);
    assign wr_x4 = wr_reg && (addr[5:0]==13);
    assign wr_y4 = wr_reg && (addr[5:0]==14);
    assign wr_ctrl_4 = wr_reg && (addr[5:0]==15);
    assign wr_x5 = wr_reg && (addr[5:0]==16);
    assign wr_y5 = wr_reg && (addr[5:0]==17);
    assign wr_ctrl_5 = wr_reg && (addr[5:0]==18);
    assign wr_x6 = wr_reg && (addr[5:0]==19);
    assign wr_y6 = wr_reg && (addr[5:0]==20);
    assign wr_ctrl_6 = wr_reg && (addr[5:0]==21);
    assign wr_x7 = wr_reg && (addr[5:0]==22);
    assign wr_y7 = wr_reg && (addr[5:0]==23);
    assign wr_ctrl_7 = wr_reg && (addr[5:0]==24);
    assign wr_x8 = wr_reg && (addr[5:0]==25);
    assign wr_y8 = wr_reg && (addr[5:0]==26);
    assign wr_ctrl_8 = wr_reg && (addr[5:0]==27);
    assign wr_x9 = wr_reg && (addr[5:0]==28);
    assign wr_y9 = wr_reg && (addr[5:0]==29);
    assign wr_ctrl_9 = wr_reg && (addr[5:0]==30);
    assign wr_x10 = wr_reg && (addr[5:0]==31);
    assign wr_y10 = wr_reg && (addr[5:0]==32);
    assign wr_ctrl_10 = wr_reg && (addr[5:0]==33);
    assign wr_x11 = wr_reg && (addr[5:0]==34);
    assign wr_y11 = wr_reg && (addr[5:0]==35);
    assign wr_ctrl_11 = wr_reg && (addr[5:0]==36);
    assign wr_x12 = wr_reg && (addr[5:0]==37);
    assign wr_y12 = wr_reg && (addr[5:0]==38);
    assign wr_ctrl_12 = wr_reg && (addr[5:0]==39);
    assign wr_x13 = wr_reg && (addr[5:0]==40);
    assign wr_y13 = wr_reg && (addr[5:0]==41);
    assign wr_ctrl_13 = wr_reg && (addr[5:0]==42);
    assign wr_x14 = wr_reg && (addr[5:0]==43);
    assign wr_y14 = wr_reg && (addr[5:0]==44);
    assign wr_ctrl_14 = wr_reg && (addr[5:0]==45);
    assign wr_x15 = wr_reg && (addr[5:0]==46);
    assign wr_y15 = wr_reg && (addr[5:0]==47);
    assign wr_ctrl_15 = wr_reg && (addr[5:0]==48);
    assign wr_x16 = wr_reg && (addr[5:0]==49);
    assign wr_y16 = wr_reg && (addr[5:0]==50);
    assign wr_ctrl_16 = wr_reg && (addr[5:0]==51);
    assign wr_x17 = wr_reg && (addr[5:0]==52);
    assign wr_y17 = wr_reg && (addr[5:0]==53);
    assign wr_ctrl_17 = wr_reg && (addr[5:0]==54);
    assign wr_x18 = wr_reg && (addr[5:0]==55);
    assign wr_y18 = wr_reg && (addr[5:0]==56);
    assign wr_ctrl_18 = wr_reg && (addr[5:0]==57);
    assign wr_x19 = wr_reg && (addr[5:0]==58);
    assign wr_y19 = wr_reg && (addr[5:0]==59);
    assign wr_ctrl_19 = wr_reg && (addr[5:0]==60);
   // chrome-key blending and multiplexing
   assign chrom_rgb = (sprite_rgb != KEY_COLOR) ? sprite_rgb : si_rgb;
   assign so_rgb = (bypass_reg) ? si_rgb : chrom_rgb;
endmodule