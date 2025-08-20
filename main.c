#include <stdio.h>
#include <stdint.h>


/* Opaque handle to the library context. A view is nowthing more than a
 * single "page" in the library, and it manages all the state related to it.
 * You will pass it a big enough chunk of memory at startup, and that will be
 * everything it will depend on! */
typedef struct yume_view yume_view;

yume_view *yume_create_view();
void yume_set_view(yume_view *view);
yume_view *yume_get_view();


#define YUME_SIZING_FIT(...) (yume_sizing_axis){ .size = { .bounds = { __VA_ARGS__ } } .type = YUME__SIZING_TYPE_FIT }
#define YUME_SIZING_GROW(...) (yume_sizing_axis){ .size = { .bounds = { __VA_ARGS__ } } .type = YUME__SIZING_TYPE_GROW }
#define YUME_SIZING_PERCENT(percent_of_parent) (yume_sizing_axis){ .size = { .percent = percent_of_parent } .type = YUME__SIZING_TYPE_PERCENT }
#define YUME_SIZING_FIXED(size) (yume_sizing_axis){ .size = { .bounds = { size, size } } .type = YUME__SIZING_TYPE_FIXED }

typedef enum {
    YUME__SIZING_TYPE_FIT = 0,
    YUME__SIZING_TYPE_GROW,
    YUME__SIZING_TYPE_PERCENT,
    YUME__SIZING_TYPE_FIXED,
} yume__sizing_type;

typedef struct {
    float min, max;
} yume_sizing_interval;

typedef struct {
    union {
        yume_sizing_interval bounds;
        float percent;
    } size;
    yume__sizing_type type;
} yume_sizing_axis;

typedef struct {
    yume_sizing_axis width;
    yume_sizing_axis height;
} yume_sizing;

typedef struct {
    uint16_t left;
    uint16_t right;
    uint16_t top;
    uint16_t bottom;
} yume_padding;

typedef enum {
    YUME__LAYOUT_ALIGNMENT_START,
    YUME__LAYOUT_ALIGNMENT_CENTER,
    YUME__LAYOUT_ALIGNMENT_END,
} yume__layout_alignment;

typedef struct {
    yume__layout_alignment align_x;
    yume__layout_alignment align_y;
} yume_content_alignment;

typedef enum {
    YUME_ORIENTATION_VERTICAL,
    YUME_ORIENTATION_HORIZONTAL,
} yume_layout_orientation;

/*This struct specifies the layout of a node: size, orientation of children, padding etc. */
typedef struct {
    yume_sizing sizing;
    yume_padding padding;
    uint16_t gap;
    yume_content_alignemnt align_content;
    yume_layout_orientation orientation;
} yume_layout_config;

/* Allows to set an image background to a node.*/
typedef struct {
    void *data;
} yume_image_config;

#define YUME_COLOR(hex) (yume_color){ .a = (hex >> 24) & 0xff, .r = (hex >> 16) & 0xff, .g = (hex >> 8) & 0xff, .b = hex & 0xff }
typedef struct { float a, r, g, b } yume_color;

typedef struct {
    uint16_t left;
    uint16_t right;
    uint16_t top;
    uint16_t bottom;
} yume_border_width;

typedef struct {
    yume_border_width width;
    yume_color color;
} yume_border_config;

/* This struct will be the primary interface for the user to specify the look of a view node.
 * Each node configuration will be internally represented using the concepts defined here.
 * This struct is indeed the main public API of the view definition
 * Every node is implicitly treated in a similar fashion to the CSS flexbox. */
typedef struct {
    /* User passed id, used to identify nodes. */
    yume_element_id id;
    /* All the possible options on the layout of the node and of its children. */
    yume_layout_config layout;
    /* If image.data is nonzero, the image will be layed over the background color, and alpha blending is done by the renderer */
    yume_image_config image; 
    /* the background color of the node. */
    yume_color background_color;
    /* The border of this layout node */
    yume_border_config border;
} yume_layout_node_config;


static uint8_t YUME__MAIN_MACRO_HELPER = 0; 
#define YUME(...)                                            \
    for (                                                    \
        YUME__MAIN_MACRO_HELPER = (                          \
            yume__open_node(),                               \
            yume_configure_open_node(__VA_ARGS__),           \
            0);                                              \
        YUME__MAIN_MACRO_HELPER < 1;                         \
        YUME__MAIN_MACRO_HELPER = 1, yume__close_node()      \
    )                                                        \


int main(void)
{
    printf("Hello, yume!\n");
    return 0;
}
