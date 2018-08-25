function keystroke_sniffer_event_handler()
		if SHIFT_KEY then
			keystroke_debug_string = "Key no. " .. VKEY .. " was pressed (shifted, "
		else
			keystroke_debug_string = "Key no. " .. VKEY .. " was pressed (unshifted, "
		end
		if OPTION_KEY then
			keystroke_debug_string = keystroke_debug_string .. "with option key down and "
		else
			keystroke_debug_string = keystroke_debug_string .. "with option key up and "
		end
		if CONTROL_KEY then
			keystroke_debug_string = keystroke_debug_string .. "control key down), KEY_ACTION = " .. KEY_ACTION
		else
			keystroke_debug_string = keystroke_debug_string .. "control key up), KEY_ACTION = " .. KEY_ACTION
		end
		RESUME_KEY = true
end

do_on_keystroke("keystroke_sniffer_event_handler()")

function print_keystroke_sniffer_result()
		draw_string(200, 300, keystroke_debug_string)
end

do_every_draw("print_keystroke_sniffer_result()")
