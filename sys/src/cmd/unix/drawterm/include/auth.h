/*
 * This file is part of the UCB release of Plan 9. It is subject to the license
 * terms in the LICENSE file found in the top-level directory of this
 * distribution and at http://akaros.cs.berkeley.edu/files/Plan9License. No
 * part of the UCB release of Plan 9, including this file, may be copied,
 * modified, propagated, or distributed except according to the terms contained
 * in the LICENSE file.
 */

#ifdef PLAN9
#pragma	src	"/sys/src/libauth"
#pragma	lib	"libauth.a"
#endif

/*
 * Interface for typical callers.
 */

typedef struct	AuthInfo	AuthInfo;
typedef struct	Chalstate	Chalstate;
typedef struct	Chapreply	Chapreply;
typedef struct	MSchapreply	MSchapreply;
typedef struct	UserPasswd	UserPasswd;
typedef struct	AuthRpc		AuthRpc;

enum
{
	MAXCHLEN=	256,		/* max challenge length	*/
	AMAXNAMELEN=	256,		/* maximum name length */
	MD5LEN=		16,

	ARok = 0,			/* rpc return values */
	ARdone,
	ARerror,
	ARneedkey,
	ARbadkey,
	ARwritenext,
	ARtoosmall,
	ARtoobig,
	ARrpcfailure,
	ARphase,

	AuthRpcMax = 4096,
};

struct AuthRpc
{
	int afd;
	char ibuf[AuthRpcMax];
	char obuf[AuthRpcMax];
	char *arg;
	uint narg;
};

struct AuthInfo
{
	char	*cuid;		/* caller id */
	char	*suid;		/* server id */
	char	*cap;		/* capability (only valid on server side) */
	int	nsecret;	/* length of secret */
	uchar	*secret;	/* secret */
};

struct Chalstate
{
	char	*user;
	char	chal[MAXCHLEN];
	int	nchal;
	void	*resp;
	int	nresp;

/* for implementation only */
	int	afd;			/* to factotum */
	AuthRpc	*rpc;			/* to factotum */
	char	userbuf[AMAXNAMELEN];	/* temp space if needed */
	int	userinchal;		/* user was sent to obtain challenge */
};

struct	Chapreply		/* for protocol "chap" */
{
	uchar	id;
	char	resp[MD5LEN];
};

struct	MSchapreply	/* for protocol "mschap" */
{
	char	LMresp[24];		/* Lan Manager response */
	char	NTresp[24];		/* NT response */
};

struct	UserPasswd
{
	char	*user;
	char	*passwd;
};

extern	int	newns(char*, char*);
extern	int	addns(char*, char*);

extern	int	noworld(char*);
extern	int	amount(int, char*, int, char*);

/* these two may get generalized away -rsc */
extern	int	login(char*, char*, char*);
extern	int	httpauth(char*, char*);

typedef struct Attr Attr;
typedef struct String String;
enum {
	AttrNameval,		/* name=val -- when matching, must have name=val */
	AttrQuery,		/* name? -- when matching, must be present */
	AttrDefault,		/* name:=val -- when matching, if present must match INTERNAL */
};
struct Attr
{
	int type;
	Attr *next;
	char *name;
	char *val;
};

typedef int AuthGetkey(char*);

int	_attrfmt(Fmt*);
Attr	*_copyattr(Attr*);
Attr	*_delattr(Attr*, char*);
Attr	*_findattr(Attr*, char*);
void	_freeattr(Attr*);
Attr	*_mkattr(int, char*, char*, Attr*);
Attr	*_parseattr(char*);
char	*_strfindattr(Attr*, char*);
#ifdef VARARGCK
#pragma varargck type "A" Attr*
#endif

extern AuthInfo*	fauth_proxy(int, AuthRpc *rpc, AuthGetkey *getkey, char *params);
extern AuthInfo*	auth_proxy(int fd, AuthGetkey *getkey, char *fmt, ...);
extern int		auth_getkey(char*);
extern int		(*amount_getkey)(char*);
extern void		auth_freeAI(AuthInfo *ai);
extern int		auth_chuid(AuthInfo *ai, char *ns);
extern Chalstate	*auth_challenge(char*, ...);
extern AuthInfo*	auth_response(Chalstate*);
extern int		auth_respond(void*, uint, char*, uint, void*, uint, AuthGetkey *getkey, char*, ...);
extern void		auth_freechal(Chalstate*);
extern AuthInfo*	auth_userpasswd(char *user, char *passwd);
extern UserPasswd*	auth_getuserpasswd(AuthGetkey *getkey, char*, ...);
extern AuthInfo*	auth_getinfo(AuthRpc *rpc);
extern AuthRpc*		auth_allocrpc(int afd);
extern Attr*		auth_attr(AuthRpc *rpc);
extern void		auth_freerpc(AuthRpc *rpc);
extern uint		auth_rpc(AuthRpc *rpc, char *verb, void *a, int n);
extern int		auth_wep(char*, char*, ...);

#ifdef VARARGCK
#pragma varargck argpos auth_proxy 3
#pragma varargck argpos auth_challenge 1
#pragma varargck argpos auth_respond 3
#pragma varargck argpos auth_getuserpasswd 2
#endif