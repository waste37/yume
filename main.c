#define YUME_IMPLEMENTATION
#include "yume.h"

int main(void)
{
    yume_view *view = yume_create_view((yume_vec){1024, 728});

    //while (1) {

    yume_begin_definition();
    YUME({}) { // 1

        YUME({}) { // 2

        }

        YUME({}) {  // 3
            YUME({}) { // 4

                YUME({}); // 5
            }

            YUME({}) { // 6

            }
        }

        YUME({ // 7
            .layout.sizing = { 
                .width = YUME_SIZING_FIXED(100), .height = YUME_SIZING_FLEX(0, 100) 
            },
            .background_color = YUME_COLOR(0x000000ff)
        });
    }
    yume_end_definition();

    //}



    free(view);
    return 0;
}
