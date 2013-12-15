#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x60, 0xEB, 0xFA, 0x97, 0xB3, 0xBC, 0x49, 0xB7, 0xBF, 0x1E, 0x4C, 0x48, 0xF7, 0x6C, 0x96, 0x38 }
PBL_APP_INFO(MY_UUID,
             "Game of Life", "GDCR13",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window window;
Layer layer;

#define COOKIE_MY_TIMER 1
#define SIZE 10

#define rows 16
#define cols 15

#define TICK 100

bool cells[rows][cols];
bool newcells[rows][cols];

int x;
AppTimerHandle timer_handle;

void layer_update_callback(Layer *me, GContext* ctx) {
  // graphics_context_set_fill_color(ctx, GColorWhite);
  // graphics_fill_rect(ctx, GRect(0,0, rows * SIZE, cols *SIZE ), 0,
  //       0
  //     );
  graphics_context_set_fill_color(ctx, GColorBlack);
  for (int row=0; row < rows ; row++){
    for (int col=0; col < cols ; col++){
      if (cells[row][col]){
        // graphics_fill_circle(ctx, GPoint(col*SIZE, row*SIZE), SIZE/2);

        graphics_fill_rect(ctx, GRect(col * SIZE, row * SIZE, SIZE, SIZE ),
          0, 0);
      }
    }
  }
}

int count_neighbours(int row,int col){
  int count = 0;
  for (int dRow = -1; dRow <= 1; dRow++){
    for (int dCol = -1; dCol <= 1; dCol++){
      int nRow = row + dRow;
      int nCol = col + dCol;
      if (dRow == 0 && dCol == 0) continue;
      if (nRow < 0 || nRow >= rows) continue;
      if (nCol < 0 || nCol >= cols) continue;
      if (cells[row + dRow][col + dCol]){
        count ++;
      }
    }
  }
  return count;
}

void generation(){
  memset(newcells, 0, rows*cols*sizeof(bool));
  for (int row=0; row < rows ; row++){
    for (int col=0; col < cols ; col++){
      int neighbours = count_neighbours(row, col);
      if (cells[row][col]){
        newcells[row][col] = (neighbours == 2 || neighbours == 3);
      } else {
        newcells[row][col] = (neighbours == 3);
      }
    }
  }
  memcpy(cells, newcells, rows*cols*sizeof(bool));
}


void handle_timer(AppContextRef ctx, AppTimerHandle handle, uint32_t cookie) {

  if (cookie == COOKIE_MY_TIMER) {
    generation();
    layer_mark_dirty(&layer);
    timer_handle = app_timer_send_event(ctx, TICK /* milliseconds */, COOKIE_MY_TIMER);
  }

}

void reset(){
  for (int row = 0; row < rows ; row++){
    for (int col = 0; col < cols ; col++){
      cells[row][col] = (rand() % 2 == 0);
    }
  }
}


void single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  reset();
}

void click_config_provider(ClickConfig **config, Window *window) {
  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) single_click_handler;
  config[BUTTON_ID_UP]->click.handler = (ClickHandler) single_click_handler;
  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) single_click_handler;
}



void handle_init(AppContextRef ctx) {

  x = 0;
  window_init(&window, "Game of Life");
  window_stack_push(&window, true /* Animated */);
  window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);


  layer_init(&layer, window.layer.frame);
  layer.update_proc = &layer_update_callback;
  layer_add_child(&window.layer, &layer);

  reset();

  timer_handle = app_timer_send_event(ctx, TICK /* milliseconds */, COOKIE_MY_TIMER);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .timer_handler = &handle_timer
  };
  app_event_loop(params, &handlers);
}
