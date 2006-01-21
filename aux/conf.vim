" rc syntax highlighting

syn keyword stdhapiValues true false on off yes no LOG_DEBUG LOG_INFO LOG_NOTICE LOG_WARNING LOG_ERROR LOG_CVSHEADER LOG_SQL BRIGHTMAGENTA MySQL PostgreSQL SQLite null none FLOW_CONTROL_HARDWARE SOFTWARE_CONTROL_SOFTWARE STOP_BITS_1 STOP_BITS_2 PARITY_CHECK PARITY_ODD CANONICAL ECHO CR2NL BITS_PER_BYTE_8 BITS_PER_BYTE_7 BITS_PER_BYTE_6 BITS_PER_BYTE_5
syn keyword stdhapiKeyword log_mask esc_delay disable_XON use_mouse latency command_compose_character command_compose_delay attribute_disabled attribute_enabled attribute_focused attribute_statusbar attribute_search_highlight ignore_signal_SIGINT ignore_signal_SIGTSTP ignore_signal_SIGQUIT serial_device serial_baudrate collector_connection_timeout data_base_driver instance_name serial_flags
syn keyword stdhapiCommand set_env
syn keyword stdhapiColorRed RED
syn keyword stdhapiColorGreen GREEN
syn keyword stdhapiColorBrown BROWN
syn keyword stdhapiColorBlue BLUE
syn keyword stdhapiColorMagenta MAGENTA
syn keyword stdhapiColorCyan CYAN
syn keyword stdhapiColorLightgray LIGHTGRAY
syn keyword stdhapiColorGray GRAY
syn keyword stdhapiColorBrightRed BRIGHTRED
syn keyword stdhapiColorBrightGreen BRIGHTGREEN
syn keyword stdhapiColorYellow YELLOW
syn keyword stdhapiColorBrightBlue BRIGHTBLUE
"syn keyword stdhapiColorBrightMagenta BRIGHTMAGENTA
syn keyword stdhapiColorBrightCyan BRIGHTCYAN
syn keyword stdhapiColorWhite WHITE
syn keyword stdhapiColorBlack BLACK
syn match stdhapiSection	/^\[.*\]$/
syn match stdhapiValues	/\<[0-9]\+\>/
syn match stdhapiValues	/B[0-9]\+/
syn match stdhapiKeyword /\<leave_ctrl_\([cqsz]\>\|\\\)/
hi stdhapiValues cterm=NONE ctermfg=magenta gui=NONE guifg=magenta
hi stdhapiCommand cterm=NONE ctermfg=yellow gui=NONE guifg=yellow
hi stdhapiKeyword cterm=NONE ctermfg=green gui=NONE guifg=green
hi stdhapiSection cterm=NONE ctermfg=red gui=NONE guifg=red
hi stdhapiColorRed cterm=NONE ctermfg=darkred gui=NONE guifg=darkred
hi stdhapiColorGreen cterm=NONE ctermfg=darkgreen gui=NONE guifg=darkgreen
hi stdhapiColorBrown cterm=NONE ctermfg=brown gui=NONE guifg=brown
hi stdhapiColorBlue cterm=NONE ctermfg=darkblue gui=NONE guifg=darkblue
hi stdhapiColorMagenta cterm=NONE ctermfg=darkmagenta gui=NONE guifg=darkmagenta
hi stdhapiColorCyan cterm=NONE ctermfg=darkcyan gui=NONE guifg=darkcyan
hi stdhapiColorLightgray cterm=NONE ctermfg=gray gui=NONE guifg=gray
hi stdhapiColorGray cterm=NONE ctermfg=darkgray gui=NONE guifg=darkgray
hi stdhapiColorBrightRed cterm=NONE ctermfg=red gui=NONE guifg=red
hi stdhapiColorBrightGreen cterm=NONE ctermfg=green gui=NONE guifg=green
hi stdhapiColorYellow cterm=NONE ctermfg=yellow gui=NONE guifg=yellow
hi stdhapiColorBrightBlue cterm=NONE ctermfg=blue gui=NONE guifg=blue
hi stdhapiColorBrightCyan cterm=NONE ctermfg=cyan gui=NONE guifg=cyan
hi stdhapiColorWhite cterm=NONE ctermfg=white gui=NONE guifg=white
hi stdhapiColorBlack cterm=NONE ctermfg=black ctermbg=gray gui=NONE guifg=black guibg=gray

