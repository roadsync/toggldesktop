//
//  NSColor+VisibleColor.swift
//  TogglDesktop
//
//  Created by Nghia Tran on 8/12/19.
//  Copyright © 2019 Alari. All rights reserved.
//

import Foundation

@objc extension NSColor {

    @objc func visibleColor() -> NSColor {
        guard let window = NSApplication.shared.keyWindow else { return self }
        let darkMode = window.isDarkMode
        let bottomThreshold: CGFloat = 75.0 / 255.0
        let topThreshold: CGFloat = 180.0 / 255.0

        // If darkmode and color is black-based
        // or light mode and color is white-based
        if (darkMode && (redComponent + blueComponent + greenComponent) <= 3.0 * bottomThreshold) ||
            (!darkMode && (redComponent + blueComponent + greenComponent) >= 3.0 * topThreshold) {

            // Revert color to make it visible
            let red = 1.0 - redComponent
            let green = 1.0 - greenComponent
            let blue = 1.0 - blueComponent
            return NSColor(calibratedRed: red, green: green, blue: blue, alpha: 1.0)
        }
        return self
    }
}