 @page custom Creating New Widgets

This chapter discusses how to extend and customize widgets.

Before creating new widget types, make sure you understand @ref colors_themes
to know what you can already change about how a widget looks and operates.  You
can change colors, fonts, and even the complete draw function of existing widget
instances and types without having to create custom widget types.

EGT is designed to support extension by way of custom widgets and functionality.
Essentially, to change the behavior or look and feel of an existing widget,
create a derived class of a similar and existing widget and reimplement the
functions that should be changed.  If you are creating a completely new type of
widget, you would inherit directly from Widget or one of the other base
widget types.

An example of this happening is fluent throughout the EGT library itself.  For
example, egt::CheckBox implements a standard traditional checkBox control that
shows an *X* when the checkBox is egt::v1::CheckBox::checked().  However, to
change the look and keep the same logical operation, egt::ToggleBox derives from
egt::CheckBox and effectively changes the draw() method.

The new widget class that only intends to change the look of a widget would look
something like this:

@code{.cpp}
class ToggleBox : public CheckBox
{
public:

    // standard draw function, which usually calls Drawer<T>::draw()
    virtual void draw(Painter& painter, const Rect& rect) override
    {
        Drawer<ToggleBox>::draw(*this, painter, rect);
    }

    // default_draw() function to do real drawing which allows this widget to be themeable
    static void default_draw(ToggleBox& widget, Painter& painter, const Rect& rect);

};
@endcode

This new widget is basically a CheckBox, but we want to change how it looks. The
new ToggleBox::default_draw() method looks like this:

@snippet "../src/checkbox.cpp" ToggleBoxDraw
