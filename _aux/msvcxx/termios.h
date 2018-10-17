#ifndef YAAL_MSVCXX_TERMIOS_H_INCLUDED
#define YAAL_MSVCXX_TERMIOS_H_INCLUDED 1

typedef int long speed_t;
typedef int long unsigned tcflag_t;
typedef char unsigned cc_t;

static int const TCSANOW   = 0;
static int const TCSAFLUSH = 2;

static int const TCIFLUSH  = 1;
static int const TCOFLUSH  = 2;
static int const TCIOFLUSH = 3;

/* c_iflag */
static tcflag_t const IGNBRK     = 1 << 0;
static tcflag_t const BRKINT     = 1 << 1;
static tcflag_t const IGNPAR     = 1 << 2;
static tcflag_t const PARMARK    = 1 << 3;
static tcflag_t const INPCK      = 1 << 4;
static tcflag_t const ISTRIP     = 1 << 5;
static tcflag_t const ICRNL      = 1 << 8;
static tcflag_t const IXON       = 1 << 10;
static tcflag_t const IXANY      = 1 << 11;
static tcflag_t const IXOFF      = 1 << 12;
static tcflag_t const IMAXBEL    = 1 << 13;
static tcflag_t const W_INSERT   = 1 << 16;
static tcflag_t const W_MOUSE    = 1 << 17;
static tcflag_t const W_QUICK    = 1 << 18;
static tcflag_t const W_WININPUT = 1 << 19;
static tcflag_t const W_IVIRTTRM = 1 << 20;

/* c_oflag */
static tcflag_t const W_OPROCESS = 1 << 16;
static tcflag_t const W_WRAP_EOL = 1 << 17;
static tcflag_t const W_OVIRTTRM = 1 << 18;
static tcflag_t const W_DIS_NLCR = 1 << 18;
static tcflag_t const W_LVB_GRID = 1 << 20;

/* c_cflag */
static tcflag_t const CS5        = 0;
static tcflag_t const CS6        = 1 << 8;
static tcflag_t const CS7        = 1 << 9;
static tcflag_t const CS8        = CS6 | CS7;
static tcflag_t const CSIZE      = CS5 | CS6 | CS7 | CS8;
static tcflag_t const CSTOPB     = 1 << 10;
static tcflag_t const CREAD      = 1 << 11;
static tcflag_t const PARENB     = 1 << 12;
static tcflag_t const PARODD     = 1 << 13;
static tcflag_t const CCTS_OFLOW = 1 << 16;
static tcflag_t const CRTS_IFLOW = 1 << 17;
static tcflag_t const CRTSCTS    = CCTS_OFLOW | CRTS_IFLOW;

/* c_lflag */
static tcflag_t const ISIG   = 1 << 0;
static tcflag_t const ECHO   = 1 << 3;
static tcflag_t const ICANON = 1 << 8;
static tcflag_t const IEXTEN = 1 << 10;

static int const VEOF     = 0;
static int const VEOL     = 1;
static int const VEOL2    = 2;
static int const VERASE   = 3;
static int const VWERASE  = 4;
static int const VKILL    = 5;
static int const VREPRINT = 6;
static int const VINTR    = 8;
static int const VQUIT    = 9;
static int const VSUSP    = 10;
static int const VSTART   = 12;
static int const VSTOP    = 13;
static int const VLNEXT   = 14;
static int const VDISCARD = 15;
static int const VMIN     = 16;
static int const VTIME    = 17;

static speed_t const B230400 = 230400;
static speed_t const B115200 = 115200;
static speed_t const B57600  = 57600;
static speed_t const B38400  = 38400;
static speed_t const B19200  = 19200;
static speed_t const B9600   = 9600;
static speed_t const B4800   = 4800;
static speed_t const B2400   = 2400;

struct termios {
  tcflag_t c_iflag;
  tcflag_t c_oflag;
  tcflag_t c_cflag;
  tcflag_t c_lflag;
	static int const NCCS = 20;
  cc_t c_cc[NCCS];
};

int tcgetattr( int, struct termios* );
int cfsetospeed( struct termios*, speed_t );
int cfsetispeed( struct termios*, speed_t );
int tcsetattr( int, int, struct termios const* );
int tcflush( int, int );
int tcdrain( int );
int tcsendbreak( int, int );

#endif /* not YAAL_MSVCXX_TERMIOS_H_INCLUDED */

