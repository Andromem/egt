@page animations Animations

[TOC]

This chapter discusses how to to use animations.

@section animations_classes Animation Classes

EGT provides a variety of classes for working with animations.  The
egt::Animation, egt::AnimationSequence, egt::AutoAnimation, and
egt::PropertyAnimatorType classes all provide ways to animate almost anything.
These classes can be instantiated and then attached to any property of widgets;
for example, a widget's position or even egt::Font size.  For that matter, you
don't even have to animate widgets specifically.  For example, you could provide
values from an animation class right into the brightness value of an LCD
controller to slowly fade out the brightness of an LCD display over some time
you specify.

On top of these base animation classes, there are also higher order classes such
as Sprite that provide a built in way to animate sprite sheets and
even different strips within the same sheet.  A benefit of having EGT do this is
it can use hardware accelerated composition to animate the Sprite when
available.

There are a couple basic concepts to understand when working with animations in
EGT.  An animation, in its most basic form, is some computed @b value in some
@b range over some period of @b time.  If we were to graph this linearly with
time on the X axis and a value on the Y axis, it would look like this.

@image html easing_graph.png "Easing Curve Graph"
@image latex easing_graph.png "Easing Curve Graph" width=8cm

Of course, the interesting question is how is that value computed over a
specified timeframe? The most straightforward answer is just linearly as shown.
The min value is at the starting time and the max value is at the ending time, and
every value in between is evenly spread out. However, more complex versions of
this are necessary to add some life and diversity to animations, which leads to
the idea of @b easing @b functions. Easing functions define what that value
should be, linear or otherwise, at a given point in time for the animation.

@section animations_easingfunctions Predefined Easing Functions

There are quite a few predefined @ref easing_functions in EGT, but it is also
possible to define your own.  When creating any animation, you can pass a custom
easing function or select one of the predefined ones.

All easing functions take a percent time, and return a percent value.  They have
a very simple API, but what happens inside the easing function can be anywhere
from a simple linear value to a complex mathematical curve. For example, the
simplest of easing functions is the egt::easing_linear() function that is
implemented like this:

@code{.cpp}
EasingScalar easing_linear(EasingScalar p)
{
    return p;
}
@endcode

@imageSize{linear.png,width:320px;}
@image html easing/linear.png "Linear Easing Curve"
@image latex easing/linear.png "Linear Easing Curve" width=8cm

A more complicated easing function might include a circular calculation such
that the curve eases the value up slowly at first and then quickly at the end:

@code{.cpp}
EasingScalar easing_circular_easein(EasingScalar p)
{
    return 1 - std::sqrt(1 - (p * p));
}
@endcode

@imageSize{circular_easein.png,width:320px;}
@image html easing/circular_easein.png "Circular Easein Easing Curve"
@image latex easing/circular_easein.png "Circular Easein Easing Curve" width=8cm

The @b examples/easing/ directory contains an example that allows you to
visually select and see some of the different easing functions and how they
work.

@see [easings.net](https://easings.net) has more information about other easing
functions.

@section animations_animating Animating Widgets

For example, to change the Y position of an egt::Button, it might involve doing
the following:

@snippet "../examples/snippets/snippets.cpp" animation1.1
