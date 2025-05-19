LVGL container
======================

This repo is a wrapper for LVGL v9.2.
This repo contains LVGL patches that are needed to use LVGL on an XCORE.
It added attributes to all function pointer in LVGL.

To use LVGL with this libaray, you need to add callback functions that are passed to LVGL to corresponding function pointer groups.

For example,
```c
__attribute__(( fptrgroup("lv_dummy_cb") ))
void my_callback(lv_obj_t * obj, lv_event_t event)
{
    // Your callback code here
}
```

Available function pointer groups are

.. csv-table:: Function pointer groups
    :header: "Name", "Function", "Used by"
    "lv_event_cb",              "void (*event_cb)(const lv_obj_class_t *, lv_event_t *)",       "lv_obj_class_t.event_cb or lv_event_dsc_t.cb"
    "lv_obj_constructor_cb",    "void (*constructor_cb)(const lv_obj_class_t *, lv_obj_t *)",   "lv_obj_class_t.constructor_cb"
    "lv_obj_destructor_cb",     "void (*destructor_cb)(const lv_obj_class_t *, lv_obj_t *)",    "lv_obj_class_t.destructor_cb"
    "lv_lcd_send_cmd_cb",       "void (*lv_lcd_send_cmd_cb_t)(lv_display_t *, const uint8_t *, size_t, const uint8_t *, size_t)",   "lv_lcd_generic_mipi_driver_t.send_cmd"
    "lv_lcd_send_color_cb",     "void (*lv_lcd_send_color_cb_t)(lv_display_t *, const uint8_t *, size_t, uint8_t *, size_t)",       "lv_lcd_generic_mipi_driver_t.send_color"
    "lv_indev_read_cb",         "void (*lv_indev_read_cb_t)(lv_indev_t *, lv_indev_data_t *)",  "lv_indev_t.read_cb"
    "lv_async_info_cb",         "void (*lv_async_cb_t)(void *)",                                        "lv_async_info_t.cb"
    "lv_cache_compare_cb",      "lv_rb_compare_res_t (*lv_rb_compare_t)(const void *, const void *)",   "lv_rb_t.compare"
    "lv_timer_cb",              "void (*lv_timer_cb_t)(lv_timer_t *)",                      "lv_timer_t.timer_cb"
    "lv_timer_resume_cb",       "void (*lv_timer_handler_resume_cb_t)(void *)",             "lv_timer_state_t.resume_cb"
    "lv_tick_get_cb",           "uint32_t (*lv_tick_get_cb_t)(void)",                       "lv_tick_state_t.tick_get_cb_t"
    "lv_tick_delay_cb",         "void (*lv_delay_cb_t)(uint32_t)",                          "lv_tick_state_t.delay_cb"
    "lv_anim_exec_cb",          "void (*lv_anim_exec_xcb_t)(void *, int32_t)",              "lv_anim_t.exec_cb"
    "lv_anim_custom_exec_cb",   "void (*lv_anim_custom_exec_cb_t)(lv_anim_t *, int32_t)",   "lv_anim_t.custom_exec_cb"
    "lv_anim_start_cb",         "void (*lv_anim_start_cb_t)(lv_anim_t *)",                  "lv_anim_t.start_cb"
    "lv_anim_completed_cb",     "void (*lv_anim_completed_cb_t)(lv_anim_t *)",              "lv_anim_t.completed_cb"
    "lv_anim_deleted_cb",       "void (*lv_anim_deleted_cb_t)(lv_anim_t *)",                "lv_anim_t.deleted_cb"
    "lv_anim_get_value_cb"      "int32_t (*lv_anim_get_value_cb_t)(lv_anim_t *)",           "lv_anim_t.get_value_cb"
    "lv_anim_path_cb",          "int32_t (*lv_anim_path_cb_t)(const lv_anim_t *)",          "lv_anim_t.path_cb or lv_obj_style_transition_dsc_t.path_cb"
    "lv_focus_cb",              "void (*lv_group_focus_cb_t)(lv_group_t *)", "lv_group_t.focus_cb"
    "lv_edge_cb",               "void (*lv_group_edge_cb_t)(lv_group_t *, bool)", "lv_group_t.edge_cb"
    "lv_layout_dsc_cb",         "void (*lv_layout_update_cb_t)(lv_obj_t *, void *)",        "lv_layout_dsc_t.cb"
    "lv_cache_compare_cb",      "lv_cache_compare_res_t (*lv_cache_compare_cb_t)(const void *, const void *)",  "lv_cache_ops_t.compare_cb"
    "lv_cache_create_cb",       "bool (*lv_cache_create_cb_t)(void *, void *)",                                 "lv_cache_ops_t.create_cb"
    "lv_cache_free_cb",         "void (*lv_cache_free_cb_t)(void *, void *)",                                   "lv_cache_ops_t.free_cb"
    "lv_cache_alloc_cb",        "void * (*lv_cache_alloc_cb_t)(void)",                                                                  "lv_cache_class_t.alloc_cb"
    "lv_cache_init_cb",         "bool (*lv_cache_init_cb_t)(lv_cache_t *)",                                                             "lv_cache_class_t.init_cb"
    "lv_cache_destroy_cb",      "void (*lv_cache_destroy_cb_t)(lv_cache_t *, void *)",                                                  "lv_cache_class_t.destroy_cb"
    "lv_cache_get_cb",          "lv_cache_entry_t * (*lv_cache_get_cb_t)(lv_cache_t *, const void *, void *)",                          "lv_cache_class_t.get_cb"
    "lv_cache_add_cb",          "lv_cache_entry_t * (*lv_cache_add_cb_t)(lv_cache_t *, const void *, void *)",                          "lv_cache_class_t.add_cb"
    "lv_cache_remove_cb",       "void (*lv_cache_remove_cb_t)(lv_cache_t *, lv_cache_entry_t *, void *)",                               "lv_cache_class_t.remove_cb"
    "lv_cache_drop_cb",         "void (*lv_cache_drop_cb_t)(lv_cache_t *, const void *, void *)",                                       "lv_cache_class_t.drop_cb"
    "lv_cache_drop_all_cb",     "void (*lv_cache_drop_all_cb_t)(lv_cache_t *, void *)",                                                 "lv_cache_class_t.drop_all_cb"
    "lv_cache_get_victim_cb",   "lv_cache_entry_t * (*lv_cache_get_victim_cb)(lv_cache_t *, void *)",                                   "lv_cache_class_t.get_victim_cb"
    "lv_cache_reserve_cond_cb", "lv_cache_reserve_cond_res_t (*lv_cache_reserve_cond_cb)(lv_cache_t *, const void *, size_t, void *)",  "lv_cache_class_t.reserve_cond_cb"
    "lv_fragment_constructor_cb",     "void (*constructor_cb)(lv_fragment_t *, void *)",                        "lv_fragment_class_t.constructor_cb"
    "lv_fragment_destructor_cb",      "void (*destructor_cb)(lv_fragment_t *)",                                 "lv_fragment_class_t.destructor_cb"
    "lv_fragment_attached_cb",        "void (*attached_cb)(lv_fragment_t *)",                                   "lv_fragment_class_t.attached_cb"
    "lv_event_cb",                    "void (*detached_cb)(lv_fragment_t *)",                                   "lv_fragment_class_t.detached_cb
    "lv_fragment_create_obj_cb",      "lv_obj_t * (*create_obj_cb)(lv_fragment_t *, lv_obj_t *)",               "lv_fragment_class_t.create_obj_cb"
    "lv_fragment_obj_created_cb",     "void (*obj_created_cb)(lv_fragment_t *, lv_obj_t *)",                    "lv_fragment_class_t.obj_created_cb"
    "lv_fragment_obj_will_delete_cb", "void (*obj_will_delete_cb)(lv_fragment_t *, lv_obj_t *)",                "lv_fragment_class_t.obj_will_delete_cb"
    "lv_fragment_obj_deleted_cb",     "void (*obj_deleted_cb)(lv_fragment_t *, lv_obj_t *)",                    "lv_fragment_class_t.obj_deleted_cb"
    "lv_fragment_event_cb",           "bool (*event_cb)(lv_fragment_t *, int code, void *)",                    "lv_fragment_class_t.event_cb"
    "lv_observer_cb",                 "void (*lv_observer_cb_t)(lv_observer_t *, lv_subject_t *)",              "lv_observer_t.cb"
    "lv_display_flush_cb",      "void (*lv_display_flush_cb_t)(lv_display_t *, const lv_area_t *, uint8_t *)",  "lv_display_t.flush_cb"
    "lv_display_flush_wait_cb", "void (*lv_display_flush_wait_cb_t)(lv_display_t *)",                           "lv_display_t.flush_wait_cb"
    "lv_display_layer_init",    "void (*layer_init)(lv_display_t *, lv_layer_t *)",                             "lv_display_t.layer_init"
    "lv_display_layer_deinit",  "void (*layer_deinit)(lv_display_t *, lv_layer_t *)",                           "lv_display_t.layer_deinit"
    "lv_lru_get_data_size_cb_cb",   "uint32_t (get_data_size_cb_t)(const void *)",                              "lv_lru_rb_t_.get_data_size_cb"
    "lv_draw_buf_malloc_cb",        "void * (*lv_draw_buf_malloc_cb)(size_t, lv_color_format_t)",                       "lv_draw_buf_handlers_t.buf_malloc_cb"
    "lv_draw_buf_free_cb",          "void (*lv_draw_buf_free_cb)(void *)",                                              "lv_draw_buf_handlers_t.buf_free_cb"
    "lv_draw_align_pointer_cb",     "void * (*lv_draw_buf_align_cb)(void *, lv_color_format_t)",                        "lv_draw_buf_handlers_t.align_pointer_cb"
    "lv_draw_invalidate_cache_cb",  "void (*lv_draw_buf_cache_operation_cb)(const lv_draw_buf_t *, const lv_area_t *)", "lv_draw_buf_handlers_t.invalidate_cache_cb"
    "lv_draw_flush_cache_cb",       "void (*lv_draw_buf_cache_operation_cb)(const lv_draw_buf_t *, const lv_area_t *)", "lv_draw_buf_handlers_t.flush_cache_cb"
    "lv_draw_width_to_stride_cb",   "uint32_t (*lv_draw_buf_width_to_stride_cb)(uint32_t, lv_color_format_t)",          "lv_draw_buf_handlers_t.width_to_stride_cb"
    "lv_draw_sw_mask_cb",           "lv_draw_sw_mask_res_t (*lv_draw_sw_mask_xcb_t)(lv_opa_t *, int32_t, int32_t, int32_t, void *)",  "lv_draw_sw_mask_common_dsc_t.cb"
    "lv_image_deocder_info_cb",     "lv_result_t (*lv_image_decoder_info_f_t)(lv_image_decoder_t *, lv_image_decoder_dsc_t *, lv_image_header_t *)",                  "lv_image_decoder_t.info_cb"
    "lv_image_deocder_open_cb",     "lv_result_t (*lv_image_decoder_open_f_t)(lv_image_decoder_t *, lv_image_decoder_dsc_t *)",                                       "lv_image_decoder_t.open_cb"
    "lv_image_deocder_get_area_cb", "lv_result_t (*lv_image_decoder_get_area_cb_t)(lv_image_decoder_t *, lv_image_decoder_dsc_t *, const lv_area_t *, lv_area_t *)",  "lv_image_decoder_t.get_area_cb"
    "lv_image_deocder_close_cb",    "void (*lv_image_decoder_close_f_t)(lv_image_decoder_t *, lv_image_decoder_dsc_t *)",                                             "lv_image_decoder_t.close_cb"
    "lv_font_get_glyph_dsc",        "bool (*get_glyph_dsc)(const lv_font_t *, lv_font_glyph_dsc_t *, uint32_t, uint32_t)",  "lv_font_t.get_glyph_dsc"
    "lv_font_get_glyph_bitmap",     "const void * (*get_glyph_bitmap)(lv_font_glyph_dsc_t *, lv_draw_buf_t *)",             "lv_font_t.get_glyph_bitmap"
    "lv_font_release_glyph",        "void (*release_glyph)(const lv_font_t *, lv_font_glyph_dsc_t *)",                      "lv_font_t.release_glyph"
    "lv_fs_ready_cb",               "bool (*ready_cb)(lv_fs_drv_t *)",                                                    "lv_fs_drv_t.ready_cb"
    "lv_fs_open_cb",                "void * (*open_cb)(lv_fs_drv_t *, const char *, lv_fs_mode_t)",                       "lv_fs_drv_t.open_cb"
    "lv_fs_close_cb",               "lv_fs_res_t (*close_cb)(lv_fs_drv_t *, void *)",                                     "lv_fs_drv_t.close_cb"
    "lv_fs_read_cb",                "lv_fs_res_t (*read_cb)(lv_fs_drv_t *, void *, void *, uint32_t, uint32_t *)",        "lv_fs_drv_t.read_cb"
    "lv_fs_write_cb",               "lv_fs_res_t (*write_cb)(lv_fs_drv_t *, void *, const void *, uint32_t, uint32_t *)", "lv_fs_drv_t.write_cb"
    "lv_fs_seek_cb",                "lv_fs_res_t (*seek_cb)(lv_fs_drv_t *, void *, uint32_t, lv_fs_whence_t)",            "lv_fs_drv_t.seek_cb"
    "lv_fs_tell_cb",                "lv_fs_res_t (*tell_cb)(lv_fs_drv_t *, void *, uint32_t *)",                          "lv_fs_drv_t.tell_cb"
    "lv_fs_dir_open_cb",            "void * (*dir_open_cb)(lv_fs_drv_t *, const char *)",                                 "lv_fs_drv_t.dir_open_cb"
    "lv_fs_dir_read_cb",            "lv_fs_res_t (*dir_read_cb)(lv_fs_drv_t *, void *, char *, uint32_t)",                "lv_fs_drv_t.dir_read_cb"
    "lv_fs_dir_close_cb",           "lv_fs_res_t (*dir_close_cb)(lv_fs_drv_t *, void *)",                                 "lv_fs_drv_t.dir_close_cb"
    "lv_profiler_tick_get_cb",      "uint32_t (*tick_get_cb)(void)",              "lv_profiler_builtin_config_t.tick_get_cb"
    "lv_profiler_flush_cb",         "void (*flush_cb)(const char * buf)",         "lv_profiler_builtin_config_t.flush_cb"
    "lv_profiler_tid_get_cb",       "int (*tid_get_cb)(void)",                    "lv_profiler_builtin_config_t.tid_get_cb"
    "lv_profiler_cpu_get_cb",       "int (*cpu_get_cb)(void)",                    "lv_profiler_builtin_config_t.cpu_get_cb"
    "lv_scene_create_cb",       "void (*create_cb)(void)",          "scene_dsc_t.create_cb"
    "lv_demo_entry_cb",         "void (*demo_method_cb)(void)",     "demo_entry_info_t.entry_cb"