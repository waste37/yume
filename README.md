# Yume GUI library
This library is intended to be in the set of basic tools is will use from now on to do my projects.

The idea behind it is based on the concept of the [clay](https://https://github.com/nicbarker/clay) library,
however yume is implemented entirely from scratch.
The objective of this library is learning immediate UI. If you want to use a library that has the features of yume, use **Clay**, yume is not and will never be dependable.

The library will be ultra modular, in the sense that it provides a way to:
- specify declaratively your GUI view
- get information about what to draw, where and other details like colors or font/text contents
- pass in pointer information, to make queries on hovering, clicking and scrolling. The scrolling will be handled.
- send out errors to an error handler function.

All the outside components such as:
- IO handler
- layout declarations manager
- Actual UI components (buttons, sliders, etc)
- renderer

Will depend on the yume interface, which shouldn't change, or at least only increase.
