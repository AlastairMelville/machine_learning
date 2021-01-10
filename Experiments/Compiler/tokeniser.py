import tokenize

with tokenize.open("C:/Users/alime/OneDrive/My Documents/Godot/Node3D.gd") as f:
    tokens = tokenize.generate_tokens(f.readline)
    for token in tokens:
        print(token)