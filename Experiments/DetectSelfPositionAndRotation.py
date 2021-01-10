
from pynput import mouse, keyboard


def on_move(x, y):
    print('Pointer moved to {0}'.format((x, y)));
    return; # TEMP
    Cpp_OnMove(x, y);
    
    MouseMovement = (x - MousePosition[0], y - MousePosition[1]);
    #MoveDistance = MouseMovement[0] + MouseMovement[1];
    
    global CameraRotation;
    CameraRotation += (MouseMovement[0], MouseMovement[1], 0.0);
    
    # Update the mouse position
    global MousePosition;
    MousePosition = (x, y);
    
    global LastMouseMovement;
    LastMouseMovement = MouseMovement;
    

def on_click(x, y, button, pressed):
    print('{0} at {1}'.format(
        'Pressed' if pressed else 'Released',
        (x, y)))
    if not pressed:
        # Stop listener
        return False

def on_scroll(x, y, dx, dy):
    print('Scrolled {0} at {1}'.format('down' if dy < 0 else 'up', (x, y)))


def on_press(key):
    try:
        print('alphanumeric key {0} pressed'.format(
            key.char))
    except AttributeError:
        print('special key {0} pressed'.format(
            key))

def on_release(key):
    print('{0} released'.format(
        key))
    if key == keyboard.Key.esc:
        # Stop listener
        return False


def main():
    MouseListener = mouse.Listener(
        on_move=on_move,
        on_click=on_click,
        on_scroll=on_scroll)
    MouseListener.start()
    
    # Collect events until released
    with keyboard.Listener(
            on_press=on_press, 
            on_release=on_release) as KeyboardListener:
        KeyboardListener.join()


#MousePosition = mouse.position;
#LastMouseMovement = (0.0, 0.0);

#MouseDeltaToCameraRotation = (1.0, 1.0, 1.0);

#CharacterPosition = (0.0, 0.0, 0.0); # X, Y & Z
#CameraRotation = (0.0, 0.0, 0.0); # Pitch, Yaw & Roll

if __name__ == "__main__":
    main();