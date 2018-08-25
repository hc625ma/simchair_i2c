if (AIRCRAFT_FILENAME == 'Bell412.acf') then
	idle_sw_1 = 0
	idle_sw_2 = 0
	starter1_sw = 0
	starter2_sw = 0
	dataref("ab412_idle_stop_1", "412/engine/switches/idle_stop_1", "writable")
	dataref("ab412_idle_stop_2", "412/engine/switches/idle_stop_2", "writable")
	
	dataref("ab412_engine_start", "412/engine/switches/engine_start", "writable")


	function idle_stop()
		if (KEY_ACTION=="pressed") then 
			if (VKEY==85) then  -- y
				idle_sw_1 = 1 - idle_sw_1
				ab412_idle_stop_1 = idle_sw_1				
			elseif (VKEY==89) then --u
				idle_sw_2 = 1 - idle_sw_2
				ab412_idle_stop_2 = idle_sw_2
			end	 
			RESUME_KEY = true
		end
	end
	
	function engine_start()
		if (KEY_ACTION=="pressed") then 
			if (VKEY==79) then -- i
				starter1_sw = 1 - starter1_sw
				ab412_engine_start = starter1_sw
			elseif (VKEY==73) then -- o
				starter2_sw = -1 * ((starter2_sw + 1) % 2)
				ab412_engine_start = starter2_sw
			end	 
			RESUME_KEY = true
		end
	end

	do_on_keystroke("idle_stop()")
	do_on_keystroke("engine_start()")

end
