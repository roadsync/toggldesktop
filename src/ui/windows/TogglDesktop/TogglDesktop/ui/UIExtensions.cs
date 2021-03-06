﻿using System;
using System.Drawing;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Forms;
using System.Windows.Media;
using System.Windows.Media.Media3D;
using System.Windows.Threading;
using Control = System.Windows.Controls.Control;
using Panel = System.Windows.Controls.Panel;
using TextBoxBase = System.Windows.Controls.Primitives.TextBoxBase;

namespace TogglDesktop
{
static class UIExtensions
{
    public static bool TryBeginInvoke(this Control control, Action action)
    {
        if (control.Dispatcher.CheckAccess())
            return false;

        control.Dispatcher.BeginInvoke(action);

        return true;
    }

    #region overloads

    public static bool TryBeginInvoke<T0>(this Control control, Action<T0> action, T0 p0)
    {
        return control.TryBeginInvoke(() => action(p0));
    }
    public static bool TryBeginInvoke<T0, T1>(this Control control, Action<T0, T1> action, T0 p0, T1 p1)
    {
        return control.TryBeginInvoke(() => action(p0, p1));
    }
    public static bool TryBeginInvoke<T0, T1, T2>(this Control control, Action<T0, T1, T2> action, T0 p0, T1 p1, T2 p2)
    {
        return control.TryBeginInvoke(() => action(p0, p1, p2));
    }
    public static bool TryBeginInvoke<T0, T1, T2, T3>(this Control control,
            Action<T0, T1, T2, T3> action, T0 p0, T1 p1, T2 p2, T3 p3)
    {
        return control.TryBeginInvoke(() => action(p0, p1, p2, p3));
    }
    public static bool TryBeginInvoke<T0, T1, T2, T3, T4>(this Control control,
            Action<T0, T1, T2, T3, T4> action, T0 p0, T1 p1, T2 p2, T3 p3, T4 p4)
    {
        return control.TryBeginInvoke(() => action(p0, p1, p2, p3, p4));
    }

    #endregion

    public static void ShowOnlyIf(this UIElement control, bool condition)
    {
        control.Visibility = condition
                             ? Visibility.Visible
                             : Visibility.Collapsed;
    }
    public static void ShowOnlyIf(this UIElement control, bool condition, bool hideInsteadOfCollapse)
    {
        control.Visibility = condition
                             ? Visibility.Visible
                             : hideInsteadOfCollapse
                             ? Visibility.Hidden
                             : Visibility.Collapsed;
    }

    public static void RemoveFromParent(this UIElement control)
    {
        var parent = VisualTreeHelper.GetParent(control);
        if (parent == null)
            return;

        var parentAsPanel = parent as Panel;
        if (parentAsPanel != null)
        {
            parentAsPanel.Children.Remove(control);
            return;
        }

        var parentAsContentControl = parent as ContentControl;
        if (parentAsContentControl != null)
        {
            parentAsContentControl.Content = null;
            return;
        }

        var parentAsDecorator = parent as Decorator;
        if (parentAsDecorator != null)
        {
            parentAsDecorator.Child = null;
        }
    }

    public static void ShowOnTop(this Window window)
    {
        window.Show();
        if (window.WindowState == WindowState.Minimized)
        {
            window.WindowState = WindowState.Normal;
        }
        window.Activate();
    }

    public static Rect GetCurrentScreenRectangle(this Window window)
    {
        if (window == null)
        {
            throw new ArgumentNullException(nameof(window), "Window cannot be null");
        }

        var screen = Screen.FromRectangle(new Rectangle(
            (int)window.Left, (int)window.Top,
            (int)window.Width, (int)window.Height
        ));

        var area = screen.WorkingArea;

        var topLeft = new System.Windows.Point(area.Left, area.Top);
        var bottomRight = new System.Windows.Point(area.Right, area.Bottom);

        var presentationSource = PresentationSource.FromVisual(window);
        if (presentationSource != null)
        {
            var compositionTarget = presentationSource.CompositionTarget;
            if (compositionTarget != null)
            {
                var t = compositionTarget.TransformFromDevice;

                topLeft = t.Transform(topLeft);
                bottomRight = t.Transform(bottomRight);
            }
        }

        return new Rect(topLeft, bottomRight);
    }

    public static void ClearUndoHistory(this TextBoxBase textBox)
    {
        textBox.IsUndoEnabled = false;
        textBox.IsUndoEnabled = true;
    }

    public static T FindParent<T>(this DependencyObject element)
        where T : class
    {
        if (element == null) return null;

        var parent = VisualTreeHelper.GetParent(element.FindVisualTreeRoot());
        while (parent != null && !(parent is T))
        {
            parent = VisualTreeHelper.GetParent(parent);
        }

        return parent as T;
    }

    private static DependencyObject FindVisualTreeRoot(this DependencyObject d)
    {
        var current = d;
        var result = d;

        while (current != null)
        {
            result = current;
            if (current is Visual || current is Visual3D)
            {
                break;
            }

            current = LogicalTreeHelper.GetParent(current);
        }

        return result;
    }
}
}