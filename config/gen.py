import os


# Font settings
class Font:
	def __init__(self, family, size):
		self.family = family
		self.size = size

font_1 = Font("font/JetBrainsMonoNL-Regular.ttf", 16)
font_2 = Font("font/JetBrainsMonoNL-Regular.ttf", 16)


# Basic globals
auto_indent = True

def generate_config():
	if not os.path.exists(os.path.join("out")):
		os.mkdir("out")
	
	os.chdir("out")
	
	file = open("config", "w")
	src = ""
	
	# Appending the font information
	src += f"FAMILY1 {font_1.family}\n"
	src += f"FAMILY2 {font_2.family}\n"

	src += f"FONT_SIZE_1 {font_1.size}\n"
	src += f"FONT_SIZE_2 {font_2.size}\n"
	
	src += f"AUTO_INDENT {auto_indent}\n"
	
	# Writing it to the file
	file.write(src)
	
generate_config()