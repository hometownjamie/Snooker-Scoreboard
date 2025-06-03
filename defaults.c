/*  defaults.c
    Copyright 2025 - Jamie Scott
    This software is free to use, distribute, and modify.
    Licensed under GPL 3
*/

#ifndef SSB_H
#include "ssb.h"
#endif

static DEFAULTS defaults;
static GtkWidget *entry1, *entry2, *entry3, *entry4;

static GtkWidget *win1;

int get_match_singles(void)  {
  return(defaults.singles);
}

int get_default_frames_per_match(void)  {
  return(defaults.frames_per_match);
}

void load_defaults(void)  {
  FILE *fp;
  char temp[128];
  
  get_progdir(temp);
  strcat(temp, "/defaults.bin");
  fp = fopen(temp, "rb");
  if (fp == NULL)	{
    // printf("File open error\n");
  }
  else	{
  fread(&defaults, sizeof(defaults), 1, fp);
  fclose(fp);
  }
}

int get_default_match_time(void)  {
  return (defaults.match_time);
}

int get_default_noreds(void)  {
  return(defaults.noreds);
}

static void cb_cancel_defaults(GtkWidget *widget, gpointer entry);
static void cb_ok_defaults(GtkWidget *widget, gpointer entry);

static void cb_cancel_defaults(GtkWidget *widget, gpointer entry) {
  gtk_widget_destroy(win1);
}

static void cb_ok_defaults(GtkWidget *widget, gpointer entry) {
  char temp[40], temp2[128];
  FILE *fp;
  
  strcpy(temp, gtk_entry_get_text(GTK_ENTRY(entry1)));
  defaults.singles = atoi(temp);
  strcpy(temp, gtk_entry_get_text(GTK_ENTRY(entry2)));
  defaults.frames_per_match = atoi(temp);
  strcpy(temp, gtk_entry_get_text(GTK_ENTRY(entry3)));
  defaults.match_time = atoi(temp);
  strcpy(temp, gtk_entry_get_text(GTK_ENTRY(entry4)));
  defaults.noreds = atoi(temp);

  get_progdir(temp2);
  strcat(temp2, "/defaults.bin");
  fp = fopen(temp2, "wb");
  if (fp == NULL)
    printf("File creation error of defaults.bin\n");
  else	{    
    fwrite(&defaults, sizeof(defaults), 1, fp);
    fclose(fp);
  }
  gtk_widget_destroy(win1);
}

void defaults_main(void)  {
  GtkWidget *butn_cancel, *butn_ok;

  GtkWidget *vbox;
  GtkWidget *butbox;
  GtkWidget *grd;
  GtkWidget *label1, *label2, *label3, *label4;
  char temp[40];

  win1 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_modal(GTK_WINDOW(win1), TRUE);
  gtk_window_set_title(GTK_WINDOW(win1), "Match Settings");
  gtk_window_set_position(GTK_WINDOW(win1), GTK_WIN_POS_CENTER);
  gtk_window_move(GTK_WINDOW(win1), 600, 420);
  gtk_window_set_keep_above(GTK_WINDOW(win1), TRUE);
  gtk_window_set_decorated(GTK_WINDOW(win1), TRUE);
  gtk_container_set_border_width(GTK_CONTAINER (win1), 10);
  gtk_widget_set_size_request(win1, 250, 100);
  
  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(win1), vbox);
  grd = gtk_grid_new();
  gtk_grid_set_column_spacing (GTK_GRID(grd), 5);
  gtk_grid_set_row_spacing (GTK_GRID(grd), 5);
  gtk_container_add(GTK_CONTAINER(vbox), grd);

  label1 = gtk_label_new("Singles(1) or Doubles(2)");
  gtk_grid_attach (GTK_GRID(grd), label1, 0, 2, 2, 1);
  entry1 = gtk_entry_new();
  gtk_entry_set_width_chars(GTK_ENTRY(entry1), 5);
  sprintf(temp, "%d", defaults.singles);
  gtk_entry_set_text(GTK_ENTRY(entry1), temp);
  gtk_grid_attach (GTK_GRID(grd), entry1, 2, 2, 2, 1);
  
  label2 = gtk_label_new("Frames per Match");
  gtk_grid_attach (GTK_GRID(grd), label2, 0, 3, 2, 1);
  entry2 = gtk_entry_new();
  gtk_entry_set_width_chars(GTK_ENTRY(entry2), 5);
  sprintf(temp, "%d", defaults.frames_per_match);
  gtk_entry_set_text(GTK_ENTRY(entry2), temp);
  gtk_grid_attach (GTK_GRID(grd), entry2, 2, 3, 2, 1);
  
  label3 = gtk_label_new("Time per Match");
  gtk_grid_attach (GTK_GRID(grd), label3, 0, 4, 2, 1);
  entry3 = gtk_entry_new();
  gtk_entry_set_width_chars(GTK_ENTRY(entry3), 5);
  sprintf(temp, "%d", defaults.match_time);
  gtk_entry_set_text(GTK_ENTRY(entry3), temp);
  gtk_grid_attach (GTK_GRID(grd), entry3, 2, 4, 2, 1);
  
  label4 = gtk_label_new("Number of Reds");
  gtk_grid_attach (GTK_GRID(grd), label4, 0, 5, 2, 1);
  entry4 = gtk_entry_new();
  gtk_entry_set_width_chars(GTK_ENTRY(entry4), 2);
  sprintf(temp, "%d", defaults.noreds);/*  main.c
    Copyright 2025 - Jamie Scott
    This software is free to use, distribute, and modify.
    Licensed under GPL 3
*/

  gtk_entry_set_text(GTK_ENTRY(entry4), temp);
  gtk_grid_attach (GTK_GRID(grd), entry4, 2, 5, 2, 1);
  
  butn_cancel = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
  g_signal_connect(G_OBJECT(butn_cancel), "clicked", G_CALLBACK(cb_cancel_defaults), NULL);
  butn_ok = gtk_button_new_from_stock (GTK_STOCK_APPLY);
  g_signal_connect(G_OBJECT(butn_ok), "clicked", G_CALLBACK(cb_ok_defaults), NULL);

  butbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_container_set_border_width (GTK_CONTAINER (butbox), 5);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (butbox), GTK_BUTTONBOX_END);
  gtk_box_set_spacing (GTK_BOX (butbox), 5);
  gtk_container_add (GTK_CONTAINER (butbox), butn_cancel);
  gtk_container_add (GTK_CONTAINER (butbox), butn_ok);

  gtk_grid_attach (GTK_GRID(grd), butbox, 0, 7, 3, 1);

  gtk_widget_show_all(win1);
}

