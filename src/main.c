#include <pebble.h>
#define KEY_CONDITIONS 0

static Window *s_main_window;

static TextLayer *s_day_of_week_layer;

static GFont s_day_of_week_font;

static BitmapLayer *s_background_layer;
static BitmapLayer *s_date_digit_layer_array[4];
static BitmapLayer *s_time_digit_layer_array[4];
static BitmapLayer *s_time_of_day_layer;
static BitmapLayer *s_weather_icon_layer;

static GBitmap *s_background_bitmap = NULL;
static GBitmap *s_weather_icon = NULL;
static GBitmap *s_time_of_day_bitmap = NULL;
static GBitmap *s_date_digit_1_bitmap = NULL;
static GBitmap *s_date_digit_2_bitmap = NULL;
static GBitmap *s_date_digit_3_bitmap = NULL;
static GBitmap *s_date_digit_4_bitmap = NULL;
static GBitmap *s_time_digit_1_bitmap = NULL;
static GBitmap *s_time_digit_2_bitmap = NULL;
static GBitmap *s_time_digit_3_bitmap = NULL;
static GBitmap *s_time_digit_4_bitmap = NULL;

static uint32_t s_digit_array[10];
static uint32_t s_time_of_day_array[5];
static uint32_t s_weather_icon_array[9];

static void render_digit(BitmapLayer *layer, GBitmap **bitmap, uint8_t digit){
    GBitmap *local_bitmap = *bitmap;
    if(local_bitmap != NULL){
        gbitmap_destroy(local_bitmap);
    }
    local_bitmap = gbitmap_create_with_resource(s_digit_array[digit]);
    bitmap_layer_set_bitmap(layer, local_bitmap);
    *bitmap = local_bitmap;
}

static void render_day_of_week(uint8_t day_of_week){
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

static void render_time_of_day(char time_buffer[]){
    // hour
    static uint8_t old_hour = 99;
    uint8_t hour_buffer = (time_buffer[0] - '0') * 10 + (time_buffer[1] - '0');
    if(hour_buffer != old_hour){
        uint32_t res_id;
    
        // morning
        if(hour_buffer >= 5 && hour_buffer <= 8 && time_buffer[4] != 'P'){
            res_id = s_time_of_day_array[2];
        }
        // daytime
        else if((hour_buffer >= 9 && hour_buffer <= 11 && time_buffer[4] != 'P')
                 || (hour_buffer >= 1 && hour_buffer <= 5 && time_buffer[4] == 'P')
                 || (hour_buffer >= 13 && hour_buffer <= 17)){
            res_id = s_time_of_day_array[0];
        }
        // noon
        else if(hour_buffer == 12 && time_buffer[4] != 'A'){
            res_id = s_time_of_day_array[4];
        }
        // evening
        else if((hour_buffer >= 6 && hour_buffer <= 9 && time_buffer[4] == 'P')
                 || (hour_buffer >= 18 && hour_buffer <= 21)){
            res_id = s_time_of_day_array[1];
        }
        // night
        else{
            res_id = s_time_of_day_array[3];
        }
        
        if(s_time_of_day_bitmap != NULL){
            gbitmap_destroy(s_time_of_day_bitmap);
        }
        s_time_of_day_bitmap = gbitmap_create_with_resource(res_id);
        
        bitmap_layer_set_bitmap(s_time_of_day_layer, s_time_of_day_bitmap);
    }
    old_hour = hour_buffer;
}

static void render_date(char date_buffer[]){
    // date
    static char old_date[] = "-----";
    
    if(date_buffer[0] != old_date[0]){
        render_digit(s_date_digit_layer_array[0], &s_date_digit_1_bitmap, date_buffer[0] - '0');
    }
    if(date_buffer[1] != old_date[1]){
        render_digit(s_date_digit_layer_array[1], &s_date_digit_2_bitmap, date_buffer[1] - '0');
    }
    if(date_buffer[2] != old_date[2]){
        render_digit(s_date_digit_layer_array[2], &s_date_digit_3_bitmap, date_buffer[2] - '0');
    }
    if(date_buffer[3] != old_date[3]){
        render_digit(s_date_digit_layer_array[3], &s_date_digit_4_bitmap, date_buffer[3] - '0');
    }
    if(date_buffer[4] != old_date[4]){
        render_day_of_week(date_buffer[4] - '0');
    }
    memcpy(old_date, date_buffer, sizeof(old_date));
}

static void render_time() {
    time_t temp = time(NULL); 
    struct tm *tick_time = localtime(&temp);

    static char old_time[] = "------";
    char time_buffer[] = "0000  ";
    if(clock_is_24h_style() == true) {
        strftime(time_buffer, sizeof(time_buffer), "%H%M  ", tick_time);
    } else {
        strftime(time_buffer, sizeof(time_buffer), "%I%M%p", tick_time);
    }
    
    if(time_buffer[0] != old_time[0]){
        render_digit(s_time_digit_layer_array[0], &s_time_digit_1_bitmap, time_buffer[0] - '0');
    }
    if(time_buffer[1] != old_time[1]){
        render_digit(s_time_digit_layer_array[1], &s_time_digit_2_bitmap, time_buffer[1] - '0');
        
        // hour changed, check time of day
        render_time_of_day(time_buffer);
        
        // check date too
        char date_buffer[] = "00000";
        strftime(date_buffer, sizeof(date_buffer), "%m%d%w", tick_time);
        render_date(date_buffer);
    }
    if(time_buffer[2] != old_time[2]){
        render_digit(s_time_digit_layer_array[2], &s_time_digit_3_bitmap, time_buffer[2] - '0');
    }
    if(time_buffer[3] != old_time[3]){
        render_digit(s_time_digit_layer_array[3], &s_time_digit_4_bitmap, time_buffer[3] - '0');
    }
    memcpy(old_time, time_buffer, sizeof(old_time));
}

static void render_weather(uint8_t condition){
    if(s_weather_icon != NULL){
        gbitmap_destroy(s_weather_icon);
    }
    s_weather_icon = gbitmap_create_with_resource(s_weather_icon_array[condition]);
    bitmap_layer_set_bitmap(s_weather_icon_layer, s_weather_icon);
}

static void render_background(){
    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
}

static void setup_layers(Window *window){
    // set up background
    s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
    
    // set up date digit
    s_date_digit_layer_array[0] = bitmap_layer_create(GRect(5, 13, 17, 16));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_date_digit_layer_array[0]));
    s_date_digit_layer_array[1] = bitmap_layer_create(GRect(22, 13, 17, 16));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_date_digit_layer_array[1]));
    s_date_digit_layer_array[2] = bitmap_layer_create(GRect(51, 13, 17, 16));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_date_digit_layer_array[2]));
    s_date_digit_layer_array[3] = bitmap_layer_create(GRect(68, 13, 17, 16));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_date_digit_layer_array[3]));
    
    // set up time
    s_time_digit_layer_array[0] = bitmap_layer_create(GRect(63, 144, 17, 16));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_time_digit_layer_array[0]));
    s_time_digit_layer_array[1] = bitmap_layer_create(GRect(80, 144, 17, 16));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_time_digit_layer_array[1]));
    s_time_digit_layer_array[2] = bitmap_layer_create(GRect(105, 144, 17, 16));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_time_digit_layer_array[2]));
    s_time_digit_layer_array[3] = bitmap_layer_create(GRect(122, 144, 17, 16));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_time_digit_layer_array[3]));
    s_time_of_day_layer = bitmap_layer_create(GRect(5, 37, 75, 25));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_time_of_day_layer));
    bitmap_layer_set_compositing_mode(s_time_of_day_layer, GCompOpSet);
    
    // set up day of week
    s_day_of_week_layer = text_layer_create(GRect(85, 8, 46, 18));
    text_layer_set_background_color(s_day_of_week_layer, GColorClear);
    text_layer_set_text_color(s_day_of_week_layer, GColorOrange);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_day_of_week_layer));
    
    // set up weather icon
    s_weather_icon_layer = bitmap_layer_create(GRect(62, 74, 58, 58));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_weather_icon_layer));
    bitmap_layer_set_compositing_mode(s_weather_icon_layer, GCompOpSet);
}

static void setup_bitmaps(){
    // set up backgrounds
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
}

static void setup_bitmap_reference_arrays(){
    // set up time of day array
    uint32_t tmp_time_of_day_array[5] = {
        RESOURCE_ID_IMAGE_DAYTIME,
        RESOURCE_ID_IMAGE_EVENING,
        RESOURCE_ID_IMAGE_MORNING,
        RESOURCE_ID_IMAGE_NIGHT,
        RESOURCE_ID_IMAGE_NOON
    };
    memcpy(s_time_of_day_array, tmp_time_of_day_array, sizeof(s_time_of_day_array));
    
    // set up weather icon array
    uint32_t tmp_weather_icon_array[9] = {
        RESOURCE_ID_W_CLEARD,
        RESOURCE_ID_W_CLEARN,
        RESOURCE_ID_W_CLOUDY,
        RESOURCE_ID_W_RAIN,
        RESOURCE_ID_W_SNOW,
        RESOURCE_ID_W_THUNDER,
        RESOURCE_ID_W_FOG,
        RESOURCE_ID_W_BLANK,
        RESOURCE_ID_W_UNKNOWN
    };
    memcpy(s_weather_icon_array, tmp_weather_icon_array, sizeof(s_weather_icon_array));
    
    uint32_t tmp_digit_array[10] = {
        RESOURCE_ID_IMAGE_DIGIT_0,
        RESOURCE_ID_IMAGE_DIGIT_1,
        RESOURCE_ID_IMAGE_DIGIT_2,
        RESOURCE_ID_IMAGE_DIGIT_3,
        RESOURCE_ID_IMAGE_DIGIT_4,
        RESOURCE_ID_IMAGE_DIGIT_5,
        RESOURCE_ID_IMAGE_DIGIT_6,
        RESOURCE_ID_IMAGE_DIGIT_7,
        RESOURCE_ID_IMAGE_DIGIT_8,
        RESOURCE_ID_IMAGE_DIGIT_9
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
    setup_bitmap_reference_arrays();
    
    render_background();
    render_time();
    render_weather(7);
}

static void main_window_unload(Window *window) {
    // destroy texts and fonts
    text_layer_destroy(s_day_of_week_layer);
    APP_LOG(APP_LOG_LEVEL_INFO, "destroyed text layers");
    
    // destroy bitmaps
    gbitmap_destroy(s_background_bitmap);
    gbitmap_destroy(s_time_of_day_bitmap);
    gbitmap_destroy(s_weather_icon);
    gbitmap_destroy(s_date_digit_1_bitmap);
    gbitmap_destroy(s_date_digit_2_bitmap);
    gbitmap_destroy(s_date_digit_3_bitmap);
    gbitmap_destroy(s_date_digit_4_bitmap);
    gbitmap_destroy(s_time_digit_1_bitmap);
    gbitmap_destroy(s_time_digit_2_bitmap);
    gbitmap_destroy(s_time_digit_3_bitmap);
    gbitmap_destroy(s_time_digit_4_bitmap);
    APP_LOG(APP_LOG_LEVEL_INFO, "destroyed bitmaps");

    // destroy layers
    bitmap_layer_destroy(s_background_layer);
    uint8_t i;
    for(i = 0; i < sizeof(s_date_digit_layer_array)/sizeof(s_date_digit_layer_array[0]); i++){
        bitmap_layer_destroy(s_date_digit_layer_array[i]);
    }
    for(i = 0; i < sizeof(s_time_digit_layer_array)/sizeof(s_time_digit_layer_array[0]); i++){
        bitmap_layer_destroy(s_time_digit_layer_array[i]);
    }
    bitmap_layer_destroy(s_time_of_day_layer);
    bitmap_layer_destroy(s_weather_icon_layer);
    APP_LOG(APP_LOG_LEVEL_INFO, "destroyed bitmap layers");
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    render_time();
    
    if(tick_time->tm_min % 15 == 0) {
        // send junk message to start weather fetch
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);
        dict_write_uint8(iter, 0, 0);
        app_message_outbox_send();
    }
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    // store incoming information
    uint8_t tmp_condition = 9;

    // iterate through payload
    Tuple *t;
    for(t = dict_read_first(iterator); t != NULL; t = dict_read_next(iterator)) {
        switch(t->key) {
        case KEY_CONDITIONS:
            tmp_condition = (int)t->value->uint8;
            break;
        default:
            APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
            break;
        }
    }
    
    render_weather(tmp_condition);
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