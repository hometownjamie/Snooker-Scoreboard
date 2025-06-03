#include <gtk/gtk.h>
#include <ctype.h>
#include <string.h>

#define SSB_H
#define MAXPLAYERS 200

// int project_changed;
/*
void push_button(cairo_t *, int origin_x, int origin_y, int butn_width, int butn_height,
	int butn_color, int frame_color, char *butn_txt, int font, int font_color, int txt_halign,
	int txt_valign); */

typedef struct  {
  int singles;  //  singles = 1, doubles = 2
  int frames_per_match;
  int match_time; //  in minutes
  int noreds;
} DEFAULTS;

void defaults_main(void);
void set_defaults(int singles, int frames_per_match, int match_time);
int get_default_noreds(void);
int get_default_match_time(void);
int get_default_frames_per_match(void);
void load_defaults(void);
void match_players_main(void);
int get_match_singles(void);
void set_selplayer(char *name, int playerno);
void set_player_to_select(int playerno);
int pl_main(int featurebits);
void play_main(void);
void new_match(void);
void new_frame(void);
void fixscore_main(void);
void get_progdir(char *target);
void about_box(void);

int strnicmp(char *str1, char *str2);
void trim (char *instr);
void ltrim(char *instr);
void rtrim(char *instr);

long rgb_2_long(int r, int g, int b);
long gdkrgb_2_long(double dr, double dg, double db);
void long_2_gdkrgb(long longval, double *dr, double *dg, double *db);
void long_2_rgb(long longval, int *r, int *g, int *b);
void contrast_colour(GdkRGBA *target, GdkRGBA *source);
gboolean yes_no_box(char *dlog_title, const char *question);

// GtkWidget	*window_1, *win;
