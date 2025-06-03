/*  main.c
    Copyright 2025 - Jamie Scott
    This software is free to use, distribute, and modify.
    Licensed under GPL 3
*/
#ifndef SSB_H
#include "ssb.h"
#endif
#include <libgen.h>

#define SELECT 1
#define AED 2

int xres, yres;
char player1[40], player2[40];
GtkWidget *window_1;
int rackno, time_remaining;
int player1_score, player2_score;
static char progdir[128];

GtkWidget *PlayNewMatchMi, *PlayNextFrameMi, *PlayResumeFrameMi, *PlayFixScoresMi;

static GtkWidget *vbox;
static GdkRGBA color;

void defaults_main(void);
void load_players(void);
void load_defaults(void);
int pl_main(int featurebits);
static void match_settings_activate(void);
static void match_players_activate(void);
static void new_match_activate(void);
static void next_frame_activate(void);
static void resume_frame_activate(void);
static void fix_scores_activate(void);
static void player_list_activate(void);
static void about_activate(void);
void on_cc1_color_set(GtkColorButton *ccb);
gboolean yes_no_box(char *dlog_title, const char *question);

void get_progdir(char *target)  {
  strcpy(target, progdir);
}

void on_cc1_color_set(GtkColorButton *ccb)	{
	gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER(ccb), &color);
}

static void about_activate(void)  {
  about_box();
}

static void match_players_activate(void) {
  match_players_main();
}

static void player_list_activate(void) {
  pl_main(AED);
}

static void match_settings_activate(void) {
  defaults_main();
}

static void new_match_activate(void) {
  new_match();
}

static void next_frame_activate(void) {
  new_frame();
}

static void resume_frame_activate(void) {
  play_main();
}

static void fix_scores_activate(void) {
  fixscore_main();
}

gboolean yes_no_box(char *dlog_title, const char *question)	{
	gboolean response;
	GtkWidget *dialog;

	dialog = gtk_message_dialog_new(GTK_WINDOW(window_1), GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "%s", question);
	gtk_window_set_title(GTK_WINDOW(dialog), dlog_title);
	response = gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	return(response);
}

int main(int argc, char *argv[])	{
  GtkWidget *menubar, *FileMenu, *FileMi, *FileQuitMi;
  GtkWidget *SetupMenu, *SetupMi, *SetupPlayersMi;
  GtkWidget *PlayMenu, *PlayMi, *PlayMatchSettingsMi, *PlayMatchPlayersMi;
  GtkWidget *HelpMenu, *HelpMi, *HelpAboutMi;
  int cntr, j;
  char temp[128];
  FILE *fp;

  if (argc == 3)  {
    xres = atoi(argv[1]);
    yres = atoi(argv[2]);
  }
  else  {  
    xres = 1280;
    yres = 800;
  }

  gtk_init(&argc, &argv);

  //  get the program directory
  system ("pwd > dirname 2> /dev/null");
  fp = fopen("dirname", "rb");
  fread(progdir, 1, 127, fp);
  fclose(fp);
  system ("rm dirname");
  progdir[strlen(progdir) - 1] = '\0';
  // printf("%s\n", progdir);
  strcat(progdir, argv[0]);
  // printf("%s\n", progdir);
  //  remove any '.' in the concatenated string
  for (cntr = j = 0; cntr < strlen(progdir); cntr++)  {
    if (progdir[cntr] != '.') {
      temp[j] = progdir[cntr];
      j++;
    }
  }
  temp[j] = '\0';
  strcpy(progdir, temp);
  // printf("%s\n", progdir);
  //  remove the last '/' and anything after that
  for (cntr = strlen(progdir); cntr > 0; cntr--)  {
    if (progdir[cntr] == '/') {
      progdir[cntr] = '\0';
      break;
    }
  }
  // printf("%s\n", progdir);
  load_players();
  load_defaults();

  window_1 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//  gtk_window_fullscreen(GTK_WINDOW(window_1));
  gtk_widget_set_size_request(window_1, 370, 210);
  gtk_window_set_title (GTK_WINDOW(window_1), "Snooker Scoreboard");
  gtk_window_set_deletable(GTK_WINDOW(window_1), TRUE);
  gtk_window_move(GTK_WINDOW(window_1), 200, 100);
  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(window_1), vbox);
  gtk_window_set_mnemonics_visible(GTK_WINDOW(window_1), TRUE);

  /* --------------- start of menu bar ----------------------*/
  menubar = gtk_menu_bar_new();
  FileMenu = gtk_menu_new();
    FileMi = gtk_menu_item_new_with_mnemonic("_File");
    FileQuitMi = gtk_menu_item_new_with_label("Quit");

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(FileMi), FileMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), FileMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(FileMenu), FileQuitMi);
  g_signal_connect(G_OBJECT(FileQuitMi), "activate", G_CALLBACK(gtk_main_quit), NULL);

  SetupMenu = gtk_menu_new();
    SetupMi = gtk_menu_item_new_with_label("Players");
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), SetupMi);
    SetupPlayersMi = gtk_menu_item_new_with_label("Player List");
    
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(SetupMi), SetupMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(SetupMenu), SetupPlayersMi);
  g_signal_connect(G_OBJECT(SetupPlayersMi), "activate", G_CALLBACK(player_list_activate), NULL);

  PlayMenu = gtk_menu_new();
    PlayMi = gtk_menu_item_new_with_mnemonic("_Play");
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), PlayMi);
    PlayMatchSettingsMi = gtk_menu_item_new_with_label("Match Settings");
    PlayMatchPlayersMi = gtk_menu_item_new_with_label("Match Players");
    PlayNewMatchMi = gtk_menu_item_new_with_label("Begin New Match");
    gtk_widget_set_sensitive(PlayNewMatchMi, FALSE);   
    PlayNextFrameMi = gtk_menu_item_new_with_label("Begin Next Frame");
    gtk_widget_set_sensitive(PlayNextFrameMi, FALSE);   
    PlayResumeFrameMi = gtk_menu_item_new_with_label("Resume Frame");
    gtk_widget_set_sensitive(PlayResumeFrameMi, FALSE);   
    PlayFixScoresMi = gtk_menu_item_new_with_label("Fix Scores");
    gtk_widget_set_sensitive(PlayFixScoresMi, FALSE);   
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(PlayMi), PlayMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(PlayMenu), PlayMatchSettingsMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(PlayMenu), PlayMatchPlayersMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(PlayMenu), PlayNewMatchMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(PlayMenu), PlayNextFrameMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(PlayMenu), PlayResumeFrameMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(PlayMenu), PlayFixScoresMi);
  g_signal_connect(G_OBJECT(PlayMatchSettingsMi), "activate", G_CALLBACK(match_settings_activate), NULL);
  g_signal_connect(G_OBJECT(PlayMatchPlayersMi), "activate", G_CALLBACK(match_players_activate), NULL);
  g_signal_connect(G_OBJECT(PlayNewMatchMi), "activate", G_CALLBACK(new_match_activate), NULL);
  g_signal_connect(G_OBJECT(PlayNextFrameMi), "activate", G_CALLBACK(next_frame_activate), NULL);
  g_signal_connect(G_OBJECT(PlayResumeFrameMi), "activate", G_CALLBACK(resume_frame_activate), NULL);
  g_signal_connect(G_OBJECT(PlayFixScoresMi), "activate", G_CALLBACK(fix_scores_activate), NULL);

  HelpMenu = gtk_menu_new();
  HelpMi = gtk_menu_item_new_with_label("Help");
    HelpAboutMi = gtk_menu_item_new_with_label("About");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(HelpMi), HelpMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(HelpMenu), HelpAboutMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), HelpMi);
    g_signal_connect(G_OBJECT(HelpAboutMi), "activate", G_CALLBACK(about_activate), NULL);

  gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(window_1), "destroy", G_CALLBACK(gtk_main_quit), NULL);
  /* ------------------- end of menu bar ---------------------*/

  gtk_widget_show_all(window_1);
  gtk_widget_show(window_1);

  gtk_main();
  return 0;
}



