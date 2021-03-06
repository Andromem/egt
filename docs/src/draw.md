@page draw Drawing

[TOC]

This chapter discusses how drawing works and how rectangles are marked for
redraw.

@section draw_loop Damaging, Drawing, and the Widget Tree

Each widget has an egt::v1::Widget::box(), which holds the bounding rectangle for
the widget. This is the rectangle a widget is responsible for at some z-order
for what content should be placed inside that bounding box.

Widgets are organized into a tree.  At the top of the tree there is a top level
widget that is a special type of widget called an egt::v1::Window.  An
egt::v1::Window is a frame widget, which is a composing widget.  This is just
another way of saying a frame can deal with zero or more children widgets inside
of its egt::v1::Widget::box().

Widgets are essentially rectangles that are drawn.  The widget defines the
content of what is drawn, but all that matters to the widget is its rectangle
of content.  That widget rectangle is directly drawn into a single global
back buffer for the screen.  This is an internal buffer in memory.
This back buffer is continuously updated in place by redrawing damaged
rectangles using the widgets that cover it.  When all of the rectangles have
been drawn in a single iteration of the egt::v1::EventLoop, only the damaged
region of that back buffer is then copied to a display buffer.  The
screen may actually have more than one buffer in order to handle double
or triple buffering.  So, when switching between these buffers, the screen has
to manage damaged regions for each of the buffers.

This means there are two flows up and down the widget hierarchy.  One is
propagation of the damaged rectangles, and the other is propagation of redrawing
the damaged rectangles with the correct content.

Damage propagates *up* in the tree.  If a leaf node widget is damaged, it will
ultimately report the damaged rectangle to a parent frame that has a surface.
This is usually the top level frame or the special plane frame.

@image html widget_hierarchy_damage.png "Damage"
@image latex widget_hierarchy_damage.png "Damage" width=8cm

Drawing goes the other way and is initiated by the EventLoop.  The
EventLoop will call on all top level frames and special plane frames to
redraw their damaged regions.

@image html widget_hierarchy_draw.png "Draw"
@image latex widget_hierarchy_draw.png "Draw" width=8cm

@section draw_paint Painting

Typically, drawing should only be done inside the egt::v1::Widget::draw()
method.

Typically, EGT default widgets have another level of indirection to implement
the draw() function by using the Drawer class.  This allows draw
functions to be overwritten by the theme.  However, this is not a technical
requirement if there is no need to theme the draw() method of a widget.
See @ref colors_themes "Widget Theme" for more information.

When a widget is asked to draw, it usually uses the Painter class to perform
all drawing.  However, underneath the Painter class is a cairo context and
surface.  This means the draw function for a widget can be implemented
completely with Painter by using a combination of Painter and cairo directly,
or just using the cairo API directly.

The Painter API provides basic 2D primitive drawing functions that work well
with EGT primitives like colors and geometry classes. Painter can be used to
draw lines, rectangles, fills, and fonts.  It even supports more complicated
drawing features like gradients and arcs.

When using Painter to draw, there are also several other common
properties and resources used.  For example, the palette of the widget is
usually referenced, along with its font when applicable, and also the use of
color comes into play.

@section draw_cairo Painting Directly with Cairo

The back-end used by Painter is the excellent
[cairo graphics](https://www.cairographics.org/) library, which in turn uses
[pixman](http://www.pixman.org/) for optimized pixel manipulation.  Both cairo
and pixman take advantage of hardware acceleration when possible.

To use cairo directly in an egt::v1::Widget::draw() method, get the cairo context by
calling egt::v1::Painter::context(), and then call cairo API functions like normal.

@code{.cpp}
auto cr = painter.context().get();
cairo_set_line_width(cr, 1.0);
cairo_stroke(cr);
@endcode

Understanding the cairo drawing model, considering it is a vector drawing API,
is important to understand how to use cairo, and in some cases how to use the
Painter class in EGT.

@imageSize{architecture.png,width:500px;}
@image html cairo_drawing.png "Cairo Drawing Model [CC BY-SA 3.0 (https://creativecommons.org/licenses/by-sa/3.0)]"
@image latex cairo_drawing.png "Cairo Drawing Model [CC BY-SA 3.0 (https://creativecommons.org/licenses/by-sa/3.0)]" width=8cm

The
[Cairo Manual](https://www.cairographics.org/manual/index.html) provides
excellent documentation of the cairo API and is a useful reference.  If you are
not familiar with cairo, the
[Cairo Tutorial](https://www.cairographics.org/tutorial/) information is very
helpful to understand the basics.

@section draw_function Implementing a Draw Function

The best way to see how typical widgets are drawn is to look at the draw()
functions of default EGT widgets.  However, there are several things to take
into account when implementing a draw() function.

By default, a parent frame will first clip the drawing area to the box() of the
widget that it is asked to draw().  This means that even if the widget tries to
draw outside of its box(), it will not be seen.  This is important because EGT
needs to know where things are drawn so it can account for repainting as
necessary. If any widget were allowed to draw anywhere on the screen at any time,
EGT would have no idea what needs to be redrawn.  To circumvent this operation
at your own risk, set the egt::v1::Widget::Flag::no_clip flag on a widget.  This will
disable automatic clipping.

When the parent frame calls the draw() function of a widget, it will first setup
an egt::v1::Painter::AutoSaveRestore instance.  Therefore, there is no need to handle saving or
restoring the state in any egt::v1::Widget::draw() function as this is done automatically.

@section draw_transforms Affine Matrix Transformations

Cairo supports matrix transformations of the coordinate system.  This is an
advanced topic.  For more information on how this works, see
[Cairo Matrix Transformation](https://cairographics.org/cookbook/matrix_transform/).

An example of doing a transformation in a class derived from an ImageLabel might
look like this:

@code{.cpp}
virtual void draw(Painter& painter, const Rect& rect) override
{
    auto cr = painter.context().get();

    float yrotate;
    if (m_scale > 0.5)
        yrotate = .35 * (m_scale - .5);
    else
        yrotate = -1 * .35 * (.5 - m_scale);

    cairo_matrix_t matrix_perspective;
    cairo_matrix_init(&matrix_perspective,
                      1.0, yrotate,
                      0.0, 1.0,
                      parent()->w() / 2, 0.0);

    cairo_transform(cr, &matrix_perspective);
    cairo_translate(cr, -parent()->w() / 2, 0);

    Drawer<ImageLabel>::draw(*this, painter, rect);
}
@endcode

@section draw_zorder Z-order

The z-order of widgets is the order in which they are drawn or composed.  It is
also the order in which they receive events through the widget hierarchy as
explained in @ref events_prop. Widgets are drawn from the bottom up.  By
default, widgets are drawn in the order in which they are added to a Frame.

If you add() a Label, and then add() a background ImageLabel that covers the
entire window, you will not see the label unless the background image is
transparent. To change this, you have to modify the z-order of the widgets by
either lowering the ImageLabel with egt::v1::Widget::zorder_down() or raising
the Label with egt::v1::Widget::zorder_up().
