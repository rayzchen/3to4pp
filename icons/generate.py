from PIL import Image

im = Image.open("puzzle.png")
sizes = [16, 32, 48, 64, 128]
icons = []
for size in sizes:
	resized = im.resize((size, size), Image.Resampling.LANCZOS)
	icons.append(resized)
	resized.save(f"icon{size}x{size}.png")
icons[-1].save("icons.ico", append_images=icons)
