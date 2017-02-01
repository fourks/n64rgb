//////////////////////////////////////////////////////////////////////////////////
// Company: Circuit-Board.de
// Engineer: borti4938
// (initial design file by Ikari_01)
//
// Module Name:    n64rgb
// Project Name:   n64rgb
// Target Devices: MaxII
// Tool versions:  Altera Quartus Prime
// Description:
//
// Dependencies:
//
// Revision: 4
// Additional Comments: BUFFERED version (no color shifting around edges)
//                      deactivation of de-blur if wanted
//                      15bit color mode (5bit for each color) if wanted
///////////////////////////////////////////////////////////////////////////////////////////


module n64rgb (
  input nCLK,
  input nDSYNC,
  input [6:0] D_i,

  input DBF_ENABLE,  // feature to enable de-blur (DBF = De-Blur-Feature, 0 = feature off, 1 = feature on)
                     // (pin can be left unconnected for always on; weak pull-up assigned)
  input n15bit_mode, // 15bit color mode if input set to GND (weak pull-up assigned)
  input [4:0] dummy, // some pins are tied to Vcc/GND according to viletims design

  output nVSYNC,
  output nCLAMP,
  output nHSYNC,
  output nCSYNC,

  output [6:0] R_o,     // red data vector
  output [6:0] G_o,     // green data vector
  output [6:0] B_o      // blue data vector
);


// short description of N64s RGB and sync data demux
// =================================================
//
// pulse shapes and their realtion to each other:
// nCLK (~50MHz, Numbers representing negedge count)
// ---. 3 .---. 0 .---. 1 .---. 2 .---. 3 .---
//    |___|   |___|   |___|   |___|   |___|
// nDSYNC (~12.5MHz)                           .....
// -------.       .-------------------.
//        |_______|                   |_______
//
// more info: http://members.optusnet.com.au/eviltim/n64rgb/n64rgb.html
//


// Part 1: estimation of 240p/288p
// ===============================

reg [2:0] serr_cnt = 3'b000; // 240p: 3 hsync pulses during vsync pulse ; 480i: 6 serrated hsync per vsync
reg       n64_480i = 1'b1;

always @(negedge nCLK) begin // estimation of blur effect
  if (~nDSYNC) begin
    if(&{~S_DBr[0][3],~S_DBr[0][0],D_i[0]}) // nVSYNC low and posedge at nCSYNC
      serr_cnt <= serr_cnt + 1'b1;          // count up hsync pulses during vsync pulse
                                            // serr_cnt[2]==1 means a value >=4 -> 480i mode detected

    if(~S_DBr[0][3] & D_i[3]) begin // posedge at nVSYNC detected - set n64_480i and reset serr_cnt
      n64_480i <= serr_cnt[2];
      serr_cnt <= 3'b000;
    end
  end
end


// Part 2: blanking management
// ===========================

reg [1:0] line_cnt;   // PAL: line_cnt[1:0] == 01 ; NTSC: line_cnt[1:0] = 11
reg       vmode;      // PAL: vmode == 1          ; NTSC: vmode == 0
reg       nblank_rgb; // blanking of RGB pixels for de-blur

always @(negedge nCLK) begin
  if (~nDSYNC) begin
    if(~S_DBr[0][3] & D_i[3]) begin // posedge at nVSYNC detected - reset line_cnt and set vmode
      line_cnt <= 2'b00;
      vmode    <= ~line_cnt[1];
    end

    if(~S_DBr[0][1] & D_i[1]) // posedge nHSYNC -> increase line_cnt
      line_cnt <= line_cnt + 1'b1;

    if(~n64_480i & DBF_ENABLE) begin // 240p and de-blur enabled
      if(~S_DBr[0][0] & D_i[0]) // posedge nCSYNC -> reset blanking
        nblank_rgb <= vmode;
      else
        nblank_rgb <= ~nblank_rgb;
    end else
      nblank_rgb <= 1'b1;
  end
end


// Part 3: data demux
// ==================

reg [1:0] data_cnt = 2'b00;

reg [3:0] S_DBr[0:2]; // sync data vector buffer: {nVSYNC, nCLAMP, nHSYNC, nCSYNC}
reg [6:0] R_DBr[0:1]; // red data vector buffer
reg [6:0] G_DBr[0:1]; // green data vector buffer
reg [6:0] B_DBr[0:1]; // blue data vector buffer


initial begin
  integer idx;
  for (idx=0; idx<2; idx=idx+1) begin
   S_DBr[idx] = 4'b1111;
   R_DBr[idx] = 7'b0000000;
   G_DBr[idx] = 7'b0000000;
   B_DBr[idx] = 7'b0000000;
  end
  S_DBr[2] = 4'b1111;
end


always @(negedge nCLK) begin // data register management
  if (~nDSYNC) begin
    // shift data to the left
    S_DBr[2] <= S_DBr[1];
    S_DBr[1] <= S_DBr[0];
    R_DBr[1] <= R_DBr[0];
    G_DBr[1] <= G_DBr[0];
    B_DBr[1] <= B_DBr[0];

    // get new sync data
    S_DBr[0] <= D_i[3:0];

    // reset data counter
    data_cnt <= 2'b01;
  end else if (nblank_rgb) begin // get RGB only if not blanked
    // demux of RGB
    case(data_cnt)
      2'b01: R_DBr[0] <= D_i;
      2'b10: G_DBr[0] <= D_i;
      2'b11: B_DBr[0] <= D_i;
    endcase

    // increment data counter
    data_cnt <= data_cnt + 1'b1;
  end
end

assign {nVSYNC, nCLAMP, nHSYNC, nCSYNC} = nDeBlur ? S_DBr[1] : S_DBr[2];
assign R_o = n15bit_mode ? R_DBr[1] : {R_DBr[1][6:2], 2'b00};
assign G_o = n15bit_mode ? G_DBr[1] : {G_DBr[1][6:2], 2'b00};
assign B_o = n15bit_mode ? B_DBr[1] : {B_DBr[1][6:2], 2'b00};

endmodule
