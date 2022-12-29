#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import signal
import time

import cv2 as cv
import mediapipe as mp

def main():
    window_name = "Mediapipe Hands"

    hands = mp.solutions.hands.Hands()

    if os.name == "nt":
        cap = cv.VideoCapture(0, cv.CAP_DSHOW)
    else:
        cap = cv.VideoCapture(0)

    fps_clock   = time.perf_counter_ns()
    fps_counter = 0
    fps         = 0

    while True:
        success, img = cap.read()
        if not success:
            print("Failed to read image from camera")
            continue

        results = hands.process(cv.cvtColor(img, cv.COLOR_BGR2RGB))

        if results.multi_hand_landmarks:
            for hand_landmarks in results.multi_hand_landmarks:
                mp.solutions.drawing_utils.draw_landmarks(
                    image         = img,
                    landmark_list = hand_landmarks,
                    connections   = mp.solutions.hands.HAND_CONNECTIONS
                )

        img = cv.putText(
            img       = img,
            text      = f"FPS: {fps}",
            org       = (50, 50),
            fontFace  = cv.FONT_HERSHEY_SIMPLEX,
            fontScale = 1,
            color     = (0x00, 0xff, 0x00),
            thickness = 2,
            lineType  = cv.LINE_AA
        )

        cv.imshow(window_name, img)
        cv.waitKey(1)

        if (cv.getWindowProperty(window_name, cv.WND_PROP_VISIBLE) < 1):
            break

        now = time.perf_counter_ns()
        if (now - fps_clock) >= 1e9:
            fps         = fps_counter
            fps_counter = 0
            fps_clock   = now
        else:
            fps_counter += 1

    cap.release()


if __name__ == '__main__':
    signal.signal(signal.SIGINT, signal.SIG_DFL)
    try:
        main()
    except KeyboardInterrupt:
        print('^C')
        exit(1)
