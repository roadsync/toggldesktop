﻿using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Media.Animation;
using TogglDesktop.Diagnostics;

namespace TogglDesktop
{
    public partial class EditViewPopup
    {
        enum AnimationStates
        {
            Opening,
            Closing,
        }

        private AnimationStates animationState = AnimationStates.Closing;

        private bool isLeft;
        private bool skipAnimation;
        private object animationToken;

        private readonly Thickness _leftResizeBorderThickness = new Thickness(8, 0, 0, 0);
        private readonly Thickness _rightResizeBorderThickness = new Thickness(0, 0, 8, 0);

        public EditViewPopup()
        {
            this.InitializeComponent();

            this.MinWidth = this.EditView.MinWidth + 16;
            this.mainGrid.Width = 0;

            Toggl.OnTimeEntryEditor += this.onTimeEntryEditor;

            KeyboardShortcuts.RegisterShortcuts(this);
        }

        private void onTimeEntryEditor(bool open, Toggl.TogglTimeEntryView te, string focusedFieldName)
        {
            if (this.TryBeginInvoke(this.onTimeEntryEditor, open, te, focusedFieldName))
                return;

            if (!this.Owner.IsVisible)
                return;
            
            using (Performance.Measure("opening edit popup"))
            {
                if (this.skipAnimation)
                {
                    this.stopAnimationOpen();
                }
                else
                {
                    this.startAnimationOpen();
                }

                this.Show();

                // focus field only if the edit popup was just open
                if (open)
                {
                    this.EditView.FocusField(focusedFieldName);
                }
            }
        }

        public void ClosePopup(bool skipAnimation = false)
        {
            if (skipAnimation || this.skipAnimation)
            {
                this.stopAnimationClose();
            }
            else
            {
                if (this.animationState == AnimationStates.Closing)
                    return;

                this.startAnimationClose();
            }
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            e.Cancel = true;
            Toggl.ViewTimeEntryList();
        }

        #region animate

        private void startAnimationOpen()
        {
            if (this.animationState == AnimationStates.Opening)
                return;

            this.animationState = AnimationStates.Opening;

            this.setAlignmentsForAnimation();
            this.mainGrid.Width = 0;
            this.EditView.Width = this.ActualWidth;
            this.mainGrid.IsEnabled = true;

            this.startAnimation(0, this.ActualWidth, this.stopAnimationOpen);
        }

        private void stopAnimationOpen()
        {
            this.animationToken = null;

            this.animationState = AnimationStates.Opening;

            this.mainGrid.BeginAnimation(WidthProperty, null);

            this.mainGrid.Width = double.NaN;
            this.EditView.Width = double.NaN;
            this.mainGrid.HorizontalAlignment = HorizontalAlignment.Stretch;
            this.EditView.HorizontalAlignment = HorizontalAlignment.Stretch;
            this.mainGrid.IsEnabled = true;
        }

        private void startAnimationClose()
        {
            if (this.animationState == AnimationStates.Closing)
                return;

            this.animationState = AnimationStates.Closing;

            this.setAlignmentsForAnimation();
            this.EditView.Width = this.ActualWidth;
            this.mainGrid.IsEnabled = false;

            this.startAnimation(this.ActualWidth, 0, this.stopAnimationClose);
        }

        private void stopAnimationClose()
        {
            this.animationToken = null;

            this.animationState = AnimationStates.Closing;

            this.mainGrid.BeginAnimation(WidthProperty, null);

            this.mainGrid.Width = 0;

            this.Hide();
        }

        private void startAnimation(double from, double to, Action stopAction)
        {
            var token = new object();
            this.animationToken = token;

            var animation = new DoubleAnimation(from, to,
               new Duration(TimeSpan.FromSeconds(0.15)),
               FillBehavior.HoldEnd);
            animation.Completed += (s, e) =>
            {
                if (token != this.animationToken)
                {
                    this.animationToken = null;
                    return;
                }
                stopAction();
            };

            this.mainGrid.BeginAnimation(WidthProperty, animation);
        }

        private void setAlignmentsForAnimation()
        {
            if (this.isLeft)
            {
                this.mainGrid.HorizontalAlignment = HorizontalAlignment.Left;
                this.EditView.HorizontalAlignment = HorizontalAlignment.Right;
            }
            else
            {
                this.mainGrid.HorizontalAlignment = HorizontalAlignment.Right;
                this.EditView.HorizontalAlignment = HorizontalAlignment.Left;
            }
        }

        #endregion

        public void SetPlacement(bool left, double x, double y, double maxWidth)
        {
            this.ResizeBorderThickness = left ? _leftResizeBorderThickness : _rightResizeBorderThickness;
            this.EditView.Margin = new Thickness(8);
            this.skipAnimation = false;
            this.EditView.Height = double.NaN;
            this.isLeft = !left;
            x += left ? (8 - this.Width) : -8;
            this.Left = x;
            this.Top = y;
            this.MaxWidth = maxWidth;
        }

        public void SetPlacementMaximized(double x, double y, double height, double maxWidth)
        {
            this.ResizeBorderThickness = new Thickness(0);
            this.EditView.Margin = new Thickness(0);
            this.skipAnimation = true;
            this.EditView.Height = height;
            this.isLeft = false;
            this.Left = x - this.Width;
            this.Top = y;
            this.MaxWidth = maxWidth;
        }
    }
}
