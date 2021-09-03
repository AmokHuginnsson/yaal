" rc syntax highlighting

syn keyword yaalValues true false on off yes no enable disable auto DEBUG INFO NOTICE WARNING ERROR CRITICAL ALERT EMERGENCY
syn keyword yaalValues BRIGHTMAGENTA MySQL PostgreSQL SQLite null none FLOW_CONTROL_HARDWARE FLOW_CONTROL_SOFTWARE
syn keyword yaalValues STOP_BITS_1 STOP_BITS_2 PARITY_CHECK PARITY_ODD CANONICAL ECHO CR2NL BITS_PER_BYTE_8 BITS_PER_BYTE_7 BITS_PER_BYTE_6 BITS_PER_BYTE_5
syn keyword yaalValues POSIX YAAL ABORT THROW FULL ABBREVIATED
syn keyword yaalKeyword attribute_disabled attribute_enabled attribute_focused attribute_search_highlight attribute_statusbar
syn keyword yaalKeyword child_kill_grace_period client_character_set command_compose_character
syn keyword yaalKeyword command_compose_delay compression_buffer_size compression_level data_base_driver default_encoding disable_XON
syn keyword yaalKeyword duration_unit_form esc_delay exception_logging huginn_max_call_stack_size ignore_signal_SIGINT ignore_signal_SIGQUIT ignore_signal_SIGTSTP
syn keyword yaalKeyword instance_name kill_grace_period latency log_level log_sql on_alloc_failure resolve_hostnames screen_background
syn keyword yaalKeyword serial_baudrate serial_device serial_flags ssl_cert ssl_key thread_stack_size
syn keyword yaalKeyword use_mouse write_timeout
syn keyword yaalCommand set_env
syn keyword yaalColorRed RED
syn keyword yaalColorGreen GREEN
syn keyword yaalColorBrown BROWN
syn keyword yaalColorBlue BLUE
syn keyword yaalColorMagenta MAGENTA
syn keyword yaalColorCyan CYAN
syn keyword yaalColorLightgray LIGHTGRAY
syn keyword yaalColorGray GRAY
syn keyword yaalColorBrightRed BRIGHTRED
syn keyword yaalColorBrightGreen BRIGHTGREEN
syn keyword yaalColorYellow YELLOW
syn keyword yaalColorBrightBlue BRIGHTBLUE
"syn keyword yaalColorBrightMagenta BRIGHTMAGENTA
syn keyword yaalColorBrightCyan BRIGHTCYAN
syn keyword yaalColorWhite WHITE
syn keyword yaalColorBlack BLACK
syn match yaalSection	/^\[.*\]$/
syn match yaalValues	/\<[0-9]\+\>/
syn match yaalValues	/B[0-9]\+/
syn match yaalKeyword /\<leave_ctrl_\([cqsz]\>\|\\\)/
syn match yaalVariables /\${[A-Za-z0-9_]\+}/ containedin=confString
syn match yaalDeclaration /[A-Za-z0-9_]\+=/me=e-1
hi yaalValues cterm=NONE ctermfg=magenta gui=NONE guifg=magenta
hi yaalCommand cterm=NONE ctermfg=yellow gui=NONE guifg=yellow
hi yaalKeyword cterm=NONE ctermfg=green gui=NONE guifg=green
hi yaalSection cterm=NONE ctermfg=red gui=NONE guifg=red
hi yaalColorRed cterm=NONE ctermfg=darkred gui=NONE guifg=darkred
hi yaalColorGreen cterm=NONE ctermfg=darkgreen gui=NONE guifg=darkgreen
hi yaalColorBrown cterm=NONE ctermfg=brown gui=NONE guifg=brown
hi yaalColorBlue cterm=NONE ctermfg=darkblue gui=NONE guifg=darkblue
hi yaalColorMagenta cterm=NONE ctermfg=darkmagenta gui=NONE guifg=darkmagenta
hi yaalColorCyan cterm=NONE ctermfg=darkcyan gui=NONE guifg=darkcyan
hi yaalColorLightgray cterm=NONE ctermfg=gray gui=NONE guifg=gray
hi yaalColorGray cterm=NONE ctermfg=darkgray gui=NONE guifg=darkgray
hi yaalColorBrightRed cterm=NONE ctermfg=red gui=NONE guifg=red
hi yaalColorBrightGreen cterm=NONE ctermfg=green gui=NONE guifg=green
hi yaalColorYellow cterm=NONE ctermfg=yellow gui=NONE guifg=yellow
hi yaalColorBrightBlue cterm=NONE ctermfg=blue gui=NONE guifg=blue
hi yaalColorBrightCyan cterm=NONE ctermfg=cyan gui=NONE guifg=cyan
hi yaalColorWhite cterm=NONE ctermfg=white gui=NONE guifg=white
hi yaalColorBlack cterm=NONE ctermfg=black ctermbg=gray gui=NONE guifg=black guibg=gray

hi link yaalVariables yaalColorBrightGreen
hi link yaalDeclaration yaalColorCyan
