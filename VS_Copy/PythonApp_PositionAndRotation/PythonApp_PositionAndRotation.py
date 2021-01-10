
from pynput import mouse, keyboard

# c++ modules Start
#import DetectSelfPositionAndRotation
# c++ modules End

#obj = DetectSelfPositionAndRotation.TransformUtilities();

def on_move(x, y):
    print('Pointer moved to {0}'.format((x, y)));
    #obj.Cpp_OnMouseMove(x, y);

def on_click(x, y, button, pressed):
    print('{0} at {1}'.format('Pressed' if pressed else 'Released', (x, y)));

    if not pressed:
        # Stop listener
        return False

def on_scroll(x, y, dx, dy):
    print('Scrolled {0} at {1}'.format('down' if dy < 0 else 'up', (x, y)))


def on_press(key):
    try:
        print('alphanumeric key {0} pressed'.format(key.char));
        #obj.Cpp_OnKeyPresssed(key.char);

    except AttributeError:
        pass;
        print('special key {0} pressed'.format(key));

def on_release(key):
    #print('{0} released'.format(key));
    #obj.Cpp_OnKeyReleased(key.char);
    
    if key == keyboard.Key.esc:
        # Stop listener
        return False


def main():
    MouseListener = mouse.Listener(
        on_move=on_move,
        on_click=on_click,
        on_scroll=on_scroll)
    MouseListener.start()
    
    KeyboardListener = keyboard.Listener(
            on_press=on_press, 
            on_release=on_release)
    KeyboardListener.start()

    # Testing Start
    #with keyboard.Listener(
    #        on_press=on_press,
    #        on_release=on_release) as KeyboardListener:
    #    KeyboardListener.join()
    # Testing End



































#from itertools import islice
#from random import random
#from time import perf_counter

#COUNT = 500000  # Change this value depending on the speed of your computer
#DATA = list(islice(iter(lambda: (random() - 0.5) * 3.0, None), COUNT))

#e = 2.7182818284590452353602874713527

#import DetectSelfPositionAndRotation

#print(DetectSelfPositionAndRotation.fast_tanh2(1));

#import cv2

#def sinh(x):
#    return (1 - (e ** (-2 * x))) / (2 * (e ** -x))

#def cosh(x):
#    return (1 + (e ** (-2 * x))) / (2 * (e ** -x))

#def tanh(x):
#    tanh_x = sinh(x) / cosh(x)
#    return tanh_x

#def test(fn, name):
#    start = perf_counter()
#    result = fn(DATA)
#    duration = perf_counter() - start
#    print('{} took {:.3f} seconds\n\n'.format(name, duration))

#    for d in result:
#        assert -1 <= d <= 1, " incorrect values"

#if __name__ == "__main__":
#    print('Running benchmarks with COUNT = {}'.format(COUNT))

#    test(lambda d: [tanh(x) for x in d], '[tanh(x) for x in d] (Python implementation)')
