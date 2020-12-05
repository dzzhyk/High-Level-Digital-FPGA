set C_TypeInfoList {{ 
"my_engine" : [[], { "return": [[], "void"]} , [{"ExternC" : 0}], [ {"img": [[], {"array": ["0", [1024]]}] }, {"weight": [[], {"array": ["0", [144]]}] }, {"out": [[], {"array": ["1", [9]]}] }],[],""], 
"1": [ "dout_t", {"typedef": [[[],"2"],""]}], 
"2": [ "ap_int<24>", {"hls_type": {"ap_int": [[[[], {"scalar": { "int": 24}}]],""]}}], 
"0": [ "din_t", {"typedef": [[[],"3"],""]}], 
"3": [ "ap_int<8>", {"hls_type": {"ap_int": [[[[], {"scalar": { "int": 8}}]],""]}}]
}}
set moduleName my_engine
set isTaskLevelControl 1
set isCombinational 0
set isDatapathOnly 0
set isPipelined 0
set pipeline_type none
set FunctionProtocol ap_ctrl_hs
set isOneStateSeq 0
set ProfileFlag 0
set StallSigGenFlag 0
set isEnableWaveformDebug 1
set C_modelName {my_engine}
set C_modelType { void 0 }
set C_modelArgList {
	{ img_V int 8 regular {array 1024 { 1 1 } 1 1 }  }
	{ weight_V int 8 regular {array 144 { 1 3 } 1 1 }  }
	{ out_V int 24 regular {array 9 { 0 3 } 0 1 }  }
}
set C_modelArgMapList {[ 
	{ "Name" : "img_V", "interface" : "memory", "bitwidth" : 8, "direction" : "READONLY", "bitSlice":[{"low":0,"up":7,"cElement": [{"cName": "img.V","cData": "int8","bit_use": { "low": 0,"up": 7},"cArray": [{"low" : 0,"up" : 1023,"step" : 1}]}]}]} , 
 	{ "Name" : "weight_V", "interface" : "memory", "bitwidth" : 8, "direction" : "READONLY", "bitSlice":[{"low":0,"up":7,"cElement": [{"cName": "weight.V","cData": "int8","bit_use": { "low": 0,"up": 7},"cArray": [{"low" : 0,"up" : 143,"step" : 1}]}]}]} , 
 	{ "Name" : "out_V", "interface" : "memory", "bitwidth" : 24, "direction" : "WRITEONLY", "bitSlice":[{"low":0,"up":23,"cElement": [{"cName": "out.V","cData": "int24","bit_use": { "low": 0,"up": 23},"cArray": [{"low" : 0,"up" : 8,"step" : 1}]}]}]} ]}
# RTL Port declarations: 
set portNum 19
set portList { 
	{ ap_clk sc_in sc_logic 1 clock -1 } 
	{ ap_rst sc_in sc_logic 1 reset -1 active_high_sync } 
	{ ap_start sc_in sc_logic 1 start -1 } 
	{ ap_done sc_out sc_logic 1 predone -1 } 
	{ ap_idle sc_out sc_logic 1 done -1 } 
	{ ap_ready sc_out sc_logic 1 ready -1 } 
	{ img_V_address0 sc_out sc_lv 10 signal 0 } 
	{ img_V_ce0 sc_out sc_logic 1 signal 0 } 
	{ img_V_q0 sc_in sc_lv 8 signal 0 } 
	{ img_V_address1 sc_out sc_lv 10 signal 0 } 
	{ img_V_ce1 sc_out sc_logic 1 signal 0 } 
	{ img_V_q1 sc_in sc_lv 8 signal 0 } 
	{ weight_V_address0 sc_out sc_lv 8 signal 1 } 
	{ weight_V_ce0 sc_out sc_logic 1 signal 1 } 
	{ weight_V_q0 sc_in sc_lv 8 signal 1 } 
	{ out_V_address0 sc_out sc_lv 4 signal 2 } 
	{ out_V_ce0 sc_out sc_logic 1 signal 2 } 
	{ out_V_we0 sc_out sc_logic 1 signal 2 } 
	{ out_V_d0 sc_out sc_lv 24 signal 2 } 
}
set NewPortList {[ 
	{ "name": "ap_clk", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "clock", "bundle":{"name": "ap_clk", "role": "default" }} , 
 	{ "name": "ap_rst", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "reset", "bundle":{"name": "ap_rst", "role": "default" }} , 
 	{ "name": "ap_start", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "start", "bundle":{"name": "ap_start", "role": "default" }} , 
 	{ "name": "ap_done", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "predone", "bundle":{"name": "ap_done", "role": "default" }} , 
 	{ "name": "ap_idle", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "done", "bundle":{"name": "ap_idle", "role": "default" }} , 
 	{ "name": "ap_ready", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "ready", "bundle":{"name": "ap_ready", "role": "default" }} , 
 	{ "name": "img_V_address0", "direction": "out", "datatype": "sc_lv", "bitwidth":10, "type": "signal", "bundle":{"name": "img_V", "role": "address0" }} , 
 	{ "name": "img_V_ce0", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "img_V", "role": "ce0" }} , 
 	{ "name": "img_V_q0", "direction": "in", "datatype": "sc_lv", "bitwidth":8, "type": "signal", "bundle":{"name": "img_V", "role": "q0" }} , 
 	{ "name": "img_V_address1", "direction": "out", "datatype": "sc_lv", "bitwidth":10, "type": "signal", "bundle":{"name": "img_V", "role": "address1" }} , 
 	{ "name": "img_V_ce1", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "img_V", "role": "ce1" }} , 
 	{ "name": "img_V_q1", "direction": "in", "datatype": "sc_lv", "bitwidth":8, "type": "signal", "bundle":{"name": "img_V", "role": "q1" }} , 
 	{ "name": "weight_V_address0", "direction": "out", "datatype": "sc_lv", "bitwidth":8, "type": "signal", "bundle":{"name": "weight_V", "role": "address0" }} , 
 	{ "name": "weight_V_ce0", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "weight_V", "role": "ce0" }} , 
 	{ "name": "weight_V_q0", "direction": "in", "datatype": "sc_lv", "bitwidth":8, "type": "signal", "bundle":{"name": "weight_V", "role": "q0" }} , 
 	{ "name": "out_V_address0", "direction": "out", "datatype": "sc_lv", "bitwidth":4, "type": "signal", "bundle":{"name": "out_V", "role": "address0" }} , 
 	{ "name": "out_V_ce0", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "out_V", "role": "ce0" }} , 
 	{ "name": "out_V_we0", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "out_V", "role": "we0" }} , 
 	{ "name": "out_V_d0", "direction": "out", "datatype": "sc_lv", "bitwidth":24, "type": "signal", "bundle":{"name": "out_V", "role": "d0" }}  ]}

set RtlHierarchyInfo {[
	{"ID" : "0", "Level" : "0", "Path" : "`AUTOTB_DUT_INST", "Parent" : "", "Child" : ["1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13"],
		"CDFG" : "my_engine",
		"Protocol" : "ap_ctrl_hs",
		"ControlExist" : "1", "ap_start" : "1", "ap_ready" : "1", "ap_done" : "1", "ap_continue" : "0", "ap_idle" : "1",
		"Pipeline" : "None", "UnalignedPipeline" : "0", "RewindPipeline" : "0", "ProcessNetwork" : "0",
		"II" : "0",
		"VariableLatency" : "1", "ExactLatency" : "-1", "EstimateLatencyMin" : "14613", "EstimateLatencyMax" : "44309",
		"Combinational" : "0",
		"Datapath" : "0",
		"ClockEnable" : "0",
		"HasSubDataflow" : "0",
		"InDataflowNetwork" : "0",
		"HasNonBlockingOperation" : "0",
		"Port" : [
			{"Name" : "img_V", "Type" : "Memory", "Direction" : "I"},
			{"Name" : "weight_V", "Type" : "Memory", "Direction" : "I"},
			{"Name" : "out_V", "Type" : "Memory", "Direction" : "O"}]},
	{"ID" : "1", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.linebuf_val_0_V_U", "Parent" : "0"},
	{"ID" : "2", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.linebuf_val_1_V_U", "Parent" : "0"},
	{"ID" : "3", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.window_val_0_V_0_U", "Parent" : "0"},
	{"ID" : "4", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.window_val_0_V_1_U", "Parent" : "0"},
	{"ID" : "5", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.window_val_0_V_2_U", "Parent" : "0"},
	{"ID" : "6", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.window_val_1_V_0_U", "Parent" : "0"},
	{"ID" : "7", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.window_val_1_V_1_U", "Parent" : "0"},
	{"ID" : "8", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.window_val_1_V_2_U", "Parent" : "0"},
	{"ID" : "9", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.window_val_2_V_0_U", "Parent" : "0"},
	{"ID" : "10", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.window_val_2_V_1_U", "Parent" : "0"},
	{"ID" : "11", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.window_val_2_V_2_U", "Parent" : "0"},
	{"ID" : "12", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.new_pixel_V_U", "Parent" : "0"},
	{"ID" : "13", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.my_engine_mux_94_ncg_U1", "Parent" : "0"}]}


set ArgLastReadFirstWriteLatency {
	my_engine {
		img_V {Type I LastRead 9 FirstWrite -1}
		weight_V {Type I LastRead 11 FirstWrite -1}
		out_V {Type O LastRead -1 FirstWrite 8}}}

set hasDtUnsupportedChannel 0

set PerformanceInfo {[
	{"Name" : "Latency", "Min" : "14613", "Max" : "44309"}
	, {"Name" : "Interval", "Min" : "14614", "Max" : "44310"}
]}

set PipelineEnableSignalInfo {[
]}

set Spec2ImplPortList { 
	img_V { ap_memory {  { img_V_address0 mem_address 1 10 }  { img_V_ce0 mem_ce 1 1 }  { img_V_q0 mem_dout 0 8 }  { img_V_address1 mem_address 1 10 }  { img_V_ce1 mem_ce 1 1 }  { img_V_q1 mem_dout 0 8 } } }
	weight_V { ap_memory {  { weight_V_address0 mem_address 1 8 }  { weight_V_ce0 mem_ce 1 1 }  { weight_V_q0 mem_dout 0 8 } } }
	out_V { ap_memory {  { out_V_address0 mem_address 1 4 }  { out_V_ce0 mem_ce 1 1 }  { out_V_we0 mem_we 1 1 }  { out_V_d0 mem_din 1 24 } } }
}

set busDeadlockParameterList { 
}

# RTL port scheduling information:
set fifoSchedulingInfoList { 
}

# RTL bus port read request latency information:
set busReadReqLatencyList { 
}

# RTL bus port write response latency information:
set busWriteResLatencyList { 
}

# RTL array port load latency information:
set memoryLoadLatencyList { 
}
