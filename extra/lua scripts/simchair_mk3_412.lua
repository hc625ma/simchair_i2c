dataref("ab412_idle_stop_1", "412/engine/switches/idle_stop_1", "writable")
dataref("ab412_idle_stop_2", "412/engine/switches/idle_stop_2", "writable")
dataref("ab412_engine_start", "412/engine/switches/engine_start", "writable")

function b412_idle_stop_diseng()
	ab412_idle_stop_1 = 0
	ab412_idle_stop_2 = 0
	return
end


create_command("Simchair MKIII/412/idle_stop_1", "idle_stop_1", "ab412_idle_stop_1 = 1", "","")
create_command("Simchair MKIII/412/idle_stop_disengage", "idle_stop_disengage", "b412_idle_stop_diseng()", "","")
create_command("Simchair MKIII/412/idle_stop_2", "idle_stop_2", "ab412_idle_stop_2 = 1","","")
create_command("Simchair MKIII/412/start_eng_1", "start_eng_1", "ab412_engine_start = -1","","")
create_command("Simchair MKIII/412/start_eng_2", "start_eng_2", "ab412_engine_start = 1","","")
create_command("Simchair MKIII/412/starter_disengage", "starter_disengage", "ab412_engine_start = 0","","")


	