#ifndef YUME_H
#define YUME_H

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

/* MACROS *****************************************************************************************/

#define sizeof(x)  (ptrdiff_t)sizeof(x)
#define countof(x) (sizeof(x)/sizeof(x[0]))
#define lengthof(x)  (countof(x) - 1)
#define alignof(x)  _Alignof(x)

#define YUME_MAX_HEIGHT (1 << 20)
#define YUME_COLOR(hex) (yume_color){ .a = (hex >> 24) & 0xff, .r = (hex >> 16) & 0xff, .g = (hex >> 8) & 0xff, .b = hex & 0xff }
#define YUME_S(s) (yume_str){ .ptr = s, .len = lengthof(s) }
#define YUME_BORDER_ALL(px) (yume_border_width) { px, px, px, px }
#define YUME_PADDING_ALL(px) (yume_padding) { px, px, px, px }
#define YUME_SIZING_FILL(...)  (yume_sizing_axis){ .size.bounds = { __VA_ARGS__ }, .type = YUME__SIZING_TYPE_FILL }
#define YUME_SIZING_FLEX(...) (yume_sizing_axis){ .size.bounds = { __VA_ARGS__ }, .type = YUME__SIZING_TYPE_FLEX }
#define YUME_SIZING_PERCENT(percent_of_parent) (yume_sizing_axis){ .size = { .percent = percent_of_parent }, .type = YUME__SIZING_TYPE_PERCENT }
#define YUME_SIZING_FIXED(s) (yume_sizing_axis){ .size.bounds = { .min = s, .max = s }, .type = YUME__SIZING_TYPE_FIXED } 

static uint8_t YUME__MAIN_MACRO_HELPER = 0; 
#define YUME(...)                                                                        \
    for (YUME__MAIN_MACRO_HELPER = (                                                     \
            yume__open_node(),                                                           \
            yume__configure_open_node(((yume__config_wrapper){ __VA_ARGS__ }).wrapped),  \
            0);                                                                          \
        YUME__MAIN_MACRO_HELPER < 1; YUME__MAIN_MACRO_HELPER = 1, yume__close_node()     \
    )                                                                                    \

/* BASIC TYPES ************************************************************************************/

typedef struct {
    ptrdiff_t len;
    char *ptr;
} yume_str;

typedef struct { 
    float a, r, g, b; 
} yume_color;

typedef struct {
    float x, y;
} yume_vec;

typedef struct {
    float x, y, w, h;
} yume_aabb;

/* CONFIGURATION OPTIONS **************************************************************************/

typedef struct {
    uint32_t id;
    yume_str string_id;
} yume_id;

typedef enum {
    YUME__SIZING_TYPE_FILL = 0, /* Grow trying to fill the parent */
    YUME__SIZING_TYPE_FLEX,     /* Grow trying to fit the children */
    YUME__SIZING_TYPE_PERCENT,  /* Size relative to the parent */
    YUME__SIZING_TYPE_FIXED,    /* Size fixed */
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
    YUME__LAYOUT_ALIGNMENT_START, /* uninitialized means align children to the beginning of the axis, i.e. top left */
    YUME__LAYOUT_ALIGNMENT_CENTER,
    YUME__LAYOUT_ALIGNMENT_END,
} yume__layout_alignment;

typedef struct {
    yume__layout_alignment align_x;
    yume__layout_alignment align_y;
} yume_content_alignment;

typedef enum {
    YUME_ORIENTATION_VERTICAL = 0, /* uninitialized means orient children vertically */ 
    YUME_ORIENTATION_HORIZONTAL,
} yume_layout_orientation;

/* This struct specifies the layout of a node: size, orientation of children, padding etc. */
typedef struct {
    yume_sizing sizing;
    yume_padding padding;
    uint16_t gap;
    yume_content_alignment align_content;
    yume_layout_orientation orientation;
} yume_layout_config;

/* Allows to set an image background to a node.*/
typedef struct { 
    void *data; 
} yume_image_config;

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
    yume_id id;
    /* All the possible options on the layout of the node and of its children. */
    yume_layout_config layout;
    /* If image.data is nonzero, the image will be layed over the background color,
     * and alpha blending is done by the renderer */
    yume_image_config image; 
    /* the background color of the node. */
    yume_color background_color;
    /* The border of this layout node */
    yume_border_config border;
} yume_layout_node_config;


/* PUBLIC FUNCTION DECLARATIONS *******************************************************************/

/* Opaque handle to the library context. A view is nowthing more than a
 * single "page" in the library, and it manages all the state related to it.
 * You will pass it a big enough chunk of memory at startup, and that will be
 * everything it will depend on! */

typedef struct yume_view yume_view;
yume_view *yume_create_view(yume_vec dimensions);
void yume_set_view(yume_view *view);
yume_view *yume_get_view();
void yume_begin_definition();
void yume_end_definition();


/**************************************************************************************************
 *    PRIVATE FUNCTIONS and IMPLEMENTATION                                                        *
 **************************************************************************************************/


#ifdef YUME_IMPLEMENTATION

typedef struct { yume_layout_node_config wrapped; } yume__config_wrapper;

typedef struct {
    yume_layout_node_config config;
    int children[100];
    int children_count;
} yume__layout_node;

struct yume_view {
    yume_vec dimensions;

    /* Scene tree implementation */
    yume__layout_node nodes[100];
    int node_nesting[100];
    int node_count;
    int current_open_node;
    int nesting;
};

yume_view *yume__current_view_instance = 0;

yume__layout_node *yume__get_current_node(yume_view *view)
{
    return view->nodes + view->current_open_node;
}

void yume__open_node(void) 
{
    yume_view *view = yume_get_view();
    assert(view->node_count < 100 && "Max nodes reached!");
    view->node_nesting[view->node_count] = ++view->nesting;
    view->current_open_node = view->node_count++;
}

void yume__configure_open_node(yume_layout_node_config configuration) 
{
    yume_view *view = yume_get_view();
    yume__layout_node *node = yume__get_current_node(view);
    node->config = configuration;
    // here we could set the min dimensions?
}

void yume__close_node(void) 
{
    yume_view *view = yume_get_view();
    int closing_index = view->current_open_node;
    yume__layout_node *closing_node = yume__get_current_node(view);

    view->nesting--;
    for (int i = view->current_open_node; i >= 0; --i) {
        if (view->nesting == view->node_nesting[i]) {
            printf("node %d parent is %d\n", view->current_open_node, i);
            view->nodes[i].children[view->nodes[i].children_count++] = view->node_count - 1;
            view->current_open_node = i;
            break;
        }
    }
    // here we could grow them since we are already visiting
    for (int i = 0; i < closing_node->children_count; ++i) {
        int child_index = closing_node->children[i];
        yume__layout_node *child = view->nodes + child_index;
        // if the closing_node is grow, try to grow it.
        // if the child node is fit, try to fit
    }
}

void yume__reset_view_memory(yume_view *view)
{
    view->nesting = -1;
    view->node_count = 0;
}


void yume_begin_definition() 
{
    yume_view *view = yume_get_view();
    yume__reset_view_memory(view);

    yume__open_node();
    yume__configure_open_node((yume_layout_node_config) {
        .id = { .string_id = YUME_S("yume__root_node"), .id = 0, },
        .background_color = YUME_COLOR(0x00000000),
        .layout = {
            .sizing = { .width = YUME_SIZING_FIXED(view->dimensions.x), .height = YUME_SIZING_FLEX(view->dimensions.y, YUME_MAX_HEIGHT) },
            .orientation = YUME_ORIENTATION_VERTICAL,
            .gap = 0,
            .align_content = { YUME__LAYOUT_ALIGNMENT_START, YUME__LAYOUT_ALIGNMENT_START },
            .padding = YUME_PADDING_ALL(10)
        },
    });

}

void yume_end_definition() 
{
    yume__close_node();
}

yume_view *yume_create_view(yume_vec dimensions)
{
    // TODO: implement arena to avoid malloc!
    yume_view *view = malloc(sizeof *view);
    yume_set_view(view);
    view->dimensions = dimensions;
    yume__reset_view_memory(view);
    return view;
}

void yume_set_view(yume_view *view)
{
    yume__current_view_instance = view;
}

yume_view *yume_get_view()
{
    return yume__current_view_instance;
}

#endif /* YUME_IMPLEMENTATION */

#endif /* YUME_H */
