/* See LICENSE file for copyright and license details. */

/* include for media keys*/
#include <X11/XF86keysym.h>
#include <unistd.h>

/* appearance */
static unsigned int borderpx  = 3;        /* border pixel of windows */
static const Gap default_gap        = {.isgap = 1, .realgap = 10, .gappx = 25};
static unsigned int snap      = 30;       /* snap pixel */
static int showbar            = 1;        /* 0 means no bar */
static int topbar             = 0;        /* 0 means bottom bar */
static const char *fonts[]          = {
	"Fantasque Sans Mono:pixelsize=16",
	"Noto Sans Arabic:pixelsize=14",
};
static const char dmenufont[]       = "FantasqueSansMonoNerdFont:size=12";

/* standard color scheme (xresources-driven) */
static char normbgcolor[]     = "#282828";
static char normbordercolor[] = "#a89984";   /* <-- fixed: was "normbodercolor" */
static char normfgcolor[]     = "#ebdbb2";
static char selfgcolor[]      = "#504945";
static char selbordercolor[]  = "#689d6a";
static char selbgcolor[]      = "#d65d0e";

/* extra colors used by the extended color schemes (xresources-driven) */
static char col_gray1[]  = "#282828";   /* dark bg */
static char col_gray2[]  = "#a89984";   /* border / mid gray */
static char col_gray3[]  = "#ebdbb2";   /* normal fg */
static char col_gray4[]  = "#fbf1c7";   /* bright fg */
static char col_gray5[]  = "#7c6f64";   /* dim gray */
static char col_orange[] = "#fe8019";   /* accent */

static char *colors[][3]      = {
    /*               fg         bg         border   */
    [SchemeNorm]    = { col_gray2, col_gray1, col_gray4  },
    [SchemeSel]     = { col_gray2, col_gray1, col_gray2  },
    [SchemeTag]     = { col_gray2, col_gray4, col_gray2  },
    [SchemeSelT]    = { col_gray3, col_gray4, col_gray1  },
    [SchemeMode]    = { col_gray1, col_gray2, col_orange },
    [SchemeStatus]  = { col_gray2, col_gray1, col_gray5  },
    [SchemeStatusA] = { col_gray5, col_gray1, col_gray1  },
    [SchemeModeA]   = { col_gray2, col_gray4, col_gray4  },
    [SchemeA]       = { col_gray4, col_gray1, col_gray5  },
    [SchemeSelSA]   = { col_gray4, col_gray1, col_gray1  },
    [SchemeArrow1]  = { col_orange, col_gray2, col_gray2 },
    [SchemeArrow2]  = { col_gray2, col_gray4, col_gray4  },
    [SchemeArrow3]  = { col_gray4, col_gray1, col_gray1  },
};

/* volume fnct */
static const char *upvol[]   = {
	"/usr/bin/pactl", "set-sink-volume", "@DEFAULT_SINK@", "+5%",     NULL
};
static const char *downvol[] = {
	"/usr/bin/pactl", "set-sink-volume", "@DEFAULT_SINK@", "-5%",     NULL
};
static const char *mutevol[] = {
	"/usr/bin/pactl", "set-sink-mute", "@DEFAULT_SINK@", "toggle",     NULL
};

/* tagging */
static const char *tags[] = {
	"dev", "web", "app"
};

/* ytb scripts */
static const char *ytplay[]     = { "yt-clip", "play", NULL };
static const char *ytdownload[] = { "yt-clip", "download", NULL };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",		 NULL,       NULL,       1<<2,				0,           -1 },
	{ "Chromium",	NULL,       NULL,       1<<1,				0,           -1 },
	{ "Surf",		NULL,       NULL,       0,				0,           -1 },
	{ "Firefox",	NULL,       NULL,       1<<1,				0,           -1 },

};

/* layout(s) */
static float mfact     = 0.50; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "BASIC",      tile },  /* first entry is default */
	{ "STACK",      monocle },  
	{ "FLOAT",      NULL },    /* no layout function means floating behavior */
	{ NULL,		  NULL},
};

/* key definitions */
//#define MODKEY Mod4Mask
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = {
	"dmenu_run", "-m", dmenumon, NULL
};
static const char *tmic[] = { "amixer", "-D", "pulse", "sset", 
	"Capture", "toggle", NULL };
static const char *downbright[] = { "brightnessctl", "set", "5%-", NULL };
static const char *upbright[] = { "brightnessctl", "set", "5%+", NULL };
static const char *termcmd[] = { "st", "-e", "tmux", NULL };
static const char *termst[] = { "st",  NULL };
static const char *quitcmd[] = { "powerChoice", NULL };
static const char *slock[] = { "slock", NULL };
static const char *dmenubcmd[] = { "dmenuB", NULL };

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		{ "normbgcolor",        STRING,  &normbgcolor },
		{ "normbordercolor",    STRING,  &normbordercolor },
		{ "normfgcolor",        STRING,  &normfgcolor },
		{ "selbgcolor",         STRING,  &selbgcolor },
		{ "selbordercolor",     STRING,  &selbordercolor },
		{ "selfgcolor",         STRING,  &selfgcolor },
		{ "col_gray1",          STRING,  &col_gray1 },
		{ "col_gray2",          STRING,  &col_gray2 },
		{ "col_gray3",          STRING,  &col_gray3 },
		{ "col_gray4",          STRING,  &col_gray4 },
		{ "col_gray5",          STRING,  &col_gray5 },
		{ "col_orange",         STRING,  &col_orange },
		{ "borderpx",			INTEGER, &borderpx },
		{ "snap",				INTEGER, &snap },
		{ "showbar",			INTEGER, &showbar },
		{ "topbar",				INTEGER, &topbar },
		{ "nmaster",			INTEGER, &nmaster },
		{ "resizehints",		INTEGER, &resizehints },
		{ "mfact",				FLOAT,   &mfact },
};

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_t,		spawn,          {.v = termcmd } },
	{ MODKEY|ShiftMask,             XK_t,		spawn,          {.v = termst } },
	{ MODKEY|ShiftMask,             XK_d,		spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_d,		spawn,          {.v = dmenubcmd } },
	{ MODKEY,						XK_w,		spawn,			{.v = ytplay} },
	{ MODKEY|ShiftMask,				XK_w,		spawn,			{.v = ytdownload} },
	{ MODKEY,						XK_r,		reload_xresources, {0} },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_k,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_j,      focusstack,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_j,      setmfact,       {.f = -0.05} },
	{ MODKEY|ShiftMask,             XK_k,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_s,      togglesticky,   {0} },
	{ MODKEY,                       XK_z,      zoom,           {0} },
	{ MODKEY,                       XK_n,      shiftview,      {.i = +1} },
	{ MODKEY,                       XK_p,      shiftview,      {.i = -1} },
	{ MODKEY,                       XK_l,      spawn,          {.v = slock} },
	{ MODKEY,                       XK_q,      killclient,     {0} },
	{ MODKEY|ShiftMask,             XK_q,      spawn,          {.v = quitcmd } },
	{ MODKEY,                       XK_a,      togglealwaysontop,{0} },
	{ MODKEY,						XK_m,      cyclelayout,    {.i = -1} },
	{ MODKEY,                       XK_space,  togglefloating, {0} },
    { MODKEY,                       XK_minus,  setgaps,        {.i = -5 } },
    { MODKEY,                       XK_equal,  setgaps,        {.i = +5 } },
    { MODKEY|ShiftMask,             XK_minus,  setgaps,        {.i = GAP_RESET } },
    { MODKEY|ShiftMask,             XK_equal,  setgaps,        {.i = GAP_TOGGLE} },

	{ 0,                            XF86XK_AudioLowerVolume,   spawn,
		{.v = downvol } },
	{ 0,                            XF86XK_AudioMute,          spawn,
		{.v = mutevol } },
	{ 0,                            XF86XK_AudioRaiseVolume,   spawn,
		{.v = upvol   } },
	{ 0,							XF86XK_MonBrightnessUp,    spawn,
		{.v = upbright } },
	{ 0,                            XF86XK_MonBrightnessDown,  spawn,
		{.v = downbright } },
	{ 0,							XF86XK_AudioMicMute,       spawn,
		{.v = tmic } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
};

static Button buttons[] = {
    /* click                event mask      button          function        argument */
    { ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
    { ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
};
