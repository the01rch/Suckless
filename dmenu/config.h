/* appearance */
static int topbar = 1;
static char *fonts[] = { "Fantasque Sans Mono:pixelsize=16" };   /* typo fixed */
static const char *prompt = NULL;

static char *colors[SchemeLast][2] = {
    /*     fg          bg          */
    [SchemeNorm] = { "#3c3836", "#fbf1c7" },
    [SchemeSel]  = { "#fbf1c7", "#076678" },
    [SchemeOut]  = { "#fbf1c7", "#af3a03" },
    [SchemeIn]   = { "#af3a03", "#fbf1c7" },
	[SchemeInput]  = { "#3c3836", "#fbf1c7" },   // input text: light on black
};

/* Xresources — keys must match what `xrdb -query` prints */
static XResPref resources[] = {
    { "dmenu.font",   STRING, &fonts[0]              },

    { "dmenu.normfg", STRING, &colors[SchemeNorm][0] },
    { "dmenu.normbg", STRING, &colors[SchemeNorm][1] },

    { "dmenu.selfg",  STRING, &colors[SchemeSel][0]  },
    { "dmenu.selbg",  STRING, &colors[SchemeSel][1]  },

    { "dmenu.infg",    STRING, &colors[SchemeIn][0] },      // powerline fg
    { "dmenu.inbg",    STRING, &colors[SchemeIn][1] },      // powerline bg

    { "dmenu.outfg",    STRING, &colors[SchemeOut][0] },      // powerline fg
    { "dmenu.outbg",    STRING, &colors[SchemeOut][1] },      // powerline bg

    { "dmenu.inputfg", STRING, &colors[SchemeInput][0] },   // input fg
    { "dmenu.inputbg", STRING, &colors[SchemeInput][1] },   // input bg
    /* add the other keys only if your theme files define them */
};

static unsigned int lines = 0;
static const char worddelimiters[] = " ";
