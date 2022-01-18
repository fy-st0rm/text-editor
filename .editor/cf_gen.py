import os


# Basic globals
AUTO_INDENT = True
SYNTAX_ON = True
MAX_BUFFERS= 10

# Colors
class Colors:
	def __init__(self):
		# Backgrounds
		self.editor_bg  = "282828"
		self.line_bg    = "282828"
		self.bar_bg     = "3c3836"

		# Foregrounds
		self.editor_fg  = "fbf1c7"
		self.line_fg    = "928374"
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


class Syntax:
	def __init__(self):
		self.comment  = "928374"
		self.types    = "fabd2f"
		self.string   = "b8bb26"
		self.keywords = "fb4934"
		self.functions= "98971a"
		self.symbols  = "689d6a"
		self.constants= "d3869b"


colors = Colors()
syntax = Syntax()


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
default_buffer = Buffer("**buffers**", True)
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
	
	src += f"AUTO_INDENT {AUTO_INDENT}\n"
	src += f"SYNTAX_ON {SYNTAX_ON}\n"
	src += f"MAX_BUFF {MAX_BUFFERS}\n"

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

	# Appending syntax color values
	src += f"COMMENT {syntax.comment}\n"
	src += f"TYPES {syntax.types}\n"
	src += f"STRING {syntax.string}\n"
	src += f"KEYWORDS {syntax.keywords}\n"
	src += f"FUNCTIONS {syntax.functions}\n"
	src += f"SYMBOLS {syntax.symbols}\n"
	src += f"CONSTANTS {syntax.constants}\n"

	# Adding the buffers
	for i in buffers:
		src += f"BUFFER_NEW {i.file_name} {i.modifiable}\n"
	
	# Writing it to the file
	file.write(src)
	
	print("Config has been generated..")
	
if __name__ == "__main__":
	generate_config()
