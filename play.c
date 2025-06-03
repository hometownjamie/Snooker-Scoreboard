/*  play.c
    Copyright 2025 - Jamie Scott
    This software is free to use, distribute, and modify.
    Licensed under GPL 3
*/
#ifndef SSB_H
#include "ssb.h"
#endif
#include <gdk/gdkkeysyms.h>
#include <time.h>
#include <unistd.h>

extern GtkWidget *PlayNewMatchMi, *PlayNextFrameMi, *PlayResumeFrameMi, *PlayFixScoresMi;

extern int xres, yres;
static int timer_id;
static int frame_over;
static GtkWidget *playwin, *win1, *eb, *darea;
static GtkWidget *entry1, *entry2, *entry3, *entry4;
static GdkRGBA bgcolour;
static int time_remaining;  //  in seconds
extern char matchplayers[4][40];
static int singles;
static int t1_score, t2_score, t1_frames, t2_frames;
static int lastballwasred, breakpoints, current_player, last_foul;
static int balls_remaining[7];
static int toggle = 0;
typedef struct  {
  int t1_score;
  int t2_score;
  int t1_frames;
  int t2_frames;
  int breakpoints;
  int balls_remaining[7];
  int frame_over;
}
UNDO_DATA;
static UNDO_DATA undo_data[10];
static UNDO_DATA save_data;
static UNDO_DATA redo_data[10];
 
void new_match(void);

static void cb_cancel_fixscore(GtkWidget *widget, gpointer entry);
static void cb_ok_fixscore(GtkWidget *widget, gpointer entry);
static int cb_destroy();
static void save_undo(void);
static void perform_undo(void);
static int undo_levels, redo_levels;
static void init_frame(void);
static void make_click_sound(void);
static void redo(void);

static void save_undo(void) {
  int cntr;
  
  //  compare current status to last saved
  save_data.t1_score = t1_score;
  save_data.t2_score = t2_score;
  save_data.t1_frames = t1_frames;
  save_data.t2_frames = t2_frames;
  save_data.breakpoints = breakpoints;
  memcpy(&save_data.balls_remaining[0], &balls_remaining[0], sizeof(balls_remaining));
  save_data.frame_over = frame_over;
  if (memcmp(&save_data, &undo_data[0], sizeof(save_data)) == 0) {    // no change - nothing to save
    // printf("Not saving save data\n");
    return;
  }
  
  //  shift up the undo_data
  for (cntr = 9; cntr > 0; cntr--)  {
    memcpy(&undo_data[cntr], &undo_data[cntr-1], sizeof(undo_data[0]));
  }
  
  //  save current data into undo_data[0]
  memcpy(&undo_data[0], &save_data, sizeof(save_data));
  if (++undo_levels > 10)
    undo_levels = 10;
/*  printf("Saved undo_data.  undo_levels: %d\n", undo_levels);
  printf("scores:\n");
  printf("at index 0:   team 1: %d  team 2: %d\n", undo_data[0].t1_score, undo_data[0].t2_score);
  printf("at index 1:   team 1: %d  team 2: %d\n\n", undo_data[1].t1_score, undo_data[1].t2_score); */
}

static void redo(void)  {
  int cntr;

  // printf("redo_levels: %d\n", redo_levels);
  if (redo_levels == 0)
    return;
  make_click_sound();
  //  set the current data
  t1_score = redo_data[0].t1_score;
  t2_score = redo_data[0].t2_score;
  t1_frames = redo_data[0].t1_frames;
  t2_frames = redo_data[0].t2_frames;
  breakpoints = redo_data[0].breakpoints;
  memcpy(&balls_remaining[0], &redo_data[0].balls_remaining[0], sizeof(balls_remaining));
  frame_over = redo_data[0].frame_over;

  //  shift the redo data down
  for (cntr = 1; cntr < 10; cntr++)
    memcpy(&redo_data[cntr-1], &redo_data[cntr], sizeof(redo_data[0]));
  redo_levels--;
}

static void save_redo_data(void) {
  int cntr;
 
  //  shift up the redo
  for (cntr = 9; cntr > 0; cntr--)
    memcpy(&redo_data[cntr], &redo_data[cntr-1], sizeof(redo_data[0]));

  //  store current data into redo_data[0]
  redo_data[0].t1_score = t1_score;
  redo_data[0].t2_score = t2_score;
  redo_data[0].t1_frames = t1_frames;
  redo_data[0].t2_frames = t2_frames;
  redo_data[0].breakpoints = breakpoints;
  memcpy(&redo_data[0].balls_remaining[0], &balls_remaining[0], sizeof(balls_remaining));
  redo_data[0].frame_over = frame_over;

  redo_levels++;
  if (redo_levels > 10)
    redo_levels = 10;
  save_undo();
}

static void perform_undo(void) {
  int cntr;
  
  if (undo_levels == 0)   //  nothing to undo
    return;
  
  make_click_sound();
  save_redo_data();
  
  //  shift the undo data down
  for (cntr = 0; cntr < 9; cntr++)
    memcpy(&undo_data[cntr], &undo_data[cntr+1], sizeof(undo_data[0]));
    
  //  set the current data
  t1_score = undo_data[0].t1_score; 
  t2_score = undo_data[0].t2_score; 
  t1_frames = undo_data[0].t1_frames;
  t2_frames = undo_data[0].t2_frames;
  breakpoints = undo_data[0].breakpoints;
  memcpy(&balls_remaining, &undo_data[0].balls_remaining[0], sizeof(balls_remaining));
  frame_over = undo_data[0].frame_over;

  undo_levels--;
/*  printf("Performed Undo - undo_levels: %d\n", undo_levels);
  printf("scores:\n");
  printf("at index 0:   team 1: %d  team 2: %d\n", undo_data[0].t1_score, undo_data[0].t2_score);
  printf("at index 1:   team 1: %d  team 2: %d\n\n", undo_data[1].t1_score, undo_data[1].t2_score);
*/
  lastballwasred = FALSE;
}

static void cb_cancel_fixscore(GtkWidget *widget, gpointer entry) {
  gtk_widget_destroy(win1);
}

static void cb_ok_fixscore(GtkWidget *widget, gpointer entry) {
  int coloured_balls_remaining, cntr;
  
  balls_remaining[0] = atoi(gtk_entry_get_text(GTK_ENTRY(entry1)));
  t1_score = atoi(gtk_entry_get_text(GTK_ENTRY(entry2)));
  t2_score = atoi(gtk_entry_get_text(GTK_ENTRY(entry3)));
  coloured_balls_remaining = atoi(gtk_entry_get_text(GTK_ENTRY(entry4)));
  cntr = 1;
  if (coloured_balls_remaining > 0 && balls_remaining[0] == 0)
    cntr = 7 - coloured_balls_remaining;
  for (; cntr < 7; cntr++)
    balls_remaining[cntr] = 1;
  gtk_widget_destroy(win1);
}

void fixscore_main(void)  {
  GtkWidget *butn_cancel, *butn_ok;

  GtkWidget *vbox;
  GtkWidget *butbox;
  GtkWidget *grd;
  GtkWidget *label1, *label2, *label3, *label4;
  char temp[40];
  int cntr, coloured_balls_remaining;

  win1 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_modal(GTK_WINDOW(win1), TRUE);
  gtk_window_set_title(GTK_WINDOW(win1), "Fix Scores");
  gtk_window_set_position(GTK_WINDOW(win1), GTK_WIN_POS_CENTER);
//  gtk_window_move(GTK_WINDOW(win1), 600, 420);
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

  label1 = gtk_label_new("Reds Remaining");
  gtk_grid_attach (GTK_GRID(grd), label1, 0, 2, 2, 1);
  entry1 = gtk_entry_new();
  gtk_entry_set_width_chars(GTK_ENTRY(entry1), 5);
  sprintf(temp, "%2d", balls_remaining[0]);
  gtk_entry_set_text(GTK_ENTRY(entry1), temp);
  gtk_grid_attach (GTK_GRID(grd), entry1, 2, 2, 2, 1);
  
  for (cntr = 1, coloured_balls_remaining = 0; cntr < 7; cntr++)  {
    if (balls_remaining[cntr] > 0)
      coloured_balls_remaining++;
  }
 label2 = gtk_label_new("Team 1 / Player 1 Points");
  gtk_grid_attach (GTK_GRID(grd), label2, 0, 4, 2, 1);
  entry2 = gtk_entry_new();
  gtk_entry_set_width_chars(GTK_ENTRY(entry2), 5);
  sprintf(temp, "%d", t1_score);
  gtk_entry_set_text(GTK_ENTRY(entry2), temp);
  gtk_grid_attach (GTK_GRID(grd), entry2, 2, 4, 2, 1);
  
  label3 = gtk_label_new("Team 1 / Player 1 Points");
  gtk_grid_attach (GTK_GRID(grd), label3, 0, 5, 2, 1);
  entry3 = gtk_entry_new();
  gtk_entry_set_width_chars(GTK_ENTRY(entry3), 5);
  sprintf(temp, "%d", t2_score);
  gtk_entry_set_text(GTK_ENTRY(entry3), temp);
  gtk_grid_attach (GTK_GRID(grd), entry3, 2, 5, 2, 1);
  
  label4 = gtk_label_new("Coloured Balls Remaining");
  gtk_grid_attach (GTK_GRID(grd), label4, 0, 3, 2, 1);
  entry4 = gtk_entry_new();
  gtk_entry_set_width_chars(GTK_ENTRY(entry4), 5);
  sprintf(temp, "%2d", coloured_balls_remaining);
  gtk_entry_set_text(GTK_ENTRY(entry4), temp);
  gtk_grid_attach (GTK_GRID(grd), entry4, 2, 3, 2, 1);
  
  butn_cancel = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
  g_signal_connect(G_OBJECT(butn_cancel), "clicked", G_CALLBACK(cb_cancel_fixscore), NULL);
  butn_ok = gtk_button_new_from_stock (GTK_STOCK_APPLY);
  g_signal_connect(G_OBJECT(butn_ok), "clicked", G_CALLBACK(cb_ok_fixscore), NULL);

  butbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_container_set_border_width (GTK_CONTAINER (butbox), 5);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (butbox), GTK_BUTTONBOX_END);
  gtk_box_set_spacing (GTK_BOX (butbox), 5);
  gtk_container_add (GTK_CONTAINER (butbox), butn_cancel);
  gtk_container_add (GTK_CONTAINER (butbox), butn_ok);

  gtk_grid_attach (GTK_GRID(grd), butbox, 0, 6, 3, 1);

  gtk_widget_show_all(win1);
}

static gboolean on_key_press_event(GtkWidget(*widget), GdkEventKey *e, gpointer user_data)	{
  int tscore = 0;
  int foul_points = 0;
  int doclick = TRUE;
  int doclick2 = TRUE;
/*  
  printf("Key value: %0x ", e->keyval);
  if (e->state & GDK_SHIFT_MASK)
    printf("shift\n");
  else
    printf(" no shift\n");
    */
    
  switch (e->keyval)  {
    case GDK_KEY_F4:
      foul_points =+ 4;
      lastballwasred = FALSE;
      last_foul = TRUE;
      break;
     case GDK_KEY_F5:
      foul_points =+ 5;
      lastballwasred = FALSE;
      last_foul = TRUE;
      break;
    case GDK_KEY_F6:
      foul_points =+ 6;
      lastballwasred = FALSE;
      last_foul = TRUE;
      break;
    case GDK_KEY_F7:
      foul_points =+ 7;
      lastballwasred = FALSE;
      last_foul = TRUE;
      break;
   case GDK_KEY_KP_1:
   case GDK_KEY_1:
      if (balls_remaining[0] > 0) {
        tscore++;
        balls_remaining[0]--;      
        lastballwasred = TRUE;
      }
      break;
    case GDK_KEY_KP_2:
    case GDK_KEY_2:
      if (lastballwasred == TRUE) {
        tscore += 2;
        lastballwasred = FALSE;
      }
      else  {
        if (balls_remaining[0] > 0) {
          tscore += 3;
          balls_remaining[0]--;
        }
        else  {
          if (balls_remaining[1] == 1)  {
            tscore += 2;
            balls_remaining[1] = 0;
          }
        }
      }
      break;
    case GDK_KEY_KP_3:
    case GDK_KEY_3:
      if (lastballwasred == TRUE) {
        lastballwasred = FALSE;
        tscore += 3;
        lastballwasred = FALSE;
      }
      else  {
        if (balls_remaining[0] > 0) {
          tscore += 4;
          balls_remaining[0]--;
        }
        else  {
          if (balls_remaining[2] == 1 && balls_remaining[1] == 0)  {
            tscore += 3;
            balls_remaining[2] = 0;
          }
        }
      }
      break;
    case GDK_KEY_KP_4:
    case GDK_KEY_4:
      if (lastballwasred == TRUE) {
        lastballwasred = FALSE;
        tscore += 4;
        lastballwasred = FALSE;
      }
      else  {
        if (balls_remaining[0] > 0) {
          tscore += 5;
          balls_remaining[0]--;
        }
        else  {
          if (balls_remaining[3] == 1 && balls_remaining[2] == 0)  {
            tscore += 4;
            balls_remaining[3] = 0;
          }
        }
      }
      break;
    case GDK_KEY_KP_5:
    case GDK_KEY_5:
      if (lastballwasred == TRUE) {
        lastballwasred = FALSE;
        tscore += 5;
        lastballwasred = FALSE;
      }
      else  {
        if (balls_remaining[0] > 0) {
          tscore += 6;
          balls_remaining[0]--;
        }
        else  {
          if (balls_remaining[4] == 1 && balls_remaining[3] == 0)  {
            tscore += 5;
            balls_remaining[4] = 0;
          }
        }
      }
      break;
    case GDK_KEY_KP_6:
    case GDK_KEY_6:
      if (lastballwasred == TRUE) {
        lastballwasred = FALSE;
        tscore += 6;
        lastballwasred = FALSE;
      }
      else  {
        if (balls_remaining[0] > 0) {
          tscore += 7;
          balls_remaining[0]--;
        }
        else  {
          if (balls_remaining[5] == 1 && balls_remaining[4] == 0)  {
            tscore += 6;
            balls_remaining[5] = 0;
          }
        }
      }
      break;
    case GDK_KEY_KP_7:
    case GDK_KEY_7:
      if (lastballwasred == TRUE) {
        lastballwasred = FALSE;
        tscore += 7;
        lastballwasred = FALSE;
      }
      else  {
        if (balls_remaining[0] > 0) {
          tscore += 8;
          balls_remaining[0]--;
        }
        else  {
          if (balls_remaining[6] == 1 && balls_remaining[5] == 0)  {
            tscore += 7;
            balls_remaining[6] = 0;
            frame_over = TRUE;
            if (t1_score > t2_score)
              t1_frames++;
            else
              t2_frames++;
          }
        }
      }
      break;
    case GDK_KEY_minus:
    case GDK_KEY_KP_Subtract:   //  if a red falls on a foul
      if (last_foul == TRUE && balls_remaining[0] > 0)
        balls_remaining[0]--;
      break;
    default:
      doclick = FALSE;
  }
  if (balls_remaining[0] <= 0)  {
    balls_remaining[0] = 0;
  }
  breakpoints += tscore;
  if (current_player == 0)  {
    t1_score += tscore;
    t2_score += foul_points;
  }
  else  {
    t2_score += tscore;
    t1_score += foul_points;
  }
  
  if (balls_remaining[6] == 0)  {
    gtk_widget_set_sensitive(PlayResumeFrameMi, FALSE);
    gtk_widget_set_sensitive(PlayFixScoresMi, FALSE);
  }

  switch (e->keyval)  {
    case GDK_KEY_Left:
      if (current_player == 1)  {
        current_player = 0;
        breakpoints = 0;
        lastballwasred = FALSE;
        last_foul = FALSE;
        save_undo();
      }
      break;
    case GDK_KEY_Right:
      if (current_player == 0)  {
        current_player = 1;
        breakpoints = 0;
        lastballwasred = FALSE;
        last_foul = FALSE;
        save_undo();
      }  
      break;
    case GDK_KEY_U:   //  undo
       doclick2 = FALSE;
     perform_undo();
      break;
    case GDK_KEY_R:   //  redo
      doclick2 = FALSE;
      redo();
      break;
    case GDK_KEY_N:   //  next frame
      if (t1_frames + t2_frames < get_default_frames_per_match() && frame_over == TRUE)
      printf("Beginning next frame\n");
        init_frame();
      break;
    case GDK_KEY_Escape:
      doclick2 = FALSE;
      if (frame_over == TRUE) {
        t1_score = t2_score = 0;
        gtk_widget_set_sensitive(PlayResumeFrameMi, FALSE);
        gtk_widget_set_sensitive(PlayFixScoresMi, FALSE);
        if (get_default_frames_per_match() > 0) {
          if (t1_frames + t2_frames == get_default_frames_per_match())
            gtk_widget_set_sensitive(PlayNextFrameMi, FALSE);
        }
      }
      gtk_widget_destroy(playwin);
      break;
    case GDK_KEY_G:      //  give up
      if (frame_over == FALSE)  {
        if (t1_score > t2_score)
          t1_frames++;
        else
          t2_frames++;
      }
      frame_over = TRUE;
      break;
    default:
      doclick2 = FALSE;
  } 
  if (doclick == TRUE || doclick2 == TRUE)
    make_click_sound();
  return(FALSE);
}

static void make_click_sound(void)  {
  system ("mpg123 -q click.mp3 > /dev/null 2>&1 &");
}

static void init_frame(void) {
  int cntr;
  
  undo_data[0].balls_remaining[0] = balls_remaining[0] = get_default_noreds();
  for (cntr = 1; cntr < 7; cntr++)
    undo_data[0].balls_remaining[cntr] = balls_remaining[cntr] = 1;
  undo_data[0].t1_score = t1_score = undo_data[0].t2_score = t2_score = 0;
  lastballwasred = FALSE;
  last_foul = FALSE;
  undo_data[0].breakpoints = breakpoints = 0;
  current_player = 0;
  undo_data[0].frame_over = frame_over = FALSE;
  undo_levels = 0;
  redo_levels = 0;
  gtk_widget_set_sensitive(PlayNextFrameMi, TRUE);
  gtk_widget_set_sensitive(PlayResumeFrameMi, TRUE);
  gtk_widget_set_sensitive(PlayFixScoresMi, TRUE);
  system ("amixer set Master 50% > /dev/null");
}

void draw_sb(cairo_t *cr)  {
  char temp[128];
  int lasty, points_remaining, cntr, coloured_balls_remaining;
  time_t t;
  struct tm date;
  
  if (cr == NULL)	{
    printf("null pointer to cr in draw_sb\n");
    return;
  }
  
  //  time remaining
  cairo_set_font_size(cr, 72);
  cairo_set_source_rgb(cr, 0, 0xff, 0xff);
  if (get_default_match_time() > 0) {
    sprintf(temp, "%02d:%02d", time_remaining/60, time_remaining%60);
    cairo_move_to(cr, xres / 2 - 131, 80);
  }
  else  {
  t = time(NULL);
  date = *localtime(&t);
  if (date.tm_hour > 12)
    sprintf(temp, "%2d:%02d PM", date.tm_hour - 12, date.tm_min);
  else
    sprintf(temp, "%2d:%02d AM", date.tm_hour, date.tm_min);
  cairo_move_to(cr, xres / 2 - 140, 80);
  }
  cairo_show_text(cr, temp);

//  horizontal line
  cairo_new_path (cr);
  cairo_set_source_rgb(cr, 0xff, 0, 0);
  cairo_set_line_width (cr, 10);
  cairo_move_to(cr, 10, 100);
  cairo_rel_line_to(cr, xres - 22, 0);
  cairo_stroke(cr); 
  
 //  vertical line
  cairo_new_path (cr);
  cairo_set_line_width (cr, 8);
  cairo_move_to(cr, (xres - 32) / 2, 100);
  cairo_rel_line_to(cr, 0, 350);
  cairo_stroke(cr); 
  
 //  players
  singles = get_match_singles();
  cairo_set_font_size(cr, 48);
  if (singles == 1) {
    if (current_player == 0)
      cairo_set_source_rgb(cr, 0, 0xff, 0);
    else
      cairo_set_source_rgb(cr, 0xff, 0, 0xff);
    cairo_move_to(cr, 50, 150);
    cairo_show_text(cr, matchplayers[0]);
    if (current_player == 1)
      cairo_set_source_rgb(cr, 0, 0xff, 0);
    else
      cairo_set_source_rgb(cr, 0xff, 0, 0xff);
    cairo_move_to(cr, xres / 2 + 50, 150);
    cairo_show_text(cr, matchplayers[1]);
  }
  else  {
    if (current_player == 0)     
      cairo_set_source_rgb(cr, 0, 0xff, 0);
    else
      cairo_set_source_rgb(cr, 0xff, 0, 0xff);      
    cairo_move_to(cr, 50, 150);
    cairo_show_text(cr, matchplayers[0]);
    cairo_move_to(cr, 50, 200);
    cairo_show_text(cr, matchplayers[1]);
    cairo_move_to(cr, xres / 2 + 50, 150);
    if (current_player == 1)
      cairo_set_source_rgb(cr, 0, 0xff, 0);
    else
      cairo_set_source_rgb(cr, 0xff, 0, 0xff);
    cairo_show_text(cr, matchplayers[2]);
    cairo_move_to(cr, xres / 2 + 50, 200);
    cairo_show_text(cr, matchplayers[3]);
 }

  cairo_set_font_size(cr, 72);
  //  team1 score
  lasty = 280;
  cairo_move_to(cr, 210, lasty);
  sprintf(temp, "%3d", t1_score);
  if (current_player == 0)
    cairo_set_source_rgb(cr, 0, 0xff, 0);
  else
    cairo_set_source_rgb(cr, 0xff, 0, 0xff);
  cairo_show_text(cr, temp);

  //  team2 score
  cairo_move_to(cr, 900, lasty);
  sprintf(temp, "%3d", t2_score);
  if (current_player == 0)
   cairo_set_source_rgb(cr, 0xff, 0, 0xff);
  else
     cairo_set_source_rgb(cr, 0, 0xff, 0);
  cairo_show_text(cr, temp);  

  //  break points or final
  cairo_set_font_size(cr, 48);
  lasty += 70;
  cairo_set_source_rgb(cr, 0, 0xff, 0);
  if (balls_remaining[6] == 0)   //  black ball gone
    frame_over = TRUE;
  if (frame_over == TRUE) {
    if (t1_score > t2_score)
      cairo_move_to(cr, xres / 3 - 285, lasty);
    else
      cairo_move_to(cr, xres / 3 * 2, lasty);
    cairo_show_text(cr, "Final");
  }
  else  {
    sprintf(temp, "Break: %3d", breakpoints);
    cairo_set_source_rgb(cr, 0, 0xff, 0);
    if (current_player == 0)
      cairo_move_to(cr, xres / 3 - 285, lasty);
    else
      cairo_move_to(cr, xres / 3 * 2, lasty);
    cairo_show_text(cr, temp);
  }

  //  frames
  lasty += 60;
  sprintf(temp, "Frames: %d", t1_frames);
  cairo_move_to(cr, xres / 3 - 285, lasty);
  if (current_player == 0)
    cairo_set_source_rgb(cr, 0, 0xff, 0);
  else
    cairo_set_source_rgb(cr, 0xff, 0, 0xff);
  cairo_show_text(cr, temp);
  sprintf(temp, "Frames: %d", t2_frames);
  cairo_move_to(cr, xres / 3 * 2, lasty);
  if (current_player == 0)
    cairo_set_source_rgb(cr, 0xff, 0, 0xff);
  else
    cairo_set_source_rgb(cr, 0, 0xff, 0);
  cairo_show_text(cr, temp);

lasty += 50;
//  horizontal line
  cairo_new_path (cr);
  cairo_set_source_rgb(cr, 0xff, 0, 0);
  cairo_set_line_width (cr, 10);
  cairo_move_to(cr, 10, lasty);
  cairo_rel_line_to(cr, xres - 22, 0);
  cairo_stroke(cr); 
  
  //  points remaining and spread
  points_remaining = balls_remaining[0] * 8;
  if (lastballwasred == TRUE)
    points_remaining += 7;
  for (cntr = 1; cntr < 7; cntr++)  {
    if (balls_remaining[cntr] == 1)
      points_remaining += cntr+1;
  }
  sprintf(temp, "Difference: %d       Remaining: %d", abs(t1_score - t2_score), points_remaining);
  lasty += 100;
  cairo_set_font_size(cr, 48);
  cairo_set_source_rgb(cr, 0, 0xff, 0xff);
  cairo_move_to(cr, xres / 2 - 370, lasty);
  cairo_show_text(cr, temp);
  
  //  balls remaining
//   cairo_set_font_size(cr, 16);
// cairo_set_source_rgb(cr, 0xff, 0xff, 0);
  coloured_balls_remaining = 6;
  for (cntr = 1; cntr < 7; cntr++)  {
    if (balls_remaining[cntr] == 0)
      coloured_balls_remaining--;
  }
  sprintf(temp, "Reds Remaining: %d  Coloured Balls Remaining: %d", balls_remaining[0], coloured_balls_remaining);
  lasty += 75;
  cairo_move_to(cr, xres / 2 - 510, lasty);
  cairo_set_font_size(cr, 40);
  cairo_show_text(cr, temp);
}

static gboolean processing_loop(void)	{ //	called every 500 ms
  gtk_widget_queue_draw (GTK_WIDGET(darea));
  if (toggle == 0) {
    toggle = 1;
    return(TRUE);
  }
  else
    toggle = 0;
  //&& t1_frames + t2_frames < get_default_frames_per_match()) {
  if (get_default_match_time() > 0) {   //  timed match
    if (time_remaining > 0)
      time_remaining--;
    if (time_remaining == 0 && frame_over == FALSE)  {
      frame_over = TRUE;
      gtk_widget_set_sensitive(PlayNextFrameMi, FALSE);
      system ("nohup mpg123 -q beep.mp3 > /dev/null 2>&1 &");
     if (t1_score > t2_score)
        t1_frames++;
      else
        t2_frames++;
    }
  }
  else  {  //  not timed match
    if (balls_remaining[6] == 0 && frame_over == FALSE) {  //  black ball gone
      frame_over = TRUE;
      gtk_widget_set_sensitive(PlayNextFrameMi, TRUE);
      if (t1_score > t2_score)
        t1_frames++;
      else
        t2_frames++;
    }
  }
  //  Check for match over
  if (get_default_frames_per_match() == t1_frames + t2_frames)  //  match over
    gtk_widget_set_sensitive(PlayNextFrameMi, FALSE);
  return(TRUE);
}

static int cb_destroy()	{
  g_source_remove(timer_id);
  gtk_widget_destroy(playwin);
  return(FALSE);
}

static gboolean on_draw_event(GtkDrawingArea *widget, cairo_t *cr)	{
  if (cr == NULL)	{
    printf("null pointer to cr in on_draw_event\n");
    return FALSE;
  }
  draw_sb(cr);
  return FALSE;
}

void play_main(void)	{
  
//  printf("Screen Resolution: %d x %d\n", xres, yres);
  bgcolour.red = bgcolour.green = bgcolour.blue = 0;
  bgcolour.alpha = 0xff;
  playwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(playwin), GTK_WIN_POS_CENTER);
  gtk_window_set_title (GTK_WINDOW(playwin), "Snooker Scoreboard");
  gtk_window_set_default_size(GTK_WINDOW(playwin), xres, yres);
  gtk_window_set_resizable(GTK_WINDOW(playwin), TRUE);
  gtk_window_set_decorated(GTK_WINDOW(playwin), TRUE);
//  gtk_window_set_deletable(GTK_WINDOW(playwin), TRUE);
  gtk_widget_override_background_color(GTK_WIDGET(playwin), GTK_STATE_NORMAL, &bgcolour);

  eb = gtk_event_box_new();
  darea = gtk_drawing_area_new();
  gtk_container_add(GTK_CONTAINER(playwin), eb);
  gtk_container_add(GTK_CONTAINER(eb), darea);
  g_signal_connect(G_OBJECT(darea), "draw", G_CALLBACK(on_draw_event), NULL);
  g_signal_connect (G_OBJECT (playwin), "key_press_event", G_CALLBACK (on_key_press_event), NULL);
  g_signal_connect(G_OBJECT(playwin), "destroy", G_CALLBACK(cb_destroy), NULL);

  gtk_widget_show_all(playwin);

  gtk_widget_queue_draw (GTK_WIDGET(darea));

  timer_id = g_timeout_add(500, (GSourceFunc)processing_loop, NULL);
}

void new_match(void) {
  load_defaults();
  t1_frames = t2_frames = 0;
  time_remaining = get_default_match_time() * 60;
  init_frame();
  play_main();
}

void new_frame(void) {
  init_frame();
  play_main();
}

void update_scores(int redsremaining, int score1, int score2) {
  balls_remaining[0] = redsremaining;
  t1_score = score1;
  t2_score = score2;
}

