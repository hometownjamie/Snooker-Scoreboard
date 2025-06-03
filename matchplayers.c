/*  matchplayers.c
    Copyright 2025 - Jamie Scott
    This software is free to use, distribute, and modify.
    Licensed under GPL 3
*/

#ifndef SSB_H
#include "ssb.h"
#endif

extern GtkWidget *PlayNewMatchMi;
extern GtkWidget *window_1;
extern int xres, yres;
char matchplayers[4][40];
static char selplayer[40];

static int singles;

static GtkWidget *labname1, *labname2, *labname3, *labname4;

static GtkWidget *win1;

void set_selplayer(char *name, int player_num_selected)  {
  strcpy(matchplayers[player_num_selected - 1], name);
  switch (player_num_selected)  {
    case 1:
      gtk_label_set_text(GTK_LABEL(labname1), matchplayers[0]);
      break;
    case 2:
      gtk_label_set_text(GTK_LABEL(labname2), matchplayers[1]);
      break;
    case 3:
      gtk_label_set_text(GTK_LABEL(labname3), matchplayers[2]);
      break;
    case 4:
      gtk_label_set_text(GTK_LABEL(labname4), matchplayers[3]);
      break;
  }
  if (strlen(matchplayers[0]) > 0 && strlen(matchplayers[1]) > 0) {
    if (singles == 1) {
      gtk_widget_set_sensitive(PlayNewMatchMi, TRUE);
    }
    else  {
      if (strlen(matchplayers[2]) > 0 && strlen(matchplayers[3]) > 0)
        gtk_widget_set_sensitive(PlayNewMatchMi, TRUE);
    }
  }
}
  
static void cb_cancel_defaults(GtkWidget *widget, gpointer entry);
static void cb_ok_defaults(GtkWidget *widget, gpointer entry);

static void cb_cancel_defaults(GtkWidget *widget, gpointer entry) {
  gtk_widget_destroy(win1);
}

static void cb_ok_defaults(GtkWidget *widget, gpointer entry) {
  gtk_widget_destroy(win1);
}

static void cb_selbut1(GtkWidget *widget, gpointer selection) {
  strcpy(selplayer, "");
  set_player_to_select(1);
  pl_main(1); //  select and cancel only
  strcpy(matchplayers[0], selplayer);
}

static void cb_selbut2(GtkWidget *widget, gpointer selection) {
  strcpy(selplayer, "");
  set_player_to_select(2);
  pl_main(1); //  select and cancel only
  strcpy(matchplayers[1], selplayer);
}

static void cb_selbut3(GtkWidget *widget, gpointer selection) {
  strcpy(selplayer, "");
  set_player_to_select(3);
  pl_main(1); //  select and cancel only
  strcpy(matchplayers[2], selplayer);
}

static void cb_selbut4(GtkWidget *widget, gpointer selection) {
  strcpy(selplayer, "");
  set_player_to_select(4);
  pl_main(1); //  select and cancel only
  strcpy(matchplayers[3], selplayer);
}

void match_players_main(void)  {
  GtkWidget *butn_cancel, *butn_close;

  GtkWidget *vbox;
  GtkWidget *grd;
  GtkWidget *label1, *label2, *label3, *label4, *labteam1, *labteam2;
  GtkWidget *selbut1, *selbut2, *selbut3, *selbut4;
  int cntr;

  // gtk_window_get_size(GTK_WINDOW(window_1), &xres, &yres);

  singles = get_match_singles();
  for (cntr = 0; cntr < 4; cntr++)
    strcpy(matchplayers[cntr], "");
    
  win1 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  // gtk_window_set_modal(GTK_WINDOW(win1), TRUE);
  gtk_window_set_title(GTK_WINDOW(win1), "Match Players");
  gtk_window_set_position(GTK_WINDOW(win1), GTK_WIN_POS_CENTER);
  gtk_window_set_keep_above(GTK_WINDOW(win1), TRUE);
  gtk_window_set_decorated(GTK_WINDOW(win1), TRUE);
  gtk_container_set_border_width(GTK_CONTAINER (win1), 10);
  gtk_widget_set_size_request(win1, 250, 100);
  
  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(win1), vbox);
  grd = gtk_grid_new();
  gtk_grid_set_column_spacing (GTK_GRID(grd), 10);
  gtk_grid_set_row_spacing (GTK_GRID(grd), 10);
  gtk_container_add(GTK_CONTAINER(vbox), grd);

  if (singles == 2) {
    labteam1 = gtk_label_new("--Team 1--");
    gtk_grid_attach (GTK_GRID(grd), labteam1, 0, 1, 2, 1);
  }

  label1 = gtk_label_new("Player 1");
  gtk_grid_attach (GTK_GRID(grd), label1, 0, 2, 1, 1);
  if (strlen(matchplayers[0]) == 0)
    labname1 = gtk_label_new(" Not Selected ");
  else
    labname1 = gtk_label_new(matchplayers[0]);
  gtk_grid_attach (GTK_GRID(grd), labname1, 1, 2, 1, 1);
  selbut1 = gtk_button_new_with_label("Choose");
  g_signal_connect(G_OBJECT(selbut1), "clicked", G_CALLBACK(cb_selbut1), NULL);
  gtk_grid_attach (GTK_GRID(grd), selbut1, 2, 2, 1, 1);
  
  label2 = gtk_label_new("Player 2");
  gtk_grid_attach (GTK_GRID(grd), label2, 0, 3, 1, 1);
  if (strlen(matchplayers[1]) == 0)
    labname2 = gtk_label_new(" Not Selected ");
  else
    labname2 = gtk_label_new(matchplayers[1]);
   gtk_grid_attach (GTK_GRID(grd), labname2, 1, 3, 1, 1);
  selbut2 = gtk_button_new_with_label("Choose");
  g_signal_connect(G_OBJECT(selbut2), "clicked", G_CALLBACK(cb_selbut2), NULL);
  gtk_grid_attach (GTK_GRID(grd), selbut2, 2, 3, 1, 1);
  
  if (singles == 2) { //  doubles play
    labteam2 = gtk_label_new("--Team 2--");
    gtk_grid_attach (GTK_GRID(grd), labteam2, 0, 4, 2, 1);
    label3 = gtk_label_new("Player 3");
    gtk_grid_attach (GTK_GRID(grd), label3, 0, 5, 1, 1);
    if (strlen(matchplayers[2]) == 0)
      labname3 = gtk_label_new(" Not Selected ");
    else
      labname3 = gtk_label_new(matchplayers[2]);
    gtk_grid_attach (GTK_GRID(grd), labname3, 1, 5, 1, 1);
    selbut3 = gtk_button_new_with_label("Choose");
    g_signal_connect(G_OBJECT(selbut3), "clicked", G_CALLBACK(cb_selbut3), NULL);
    gtk_grid_attach (GTK_GRID(grd), selbut3, 2, 5, 1, 1);
  
    label4 = gtk_label_new("Player 4");
    gtk_grid_attach (GTK_GRID(grd), label4, 0, 6, 1, 1);
    if (strlen(matchplayers[3]) == 0)
      labname4 = gtk_label_new(" Not Selected ");
    else
      labname4 = gtk_label_new(matchplayers[3]);
     gtk_grid_attach (GTK_GRID(grd), labname4, 1, 6, 1, 1);
    selbut4 = gtk_button_new_with_label("Choose");
    g_signal_connect(G_OBJECT(selbut4), "clicked", G_CALLBACK(cb_selbut4), NULL);
    gtk_grid_attach (GTK_GRID(grd), selbut4, 2, 6, 1, 1);
  }
  butn_cancel = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
  g_signal_connect(G_OBJECT(butn_cancel), "clicked", G_CALLBACK(cb_cancel_defaults), NULL);
  butn_close = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  g_signal_connect(G_OBJECT(butn_close), "clicked", G_CALLBACK(cb_ok_defaults), NULL);

  gtk_grid_attach (GTK_GRID(grd), butn_close, 0, 7, 3, 1);

  gtk_widget_show_all(win1);
}

