import os


# Basic globals
auto_indent = True

# Colors
class Colors:
	def __init__(self):
		# Backgrounds
		self.editor_bg  = "282828"
		self.line_bg    = "282828"
		self.bar_bg     = "3c3836"

		# Foregrounds
		self.editor_fg  = "fbf1c7"
		self.line_fg    = "fbf1c7"
		self.bar_fg     = "fbf1c7"
		self.cur_line_fg= "fe8019"

		# Modes color
		self.normal_md  = "b8bb26"
		self.insert_md  = "83a598"
		self.visual_md  = "fe8019"
		self.command_md = "b8bb26"

		# Cursor
		self.cursor		= "ebdbb2"
		self.selection  = "928374"


colors = Colors()


class Syntax:
	def __init__(self):
		self.comment = ""
		self.types   = ""
		self.strings = ""
		self.keywords= ""


# Font settings
class Font:
	def __init__(self, family, size):
		self.family = family
		self.size = size

home = os.environ["HOME"]
font_1 = Font(f"{home}/.config/.editor/font/JetBrainsMonoNL-Regular.ttf", 16)
font_2 = Font(f"{home}/.config/.editor/font/JetBrainsMonoNL-Regular.ttf", 16)


# Buffer
class Buffer:
	def __init__(self, file_name, modifiable):
		self.file_name = file_name
		self.modifiable = modifiable

buffers = []

# default buffer
default_buffer = Buffer("\"\"", True)
buffers.append(default_buffer)


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

	# Appending color values
	src += f"EDITOR_BG {colors.editor_bg}\n"
	src += f"LINE_BG {colors.line_bg}\n"
	src += f"BAR_BG {colors.bar_bg}\n"
	src += f"EDITOR_FG {colors.editor_fg}\n"
	src += f"LINE_FG {colors.line_fg}\n"
	src += f"BAR_FG {colors.line_fg}\n"
	src += f"CUR_LINE_FG {colors.cur_line_fg}\n"
	src += f"NORMAL_MD {colors.normal_md}\n"
	src += f"INSERT_MD {colors.insert_md}\n"
	src += f"VISUAL_MD {colors.visual_md}\n"
	src += f"COMMAND_MD {colors.command_md}\n"
	src += f"CURSOR {colors.cursor}\n"
	src += f"SELECTION {colors.selection}\n"

	for i in buffers:
		src += f"BUFFER_NEW {i.file_name} {i.modifiable}\n"
	
	# Writing it to the file
	file.write(src)
	
if __name__ == "__main__":
	generate_config()
