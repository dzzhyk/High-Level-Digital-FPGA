############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
############################################################
open_project conv
set_top my_engine
add_files conv/conv.cpp
add_files conv/conv.h
add_files -tb conv/conv_test.cpp
open_solution "init"
set_part {xc7k160tlffv676-2l} -tool vivado
create_clock -period 10 -name default
#source "./conv/init/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -format ip_catalog
