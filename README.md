![Microchip](docs/src/microchip_logo.png)

# Microchip Graphics Toolkit

MUI is a C++ GUI toolkit for Linux on Microchip AT91/SAMA5 parts.  MUI provides
modern GUI functionality, look-and-feel, and performance.

## Features

- Antialiased 2D vector graphics.
- Optimized for fast redrawing without hardware acceleration.
- X11, DRM/KMS, and fbdev backends.
- Support for libplanes and hardware LCD planes.
- Animations with a variety of default easing functions.
- A rich widget set designed for embedded touchscreens with everything from buttons, to sliders, to charts.
- Customizable widgets and color schemes.
- evdev, tslib, mouse, touchscreen, and keyboard input.
- Entire library and simple application in under 50K bytes.
- Modern C++11 design and simplicity.

## Dependencies

- libplanes >= 1.0.0
- libdrm >= 2.4.0
- cJSON >= 1.6.0
- lua >= 5.3.1
- cairo >= 1.14.6
- tslib >= 1.15
- gstreamer >= 1.12
- libjpeg
- rapidxml

## License

MUI is released under the terms of the `MIT` license. See the `COPYING`
file for more information.
