# direct inputs
# source to this solution and code:
# http://stackoverflow.com/questions/14489013/simulate-python-keypresses-for-controlling-a-game
#https://github.com/SerpentAI/SerpentAI/blob/dev/serpent/input_controllers/native_win32_input_controller.py
# https://github.com/SerpentAI/SerpentAI/blob/dev/serpent/input_controllers/native_win32_input_controller.py

import ctypes
import time

SendInput = ctypes.windll.user32.SendInput


Keys = {
'A': 0x1E,


'D': 0x20,
'E': 0x12,
'R': 0x13,
'S': 0x1F,

'W': 0x11

}

mouse_button_down_mapping = {
    "LeftMouseButton": 0x0002,
    "MiddleMouseButton": 0x0020,
    "RightMouseButton": 0x0008
}

mouse_button_up_mapping = {
    "LeftMouseButton": 0x0004,
    "MiddleMouseButton": 0x0040,
    "RightMouseButton": 0x0010
}

W = 0x11
A = 0x1E
S = 0x1F
D = 0x20

NP_2 = 0x50
NP_4 = 0x4B
NP_6 = 0x4D
NP_8 = 0x48

# C struct redefinitions 
PUL = ctypes.POINTER(ctypes.c_ulong)
class KeyBdInput(ctypes.Structure):
    _fields_ = [("wVk", ctypes.c_ushort),
                ("wScan", ctypes.c_ushort),
                ("dwFlags", ctypes.c_ulong),
                ("time", ctypes.c_ulong),
                ("dwExtraInfo", PUL)]

class HardwareInput(ctypes.Structure):
    _fields_ = [("uMsg", ctypes.c_ulong),
                ("wParamL", ctypes.c_short),
                ("wParamH", ctypes.c_ushort)]

class MouseInput(ctypes.Structure):
    _fields_ = [("dx", ctypes.c_long),
                ("dy", ctypes.c_long),
                ("mouseData", ctypes.c_ulong),
                ("dwFlags", ctypes.c_ulong),
                ("time",ctypes.c_ulong),
                ("dwExtraInfo", PUL)]

class Input_I(ctypes.Union):
    _fields_ = [("ki", KeyBdInput),
                 ("mi", MouseInput),
                 ("hi", HardwareInput)]

class Input(ctypes.Structure):
    _fields_ = [("type", ctypes.c_ulong),
                ("ii", Input_I)]

# Actuals Functions

def PressKey(hexKeyCode):
    extra = ctypes.c_ulong(0)
    ii_ = Input_I()
    ii_.ki = KeyBdInput( 0, hexKeyCode, 0x0008, 0, ctypes.pointer(extra) )
    x = Input( ctypes.c_ulong(1), ii_ )
    ctypes.windll.user32.SendInput(1, ctypes.pointer(x), ctypes.sizeof(x))

def ReleaseKey(hexKeyCode):
    extra = ctypes.c_ulong(0)
    ii_ = Input_I()
    ii_.ki = KeyBdInput( 0, hexKeyCode, 0x0008 | 0x0002, 0, ctypes.pointer(extra) )
    x = Input( ctypes.c_ulong(1), ii_ )
    ctypes.windll.user32.SendInput(1, ctypes.pointer(x), ctypes.sizeof(x))

def LeftClick(SleepTime = 0.1):
    extra = ctypes.c_ulong(0)
    ii_ = Input_I()
    ii_.mi = MouseInput(0, 0, 0, 0x0002, 0, ctypes.pointer(extra))
    x = Input(ctypes.c_ulong(0), ii_)
    ctypes.windll.user32.SendInput(1, ctypes.pointer(x), ctypes.sizeof(x))

    time.sleep(SleepTime)
    
    extra = ctypes.c_ulong(0)
    ii_ = Input_I()
    ii_.mi = MouseInput(0, 0, 0, 0x0004, 0, ctypes.pointer(extra))
    x = Input(ctypes.c_ulong(0), ii_)
    ctypes.windll.user32.SendInput(1, ctypes.pointer(x), ctypes.sizeof(x))
    
def ClickMouse(ButtonName="LeftMouseButton", SleepTime = 0.1):
    extra = ctypes.c_ulong(0)
    ii_ = Input_I()
    ii_.mi = MouseInput(0, 0, 0, mouse_button_down_mapping[ButtonName], 0, ctypes.pointer(extra))
    x = Input(ctypes.c_ulong(0), ii_)
    ctypes.windll.user32.SendInput(1, ctypes.pointer(x), ctypes.sizeof(x))

    time.sleep(SleepTime)
    
    extra = ctypes.c_ulong(0)
    ii_ = Input_I()
    ii_.mi = MouseInput(0, 0, 0, mouse_button_up_mapping[ButtonName], 0, ctypes.pointer(extra))
    x = Input(ctypes.c_ulong(0), ii_)
    ctypes.windll.user32.SendInput(1, ctypes.pointer(x), ctypes.sizeof(x))
    
def PressMouse(ButtonName="LeftMouseButton"):
    extra = ctypes.c_ulong(0)
    ii_ = Input_I()
    ii_.mi = MouseInput(0, 0, 0, mouse_button_down_mapping[ButtonName], 0, ctypes.pointer(extra))
    x = Input(ctypes.c_ulong(0), ii_)
    ctypes.windll.user32.SendInput(1, ctypes.pointer(x), ctypes.sizeof(x))

def ReleaseMouse(ButtonName="LeftMouseButton"):    
    extra = ctypes.c_ulong(0)
    ii_ = Input_I()
    ii_.mi = MouseInput(0, 0, 0, mouse_button_up_mapping[ButtonName], 0, ctypes.pointer(extra))
    x = Input(ctypes.c_ulong(0), ii_)
    ctypes.windll.user32.SendInput(1, ctypes.pointer(x), ctypes.sizeof(x))
    
def MoveMouse(x, y):
    extra = ctypes.c_ulong(0)
    ii_ = Input_I()
    ii_.mi = MouseInput(x, y, 0, 0x0001, 0, ctypes.pointer(extra))#(0x0001 | 0x8000), 0, ctypes.pointer(extra))
    x = Input(ctypes.c_ulong(0), ii_)
    ctypes.windll.user32.SendInput(1, ctypes.pointer(x), ctypes.sizeof(x))

def move(self, x=None, y=None, duration=0.25, absolute=True, interpolate=True, **kwargs):
    if ("force" in kwargs and kwargs["force"] is True) or self.game_is_focused:
        if absolute:
            x += self.game.window_geometry["x_offset"]
            y += self.game.window_geometry["y_offset"]

            current_pixel_coordinates = win32api.GetCursorPos()
            start_coordinates = self._to_windows_coordinates(*current_pixel_coordinates)

            end_coordinates = self._to_windows_coordinates(x, y)

            if interpolate:
                coordinates = self._interpolate_mouse_movement(
                    start_windows_coordinates=start_coordinates,
                    end_windows_coordinates=end_coordinates
                )
            else:
                coordinates = [end_coordinates]

            for x, y in coordinates:
                extra = ctypes.c_ulong(0)
                ii_ = Input_I()
                ii_.mi = MouseInput(x, y, 0, (0x0001 | 0x8000), 0, ctypes.pointer(extra))
                x = Input(ctypes.c_ulong(0), ii_)
                ctypes.windll.user32.SendInput(1, ctypes.pointer(x), ctypes.sizeof(x))

                time.sleep(duration / len(coordinates))
        else:
            x = int(x)
            y = int(y)

            coordinates = self._interpolate_mouse_movement(
                start_windows_coordinates=(0, 0),
                end_windows_coordinates=(x, y)
            )

            for x, y in coordinates:
                extra = ctypes.c_ulong(0)
                ii_ = Input_I()
                ii_.mi = MouseInput(x, y, 0, 0x0001, 0, ctypes.pointer(extra))
                x = Input(ctypes.c_ulong(0), ii_)
                ctypes.windll.user32.SendInput(1, ctypes.pointer(x), ctypes.sizeof(x))

                time.sleep(duration / len(coordinates))

if __name__ == '__main__':
    time.sleep(5)
    #PressMouse("LeftMouseButton");
    #time.sleep(5)
    #ReleaseMouse("LeftMouseButton");
    #time.sleep(1)
    for i in range(10):
        MoveMouse(1, 0);
