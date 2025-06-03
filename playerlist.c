/*  playerlist.c
    Copyright 2025 - Jamie Scott
    This software is free to use, distribute, and modify.
    Licensed under GPL 3
*/
#ifndef SSB_H
#include "ssb.h"
#endif

#define SELECT 1
#define AED 2

int noplayers = 0;
static char playernames[MAXPLAYERS][40];
int playerlist_changed = FALSE;

static GtkWidget *win1, *win2;
static GtkTreeViewColumn *column1;
static GtkWidget *list;
static GtkWidget *entry_playername;

static int player_to_select;
static int playerlist_index;

static void build_list(void);
static void init_list(GtkWidget *list);
void str_replace_chars(char *instr, char replace, char with);
void trim (char *instr);
void ltrim(char *instr);
void rtrim(char *instr);

void set_player_to_select(int num)  {
  player_to_select = num;
 }
  
void load_players(void)	{
  char temp[128];
  FILE *fp;
  
  get_progdir(temp);
  strcat(temp, "/playerlist.bin");
  // printf("file: %s\n", temp);
  fp = fopen(temp, "rb");
  if (fp == NULL)	{
    // printf("File open error\n");
  }
  else	{
  fread(&noplayers, sizeof(int), 1, fp);
  fread(&playernames[0], sizeof(playernames[0]), noplayers, fp);
  fclose(fp);
  }
}

static void cb_delplayername(GtkWidget *widget, gpointer selection) {
  GtkTreeModel *model;
  GtkTreeIter iter;
  GtkWidget *dialog;
  gchar *name;

  model = gtk_tree_view_get_model(GTK_TREE_VIEW(list));
  gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), &model, &iter);
  gtk_tree_model_get (model, &iter, 0, &name, -1);
  if (strlen(name) == 0)  {
    // nothing selected
    dialog = gtk_message_dialog_new(GTK_WINDOW(win1), GTK_DIALOG_DESTROY_WITH_PARENT,
      			GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Nothing Selected");
    gtk_window_set_title(GTK_WINDOW(dialog), "Error");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return;
  }
  for (playerlist_index = 0; playerlist_index < noplayers; playerlist_index++)  {
    if (strcmp(name, playernames[playerlist_index]) == 0)
      break;
  }
  for (; playerlist_index < noplayers - 1; playerlist_index++)
    strcpy (playernames[playerlist_index], playernames[playerlist_index + 1]);

  playerlist_changed = TRUE;
  noplayers--;

  build_list();
}

static void cb_cancel_playernameedit(GtkWidget *widget, gpointer entry) {
	gtk_widget_destroy(win2);
}

static void cb_cancel_playernameadd(GtkWidget *widget, gpointer entry) {
  gtk_widget_destroy(win2);
}

static void cb_ok_playernameedit(GtkWidget *widget, gpointer entry) {
  char temp[128];

  strcpy(temp, gtk_entry_get_text(GTK_ENTRY(entry_playername)));

  if (strlen(temp) > 0) {
    strcpy(playernames[playerlist_index], temp);
    playerlist_changed = TRUE;
    build_list();
  }
  gtk_widget_destroy(win2);
}

static void cb_ok_playeradd(GtkWidget *widget, gpointer entry) {
  if (noplayers == MAXPLAYERS)  {
    printf("Player list full\n");
    return;
  }
  strcpy(playernames[noplayers], gtk_entry_get_text(GTK_ENTRY(entry_playername)));
  noplayers++;
  playerlist_changed = TRUE;
  build_list();
  gtk_widget_destroy(win2);
}

static void cb_edit_playername(GtkWidget *widget, gpointer selection) {
  GtkWidget *dialog;
  GtkWidget *grd, *butbox;
  GtkWidget *label1;
  GtkWidget *butn_cancel, *butn_ok;
  GtkTreeModel *model;
  GtkTreeIter  iter;
  gchar *orig_name;

  model = gtk_tree_view_get_model(GTK_TREE_VIEW(list));
  gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), &model, &iter);
  gtk_tree_model_get (model, &iter, 0, &orig_name, -1);
  // printf("Selected Name: %s\n", orig_name);
  if (strlen(orig_name) == 0) {
    // nothing selected
    dialog = gtk_message_dialog_new(GTK_WINDOW(win2), GTK_DIALOG_DESTROY_WITH_PARENT,
      			GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Nothing Selected");
    gtk_window_set_title(GTK_WINDOW(dialog), "Error");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return;
  }
  for (playerlist_index = 0; playerlist_index < noplayers; playerlist_index++) {
    if (strcmp(playernames[playerlist_index], orig_name) == 0)
      break;
  }
  win2 = gtk_window_new(GTK_WINDOW_POPUP);
  gtk_window_set_title(GTK_WINDOW(win2), "Edit Player");
  gtk_window_set_modal(GTK_WINDOW(win2), TRUE);
  gtk_window_set_keep_above(GTK_WINDOW(win2), TRUE);
  gtk_window_set_position(GTK_WINDOW(win2), GTK_WIN_POS_CENTER);
//  gtk_widget_set_size_request(win2, 370, 210);
  gtk_container_set_border_width(GTK_CONTAINER (win2), 10);

  grd = gtk_grid_new();
  gtk_grid_set_column_spacing (GTK_GRID(grd), 5);
  gtk_container_add(GTK_CONTAINER(win2), grd);

  butn_cancel = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
  g_signal_connect(G_OBJECT(butn_cancel), "clicked", G_CALLBACK(cb_cancel_playernameedit), selection);
  butn_ok = gtk_button_new_from_stock (GTK_STOCK_OK);
  g_signal_connect(G_OBJECT(butn_ok), "clicked", G_CALLBACK(cb_ok_playernameedit), selection);

  butbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_container_set_border_width (GTK_CONTAINER (butbox), 5);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (butbox), GTK_BUTTONBOX_END);
  gtk_box_set_spacing (GTK_BOX (butbox), 5);
  gtk_container_add (GTK_CONTAINER (butbox), butn_cancel);
  gtk_container_add (GTK_CONTAINER (butbox), butn_ok);

  gtk_grid_attach (GTK_GRID(grd), butbox, 0, 5, 2, 1);

  label1 = gtk_label_new("Edit Name");
  gtk_grid_attach (GTK_GRID(grd), label1, 0, 0, 1, 1);

  entry_playername = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(entry_playername), orig_name);
  gtk_entry_set_width_chars(GTK_ENTRY(entry_playername), 40);
  gtk_grid_attach (GTK_GRID(grd), entry_playername, 1, 0, 1, 1);

  gtk_widget_show_all (win2);
}

int get_sel_tag_index(void) {
  return (playerlist_index);
}

static void close_playerlist(GtkWidget *widget, gpointer selection)  {
  FILE *fp;
  char temp[128];
  
  if (playerlist_changed == TRUE) {
    get_progdir(temp);
    strcat(temp, "/playerlist.bin");
    fp = fopen(temp, "wb");
    if (fp == NULL)
      printf("File creation error of playerlist.bin\n");
    else	{    
      fwrite(&noplayers, sizeof(int), 1, fp);
      fwrite(&playernames[0], sizeof(playernames[0]), noplayers, fp);
      fclose(fp);
      playerlist_changed = FALSE;
    }
  }
  gtk_widget_destroy(win1);
}

static void cb_newplayer(GtkWidget *widget, gpointer selection) {
  GtkWidget *grd, *butbox;
  GtkWidget *label1;
  GtkWidget *butn_cancel, *butn_ok;
  GtkTreeModel *model;
  GtkTreeIter  iter;

  model = gtk_tree_view_get_model(GTK_TREE_VIEW(list));
  gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), &model, &iter);

  model = gtk_tree_view_get_model(GTK_TREE_VIEW(list));

  win2 = gtk_window_new(GTK_WINDOW_POPUP);

  gtk_window_set_title(GTK_WINDOW(win2), "New Player");
  gtk_window_set_modal(GTK_WINDOW(win2), TRUE);
  gtk_window_set_position(GTK_WINDOW(win2), GTK_WIN_POS_CENTER);
  gtk_container_set_border_width(GTK_CONTAINER (win2), 10);

  grd = gtk_grid_new();
  gtk_grid_set_column_spacing (GTK_GRID(grd), 5);
  gtk_container_add(GTK_CONTAINER(win2), grd);

  butn_cancel = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
  g_signal_connect(G_OBJECT(butn_cancel), "clicked", G_CALLBACK(cb_cancel_playernameadd), selection);
  butn_ok = gtk_button_new_from_stock (GTK_STOCK_OK);
  g_signal_connect(G_OBJECT(butn_ok), "clicked", G_CALLBACK(cb_ok_playeradd), selection);
  butbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_container_set_border_width (GTK_CONTAINER (butbox), 5);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (butbox), GTK_BUTTONBOX_END);
  gtk_box_set_spacing (GTK_BOX (butbox), 5);
  gtk_container_add (GTK_CONTAINER (butbox), butn_cancel);
  gtk_container_add (GTK_CONTAINER (butbox), butn_ok);

  gtk_grid_attach (GTK_GRID(grd), butbox, 0, 5, 2, 1);

  label1 = gtk_label_new("Player Name");
  gtk_grid_attach (GTK_GRID(grd), label1, 0, 0, 1, 1);

  entry_playername = gtk_entry_new();
  gtk_window_activate_default(GTK_WINDOW(win2));
  gtk_entry_set_activates_default(GTK_ENTRY(entry_playername), TRUE);
  gtk_entry_set_text(GTK_ENTRY(entry_playername), "");
  gtk_entry_set_width_chars(GTK_ENTRY(entry_playername), 40);
  gtk_grid_attach (GTK_GRID(grd), entry_playername, 1, 0, 1, 1);

  gtk_widget_show_all (win2);
}

void strupr(char *s)  {
  int x;

  for (x = 0; x < strlen(s); x++)  {
    if (s[x] > 96 && s[x] < 123)
      s[x] -= 32;
  }
}

int strnicmp(char *str1, char *str2)  {
  char s1[1024], s2[1024];
  int x;

  strcpy(s1, str1);
  strcpy(s2, str2);
  strupr(s1);
  str_replace_chars(s1, 0x3c, 0x20);      // <
  str_replace_chars(s2, 0x3c, 0x20);
  str_replace_chars(s1, 0x3e, 0x20);      //  >
  str_replace_chars(s2, 0x3e, 0x20);
  strupr(s2);
  trim(s1);
  trim(s2);
  x = strncmp(s1, s2, strlen(s1));
//  // printf ("s1-%s- s2-%s-  Comp. Result: %d\n", s1, s2, x);
  return (x);

}

void str_replace_chars(char *instr, char replace, char with) {
  int x;

  for (x = 0; x < strlen(instr); x++) {
    if (instr[x] == replace)
      instr[x] = with;
  }
}

void trim (char *instr) {
  ltrim(instr);
  rtrim(instr);
}

void ltrim(char *instr) {
  int x;
  char s[1024];

  strcpy(s, instr);
  for (x = 0; x < strlen(instr); x++)  {
    if (s[x] != 0x20 && s[x] != 9)  {
      strcpy (instr, &s[x]);
      break;
    }
  }
}

void rtrim(char *instr) {
  int x;

  for (x = strlen(instr) - 1; x > 0; x--)  {
    if (isblank(instr[x]))
      instr[x] = '\0';
    else
      break;
  }
}

//  Used to clear and populate the data table
static void build_list(void)	{
 GtkListStore *store;
  GtkTreeIter iter;
  char str_col_name[40], temp[40];
  int playerindex, x;

  store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list)));
  gtk_list_store_clear(store);

  // Sort the player names  
  for (playerindex = 0; playerindex < noplayers - 1; playerindex++)	{
    if(strnicmp(playernames[playerindex], playernames[playerindex + 1]) > 0)  {
      strcpy(temp, playernames[playerindex]);
      strcpy(playernames[playerindex], playernames[playerindex + 1]);
      strcpy(playernames[playerindex + 1], temp);
      playerindex = -1;
    }
  }
  
  //  finally we rebuild the list store
  for (x = 0; x < noplayers; x++) {
    gtk_list_store_append(store, &iter);
    sprintf (temp, "%d", x);
    strcpy(str_col_name, playernames[x]);
    gtk_list_store_set(store, &iter, 0, &str_col_name, -1);
  }
}

static void select_item(GtkWidget *widget, gpointer selection) {
  GtkTreeModel *model;
  GtkTreeIter  iter;
  gchar *player_name;

  model = gtk_tree_view_get_model(GTK_TREE_VIEW(list));

  if (gtk_tree_model_get_iter_first(model, &iter) == FALSE)
      return;

  if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), &model, &iter)) {
    gtk_tree_model_get (model, &iter, 0, &player_name, -1);
  }
  else
    playerlist_index = -1;

  set_selplayer(player_name, player_to_select);
  //sendtag(dest_id, playerlist_index);
  // updt_seltag(playerlist_index);
  gtk_widget_destroy(win1);
}

void entry_changed_event_cb (GtkWidget *win, GdkEvent *event,
		gpointer selection)	{

 build_list();
  return;
}

static void init_list(GtkWidget *list) {
  GtkCellRenderer    *renderer;
  GtkListStore       *store;
  char colname1[20];

  sprintf (colname1, " %s ", "Name");
  renderer = gtk_cell_renderer_text_new();
  column1 = gtk_tree_view_column_new_with_attributes(colname1, renderer, "text", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(list), column1);

  store = gtk_list_store_new(1, G_TYPE_STRING);

  gtk_tree_view_set_model(GTK_TREE_VIEW(list), GTK_TREE_MODEL(store));

  g_object_unref(store);
}

int pl_main(int featurebits) {

  GtkWidget *sw;

  GtkWidget *remove;
  GtkWidget *add;
  GtkWidget *edit;
  GtkWidget *select;
  GtkWidget *cancel;

  GtkWidget *vbox;
  GtkWidget *hbox;
  GtkWidget *hbox2;

  GtkTreeSelection *selection;

  playerlist_index = -1;  // start off with no tag tag_selected

  win1 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_modal(GTK_WINDOW(win1), TRUE);
  gtk_window_set_title(GTK_WINDOW(win1), "Player List");
//  gtk_window_set_position(GTK_WINDOW(win1), GTK_WIN_POS_CENTER);
  gtk_window_move(GTK_WINDOW(win1), 100, 100);
  gtk_window_set_keep_above(GTK_WINDOW(win1), TRUE);
  gtk_window_set_decorated(GTK_WINDOW(win1), TRUE);
  gtk_container_set_border_width(GTK_CONTAINER (win1), 10);
  gtk_widget_set_size_request(win1, 500, 400);

  sw = gtk_scrolled_window_new(NULL, NULL);
  list = gtk_tree_view_new();

  gtk_container_add(GTK_CONTAINER(sw), list);

  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(sw),
            GTK_SHADOW_ETCHED_IN);

  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(list), TRUE);
	gtk_tree_view_set_headers_clickable (GTK_TREE_VIEW(list), TRUE);
  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  gtk_box_pack_start(GTK_BOX(vbox), sw, TRUE, TRUE, 5);

  hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

  if (featurebits & AED)  {
  add = gtk_button_new_with_label("Add New");
  edit = gtk_button_new_with_label(" Edit ");
  remove = gtk_button_new_with_label("Delete");
  }
  if (featurebits & SELECT)  {
    select = gtk_button_new_with_label("Select");
    cancel = gtk_button_new_with_label("Cancel");
  }
  else
    cancel = gtk_button_new_with_label("Save / Close");

  if (featurebits & AED)  {
    gtk_box_pack_start(GTK_BOX(hbox2), add, FALSE, TRUE, 3);
    gtk_box_pack_start(GTK_BOX(hbox2), edit, FALSE, TRUE, 3);
    gtk_box_pack_start(GTK_BOX(hbox2), remove, FALSE, TRUE, 3);
  }
  if (featurebits & SELECT)
    gtk_box_pack_start(GTK_BOX(hbox2), select, FALSE, TRUE, 3);
  gtk_box_pack_start(GTK_BOX(hbox2), cancel, FALSE, TRUE, 3);

  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 3);
  gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, TRUE, 3);

  gtk_container_add(GTK_CONTAINER(win1), vbox);

  init_list(list);
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list));

  if (featurebits & AED) {
    g_signal_connect(G_OBJECT(add), "clicked", G_CALLBACK(cb_newplayer), selection);
    g_signal_connect(G_OBJECT(edit), "clicked", G_CALLBACK(cb_edit_playername), selection);
    g_signal_connect(G_OBJECT(remove), "clicked", G_CALLBACK(cb_delplayername), selection);
  }

  if (featurebits & SELECT)
    g_signal_connect(G_OBJECT(select), "clicked", G_CALLBACK(select_item), selection);

  g_signal_connect(G_OBJECT(cancel), "clicked", G_CALLBACK(close_playerlist), NULL);
  g_signal_connect(G_OBJECT(win1), "destroy", G_CALLBACK(close_playerlist), NULL);

  gtk_widget_show_all(win1);
  // gtk_widget_grab_focus(sw);
  build_list();	// populate the entire list
  return 0;
}
