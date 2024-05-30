from PIL import Image
import numpy as np

im = Image.open("puzzle.png").convert('RGBA')
data = np.array(im)
mask = np.all(data[:,:,:3] == [51, 51, 51], axis = -1)
data[mask] = [255, 255, 255, 0]
im = Image.fromarray(data)

sizes = [16, 32, 48, 64, 128]
icons = []
for size in sizes:
	resized = im.resize((size, size), Image.Resampling.LANCZOS)
	icons.append(resized)
	resized.save(f"icon{size}x{size}.png")
icons[-1].save("icons.ico", append_images=icons)
