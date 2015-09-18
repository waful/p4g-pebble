#include <pebble.h>
    
#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
    
static Window *s_main_window;

static TextLayer *s_day_of_week_layer;

static GFont s_day_of_week_font;

static BitmapLayer *s_background_layer;
static BitmapLayer *s_date_digit_1_layer;
static BitmapLayer *s_date_digit_2_layer;
static BitmapLayer *s_date_digit_3_layer;
static BitmapLayer *s_date_digit_4_layer;
static BitmapLayer *s_time_digit_1_layer;
static BitmapLayer *s_time_digit_2_layer;
static BitmapLayer *s_time_digit_3_layer;
static BitmapLayer *s_time_digit_4_layer;

static GBitmap *s_background_bitmap;
static GBitmap *s_digit_array[10];

static void render_digit_layer_with_bitmap(BitmapLayer* layer, uint8_t digit){
    // get the right bitmap
    GBitmap *b_digit = s_digit_array[digit];
    bitmap_layer_set_bitmap(layer, b_digit);
}

static void render_day_of_week_layer(uint8_t day_of_week){
    switch(day_of_week){
        case 1:
            text_layer_set_text(s_day_of_week_layer, "MON");
            break;
        case 2:
            text_layer_set_text(s_day_of_week_layer, "TUE");
            break;
        case 3:
            text_layer_set_text(s_day_of_week_layer, "WED");
            break;
        case 4:
            text_layer_set_text(s_day_of_week_layer, "THU");
            break;
        case 5:
            text_layer_set_text(s_day_of_week_layer, "FRI");
            break;
        case 6:
            text_layer_set_text(s_day_of_week_layer, "SAT");
            break;
        default:
            text_layer_set_text(s_day_of_week_layer, "SUN");
            break;
    }
}

static void update_time() {
    time_t temp = time(NULL); 
    struct tm *tick_time = localtime(&temp);
    static char old_time[] = "----";
    static char old_date[] = "-----";

    char time_buffer[] = "0000";
    char date_buffer[] = "00000";
    if(clock_is_24h_style() == true) {
        strftime(time_buffer, sizeof(time_buffer), "%H%M", tick_time);
    } else {
        strftime(time_buffer, sizeof(time_buffer), "%I%M", tick_time);
    }
    strftime(date_buffer, sizeof(date_buffer), "%m%d%w", tick_time);
    
    if(time_buffer[0] != old_time[0]){
        render_digit_layer_with_bitmap(s_time_digit_1_layer, time_buffer[0] - '0');
    }
    if(time_buffer[1] != old_time[1]){
        render_digit_layer_with_bitmap(s_time_digit_2_layer, time_buffer[1] - '0');
    }
    if(time_buffer[2] != old_time[2]){
        render_digit_layer_with_bitmap(s_time_digit_3_layer, time_buffer[2] - '0');
    }
    if(time_buffer[3] != old_time[3]){
        render_digit_layer_with_bitmap(s_time_digit_4_layer, time_buffer[3] - '0');
    }
    memcpy(old_time, time_buffer, sizeof(old_time));
    
    
    if(date_buffer[0] != old_date[0]){
        render_digit_layer_with_bitmap(s_date_digit_1_layer, date_buffer[0] - '0');
    }
    if(date_buffer[1] != old_date[1]){
        render_digit_layer_with_bitmap(s_date_digit_2_layer, date_buffer[1] - '0');
    }
    if(date_buffer[2] != old_date[2]){
        render_digit_layer_with_bitmap(s_date_digit_3_layer, date_buffer[2] - '0');
    }
    if(date_buffer[3] != old_date[3]){
        render_digit_layer_with_bitmap(s_date_digit_4_layer, date_buffer[3] - '0');
    }
    if(date_buffer[4] != old_date[4]){
        render_day_of_week_layer(date_buffer[4] - '0');
    }
    memcpy(old_date, date_buffer, sizeof(old_date));
}

static void update_temp(char temp[], uint8_t condition){
    
}

static void update_background(){
    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
}

static void setup_layers(Window *window){
    // set up background
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
    s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
    
    // set up date digit
    s_date_digit_1_layer = bitmap_layer_create(GRect(5, 13, 17, 16));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_date_digit_1_layer));
    s_date_digit_2_layer = bitmap_layer_create(GRect(22, 13, 17, 16));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_date_digit_2_layer));
    s_date_digit_3_layer = bitmap_layer_create(GRect(51, 13, 17, 16));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_date_digit_3_layer));
    s_date_digit_4_layer = bitmap_layer_create(GRect(68, 13, 17, 16));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_date_digit_4_layer));
    
    // set up time
    s_time_digit_1_layer = bitmap_layer_create(GRect(63, 144, 17, 16));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_time_digit_1_layer));
    s_time_digit_2_layer = bitmap_layer_create(GRect(80, 144, 17, 16));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_time_digit_2_layer));
    s_time_digit_3_layer = bitmap_layer_create(GRect(105, 144, 17, 16));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_time_digit_3_layer));
    s_time_digit_4_layer = bitmap_layer_create(GRect(122, 144, 17, 16));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_time_digit_4_layer));
    
    // set up day of week
    s_day_of_week_layer = text_layer_create(GRect(85, 8, 46, 18));
    text_layer_set_background_color(s_day_of_week_layer, GColorClear);
    text_layer_set_text_color(s_day_of_week_layer, GColorOrange);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_day_of_week_layer));
}

static void setup_bitmaps(){
    // set up backgrounds
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
    
    // set up digits array
    GBitmap *tmp_digit_array[10] = {
        gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_0),
        gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_1),
        gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_2),
        gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_3),
        gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_4),
        gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_5),
        gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_6),
        gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_7),
        gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_8),
        gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_9)
    };
    memcpy(s_digit_array, tmp_digit_array, sizeof(s_digit_array));
}

static void setup_texts_and_fonts(){
    // day of week
    s_day_of_week_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
    text_layer_set_font(s_day_of_week_layer, s_day_of_week_font);
    text_layer_set_text_alignment(s_day_of_week_layer, GTextAlignmentCenter);
}

static void main_window_load(Window *window) {
    setup_layers(window);
    setup_bitmaps();
    setup_texts_and_fonts();
    
    update_background();
    update_time();
    update_temp("??", -1);
}

static void main_window_unload(Window *window) {
    // destroy texts and fonts
    text_layer_destroy(s_day_of_week_layer);
    
    // destroy bitmaps
    gbitmap_destroy(s_background_bitmap);
    uint8_t i;
    for(i = 0; i < sizeof(s_digit_array); i++){
        gbitmap_destroy(s_digit_array[i]);
    }

    // destroy layers
    bitmap_layer_destroy(s_background_layer);
    bitmap_layer_destroy(s_date_digit_1_layer);
    bitmap_layer_destroy(s_date_digit_2_layer);
    bitmap_layer_destroy(s_date_digit_3_layer);
    bitmap_layer_destroy(s_date_digit_4_layer);
    bitmap_layer_destroy(s_time_digit_1_layer);
    bitmap_layer_destroy(s_time_digit_2_layer);
    bitmap_layer_destroy(s_time_digit_3_layer);
    bitmap_layer_destroy(s_time_digit_4_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
    
    // Get weather update every 30 minutes
    if(tick_time->tm_min % 30 == 0) {
        // Begin dictionary
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);

        // Add a key-value pair
        dict_write_uint8(iter, 0, 0);

        // Send the message!
        app_message_outbox_send();
    }
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    // Store incoming information
    char tmp_temperature[3];
    uint8_t tmp_condition;

    // iterate through payload
    Tuple *t;
    for(t = dict_read_first(iterator); t != NULL; t = dict_read_next(iterator)) {
        switch(t->key) {
        case KEY_TEMPERATURE:
            snprintf(tmp_temperature, sizeof(tmp_temperature), "%d", (int)t->value->int8);
            break;
        case KEY_CONDITIONS:
            tmp_condition = (int)t->value->uint8;
            break;
        default:
            APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
            break;
        }
    }
    
    update_temp(tmp_temperature, tmp_condition);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
    
static void init() {
    // setup window
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });
    window_stack_push(s_main_window, true);
    
    // setup tick timer
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    
    // register callbacks
    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);
    
    // open appmessage
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
    // destroy window
    window_destroy(s_main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
    return 0;
}