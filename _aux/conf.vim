" rc syntax highlighting

syn keyword yaalValues true false on off yes no LOG_DEBUG LOG_INFO LOG_NOTICE LOG_WARNING LOG_ERROR LOG_VCSHEADER LOG_SQL BRIGHTMAGENTA MySQL PostgreSQL SQLite null none FLOW_CONTROL_HARDWARE SOFTWARE_CONTROL_SOFTWARE STOP_BITS_1 STOP_BITS_2 PARITY_CHECK PARITY_ODD CANONICAL ECHO CR2NL BITS_PER_BYTE_8 BITS_PER_BYTE_7 BITS_PER_BYTE_6 BITS_PER_BYTE_5
syn keyword yaalKeyword log_mask resolve_hostnames write_timeout ssl_key ssl_cert esc_delay disable_XON use_mouse latency command_compose_character command_compose_delay attribute_disabled attribute_enabled attribute_focused attribute_statusbar attribute_search_highlight ignore_signal_SIGINT ignore_signal_SIGTSTP ignore_signal_SIGQUIT serial_device serial_baudrate collector_connection_timeout data_base_driver instance_name serial_flags default_encoding compression_level compression_buffer_size
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
