#include "main.h"
#include "pebble.h"

static Window *window;

// 日付レイヤー削除(11/4)
// bk
//static Layer *s_simple_bg_layer, *s_date_layer, *s_hands_layer;
//static TextLayer *s_day_label, *s_num_label;
static Layer *window_layer;
static Layer *s_simple_bg_layer, *s_hands_layer;
static TextLayer *s_num_label;

// 背景画像レイヤー(11/3)
static BitmapLayer *bitmap_layer;
static GBitmap *bitmap, *bitmap_BToff;

// 元の時刻マーカーパス削除
//static GPath *s_tick_paths[NUM_CLOCK_TICKS];
static GPath *s_minute_arrow, *s_hour_arrow, *s_marker_0;
// 日付表示
// static char s_num_buffer[4], s_day_buffer[6];

// Bluetoothステータス(11/30)
static char bt_status[4] = "BT";
// 過去のBluetoothステータス
static int bt_st_old = 1;
static bool bt_connection_old;

static void bg_update_proc(Layer *layer, GContext *ctx) {
  // 背景画像使用のため背景色削除
  //graphics_context_set_fill_color(ctx, GColorBlack);
  //graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
  //円を描く
  //太さ10
//  graphics_context_set_stroke_width(ctx,2);
  //色：赤 GColorRed
//  graphics_context_set_stroke_color(ctx, GColorRed);
//  graphics_draw_circle(ctx, GPoint(175 - 1,  90 - 1), 3);

  // 0,3,6,9時のマーカー
  // pattern_1: 棒状  
  graphics_context_set_stroke_width(ctx,MARKER_MAIN_SIZE);
  graphics_context_set_stroke_color(ctx, MARKER_MAIN_COLOR); 
  graphics_context_set_fill_color(ctx,MARKER_MAIN_COLOR);
  gpath_draw_filled(ctx, s_marker_0);
//  graphics_draw_line(ctx, GPoint( 90, 10), GPoint( 90,  0)); // 0時
  graphics_draw_line(ctx, GPoint(170, 90), GPoint(180, 90));
  graphics_draw_line(ctx, GPoint( 90,170), GPoint( 90,180));
  graphics_draw_line(ctx, GPoint( 10, 90), GPoint(  0, 90));

  // pattern_2: 丸
//  graphics_context_set_fill_color(ctx, GColorRichBrilliantLavender);
//  graphics_fill_circle(ctx, GPoint(175 - 1,  90), 3);
//  graphics_fill_circle(ctx, GPoint( 90, 175 - 1), 3);
//  graphics_fill_circle(ctx, GPoint(  5,  90), 3);
//  graphics_fill_circle(ctx, GPoint( 90,   5), 3);

  // 0,3,6,9時以外のマーカー
  // マーカー色
  graphics_context_set_fill_color(ctx, MARKER_SUB_COLOR);
  graphics_fill_circle(ctx, GPoint( 159, 130), MARKER_SUB_SIZE);
  graphics_fill_circle(ctx, GPoint( 130, 159), MARKER_SUB_SIZE);
  graphics_fill_circle(ctx, GPoint(  50, 159), MARKER_SUB_SIZE);
  graphics_fill_circle(ctx, GPoint(  21, 130), MARKER_SUB_SIZE);
  graphics_fill_circle(ctx, GPoint(  21,  50), MARKER_SUB_SIZE);
  graphics_fill_circle(ctx, GPoint(  50,  21), MARKER_SUB_SIZE);
  graphics_fill_circle(ctx, GPoint( 130,  21), MARKER_SUB_SIZE);
  graphics_fill_circle(ctx, GPoint( 159,  50), MARKER_SUB_SIZE);

//  graphics_fill_circle(ctx, GPoint( 166, 134), MARKER_SUB_SIZE);
//  graphics_fill_circle(ctx, GPoint( 134, 166), MARKER_SUB_SIZE);
//  graphics_fill_circle(ctx, GPoint(  46, 166), MARKER_SUB_SIZE);
//  graphics_fill_circle(ctx, GPoint(  14, 134), MARKER_SUB_SIZE);
//  graphics_fill_circle(ctx, GPoint(  14,  46), MARKER_SUB_SIZE);
//  graphics_fill_circle(ctx, GPoint(  46,  14), MARKER_SUB_SIZE);
//  graphics_fill_circle(ctx, GPoint( 134,  14), MARKER_SUB_SIZE);
//  graphics_fill_circle(ctx, GPoint( 166,  46), MARKER_SUB_SIZE);
  
// 元のマーカー描画削除  
//  graphics_context_set_fill_color(ctx, GColorWhite);
//  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
//    gpath_draw_filled(ctx, s_tick_paths[i]);
//  }

}

// 時刻針の更新
static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  // 秒針の長さ
//  int16_t second_hand_length = bounds.size.w / 2;

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  // 秒針の角度
//  int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
  // 秒針の座標
//  GPoint second_hand = {
//    .x = (int16_t)(sin_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.x,
//    .y = (int16_t)(-cos_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.y,
//  };

  // second hand
//  graphics_context_set_stroke_color(ctx, GColorWhite);
//  graphics_draw_line(ctx, second_hand, center);

  // minute hand
  graphics_context_set_fill_color(ctx, MINUTE_HAND_COLOR);
//  graphics_context_set_stroke_color(ctx, MINUTE_HAND_COLOR);
  gpath_rotate_to(s_minute_arrow, TRIG_MAX_ANGLE * t->tm_min / 60);
  gpath_draw_filled(ctx, s_minute_arrow);
//  gpath_draw_outline(ctx, s_minute_arrow);

  // hour hand
  graphics_context_set_fill_color(ctx, HOUR_HAND_COLOR);
//  graphics_context_set_stroke_color(ctx, HOUR_HAND_COLOR);
  gpath_rotate_to(s_hour_arrow, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));
  gpath_draw_filled(ctx, s_hour_arrow);
//  gpath_draw_outline(ctx, s_hour_arrow);

  // dot in the middle
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 2);
  graphics_draw_circle(ctx, center, 3);
  // ■の点を削除(11/29)
//  graphics_context_set_fill_color(ctx, GColorBlack);
//  graphics_fill_rect(ctx, GRect(bounds.size.w / 2 - 1, bounds.size.h / 2 - 1, 3, 3), 0, GCornerNone);
//  graphics_fill_rect(ctx, GRect(90, 45, 3, 3), 0, GCornerNone);
}

// Bluetoothハンドラ
static void handle_bluetooth(bool connected) {
  if (connected) {
      //接続に変化したらバイブ
      if(!bt_connection_old){
         vibes_short_pulse();
         bt_connection_old = true;
      };
      snprintf(bt_status,4,"BT");
      // bitmap_layer に画像を設定
      bitmap_layer_set_bitmap(bitmap_layer, bitmap);
   } else {
      //切断に変化したらバイブ
      if(bt_connection_old){
         vibes_short_pulse();
         bt_connection_old = false;
      };
      snprintf(bt_status,4,"--");
      // bitmap_layer に画像を設定
      bitmap_layer_set_bitmap(bitmap_layer, bitmap_BToff);
  }
  // bitmap_layer を root_layer に重ねる
  layer_add_child(window_layer, bitmap_layer_get_layer(bitmap_layer));
  layer_set_update_proc(s_simple_bg_layer, bg_update_proc);
  layer_add_child(window_layer, s_simple_bg_layer);
  layer_set_update_proc(s_hands_layer, hands_update_proc);
  layer_add_child(window_layer, s_hands_layer);
}

// 日付更新
//static void date_update_proc(Layer *layer, GContext *ctx) {
//  time_t now = time(NULL);
//  struct tm *t = localtime(&now);
//
//  strftime(s_day_buffer, sizeof(s_day_buffer), "%a", t);
//  text_layer_set_text(s_day_label, s_day_buffer);
//
//  strftime(s_num_buffer, sizeof(s_num_buffer), "%d", t);
//  text_layer_set_text(s_num_label, s_num_buffer);
//}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(window));
}

static void window_load(Window *window) {
  window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  bt_connection_old = connection_service_peek_pebble_app_connection();

  s_simple_bg_layer = layer_create(bounds);
  s_hands_layer = layer_create(bounds);

  // リソース "IMAGE_BG" から画像を生成
  bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG);
  bitmap_BToff = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG2);
  // bitmap_layer を作成
  bitmap_layer = bitmap_layer_create(layer_get_bounds(window_layer));

  handle_bluetooth(bt_connection_old);
  
//  if (bt_st_old == 1){
    // bitmap_layer に画像を設定
//    bitmap_layer_set_bitmap(bitmap_layer, bitmap);
//  } else {
    // bitmap_layer に画像を設定
//    bitmap_layer_set_bitmap(bitmap_layer, bitmap_BToff);
//  };
}

static void window_unload(Window *window) {
  layer_destroy(s_simple_bg_layer);
//  layer_destroy(s_date_layer);

//  text_layer_destroy(s_day_label);
  text_layer_destroy(s_num_label);

  layer_destroy(s_hands_layer);
}

static void init() {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);

//  s_day_buffer[0] = '\0';
//  s_num_buffer[0] = '\0';

  // init hand paths
  s_minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
  s_hour_arrow = gpath_create(&HOUR_HAND_POINTS);

  window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  GPoint center = grect_center_point(&bounds);
  gpath_move_to(s_minute_arrow, center);
  gpath_move_to(s_hour_arrow, center);

//  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
//    s_tick_paths[i] = gpath_create(&ANALOG_BG_POINTS[i]);
//  }

  s_marker_0 = gpath_create(&MARKER_0_POINTS);
  
  // 更新を分単位にする(11/4)
//  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
  
  // Bluetooth接続状態サービスに登録する(11/30)
  connection_service_subscribe((ConnectionHandlers){
    .pebble_app_connection_handler = handle_bluetooth
  });
}

static void deinit() {
  gpath_destroy(s_minute_arrow);
  gpath_destroy(s_hour_arrow);

//  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
//    gpath_destroy(s_tick_paths[i]);
//  }

  // マーカー
  gpath_destroy(s_marker_0);

  tick_timer_service_unsubscribe();
  connection_service_unsubscribe();
  window_destroy(window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}