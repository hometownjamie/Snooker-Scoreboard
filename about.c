/*  about.c
    Copyright 2025 - Jamie Scott
    This software is free to use, distribute, and modify.
    Licensed under GPL 3
*/
#ifndef SSB_H
#include "ssb.h"
#endif

void about_box(void)  {
  GtkWidget *dialog;
  const gchar *authors[] = {"Jamie Scott", NULL};
  char temp[128];
  GdkPixbuf *logo;
  
  get_progdir(temp);
  strcat(temp, "/snooker.jpeg");
  logo = gdk_pixbuf_new_from_file(temp, NULL);
  dialog = gtk_about_dialog_new();
  gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "Snooker Scoreboard");
  gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "Version 1.0");
  gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), "Copyright © 2025 Jamie Scott");
  gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), authors);
  gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(dialog), "This program comes with absolutely no warranty.  See the GNU General Public License, version 3 or later for details.");
  gtk_about_dialog_set_wrap_license(GTK_ABOUT_DIALOG(dialog), TRUE);
  gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), logo);
  gtk_widget_show_all(dialog);

}

